//
// Created by AlexeyPeov on 26.06.2023.
//

#ifndef MAP_MAP_H
#define MAP_MAP_H

#include <stdint.h>
#include <set>
#include <memory>


template <typename Key, typename Value>
class Map {
private:

    struct Node{
        Key key;
        Value value;
        std::unique_ptr<Node> left = nullptr;
        std::unique_ptr<Node> right = nullptr;

        Node(Key key, Value value);

        ~Node();
    };

    std::unique_ptr<Node> head = nullptr;

    std::set<Key> keys_set;

public:

    Map();

    ~Map();

    Map(const Map& other);

    Map(Map&& other) noexcept;

    void insert(Key key, Value value);

    size_t size();

    std::set<Key> keys();

    void print();
};

template<typename Key, typename Value>
Map<Key, Value>::Node::~Node() {
    // если очень хочется
    // std::cout << "destr node, k: " << this->key << " v: " << this->value << '\n';
}

template<typename Key, typename Value>
Map<Key, Value>::Node::Node(Key key, Value value) {
        this->key = key;
        this->value = value;
}

template <typename Key, typename Value>
Map<Key, Value>::Map() = default;

template <typename Key, typename Value>
Map<Key, Value>::~Map(){
    // если очень хочется
    // std::cout << "destr map\n";
}

template<typename Key, typename Value>
Map<Key, Value>::Map(const Map &other) {

    if(!other.head) return;

    this->keys_set = other.keys_set;

    this->head = std::make_unique<Node>(other.head->key, other.head->value);


    auto populate = [](auto& self, const Node* other_node, Node* to_copy_to){
        if(!other_node) return;

        if(other_node->left){
            to_copy_to->left = std::make_unique<Node>(other_node->left->key, other_node->left->value);
            self(self, other_node->left.get(), to_copy_to->left.get());
        }

        if(other_node->right){
            to_copy_to->right = std::make_unique<Node>(other_node->right->key, other_node->right->value);
            self(self, other_node->right.get(), to_copy_to->right.get());
        }

    };

    populate(populate, other.head.get(), this->head.get());

}


template<typename Key, typename Value>
Map<Key, Value>::Map(Map &&other) noexcept {
    this->keys_set = std::move(other.keys_set);
    this->head = std::move(other.head);
}


template <typename Key, typename Value>
void Map<Key, Value>::insert(Key key, Value value){

    this->keys_set.insert(key);

    if(!head){
        this->head = std::make_unique<Node>(key,value);
        return;
    }


    Node* temp = this->head.get();
    Node* main = nullptr;

    bool is_left = false;

    while(temp){
        main = temp;

        if(temp->key < key){
            temp = temp->left.get();
            is_left = true;
        } else if(temp->key > key) {
            temp = temp->right.get();
            is_left = false;
        } else {
            temp->value = value;
            return;
        }
    }

    if(is_left) {
        main->left.reset(std::make_unique<Node>(key,value).release());
    } else {
        main->right.reset(std::make_unique<Node>(key,value).release());
    }

}

template <typename Key, typename Value>
void Map<Key, Value>::print(){

    auto print = [](auto& self, auto node, auto drawing, bool is_left){

        if(!node) return;

        std::cout << drawing;

        if(is_left) std::cout << "|--";
        else std::cout << (char)192 << "--";

        std::cout << "v: " << node->value << '\n';

        if(node->left){
            self(self, node->left.get(), drawing + (is_left ? "|   " : "    "), true);
        }
        if(node->right){
            self(self, node->right.get(), drawing + (is_left ? "|   " : "    "), false);
        }

    };

    std::string drawing;

    print(print, head.get(), drawing, false);

}

template <typename Key, typename Value>
size_t Map<Key, Value>::size(){
    return keys_set.size();
}

template <typename Key, typename Value>
std::set<Key> Map<Key, Value>::keys(){
    return keys_set;
}

#endif //MAP_MAP_H
