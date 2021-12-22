#pragma once

#include <ichor/DependencyManager.h>
#include <ichor/optional_bundles/logging_bundle/Logger.h>
#include <ichor/optional_bundles/timer_bundle/TimerService.h>
#include <ichor/Service.h>
#include <ichor/LifecycleManager.h>
#include <ichor/CommunicationChannel.h>
#include "CustomEvent.h"

using namespace Ichor;

struct IPlaceService {
};

class PlaceService final : public IPlaceService, public Service<PlaceService> {
public:
    PlaceService(DependencyRegister &reg, Properties props, DependencyManager *mng) : Service(std::move(props), mng) {
        reg.registerDependency<ILogger>(this, true);
    }
    ~PlaceService() final = default;

    bool start() final {
        ICHOR_LOG_INFO(_logger, "PlaceService started");
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
        ICHOR_LOG_INFO(_logger, "PlaceService stopped");
        return true;
    }

    void addDependencyInstance(ILogger *logger, IService *) {
        _logger = logger;
    }

    void removeDependencyInstance(ILogger *logger, IService *) {
        _logger = nullptr;
    }

    Generator<bool> handleEvent(TimerEvent const * const evt) {
        ICHOR_LOG_INFO(_logger, "Timer {} starting 'long' task", getServiceId());
        u64 period = get_time_us();
        // getManager()->getCommunicationChannel()->broadcastEvent<CustomEvent>(getManager(), getServiceId(),3, period);
        
        getManager()->getCommunicationChannel()->sendEventTo<CustomEvent>(2, getServiceId(),10, period);
        co_return (bool)PreventOthersHandling;

    }

private:
    ILogger *_logger{nullptr};
    std::unique_ptr<EventHandlerRegistration, Deleter> _timerEventRegistration{nullptr};
    uint64_t _timerTriggerCount{0};
    Timer* _timerManager{nullptr};
    typedef unsigned long long u64;

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