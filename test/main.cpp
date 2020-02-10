#include <string>
#include <cstdio>
#include <functional>

#include <MessageBus/Bus.h>
#include <MessageBus/BaseMessage.h>
#include <MessageBus/Listener.h>
#include <MessageBus/Publisher.h>

struct  NameMessage : MessageBus::BaseMessage {
    std::string name;

    explicit NameMessage(std::string n) : name(n) {};
};

struct  DataMessage : MessageBus::BaseMessage {
    std::vector<int> data;

    explicit DataMessage(std::vector<int> d) : data(std::move(d)) {};
};

struct A : MessageBus::Publisher {
    A(MessageBus::Bus& bus) : MessageBus::Publisher(bus) {};
};

struct B : MessageBus::Listener {
    B(MessageBus::Bus& bus) : MessageBus::Listener(bus) {};

    void onNameMessage(const NameMessage& msg) {
        printf("From B : Hello your name is %s\n", msg.name.c_str());
    }

    void OnDataMessage(const DataMessage& msg) {
        printf("From B : Data : [ ");
        for (auto i : msg.data) {
            printf("%d ", i);
        }
        printf("]\n");
    }
};

struct C : MessageBus::Listener, MessageBus::Publisher {
    C(MessageBus::Bus& bus) : MessageBus::Listener(bus), MessageBus::Publisher(bus) {};

    void onNameMessage(const NameMessage& msg) {
        printf("From C : Hello your name is %s\n", msg.name.c_str());
    }
};


int main() {
    MessageBus::Bus bus;
    A a(bus);
    B b(bus);
    C c(bus);

    // one listener / one publisher
    b.on<NameMessage>(std::bind(&B::onNameMessage, &b, std::placeholders::_1));
    a.publish(NameMessage{"A"});

    //two listener / one publisher
    c.on<NameMessage>(std::bind(&C::onNameMessage, &c, std::placeholders::_1));
    a.publish(NameMessage{ "A" });

    // publish on other thread
    a.publish_on_queue(NameMessage{ "A" });

    // listerner can also be publisher if specified
    b.on<DataMessage>(std::bind(&B::OnDataMessage, &b, std::placeholders::_1));
    c.publish(DataMessage{ {1,2,3,4,9,8,7,6,5} });

    // publish on other thread
    a.publish_on_queue(NameMessage{ "A" });

    // listerner can also be publisher if specified
    b.on<DataMessage>(std::bind(&B::OnDataMessage, &b, std::placeholders::_1));
    c.publish(DataMessage{ {1,2,3,4,9,8,7,6,5} });

    // publish on other thread
    a.publish_on_queue(NameMessage{ "A" });

    // listerner can also be publisher if specified
    b.on<DataMessage>(std::bind(&B::OnDataMessage, &b, std::placeholders::_1));
    c.publish(DataMessage{ {1,2,3,4,9,8,7,6,5} });
}

//Ouput : 
//From B : Hello your name is A
//From B : Hello your name is A
//From C : Hello your name is A
//From B : Data: [1 2 3 4 9 8 7 6 5]
//From B : Hello your name is A
//From C : Hello your name is A
