#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    int rand_num = 0, final_num;
    int type, len, i;
    char *str = NULL;
    FILE *fp;

    scanf("%d%d", &type, &len);

    str = (char *)calloc(len+5, 1);
    if (str == NULL) {
        printf("memory calloc failed!\n");
        return -1;
    }
    srand (time(NULL));

    for (i = 0; i < len; i++) {

        rand_num = rand();
        if (type == 0)
        {
            final_num = rand_num % 106 + 21;
            *(str+i) = (char)final_num;     
        }
        else
        {
            final_num = rand_num % 3;
            switch(final_num)  
            {  
                case 0:  
                    *(str+ i) = rand_num%26 + 'a';   
                    break;  
                case 1:  
                    *(str+ i) = rand_num%26 + 'A';   
                     break;  
                case 2:  
                    *(str+ i) = rand_num%10 + '0';   
                    break;  
            }  
        }
    }
    // printf("%s\n", str);

    fp =fopen("else.txt", "w");
    if (fp == NULL){
        printf("open file failed\n");
        return -1;
    }
    fwrite(str, 1, strlen(str), fp);
    fclose(fp); 

    free(str);
    return 0;
}