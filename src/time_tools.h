#ifndef TIME_TOOLS_H 
#define TIME_TOOLS_H
#include <time.h>

#define SEC_TO_NSEC 1000000000

inline long calc_diff_nsec(struct timespec *start, struct timespec *end);

#endif