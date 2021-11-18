#pragma once

#include <ichor/Events.h>

namespace Ichor {
    struct ITimer {
        virtual void startTimer() = 0;
        virtual void stopTimer() = 0;
        [[nodiscard]] virtual bool running() const noexcept = 0;
        virtual void setInterval(uint64_t nanoseconds) noexcept = 0;
        virtual void setDeadline(uint64_t milliseconds) noexcept = 0;
        virtual void setPriority(uint64_t priority) noexcept = 0;
        [[nodiscard]] virtual uint64_t getPriority() const noexcept = 0;
        virtual void insertEventLoop() = 0;

        template <typename Dur>
        void setDeadlineInterval(Dur duration) noexcept {
            setDeadline(std::chrono::milliseconds(duration).count());
        }
        template <typename Dur>
        void setChronoInterval(Dur duration) noexcept {
            setInterval(std::chrono::nanoseconds(duration).count());
        }
    };
}