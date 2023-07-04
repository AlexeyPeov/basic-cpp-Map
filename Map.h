//
// Created by AlexeyPeov on 26.06.2023.
//

#ifndef MAP_MAP_H
#define MAP_MAP_H

#include <set>
#include <memory>
#include <mutex>
#include <iostream>


template<typename Key, typename Value>
class Map {
public:

    Map() = default;

    ~Map() = default;

    Map(const Map &other) {
        clear();
        m_keys_set = other.m_keys_set;
        if (!other.m_head)
            m_head.release();

        m_head = std::make_unique<Node>(*other.m_head.get());
    }

    Map(Map &&other) noexcept {
        m_keys_set = std::move(other.m_keys_set);
        m_head = std::move(other.m_head);
    }

    Map &operator=(const Map &other) {
        clear();
        m_keys_set = other.m_keys_set;

        if (!other.m_head)
            m_head.release();

        m_head = std::make_unique<Node>(*other.m_head.get());
        return *this;
    }

    Map &operator=(Map &&other) noexcept {
        m_keys_set = std::move(other.m_keys_set);
        m_head = std::move(other.m_head);
        return *this;
    }

    template<typename K, typename V>
    void insert(K &&key, V &&value) {

        std::lock_guard<std::mutex> guard(m_mutex);

        m_keys_set.emplace(key);

        m_head = insert<K, V>(
                std::move(m_head),
                std::forward<K>(key),
                std::forward<V>(value)
        );

    }

    size_t size() const {
        std::lock_guard<std::mutex> guard(m_mutex);
        return m_keys_set.size();
    }

    const std::set<Key> &keys() {
        std::lock_guard<std::mutex> guard(m_mutex);
        return m_keys_set;
    }

    template<typename K>
    bool contains(K &&key) {
        std::lock_guard<std::mutex> guard(m_mutex);
        return m_keys_set.count(std::forward<K>(key)) > 0;
    }

    template<typename K>
    Value *find(K &&key) {
        std::lock_guard<std::mutex> guard(m_mutex);

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
        std::lock_guard<std::mutex> guard(m_mutex);

        auto print = [](auto &self, auto node, auto drawing, bool is_left) {

            if (!node) return;

            std::cout << drawing;

            if (is_left) std::cout << "|--";
            else std::cout << (char) 192 << "--";

            std::cout << node->m_key << '\n';

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
        std::lock_guard<std::mutex> guard(m_mutex);

        m_head.reset();
        m_keys_set.clear();
    }

    template<typename K>
    void remove(K &&key) {

        std::lock_guard<std::mutex> guard(m_mutex);

        m_keys_set.erase(key);

        m_head = remove<K>(std::move(m_head), std::forward<K>(key));

    }


private:
    class Node {
    public:
        Key m_key;
        Value m_value;

        uint32_t m_height = 0;

        std::unique_ptr<Node> m_left = nullptr;
        std::unique_ptr<Node> m_right = nullptr;


        template<typename K, typename V>
        Node(K &&key, V &&value, uint32_t height) {
            m_key = std::forward<K>(key);
            m_value = std::forward<V>(value);
            m_height = height;
        }

        Node() = default;

        Node(const Node &other) {

            m_key = other.m_key;
            m_value = other.m_value;
            m_height = other.m_height;

            auto populate = [](auto &self, const Node *other_node,
                               Node *to_copy_to) {
                if (!other_node) return;

                if (other_node->m_left) {

                    to_copy_to->m_left = std::make_unique<Node>(
                            other_node->m_left->m_key,
                            other_node->m_left->m_value,
                            other_node->m_left->m_height
                    );

                    self(self, other_node->m_left.get(),
                         to_copy_to->m_left.get());
                }

                if (other_node->m_right) {

                    to_copy_to->m_right = std::make_unique<Node>(
                            other_node->m_right->m_key,
                            other_node->m_right->m_value,
                            other_node->m_right->m_height
                    );

                    self(self, other_node->m_right.get(),
                         to_copy_to->m_right.get());
                }

            };

            populate(populate, &other, this);
        }

        ~Node() = default;
    };

    std::unique_ptr<Node> m_head = nullptr;

    std::set<Key> m_keys_set;

    mutable std::mutex m_mutex;

    template<typename K, typename V>
    std::unique_ptr<Node> insert(std::unique_ptr<Node> node, K &&key, V &&value) {
        if (!node) {
            node = std::make_unique<Node>(std::forward<K>(key), std::forward<V>(value), 0);
            return node;
        }

        if (key < node->m_key) {
            node->m_left = insert(std::move(node->m_left), std::forward<K>(key), std::forward<V>(value));
        } else if (key > node->m_key) {
            node->m_right = insert(std::move(node->m_right), std::forward<K>(key), std::forward<V>(value));
        } else {
            node->m_value = std::forward<V>(value);
            return node;
        }

        update_height(node);

        int balance = get_balance(node);

        if (balance > 1 && key < node->m_left->m_key)
            return right_rotate(std::move(node));

        if (balance < -1 && key > node->m_right->m_key)
            return left_rotate(std::move(node));

        if (balance > 1 && key > node->m_left->m_key) {
            node->m_left = left_rotate(std::move(node->m_left));
            return right_rotate(std::move(node));
        }

        if (balance < -1 && key < node->m_right->m_key) {
            node->m_right = right_rotate(std::move(node->m_right));
            return left_rotate(std::move(node));
        }

        return node;
    }

    template<typename K>
    std::unique_ptr<Node> remove(std::unique_ptr<Node> node, K &&key) {
        if (!node)
            return node;

        if (key < node->m_key)
            node->m_left = remove(std::move(node->m_left), std::forward<K>(key));
        else if (key > node->m_key)
            node->m_right = remove(std::move(node->m_right), std::forward<K>(key));
        else {
            if (!node->m_left || !node->m_right)
                node = std::move((node->m_left) ? node->m_left : node->m_right);
            else {
                Node* max_value_node = get_max_value_node(node->m_left.get());
                node->m_key = max_value_node->m_key;
                node->m_value = max_value_node->m_value;
                node->m_left = remove(std::move(node->m_left), std::move(max_value_node->m_key));
            }
        }

        if (!node)
            return node;

        update_height(node);

        int balance = get_balance(node);

        if (balance > 1 && get_balance(node->m_left) >= 0) {
            return right_rotate(std::move(node));
        }

        if (balance > 1 && get_balance(node->m_left) < 0) {
            node->m_left = left_rotate(std::move(node->m_left));
            return right_rotate(std::move(node));
        }

        if (balance < -1 && get_balance(node->m_right) <= 0) {
            return left_rotate(std::move(node));
        }

        if (balance < -1 && get_balance(node->m_right) > 0) {
            node->m_right = right_rotate(std::move(node->m_right));
            return left_rotate(std::move(node));
        }

        return node;
    }

    void update_height(std::unique_ptr<Node>& node) {
        int leftHeight = (node->m_left) ? node->m_left->m_height : -1;
        int rightHeight = (node->m_right) ? node->m_right->m_height : -1;

        node->m_height = std::max(leftHeight, rightHeight) + 1;
    }

    int get_balance(std::unique_ptr<Node>& node) {
        int leftHeight = (node->m_left) ? node->m_left->m_height : -1;
        int rightHeight = (node->m_right) ? node->m_right->m_height : -1;

        return leftHeight - rightHeight;
    }

    std::unique_ptr<Node> right_rotate(std::unique_ptr<Node> y) {
        auto x = std::move(y->m_left);
        y->m_left = std::move(x->m_right);

        update_height(y);

        x->m_right = std::move(y);
        update_height(x);

        return x;
    }

    std::unique_ptr<Node> left_rotate(std::unique_ptr<Node> x) {
        auto y = std::move(x->m_right);
        x->m_right = std::move(y->m_left);

        update_height(x);

        y->m_left = std::move(x);
        update_height(y);

        return y;
    }

    Node* get_max_value_node(Node* node) {
        Node* current = node;

        while (current && current->m_right) {
            current = current->m_right.get();
        }

        return current;
    }

};


#endif //MAP_MAP_H
