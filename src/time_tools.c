#include "time_tools.h"
#include "define.h"

long calc_diff_nsec(struct timespec *start, struct timespec *end){
    return (end->tv_sec * (long)(SEC_TO_NSEC) + end->tv_nsec) -
    (start->tv_sec * (long)(SEC_TO_NSEC) + start->tv_nsec);
}