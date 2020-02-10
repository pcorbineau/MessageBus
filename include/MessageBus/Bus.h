#pragma once

#include <memory>
#include <thread>
#include <queue>
#include <utility>
#include <mutex>
#include <condition_variable>

#include "BaseMessage.h"
#include "MessageHandler.h"

namespace MessageBus {

struct QueueMessage {
    std::unique_ptr<BaseMessage> message;
    std::shared_ptr<BaseHandler> handler;

    explicit QueueMessage(std::unique_ptr<BaseMessage> msg, std::shared_ptr<BaseHandler> hdlr) : message(std::move(msg)), handler(hdlr)
    {

    }
};

class Bus {
public:

    Bus();
    ~Bus();

    template <typename Message>
    void on(Callback<Message> cb);

    template <typename Message>
    void publish(const Message& msg);

    template <typename Message>
    void publish_on_queue(const Message& msg);

private:
    static std::size_t next_type() noexcept;

    template<typename>
    static std::size_t event_type() noexcept;

    template <typename Message>
    MessageHandler<Message>& handler();

    template <typename Message>
    std::shared_ptr<BaseHandler> handler_ptr();

    void thread_work();

private:
    std::vector<std::shared_ptr<BaseHandler>> _handlers;
    std::thread _event_thread;
    std::queue<QueueMessage> _message_queue;
    bool _quit_thread{ false };
    std::mutex _queue_mutex;
    std::condition_variable _queue_cv;
};

Bus::Bus() : _event_thread(&Bus::thread_work, this)
{

}

Bus::~Bus()
{
    _quit_thread = true;
    _queue_cv.notify_one();

    if (_event_thread.joinable()) {
        _event_thread.join();
    }

    while (!_message_queue.empty()) {
        auto& message = _message_queue.front();
        message.handler->publish(std::move(message.message));
        _message_queue.pop();
    }
}

void Bus::thread_work()
{
    while (!_quit_thread) {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _queue_cv.wait(lock, [&]() {return !_message_queue.empty(); });

        auto& message = _message_queue.front();
        message.handler->publish(std::move(message.message));
        _message_queue.pop();
    }

}

template <typename Message>
void Bus::on(Callback<Message> cb)
{
    handler<Message>().on(cb);
}

template <typename Message>
void Bus::publish(const Message& msg)
{
    handler<Message>().publish(msg);
}

template <typename Message>
void Bus::publish_on_queue(const Message& msg)
{
    std::lock_guard<std::mutex> lock(_queue_mutex);
    std::shared_ptr<BaseHandler> ptr = handler_ptr<Message>();
    _message_queue.emplace(std::make_unique<Message>(msg), ptr);
    _queue_cv.notify_one();
}

std::size_t Bus::next_type() noexcept {
    static std::size_t counter = 0;
    return counter++;
}

template<typename>
std::size_t Bus::event_type() noexcept {
    static std::size_t value = next_type();
    return value;
}

template <typename Message>
MessageHandler<Message>& Bus::handler() {
    std::size_t type = event_type<Message>();

    if (!(type < _handlers.size())) {
        _handlers.resize(type + 1);
    }

    if (!_handlers[type]) {
        _handlers[type] = std::make_unique<MessageHandler<Message>>();
    }

    return static_cast<MessageHandler<Message>&>(*_handlers[type]);
}

template <typename Message>
std::shared_ptr<BaseHandler> Bus::handler_ptr() {
    std::size_t type = event_type<Message>();

    if (!(type < _handlers.size())) {
        _handlers.resize(type + 1);
    }

    if (!_handlers[type]) {
        _handlers[type] = std::make_unique<MessageHandler<Message>>();
    }

    return _handlers[type];
}

}