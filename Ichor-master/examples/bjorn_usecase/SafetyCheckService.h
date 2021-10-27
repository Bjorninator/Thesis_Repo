#pragma once

#include <ichor/DependencyManager.h>
#include <ichor/optional_bundles/logging_bundle/Logger.h>
#include <ichor/optional_bundles/timer_bundle/TimerService.h>
#include <ichor/Service.h>
#include <ichor/LifecycleManager.h>
#include <ichor/CommunicationChannel.h>

using namespace Ichor;

struct ISafetyCheckService {
};

class SafetyCheckService final : public ISafetyCheckService, public Service<SafetyCheckService> {
public:
    SafetyCheckService(DependencyRegister &reg, Properties props, DependencyManager *mng) : Service(std::move(props), mng) {
        reg.registerDependency<ILogger>(this, true);
    }
    ~SafetyCheckService() final = default;

    bool start() final {
        ICHOR_LOG_INFO(_logger, "SafetyCheckService started");
        _timerManager = getManager()->createServiceManager<Timer, ITimer>();
        _timerManager->setChronoInterval(std::chrono::milliseconds(500));
        _timerEventRegistration = getManager()->registerEventHandler<TimerEvent>(this, _timerManager->getServiceId());
        _timerManager->startTimer();
        return true;
    }

    bool stop() final {
        _timerEventRegistration.reset();
        _timerManager = nullptr;
        ICHOR_LOG_INFO(_logger, "SafetyCheckService stopped");
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

        _timerTriggerCount++;
        std::cout << "Making a safety check\n";
        for(uint32_t i = 0; i < 5; i++) {
            //simulate long task
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
            
            // ICHOR_LOG_INFO(_logger, "Timer {} completed 'long' task {} times", getServiceId(), i);
            // schedule us again later in the event loop for the next iteration, don't let other handlers handle this event.
        
        }

        if(_timerTriggerCount == 5){
        std::cout << "Safety check failed, quiting now\n";
        // getManager()->pushEvent<QuitEvent>(getServiceId(), INTERNAL_EVENT_PRIORITY+1);
        getManager()->getCommunicationChannel()->broadcastEvent<QuitEvent>(getManager(), getServiceId(), INTERNAL_EVENT_PRIORITY+1);
        }


        co_return (bool)PreventOthersHandling;

    }

private:
    ILogger *_logger{nullptr};
    std::unique_ptr<EventHandlerRegistration, Deleter> _timerEventRegistration{nullptr};
    uint64_t _timerTriggerCount{0};
    Timer* _timerManager{nullptr};
    uint64_t waarom{0};
};