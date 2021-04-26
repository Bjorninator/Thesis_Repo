#define _GNU_SOURCE
#include <sched.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <stdio.h>     
#include <stdlib.h>     
#include <unistd.h>    
#include <errno.h>     
#include <string.h>     
#include <stdbool.h>  

#define CSEC_STEP 25            /* CPU centiseconds between messages */

static void
useCPU(char *msg)
{
    struct tms tms;
    int cpuCentisecs, prevStep, prevSec;

    prevStep = 0;
    prevSec = 0;
    for (;;) {
        if (times(&tms) == -1)
            perror("times");
        cpuCentisecs = (tms.tms_utime + tms.tms_stime) * 100 /
                        sysconf(_SC_CLK_TCK);

        if (cpuCentisecs >= prevStep + CSEC_STEP) {
            prevStep += CSEC_STEP;
            printf("%s (PID %ld) cpu=%0.2f\n", msg, (long) getpid(),
                    cpuCentisecs / 100.0);
        }

        if (cpuCentisecs > 300)         
            break;

        if (cpuCentisecs >= prevSec + 100) {    
            prevSec = cpuCentisecs;
            sched_yield();
        }
    }
}

int
main(int argc, char *argv[])
{
    struct rlimit rlim;
    struct sched_param sp;
    cpu_set_t set;

    setbuf(stdout, NULL);            
    CPU_ZERO(&set);
    CPU_SET(1, &set);

    if (sched_setaffinity(getpid(), sizeof(set), &set) == -1)
        perror("sched_setaffinity");

    
    rlim.rlim_cur = rlim.rlim_max = 50;
    if (setrlimit(RLIMIT_CPU, &rlim) == -1)
        perror("setrlimit");


    sp.sched_priority = sched_get_priority_min(SCHED_FIFO);
    if (sp.sched_priority == -1)
        perror("sched_get_priority_min");

    if (sched_setscheduler(0, SCHED_FIFO, &sp) == -1)
        perror("sched_setscheduler");

    switch (fork()) {
    case -1:
        perror("fork");

    case 0:
        useCPU("child ");
        exit(EXIT_SUCCESS);

    default:
        useCPU("parent");
        exit(EXIT_SUCCESS);
    }
}
