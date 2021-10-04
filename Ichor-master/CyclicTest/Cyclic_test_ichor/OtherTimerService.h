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
        _timerManager->setDeadlineInterval(std::chrono::milliseconds(80));
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
        period = evt->period;
        now = (get_time_us() - period);
       // std::cout << now << "\n";
        average += now;
        if (now < min){ min = now;}

        if (now > 0 && now > max) {max = now;}
        _timerTriggerCount++;

        if(_timerTriggerCount == 200) {
            getManager()->pushEvent<QuitEvent>(getServiceId(), INTERNAL_EVENT_PRIORITY+1);
            average = average / 200; 
             std::cout << "average: " << average  <<"\n";
             std::cout << "minimum: " << min  <<"\n";
             std::cout << "maximum: " << max  <<"\n";
        }
        co_return (bool)PreventOthersHandling;
    }

    typedef unsigned long long u64;
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

private:
    ILogger *_logger{nullptr};
    std::unique_ptr<EventHandlerRegistration, Deleter> _timerEventRegistration{nullptr};
    uint64_t _timerTriggerCount{0};
    Timer* _timerManager{nullptr};

};