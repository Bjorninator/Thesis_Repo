#pragma once

#include <ichor/DependencyManager.h>
#include <ichor/optional_bundles/logging_bundle/Logger.h>
#include <ichor/optional_bundles/timer_bundle/TimerService.h>
#include <ichor/Service.h>
#include <ichor/LifecycleManager.h>

using namespace Ichor;


struct IOtherTimerService {
};

class OtherTimerService final : public IOtherTimerService, public Service<OtherTimerService> {
public:
    OtherTimerService(DependencyRegister &reg, Properties props, DependencyManager *mng) : Service(std::move(props), mng) {
        reg.registerDependency<ILogger>(this, true);
    }
    ~OtherTimerService() final = default;

    bool start() final {
        ICHOR_LOG_INFO(_logger, "OtherTimerService started");
        _timerManager = getManager()->createServiceManager<Timer, ITimer>();
        _timerManager->setChronoInterval(std::chrono::milliseconds(500));
        _timerManager->setDeadlineInterval(std::chrono::milliseconds(20));
        _timerEventRegistration = getManager()->registerEventHandler<TimerEvent>(this, _timerManager->getServiceId());
        _timerManager->startTimer();
        return true;
    }

    bool stop() final {
        _timerEventRegistration.reset();
        _timerManager = nullptr;
        ICHOR_LOG_INFO(_logger, "OtherTimerService stopped");
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

        _timerTriggerCount++;
        for(uint32_t i = 0; i < 2; i++) {
            //simulate long task
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
            ICHOR_LOG_INFO(_logger, "Timer {} completed other 'long' task {} times", getServiceId(), i);
            // schedule us again later in the event loop for the next iteration, don't let other handlers handle this event.
            // co_yield (bool)PreventOthersHandling;
        }

        if(_timerTriggerCount == 7) {
            getManager()->pushEvent<QuitEvent>(getServiceId(), INTERNAL_EVENT_PRIORITY+1);
        }

        ICHOR_LOG_INFO(_logger, "Timer {} completed other 'long' task", getServiceId());
        co_return (bool)PreventOthersHandling;
    }

private:
    ILogger *_logger{nullptr};
    std::unique_ptr<EventHandlerRegistration, Deleter> _timerEventRegistration{nullptr};
    uint64_t _timerTriggerCount{0};
    Timer* _timerManager{nullptr};
};