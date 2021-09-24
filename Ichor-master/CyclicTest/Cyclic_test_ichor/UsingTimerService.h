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
        _timerManager = getManager()->createServiceManager<Timer, ITimer>();
        _timerManager->setChronoInterval(std::chrono::milliseconds(200));
        _timerManager->setDeadlineInterval(std::chrono::milliseconds(15));
        _timerEventRegistration = getManager()->registerEventHandler<TimerEvent>(this, _timerManager->getServiceId());
        _timerManager->startTimer();
        return true;
    }

    bool stop() final {
        _timerEventRegistration.reset();
        _timerManager = nullptr;
        return true;
    }

    void addDependencyInstance(ILogger *logger, IService *) {
        _logger = logger;
    }

    void removeDependencyInstance(ILogger *logger, IService *) {
        _logger = nullptr;
    }

    Generator<bool> handleEvent(TimerEvent const * const evt) {
        _timerTriggerCount++;
        for(uint32_t i = 0; i < 1; i++) {
            //simulate long task
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
            // schedule us again later in the event loop for the next iteration, don't let other handlers handle this event.
           // co_yield (bool)PreventOthersHandling;
        }
        co_return (bool)PreventOthersHandling;
    }

private:
    ILogger *_logger{nullptr};
    std::unique_ptr<EventHandlerRegistration, Deleter> _timerEventRegistration{nullptr};
    uint64_t _timerTriggerCount{0};
    Timer* _timerManager{nullptr};
};