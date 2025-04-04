
#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <utility>

template <class _Tp, class _Alloc = std::allocator<_Tp>>
class Vector {
   public:
    using value_type = _Tp;
    using allocator_type = _Alloc;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = _Tp*;
    using const_pointer = _Tp const*;
    using reference = _Tp&;
    using const_reference = _Tp const&;
    using iterator = _Tp*;
    using const_iterator = _Tp const*;
    using reverse_iterator = std::reverse_iterator<_Tp*>;
    using const_reverse_iterator = std::reverse_iterator<_Tp const*>;

    Vector() noexcept {
        _M_data = nullptr;
        _M_size = 0;
        _M_cap = 0;
    }

    Vector(std::initializer_list<_Tp> __ilist, _Alloc const& alloc = _Alloc())
        : Vector(__ilist.begin(), __ilist.end(), alloc) {}

    explicit Vector(std::size_t __n, _Alloc const& alloc = _Alloc()) : _M_alloc(alloc) {
        _M_data = _M_alloc.allocate(__n);
        _M_cap = _M_size = __n;
        for (std::size_t __i = 0; __i != __n; __i++) {
            std::construct_at(&_M_data[__i]);  // _M_data[__i] = 0
        }
    }

    Vector(std::size_t __n, _Tp const& val, _Alloc const& alloc = _Alloc()) : _M_alloc(alloc) {
        _M_data = _M_alloc.allocate(__n);
        _M_cap = _M_size = __n;
        for (std::size_t __i = 0; __i != __n; __i++) {
            std::construct_at(&_M_data[__i], val);  // _M_data[__i] = val
        }
    }

    template <class _InputIt,
              class = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag,
                                                         typename std::iterator_traits<_InputIt>::iterator_category>>>
    Vector(_InputIt __first, _InputIt __last, _Alloc const& alloc = _Alloc()) : _M_alloc(alloc) {
        std::size_t __n = __last - __first;
        _M_data = _M_alloc.allocate(__n);
        _M_cap = _M_size = __n;
        for (std::size_t __i = 0; __i != __n; __i++) {
            std::construct_at(&_M_data[__i], *__first);
            ++__first;
        }
    }

    Vector(Vector&& __that) noexcept(
        std::is_nothrow_move_constructible<_Alloc>::value&& std::is_nothrow_move_constructible<_Tp>::value)
        : _M_alloc(std::move(__that._M_alloc)) {
        if (std::allocator_traits<_Alloc>::propagate_on_container_move_assignment::value ||
            _M_alloc == __that._M_alloc) {
            _M_data = __that._M_data;
            _M_size = __that._M_size;
            _M_cap = __that._M_cap;
            __that._M_data = nullptr;
            __that._M_size = 0;
            __that._M_cap = 0;
        } else {
            reserve(__that._M_size);
            for (auto& elem : __that) {
                emplace_back(std::move(elem));
            }
            __that.clear();
        }
    }

    Vector(Vector&& __that, _Alloc const& alloc) noexcept : _M_alloc(alloc) {
        _M_data = __that._M_data;
        _M_size = __that._M_size;
        _M_cap = __that._M_cap;
        __that._M_data = nullptr;
        __that._M_size = 0;
        __that._M_cap = 0;
    }

    Vector(Vector const& __that) : _M_alloc(__that._M_alloc) {
        _M_cap = _M_size = __that._M_size;
        if (_M_size != 0) {
            _M_data = _M_alloc.allocate(_M_size);
            for (std::size_t __i = 0; __i != _M_size; __i++) {
                std::construct_at(&_M_data[__i], std::as_const(__that._M_data[__i]));
            }
        } else {
            _M_data = nullptr;
        }
    }

    Vector(Vector const& __that, _Alloc const& alloc) : _M_alloc(alloc) {
        _M_cap = _M_size = __that._M_size;
        if (_M_size != 0) {
            _M_data = _M_alloc.allocate(_M_size);
            for (std::size_t __i = 0; __i != _M_size; __i++) {
                std::construct_at(&_M_data[__i], std::as_const(__that._M_data[__i]));
            }
        } else {
            _M_data = nullptr;
        }
    }

    ~Vector() noexcept {
        for (std::size_t __i = 0; __i != _M_size; __i++) {
            std::destroy_at(&_M_data[__i]);
        }
        if (_M_cap != 0) {
            _M_alloc.deallocate(_M_data, _M_cap);
        }
    }

    Vector& operator=(Vector const& __that) {
        if (&__that == this) [[unlikely]]
            return *this;
        reserve(__that._M_size);
        _M_size = __that._M_size;
        for (std::size_t __i = 0; __i != _M_size; __i++) {
            std::construct_at(&_M_data[__i], std::as_const(__that._M_data[__i]));
        }
        return *this;
    }

    Vector& operator=(Vector&& __that) {
        if (&__that == this) [[unlikely]]
            return *this;
        if (std::allocator_traits<_Alloc>::propagate_on_container_move_assignment::value ||
            _M_alloc == __that._M_alloc) {
            for (std::size_t __i = 0; __i != _M_size; __i++) {
                std::destroy_at(&_M_data[__i]);
            }
            if (_M_cap != 0) {
                _M_alloc.deallocate(_M_data, _M_cap);
            }
            _M_data = __that._M_data;
            _M_size = __that._M_size;
            _M_cap = __that._M_cap;
            __that._M_data = nullptr;
            __that._M_size = 0;
            __that._M_cap = 0;
        } else {
            clear();
            reserve(__that._M_size);
            for (auto& elem : __that) {
                emplace_back(std::move(elem));
            }
            __that.clear();
        }
        return *this;
    }

    Vector& operator=(std::initializer_list<_Tp> __ilist) {
        assign(__ilist.begin(), __ilist.end());
        return *this;
    }

    void assign(std::size_t __n, _Tp const& val) {
        clear();
        reserve(__n);
        _M_size = __n;
        for (std::size_t __i = 0; __i != __n; __i++) {
            std::construct_at(&_M_data[__i], val);
        }
    }

    template <class _InputIt,
              class = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag,
                                                         typename std::iterator_traits<_InputIt>::iterator_category>>>
    void assign(_InputIt __first, _InputIt __last) {
        clear();
        std::size_t __n = __last - __first;
        reserve(__n);
        _M_size = __n;
        for (std::size_t __i = 0; __i != __n; __i++) {
            std::construct_at(_M_data[__i], *__first);
            ++__first;
        }
    }

    void assign(std::initializer_list<_Tp> __ilist) {
        assign(__ilist.begin(), __ilist.end());
    }

    _Alloc get_allocator() const noexcept {
        return _M_alloc;
    }

    _Tp* begin() noexcept {
        return _M_data;
    }

    _Tp const* begin() const noexcept {
        return _M_data;
    }

    _Tp* end() noexcept {
        return _M_data + _M_size;
    }

    _Tp const* end() const noexcept {
        return _M_data + _M_size;
    }

    std::reverse_iterator<_Tp*> rbegin() noexcept {
        return std::make_reverse_iterator(_M_data + _M_size);
    }

    std::reverse_iterator<_Tp const*> rbegin() const noexcept {
        return std::make_reverse_iterator(_M_data + _M_size);
    }

    std::reverse_iterator<_Tp*> rend() noexcept {
        return std::make_reverse_iterator(_M_data);
    }

    std::reverse_iterator<_Tp const*> rend() const noexcept {
        return std::make_reverse_iterator(_M_data);
    }

    _Tp const* cbegin() const noexcept {
        return _M_data;
    }

    _Tp const* cend() const noexcept {
        return _M_data + _M_size;
    }

    std::reverse_iterator<_Tp const*> crbegin() const noexcept {
        return std::make_reverse_iterator(_M_data + _M_size);
    }

    std::reverse_iterator<_Tp const*> crend() const noexcept {
        return std::make_reverse_iterator(_M_data);
    }

    std::size_t size() const noexcept {
        return _M_size;
    }

    static constexpr std::size_t max_size() noexcept {
        return std::numeric_limits<std::size_t>::max() / sizeof(_Tp);
    }

    void resize(std::size_t __n) {
        if (__n < _M_size) {
            for (std::size_t __i = __n; __i != _M_size; __i++) {
                std::destroy_at(&_M_data[__i]);
            }
            _M_size = __n;
        } else if (__n > _M_size) {
            reserve(__n);
            for (std::size_t __i = _M_size; __i != __n; __i++) {
                std::construct_at(&_M_data[__i]);  // _M_data[__i] = 0
            }
        }
        _M_size = __n;
    }

    void resize(std::size_t __n, _Tp const& val) {
        if (__n < _M_size) {
            for (std::size_t __i = __n; __i != _M_size; __i++) {
                std::destroy_at(&_M_data[__i]);
            }
            _M_size = __n;
        } else if (__n > _M_size) {
            reserve(__n);
            for (std::size_t __i = _M_size; __i != __n; __i++) {
                std::construct_at(&_M_data[__i], val);  // _M_data[__i] = val
            }
        }
        _M_size = __n;
    }

    void shrink_to_fit() {
        auto __old_data = _M_data;
        auto __old_cap = _M_cap;
        _M_cap = _M_size;
        if (_M_size == 0) {
            _M_data = nullptr;
        } else {
            _M_data = _M_alloc.allocate(_M_size);
        }
        if (__old_cap != 0) [[likely]] {
            for (std::size_t __i = 0; __i != _M_size; __i++) {
                std::construct_at(&_M_data[__i],
                                  std::move_if_noexcept(__old_data[__i]));  // _M_data[__i] = std::move(__old_data[__i])
                std::destroy_at(&__old_data[__i]);
            }
            _M_alloc.deallocate(__old_data, __old_cap);
        }
    }

    std::size_t capacity() const noexcept {
        return _M_cap;
    }

    bool empty() const noexcept {
        return _M_size == 0;
    }

    void reserve(std::size_t __n) {
        if (__n <= _M_cap) return;
        __n = std::max(__n, _M_cap * 2 + 1);  // Ensure exponential growth even when _M_cap is 0

        pointer __new_data = _M_alloc.allocate(__n);
        std::size_t __i = 0;
        try {
            for (; __i < _M_size; ++__i) {
                std::construct_at(__new_data + __i, std::move_if_noexcept(_M_data[__i]));
            }
        } catch (...) {
            // Cleanup new_data on failure
            for (std::size_t __j = 0; __j < __i; ++__j) {
                std::destroy_at(__new_data + __j);
            }
            _M_alloc.deallocate(__new_data, __n);
            throw;
        }

        // Destroy old elements and deallocate
        for (std::size_t __j = 0; __j < _M_size; ++__j) {
            std::destroy_at(_M_data + __j);
        }
        if (_M_cap != 0) {
            _M_alloc.deallocate(_M_data, _M_cap);
        }

        _M_data = __new_data;
        _M_cap = __n;
    }

    _Tp const& operator[](std::size_t __i) const noexcept {
        return _M_data[__i];
    }

    _Tp& operator[](std::size_t __i) noexcept {
        return _M_data[__i];
    }

    _Tp& at(std::size_t __i) {
        if (__i >= _M_size) [[unlikely]]
            throw std::out_of_range("vector::at");
        return _M_data[__i];
    }

    _Tp const& at(std::size_t __i) const {
        if (__i >= _M_size) [[unlikely]]
            throw std::out_of_range("vector::at");
        return _M_data[__i];
    }

    _Tp& front() noexcept {
        return *_M_data;
    }

    _Tp const& front() const noexcept {
        return *_M_data;
    }

    _Tp& back() noexcept {
        return _M_data[_M_size - 1];
    }

    _Tp const& back() const noexcept {
        return _M_data[_M_size - 1];
    }

    _Tp* data() noexcept {
        return _M_data;
    }

    _Tp const* data() const noexcept {
        return _M_data;
    }

    void push_back(_Tp const& val) {
        if (_M_size + 1 >= _M_cap) [[unlikely]]
            reserve(_M_size + 1);
        std::construct_at(&_M_data[_M_size], val);
        _M_size = _M_size + 1;
    }

    void push_back(_Tp&& val) {
        if (_M_size + 1 >= _M_cap) [[unlikely]]
            reserve(_M_size + 1);
        std::construct_at(&_M_data[_M_size], std::move(val));
        _M_size = _M_size + 1;
    }

    template <class... Args>
    _Tp& emplace_back(Args&&... __args) {
        if (_M_size + 1 >= _M_cap) [[unlikely]]
            reserve(_M_size + 1);
        _Tp* __p = &_M_data[_M_size];
        std::construct_at(__p, std::forward<Args>(__args)...);
        _M_size = _M_size + 1;
        return *__p;
    }

    void pop_back() noexcept {
        _M_size -= 1;
        std::destroy_at(&_M_data[_M_size]);
    }

    template <class... Args>
    _Tp* emplace(_Tp const* __it, Args&&... __args) {
        std::size_t __j = __it - _M_data;
        reserve(_M_size + 1);
        // __j ~ _M_size => __j + 1 ~ _M_size + 1
        for (std::size_t __i = _M_size; __i != __j; __i--) {
            std::construct_at(&_M_data[__i], std::move(_M_data[__i - 1]));
            std::destroy_at(&_M_data[__i - 1]);
        }
        _M_size += 1;
        std::construct_at(&_M_data[__j], std::forward<Args>(__args)...);
        return _M_data + __j;
    }

    _Tp* insert(_Tp const* __it, _Tp&& val) {
        std::size_t __j = __it - _M_data;
        reserve(_M_size + 1);
        // __j ~ _M_size => __j + 1 ~ _M_size + 1
        for (std::size_t __i = _M_size; __i != __j; __i--) {
            std::construct_at(&_M_data[__i], std::move(_M_data[__i - 1]));
            std::destroy_at(&_M_data[__i - 1]);
        }
        _M_size += 1;
        std::construct_at(&_M_data[__j], std::move(val));
        return _M_data + __j;
    }

    _Tp* insert(_Tp const* __it, _Tp const& val) {
        std::size_t __j = __it - _M_data;
        reserve(_M_size + 1);
        auto __insert_pos = _M_data + __j;
        if (_M_size > __j) {
            std::move_backward(__insert_pos, _M_data + _M_size, _M_data + _M_size + 1);
        }
        std::construct_at(__insert_pos, val);
        ++_M_size;
        return __insert_pos;
    }

    _Tp* insert(_Tp const* __it, std::size_t __n, _Tp const& val) {
        std::size_t __j = __it - _M_data;
        if (__n == 0) [[unlikely]]
            return const_cast<_Tp*>(__it);
        reserve(_M_size + __n);
        // __j ~ _M_size => __j + __n ~ _M_size + __n
        for (std::size_t __i = _M_size; __i != __j; __i--) {
            std::construct_at(&_M_data[__i + __n - 1], std::move(_M_data[__i - 1]));
            std::destroy_at(&_M_data[__i - 1]);
        }
        _M_size += __n;
        for (std::size_t __i = __j; __i != __j + __n; __i++) {
            std::construct_at(&_M_data[__i], val);
        }
        return _M_data + __j;
    }

    template <class _InputIt,
              class = std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag,
                                                         typename std::iterator_traits<_InputIt>::iterator_category>>>
    _Tp* insert(_Tp const* __it, _InputIt __first, _InputIt __last) {
        std::size_t __j = __it - _M_data;
        std::size_t __n = __last - __first;
        if (__n == 0) [[unlikely]]
            return const_cast<_Tp*>(__it);
        reserve(_M_size + __n);
        for (std::size_t __i = _M_size; __i != __j; __i--) {
            std::construct_at(&_M_data[__i + __n - 1], std::move(_M_data[__i - 1]));
            std::destroy_at(&_M_data[__i - 1]);
        }
        _M_size += __n;
        for (std::size_t __i = __j; __i != __j + __n; __i++) {
            std::construct_at(&_M_data[__i], *__first);
            ++__first;
        }
        return _M_data + __j;
    }

    _Tp* insert(_Tp const* __it, std::initializer_list<_Tp> __ilist) {
        return insert(__it, __ilist.begin(), __ilist.end());
    }

    _Tp* erase(_Tp const* __it) noexcept(std::is_nothrow_move_assignable_v<_Tp>) {
        std::size_t __i = __it - _M_data;
        for (std::size_t __j = __i + 1; __j != _M_size; __j++) {
            _M_data[__j - 1] = std::move(_M_data[__j]);
        }
        _M_size -= 1;
        std::destroy_at(&_M_data[_M_size]);
        return const_cast<_Tp*>(__it);
    }

    _Tp* erase(_Tp const* __first, _Tp const* __last) noexcept(std::is_nothrow_move_assignable_v<_Tp>) {
        std::size_t diff = __last - __first;
        for (std::size_t __j = __last - _M_data; __j != _M_size; __j++) {
            _M_data[__j - diff] = std::move(_M_data[__j]);
        }
        _M_size -= diff;
        for (std::size_t __j = _M_size; __j != _M_size + diff; __j++) {
            std::destroy_at(&_M_data[__j]);
        }
        return const_cast<_Tp*>(__first);
    }

    void swap(Vector& __that) noexcept {
        std::swap(_M_data, __that._M_data);
        std::swap(_M_size, __that._M_size);
        std::swap(_M_cap, __that._M_cap);
        std::swap(_M_alloc, __that._M_alloc);
    }

    void clear() noexcept {
        for (std::size_t __i = 0; __i != _M_size; __i++) {
            std::destroy_at(&_M_data[__i]);
        }
        _M_size = 0;
    }

    bool operator==(Vector<_Tp> const& __that) const noexcept {
        return std::equal(this->begin(), this->end(), __that.begin(), __that.end());
    }

    bool operator!=(Vector<_Tp> const& __that) const noexcept {
        return !(*this == __that);
    }

    bool operator<(Vector<_Tp> const& __that) const noexcept {
        return std::lexicographical_compare(this->begin(), this->end(), __that.begin(), __that.end());
    }

    bool operator>(Vector<_Tp> const& __that) const noexcept {
        return __that < *this;
    }

    bool operator<=(Vector<_Tp> const& __that) const noexcept {
        return !(__that < *this);
    }

    bool operator>=(Vector<_Tp> const& __that) const noexcept {
        return !(*this < __that);
    }

   private:
    _Tp* _M_data;
    std::size_t _M_size;
    std::size_t _M_cap;
    [[no_unique_address]] _Alloc _M_alloc;
};

int main() {
    Vector<int> arr;
    for (int i = 0; i < 16; i++) {  // O(n)
        printf("arr.push_back(%d)\n", i);
        arr.push_back(i);  // O(1)+
    }
    arr.insert(arr.begin() + 3, {40, 41, 42});
    for (size_t i = 0; i < arr.size(); i++) {
        printf("arr[%zd] = %d\n", i, arr[i]);
    }

    Vector<int> bar(3);
    printf("arr.size() = %zd\n", arr.size());
    printf("bar.size() = %zd\n", bar.size());
    bar = std::move(arr);
    printf("arr.size() = %zd\n", arr.size());
    printf("bar.size() = %zd\n", bar.size());
    printf("sizeof(Vector) = %zd\n", sizeof(Vector<int>));

    Vector<int> vec{1, 2, 3, 4, 5};
    printf("vec.size() = %zd\n", vec.size());
    printf("vec.capacity() = %zd\n", vec.capacity());
    vec.resize(10);
    std::iota(vec.begin(), vec.end(), 1);
    arr.resize(10);
    std::iota(arr.begin(), arr.end(), 1);
    printf("arr == vec: %s\n", arr == vec ? "true" : "false");
}