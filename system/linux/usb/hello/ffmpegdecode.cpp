
#include "ffmpegdecode.h"
extern pthread_rwlock_t rwlock;


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
	}
    if (audioindex == -1){
        printf("Didn't find a audio stream.\n");
//        return ;
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

void StreamPacketCallback(void *param, AVPacket *packet, int flag)
{
    if (NULL == param) {
        printf("invalid param!\n");
        return;
    }
    TAVInfo *arg = (TAVInfo *)param;

//    printf("[%s][%d] stream index:%d\n", __FUNCTION__, __LINE__, packet->stream_index);

    if (flag) {
        if (arg->file){
            fflush(arg->file);
            if (ferror(arg->file)){
                printf("[%s][%d][%s]: Err: Unknown!***\n", __FUNCTION__, __LINE__, arg->save_path);
            }
            fclose(arg->file);
            arg->file = NULL;
            arg->save_count++;
        }
        if (arg->usbfile){
            fflush(arg->usbfile);
            if (ferror(arg->usbfile)){
                printf("[%s][%d][%s]: Err: Unknown!***\n", __FUNCTION__, __LINE__, arg->usb_save_path);
            }
            fclose(arg->usbfile);
            arg->usbfile = NULL;
        }
        printf("[%s][%d] exit\n", __FUNCTION__, __LINE__);
        return;
    }

    if (NULL == arg->file) {
        struct tm *t;
        bool isStore = false;
        time_t tt;
        memset(arg->localtime, 0, 32);
        time(&tt);
        t=localtime(&tt);
        sprintf(arg->localtime, "%d-%d-%d-%d-%d-%d.h264", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
        memset(arg->save_path, 0, SAVE_PATH_LEN);

        pthread_rwlock_rdlock(&rwlock); //lock
        for (int i = 0; i < g_record.storage_manage.count; i++) {
            printf("[%s][%d] count:%d, index:%d state:%d, path:%s, partition:%s\n", __FUNCTION__, __LINE__, g_record.storage_manage.count, i, \
                   g_record.storage_manage.device[i].state, g_record.storage_manage.device[i].path, g_record.storage_manage.device[i].partition_name);
            if (STORAGE_DEVICE_MOUNTED == g_record.storage_manage.device[i].state) {
                if (USB_DEVICE == g_record.storage_manage.device[i].type) {
                    if (g_record.storage_manage.device[i].authorize) {
                        sprintf(arg->save_path, "%s/%s%s", g_record.storage_manage.device[i].path, arg->save, arg->localtime);
                        isStore = true;
                        break;
                    }
                } else if (HDD_DEVICE == g_record.storage_manage.device[i].type) {
                    sprintf(arg->save_path, "%s/%s%s", g_record.storage_manage.device[i].path, arg->save, arg->localtime);
                    isStore = true;
                    break;
                }
            }
        }
        pthread_rwlock_unlock(&rwlock); //unlock

        if (!isStore) {
//            printf("[%s][%d] exit\n", __FUNCTION__, __LINE__);
            return;
        }
//        strcpy(arg->save_path, arg->save);
//        strcat(arg->save_path, arg->localtime);

        arg->file = fopen(arg->save_path, "wb");
        if (arg->file) {
            clock_gettime(CLOCK_MONOTONIC, &arg->startTime);
            printf("saveStreamFile  Open! %s\n", arg->save_path);
        } else {
            printf("saveStreamFile: Err: Open! %s\n", arg->save_path);
        }
    }

    if (arg->file){
        fwrite(packet->data, 1, packet->size, arg->file);

        clock_gettime(CLOCK_MONOTONIC, &arg->nowTime);
        if (arg->nowTime.tv_sec > arg->startTime.tv_sec + arg->duration_sec) {
            fflush(arg->file);
            if (ferror(arg->file)){
                printf("saveStreamFile[%s]: Err: Unknown!***\n", arg->save_path);
            }
            fclose(arg->file);
            arg->file = NULL;
            arg->save_count++;

            arg->thread_exit = 1;
        }
    }
//    printf("[%s][%d] exit\n", __FUNCTION__, __LINE__);
    return ;
}
