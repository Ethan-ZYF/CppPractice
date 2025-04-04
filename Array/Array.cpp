
#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <utility>

template <class _Tp, size_t _N>
class Array {
   public:
    using value_type = _Tp;
    using iteraotr = _Tp*;
    using const_iterator = const _Tp*;
    using reverse_iterator = std::reverse_iterator<_Tp*>;
    using const_reverse_iterator = std::reverse_iterator<_Tp const*>;

    _Tp& operator[](size_t __i) noexcept {
        return _M_elements[__i];
    }

    const _Tp& operator[](size_t __i) const noexcept {
        return _M_elements[__i];
    }

    _Tp& at(size_t __i) {
        if (__i >= _N) [[unlikely]]
            throw std::runtime_error("out of range! __i = " + std::to_string(__i));
        return _M_elements[__i];
    }

    const _Tp& at(size_t __i) const {
        if (__i >= _N) [[unlikely]]  //.cold, not use the precious L1 cache memory
            throw std::runtime_error("out of range! __i = " + std::to_string(__i));
        return _M_elements[__i];
    }

    static constexpr size_t size() noexcept {
        // static because it not rely on any variable instance
        // constexpr because we can calculate the size during compliation
        return _N;
    }

    static constexpr size_t max_size() noexcept {
        // static because it not rely on any variable instance
        // constexpr because we can calculate the size during compliation
        return _N;
    }

    _Tp* data() noexcept {
        return _M_elements;
    }

    const _Tp* cdata() const noexcept {
        return _M_elements;
    }

    _Tp* begin() noexcept {
        return _M_elements;
    }

    const _Tp* cbegin() const noexcept {
        return _M_elements;
    }

    _Tp* end() noexcept {
        return _M_elements + _N;
    }

    const _Tp* cend() const noexcept {
        return _M_elements + _N;
    }

    _Tp* rbegin() noexcept {
        return _M_elements + _N - 1;
    }

    const _Tp* crbegin() const noexcept {
        return _M_elements + _N - 1;
    }

    _Tp* rend() noexcept {
        return _M_elements - 1;
    }

    const _Tp* rcend() const noexcept {
        return _M_elements - 1;
    }

    _Tp& front() noexcept {
        return _M_elements[0];
    }

    const _Tp& front() const noexcept {
        return _M_elements[0];
    }

    _Tp& back() noexcept {
        return _M_elements[_N - 1];
    }

    const _Tp& back() const noexcept {
        return _M_elements[_N - 1];
    }

    void fill(_Tp const& __val) noexcept(std::is_nothrow_copy_assignable_v<_Tp>) {
        for (size_t __i = 0; __i < _N; __i++) {
            _M_elements[__i] = __val;
        }
    }

    void swap(Array& __that) noexcept(std::is_nothrow_swappable_v<_Tp>) {
        for (size_t __i = 0; __i < _N; __i++) {
            std::swap(_M_elements[__i], __that._M_elements[__i]);
        }
    }

   private:
    _Tp _M_elements[_N];
};

template <class _Tp>
class Array<_Tp, 0> {
   public:
    _Tp& operator[](size_t __i) noexcept {
        std::unreachable();
    }

    const _Tp& operator[](size_t __i) const noexcept {
        std::unreachable();
    }

    _Tp& at(size_t __i) {
        throw std::runtime_error("out of range! __i = " + std::to_string(__i));
    }

    const _Tp& at(size_t __i) const {
        throw std::runtime_error("out of range! __i = " + std::to_string(__i));
    }

    static constexpr size_t size() noexcept {
        return 0;
    }

    static constexpr size_t max_size() noexcept {
        return 0;
    }

    _Tp* data() noexcept {
        return nullptr;
    }

    const _Tp* cdata() const noexcept {
        return nullptr;
    }

    _Tp* begin() noexcept {
        return nullptr;
    }

    const _Tp* cbegin() const noexcept {
        return nullptr;
    }

    _Tp* end() noexcept {
        return nullptr;
    }

    const _Tp* cend() const noexcept {
        return nullptr;
    }

    _Tp* rbegin() noexcept {
        return nullptr;
    }

    const _Tp* crbegin() const noexcept {
        return nullptr;
    }

    _Tp* rend() noexcept {
        return nullptr;
    }

    const _Tp* rcend() const noexcept {
        return nullptr;
    }

    _Tp& front() noexcept {
        std::unreachable();
    }

    const _Tp& front() const noexcept {
        std::unreachable();
    }

    _Tp& back() noexcept {
        std::unreachable();
    }

    const _Tp& back() const noexcept {
        std::unreachable();
    }

    void fill(const _Tp&) noexcept {}

    void swap(Array&) noexcept {}
};

template <class _Tp, class... _Ts>
Array(_Tp, _Ts...) -> Array<std::enable_if_t<(std::is_same_v<_Tp, _Ts> && ...), _Tp>, 1 + sizeof...(_Ts)>;

void func(Array<int, 5>& arr) {
    for (auto it = arr.begin(); it != arr.end(); ++it) {
        *it = it - arr.begin();
    }
}

int main() {
    Array<int, 5> arr{};
    for (size_t i = 0; i < arr.size(); i++) {
        std::cout << arr[i] << std::endl;
    }
    func(arr);
    for (size_t i = 0; i < arr.size(); i++) {
        std::cout << arr[i] << std::endl;
    }
    return 0;
}