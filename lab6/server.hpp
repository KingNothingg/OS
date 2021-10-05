#pragma once

#include <cstdlib>
#include <string>
#include <unistd.h>
#include <zmqpp/zmqpp.hpp>

const std::string host = "tcp://127.0.0.1:";

bool sendMessage(zmqpp::socket &socket, const std::string &message_string) {
    zmqpp::message message;
    message << message_string;
    try {
        socket.send(message);
        return true;
    } catch(...) {
        return false;
    }
}

std::string receiveMessage(zmqpp::socket &socket){
    zmqpp::message message;
    try{
        socket.receive(message);
    }
    catch (zmqpp::zmq_internal_exception& e){
        message = false;
    }
    std::string recievedMessage;
    message >> recievedMessage;
    if (recievedMessage.empty()){
        return "Error: Node is not available";
    }
    return recievedMessage;
}

int bindSocket(zmqpp::socket &socket){
    int port = 6969;
    while (true){
        try{
            socket.bind(host + std::to_string(port));
            break;
        }
        catch (...){
            port++;
        }
    }
    return port;
}

void createNode(int id, int port){
    execl("./child", "child", std::to_string(id).c_str(), std::to_string(port).c_str(), NULL);
}