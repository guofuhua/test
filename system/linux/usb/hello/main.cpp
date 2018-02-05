#if 1
//#include <QCoreApplication>
//#include <QDebug>
#include "storage.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include "ffmpegdecode.h"
#include "threadpool/threadpool.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "storage.h"
#include "defbase.h"

extern pthread_rwlock_t rwlock;
extern MONITOR_RECORD g_record;
extern TAVInfo avInfoU[MAX_CHANNEL];

int tasks = 0, done = 0;
pthread_mutex_t lock;
TVideoSaveInfo g_usb_hotplug;
TAVInfo avInfoU[MAX_CHANNEL];
threadpool_t *pool;

extern int test_main();
extern int process_dev(STORAGE_DEVICE_MANAGE *manage);
extern void get_usb_path(char *buf, int flag);
extern void check_mount_event();

#define UEVENT_BUFFER_SIZE 2048

static int init_hotplug_sock()
{
    const int buffersize = 1024;
    int ret;

    struct sockaddr_nl snl;
    bzero(&snl, sizeof(struct sockaddr_nl));
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = getpid();
    snl.nl_groups = 1;

    int s = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (s == -1)
    {
        perror("socket");
        return -1;
    }
    setsockopt(s, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize));

    ret = bind(s, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl));
    if (ret < 0)
    {
        perror("bind");
        close(s);
        return -1;
    }

    return s;
}

void check_video_path(TVideoSaveInfo *arg)
{
    pthread_rwlock_wrlock(&rwlock); //lock
    arg->is_disk_save = 0;
    arg->is_usb_save = 0;
    for (int i = 0; i < g_record.storage_manage.count; i++) {
        printf("[%s][%d] count:%d, index:%d state:%d, path:%s, partition:%s, type:%d, auth:%d\n", __FUNCTION__, __LINE__, g_record.storage_manage.count, i, \
               g_record.storage_manage.device[i].state, g_record.storage_manage.device[i].path, g_record.storage_manage.device[i].partition_name, g_record.storage_manage.device[i].type, \
               g_record.storage_manage.device[i].authorize);
        if (STORAGE_DEVICE_MOUNTED == g_record.storage_manage.device[i].state) {
            if (USB_DEVICE == g_record.storage_manage.device[i].type) {
                if (g_record.storage_manage.device[i].authorize) {
                    strcpy(arg->usb_path, g_record.storage_manage.device[i].path);
                    arg->is_usb_save = 1;
//                    break;
                }
            } else if (HDD_DEVICE == g_record.storage_manage.device[i].type) {
                strcpy(arg->disk_path, g_record.storage_manage.device[i].path);
                arg->is_disk_save = 1;
//                break;
            }
        }
    }
    if (arg->is_usb_save) {
        printf("[%s][%d] threadpool_add avInfoU\n", __FUNCTION__, __LINE__);
        threadpool_add(pool, StreamProcess, (void *)&avInfoU[0], 0);
    }
    pthread_rwlock_unlock(&rwlock); //unlock
    return ;
}

void check_hotplug(char *buf, int len)
{
//    printf("received %d bytes\n",len);
    char *token = buf;
    char *key;
    THOT_PLUG_MSG msg;
    char          seps[] = "\n";
    memset(&msg, 0, sizeof(THOT_PLUG_MSG));
    for(int i=0;i<len;i++)
        if(*(buf+i)=='\0')
            buf[i]='\n';
    token = strtok(buf, seps);
    while (NULL != token) {
//        printf("token: %s\n", token);
        switch (*token) {
        case 'A':
            if (0 == strncmp("ACTION=", token, 7)) {
                if ('a' == *(token + 7)) {
                    msg.action = ACTION_ADD; //ACTION=add
                } else {
                    msg.action = ACTION_REMOVE; //ACTION=remove
                }
            }
            break;
        case 'D':
            if (SUBSYSTEM_BLOCK == msg.block) {
                if (0 == strncmp("DEVNAME=", token, 8)) {
                    strcpy(msg.name, (token + 8));
#if (1)
                    printf("token: %s\n", token);
#endif
                } else if (0 == strncmp("DEVTYPE=", token, 8)) {
                    key = token + 8;
                    if (0 == strncmp("disk", key, 4)) {
                        msg.disk = DEVTYPE_DISK;
                    } else if (0 == strncmp("partition", key, 9)) {
                        msg.disk = DEVTYPE_PARTITION;
                    }
                }
            } else if (0 == strncmp("DEVPATH=", token, 8)) {
                if (strstr(token, "ahci")) {
                    msg.type = HDD_DEVICE;
                } else if (strstr(token, "usb")) {
                    msg.type = USB_DEVICE;
                }
            }
            break;
        case 'S':
            if (0 == strcmp("SUBSYSTEM=block", token))
            {
                msg.block = SUBSYSTEM_BLOCK;
            }
            break;
        case 'N':
            if (0 == strncmp("NPARTS=", token, 7))
            {
                msg.npart = atoi(token + 7);
            }
            break;
        case 'P':
            if (0 == strncmp("PARTN=", token, 6))
            {
                msg.partn = atoi(token + 6);
            }
            break;
        default:
            break;
        }

        token = strtok(NULL, seps);
    }

    if (SUBSYSTEM_BLOCK == msg.block) {
        if (ACTION_ADD == msg.action) {
            if (DEVTYPE_DISK == msg.disk) {
                pthread_rwlock_wrlock(&rwlock);
                strcpy(g_record.storage_manage.device[g_record.storage_manage.count].name, msg.name);
                g_record.storage_manage.device[g_record.storage_manage.count].partition_count = msg.npart;
                g_record.storage_manage.device[g_record.storage_manage.count].totalspace = 0; //add for future
                g_record.storage_manage.device[g_record.storage_manage.count].type = msg.type;
                if (USB_DEVICE == msg.type) {
                    get_usb_path(g_record.storage_manage.device[g_record.storage_manage.count].path, 1);
//                    strncpy(g_record.storage_manage.device[g_record.storage_manage.count].path, "/hdd/sdb1", SAVE_PATH_LEN);
                } else if (HDD_DEVICE == msg.type) {
                    strncpy(g_record.storage_manage.device[g_record.storage_manage.count].path, "/hdd/sda1", SAVE_PATH_LEN);
                }
                g_record.storage_manage.count++;

                if (0 == msg.npart) {
                    sprintf(g_record.storage_manage.device[g_record.storage_manage.count - 1].partition_name, "/dev/%s", msg.name);
                    g_record.storage_manage.device[g_record.storage_manage.count - 1].state = STORAGE_DEVICE_INSERT;
                }
                pthread_rwlock_unlock(&rwlock);
                check_mount_event();
                check_video_path(&g_usb_hotplug);
            } else if (DEVTYPE_PARTITION == msg.disk) {
                if (1 == msg.partn) {
                    pthread_rwlock_wrlock(&rwlock);
                    sprintf(g_record.storage_manage.device[g_record.storage_manage.count - 1].partition_name, "/dev/%s", msg.name);
                    g_record.storage_manage.device[g_record.storage_manage.count - 1].partition_size[msg.partn] = 0; //add for future;
                    g_record.storage_manage.device[g_record.storage_manage.count - 1].state = STORAGE_DEVICE_INSERT;
                    pthread_rwlock_unlock(&rwlock);

                    check_mount_event();
                    check_video_path(&g_usb_hotplug);
                } else {
                    // multi partion, only mount the first partition;
                    printf("[%s][%d] Multi partition, current partition(%s)(%d)\n", __FUNCTION__, __LINE__, msg.name, msg.partn);
                }
            }
        } else if (ACTION_REMOVE == msg.action) {
            pthread_rwlock_wrlock(&rwlock);
            for (int i = 0; i < g_record.storage_manage.count; i++) {
                if (0 == strncmp(g_record.storage_manage.device[i].name, msg.name, 3)) {
                    g_record.storage_manage.device[i].state = STORAGE_DEVICE_REMOVED;
                    for (int j = 0; j < MAX_CHANNEL; j++) {
                        avInfoU[j].thread_exit = 1;
                    }
                    if (0 == strcmp(g_usb_hotplug.usb_path, g_record.storage_manage.device[i].path)) {
                        g_usb_hotplug.is_usb_save = 0;
                    }
                    break;
                }
            }
            pthread_rwlock_unlock(&rwlock);
            check_mount_event();
            check_video_path(&g_usb_hotplug);
        }
    }
//    printf("received %d bytes\n%s\n",len,buf);
}

void hotplug(void *param)
{
    if (NULL == param) {
        printf("invalid param!\n");
        return;
    }
    TVideoSaveInfo *arg = (TVideoSaveInfo *)param;
    fd_set fds;
    int ret, rcvlen;
    struct timeval tv;
    storage::init();
    int hotplug_sock = init_hotplug_sock();
    pthread_rwlock_wrlock(&rwlock);
    test_main();
    pthread_rwlock_unlock(&rwlock);
    check_video_path(arg);

    while (!arg->thread_exit) {
        /* Netlink message buffer */
        char buf[UEVENT_BUFFER_SIZE * 2] = { 0 };
        FD_ZERO(&fds);
        FD_SET(hotplug_sock, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 100 * 1000;
        ret = select(hotplug_sock + 1, &fds, NULL, NULL, &tv);
        if(ret < 0)
            continue;
        if(!(ret > 0 && FD_ISSET(hotplug_sock, &fds)))
            continue;
        /* receive data */
        rcvlen = recv(hotplug_sock, &buf, sizeof(buf), 0);
        if (rcvlen > 0) {
            printf("%s\n", buf);
            /*You can do something here to make the program more perfect!!!*/
            check_hotplug(buf, rcvlen);
        }
    }

    close(hotplug_sock);
    printf("[%s][%d] exit\n", __FUNCTION__, __LINE__);
    return ;
}







void dummy_task(void *arg) {
    UNUSED(arg);
    usleep(10000);
    pthread_mutex_lock(&lock);
    /* 记录成功完成的任务数 */
    done++;
    pthread_mutex_unlock(&lock);
}

int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    //    QCoreApplication a(argc, argv);

    int thread_count = 32;
    int queue_size = 256;
//    threadpool_t *pool;
    TAVInfo avInfo[MAX_CHANNEL];
    memset(avInfo, 0, sizeof(TAVInfo) * MAX_CHANNEL);
    memset(avInfoU, 0, sizeof(TAVInfo) * MAX_CHANNEL);
    memset(&g_usb_hotplug, 0, sizeof(TVideoSaveInfo));
    storage::init();

    pthread_rwlock_wrlock(&rwlock);
    for (int i = 0; i < g_record.channel_count; i++) {
        avInfo[i].channel_id = g_record.channel[i].channel_id;
        if (CHANNEL_SAVE_MAIN == g_record.channel[i].save_mode) {
            strncpy(avInfo[i].rtsp, g_record.channel[i].mainstream, RTSP_PATH_LEN);
            strncpy(avInfo[i].save, g_record.channel[i].save_main_path, SAVE_PATH_LEN);
        } else {
            strncpy(avInfo[i].rtsp, g_record.channel[i].substream, RTSP_PATH_LEN);
            strncpy(avInfo[i].save, g_record.channel[i].save_sub_path, SAVE_PATH_LEN);
        }
        avInfo[i].callback = StreamPacketCallback;
        avInfo[i].file = NULL;
        avInfo[i].usbfile = NULL;
        avInfo[i].duration_sec = 900;
    }
    for (int i = 0; i < g_record.channel_count; i++) {
        int j = i;
        avInfoU[j].channel_id = g_record.channel[i].channel_id;
        if (CHANNEL_SAVE_SUB == g_record.channel[i].save_mode) {
            strncpy(avInfoU[j].rtsp, g_record.channel[i].mainstream, RTSP_PATH_LEN);
            strncpy(avInfoU[j].save, g_record.channel[i].save_main_path, SAVE_PATH_LEN);
        } else {
            strncpy(avInfoU[j].rtsp, g_record.channel[i].substream, RTSP_PATH_LEN);
            strncpy(avInfoU[j].save, g_record.channel[i].save_sub_path, SAVE_PATH_LEN);
        }
        avInfoU[j].callback = StreamPacketCallback;
        avInfoU[j].file = NULL;
        avInfoU[j].usbfile = NULL;
        avInfoU[j].duration_sec = 30;
        avInfoU[j].is_usb_stream = 1;
    }
    pthread_rwlock_unlock(&rwlock);

    /* 初始化互斥锁 */
    pthread_mutex_init(&lock, NULL);

    /* 断言线程池创建成功 */
    assert((pool = threadpool_create(thread_count, queue_size, 0)) != NULL);
    fprintf(stderr, "Pool started with %d threads and "
            "queue size of %d\n", thread_count, queue_size);
    threadpool_add(pool, hotplug, (void *)&g_usb_hotplug, 0);
    threadpool_add(pool, StreamProcess, (void *)&avInfo[0], 0);
//    threadpool_add(pool, StreamProcess, (void *)&avInfo[1], 0);
//    threadpool_add(pool, StreamProcess, (void *)&avInfo[2], 0);
//    threadpool_add(pool, StreamProcess, (void *)&avInfo[3], 0);
//    threadpool_add(pool, StreamProcess, (void *)&avInfo[4], 0);

    /* 只要任务队列还没满，就一直添加 */
    while(threadpool_add(pool, &dummy_task, NULL, 0) == 0) {
        pthread_mutex_lock(&lock);
        tasks++;
        pthread_mutex_unlock(&lock);
    }

    fprintf(stderr, "Added %d tasks\n", tasks);
//    sleep(180);
//    for (int i = 0; i < MAX_CHANNEL; i++) {
//        avInfo[i].thread_exit = 1;
//    }
//    g_usb_hotplug.thread_exit = 1;
    /* 这时候销毁线程池,0 代表 immediate_shutdown */
    assert(threadpool_destroy(pool, 0) == 0);

    return 0;
}
#endif
