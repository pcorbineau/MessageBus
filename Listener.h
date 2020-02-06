#pragma once

namespace MessageBus {

class Listener {
public:
    Listener(Bus& bus);

    template <typename Message>
    void on(Callback<Message> cb);

private:
    Bus& m_Bus;
};

Listener::Listener(Bus& bus) : m_Bus(bus)
{

}

template <typename Message>
void Listener::on(Callback<Message> cb)
{
    m_Bus.on<Message>(cb);
}

}