#pragma once

#include <ichor/DependencyManager.h>
#include <ichor/optional_bundles/logging_bundle/Logger.h>
#include <ichor/optional_bundles/timer_bundle/TimerService.h>
#include <ichor/Service.h>
#include <ichor/LifecycleManager.h>

using namespace Ichor;


struct IUsingTimerService {
};

class UsingTimerService final : public IUsingTimerService, public Service<UsingTimerService> {
public:
    UsingTimerService(DependencyRegister &reg, Properties props, DependencyManager *mng) : Service(std::move(props), mng) {
        reg.registerDependency<ILogger>(this, true);
    }
    ~UsingTimerService() final = default;

    bool start() final {
        _customEventHandler = getManager()->registerEventHandler<Custom1Event>(this);

        u64 period = get_time_us();
        typedef std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> MyTimePoint;
        MyTimePoint startTimePoint = std::chrono::time_point_cast<MyTimePoint::duration>(std::chrono::steady_clock::time_point(std::chrono::steady_clock::now()));
        startTimePoint += std::chrono::milliseconds(15);
        getManager()->pushPrioritisedEvent<Custom1Event>(getServiceId(),10 , 15, period, startTimePoint);
        return true;
    }

    bool stop() final {
        _customEventHandler.reset();
        _timerManager = nullptr;
        return true;
    }

    void addDependencyInstance(ILogger *logger, IService *) {
        _logger = logger;
    }

    void removeDependencyInstance(ILogger *logger, IService *) {
        _logger = nullptr;
    }

    Generator<bool> handleEvent(Custom1Event const * const evt) {
        u64 period = evt->period;
        u64 check = get_time_us();

        int speed = check - period;
        std::cout << speed << "\n";
        
        
        getManager()->pushEvent<QuitEvent>(getServiceId(), INTERNAL_EVENT_PRIORITY+1);
        
        
        co_return (bool)PreventOthersHandling;
    }

private:
    typedef unsigned long long u64;
    ILogger *_logger{nullptr};
    std::unique_ptr<EventHandlerRegistration, Deleter> _customEventHandler{nullptr};
    uint64_t _timerTriggerCount{0};
    Timer* _timerManager{nullptr};
     
    // geeft micro terug
    static u64 get_time_us(void)
    {
        struct timespec ts;
        u64 time;

        clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
        time = ts.tv_sec * 1000000;
        time += ts.tv_nsec / 1000;

        return time;
    }

};