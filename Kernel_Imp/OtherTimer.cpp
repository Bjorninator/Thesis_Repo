#include <mutex>
#include <iostream>

 typedef std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> MyTimePoint;
 typedef unsigned long long u64;
 
int main()
{
   while(true) {

    clock_gettime(CLOCK_REALTIME, &time1);

    loopRate.sleep();

    clock_gettime(CLOCK_REALTIME, &time2);
    
    time_between = ((double)time2.tv_sec*SEC2NANOSEC + (double)time2.tv_nsec) - ((double)time1.tv_sec*SEC2NANOSEC + (double)time1.tv_nsec);  

    printf("frequency measured with gettime is %f Hz\n", 1/(time_between/1000000000));
}
}

 static u64 get_time_us(void)
        {
            struct timespec ts;
            u64 time;

            clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
            time = ts.tv_sec * 1000000;
            time += ts.tv_nsec / 1000;

            return time;
        }