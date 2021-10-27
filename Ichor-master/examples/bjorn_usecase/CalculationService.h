#pragma once

#include <ichor/DependencyManager.h>
#include <ichor/optional_bundles/logging_bundle/Logger.h>
#include <ichor/optional_bundles/timer_bundle/TimerService.h>
#include <ichor/Service.h>
#include <chrono>
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
         _customEventHandler = getManager()->registerEventHandler<Custom1Event>(this);
        return true;
    }

    bool stop() final {
        _customEventHandler.reset();
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

    Generator<bool> handleEvent(Custom1Event const * const evt) {
        ICHOR_LOG_INFO(_logger, "Timer {} starting other 'long' task", getServiceId());
            u64 period = evt->period;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
            std::cout << "Calculating the sensor data \n";
            ICHOR_LOG_INFO(_logger, "Timer {} completed calculation task", getServiceId());

            u64 check = get_time_us();
            // std::cout << "period: " << period << "\n";
            // std::cout << "check: " << check << "\n";
            if(period < check){
                std::cout << "did not make it\n";
            } 
            // getManager()->pushEvent<QuitEvent>(getServiceId(), INTERNAL_EVENT_PRIORITY+1);
            co_return (bool)PreventOthersHandling;
    }

private:
    typedef unsigned long long u64;
    ILogger *_logger{nullptr};
    std::unique_ptr<EventHandlerRegistration, Deleter> _customEventHandler{nullptr};
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