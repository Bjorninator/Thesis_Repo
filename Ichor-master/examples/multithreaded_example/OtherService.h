#pragma once

#include <ichor/DependencyManager.h>
#include <ichor/optional_bundles/logging_bundle/Logger.h>
#include <ichor/Service.h>
#include <ichor/LifecycleManager.h>
#include "CustomEvent.h"

using namespace Ichor;

class OtherService final : public Service<OtherService> {
public:
    OtherService(DependencyRegister &reg, Properties props, DependencyManager *mng) : Service(std::move(props), mng) {
        reg.registerDependency<ILogger>(this, true);
    }
    ~OtherService() final = default;
    bool start() final {
        ICHOR_LOG_INFO(_logger, "OtherService started with dependency");
        _customEventHandler = getManager()->registerEventHandler<CustomEvent>(this);
        // getManager()->pushEvent<QuitEvent>(getServiceId());
        return true;
    }

    bool stop() final {
        ICHOR_LOG_INFO(_logger, "OtherService stopped with dependency");
        _customEventHandler.reset();
        return true;
    }

    void addDependencyInstance(ILogger *logger, IService *isvc) {
        _logger = logger;

        ICHOR_LOG_INFO(_logger, "Inserted logger svcid {} for svcid {}", isvc->getServiceId(), getServiceId());
    }

    void removeDependencyInstance(ILogger *logger, IService *) {
        _logger = nullptr;
    }

    Generator<bool> handleEvent(CustomEvent const * const evt) {
        ICHOR_LOG_INFO(_logger, "Handling custom event");
        // std::cout <<"Otherservice: " <<getServiceId() << "\n";
        // counter++;
        // if(counter == 6){
            // getManager()->pushEvent<QuitEvent>(getServiceId());
            // getManager()->getCommunicationChannel()->broadcastEvent<QuitEvent>(getManager(), getServiceId(), INTERNAL_EVENT_PRIORITY+1);
        // }
        // we dealt with it, don't let other services handle this event
        co_return (bool)PreventOthersHandling;
    }

private:
    ILogger *_logger;
    int counter{0};
    std::unique_ptr<EventHandlerRegistration, Deleter> _customEventHandler{nullptr};
};