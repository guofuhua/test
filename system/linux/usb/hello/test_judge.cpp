/*************************************************************************
    > File Name: test.c
    > Author: lizhu
    > Mail: 1489306911@qq.com
    > Created Time: 2015年11月28日 星期六 11时05分05秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "storage.h"
#define STANDARD_6A_LICENSE_FILE    ("license.txt")

extern void set_ALERT_Msg(char * buf);
#define FILE_DEV_CHECK "/proc/scsi/scsi" //用来检测设备数和类型
#define FILE_MOUNT_CHECK "/proc/mounts" //用来检测设备是否被mount
#define FILE_DISC_PARTS_CHECK "/proc/partitions" //用来检测设备的分区情况
#define FILE_DEV_STATU_TEMPL "/proc/scsi/usb-storage-%d/%d"   //用来检测某一设备是否在插入状态
#define FILE_DEV_PART_TEMPL "/dev/scsi/host%d/bus0/target0/lun0/" //具体的设备
#define USB_CDROM_MP "/tmp/cdrom"
#define USB_DISK_MP "/tmp/usbdisk"
#define DBG_PRINT(x) printf("[AutoMounter]");printf x
#define DBG_PRINT2(x) printf("[AutoMounter]");printf x
#define MAX_NAME_LEN 64
#define MAX_PART_NUM 6   //最多允许6个分区
typedef struct s_scsi_usb_dev
{
    int type; /*1 cdrom 2 disk */
    int index; /*like host0 host1*/
    char file_statu[MAX_NAME_LEN]; /*like "/proc/scsi/usb-storage-%d/%d"*/
    char devfile_parts[MAX_PART_NUM][MAX_NAME_LEN]; /*存储每个分区的设备文件*/
    char mount_path[MAX_PART_NUM][MAX_NAME_LEN]; /*与上面对应的mount点*/
    int part_num; //分区数
    struct s_scsi_usb_dev * next_dev; //指向下一个备
} SCSI_USB_DEV;
static int check_mount(STORAGE_DEVICE_INFO *dev, char *dev_path);
static int do_mount(STORAGE_DEVICE_INFO *dev);
static int do_umount(STORAGE_DEVICE_INFO *dev);
int process_dev(STORAGE_DEVICE_MANAGE *manage);
void get_usb_path(char *buf, int flag);
void check_mount_event();

extern pthread_rwlock_t rwlock;
extern MONITOR_RECORD g_record;

#define DEVTYPE_DISK 2
#define DEVTYPE_U    3
//外部设备的子结构
typedef struct tagDevParam_T
{
    unsigned char  devname[12];
    unsigned char  devtype;///2----硬盘，3----U盘
    unsigned char  partition_count;
    unsigned int dev_totalspace;//KB
    unsigned int partition_size[12];/*分区大小，单位KB*/
} tagDevParam_t, *ptagDevParam_t;

typedef struct tagDevinfo_T
{
    tagDevParam_t dev[8];
    int     devcount; /*总共的设备数量*/
    int     disk_num;
} tagDevinfo_t, *ptagDevinfo_t;


int DvrCpFile(const char *srcFile, const char *targetFile, int findString, const char *string)
{
    int ret = 0;
    FILE *srcfp = NULL;
    FILE *targetfp = NULL;
#if (DEBUG)
    int length = 0;
#endif
    ssize_t size;
    size_t len = 0;
    char *p = NULL;
    char *line = NULL;

    if(access(srcFile, F_OK) != 0)
    {
        printf("ERROR: src file is not exist!");
        return -1;
    }
    if(ret == 0)
    {
        srcfp = fopen(srcFile, "r");
        targetfp = fopen(targetFile, "w+");
        if((targetfp != NULL) && (srcfp != NULL))
        {
            while((size = getline(&line, &len, srcfp)) != -1)
            {
//                printf("[%s][%d] line:%s, size:%d\n", __FUNCTION__, __LINE__, line, size);
                if(size > 0)
                {
                    if(findString == 1)
                    {
                        p = strstr(line, string);
                        if(p != NULL)
                        {
#if (DEBUG)
                            length = fwrite(line, 1, size, targetfp);
                            printf("[%s][%d] line:%s, size:%d, length:%d\n", __FUNCTION__, __LINE__, line, size, length);
#else
                            fwrite(line, 1, size, targetfp);
#endif
                        }
                    }
                    else
                    {
#if (DEBUG)
                            length = fwrite(line, 1, size, targetfp);
                            printf("[%s][%d] line:%s, size:%d, length:%d\n", __FUNCTION__, __LINE__, line, size, length);
#else
                            fwrite(line, 1, size, targetfp);
#endif
                    }
                }
            }
            fclose(srcfp);
            fclose(targetfp);
            if(line)
            {
                free(line);
            }
            ret = 0;
        }
        else
        {
            if(srcfp)
            {
                printf("ERROR: open flie %s", targetFile);
                fclose(srcfp);
            }
            if(targetfp)
            {
                printf("ERROR: open flie %s", srcFile);
                fclose(targetfp);
            }
            ret = -1;
        }
    }
    return ret;
}

STORAGE_DEVICE_TYPE_E CheckIsDiskOrUsbDisk(char *devname)
{
    FILE *fp;
    STORAGE_DEVICE_TYPE_E  ret = UNKNOW_DEVICE;
    char buffer[80], * line = NULL;
    size_t len = 0;
    int fal=0;
    int type = -1;

    memset(buffer, 0, 80);
    char tmpfilename[64];
    if(tmpnam(tmpfilename) == NULL)
    {
        sprintf(tmpfilename, "%s", "/tmp/check_dev_type.txt");
    }
    fp = fopen(tmpfilename, "w+");
    sprintf(buffer, "/sys/block/%s/removable", devname);
    DvrCpFile(buffer, tmpfilename, fal, NULL);
    getline(&line, &len, fp);
    fclose(fp);
    type = atoi(line);
    switch(type)
    {
    case 0:
        ret = HDD_DEVICE;
        break;
    case 1:
        ret = USB_DEVICE;
        break;
    default:
        ret = UNKNOW_DEVICE;
        break;
    }
    if(line)
    {
        free(line);
    }
    remove(tmpfilename);
    return ret;
}
int test_main(){

    int           xx=1,i=0;
    STORAGE_DEVICE_TYPE_E dev_tpye;
    ssize_t       size;
    size_t        len = 0;
    unsigned char k;
    char          tmp_devname[12];
    char          *line = NULL;
    char          *pname;
    char          *token;
    char          seps[] = " ";
    FILE          *fp = NULL;

    bzero(tmp_devname, 12);
//    bzero(&devinfo, sizeof(devinfo));
    DvrCpFile("/proc/partitions", "/tmp/partitions.txt", xx, "sd");

    fp = fopen("/tmp/partitions.txt", "r");
    /*获得系统 硬盘 和移动设备总结构*/
    while((size = getline(&line, &len, fp)) != -1)
    {
        printf("line:%s\n", line);
        pname = strrchr(line, ' ');
        if(pname == NULL)
            continue;
        pname++;
        printf("pname:%s\n", pname);
        if(memcmp(pname, tmp_devname, 3) == 0) //新分区
        {
            i = 0;
            token = strtok(line, seps);
            while(token != NULL)
            {
                if(i == 2)
                {
                    k = g_record.storage_manage.device[g_record.storage_manage.count - 1].partition_count;
                    g_record.storage_manage.device[g_record.storage_manage.count - 1].partition_size[k]=atoi(token);
                }
                i++;
                token = strtok(NULL, seps);
            }
            g_record.storage_manage.device[g_record.storage_manage.count - 1].partition_count++;
//            sprintf(g_record.storage_manage.device[g_record.storage_manage.count - 1].partition_name, "/dev/%s", pname);
            sprintf(g_record.storage_manage.device[g_record.storage_manage.count - 1].partition_name, "/dev/%s1", g_record.storage_manage.device[g_record.storage_manage.count - 1].name);
        }
        else//新设备
        {
            memcpy(g_record.storage_manage.device[g_record.storage_manage.count].name, pname, 3);
            sprintf(g_record.storage_manage.device[g_record.storage_manage.count].partition_name, "/dev/%s", g_record.storage_manage.device[g_record.storage_manage.count].name);
            i = 0;
            token = strtok(line, seps);
            while(token != NULL)
            {
                if(i == 2)
                {
                    g_record.storage_manage.device[g_record.storage_manage.count].totalspace = atoi(token);
                }
                printf("[%s][%d] i:%d token:%s, space:%d Kb\n", __FUNCTION__, __LINE__, i, token, g_record.storage_manage.device[g_record.storage_manage.count].totalspace);
                i++;
                token = strtok(NULL, seps);
            }
            g_record.storage_manage.count++;
        }
        memcpy(tmp_devname, pname, 3);
    }

    printf("dve count =%d\n",g_record.storage_manage.count);
    for(i = 0; i < g_record.storage_manage.count; i++){
        printf("dev name=%s\n",g_record.storage_manage.device[i].name);

        dev_tpye = CheckIsDiskOrUsbDisk(g_record.storage_manage.device[i].name);
        g_record.storage_manage.device[i].type = dev_tpye;
        if(g_record.storage_manage.device[i].type == USB_DEVICE)
        {
            printf("this is USB drive\n");
            g_record.storage_manage.device[i].state = STORAGE_DEVICE_INSERT;
            get_usb_path(g_record.storage_manage.device[i].path, 1);
            g_record.storage_manage.device[i].authorize = 1; //guofh test
        }
        if(g_record.storage_manage.device[i].type == HDD_DEVICE)
        {
            printf("this is  hard drive\n");
            g_record.storage_manage.device[i].state = STORAGE_DEVICE_INSERT;
            memset(g_record.storage_manage.device[i].path, 0, SAVE_PATH_LEN);
            strncpy(g_record.storage_manage.device[i].path, "/hdd/sda1", SAVE_PATH_LEN);
        }
    }

    printf("device count:%d\n", g_record.storage_manage.count);
    for(i = 0; i < g_record.storage_manage.count; i++) {
        printf("device name:%s\n", g_record.storage_manage.device[i].name);
        printf("device type:%d\n", g_record.storage_manage.device[i].type);
        printf("partition count:%d\n", g_record.storage_manage.device[i].partition_count);
        printf("device total spaces:%d Kb\n", g_record.storage_manage.device[i].totalspace);
        for (int j = 0; j < g_record.storage_manage.device[i].partition_count; j++) {
            printf("partition %d size:%d Kb\n", j+1, g_record.storage_manage.device[i].partition_size[j]);
        }
    }

    process_dev(&g_record.storage_manage);
    printf("mount over\n");

    return 0;
}

void get_usb_path(char *buf, int flag)
{
    static int resource[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    if (1 == flag) {
        memset(buf, 0, SAVE_PATH_LEN);
        for (int i = 0; i < 10; i++) {
            if (0 == resource[i]) {
                sprintf(buf, "/hdd/usb%d", i + 1);
                printf("[%s] apply:%d, index:%d\n", __FUNCTION__, i, resource[i]);
                resource[i] = 1;
                break;
            }
        }
    } else if (2 == flag) {
        resource[buf[strlen(buf)-1] - '1'] = 1;
        printf("[%s] modified, len:%d, index:%d\n", __FUNCTION__, strlen(buf), buf[strlen(buf)-1] - '1');
    } else {
        resource[buf[strlen(buf)-1] - '1'] = 0;
        printf("[%s] clean, len:%d, index:%d\n", __FUNCTION__, strlen(buf), buf[strlen(buf)-1] - '1');
    }

//    printf("[%s][%d] ", __FUNCTION__, __LINE__);
//    for (int i = 0; i < 9; i++) {
//        printf("%d ", i + 1);
//    }
//    printf("\n");
    printf("[%s][%d] ", __FUNCTION__, __LINE__);
    for (int i = 0; i < 9; i++) {
        printf("%d ", resource[i]);
    }
    printf("\n");
}

int check_udisk_authorize(char *path)
{
    ssize_t       size;
    size_t        len = 0;
    char          *line = NULL;
    char          *pname;
    int step = 0;
    char key[5][2][12] = {{{"Username"}, 0}, {{"Password"}, 0}, {{"Channel"}, 0}, {{"TimeFrom"}, 0}, {{"TimeSpan"}, 0}};
    printf("[%s] key size:%d\n", __FUNCTION__, sizeof(key));
    char license_file[SAVE_PATH_LEN];
    sprintf(license_file, "%s/%s", path, STANDARD_6A_LICENSE_FILE);
    if (access(license_file, R_OK)) {
        printf("[%s][%d] access errno=%d, Mesg:%s\n", __FUNCTION__, __LINE__, errno, strerror(errno));
    } else {
        FILE *file = fopen(license_file, "r");
        if (NULL == file) {
            printf("[%s][%d] fopen errno=%d, Mesg:%s\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        } else {
            /* get 6A license */
            while((size = getline(&line, &len, file)) != -1)
            {
                printf("line:%s\n", line);
                if ((pname = strstr(line, key[step][0]))) {
                    printf("[%s] key:%s\n", __FUNCTION__, pname);
                    if ((pname = strchr(pname, '['))) {
                        printf("[%s] value:%s\n", __FUNCTION__, pname);
                        step++;
                    }
                }
            }
            fclose(file);
        }
    }
    printf("[%s][%d] exit(%d)\n", __FUNCTION__, __LINE__, step);
    return step;
}

void check_mount_event()
{
    int loop;
    char path[SAVE_PATH_LEN];
    struct timespec start_time, now_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    do {
        loop = 0;
        usleep(500000);
        pthread_rwlock_rdlock(&rwlock);     //lock
        for (int i = 0; i < g_record.storage_manage.count; i++) {
            printf("[%s][%d] count:%d, index:%d state:%d, path:%s, partition:%s\n", __FUNCTION__, __LINE__, g_record.storage_manage.count, i, g_record.storage_manage.device[i].state, g_record.storage_manage.device[i].path, g_record.storage_manage.device[i].partition_name);
            if (STORAGE_DEVICE_REMOVED == g_record.storage_manage.device[i].state) {
                loop |= 1;
                if (check_mount(&g_record.storage_manage.device[i], NULL))//检测设备是否mount上了
                {
                    if (0 == do_umount(&g_record.storage_manage.device[i])) {
//                        g_record.storage_manage.device[i].state = STORAGE_DEVICE_NONE;

                        get_usb_path(g_record.storage_manage.device[i].path, 0);
                        pthread_rwlock_unlock(&rwlock); //unlock
                        pthread_rwlock_wrlock(&rwlock); //write lock
                        for (int j = i; j < g_record.storage_manage.count - 1; j++) {
                            memcpy(&g_record.storage_manage.device[j], &g_record.storage_manage.device[j + 1], sizeof(STORAGE_DEVICE_INFO));
                        }
                        memset(&g_record.storage_manage.device[g_record.storage_manage.count - 1], 0 ,sizeof(STORAGE_DEVICE_INFO));
                        g_record.storage_manage.count--;
                        pthread_rwlock_unlock(&rwlock); //write unlock
                        pthread_rwlock_rdlock(&rwlock); //lock
                        i--;    //delete one data;
                    } else {
                        clock_gettime(CLOCK_MONOTONIC, &now_time);
                        if (now_time.tv_sec > start_time.tv_sec + 2) {
                            pthread_rwlock_unlock(&rwlock); //unlock
                            pthread_rwlock_wrlock(&rwlock); //write lock
                            g_record.storage_manage.device[i].state = STORAGE_DEVICE_MOUNT_E;
                            pthread_rwlock_unlock(&rwlock); //write unlock
                            pthread_rwlock_rdlock(&rwlock); //lock
                        }
                    }
                } else {
                    get_usb_path(g_record.storage_manage.device[i].path, 0);
                    pthread_rwlock_unlock(&rwlock); //unlock
                    pthread_rwlock_wrlock(&rwlock); //write lock
                    for (int j = i; j < g_record.storage_manage.count - 1; j++) {
                        memcpy(&g_record.storage_manage.device[j], &g_record.storage_manage.device[j + 1], sizeof(STORAGE_DEVICE_INFO));
                    }
                    memset(&g_record.storage_manage.device[g_record.storage_manage.count - 1], 0 ,sizeof(STORAGE_DEVICE_INFO));
                    g_record.storage_manage.count--;
                    pthread_rwlock_unlock(&rwlock); //write unlock
                    pthread_rwlock_rdlock(&rwlock); //lock
                    i--;    //delete one data;
                }
                break;
            } else if (STORAGE_DEVICE_INSERT == g_record.storage_manage.device[i].state) {
                loop |= 2;
                if (!check_mount(&g_record.storage_manage.device[i], path))//检测设备是否mount上了
                {
                    if (0 == do_mount(&g_record.storage_manage.device[i])) {
                        strncpy(path, g_record.storage_manage.device[i].path, SAVE_PATH_LEN);
                        pthread_rwlock_unlock(&rwlock); //unlock

                        /** check udisk is authorized */

                        pthread_rwlock_wrlock(&rwlock); //write lock
                        g_record.storage_manage.device[i].state = STORAGE_DEVICE_MOUNTED;
                        pthread_rwlock_unlock(&rwlock); //write unlock
                        pthread_rwlock_rdlock(&rwlock); //lock
                    } else {
                        clock_gettime(CLOCK_MONOTONIC, &now_time);
                        if ((EINVAL == errno) || (now_time.tv_sec > start_time.tv_sec + 2)) {
                            pthread_rwlock_unlock(&rwlock); //unlock
                            pthread_rwlock_wrlock(&rwlock); //write lock
                            g_record.storage_manage.device[i].state = STORAGE_DEVICE_MOUNT_E;
                            pthread_rwlock_unlock(&rwlock); //write unlock
                            pthread_rwlock_rdlock(&rwlock); //lock
                        }
                    }
                } else {
                    pthread_rwlock_unlock(&rwlock); //unlock
                    pthread_rwlock_wrlock(&rwlock); //write lock
                    get_usb_path(g_record.storage_manage.device[i].path, 0);
                    strncpy(g_record.storage_manage.device[i].path, path, SAVE_PATH_LEN);
                    get_usb_path(g_record.storage_manage.device[i].path, 2);
                    g_record.storage_manage.device[i].state = STORAGE_DEVICE_MOUNTED;
                    pthread_rwlock_unlock(&rwlock); //write unlock
                    pthread_rwlock_rdlock(&rwlock); //lock
                }
            }
        }

        pthread_rwlock_unlock(&rwlock); //unlock
    }while (loop);
}


int process_dev(STORAGE_DEVICE_MANAGE *manage)
{
    char path[SAVE_PATH_LEN];
    for (int i = 0; i < manage->count; i++) {
        printf("[%s][%d] count:%d, index:%d state:%d, path:%s, partition:%s\n", __FUNCTION__, __LINE__, manage->count, i, manage->device[i].state, manage->device[i].path, manage->device[i].partition_name);
//        printf("[%s][%d] count:%d, index:%d state:%d, path:%s\n", __FUNCTION__, __LINE__, manage->count, i, manage->device[i].state, manage->device[i].path);
        if (STORAGE_DEVICE_INSERT == manage->device[i].state)//检测设备是否插上
        {
            if (!check_mount(&manage->device[i], path))//检测设备是否mount上了
            {
                if (0 == do_mount(&manage->device[i])) {
                    manage->device[i].state = STORAGE_DEVICE_MOUNTED;
                } else {
                    manage->device[i].state = STORAGE_DEVICE_MOUNT_E;
                }
            } else {
                get_usb_path(manage->device[i].path, 0);
                strncpy(manage->device[i].path, path, SAVE_PATH_LEN);
                get_usb_path(manage->device[i].path, 2);
                manage->device[i].state = STORAGE_DEVICE_MOUNTED;
                if (USB_DEVICE == manage->device[i].type) {
                    check_udisk_authorize(manage->device[i].path);
                }
            }
        }
//        else if (STORAGE_DEVICE_MOUNTED != manage->device[i].state)
//        {
//            if (check_mount(&manage->device[i], path))
//            {
//                if (0 == do_umount(&manage->device[i])) {
//                    manage->device[i].state = STORAGE_DEVICE_REMOVED;
//                }
//            }
//        }
        printf("[%s][%d] count:%d, index:%d state:%d, path:%s\n", __FUNCTION__, __LINE__, manage->count, i, manage->device[i].state, manage->device[i].path);
    }
    return 0;
}

static int check_mount(STORAGE_DEVICE_INFO * dev, char * dev_path)
{
    FILE *fp = NULL;
    ssize_t       size;
    size_t        len = 0;
    char          *line = NULL;
    char          *token;
    char          seps[] = " ";
    int ret = 0;

    fp = fopen(FILE_MOUNT_CHECK, "r");
    while((size = getline(&line, &len, fp)) != -1)
    {
//        printf("line:%s\n", line);
        token = strtok(line, seps);
        if ((token != NULL) && (0 == strcmp(token, dev->partition_name)))
        {
//            printf("token1:%s\n", token);
            token = strtok(NULL, seps);
            if(NULL != token)
            {
                printf("[%s] token2:%s\n", __FUNCTION__, token);
                if (NULL != dev_path) {
//                    get_usb_path(dev->path, 0);
                    strncpy(dev_path, token, SAVE_PATH_LEN);
//                    get_usb_path(dev->path, 2);
//                    dev->state = STORAGE_DEVICE_MOUNTED;
                }
                ret = 1;
                break;
            }
        }
    }
    fclose(fp);

    return ret;
}

static int do_mount(STORAGE_DEVICE_INFO * dev)
{
    if(access(dev->path, X_OK) != 0) {
        printf("[%s] access %s error, errno=%d, Mesg:%s\n", __FUNCTION__, dev->path, errno, strerror(errno));
        if (0 != mkdir(dev->path, 0777)) {
            printf("[%s] mkdir errno=%d, Mesg:%s\n", __FUNCTION__, errno, strerror(errno));
        }
    }
    printf("[%s] mount %s %s\n", __FUNCTION__, dev->partition_name, dev->path);
    if (dev->partition_count) {
//        EACCES;
        if (mount(dev->partition_name, dev->path, "vfat" ,MS_MGC_VAL, "codepage=936,iocharset=gb2312") == 0)
//        if (mount(dev->partition_name, dev->path, "exfat" ,MS_MGC_VAL, NULL) == 0)
        {
//            dev->state = STORAGE_DEVICE_MOUNTED;
            return 0;
        }
        else
        {
            printf("[%s][%d] mount errno=%d, Mesg:%s\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        }
    } else {
        if (mount(dev->partition_name, dev->path, "exfat" , MS_MGC_VAL, "codepage=936,iocharset=gb2312") == 0)
        {
//            dev->state = STORAGE_DEVICE_MOUNTED;
            return 0;
        }
        else
        {
            printf("[%s][%d] mount errno=%d, Mesg:%s\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        }
    }
    return 1;
}

static int do_umount(STORAGE_DEVICE_INFO * dev)
{
    printf("[%s] umount %s %s\n", __FUNCTION__, dev->partition_name, dev->path);
    if (umount(dev->path) == 0)
    {
        return 0;
    }
    else
    {
        printf("umount errno=%d, Mesg:%s\n",errno, strerror(errno));
    }
    return 1;
}
