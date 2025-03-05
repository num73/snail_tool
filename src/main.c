#include <unistd.h>
#include <stdio.h>
#include "define.h"
#include "st.h"
#include <stdlib.h>

int main(int argc, char * argv[]) {
    printf("====================================\n");
    printf("=       Welcome to Snail Tool      =\n");
    printf("=            author: num73         =\n");
    printf("====================================\n");

    int opt;
    struct st_options st_opt;

    while((opt = getopt(argc, argv, "rwRb:f:p:")) != -1) {
        switch (opt) {
            case 'r':
                st_opt.read = 1;
                break;
            case 'w':
                st_opt.write = 1;
                break;
            case 'R':
                st_opt.random = 1;
                break;
            case 'b':
                st_opt.record_size = atoi(optarg);
                break;
            case 'f':
                st_opt.file_size = atoi(optarg) * _MB;
                break;
            case 'p':
                st_opt.file_path = optarg;
                break;
            default:
                printf("Usage: %s -r -w -R -b <record_size> -f <file_size> -p <file_path>\n", argv[0]);
                return -1;
        
        }
    }

    // st_test();
    do_test(&st_opt);
    return 0;
}