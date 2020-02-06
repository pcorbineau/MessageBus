#pragma once

namespace MessageBus {

class Bus;

class Publisher {
public:
    Publisher(Bus& bus);

    template <typename Message>
    void publish(const Message& msg);

    template <typename Message>
    void publish_on_queue(const Message& msg);

private:
    Bus& m_Bus;
};

Publisher::Publisher(Bus& bus) : m_Bus(bus)
{

}

template <typename Message>
void Publisher::publish(const Message& msg)
{
    m_Bus.publish(msg);
}

template <typename Message>
void Publisher::publish_on_queue(const Message& msg)
{
    m_Bus.publish_on_queue(msg);
}

}