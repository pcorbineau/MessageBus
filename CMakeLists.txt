cmake_minimum_required(VERSION 3.14)

project(MessageBus
  VERSION 1.0.0
  DESCRIPTION "C++ Event Message API"
  LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME}
  INTERFACE
    include/MessageBus/BaseMessage.h
    include/MessageBus/Bus.h
    include/MessageBus/Listener.h
    include/MessageBus/Publisher.h
    include/MessageBus/Util.h
    include/MessageBus/MessageHandler.h)

target_include_directories(${PROJECT_NAME} INTERFACE include)

add_executable(test test/main.cpp)
target_link_libraries(test ${PROJECT_NAME})