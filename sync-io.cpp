#include<iostream>
#include "liburing.h"
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <time.h>

using namespace std;

#define BUF_SIZE 1024
char buffer[BUF_SIZE];

int bench_write(char* out){
    int outfd;
    outfd = open(out, O_WRONLY|O_CREAT|O_TRUNC|O_APPEND, 0644);

    int limit_size = BUF_SIZE *1024;
    int offset = 0;
    int ret;

    while(offset < limit_size){
        // sync write
        ret = write(outfd, &buffer, BUF_SIZE);
        if(ret<0){
            cout << "write buffer error: " << ret << endl;
            return 1;
        }

        // sync fsync
        ret = fdatasync(outfd);
        if(ret<0){
            cout << "fdatasync error: " << ret << endl;
            return 1;
        }

        // advance the offset
        offset += BUF_SIZE;
    }
    return 0;
}

int main(int argc, char* argv[]){

    if (argc != 2){
        printf("%s outfile", argv[0]);
        return 1;
    }
    char* out = argv[1];

    // memset the buffer to '1'*1024
    memset(buffer, '1', sizeof(buffer)/sizeof(char));

    clock_t begin = clock();
    cout << "start the bench write" << endl;
    if(bench_write(out)){
        cout << "bench write error" << endl;
        return 1;
    }
    clock_t end = clock();
    cout << "End the native write & fdatasync bench write: "
         << (double)(end - begin) / (double)CLOCKS_PER_SEC << "(sec)" << endl;
    return 0;
}
