#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include "spike/hub/speaker.h"  /* debug */

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "../D_DEVICE/comm.h"

#include "song.h"

#define SONG_BUF 64

#define DO_L 261
#define RE_L 293
#define MI_L 329
#define FA_L 349
#define SO_L 392
#define RA_L 440
#define SI_L 493
#define DO_H 523
#define RE_H 587
#define MI_H 659
#define FA_H 698
#define SO_H 784
#define RA_H 880
#define SI_H 987

uint16_t song_hz_old;
uint16_t u16_song_index;
uint16_t u16_song_buf[SONG_BUF] =
{
    MI_H,MI_H,SI_L,DO_H,RE_H,RE_H,DO_H,SI_L,
    RA_L,RA_L,RA_L,DO_H,MI_H,MI_H,RE_H,DO_H,
    SI_L,SI_L,SI_L,DO_H,RE_H,RE_H,MI_H,MI_H,
    DO_H,DO_H,RA_L,RA_L,RA_L,RA_L,   0,   0,
       0,RE_H,RE_H,FA_H,RA_H,RA_H,SO_H,FA_H,
    MI_H,MI_H,   0,DO_H,MI_H,MI_H,RE_H,DO_H,
    SI_L,SI_L,SI_L,DO_H,RE_H,RE_H,MI_H,MI_H,
    DO_H,DO_H,RA_L,RA_L,RA_L,RA_L,RA_L,RA_L,
};

void ini_song( void ){
    u16_song_index = 0;
    song_hz_old    = 0;
}

void cyc_song( void ){
    uint16_t song_hz;
    
    if( 0 == g_u16_comm_rx_flg ){
        u16_song_index = 0;
        return;
        /* 以降の処理は実施しない */
    }
    g_u16_comm_rx_flg = 0;

    song_hz = u16_song_buf[u16_song_index];
    if( SONG_BUF-1 > u16_song_index ){
        u16_song_index += 1;
    }
    else{
        u16_song_index = 0;
    }
    if( song_hz_old != song_hz ){
        // hub_speaker_set_volume(20);
        // hub_speaker_play_tone(song_hz, 200);
    }
    song_hz_old = song_hz;
    // hub_speaker_set_volume(20);
    hub_speaker_set_volume(0);
    hub_speaker_play_tone(song_hz, 200);
}