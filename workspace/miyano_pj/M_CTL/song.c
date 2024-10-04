#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include "spike/hub/speaker.h"  /* debug */

#include <miyano_pj.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

#include "../D_DEVICE/comm.h"

#include "song.h"

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


#define SONG_BUF 64
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
uint16_t u16_song_nobashi[SONG_BUF] =
{
    2,0,1,1,2,0,1,1,
    2,0,1,1,2,0,1,1,
    2,0,1,1,2,0,2,0,
    2,0,2,0,2,0,0,0,
    
    0,2,0,1,2,0,1,1,
    2,0,0,1,2,0,1,1,
    2,0,1,1,2,0,2,0,
    2,0,2,0,2,0,0,0,
};

// #define SONG_BUF 72
// uint16_t u16_song_buf[SONG_BUF] =
// {
//        0,   0,   0,   0,   0,   0,SO_L,   0,

//     DO_H,   0,   0,DO_H,DO_H,   0,MI_H,   0,
//     RE_H,   0,   0,DO_H,RE_H,   0,MI_H,   0,
//     DO_H,   0,DO_H,   0,MI_H,   0,SO_H,   0,
//     RA_H,   0,   0,   0,   0,   0,RA_H,   0,

//     SO_H,   0,   0,MI_H,MI_H,   0,DO_H,   0,
//     RE_H,   0,   0,DO_H,RE_H,   0,MI_H,   0,
//     DO_H,   0,   0,RA_L,RA_L,   0,SO_L,   0,
//     DO_H,   0,   0,   0,   0,   0,   0,   0,
// };
// uint16_t u16_song_nobashi[SONG_BUF] =
// {
//     0,0,0,0,0,0,2,0,

//     3,0,0,1,2,0,2,0,
//     3,0,0,1,2,0,2,0,
//     2,0,2,0,2,0,2,0,
//     6,0,0,0,0,0,2,0,
    
//     3,0,0,1,2,0,2,0,
//     3,0,0,1,2,0,2,0,
//     3,0,0,1,2,0,2,0,
//     6,0,0,0,0,0,0,0,
// };

void ini_song( void ){
    u16_song_index = 0;
    song_hz_old    = 0;
}

void cyc_song( void ){
    uint16_t song_hz;
    uint16_t song_nobashi;
    
    // if( 0 == g_u16_comm_rx_flg ){
    if( 0 == comm_reset_flg ){
        u16_song_index = 0;
        return;
        /* 以降の処理は実施しない */
    }
    // g_u16_comm_rx_flg = 0;

    song_hz      = u16_song_buf[u16_song_index];
    song_nobashi = u16_song_nobashi[u16_song_index];
    if( SONG_BUF-1 > u16_song_index ){
        u16_song_index += 1;
    }
    else{
        u16_song_index = 0;
    }
    if(0 == song_nobashi){
        return;
    }
    song_hz_old = song_hz;
    hub_speaker_set_volume(30);
    // hub_speaker_set_volume(0);
    hub_speaker_play_tone( song_hz, 30*song_nobashi );
}