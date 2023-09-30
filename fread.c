#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define BUFFER_SIZE 1024 // 初始缓冲区大小

int main() {
    struct timeval start_time, end_time;
    long long total_bytes_read;
    
    // 打开文件
    FILE *file = fopen("./testfile", "rb");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // 初始化缓冲区
    char *buffer = (char *)malloc(BUFFER_SIZE);
    if (buffer == NULL) {
        perror("malloc");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // 测试不同缓冲区大小
    for (int buffer_size = 1; buffer_size <= 524288; buffer_size *= 2) {
        // 重置文件指针
        fseek(file, 0, SEEK_SET);
        total_bytes_read = 0;

        // 获取开始时间
        gettimeofday(&start_time, NULL);

        // 读取文件内容
        while (1) {
            size_t bytes_read = fread(buffer, 1, buffer_size, file);
            if (bytes_read == 0) {
                // 已经到达文件末尾
                break;
            }
            total_bytes_read += bytes_read;
        }

        // 获取结束时间
        gettimeofday(&end_time, NULL);
        
        // 计算读取所花费的时间（以毫秒为单位）
        long long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000LL +
                                (end_time.tv_usec - start_time.tv_usec) / 1000LL;
        
        // 打印结果
        printf("Buffer size: %d bytes\n", buffer_size);
        printf("Total bytes read: %lld\n", total_bytes_read);
        printf("Time taken: %lld ms\n", elapsed_time);
        printf("Read rate: %.2f MB/s\n", (double)total_bytes_read / (double)elapsed_time / 1024.0);
    }

    // 关闭文件和释放内存
    fclose(file);
    // free(buffer);

    return 0;
}
