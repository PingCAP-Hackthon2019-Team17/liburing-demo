#include <iostream>
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

#define QD 64

#define BUF_SIZE 1024
char buffer[BUF_SIZE];

static int setup_context(unsigned entries, struct io_uring *ring)
{
	int ret;
	ret = io_uring_queue_init(entries, ring, 0);
	if (ret < 0) {
		fprintf(stderr, "queue_init: %s\n", strerror(-ret));
		return -1;
	}
	return 0;
}

int bench_write(char* out){
    int outfd;
    struct io_uring ring;
    outfd = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if(setup_context(QD, &ring)){
        return 1;
    }

    int limit_size = BUF_SIZE * 1024;
    int offset = 0;
    int ret;

    struct io_uring_sqe *sqe;
    struct io_uring_cqe *cqe;
    struct iovec iov ;
    iov.iov_base = &buffer;
    iov.iov_len = BUF_SIZE;

    while(offset < limit_size){
        // async write
        sqe = io_uring_get_sqe(&ring);
        io_uring_prep_writev(sqe, outfd, &iov, 1, offset);
        io_uring_sqe_set_data(sqe, &iov);
        io_uring_submit(&ring);

        // async fsync
        sqe = io_uring_get_sqe(&ring);
        io_uring_prep_fsync(sqe, outfd, IORING_FSYNC_DATASYNC);
        io_uring_submit(&ring);

        // wait the write + fsync result.
        for(int i = 0; i < 2; i++){
            ret =io_uring_wait_cqe(&ring, &cqe);
            if(ret<0){
                printf("wait cqe: %s\n", strerror(ret));
                return 1;
            }
            io_uring_cqe_seen(&ring, cqe);
        }

        // advance the offset.
        offset += BUF_SIZE;
    }
    close(outfd);
    return 0;
}

int main(int argc, char* argv[]){
    if(argc !=2){
        printf("%s outfile", argv[0]);
        return 1;
    }
    char* out = argv[1];

    // memset the buffer to '1' * 1024
    memset(buffer, '1', sizeof(buffer)/sizeof(char));

    clock_t begin = clock();
    cout << "start the bench write"<< endl;
    if(bench_write(out)){
        cout << "bench write error" << endl;
        return 1;
    }
    clock_t end = clock();
    cout << "End the uring-async write & sync bench write: " << (end - begin) << "(ms)" << endl;
    return 0;
}
