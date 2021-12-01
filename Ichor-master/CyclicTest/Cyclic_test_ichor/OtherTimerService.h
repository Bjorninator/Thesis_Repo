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
        _timerManager->setChronoInterval(std::chrono::milliseconds(5));
        _timerManager->setDeadlineInterval(std::chrono::milliseconds(5));
        _timerEventRegistration = getManager()->registerEventHandler<TimerEvent>(this, _timerManager->getServiceId());
        _timerManager->startTimer();
        std::cout << "START LATENCY TEST, " << get_time_us() << "\n";
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
        // std::cout << "START EVENT, " << get_time_us() << "\n";
        period = evt->period;
        now = (get_time_us() - period);
        if(startup){
            startup = false;
        }
        else {
           // std::cout <<"Overal,"<<_timerTriggerCount << "," << now << "\n";
            average += now;
            if (now < min){ min = now; minCounter = _timerTriggerCount;}

            if (now > 0 && now > max) {max = now; maxCounter = _timerTriggerCount;}
            _timerTriggerCount++;
                                    
            if(_timerTriggerCount == 10000) {
                getManager()->pushEvent<QuitEvent>(getServiceId(), INTERNAL_EVENT_PRIORITY+1);
                average = average / 10000; 
                std::cout << "maxcounter: " << maxCounter << " mincounter: " << minCounter <<"\n";
                std::cout << "average: " << average  <<"\n";
                std::cout << "minimum: " << min  <<"\n";
                std::cout << "maximum: " << max  <<"\n";
            }
        }
        // std::cout << "END EVENT, " << get_time_us() << "\n";
        co_return (bool)PreventOthersHandling;
        
    }

    typedef unsigned long long u64;
    u64 period;
    u64 now;
    u64 average{0};
    u64 min{500};
    u64 max{0};
    u64 maxCounter{0};
    u64 minCounter{0};

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

private:
    ILogger *_logger{nullptr};
    std::unique_ptr<EventHandlerRegistration, Deleter> _timerEventRegistration{nullptr};
    std::unique_ptr<EventHandlerRegistration, Deleter> _customEventHandler{nullptr};
    uint64_t _timerTriggerCount{0};
    bool startup{true};
    Timer* _timerManager{nullptr};

};