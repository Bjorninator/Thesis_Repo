#pragma once

#include <ichor/DependencyManager.h>
#include <ichor/optional_bundles/logging_bundle/Logger.h>
#include <ichor/optional_bundles/timer_bundle/TimerService.h>
#include <ichor/Service.h>
#include <ichor/LifecycleManager.h>
#include "CustomEvent.h"

using namespace Ichor;


struct IUsingTimerService1 : virtual public IService {
};

class UsingTimerService1 final : public IUsingTimerService1, public Service<UsingTimerService1> {
public:
    UsingTimerService1(DependencyRegister &reg, IchorProperties props, DependencyManager *mng) : Service(std::move(props), mng) {
        reg.registerDependency<ILogger>(this, true);
    }
    ~UsingTimerService1() final = default;

    bool start() final {
        _customEventHandler = getManager()->registerEventHandler<CustomEvent>(this);
        now = get_time_us();
        getManager()->pushEvent<CustomEvent>(getServiceId(), 2 , 15);
        return true;
    }

    bool stop() final {
        _timerEventRegistration.reset();
        _timerManager = nullptr;
        return true;
    }

    void addDependencyInstance(ILogger *logger) {
        _logger = logger;
    }

    void removeDependencyInstance(ILogger *logger) {
        _logger = nullptr;
    }

    Generator<bool> handleEvent(CustomEvent const * const evt) {
        next = get_time_us();

        std::cout << (next - now) << "\n";
        _timerTriggerCount++;
        for(uint32_t i = 0; i < 1; i++) {
            //simulate long task
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
            // schedule us again later in the event loop for the next iteration, don't let other handlers handle this event.
           // co_yield (bool)PreventOthersHandling;
        }
        getManager()->pushEvent<QuitEvent>(getServiceId());
        co_return (bool)PreventOthersHandling;
    }

    typedef unsigned long long u64;
    u64 now;
    u64 next;
    u64 difference;
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
    std::unique_ptr<EventHandlerRegistration, Deleter> _customEventHandler{nullptr};
};