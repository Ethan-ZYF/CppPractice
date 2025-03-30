#include <atomic>
#include <iostream>

// constructor
// destructor
// copy constructor
// copy assignment
// move constructor
// move assignment
// dereference, arrow operator
// reference count, pointer, reset

template <typename T>
class SharedPtr {
    T* m_ptr;
    std::atomic<size_t>* ref_cnt;

    void release() {
        if (ref_cnt && ref_cnt->fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete m_ptr;
            delete ref_cnt;
        }
    }

   public:
    SharedPtr(T* ptr_ = nullptr) : m_ptr(ptr_), ref_cnt(ptr_ ? new std::atomic<size_t>(1) : nullptr) {}

    ~SharedPtr() {
        release();
    }

    SharedPtr(const SharedPtr<T>& other) {
        m_ptr = other.m_ptr;
        ref_cnt = other.ref_cnt;
        if (ref_cnt) {
            ref_cnt->fetch_add(1, std::memory_order_relaxed);
        }
    }

    SharedPtr<T>& operator=(const SharedPtr<T>& other) {
        if (this != &other) {
            release();
            m_ptr = other.m_ptr;
            ref_cnt = other.ref_cnt;
            if (ref_cnt) {
                ref_cnt->fetch_add(1, std::memory_order_relaxed);
            }
        }
        return *this;
    }

    SharedPtr(SharedPtr<T>&& other) noexcept {
        m_ptr = other.m_ptr;
        ref_cnt = other.ref_cnt;
        other.m_ptr = nullptr;
        other.ref_cnt = nullptr;
    }

    SharedPtr<T>& operator=(SharedPtr<T>&& other) noexcept {
        if (this != &other) {
            release();
            m_ptr = other.m_ptr;
            ref_cnt = other.ref_cnt;
            other.m_ptr = nullptr;
            other.ref_cnt = nullptr;
        }
        return *this;
    }

    T& operator*() const {
        return *m_ptr;
    }

    T* operator->() const {
        return m_ptr;
    }

    T* get() const {
        return m_ptr;
    }

    size_t use_count() const {
        return ref_cnt ? ref_cnt->load(std::memory_order_acquire) : 0;
    }

    void reset(T* p = nullptr) {
        release();
        m_ptr = p;
        ref_cnt = p ? new std::atomic<size_t>(1) : nullptr;
    }
};

int main() {
    SharedPtr<int> sp1(new int(42));
    std::cout << "sp1 use_count: " << sp1.use_count() << "\n";  // Expected: 1

    SharedPtr<int> sp2 = sp1;
    std::cout << "sp1 use_count: " << sp1.use_count() << "\n";  // Expected: 2
    std::cout << "sp2 use_count: " << sp2.use_count() << "\n";  // Expected: 2

    sp2.reset(new int(100));
    std::cout << "sp1 use_count: " << sp1.use_count() << "\n";  // Expected: 1
    std::cout << "sp2 use_count: " << sp2.use_count() << "\n";  // Expected: 1
}