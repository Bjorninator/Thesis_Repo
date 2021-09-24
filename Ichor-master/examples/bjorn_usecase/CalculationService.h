#pragma once

#include <ichor/DependencyManager.h>
#include <ichor/optional_bundles/logging_bundle/Logger.h>
#include <ichor/optional_bundles/timer_bundle/TimerService.h>
#include <ichor/Service.h>
#include <ichor/LifecycleManager.h>

using namespace Ichor;


struct ICalculationService{
};

class CalculationService final : public ICalculationService, public Service<CalculationService> {
public:
    CalculationService(DependencyRegister &reg, Properties props, DependencyManager *mng) : Service(std::move(props), mng) {
        reg.registerDependency<ILogger>(this, true);
    }
    ~CalculationService() final = default;

    bool start() final {
        ICHOR_LOG_INFO(_logger, "CalculationService started");
        _timerManager = getManager()->createServiceManager<Timer, ITimer>();
        _timerManager->setChronoInterval(std::chrono::milliseconds(500));
        _timerEventRegistration = getManager()->registerEventHandler<TimerEvent>(this, _timerManager->getServiceId());
        _timerManager->startTimer();
        return true;
    }

    bool stop() final {
        _timerEventRegistration.reset();
        _timerManager = nullptr;
        ICHOR_LOG_INFO(_logger, "CalculationService stopped");
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

            int yi = 0; 
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
            ICHOR_LOG_INFO(_logger, "Timer {} completed calculation task", getServiceId());
            getManager()->pushEvent<QuitEvent>(getServiceId(), INTERNAL_EVENT_PRIORITY+1);
            co_return (bool)PreventOthersHandling;
    }

private:
    ILogger *_logger{nullptr};
    std::unique_ptr<EventHandlerRegistration, Deleter> _timerEventRegistration{nullptr};
    uint64_t _timerTriggerCount{0};
    Timer* _timerManager{nullptr};
};