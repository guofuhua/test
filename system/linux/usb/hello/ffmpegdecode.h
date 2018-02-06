
#ifndef _FFMPEGDECODE_H_
#define _FFMPEGDECODE_H_
extern "C"
{
#ifdef __cplusplus
 #define __STDC_CONSTANT_MACROS
 #ifdef _STDINT_H
  #undef _STDINT_H
 #endif
 # include <stdint.h>
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"

}
typedef void(*streamcallback)(void *param, AVPacket *packet, int flag);

#include "storage.h"
typedef struct {
    int channel_id;
    char rtsp[RTSP_PATH_LEN];
    char save[SAVE_PATH_LEN];
    streamcallback callback;
    struct timespec startTime;
    struct timespec usbstartTime;
    struct timespec nowTime;
    char localtime[32];
    char save_path[SAVE_PATH_LEN];
    char usb_save_path[SAVE_PATH_LEN];
    FILE *file;
    FILE *usbfile;
    int save_count;
    int thread_exit;
    int duration_sec;
    int is_audio;
    int audio_index;
    int vedio_index;
    int is_usb_stream;
    time_t open_again;
}TAVInfo;

typedef struct {
//    int duration_sec;
    int is_save;
    int is_usb_save;
    int is_disk_save;
    int thread_exit;
    char usb_path[SAVE_PATH_LEN];
    char disk_path[SAVE_PATH_LEN];
}TVideoSaveInfo;

extern void StreamProcess(void *param);
void StreamPacketCallback(void *param, AVPacket *packet, int flag);
#endif
