#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>

int main() {
    struct timeval start_time, end_time;
    long long total_bytes_read;
    int n;
    
    // 从1字节到512KB的不同缓冲区大小
    for (int BUFFER_SIZE = 1; BUFFER_SIZE <= 524288; BUFFER_SIZE *= 2) {
        char buffer[BUFFER_SIZE]; // 读取缓冲区
        total_bytes_read = 0;
        
        // 获取开始时间
        gettimeofday(&start_time, NULL);
        
        // 打开文件
        int file_no = open("./testfile", O_RDONLY);
        if (file_no == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        // 读取文件内容
        while (1) {
            n = read(file_no, buffer, BUFFER_SIZE);
            if (n < 0) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            if (n == 0) {
                // 已经到达文件末尾
                break;
            }
            total_bytes_read += n;
        }

        // 获取结束时间
        gettimeofday(&end_time, NULL);
        
        // 计算读取所花费的时间（以毫秒为单位）
        long long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000LL +
                                (end_time.tv_usec - start_time.tv_usec) / 1000LL;
        
        // 打印结果
        printf("Buffer size: %d bytes\n", BUFFER_SIZE);
        printf("Total bytes read: %lld\n", total_bytes_read);
        printf("Time taken: %lld ms\n", elapsed_time);
        printf("Read rate: %.2f MB/s\n", (double)total_bytes_read / (double)elapsed_time / 1024.0);

        // 关闭文件
        close(file_no);
    }
    
    return 0;
}
