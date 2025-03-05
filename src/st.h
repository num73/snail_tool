#ifndef ST_H
#define ST_H

#include <stdint.h>
struct st_options{
    
    int read; // -r
    int write; // -w
    int random; // -R
    uint64_t record_size; // -b
    uint64_t file_size; // -f
    char *file_path; // -p
};

void st_test();

void do_test(struct st_options *opt);

#endif
