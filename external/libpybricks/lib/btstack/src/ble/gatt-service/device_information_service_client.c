/*
 * Copyright (C) 2021 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */

#define BTSTACK_FILE__ "device_information_service_client.c"

#include "btstack_config.h"

#include <stdint.h>
#include <string.h>


#include "ble/gatt-service/device_information_service_client.h"

#include "btstack_memory.h"
#include "ble/att_db.h"
#include "ble/core.h"
#include "ble/gatt_client.h"
#include "ble/sm.h"
#include "bluetooth_gatt.h"
#include "btstack_debug.h"
#include "btstack_event.h"
#include "btstack_run_loop.h"
#include "gap.h"

#define DEVICE_INFORMATION_MAX_STRING_LEN 32


typedef enum {
    DEVICE_INFORMATION_SERVICE_CLIENT_STATE_IDLE,
    DEVICE_INFORMATION_SERVICE_CLIENT_STATE_W2_QUERY_SERVICE,
    DEVICE_INFORMATION_SERVICE_CLIENT_STATE_W4_SERVICE_RESULT,
    DEVICE_INFORMATION_CLIENT_STATE_W4_CHARACTERISTIC_RESULT
} device_information_service_client_state_t;

typedef struct {
    hci_con_handle_t con_handle;
    device_information_service_client_state_t  state;
    btstack_packet_handler_t client_handler;

    // service
    uint16_t start_handle;
    uint16_t end_handle;
    uint8_t  num_instances;

    // index of next characteristic to query
    uint8_t characteristic_index;
} device_information_service_client_t;

static device_information_service_client_t device_information_service_client;

static void handle_gatt_client_event(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
static void device_information_service_emit_string_value(device_information_service_client_t * client, uint8_t subevent, uint8_t att_status, const uint8_t * value, uint16_t value_len);
static void device_information_service_emit_system_id(device_information_service_client_t * client, uint8_t subevent, uint8_t att_status, const uint8_t * value, uint16_t value_len);
static void device_information_service_emit_certification_data_list(device_information_service_client_t * client, uint8_t subevent, uint8_t att_status, const uint8_t * value, uint16_t value_len);
static void device_information_service_emit_pnp_id(device_information_service_client_t * client, uint8_t subevent, uint8_t att_status, const uint8_t * value, uint16_t value_len);

// list of uuids and how they are reported as events
static struct device_information_characteristic {
    uint16_t uuid;
    uint8_t  subevent;
    void (*handle_value)(device_information_service_client_t * client, uint8_t subevent, uint8_t att_status, const uint8_t * value, uint16_t value_len);
} device_information_characteristics[] = {
    {ORG_BLUETOOTH_CHARACTERISTIC_MANUFACTURER_NAME_STRING, GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_MANUFACTURER_NAME, device_information_service_emit_string_value},
    {ORG_BLUETOOTH_CHARACTERISTIC_MODEL_NUMBER_STRING, GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_MODEL_NUMBER, device_information_service_emit_string_value},
    {ORG_BLUETOOTH_CHARACTERISTIC_SERIAL_NUMBER_STRING, GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_SERIAL_NUMBER, device_information_service_emit_string_value},
    {ORG_BLUETOOTH_CHARACTERISTIC_HARDWARE_REVISION_STRING, GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_HARDWARE_REVISION, device_information_service_emit_system_id},
    {ORG_BLUETOOTH_CHARACTERISTIC_FIRMWARE_REVISION_STRING, GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_FIRMWARE_REVISION, device_information_service_emit_string_value},
    {ORG_BLUETOOTH_CHARACTERISTIC_SOFTWARE_REVISION_STRING, GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_SOFTWARE_REVISION, device_information_service_emit_string_value},

    {ORG_BLUETOOTH_CHARACTERISTIC_SYSTEM_ID, GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_SYSTEM_ID, device_information_service_emit_string_value},
    {ORG_BLUETOOTH_CHARACTERISTIC_IEEE_11073_20601_REGULATORY_CERTIFICATION_DATA_LIST, GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_IEEE_REGULATORY_CERTIFICATION, device_information_service_emit_certification_data_list},
    {ORG_BLUETOOTH_CHARACTERISTIC_PNP_ID, GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_PNP_ID, device_information_service_emit_pnp_id}
};

static const uint8_t num_information_fields = sizeof(device_information_characteristics)/sizeof(struct device_information_characteristic);

static device_information_service_client_t * device_information_service_client_get_client(void){
    return &device_information_service_client;
}

static device_information_service_client_t * device_information_service_get_client_for_con_handle(hci_con_handle_t con_handle){
    if (device_information_service_client.con_handle == con_handle){
        return &device_information_service_client;
    } 
    return NULL;
}

static void device_information_service_finalize_client(device_information_service_client_t * client){
    client->state = DEVICE_INFORMATION_SERVICE_CLIENT_STATE_IDLE;
    client->con_handle = HCI_CON_HANDLE_INVALID;
}

static void device_information_service_emit_query_done(device_information_service_client_t * client, uint8_t att_status){
    uint8_t event[6];
    int pos = 0;
    event[pos++] = HCI_EVENT_GATTSERVICE_META;
    event[pos++] = sizeof(event) - 2;
    event[pos++] = GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_DONE;
    little_endian_store_16(event, pos, client->con_handle);
    pos += 2;
    event[pos++] = att_status;
    (*client->client_handler)(HCI_EVENT_PACKET, 0, event, pos);
}

static void device_information_service_emit_string_value(device_information_service_client_t * client, uint8_t subevent, uint8_t att_status, const uint8_t * value, uint16_t value_len){
    uint8_t event[6 + DEVICE_INFORMATION_MAX_STRING_LEN + 1];
    int pos = 0;
    
    event[pos++] = HCI_EVENT_GATTSERVICE_META;
    pos++; 
    event[pos++] = subevent;
    little_endian_store_16(event, pos, client->con_handle);
    pos += 2;
    event[pos++] = att_status;

    uint16_t bytes_to_copy = btstack_min(value_len, DEVICE_INFORMATION_MAX_STRING_LEN);
    memcpy((char*)&event[pos], value, bytes_to_copy);
    pos += bytes_to_copy;
    event[pos++] = 0;

    event[2] = pos - 2;
    (*client->client_handler)(HCI_EVENT_PACKET, 0, event, pos);
}

static void device_information_service_emit_system_id(device_information_service_client_t * client, uint8_t subevent, uint8_t att_status, const uint8_t * value, uint16_t value_len){
    if (value_len != 8) return;

    uint8_t event[14];
    uint16_t pos = 0;
    event[pos++] = HCI_EVENT_GATTSERVICE_META;
    event[pos++] = sizeof(event) - 2;
    event[pos++] = subevent;
    little_endian_store_16(event, pos, client->con_handle);
    pos += 2;
    event[pos++] = att_status;
    memcpy(event+pos, value, 8);
    pos += 8;
    
    (*client->client_handler)(HCI_EVENT_PACKET, 0, event, pos);
}

static void device_information_service_emit_certification_data_list(device_information_service_client_t * client, uint8_t subevent, uint8_t att_status, const uint8_t * value, uint16_t value_len){
    if (value_len != 4) return;

    uint8_t event[10];
    int pos = 0;
    event[pos++] = HCI_EVENT_GATTSERVICE_META;
    event[pos++] = sizeof(event) - 2;
    event[pos++] = subevent;
    little_endian_store_16(event, pos, client->con_handle);
    pos += 2;
    event[pos++] = att_status;
    memcpy(event + pos, value, 4);
    pos += 4;
    
    (*client->client_handler)(HCI_EVENT_PACKET, 0, event, pos);
}

static void device_information_service_emit_pnp_id(device_information_service_client_t * client, uint8_t subevent, uint8_t att_status, const uint8_t * value, uint16_t value_len){
    if (value_len != 7) return;

    uint8_t event[13];
    uint16_t pos = 0;
    event[pos++] = HCI_EVENT_GATTSERVICE_META;
    event[pos++] = sizeof(event) - 2;
    event[pos++] = subevent;
    little_endian_store_16(event, pos, client->con_handle);
    pos += 2;
    event[pos++] = att_status;
    memcpy(event + pos, value, 7);
    pos += 7;

    (*client->client_handler)(HCI_EVENT_PACKET, 0, event, pos);
}


static void device_information_service_run_for_client(device_information_service_client_t * client){
    uint8_t att_status;

    switch (client->state){
        case DEVICE_INFORMATION_SERVICE_CLIENT_STATE_W2_QUERY_SERVICE:
            client->state = DEVICE_INFORMATION_SERVICE_CLIENT_STATE_W4_SERVICE_RESULT;
            att_status = gatt_client_discover_primary_services_by_uuid16(handle_gatt_client_event, client->con_handle, ORG_BLUETOOTH_SERVICE_DEVICE_INFORMATION);
            // TODO handle status
            UNUSED(att_status);
            break;
        
        case DEVICE_INFORMATION_CLIENT_STATE_W4_CHARACTERISTIC_RESULT:
            if (client->characteristic_index < num_information_fields){
                att_status = gatt_client_read_value_of_characteristics_by_uuid16(
                    handle_gatt_client_event, 
                    client->con_handle, client->start_handle, client->end_handle, 
                    device_information_characteristics[client->characteristic_index].uuid);
                // TODO handle status
                UNUSED(att_status);
            }
            break;

        default:
            break;
    }
}

static void handle_gatt_client_event(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(packet_type); 
    UNUSED(channel);     
    UNUSED(size);        

    uint8_t att_status;
    device_information_service_client_t * client;
    gatt_client_service_t service;

    switch(hci_event_packet_get_type(packet)){
        
        case GATT_EVENT_SERVICE_QUERY_RESULT:
            client = device_information_service_get_client_for_con_handle(gatt_event_service_query_result_get_handle(packet));
            btstack_assert(client != NULL);

            if (client->state != DEVICE_INFORMATION_SERVICE_CLIENT_STATE_W4_SERVICE_RESULT) {
                device_information_service_emit_query_done(client, GATT_CLIENT_IN_WRONG_STATE);  
                device_information_service_finalize_client(client);      
                break;
            }

            gatt_event_service_query_result_get_service(packet, &service);
            client->start_handle = service.start_group_handle;
            client->end_handle = service.end_group_handle;
            client->num_instances++;
            client->characteristic_index = 0;
            break;

        case GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT:
            client = device_information_service_get_client_for_con_handle(gatt_event_characteristic_value_query_result_get_handle(packet));
            btstack_assert(client != NULL);

            (device_information_characteristics[client->characteristic_index].handle_value(
                client, device_information_characteristics[client->characteristic_index].subevent, 
                ATT_ERROR_SUCCESS,
                gatt_event_characteristic_value_query_result_get_value(packet), 
                gatt_event_characteristic_value_query_result_get_value_length(packet)));
            break;

        case GATT_EVENT_QUERY_COMPLETE:
            client = device_information_service_get_client_for_con_handle(gatt_event_query_complete_get_handle(packet));
            btstack_assert(client != NULL);
            
            att_status = gatt_event_query_complete_get_att_status(packet);
            switch (client->state){
                case DEVICE_INFORMATION_SERVICE_CLIENT_STATE_W4_SERVICE_RESULT:
                    if (att_status != ATT_ERROR_SUCCESS){
                        device_information_service_emit_query_done(client, att_status);  
                        device_information_service_finalize_client(client);
                        break;  
                    }

                    if (client->num_instances != 1){
                        device_information_service_emit_query_done(client, ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE); 
                        device_information_service_finalize_client(client);
                        break;   
                    }

                    client->state = DEVICE_INFORMATION_CLIENT_STATE_W4_CHARACTERISTIC_RESULT;
                    device_information_service_run_for_client(client);
                    break;
                
                case DEVICE_INFORMATION_CLIENT_STATE_W4_CHARACTERISTIC_RESULT:
                    if (att_status != ATT_ERROR_SUCCESS){
                        (device_information_characteristics[client->characteristic_index].handle_value(
                            client, device_information_characteristics[client->characteristic_index].subevent, 
                            att_status,
                            gatt_event_characteristic_value_query_result_get_value(packet), 
                            gatt_event_characteristic_value_query_result_get_value_length(packet)));
                    }
                    
                    // check if there is another characteristic to query
                    if (client->characteristic_index < num_information_fields){
                        client->characteristic_index++;
                        device_information_service_run_for_client(client);
                        break;
                    } 
                    // we are done with quering all characteristics
                    device_information_service_emit_query_done(client, ERROR_CODE_SUCCESS);
                    device_information_service_finalize_client(client);  
                    break;

                default:
                    break;
            }
            break;
        default:
            break;
    }
 
}

uint8_t device_information_service_client_query(hci_con_handle_t con_handle, btstack_packet_handler_t packet_handler){
    btstack_assert(packet_handler != NULL);
    device_information_service_client_t * client = device_information_service_client_get_client();

    if (client->state != DEVICE_INFORMATION_SERVICE_CLIENT_STATE_IDLE){
        return GATT_CLIENT_IN_WRONG_STATE;
    }

    client->con_handle = con_handle;
    client->client_handler = packet_handler; 
    client->state = DEVICE_INFORMATION_SERVICE_CLIENT_STATE_W2_QUERY_SERVICE;
    device_information_service_run_for_client(client);
    return ERROR_CODE_SUCCESS;
}


void device_information_service_client_init(void){}

void device_information_service_client_deinit(void){}

