#pragma once

#include <ichor/DependencyManager.h>
#include <ichor/optional_bundles/logging_bundle/Logger.h>
#include <ichor/optional_bundles/timer_bundle/TimerService.h>
#include <ichor/Service.h>
#include <chrono>
#include <ichor/LifecycleManager.h>
#include "CustomEvent.h"


using namespace Ichor;


struct ICatchService{
};

class CatchService final : public ICatchService, public Service<CatchService> {
public:
    CatchService(DependencyRegister &reg, Properties props, DependencyManager *mng) : Service(std::move(props), mng) {
        reg.registerDependency<ILogger>(this, true);
    }
    ~CatchService() final = default;

    bool start() final {
         _customEventHandler = getManager()->registerEventHandler<CustomEvent>(this);
        return true;
    }

    bool stop() final {
        _customEventHandler.reset();
        _timerManager = nullptr;
        ICHOR_LOG_INFO(_logger, "CatchService stopped");
        return true;
    }

    void addDependencyInstance(ILogger *logger, IService *) {
        _logger = logger;
    }

    void removeDependencyInstance(ILogger *logger, IService *) {
        _logger = nullptr;
    }

    Generator<bool> handleEvent(CustomEvent const * const evt) {
        period = evt->period;
        now = (get_time_us() - period);
        std::cout <<_timerTriggerCount << "," << now << "\n";
        average += now;
        if (now < min){ min = now;}

        if (now > 0 && now > max) {max = now;}
        _timerTriggerCount++;
                                
        if(_timerTriggerCount == 8000) {
            getManager()->pushEvent<QuitEvent>(getServiceId(), INTERNAL_EVENT_PRIORITY+1);
            average = average / 8000; 
            std::cout << "average: " << average  <<"\n";
            std::cout << "minimum: " << min  <<"\n";
            std::cout << "maximum: " << max  <<"\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        co_return (bool)PreventOthersHandling;
    }

private:
    typedef unsigned long long u64;
    ILogger *_logger{nullptr};
    std::unique_ptr<EventHandlerRegistration, Deleter> _customEventHandler{nullptr};
    uint64_t _timerTriggerCount{0};
    Timer* _timerManager{nullptr};
    u64 period;
    u64 now;
    u64 average{0};
    u64 min{500};
    u64 max{0};

    int deadline{0};
    int runtime{0};

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