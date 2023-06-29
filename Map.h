//
// Created by AlexeyPeov on 26.06.2023.
//

#ifndef MAP_MAP_H
#define MAP_MAP_H

#include <set>
#include <memory>


template<typename Key, typename Value>
class Map {
private:

    class Node {
    public:
        Key m_key;
        Value m_value;
        std::unique_ptr<Node> m_left = nullptr;
        std::unique_ptr<Node> m_right = nullptr;

        template<typename K, typename V>
        Node(K &&key, V &&value) {
            m_key = std::forward<K>(key);
            m_value = std::forward<V>(value);
        }

        ~Node() = default;
    };

    std::unique_ptr<Node> m_head = nullptr;

    std::set<Key> m_keys_set;

    void copy(const Map &other) {

        if (!other.m_head) return;

        m_keys_set = other.m_keys_set;

        m_head = std::make_unique<Node>(
                other.m_head->m_key,
                other.m_head->m_value
        );


        auto populate = [](auto &self, const Node *other_node, Node *to_copy_to) {
            if (!other_node) return;

            if (other_node->m_left) {

                to_copy_to->m_left = std::make_unique<Node>(
                        other_node->m_left->m_key,
                        other_node->m_left->m_value
                );

                self(self, other_node->m_left.get(), to_copy_to->m_left.get());
            }

            if (other_node->m_right) {
                to_copy_to->m_right = std::make_unique<Node>(
                        other_node->m_right->m_key,
                        other_node->m_right->m_value);
                self(self, other_node->m_right.get(),
                     to_copy_to->m_right.get());
            }

        };

        populate(populate, other.m_head.get(), m_head.get());

    }

public:

    Map() = default;

    ~Map() = default;

    Map(const Map &other) {
        clear();
        copy(other);
    }

    Map(Map &&other) noexcept {
        m_keys_set = std::move(other.m_keys_set);
        m_head = std::move(other.m_head);
    }

    Map &operator=(const Map &other) {
        clear();
        copy(other);
    }

    Map &operator=(Map &&other) noexcept {
        m_keys_set = std::move(other.m_keys_set);
        m_head = std::move(other.m_head);
    }

    template<typename K, typename V>
    void insert(K &&key, V &&value) {

        m_keys_set.emplace(key);

        if (!m_head) {
            auto node = std::make_unique<Node>(std::forward<K>(key),
                                               std::forward<V>(value));
            m_head.reset(node.release());
            return;
        }


        Node *node = m_head.get();
        Node *parent = nullptr;

        bool is_left = false;

        while (node) {
            parent = node;

            if (key < node->m_key) {
                node = node->m_left.get();
                is_left = true;
            } else if (key > node->m_key) {
                node = node->m_right.get();
                is_left = false;
            } else {
                node->m_value = std::forward<V>(value);
                return;
            }
        }

        auto new_node = std::make_unique<Node>(
                std::forward<K>(key),
                std::forward<V>(value)
        );


        if (is_left)
            parent->m_left = std::move(new_node);
        else
            parent->m_right = std::move(new_node);

    }

    [[nodiscard]] size_t size() const {
        return m_keys_set.size();
    }

    const std::set<Key>& keys() const {
        return m_keys_set;
    }

    template<typename K>
    bool contains(K &&key) {
        return m_keys_set.count(std::forward<K>(key)) > 0;
    }

    template<typename K>
    Value *find(K &&key) {

        Node *node = m_head.get();

        while (node) {
            if (node->m_key == key) return &node->m_value;

            else if (key < node->m_key) node = node->m_left.get();
            else node = node->m_right.get();
        }

        return nullptr;
    }

    template<typename K>
    Value &operator[](K &&key) {

        Value *value = find(key);

        if (value) return *value;

        throw std::runtime_error("Key not found in Map[key]");
    }

    void print() {

        auto print = [](auto &self, auto node, auto drawing, bool is_left) {

            if (!node) return;

            std::cout << drawing;

            if (is_left) std::cout << "|--";
            else std::cout << (char) 192 << "--";

            std::cout << "k: " << node->m_key << '\n';

            if (node->m_left) {
                self(self, node->m_left.get(),
                     drawing + (is_left ? "|   " : "    "), true);
            }
            if (node->m_right) {
                self(self, node->m_right.get(),
                     drawing + (is_left ? "|   " : "    "), false);
            }

        };

        std::string drawing;

        print(print, m_head.get(), drawing, false);

    }

    void clear() {
        m_head.reset();
        m_keys_set.clear();
    }

    template<typename K>
    void remove(K &&key) {

        m_keys_set.erase(key);

        Node *node = m_head.get();
        Node *parent = nullptr;
        bool is_left = false;

        // case 1 - no child: delete
        // case 2 - one child: set parent to child
        // case 3 - 2 children: do magic

        // find node and parent of node
        while (node) {

            if (key == node->m_key) break;

            parent = node;

            if (key < node->m_key) {
                node = node->m_left.get();
                is_left = true;
            } else {
                node = node->m_right.get();
                is_left = false;
            }
        }

        if (!node) return; // key doesn't exist

        // now we have the node to remove, and it's parent

        // case 1
        if (!node->m_left && !node->m_right) {
            if (!parent) m_head.reset();
            else if (is_left) parent->m_left.reset();
            else parent->m_right.reset();

            return;
        }

        // case 2
        if (!node->m_left || !node->m_right) {

            std::unique_ptr<Node> &child = node->m_left ? node->m_left
                                                        : node->m_right;

            if (!parent) m_head = std::move(child);
            else if (is_left) parent->m_left = std::move(child);
            else parent->m_right = std::move(child);

            return;
        }

        // case 3
        Node *largest = node->m_left.get();
        Node *parent_of_largest = node;

        // go left, find the largest value
        while (largest->m_right) {
            parent_of_largest = largest;
            largest = largest->m_right.get();
        }

        node->m_key = std::move(largest->m_key);
        node->m_value = std::move(largest->m_value);


        if (parent_of_largest != node)
            parent_of_largest->m_right = std::move(largest->m_left);
        else
            parent_of_largest->m_left = std::move(largest->m_left);

    }
};


#endif //MAP_MAP_H
