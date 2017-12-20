#include <stdio.h>
#include <errno.h>

typedef struct parse_proc_status
{
	bool isNum;
	int value;
	char * name;
	char * str_value;
}parse_proc_status_t;

typedef struct parse_proc_status_node
{
	parse_proc_status_t data;
	struce parse_proc_status_node *next;
}parse_proc_status_node_t;

int main()
{
    char file[64] = {0};//文件名
    FILE *fd;         //定义文件指针fd
    char line_buff[256] = {0};  //读取行的缓冲区
	int pid = 1;
	size_t read_num = 0;
    sprintf(file,"/proc/%d/status",pid);//文件中第11行包含着

    //fprintf (stderr, "current pid:%d\n", pid);
    fd = fopen (file, "r"); //以R读的方式打开文件再赋给指针fd

    char name[32];//存放项目名称
	while(!feof(fd))
	{
		read_num = fread(line_buff, 256, 1, fd);
		if (ferror(fd))
		{
			fprintf(stderr, "[ERROR]fread return:%d ,err msg:%d\n", read_num, ferror(errno));
		}
	}

    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %s", name,file);
    sys_info.m_name = file;
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %s", name,file);
    sys_info.m_State = file;
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_Tgid);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_Pid);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_PPid);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_TracerPid);
    fgets (line_buff, sizeof(line_buff), fd);
    fgets (line_buff, sizeof(line_buff), fd);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_FDSize);
    fgets (line_buff, sizeof(line_buff), fd);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmPeak);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmSize);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmLck);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmHWM);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmRSS);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmData);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmStk);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmExe);
    fgets (line_buff, sizeof(line_buff), fd);//读取VmRSS这一行的数据,VmRSS在第15行
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmLib);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_VmPTE);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_threads);
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&sys_info.m_SigQ);
    fclose(fd);     //关闭文件fd
    return sys_info;
}

int parse_proc_status_file(char * src_buf, int size, parse_proc_status_node_t **pplist)
{
	char * p_line_begin = src_buf;
	char * p_line_end = src_buf;
	char * p_map_key = NULL;
	char * p_map_value = NULL;
	parse_proc_status_node_t *pnode = NULL;
	int i = 0;

	if ((NULL == src_buf) || (NULL == pplist))
	{
		fprintf(stderr, "[ERROR]input parameter invalid!\n");
	}

	for (i = 0; i < size; i++)
	{
		switch (*p_line_end)
		{
			case 9:
				break;
			case 0xa:
				break;
			case 0x20:
				break;
			default:
				if ('0' <= *p_line_end)
				break;
		}
		p_line_end++;
	}
}

int parse_one_line_proc_pid_status(char * src_buf, int size, parse_proc_status_node_t *pnode)
{
	int i = 0;
	char * p_line_end = src_buf;
	bool isnew_line = false;
	if (NULL == src_buf)
	{
		fprintf(stderr, "[ERROR]input parameter invalid!\n");
	}

	for (i = 0; i < size; i++)
	{
		switch (*p_line_end)
		{
			case 9:
				if (NULL == pnode)
				{
					pnode = (parse_proc_status_node_t *)malloc(sizeof(parse_proc_status_node_t));
				}
				break;
			case 0xa:
				isnew_line = true;
				break;
			case 0x20:
				break;
			case ':':
				break;
			default:
				if ('0' <= *p_line_end)
				break;
		}
		if (isnew_line)
		{
			break;
		}
		p_line_end++;
	}
}
