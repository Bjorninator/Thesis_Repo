#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <time.h>
#include <fmt/core.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <sys/syscall.h>
#include <pthread.h>

#define gettid() syscall(__NR_gettid)

#define SCHED_DEADLINE	6

/* XXX use the proper syscall numbers */
#ifdef __x86_64__
#define __NR_sched_setattr		314
#define __NR_sched_getattr		315
#endif

static volatile int done;

struct sched_attr {
	__u32 size;
	
	__u32 sched_policy;
	__u64 sched_flags;
	
	/* SCHED_NORMAL, SCHED_BATCH */
	__s32 sched_nice;
	
	/* SCHED_FIFO, SCHED_RR */
	__u32 sched_priority;
	
	/* SCHED_DEADLINE (nsec) */
	__u64 sched_runtime;
	__u64 sched_deadline;
	__u64 sched_period;
};

int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags)
{
	return syscall(__NR_sched_setattr, pid, attr, flags);
}

int sched_getattr(pid_t pid, struct sched_attr *attr, unsigned int size, unsigned int flags)
{
	return syscall(__NR_sched_getattr, pid, attr, size, flags);
}

void *run_deadline(void *data)
 {
    struct sched_attr attr;
    int x = 0, ret, idk;
    unsigned int flags = 0;
    unsigned cpu, numa;

    printf("deadline thread start %ld\n", gettid());

    attr.size = sizeof(attr);
    attr.sched_flags = 0;
    attr.sched_nice = 0;
    attr.sched_priority = 0;

     /* creates a 10ms/30ms reservation */
    attr.sched_policy = SCHED_DEADLINE;
    attr.sched_runtime = 2 * 1000000;
    attr.sched_period  = 5 * 1000000;
    attr.sched_deadline= 5 * 1000000;

    ret = sched_setattr(0, &attr, flags);
    if (ret < 0) {
        done = 0;
        perror("sched_setattr");
        exit(-1);
    }

    while (!done) {
		getcpu(&cpu, &numa);
        printf("task1: %u %u\n", cpu, numa);
        // sched_yield();
	}
    
    return NULL;
}

void *run_deadline2(void *data)
 {
    struct sched_attr attr;
    int x = 0, ret, idk;
    unsigned int flags = 0;
    unsigned cpu, numa;
    
    printf("deadline thread start %ld\n", gettid());

    attr.size = sizeof(attr);
    attr.sched_flags = 0;
    attr.sched_nice = 0;
    attr.sched_priority = 0;

     /* creates a 10ms/30ms reservation */
    attr.sched_policy = SCHED_DEADLINE;
    attr.sched_runtime = 4 * 1000000;
    attr.sched_period  = 7 * 1000000;
    attr.sched_deadline= 7 * 1000000;

    ret = sched_setattr(0, &attr, flags);
    if (ret < 0) {
        done = 0;
        perror("sched_setattr");
        exit(-1);
    }

    while (!done) {
        getcpu(&cpu, &numa);
        printf("task2: %u %u\n", cpu, numa);
        sched_yield();
    }
    return NULL;
}

int main (int argc, char **argv)
{
   
	pthread_t thread;
	printf("main thread [%ld]\n", gettid());
	pthread_create (&thread, NULL, run_deadline, NULL);
    pthread_create (&thread, NULL, run_deadline2, NULL);
 	sleep(60);
	done = 1;
	pthread_join (thread, NULL);
	
	printf("main dies [%ld]\n", gettid());
	return 0;
}