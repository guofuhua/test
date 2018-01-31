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
    FILE* fin=fopen(sIn,"r");
    FILE* fout=fopen(sOut,"w");
    int odd = 0;
    while(1)
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
                        printf("write_count=%d\n", write_count);
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
                        printf("write_count=%d\n", write_count);
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
                        printf("write_count=%d\n", write_count);
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
    FILE* fin=fopen(sIn,"r");
    FILE* fout=fopen(sOut,"w");
    unsigned char a[255];

    size_t read_count = 0;
    size_t write_count = 0;
    size_t i = 0;
    char cRead[4];
    char cChar[16] = {'0', '1', '2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    while(!feof(fin))
    {
        memset(a, 0, 255);
        read_count = fread(a, 1, 255, fin);
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
    fclose(fout);
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
