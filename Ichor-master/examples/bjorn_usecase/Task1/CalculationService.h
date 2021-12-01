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
      //  auto start = std::chrono::steady_clock::now();
        ICHOR_LOG_INFO(_logger, "Timer {} starting other 'long' task", getServiceId());
        period = evt->period;
        now = (get_time_us() - period);
        std::cout << eventCount << ", " << (now - totalRuntime) << ", "<< evt->runtime << "\n";
        // std::cout << now + evt->runtime << "\n";
        totalRuntime+= evt->runtime;
        if(now + evt->runtime > 10000){
            missedTotal++;
            missed++;
            if(missed == 2){
                std::cout << "missed two events in a row\n";
                getManager()->pushEvent<QuitEvent>(getServiceId(), INTERNAL_EVENT_PRIORITY+1);
            }else if(missed != 1){missed = 0;}
        
            
        } 

        // std::cout <<"Overal,"<<_timerTriggerCount << "," << now << "\n";
        average += now;
        if (now < min){ min = now;}

        if (now > 0 && now > max) {max = now;}
        _timerTriggerCount++;
        eventCount++;
        if(eventCount == 2){
            std::cout << totalRuntime << "\n";
            eventCount = 0;
            totalRuntime = 0;
        } 

        if(_timerTriggerCount == 100) {
            getManager()->pushEvent<QuitEvent>(getServiceId(), INTERNAL_EVENT_PRIORITY+1);
            average = average / 100; 
            std::cout << "average: " << average  <<"\n";
            std::cout << "minimum: " << min  <<"\n";
            std::cout << "maximum: " << max  <<"\n";
            std::cout << "total missed: " << missedTotal  <<"\n";
        }
        
        // auto end = std::chrono::steady_clock::now();
        // fmt::print("Program ran for {:L} µs\n", std::chrono::duration_cast<std::chrono::microseconds>(end-start).count());
        std::this_thread::sleep_for(std::chrono::microseconds((evt->runtime - 30)));
        co_return (bool)PreventOthersHandling;
    }

private:
    typedef unsigned long long u64;
    u64 period;
    u64 now;
    u64 average{0};
    u64 min{500};
    u64 max{0};

    u64 totalRuntime{0};

    int missed{0};
    int missedTotal{0};

    ILogger *_logger{nullptr};
    std::unique_ptr<EventHandlerRegistration, Deleter> _customEventHandler{nullptr};
    uint64_t _timerTriggerCount{1};
    uint64_t eventCount{0};
    Timer* _timerManager{nullptr};
    bool startup{true};

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