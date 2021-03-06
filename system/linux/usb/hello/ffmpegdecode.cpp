
#include "ffmpegdecode.h"
extern pthread_rwlock_t rwlock;
extern int mkdirs(char* sPathName);
extern int mk_all_dir(char *dir);


/************************************************************************
**函数：open_rtsp
**功能：从RTSP获取音视频流数据
**参数：
[in]  rtsp - RTSP地址
**返回：无
************************************************************************/
void StreamProcess(void *param)
{
    if (NULL == param) {
        printf("invalid param!\n");
        return;
    }
//    printf("[%s][%d] saveStreamFile  Open! \n", __FUNCTION__, __LINE__);
    TAVInfo *arg = (TAVInfo *)param;
	AVFormatContext	*pFormatCtx;
    unsigned int i;
    int videoindex, audioindex;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;
	AVFrame	*pFrame;
    AVPacket *packet;
	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();

    if (avformat_open_input(&pFormatCtx, arg->rtsp, NULL, NULL) != 0){
		printf("Couldn't open input stream.\n");
		return ;
	}
    printf(" open input stream:%s.\n", arg->rtsp);
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0){
		printf("Couldn't find stream information.\n");
		return ;
	}
	videoindex = -1;
    audioindex = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            videoindex = i;
            if (-1 != audioindex) {
                break;
            }
        } else if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioindex = i;
            if (-1 != videoindex) {
                break;
            }
        }
    }
	if (videoindex == -1){
		printf("Didn't find a video stream.\n");
		return ;
    } else {
        arg->vedio_index = videoindex;
    }
    if (audioindex == -1){
        printf("Didn't find a audio stream.\n");
//        return ;
    } else {
        arg->is_audio = 1;
        arg->audio_index = audioindex;
    }

	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL){
		printf("Codec not found.\n");
		return ;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0){
		printf("Could not open codec.\n");
		return ;
	}
	pFrame = av_frame_alloc();
	packet = (AVPacket *)av_malloc(sizeof(AVPacket));
    while (av_read_frame(pFormatCtx, packet) >= 0 && !arg->thread_exit)
	{
        if ((packet->stream_index == videoindex) || (packet->stream_index == audioindex))
        {
//            framedeal(pobj, packet);
            arg->callback(param, packet, 0);
		}	
//        printf("got_picture:%d, stream index:%d, video index:%d, audio index:%d\n", got_picture, packet->stream_index, videoindex, audioindex);
		av_free_packet(packet);
        av_frame_unref(pFrame);
    }
    arg->callback(param, packet, 1);

	av_frame_unref(pFrame);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

    printf("[%s][%d] exit\n", __FUNCTION__, __LINE__);
	return;
}

extern MONITOR_RECORD g_record;
extern TVideoSaveInfo g_usb_hotplug;

void create_video_file(int store_path, TAVInfo *arg)
{
    if (NULL == arg->file) {
        struct tm *t;
        time_t tt;
        time(&tt);
        if ((0 == store_path) || (tt < arg->open_again)) {
//            pthread_rwlock_unlock(&rwlock); //unlock
            return;
        }
        memset(arg->localtime, 0, 32);
        t=localtime(&tt);
        sprintf(arg->localtime, "%d-%d-%d.h264", t->tm_hour, t->tm_min, t->tm_sec);
        memset(arg->save_path, 0, SAVE_PATH_LEN);

        if (2 == store_path) {
            sprintf(arg->save_path, "%s/zzcx/%d-%d-%d/%s/%s", g_usb_hotplug.usb_path, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, arg->save, arg->localtime);
        } else {
            sprintf(arg->save_path, "%s/zzcx/%d-%d-%d/%s/%s", g_usb_hotplug.disk_path, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, arg->save, arg->localtime);
        }
        mk_all_dir(arg->save_path);

        arg->file = fopen(arg->save_path, "wb");
        if (arg->file) {
            clock_gettime(CLOCK_MONOTONIC, &arg->startTime);
            printf("[%s][%d] saveStreamFile  Open! %s\n", __FUNCTION__, __LINE__, arg->save_path);
        } else {
            arg->open_again = tt + 10;
//            printf("[%s][%d] saveStreamFile: Err: Open! %s\n", __FUNCTION__, __LINE__, arg->save_path);
            printf("[%s][%d] saveStreamFile: Err: Open! %s, err(%d:%s)\n", __FUNCTION__, __LINE__, arg->save_path, errno, strerror(errno));
        }
    }
    return;
}

void create_audio_file(int store_path, TAVInfo *arg)
{
    if (arg->is_audio && (NULL == arg->usbfile)) {
        struct tm *t;
        time_t tt;
        time(&tt);
        if ((0 == store_path) || (tt < arg->open_again)) {
//            pthread_rwlock_unlock(&rwlock); //unlock
            return;
        }
        memset(arg->localtime, 0, 32);
        t=localtime(&tt);
        sprintf(arg->localtime, "%d-%d-%d.aac", t->tm_hour, t->tm_min, t->tm_sec);
        memset(arg->usb_save_path, 0, SAVE_PATH_LEN);

        if (2 == store_path) {
            sprintf(arg->usb_save_path, "%s/zzcx/%d-%d-%d/%s/%s", g_usb_hotplug.usb_path, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, arg->save, arg->localtime);
        } else {
            sprintf(arg->usb_save_path, "%s/zzcx/%d-%d-%d/%s/%s", g_usb_hotplug.disk_path, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, arg->save, arg->localtime);
        }
        mk_all_dir(arg->usb_save_path);

        arg->usbfile = fopen(arg->usb_save_path, "wb");
        if (arg->usbfile) {
            clock_gettime(CLOCK_MONOTONIC, &arg->usbstartTime);
            printf("[%s][%d] saveStreamFile  Open! %s\n", __FUNCTION__, __LINE__, arg->usb_save_path);
        } else {
            arg->open_again = tt + 2;
            printf("[%s][%d] saveStreamFile: Err: Open! %s, err(%d:%s)\n", __FUNCTION__, __LINE__, arg->usb_save_path, errno, strerror(errno));
        }
    }
    return;
}

void StreamPacketCallback(void *param, AVPacket *packet, int flag)
{
    if (NULL == param) {
        printf("invalid param!\n");
        return;
    }
    TAVInfo *arg = (TAVInfo *)param;

//    printf("[%s][%d] stream index:%d\n", __FUNCTION__, __LINE__, packet->stream_index);

    pthread_rwlock_rdlock(&rwlock); //lock
    int store_path = 0;
    if (g_usb_hotplug.is_disk_save && !arg->is_usb_stream) {
        store_path = 1;
        if (NULL == arg->file) {
            create_video_file(store_path, arg);
        }
        if (arg->is_audio && (NULL == arg->usbfile)) {
            create_audio_file(store_path, arg);
        }
    } else if (g_usb_hotplug.is_usb_save && arg->is_usb_stream) {
        store_path = 2;
//        printf("[%s][%d] is_usb_save:%d, is_usb_stream:%d\n", __FUNCTION__, __LINE__, g_usb_hotplug.is_usb_save, arg->is_usb_stream);
        if (NULL == arg->file) {
            create_video_file(store_path, arg);
        }
        if (arg->is_audio && (NULL == arg->usbfile)) {
            create_audio_file(store_path, arg);
        }
    }
    pthread_rwlock_unlock(&rwlock); //unlock

    if (flag || (0 == store_path)) {
        if (arg->file) {
            fflush(arg->file);
            if (ferror(arg->file)){
                printf("[%s][%d][%s]: Err: Unknown!***\n", __FUNCTION__, __LINE__, arg->save_path);
            }
            fclose(arg->file);
            printf("[%s][%d]: close file (%s)\n", __FUNCTION__, __LINE__, arg->save_path);
            arg->file = NULL;
            arg->save_count++;
        }
        if (arg->usbfile) {
            fflush(arg->usbfile);
            if (ferror(arg->usbfile)){
                printf("[%s][%d][%s]: Err: Unknown!***\n", __FUNCTION__, __LINE__, arg->usb_save_path);
            }
            fclose(arg->usbfile);
            printf("[%s][%d]: close file (%s)\n", __FUNCTION__, __LINE__, arg->usb_save_path);
            arg->usbfile = NULL;
        }
        arg->thread_exit = 1;
        printf("[%s][%d] exit\n", __FUNCTION__, __LINE__);
        return;
    }

    if (arg->vedio_index == packet->stream_index) {
        if (arg->file){
            fwrite(packet->data, 1, packet->size, arg->file);

            clock_gettime(CLOCK_MONOTONIC, &arg->nowTime);
            if (arg->nowTime.tv_sec > arg->startTime.tv_sec + arg->duration_sec) {
                fflush(arg->file);
                if (ferror(arg->file)){
                    printf("[%s][%d] saveStreamFile[%s]: Err: Unknown!***\n", __FUNCTION__, __LINE__, arg->save_path);
                }
                fclose(arg->file);
                arg->file = NULL;
                arg->save_count++;
                printf("[%s][%d]: close file (%s)\n", __FUNCTION__, __LINE__, arg->save_path);
            }
        }
    } else if (arg->audio_index == packet->stream_index) {
        if (arg->usbfile){
            fwrite(packet->data, 1, packet->size, arg->usbfile);

            clock_gettime(CLOCK_MONOTONIC, &arg->nowTime);
            if (arg->nowTime.tv_sec > arg->usbstartTime.tv_sec + arg->duration_sec) {
                fflush(arg->usbfile);
                if (ferror(arg->usbfile)){
                    printf("[%s][%d] saveStreamFile[%s]: Err: Unknown!***\n", __FUNCTION__, __LINE__, arg->usb_save_path);
                }
                fclose(arg->usbfile);
                arg->usbfile = NULL;
                printf("[%s][%d]: close file (%s)\n", __FUNCTION__, __LINE__, arg->usb_save_path);
            }
        }
    }

//    printf("[%s][%d] exit\n", __FUNCTION__, __LINE__);
    return ;
}
