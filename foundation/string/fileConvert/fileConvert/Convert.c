#include <stdio.h>
#include <string.h>

void print_2(int val2);
/***********文本文件转二进制**********/
void Text2Bin(const char* sIn,const char* sOut){
    char a[255];
    unsigned char ch = 0;
    size_t i = 0;
    unsigned char write_buf[255];
    size_t read_count = 0;
    size_t write_count = 0;
    memset(a, 0, 255);
    FILE* fin=fopen(sIn,"rb");
    FILE* fout=fopen(sOut,"wb");
    int odd = 0;
    int run_ok = 1;
    while(run_ok)
    {
        memset(a, 0, 255);
        read_count = fread(a, 1, 255, fin);
//printf("read_count=%d\n", read_count);
        for (i = 0; i < read_count; i++)
        {
            switch(a[i])
            {
            case ' ':
                odd = 2;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                odd++;
                if (odd > 2) {
                    odd = 1;
                    write_buf[0] = ch;
                    ch = 0;
                    write_count = fwrite(write_buf, 1, 1, fout);
                    if (1 != write_count) {
                        printf("[%s][%d] write_count=%d\n", __FUNCTION__, __LINE__, write_count);
                        run_ok = 0;
                    }
                }
                ch = ch << 4;
//printf("ch = %d\n", ch);
                ch += (a[i] - '0');
                break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                odd++;
                if (odd > 2) {
                    odd = 1;
                    write_buf[0] = ch;
                    ch = 0;
                    write_count = fwrite(write_buf, 1, 1, fout);
                    if (1 != write_count) {
                        printf("[%s][%d] write_count=%d\n", __FUNCTION__, __LINE__, write_count);
                        run_ok = 0;
                    }
                }
                ch = ch << 4;
                ch += (10 + a[i] - 'a');
                break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
                odd++;
                if (odd > 2) {
                    odd = 1;
                    write_buf[0] = ch;
                    ch = 0;
                    write_count = fwrite(write_buf, 1, 1, fout);
                    if (1 != write_count) {
                        printf("[%s][%d] write_count=%d\n", __FUNCTION__, __LINE__, write_count);
                        run_ok = 0;
                    }
                }
                ch = ch << 4;
                ch += (10 + a[i] - 'A');
                break;
            default:
                printf("err bit = 0x%x\n", a[i]);
                continue;
            }
		//printf("a[i]=%c, 0x%x\n",a[i], ch);
        }
        if (feof(fin))
        {
	//printf("feof true\n");
            write_buf[0] = ch;
            ch = 0;
            write_count = fwrite(write_buf, 1, 1, fout);
            if (1 != write_count) {
                printf("write_count=%d\n", write_count);
            }
            break;
        }
	//printf("feof false\n");
    }
    fclose(fin);
    fclose(fout);
}

/***********二进制文件转文本文件**********/
void Bin2Text(const char* sIn,const char* sOut){
    FILE* fin=fopen(sIn,"rb");
    FILE* fout=fopen(sOut,"wb");
    unsigned char a[255];
    long bytes = 0;

    size_t read_count = 0;
    size_t write_count = 0;
    size_t i = 0;
    char cRead[4];
    char cChar[16] = {'0', '1', '2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

    while(!feof(fin))
    {
        memset(a, 0, 255);
        read_count = fread(a, 1, 255, fin);
        bytes += read_count;
        for (i = 0; i < read_count; i++)
        {
            cRead[0] = cChar[(a[i] / 16)];
            cRead[1] = cChar[(a[i] % 16)];
            cRead[2] = ' ';
            write_count = fwrite(cRead, 1, 3, fout);
            if (3 != write_count) {
                printf("write_count=%d\n", write_count);
            }
        }
    }
    fclose(fin);
    fflush(fout);
    if (ferror(fout)){
        printf("[%s][%d] saveStreamFile[%s]: Err: Unknown!***\n", __FUNCTION__, __LINE__, sOut);
    }
    fclose(fout);
    printf("convert :%ld, bytes\n", bytes);
}

int ConvertMain(int argc, char* argv[]){
    int i;
    char name[1024];
    if (argc < 3)
    {
        printf("Usage:./a.out bin data.bin , bin file to txt file.\n");
        printf("Usage:./a.out txt data.txt , txt file to bin file.\n");
        return 0;
    }
    for(i = 0; i<argc; i++)
    {
        printf("%d:%s\n", i, argv[i]);
    }
    memset(name, 0, sizeof(name));
    strcpy(name, argv[2]);
    if (0 == strncmp("bin", argv[1], 3))
    {
        strcat(name, ".txt");
        Bin2Text(argv[2],name);
    }
    if (0 == strncmp("txt", argv[1], 3))
    {
        strcat(name, ".bin");
        Text2Bin(argv[2],name);
    }
    //Text2Bin("0_111_1672_10-19-16-17.txt","a2.txt");
    printf("\nSuccessfully converted file!\n");
    return 0;
}

/***********二进制文件剪切文件**********/
void CutFile(const char* sIn,const char* sOut, long start_pos, long offset_pos){
    FILE* fin=fopen(sIn,"rb");
    FILE* fout=fopen(sOut,"wb");
    unsigned char a[255];
    long bytes = 0;
    fseek(fin, 0, SEEK_END);
    bytes = ftell(fin);
    if (bytes > start_pos) {
        fseek(fin, start_pos, SEEK_SET);
        if (bytes < start_pos + offset_pos) {
            offset_pos = bytes - start_pos;
        }
    } else {
        printf("[%s][%d] saveStreamFile[%s]: Err: Unknown!***\n", __FUNCTION__, __LINE__, sOut);
        return;
    }

    size_t read_count = 0;
    size_t write_count = 0;

    bytes = 0;
    while(!feof(fin))
    {
        memset(a, 0, 255);
        read_count = fread(a, 1, 255, fin);
        if (bytes + read_count > offset_pos) {
            read_count = offset_pos - bytes;
        }
        write_count = fwrite(a, 1, read_count, fout);
        if (read_count != write_count) {
            printf("read_count=%ld, write_count=%ld\n", read_count, write_count);
        }
        bytes += write_count;
        if (bytes >= offset_pos) {
            printf("bytes=%ld, offset_pos=%ld\n", bytes, offset_pos);
            break;
        }
    }
    fclose(fin);
    fflush(fout);
    if (ferror(fout)){
        printf("[%s][%d] saveStreamFile[%s]: Err: Unknown!***\n", __FUNCTION__, __LINE__, sOut);
    }
    fclose(fout);
    printf("convert :%ld, bytes\n", bytes);
}

int CuttingMain(int argc, char* argv[]){
    int i;
    char name[1024];
    char *stopstring;
    long start_pos = 0;
    long offset_pos = 0;
    if (argc < 4)
    {
        printf("Usage:./a.out 0 35210 data.bin , cut_data.bin file is create, file size 35210.\n");
        return 0;
    }
    for(i = 0; i<argc; i++)
    {
        printf("%d:%s\n", i, argv[i]);
    }
    memset(name, 0, sizeof(name));
//    strcpy(name, "cut_");
//    strcat(name, argv[3]);
    strcpy(name, argv[3]);
    strcat(name, "cut.bin");
    start_pos = strtol(argv[1], &stopstring, 10);
    offset_pos = strtol(argv[2], &stopstring, 10);
    CutFile(argv[3], name, start_pos, offset_pos);

    //Text2Bin("0_111_1672_10-19-16-17.txt","a2.txt");
    printf("\nSuccessfully cut file!\n");
    return 0;
}
