//
// Created by cxp on 2019-08-05.
//

#include "sys/time.h"

//获取系统时间
int64_t GetCurMsTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t ts = (int64_t) tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return ts;
}