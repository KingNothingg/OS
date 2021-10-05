#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>

class BTree {
private:
    struct Node{
        int id = 0;
        explicit Node(int _id) : id(_id) {}
        Node *left = nullptr;
        Node *right = nullptr;
        std::map<std::string, int> dict;
    };
    Node *root = nullptr;

    void DeleteTree() {
        DeleteHelper(root);
    }
    void DeleteHelper(Node *node) {
        if (node == nullptr) {
            return;
        }
        DeleteHelper(node->left);
        DeleteHelper(node->right);
        delete node;
    }

public:
    BTree() = default;
    ~BTree() {
        DeleteTree();
    }
    void Insert(int id) {
        if (root == nullptr) {
            root = new Node(id);
            return;
        }
        root = Insert(root, id);
    }
    Node* Insert(Node *node, int id) {
        if(node == nullptr) {
            node = new Node(id);
            return node;
        }
        if(node->id > id) {
            node->left = Insert(node->left, id);
        }
        else if(node->id < id) {
            node->right = Insert(node->right, id);
        }
        return node;
    }
    void Delete(int id) {
        if (root == nullptr) {
            return;
        }
        root = Delete(root, id);
    }
    Node *Delete(Node *node, int id) {
        if(node == nullptr) {
            return nullptr;
        }
        if (node->id < id) {
            node->right = Delete(node->right, id);
            return node;
        }
        else if(node->id > id) {
            node->left = Delete(node->left, id);
            return node;
        }
        else if(node->id == id){
            DeleteHelper(node);
            return nullptr;
        }

    }
    bool Search(int id) {
        Node *result = Search(root, id);
        return result != nullptr;
    }

    Node *Search(Node *node, int id) {
        if(node == nullptr) {
            return nullptr;
        }
        if(node->id == id) {
            return node;
        }
        else if(node->id < id) {
            return Search(node->right, id);
        }
        else if(node->id > id) {
            return Search(node->left, id);
        } else {
            return nullptr;
        }
    }

    void DictInsert(int id, std::string word, int value) {
        Node *node = Search(root, id);
        node->dict[word] = value;
    }
    void DictSearch(int id, std::string word) {
        Node *node = Search(root, id);
        if (node->dict.find(word) == node->dict.end()) {
            std::cout << "'" << word << "'" << " not found\n";
        } else {
            std::cout << node->dict[word] << '\n';
        }
    }
    std::vector<int> Serialize() {
        std::vector<int> ids;
        SerializeHelper(root, ids);
        return ids;
    }
    void SerializeHelper(Node *node, std::vector<int> &ids) {
        if (node == nullptr) {
            return;
        }
        SerializeHelper(node->left, ids);
        ids.push_back(node->id);
        SerializeHelper(node->right, ids);
    }
};


