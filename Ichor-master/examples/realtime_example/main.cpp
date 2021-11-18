#include "TestService.h"
#include "OptionalService.h"
#include <ichor/optional_bundles/logging_bundle/LoggerAdmin.h>
#include "MemoryResources.h"
#include "GlobalRealtimeSettings.h"
#include <ichor/optional_bundles/logging_bundle/NullFrameworkLogger.h>
#include <ichor/optional_bundles/logging_bundle/NullLogger.h>

#define FRAMEWORK_LOGGER_TYPE NullFrameworkLogger
#define LOGGER_TYPE NullLogger

#include <chrono>
#include <stdexcept>


using namespace std::string_literals;

std::atomic<uint64_t> idCounter = 0;

void* run_example(void*) {
    cpu_set_t lock_to_core_set;
    CPU_ZERO(&lock_to_core_set);
    CPU_SET(1, &lock_to_core_set);
    sched_setaffinity(0, sizeof(cpu_set_t), &lock_to_core_set);

    auto start = std::chrono::steady_clock::now();

    {
        DependencyManager dm{};
        dm.createServiceManager<FRAMEWORK_LOGGER_TYPE, IFrameworkLogger>({}, 10);
        dm.createServiceManager<LoggerAdmin<LOGGER_TYPE>, ILoggerAdmin>();
        dm.createServiceManager<OptionalService, IOptionalService>();
       // dm.createServiceManager<OptionalService, IOptionalService>();
        dm.createServiceManager<TestService>();
        dm.startFP();
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << "Can you print?";
    fmt::print("Program ran for {:L} µs\n", std::chrono::duration_cast<std::chrono::microseconds>(end-start).count());

    return nullptr;
}

int main() {

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
    param.sched_priority = 20;
    pthread_attr_setschedparam (&attr, &param);
    auto ret = pthread_create (&thread, &attr, run_example, nullptr);

    if(ret == EPERM) {
        throw std::runtime_error("No permissions to set realtime scheduling. Consider running under sudo/root.");
    }
    
    pthread_join(thread, NULL);

    return 0;
}
