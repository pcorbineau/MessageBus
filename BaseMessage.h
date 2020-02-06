#pragma once

namespace MessageBus {

struct BaseMessage {
    virtual ~BaseMessage() = default;
};

}