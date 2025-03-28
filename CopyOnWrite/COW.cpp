#include <cstring>
#include <iostream>
#include <stdexcept>  // For std::out_of_range

template <typename T>
class COWArray {
   public:
    // Constructor: creates an array of size n and initializes the ref-count to 1.
    COWArray(size_t n = 0) : n_(n), data_(new T[n]), ref_cnt_(new size_t(1)) {
        memset(data_, 0, n * sizeof(T));
    }

    // Copy Constructor: shallow copies and increments the ref-count.
    COWArray(const COWArray& other) : n_(other.n_), data_(other.data_), ref_cnt_(other.ref_cnt_) {
        ++(*ref_cnt_);
        std::cout << "Copy Constructor Called\n";
    }

    // Move Constructor: transfers ownership and nulls the source.
    COWArray(COWArray&& other) noexcept : n_(other.n_), data_(other.data_), ref_cnt_(other.ref_cnt_) {
        other.data_ = nullptr;
        other.ref_cnt_ = nullptr;
        other.n_ = 0;
    }

    // Copy Assignment Operator.
    COWArray& operator=(const COWArray& other) {
        std::cout << "Copy Assignment Called\n";
        if (this != &other) {
            // Release our current resources.
            Release();

            // Shallow copy the data from the other array.
            n_ = other.n_;
            data_ = other.data_;
            ref_cnt_ = other.ref_cnt_;
            ++(*ref_cnt_);
        }
        return *this;
    }

    // Move Assignment Operator.
    COWArray& operator=(COWArray&& other) noexcept {
        if (this != &other) {
            Release();

            n_ = other.n_;
            data_ = other.data_;
            ref_cnt_ = other.ref_cnt_;

            other.data_ = nullptr;
            other.ref_cnt_ = nullptr;
            other.n_ = 0;
        }
        return *this;
    }

    // Destructor: decrements the ref-count and deallocates if it reaches zero.
    ~COWArray() {
        Release();
    }

    void Modify(size_t idx, const T& val) {
        if (idx >= n_) {
            throw std::out_of_range("Index out of range");
        }

        // If the ref-count is greater than 1, we need to perform a deep-copy.
        if (*ref_cnt_ > 1) {
            --(*ref_cnt_);
            ref_cnt_ = new size_t(1);
            T* new_data = new T[n_];
            memcpy(new_data, data_, n_ * sizeof(T));
            data_ = new_data;
        }

        data_[idx] = val;
    }

    void Print() {
        for (size_t i = 0; i < n_; ++i) {
            std::cout << data_[i] << " ";
        }
        std::cout << std::endl;
    }

    void PrintAddress() {
        std::cout << "The address of the data is " << data_ << std::endl;
    }

   private:
    // Helper function to release resources: decrements ref count then frees memory if needed.
    void Release() {
        if (ref_cnt_ != nullptr) {
            if (--(*ref_cnt_) == 0) {
                delete[] data_;
                delete ref_cnt_;
            }
            ref_cnt_ = nullptr;
            data_ = nullptr;
        }
    }

    size_t n_;         // Size of the array.
    T* data_;          // Pointer to the array data.
    size_t* ref_cnt_;  // Pointer to the shared reference counter.
};

// --- Example usage ---
int main() {
    // Create a COWArray of 5 integers.
    COWArray<int> arr(5);

    // Make a shallow copy -- both share the same underlying data.
    COWArray<int> copyArr = arr;
    arr.PrintAddress();
    copyArr.PrintAddress();
    std::cout << "Before modification:" << std::endl;
    arr.Print();
    copyArr.Print();

    arr.PrintAddress();
    copyArr.PrintAddress();

    // Modify the copy; this triggers a deep-copy (copy-on-write), leaving arr unchanged.
    copyArr.Modify(2, 42);

    std::cout << "After modification:" << std::endl;
    arr.Print();
    copyArr.Print();
    arr.PrintAddress();
    copyArr.PrintAddress();

    return 0;
}