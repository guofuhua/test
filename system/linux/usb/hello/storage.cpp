#include "storage.h"
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

pthread_rwlock_t rwlock;
MONITOR_RECORD g_record;

storage::storage()
{
}

void storage::init()
{
//    struct tm *t;
//    time_t tt;
//    char nowtime[32];

//    memset(nowtime, 0, 32);
//    time(&tt);
//    t=localtime(&tt);
//    sprintf(nowtime, "%d-%d-%d-%d-%d-%d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

    pthread_rwlock_init(&rwlock, NULL);

    pthread_rwlock_wrlock(&rwlock);
    g_record.channel_count = 16;
    for (int i = 0; i < g_record.channel_count; i++) {
        g_record.channel[i].channel_id = i;
        g_record.channel[i].save_mode = CHANNEL_SAVE_MAIN;
        memset(g_record.channel[i].mainstream, 0, RTSP_PATH_LEN);
        memset(g_record.channel[i].substream, 0, RTSP_PATH_LEN);
        if (i < 8) {
            g_record.channel[i].board = BOARD_AV1;
            sprintf(g_record.channel[i].mainstream, "rtsp://192.168.60.21/chn%d", 2 * i);
            sprintf(g_record.channel[i].substream, "rtsp://192.168.60.21/chn%d", 2 * i + 1);
        } else {
            g_record.channel[i].board = BOARD_AV2;
            sprintf(g_record.channel[i].mainstream, "rtsp://192.168.60.22/chn%d", 2 * (i - 8));
            sprintf(g_record.channel[i].substream, "rtsp://192.168.60.22/chn%d", 2 * (i - 8) + 1);
        }
        memset(g_record.channel[i].save_main_path, 0, SAVE_PATH_LEN);
//        sprintf(g_record.channel[i].save_main_path, "main_chn%d-%s", g_record.channel[i].channel_id, nowtime);
        sprintf(g_record.channel[i].save_main_path, "main_chn%d-", g_record.channel[i].channel_id);
        memset(g_record.channel[i].save_sub_path, 0, SAVE_PATH_LEN);
//        sprintf(g_record.channel[i].save_sub_path, "sub_chn%d-%s", g_record.channel[i].channel_id, nowtime);
        sprintf(g_record.channel[i].save_sub_path, "sub_chn%d-", g_record.channel[i].channel_id);
    }
    pthread_rwlock_unlock(&rwlock);
}

unsigned long getSystemUsedTime()
{
    struct timespec startTime1 = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &startTime1);
    return (unsigned long) (1000*( startTime1.tv_sec) +   startTime1.tv_nsec/1000000);
}

void storage::destroy()
{
    pthread_rwlock_destroy(&rwlock);
}
