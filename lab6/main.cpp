#include "server.hpp"
#include "BTree.hpp"
#include <algorithm>
#include <csignal>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <zmqpp/zmqpp.hpp>

int main()
{
    zmqpp::context context;
    zmqpp::socket mainSocket(context, zmqpp::socket_type::req);

    int port = bindSocket(mainSocket);

    BTree tree;

    int childPid = 0;
    int childId = 0;
    int createNodeId;

    int id;
    char excmd;
    std::string word;
    int val;

    std::string sendingMsg;
    std::string receivedMsg;

    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "create") {
            std::cin >> createNodeId;
            if (childPid == 0) {
                childPid = fork();
                if (childPid == -1) {
                    std::cout << "Error: fork fails\n";
                    exit(1);
                }
                else if (childPid == 0) {
                    createNode(createNodeId, port);
                }
                else {
                    childId = createNodeId;
                    sendMessage(mainSocket, "pid");
                    receivedMsg = receiveMessage(mainSocket);
                }
            }
            else {
                sendMessage(mainSocket, "create " + std::to_string(createNodeId));
                receivedMsg = receiveMessage(mainSocket);
            }

            if (receivedMsg.substr(0, 2) == "OK") {
                tree.Insert(createNodeId);
            }

            std::cout << receivedMsg << "\n";
        }
        else if (cmd == "remove") {
            if (childPid == 0) {
                std::cout << "Error: Not found\n";
                continue;
            }
            std::cin >> createNodeId;
            if (createNodeId == childId) {
                kill(childPid, SIGTERM);
                kill(childPid, SIGKILL);
                childId = childPid = 0;
                std::cout << "OK\n";
                tree.Delete(createNodeId);
                continue;
            }
            sendingMsg = "remove " + std::to_string(createNodeId);
            sendMessage(mainSocket, sendingMsg);
            receivedMsg = receiveMessage(mainSocket);
            if (receivedMsg.substr(0, 2) == "OK")
                tree.Delete(createNodeId);
            std::cout << receivedMsg << "\n";
        }
        else if (cmd == "exec") {
            std::cin >> id >> excmd >> word;
            if (excmd == '+')
                std::cin >> val;
            sendingMsg = "exec " + std::to_string(id);
            sendMessage(mainSocket, sendingMsg);
            receivedMsg = receiveMessage(mainSocket);
            if (receivedMsg == "Node is available") {
                if (excmd == '+') {
                    tree.DictInsert(id, word, val);
                    std::cout << "OK:" << id << std::endl;
                }
                if (excmd == '?') {
                    std::cout << "OK:" << id << ": ";
                    tree.DictSearch(id, word);
                }
            }
            else {
                std::cout << receivedMsg << std::endl;
            }
        }
        else if (cmd == "pingall") {
            std::vector<int> nodesList = tree.Serialize();
            if (nodesList.empty()) {
                std::cout << "Error: Tree is empty\n";
                continue;
            }
            std::vector<int> errors;
            for (const int &i : nodesList) {
                sendMessage(mainSocket, "exec " + std::to_string(i));
                receivedMsg = receiveMessage(mainSocket);
                if (receivedMsg.substr(0, 5) == "Error") {
                    errors.push_back(i);
                }
            }
            if (errors.empty()) {
                std::cout << "Ok: -1";
            } else {
                std::cout << "Ok: ";
                for (const int &i :errors) {
                    std::cout << i << " ";
                }
            }
            std::cout << '\n';
        }
        else if (cmd == "exit") {
            break;
        }
        else {
            std::cout << "Unknown command\n";
        }
    }
    return 0;
}