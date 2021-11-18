#pragma once

#include <ichor/DependencyManager.h>
#include <ichor/optional_bundles/logging_bundle/Logger.h>
#include <ichor/optional_bundles/timer_bundle/TimerService.h>
#include <ichor/Service.h>
#include <chrono>
#include <ichor/LifecycleManager.h>

using namespace Ichor;


struct IControlLoopService {
};

class ControlLoopService final : public IControlLoopService, public Service<ControlLoopService> {
public:
    ControlLoopService(DependencyRegister &reg, Properties props, DependencyManager *mng) : Service(std::move(props), mng) {
        reg.registerDependency<ILogger>(this, true);
    }
    ~ControlLoopService() final = default;
   
    bool start() final {
        ICHOR_LOG_INFO(_logger, "ControlLoopService started");
        _timerManager = getManager()->createServiceManager<Timer, ITimer>();
        _timerManager->setChronoInterval(std::chrono::milliseconds(10));
        _timerManager->setDeadlineInterval(std::chrono::milliseconds(10));
        _timerEventRegistration = getManager()->registerEventHandler<TimerEvent>(this, _timerManager->getServiceId());
        _timerManager->startTimer();
        return true;
    }

    bool stop() final {
        _timerEventRegistration.reset();
        _timerManager = nullptr;
        ICHOR_LOG_INFO(_logger, "ControlLoopService stopped");
        return true;
    }

    void addDependencyInstance(ILogger *logger, IService *) {
        _logger = logger;
    }

    void removeDependencyInstance(ILogger *logger, IService *) {
        _logger = nullptr;
    }

    Generator<bool> handleEvent(TimerEvent const * const evt) {
        ICHOR_LOG_INFO(_logger, "Timer {} starting other 'long' task", getServiceId());
        getManager()->pushPrioritisedEvent<Custom1Event>(getServiceId(),10 , 2932, evt->period, evt->deadline);
        getManager()->pushPrioritisedEvent<Custom1Event>(getServiceId(),10 , 4067, evt->period, evt->deadline);
        co_return (bool)PreventOthersHandling;
    }

private:
    typedef unsigned long long u64;
    ILogger *_logger{nullptr};
    std::unique_ptr<EventHandlerRegistration, Deleter> _timerEventRegistration{nullptr};
    uint64_t _timerTriggerCount{0};
    Timer* _timerManager{nullptr};

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