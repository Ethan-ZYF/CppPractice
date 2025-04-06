#ifndef SKIPLIST_H
#define SKIPLIST_H
#include <iostream>
#include <random>
#include <utility>
#include <vector>

template <typename K, typename V>
class SkipList {
   public:
    SkipList(int maxLevel) : maxLevel(maxLevel), currentLevel(0) {
        header = new Node(K(), V(), maxLevel);
        for (int i = 0; i <= maxLevel; ++i) {
            header->forward.push_back(nullptr);
        }
        generator.seed(std::random_device()());
        sz = 0;
    }

    ~SkipList() {
        Node* current = header;
        while (current) {
            Node* next = current->forward[0];
            delete current;
            current = next;
        }
    }

    void insert(K key, V value) {
        std::vector<Node*> update(maxLevel + 1);
        Node* current = header;

        for (int i = currentLevel; i >= 0; --i) {
            while (current->forward[i] && current->forward[i]->key < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        if (current == nullptr || current->key != key) {
            int newLevel = randomLevel();
            if (newLevel > currentLevel) {
                for (int i = currentLevel + 1; i <= newLevel; ++i) {
                    update[i] = header;
                }
                currentLevel = newLevel;
            }

            Node* newNode = new Node(key, value, newLevel);
            for (int i = 0; i <= newLevel; ++i) {
                newNode->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = newNode;
            }
        } else {
            current->value = value;
        }
        sz++;
    }

    bool search(K key, V& value) {
        Node* current = header;

        for (int i = currentLevel; i >= 0; --i) {
            while (current->forward[i] && current->forward[i]->key < key) {
                current = current->forward[i];
            }
        }

        current = current->forward[0];

        if (current && current->key == key) {
            value = current->value;
            return true;
        }
        return false;
    }

    bool remove(K key) {
        std::vector<Node*> update(maxLevel + 1);
        Node* current = header;

        for (int i = currentLevel; i >= 0; --i) {
            while (current->forward[i] && current->forward[i]->key < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        if (current && current->key == key) {
            for (int i = 0; i <= currentLevel; ++i) {
                if (update[i]->forward[i] != current) break;
                update[i]->forward[i] = current->forward[i];
            }

            delete current;

            while (currentLevel > 0 && header->forward[currentLevel] == nullptr) {
                --currentLevel;
            }
            sz--;
            return true;
        }
        return false;
    }

    std::vector<std::pair<K, V>> rangeSearch(K start, K end) {
        std::vector<std::pair<K, V>> result;
        Node* current = header->forward[0];

        while (current && current->key < end) {
            if (current->key >= start) {
                result.push_back({current->key, current->value});
            }
            current = current->forward[0];
        }

        return result;
    }

    size_t size() const {
        return sz;
    }

    bool empty() const {
        return sz == 0;
    }

    int randomLevel() const{
        int level = 0;
        while (level < maxLevel && (rand() % 2) == 0) {
            level++;
        }
        return level;
    }

   private:
    struct Node {
        K key;
        V value;
        std::vector<Node*> forward;

        Node(K k, V v, int level) : key(k), value(v), forward(level + 1, nullptr) {}
    };

    Node* header;
    int maxLevel;
    int currentLevel;
    size_t sz = 0;
    std::mt19937 generator;
};

#endif /* SKIPLIST_H */
