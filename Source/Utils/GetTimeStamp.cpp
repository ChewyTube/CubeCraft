#include "GetTimeStamp.h"

//微秒级时间戳
long long getTimeStamp_us() {
    //从1970-01-01 00：00：00到现在的时长
    std::chrono::system_clock::duration duration_since_epoch = std::chrono::system_clock::now().time_since_epoch();
    //转换为微秒数
    long long microseconds_since_epoch = std::chrono::duration_cast<std::chrono::microseconds>(duration_since_epoch).count();
    return microseconds_since_epoch;
}

//秒级时间戳
int getTimeStamp_s() {
    time_t t;
    time(&t);
    return t;
}
