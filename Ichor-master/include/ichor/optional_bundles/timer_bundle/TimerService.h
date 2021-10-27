#pragma once

#include <ichor/Common.h>
#include <ichor/Service.h>
#include <ichor/DependencyManager.h>
#include <chrono>
#include <thread>
#include <sched.h>
#include <ichor/optional_bundles/timer_bundle/ITimer.h>

namespace Ichor {

    // Rather shaddy implementation, setting the interval does not reset the insertEventLoop function and the sleep_for is sketchy at best.
    class Timer final : public ITimer, public Service<Timer> {
    public:
        Timer() noexcept : _intervalNanosec(0), _eventInsertionThread(nullptr), _quit(true), _priority(INTERNAL_EVENT_PRIORITY), _intervalDeadline(0) {
        }

        ~Timer() noexcept final {
            stopTimer();
        }

        bool start() final {
            return true;
        }

        bool stop() final {
            stopTimer();
            return true;
        }

        void startTimer() final {
            bool expected = true;
            if(_quit.compare_exchange_strong(expected, false, std::memory_order_acq_rel)) {
                _eventInsertionThread = std::make_unique<std::thread>([this]() { this->insertEventLoop(); });
            }
        }

        void stopTimer() final {
            bool expected = false;
            if(_quit.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
                _eventInsertionThread->join();
                _eventInsertionThread = nullptr;
            }
        }

        bool running() const noexcept final {
            return !_quit.load(std::memory_order_acquire);
        };

        void setInterval(uint64_t nanoseconds) noexcept final {
            _intervalNanosec.store(nanoseconds, std::memory_order_release);
        }

        void setDeadline(uint64_t milliseconds) noexcept final {
            _intervalDeadline.store(milliseconds, std::memory_order_release);
        }

        void setPriority(uint64_t priority) noexcept final {
            _priority.store(priority, std::memory_order_release);
        }

        uint64_t getPriority() const noexcept final {
            return _priority.load(std::memory_order_acquire);
        }

    typedef std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> MyTimePoint;

    private:
        typedef unsigned long long u64;
        std::atomic<uint64_t> _intervalNanosec;
        std::unique_ptr<std::thread> _eventInsertionThread;
        std::atomic<bool> _quit;
        std::atomic<uint64_t> _priority;
        std::atomic<uint64_t> period;
        std::atomic<uint64_t> execution;
        std::atomic<uint64_t> runtimeStart;
        std::atomic<uint64_t> runtimeEnd;
        std::atomic<uint64_t> _intervalDeadline;
        struct sched_param sp;

        void insertEventLoop() {
            cpu_set_t lock_to_core_set;
            CPU_ZERO(&lock_to_core_set);
            CPU_SET(1, &lock_to_core_set);
            sched_setaffinity(0, sizeof(cpu_set_t), &lock_to_core_set);
            
            sp.sched_priority = sched_get_priority_min(SCHED_FIFO);
                if (sched_setscheduler(0, SCHED_FIFO, &sp) == -1)
                    perror("sched_setscheduler");

            while(!_quit.load(std::memory_order_acquire)) {
                runtimeStart = get_time_us();
                auto now = std::chrono::steady_clock::now();
                auto next = now + std::chrono::nanoseconds(_intervalNanosec.load(std::memory_order_acquire));
               // std::cout << _intervalNanosec.load(std::memory_order_acquire) / 1000 << "\n";
                period = get_time_us() + (_intervalNanosec.load(std::memory_order_acquire) / 1000);

                while(now < next && !_quit.load(std::memory_order_acquire)) {
                    std::this_thread::sleep_for(std::chrono::nanoseconds(_intervalNanosec.load(std::memory_order_acquire)/10));
                    now = std::chrono::steady_clock::now();
                    // period = get_time_us();
                }
                MyTimePoint startTimePoint = std::chrono::time_point_cast<MyTimePoint::duration>(std::chrono::steady_clock::time_point(std::chrono::steady_clock::now()));
                startTimePoint += std::chrono::milliseconds(_intervalDeadline.load(std::memory_order_acquire));

                getManager()->pushPrioritisedEvent<TimerEvent>(getServiceId(), _priority.load(std::memory_order_acquire), 10, period, startTimePoint);
                execution = (get_time_us() - period);
               // std::cout <<"release job:, ," << execution << "\n";
                next += std::chrono::nanoseconds(_intervalNanosec.load(std::memory_order_acquire));
            }
        }

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
}