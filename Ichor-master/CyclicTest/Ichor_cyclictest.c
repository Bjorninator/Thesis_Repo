// SPDX-License-Identifier: GPL-2.0-only

/*
 * Copyright (C) 2016 Red Hat Inc, Steven Rostedt <srostedt@redhat.com>
 * Copyright (C) 2019 John Kacur <jkacur@redhat.com>
 * Copyright (C) 2019 Clark Williams <williams@redhat.com>
 */

#include <pthread.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sched.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <getopt.h>

#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/vfs.h>

#include <linux/unistd.h>
#include <linux/magic.h>

#include <rt-utils.h>
#include <rt-sched.h>
#include <error.h>

#define _STR(x) #x
#define STR(x) _STR(x)
#ifndef MAXPATH
#define MAXPATH 1024
#endif

#define CPUSET_ALL	"my_cpuset_all"
#define CPUSET_LOCAL	"my_cpuset"

typedef unsigned long long u64;
typedef unsigned int u32;
typedef int s32;

/* Struct to transfer parameters to the thread */
struct thread_param {
	u64 runtime_us;
	u64 deadline_us;

	int mode;
	int timermode;
	int signal;
	int clock;
	unsigned long max_cycles;
	struct thread_stat *stats;
	unsigned long interval;
	int cpu;
	int node;
	int tnum;
};

/* Struct for statistics */
struct thread_stat {
	unsigned long cycles;
	unsigned long cyclesread;
	long min;
	long max;
	long act;
	double avg;
	long *values;
	long *hist_array;
	long *outliers;
	pthread_t thread;
	int threadstarted;
	int tid;
	long reduce;
	long redmax;
	long cycleofmax;
	long hist_overflow;
	long num_outliers;
};

struct sched_data {
	u64 runtime_us;
	u64 deadline_us;

	int bufmsk;

	struct thread_stat stat;

	char buff[BUFSIZ+1];
};

static int volatile shutdown;

static pthread_barrier_t barrier;

static int cpu_count;
static int all_cpus;

static int nr_threads;

static int quiet;

static void usage(int error)
{
	printf("cyclicdeadline V %1.2f\n", VERSION);
	printf("Usage:\n"
	       "cyclicdeadline <options>\n\n"
	       "-D TIME     --duration     Specify a length for the test run.\n"
	       "                           Append 'm', 'h', or 'd' to specify minutes, hours or\n"
	       "                           days\n"
	       "-h          --help         Show this help menu.\n"
	       "-i INTV     --interval     The shortest deadline for the tasks in us\n"
	       "                           (default 1000us).\n"
	       "-t NUM      --tasks      The number of tasks to run as deadline (default 1).\n"
	       "-q          --quiet        print a summary only on exit\n"
	       );
	exit(error);
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

static void print_stat(FILE *fp, struct sched_data *sd, int index, int verbose, int quiet)
{
	struct thread_stat *stat = &sd->stat;

	if (!verbose) {
		if (quiet != 1) {
			char *fmt;
			fmt = "T:%2d (%5d) I:%ld C:%7lu "
				"Min:%7ld Act:%5ld Avg:%5ld Max:%8ld\n";
			fprintf(fp, fmt, index, stat->tid,
				sd->deadline_us, stat->cycles, stat->min, stat->act,
				stat->cycles ?
				(long)(stat->avg/stat->cycles) : 0, stat->max);
		}
	} else {
		while (stat->cycles != stat->cyclesread) {
			long diff = stat->values
			    [stat->cyclesread & sd->bufmsk];

			if (diff > stat->redmax) {
				stat->redmax = diff;
				stat->cycleofmax = stat->cyclesread;
			}
			stat->cyclesread++;
		}
	}
}

static u64 do_runtime(long tid, struct sched_data *sd, u64 period)
{
	struct thread_stat *stat = &sd->stat;
	u64 next_period = period + sd->deadline_us;
	u64 now = get_time_us();
	u64 diff;

	if (now < period) {
		period = now;
		next_period = period + sd->deadline_us;
	}

	diff = now - period;
	if (diff > stat->max)
		stat->max = diff;
	if (!stat->min || diff < stat->min)
		stat->min = diff;
	stat->act = diff;
	stat->avg += (double) diff;

	stat->cycles++;

	return next_period;
}

void *run_deadline(void *data)
{
	struct sched_data *sd = data;
	struct thread_stat *stat = &sd->stat;
	struct sched_attr attr;
	long tid = gettid();
	u64 period;
	int ret;

	printf("deadline thread %ld\n", tid);

	stat->tid = tid;

	ret = sched_getattr(0, &attr, sizeof(attr), 0);
	if (ret < 0) {
		err_msg_n(errno, "[%ld]", tid);
		shutdown = 1;
		pthread_barrier_wait(&barrier);
		pthread_exit("Failed sched_getattr");
		return NULL;
	}

	pthread_barrier_wait(&barrier);

	attr.sched_policy = SCHED_DEADLINE;
	attr.sched_runtime = sd->runtime_us * 1000;
	attr.sched_deadline = sd->deadline_us * 1000;

	printf("thread[%d] runtime=%lldus deadline=%lldus\n",
	      gettid(), sd->runtime_us, sd->deadline_us);

	pthread_barrier_wait(&barrier);

	ret = sched_setattr(0, &attr, 0);
	if (ret < 0) {
		err_msg_n(errno, "[%ld]", tid);
		shutdown = 1;
		pthread_barrier_wait(&barrier);
		pthread_exit("Failed sched_setattr");
		return NULL;
	}

	pthread_barrier_wait(&barrier);

	sched_yield();
	period = get_time_us();

	while (!shutdown) {
		period = do_runtime(tid, sd, period);
		sched_yield();
	}
	ret = sched_getattr(0, &attr, sizeof(attr), 0);
	if (ret < 0) {
		err_msg_n(errno, "sched_getattr");
		pthread_exit("Failed second sched_getattr");
	}

	return NULL;
}

static void sighand(int sig)
{
	shutdown = 1;
}

static void loop(struct sched_data *sched_data, int nr_threads)
{
	int i;

	while (!shutdown) {
		for (i = 0; i < nr_threads; i++)
			print_stat(stdout, &sched_data[i], i, 0, quiet);
		usleep(10000);
		if (!quiet)
			printf("\033[%dA", nr_threads);
	}
	usleep(10000);
	if (!quiet) {
		printf("\033[%dB", nr_threads + 2);
	} else {
		for (i = 0; i < nr_threads; ++i)
			print_stat(stdout, &sched_data[i], i, 0, 0);
	}
}

int main(int argc, char **argv)
{
	struct sched_data *sched_data;
	struct sched_data *sd;
	pthread_t *thread;
	unsigned int interval = 1000;
	unsigned int step = 500;
	int percent = 60;
	int duration = 0;
	u64 runtime;
	int i;
	int c;

	cpu_count = sysconf(_SC_NPROCESSORS_CONF);
	if (cpu_count < 1)
		err_quit("Can not calculate number of CPUS\n");

	for (;;) {
		static struct option options[] = {
			{ "duration",	required_argument,	NULL,	'D' },
			{ "help",	no_argument,		NULL,	'h' },
			{ "interval",	required_argument,	NULL,	'i' },
			{ "Tasks",	required_argument,	NULL,	't' },
			{ "quiet",	no_argument,		NULL,	'q' },
			{ NULL,		0,			NULL,	0   },
		};
		c = getopt_long(argc, argv, "D:hi:t:q", options, NULL);
		if (c == -1)
			break;
		switch (c) {
		case 'i':
			interval = atoi(optarg);
			break;
		case 's':
			step = atoi(optarg);
			break;
		case 't':
			nr_threads = atoi(optarg);
			break;
		case 'D':
			duration = parse_time_string(optarg);
			break;
		case 'q':
			quiet = 1;
			break;
		case 'h':
			usage(0);
			break;
		default:
			usage(1);
		}
	}

	if (!nr_threads){
		nr_threads = 1;
	}
	
	printf("Using all CPUS\n");
	all_cpus = 1;
	

	thread = calloc(nr_threads, sizeof(*thread));
	sched_data = calloc(nr_threads, sizeof(*sched_data));
	if (!thread || !sched_data)
		fatal("allocating threads");

	/* Set up the data while sill in SCHED_FIFO */
	for (i = 0; i < nr_threads; i++) {
		sd = &sched_data[i];
		/*
		 * Interval is the deadline/period
		 * The runtime is the percentage of that period.
		 */
		runtime = interval * percent / 100;

		if (runtime < 2000) {
			/*
			 * If the runtime is less than 2ms, then we better
			 * have HRTICK enabled.
			 */
		}
		sd->runtime_us = runtime;
		sd->deadline_us = interval;

		interval += step;
	}


	pthread_barrier_init(&barrier, NULL, nr_threads + 1);

	for (i = 0; i < nr_threads; i++) {
		sd = &sched_data[i];
		pthread_create(&thread[i], NULL, run_deadline, sd);
	}

	pthread_barrier_wait(&barrier);

	if (shutdown)
		fatal("failed to setup child threads at step 1\n");

	printf("main thread %d\n", gettid());

	pthread_barrier_wait(&barrier);

	if (shutdown)
		fatal("failed to setup child threads at step 2");

	pthread_barrier_wait(&barrier);

	signal(SIGINT, sighand);
	signal(SIGTERM, sighand);
	signal(SIGALRM, sighand);
	// sighand();

	if (duration){
		alarm(duration);
	}
	loop(sched_data, nr_threads);

	printf("We reached the end\n");
	shutdown = 1;

	for (i = 0; i < nr_threads; i++) {
		// printf("does it get here\n");
		sd = &sched_data[i];
		// printf("or here?\n");
		pthread_join (thread[i], NULL);
		// printf("joined thread\n");
	}
	printf("We done here\n");

	free(thread);
	free(sched_data);

	return 0;
}
