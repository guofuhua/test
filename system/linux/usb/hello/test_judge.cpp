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
static int check_mount(STORAGE_DEVICE_INFO *dev);
static int do_mount(STORAGE_DEVICE_INFO *dev);
static int do_umount(STORAGE_DEVICE_INFO *dev);
int process_dev(STORAGE_DEVICE_MANAGE *manage);
void get_usb_path(char *buf, int flag);

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


int DvrCpFile(char *srcFile, char *targetFile, int findString, char *string)
{
    int ret = 0;
    FILE *srcfp = NULL;
    FILE *targetfp = NULL;
    int length = 0;
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
                            length = fwrite(line, 1, size, targetfp);
//                            printf("[%s][%d] line:%s, size:%d, length:%d\n", __FUNCTION__, __LINE__, line, size, length);
                        }
                    }
                    else
                    {
                        length = fwrite(line, 1, size, targetfp);
//                        printf("[%s][%d] line:%s, size:%d, length:%d\n", __FUNCTION__, __LINE__, line, size, length);
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
                printf("[%s] apply, index:%d\n", __FUNCTION__, i, resource[i]);
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


int process_dev(STORAGE_DEVICE_MANAGE *manage)
{
    for (int i = 0; i < manage->count; i++) {
        printf("[%s][%d] count:%d, index:%d state:%d, path:%s\n", __FUNCTION__, __LINE__, manage->count, i, manage->device[i].state, manage->device[i].path);
        if (STORAGE_DEVICE_INSERT == manage->device[i].state)//检测设备是否插上
        {
            if (!check_mount(&manage->device[i]))//检测设备是否mount上了
            {
                do_mount(&manage->device[i]);
            }
        }
        else if (STORAGE_DEVICE_MOUNTED != manage->device[i].state)
        {
            if (check_mount(&manage->device[i]))
            {
                do_umount(&manage->device[i]);
            }
        }
        printf("[%s][%d] count:%d, index:%d state:%d, path:%s\n", __FUNCTION__, __LINE__, manage->count, i, manage->device[i].state, manage->device[i].path);
    }
    return 0;
}

static int check_mount(STORAGE_DEVICE_INFO * dev)
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
                printf("token2:%s\n", token);
                get_usb_path(dev->path, 0);
                strncpy(dev->path, token, SAVE_PATH_LEN);
                get_usb_path(dev->path, 2);
                dev->state = STORAGE_DEVICE_MOUNTED;
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
    if (0 != mkdir(dev->path, 0777)) {
        printf("[%s] mkdir errno=%d, Mesg:%s\n", __FUNCTION__, errno, strerror(errno));
    }
    printf("mount %s %s\n", dev->partition_name, dev->path);
    if (dev->partition_count) {
        if (mount(dev->partition_name, dev->path, "vfat" ,MS_MGC_VAL, "codepage=936,iocharset=gb2312") == 0)
        {
            dev->state = STORAGE_DEVICE_MOUNTED;
            return 0;
        }
        else
        {
            printf("[%s][%d] mount errno=%d, Mesg:%s\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        }
    } else {
        if (mount(dev->partition_name, dev->path, "exfat" , MS_MGC_VAL, "codepage=936,iocharset=gb2312") == 0)
        {
            dev->state = STORAGE_DEVICE_MOUNTED;
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
    printf("umount %s %s\n", dev->partition_name, dev->path);
    if (umount(dev->path) == 0)
    {
        get_usb_path(dev->path, 0);
        dev->state = STORAGE_DEVICE_REMOVED;
        return 0;
    }
    else
    {
        printf("umount errno=%d, Mesg:%s\n",errno, strerror(errno));
    }
    return 1;
}
