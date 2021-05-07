#define MAP_SYNC 0x080000 /* perform synchronous page faults for the mapping */
#define MAP_SHARED_VALIDATE 0x03    /* share + validate extension flags */

#include <iostream>
#include <memory>
#include <fstream>

#include<sys/mman.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/time.h>

#include<immintrin.h>

#include <iostream>
#include <algorithm> // std::move_backward
#include <random> // std::default_random_engine
#include <chrono> // std::chrono::system_clock

using namespace std;
// 映射区长度，单位byte
int ACCESS_REGION_SIZE; 
int BLOCK_SIZE = 64;
int BLOCK_NUM;
int test_num = 1024*1024*16;

// open the file with mmap
char * open_file(const char * filename){
    int fd = open(filename, O_CREAT | O_RDWR, 0644);
    // mmap 是以页框大小为单位进行内存映射的
    char * pt = (char *) mmap(NULL, ACCESS_REGION_SIZE, PROT_READ|PROT_WRITE, MAP_SYNC | MAP_SHARED_VALIDATE, fd, 0 );
    if(pt == MAP_FAILED){
        cout << "Map Failed..\n";
        return NULL;
    }
    close(fd);
    return pt;
}

inline int get_random(){
    return rand()%BLOCK_NUM;
}

double testStore4CertainSize(int size){
    ACCESS_REGION_SIZE = size;
    BLOCK_NUM = ACCESS_REGION_SIZE/BLOCK_SIZE;
    int * index = new int[BLOCK_NUM];
    for(int i = 0; i < BLOCK_NUM; i++){
        index[i] = i;
    }
    shuffle(index, index + BLOCK_NUM, default_random_engine(size));

    srand((unsigned)time(NULL)); 
    struct timespec t1, t2;
    clock_gettime(CLOCK_REALTIME, &t1);
    clock_gettime(CLOCK_REALTIME, &t2);
    
    char * src = open_file("/aepmount1/data.txt");  
    long long total_delay = 0;
    __m512i dst;
    dst = _mm512_stream_load_si512((void *)src);

    int test_case = BLOCK_NUM;
    for(int i = 0; i < test_case; i++){
        // __m256i dst;
        int offset = index[i];

        clock_gettime(CLOCK_REALTIME, &t1);
        _mm512_stream_si512((__m512i *)(src + offset*BLOCK_SIZE), dst);
        clock_gettime(CLOCK_REALTIME, &t2);

        _mm_clflush(src + offset*BLOCK_SIZE);//clwb
        _mm_mfence();
        
        // delay in nanosecond
        long long delay = (long long)(t2.tv_sec - t1.tv_sec) * 1000000000LL + (t2.tv_nsec - t1.tv_nsec);
        total_delay += delay;
    }
    
    delete [] index;
    munmap(src, ACCESS_REGION_SIZE);
    // printf("Average delay is %.2f ns.\n",total_delay * 1.0/ test_case);
    // printf("%s\n", src);
    return total_delay * 1.0/ test_case;
}

int main(){
    double result;
    
    ofstream outPut("res_store.csv");
    for(int i = 64; i < test_num; i+=64){
        // cout << "Test for " << i << "kB: " <<endl;
        if(i % (1024*32) == 0) cout << i/(1024*32) <<endl;
        // result[i] = test4CertainSize(i);
        result = testStore4CertainSize(i);
        outPut << i << ", "  << result << endl;
    }

    return 0;
}