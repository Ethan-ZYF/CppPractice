#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
using namespace std;

template <typename T, typename Compare = less<T>>
class PriorityQueue {
   public:
    // Default constructor
    PriorityQueue() : size(0), cmp(Compare()) {
        heap.push_back(T());
    }

    // Construct from an array/vector
    PriorityQueue(const vector<T>& arr) : size(0), cmp(Compare()) {
        heap.push_back(T());
        for (const T& val : arr) {
            push(val);
        }
    }

    // Construct from an array/vector with custom comparator
    PriorityQueue(const vector<T>& arr, Compare cmp) : size(0), cmp(cmp) {
        heap.push_back(T());
        for (const T& val : arr) {
            push(val);
        }
    }

    // Construct with a comparator only
    PriorityQueue(Compare cmp) : size(0), cmp(cmp) {
        heap.push_back(T());
    }

    // Insert a value into the priority queue
    void push(T val) {
        heap.push_back(val);
        size++;
        int idx = size;
        while (idx > 1 && cmp(heap[idx], heap[idx >> 1])) {
            swap(heap[idx], heap[idx >> 1]);
            idx >>= 1;
        }
    }

    // Remove the top element
    void pop() {
        if (size == 0) {
            return;
        }
        heap[1] = heap[size];
        heap.pop_back();
        size--;
        int idx = 1;
        while (idx << 1 <= size) {
            int next = idx << 1;
            if (next | 1 <= size && cmp(heap[next | 1], heap[next])) {
                next++;
            }
            if (cmp(heap[next], heap[idx])) {
                swap(heap[next], heap[idx]);
                idx = next;
            } else {
                break;
            }
        }
    }

    // Return the top element
    T top() {
        if (empty()) {
            throw std::runtime_error("PriorityQueue is empty");
        }
        return heap[1];
    }

    // Check if the priority queue is empty
    bool empty() const {
        return size == 0;
    }

   private:
    vector<T> heap;
    size_t size;
    Compare cmp;
};

int main() {
    // Example usage: Creating a min-heap (default)
    PriorityQueue<int> pq;
    pq.push(4);
    pq.push(2);
    pq.push(5);
    pq.push(1);

    while (!pq.empty()) {
        cout << pq.top() << " ";
        pq.pop();
    }
    // Output should be: 1 2 4 5

    cout << endl;

    // Example usage: Creating a max-heap
    PriorityQueue<int, greater<int>> pq2;
    pq2.push(4);
    pq2.push(2);
    pq2.push(5);
    pq2.push(1);
    pq2.push(3);

    while (!pq2.empty()) {
        cout << pq2.top() << " ";
        pq2.pop();
    }
    // Output should be: 5 4 3 2 1

    cout << endl;

    return 0;
}