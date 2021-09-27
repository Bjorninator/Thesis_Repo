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
#include <error.h>

#include "UsingTimerService.h"
#include "UsingTimerService1.h"
#include "OtherTimerService.h"
#include <ichor/optional_bundles/logging_bundle/LoggerAdmin.h>
#ifdef USE_SPDLOG
#include <ichor/optional_bundles/logging_bundle/SpdlogFrameworkLogger.h>
#include <ichor/optional_bundles/logging_bundle/SpdlogLogger.h>

#define FRAMEWORK_LOGGER_TYPE SpdlogFrameworkLogger
#define LOGGER_TYPE SpdlogLogger
#else
#include <ichor/optional_bundles/logging_bundle/CoutFrameworkLogger.h>
#include <ichor/optional_bundles/logging_bundle/CoutLogger.h>

#define FRAMEWORK_LOGGER_TYPE CoutFrameworkLogger
#define LOGGER_TYPE CoutLogger
#endif
#include <chrono>
#include <iostream>
#include "MemoryResources.h"
#include "GlobalRealtimeSettings.h"

using namespace std::string_literals;

static pthread_barrier_t barrier;

static int cpu_count;
static int all_cpus;

static int nr_threads;

static int quiet;

using namespace std::string_literals;

std::atomic<uint64_t> idCounter = 0;

void* run_example(void*) {

    cpu_set_t lock_to_core_set;
    CPU_ZERO(&lock_to_core_set);
    CPU_SET(1, &lock_to_core_set);
    sched_setaffinity(0, sizeof(cpu_set_t), &lock_to_core_set);

    unsigned int interval = 1000;
	unsigned int step = 500;
	int percent = 60;
	int duration = 0;
	int i;
	int c;

	if (!nr_threads){
		nr_threads = 1;
	}

    auto start = std::chrono::steady_clock::now();

    // disable usage of default std::pmr resource, as that would allocate.
    terminating_resource terminatingResource{};
    std::pmr::set_default_resource(&terminatingResource);

    {
        buffer_resource<1024 * 1024> resourceOne{};
        buffer_resource<1024 * 1024> resourceTwo{};
        DependencyManager dm{&resourceOne, &resourceTwo};
        dm.createServiceManager<FRAMEWORK_LOGGER_TYPE, IFrameworkLogger>({}, 10);
        dm.createServiceManager<LoggerAdmin<LOGGER_TYPE>, ILoggerAdmin>();
        // dm.createServiceManager<UsingTimerService, IUsingTimerService>();
        // dm.createServiceManager<OtherTimerService, IOtherTimerService>();
        dm.startFP();
    }
    auto end = std::chrono::steady_clock::now();

    fmt::print("Program ran for {:L} µs\n", std::chrono::duration_cast<std::chrono::microseconds>(end-start).count());

    return nullptr;
}

int main() {
    std::locale::global(std::locale("en_US.UTF-8"));

    uid_t uid = getuid();
    uid_t euid = geteuid();

    if (uid !=0 || uid!=euid) {
        throw std::runtime_error("No permissions to set realtime scheduling. Consider running under sudo/root.");
    }

    GlobalRealtimeSettings settings{};

    // create a thread with realtime priority to run the program on
    pthread_t thread{};
    sched_param param{};
    pthread_attr_t attr{};
    pthread_attr_init (&attr);
    pthread_attr_setinheritsched (&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy (&attr, SCHED_FIFO);
    param.sched_priority = 99;
    pthread_attr_setschedparam (&attr, &param);
    auto ret = pthread_create (&thread, &attr, run_example, nullptr);

    if(ret == EPERM) {
        throw std::runtime_error("No permissions to set realtime scheduling. Consider running under sudo/root.");
    }

    if(ret == 0) {
        pthread_join(thread, nullptr);
    }

    return 0;
}
