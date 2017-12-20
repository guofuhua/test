#include <stdio.h>
#include <string.h>
int getSDPath(char *path);

void main()
{
    char buf[32];
    memset(buf,0,32);
    if (0 == getSDPath(buf))
    {
        printf("%s\n", buf);
    }
    return;
}

int getSDPath(char *path)
{
    if (NULL == path)
        return 1;
    FILE *ptr = NULL;
    const char *_sddev = "df | grep mmcblk | awk '{print $6}'";
    char buf[240] = {0};
    memset(buf,0,240);
    if((ptr=popen(_sddev, "r"))!=NULL)
    {
        fread(buf, sizeof(char), sizeof(buf) - 1, ptr);
        pclose(ptr);
        buf[strlen(buf)-1] = 0;
        strcpy(path, buf);
    }
    else
    {
        printf("failed 2\n");
        return 1;
    }
    return 0;
}
