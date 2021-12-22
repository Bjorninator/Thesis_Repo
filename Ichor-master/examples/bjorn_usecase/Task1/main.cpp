#include "../SafetyCheckService.h"
#include "ControlLoopService.h"
#include "CalculationService.h"

#include <ichor/CommunicationChannel.h>

#include <ichor/optional_bundles/logging_bundle/LoggerAdmin.h>

#include <ichor/optional_bundles/logging_bundle/NullFrameworkLogger.h>
#include <ichor/optional_bundles/logging_bundle/NullLogger.h>

#define FRAMEWORK_LOGGER_TYPE NullFrameworkLogger
#define LOGGER_TYPE NullLogger

#include <chrono>
#include <iostream>
#include "../MemoryResources.h"
#include "../GlobalRealtimeSettings.h"

using namespace std::string_literals;

std::atomic<uint64_t> idCounter = 0;
CommunicationChannel channel{};

void* run_example1(void*) {

    cpu_set_t lock_to_core_set;
    CPU_ZERO(&lock_to_core_set);
    CPU_SET(1, &lock_to_core_set);
    sched_setaffinity(0, sizeof(cpu_set_t), &lock_to_core_set);

    // disable usage of default std::pmr resource, as that would allocate.
    // terminating_resource terminatingResource{};
    // std::pmr::set_default_resource(&terminatingResource);

    
    //     buffer_resource<1024 * 1024> resourceOne{};
    //     buffer_resource<1024 * 1024> resourceTwo{};

    //     DependencyManager dm{&resourceOne, &resourceTwo};
        DependencyManager dm{};
        channel.addManager(&dm);

        dm.createServiceManager<FRAMEWORK_LOGGER_TYPE, IFrameworkLogger>({}, 10);
        dm.createServiceManager<LoggerAdmin<LOGGER_TYPE>, ILoggerAdmin>();
        dm.createServiceManager<ControlLoopService, IControlLoopService>();
        dm.createServiceManager<CalculationService, ICalculationService>();
        dm.startBST();

    return nullptr;
}

void* run_example2(void*) {

    cpu_set_t lock_to_core_set;
    CPU_ZERO(&lock_to_core_set);
    CPU_SET(2, &lock_to_core_set);
    sched_setaffinity(0, sizeof(cpu_set_t), &lock_to_core_set);

    // // disable usage of default std::pmr resource, as that would allocate.
    // terminating_resource terminatingResource{};
    // std::pmr::set_default_resource(&terminatingResource);

    
    //     buffer_resource<1024 * 1024> resourceOne{};
    //     buffer_resource<1024 * 1024> resourceTwo{};

    //     DependencyManager dm{&resourceOne, &resourceTwo};
        DependencyManager dm{};

        channel.addManager(&dm);

        dm.createServiceManager<FRAMEWORK_LOGGER_TYPE, IFrameworkLogger>({}, 10);
        dm.createServiceManager<LoggerAdmin<LOGGER_TYPE>, ILoggerAdmin>();
        dm.createServiceManager<SafetyCheckService, ISafetyCheckService>();
        dm.startBST();

    return nullptr;
}

int main() {

    auto start = std::chrono::steady_clock::now();
    uid_t uid = getuid();
    uid_t euid = geteuid();

    if (uid !=0 || uid!=euid) {
        throw std::runtime_error("No permissions to set realtime scheduling. Consider running under sudo/root.");
    }

   // GlobalRealtimeSettings::GlobalRealtimeSettings settings{};

    // create a thread with realtime priority to run the program on
    pthread_t thread{};
    sched_param param{};
    pthread_attr_t attr{};
    pthread_attr_init (&attr);
    // pthread_attr_setinheritsched (&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy (&attr, SCHED_FIFO);
    param.sched_priority = 99;
    pthread_attr_setschedparam (&attr, &param);
    auto ret = pthread_create (&thread, &attr, run_example1, nullptr);


    pthread_t thread1{};
    sched_param param1{};
    pthread_attr_t attr1{};
    pthread_attr_init (&attr1);
    // pthread_attr_setinheritsched (&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy (&attr1, SCHED_FIFO);
    param.sched_priority = 99;
    pthread_attr_setschedparam (&attr1, &param1);
    auto ret1 = pthread_create (&thread1, &attr1, run_example2, nullptr);

    if(ret == EPERM || ret1 == EPERM) {
        throw std::runtime_error("No permissions to set realtime scheduling. Consider running under sudo/root.");
    }
        pthread_join(thread1, nullptr);
        pthread_join(thread, nullptr);

    auto end = std::chrono::steady_clock::now();
    fmt::print("Program ran for {:L} µs\n", std::chrono::duration_cast<std::chrono::microseconds>(end-start).count());
    return 0;
}
