#include "st.h"
#include "time_tools.h"
#include "define.h"
#include <stdint.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

double seq_read(const char *path, uint64_t record_size, 
    uint64_t file_size, uint64_t times) {

    struct timespec stopwatch_start;
	struct timespec stopwatch_stop;
    volatile char *buf = (char *)malloc(record_size);
    uint64_t total_time_nsec = 0;
    int fd = open(path, O_RDONLY);
    if(fd < 0) {
        perror("open failed");
        return -1;
    }

    uint64_t cnt = 0;
    while(cnt < times) {
        clock_gettime(CLOCK_MONOTONIC, &stopwatch_start);
        read(fd, (void *)buf, record_size);
        clock_gettime(CLOCK_MONOTONIC, &stopwatch_stop);
        total_time_nsec += calc_diff_nsec(&stopwatch_start, &stopwatch_stop);
        if(cnt % (file_size / record_size) == 0) {
            lseek(fd, 0, SEEK_SET);
        }
        cnt++;
    }
    close(fd);
    free((void *)buf);
    return (double)(times * record_size) / (double)(total_time_nsec) * SEC_TO_NSEC;
}

double seq_write(const char *path, uint64_t record_size, 
    uint64_t file_size, uint64_t times) {

    struct timespec stopwatch_start;
	struct timespec stopwatch_stop;
    volatile char *buf = (char *)malloc(record_size);
    memset(buf, 'a', record_size);
    uint64_t total_time_nsec = 0;
    int fd = open(path, O_WRONLY);
    if(fd < 0) {
        perror("open failed");
        return -1;
    }

    uint64_t cnt = 0;
    while(cnt < times) {
        clock_gettime(CLOCK_MONOTONIC, &stopwatch_start);
        write(fd, (void *)buf, record_size);
        clock_gettime(CLOCK_MONOTONIC, &stopwatch_stop);
        total_time_nsec += calc_diff_nsec(&stopwatch_start, &stopwatch_stop);
        if(cnt % (file_size / record_size) == 0) {
            lseek(fd, 0, SEEK_SET);
        }
        cnt++;
    }
    close(fd);
    free((void *)buf);
    return (double)(times * record_size) / (double)(total_time_nsec) * SEC_TO_NSEC;
}

double rand_read(const char *path, uint64_t record_size, 
    uint64_t file_size, uint64_t times) {

    struct timespec stopwatch_start;
	struct timespec stopwatch_stop;
    volatile char *buf = (char *)malloc(record_size);
    uint64_t total_time_nsec = 0;
    int fd = open(path, O_RDONLY);
    if(fd < 0) {
        perror("open failed");
        return -1;
    }

    uint64_t cnt = 0;
    while(cnt < times) {
        off_t offset = (rand() % (file_size / record_size)) * record_size;
        if (lseek(fd, offset, SEEK_SET) == -1) {
            perror("Failed to seek");
            close(fd);
            exit(EXIT_FAILURE);
        }
        clock_gettime(CLOCK_MONOTONIC, &stopwatch_start);
        read(fd, (void *)buf, record_size);
        clock_gettime(CLOCK_MONOTONIC, &stopwatch_stop);
        total_time_nsec += calc_diff_nsec(&stopwatch_start, &stopwatch_stop);
  
        cnt++;
    }
    close(fd);
    free((void *)buf);
    return (double)(times * record_size) / (double)(total_time_nsec) * SEC_TO_NSEC;
}


double rand_write(const char *path, uint64_t record_size, 
    uint64_t file_size, uint64_t times) {

    struct timespec stopwatch_start;
	struct timespec stopwatch_stop;
    volatile char *buf = (char *)malloc(record_size);
    memset(buf, 'a', record_size);
    uint64_t total_time_nsec = 0;
    int fd = open(path, O_WRONLY);
    if(fd < 0) {
        perror("open failed");
        return -1;
    }

    uint64_t cnt = 0;
    while(cnt < times) {
        off_t offset = (rand() % (file_size / record_size)) * record_size;
        if (lseek(fd, offset, SEEK_SET) == -1) {
            perror("Failed to seek");
            close(fd);
            exit(EXIT_FAILURE);
        }
        clock_gettime(CLOCK_MONOTONIC, &stopwatch_start);
        write(fd, (void *)buf, record_size);
        clock_gettime(CLOCK_MONOTONIC, &stopwatch_stop);
        total_time_nsec += calc_diff_nsec(&stopwatch_start, &stopwatch_stop);
        cnt++;
    }
    close(fd);
    free((void *)buf);
    return (double)(times * record_size) / (double)(total_time_nsec) * SEC_TO_NSEC;
}


void create_file(const char *path, uint64_t record_size, uint64_t file_size) {
    printf("Creating file %s with size %lld\n", path, file_size);
    int fd = open(path, O_CREAT | O_WRONLY);
    if(fd < 0) {
        perror("open failed");
        return;
    }
    char *buf = (char *)malloc(record_size);
    for (int i = 0; i < file_size / record_size; i++) {
        write(fd, (void *)buf, record_size);
    }
    close(fd);
    free(buf);
    printf("File created\n");
}



void del_file(const char *path) {
    printf("Deleting file %s\n", path);

    if(unlink(path) < 0) {
        perror("unlink failed");
        return;
    }
    printf("File deleted\n");
}

void st_test() {
    create_file("./test", _4KB, 128 * _MB);
    double throughput = seq_read("./test", _4KB, 256 * _MB, 1000000);
    printf("Throughput: %f MB\n", throughput / 1024 / 1024);
    throughput = seq_write("./test", _4KB, 256 * _MB, 1000000);
    printf("Throughput: %f MB\n", throughput / 1024 / 1024);
    throughput = rand_read("./test", _4KB, 256 * _MB, 1000000);
    printf("Throughput: %f MB\n", throughput / 1024 / 1024);
    throughput = rand_write("./test", _4KB, 256 * _MB, 1000000);
    printf("Throughput: %f MB\n", throughput / 1024 / 1024);
    del_file("/Volumes/ABox/test");
}

void do_test(struct st_options *opt) {
    create_file(opt->file_path, opt->record_size, opt->file_size);
    if(opt->read) {
        if(opt->random) {
            printf("Random read\n");
            double throughput = rand_read(opt->file_path, opt->record_size, opt->file_size, 1000000);
            printf("Throughput: %f MB\n", throughput / 1024 / 1024);
        } else {
            printf("Sequential read\n");
            double throughput = seq_read(opt->file_path, opt->record_size, opt->file_size, 1000000);
            printf("Throughput: %f MB\n", throughput / 1024 / 1024);
        }
    } else {
        if(opt->random) {
            printf("Random write\n");
            double throughput = rand_write(opt->file_path, opt->record_size, opt->file_size, 1000000);
            printf("Throughput: %f MB\n", throughput / 1024 / 1024);
        } else {
            printf("Sequential write\n");
            double throughput = seq_write(opt->file_path, opt->record_size, opt->file_size, 1000000);
            printf("Throughput: %f MB\n", throughput / 1024 / 1024);
            
        }
    }
    del_file(opt->file_path);
}