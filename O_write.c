#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#define FILE_PATH "./output_file"  // 输出文件的路径

int main() {
    struct timeval start_time, end_time;
    
    // 打开文件并设置O_SYNC标志
    int file_fd = open(FILE_PATH, O_WRONLY | O_CREAT | O_SYNC, 0644);
    if (file_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    for (int BUFFER_SIZE = 524288; BUFFER_SIZE >=1; BUFFER_SIZE /= 2) {
        lseek(file_fd, 0, SEEK_SET);
        char buffer[BUFFER_SIZE];  // 写入缓冲区
        long long total_bytes_written = 0;

        // 获取开始时间
        gettimeofday(&start_time, NULL);

        // 循环写入数据，直到达到一定的大小
        while (total_bytes_written < 52428800) {  // 50MB
            ssize_t bytes_written = write(file_fd, buffer, BUFFER_SIZE);
            if (bytes_written == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
            total_bytes_written += bytes_written;
        }

        // 获取结束时间
        gettimeofday(&end_time, NULL);

        // 计算写入所花费的时间（以毫秒为单位）
        long long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000LL +
                                (end_time.tv_usec - start_time.tv_usec) / 1000LL;

        printf("Buffer size: %d bytes\n", BUFFER_SIZE);
        printf("Total bytes written: %lld\n", total_bytes_written);
        printf("Time taken: %lld ms\n", elapsed_time);
        printf("Write rate: %.2f MB/s\n", (double)total_bytes_written / (double)elapsed_time / 1024.0);
    }

    // 关闭文件
    close(file_fd);

    return 0;
}
