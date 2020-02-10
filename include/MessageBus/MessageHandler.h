#pragma once

#include <functional>
#include <algorithm>
#include <vector>

#include "BaseMessage.h"
#include "Util.h"

namespace MessageBus {

template <typename Message>
using Callback = std::function<void(const Message&)>;

struct BaseHandler {
    virtual ~BaseHandler() = default;
    virtual void publish(std::unique_ptr<BaseMessage> msg) = 0;
};

template <typename Message>
class MessageHandler : public BaseHandler {
public:
    void on(Callback<Message> cb);
    void publish(const Message& msg);
    virtual void publish(std::unique_ptr<BaseMessage> msg) final;

private:
    std::vector<Callback<Message>> _callbacks;
    std::vector<Message> _queue;
};

template <typename Message>
void MessageHandler<Message>::on(Callback<Message> cb)
{
    _callbacks.push_back(cb);
}

template <typename Message>
void MessageHandler<Message>::publish(const Message& msg)
{
    std::for_each(
        _callbacks.cbegin(),
        _callbacks.cend(),
        [&msg](const Callback<Message>& cb) {
            cb(msg);
        });
}

template <typename Message>
void MessageHandler<Message>::publish(std::unique_ptr<BaseMessage> msg)
{
    auto cast_msg = dynamic_unique_cast<Message>(std::move(msg));
    if (!cast_msg) {
        return;
    }

    publish(*cast_msg);
}
}