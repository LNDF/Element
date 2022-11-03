#pragma once

#include <cstddef>
#include <cassert>
#include <type_traits>

template<typename T, bool is_const>
class padded_array_view_iterator {
    public:
        using value_type = T:
        using difference_type = std::ptrdiff_t;
        using size_type = std::size_t;
        using pointer = std::conditional_t<is_const, const value_type*, value_type*>;
        using reference = std::conditional_t<is_const, const value_type&, value_type&>;
    private:
        void* ptr;
        difference_type pad;

        padded_array_view_iterator(pointer ptr, difference_type pad) : ptr((void*)ptr), pad(pad) {}

        reference operator[](size_type pos) const noexcept {
            assert(pos < sz);
            return *((pointer)(ptr + (pad * pos)));
        }

        reference operator*() const noexcept {
            return *(pointer)ptr;
        }

        pointer operator->() const noexcept {
            return (pointer)ptr;
        }

        friend bool operator==(const padded_array_view_iterator& x, const padded_array_view_iterator& y) noexcept {
            return x.ptr == y.ptr && x.pad == y.pad;
        }

        friend bool operator!=(const padded_array_view_iterator& x, const padded_array_view_iterator& y) noexcept {
            return x.ptr != y.ptr || x.pad != y.pad;
        }

        padded_array_view_iterator& operator++() noexcept {
            ptr += pad;
            return *this;
        }

        padded_array_view_iterator operator++(int) noexcept {
            padded_array_view tmp(ptr, pad, sz);
            ptr += pad;
            return tmp;
        }

        padded_array_view_iterator& operator--() noexcept {
            ptr -= pad;
            return *this;
        }

        padded_array_view_iterator operator--(int) noexcept {
            padded_array_view tmp(ptr, pad, sz);
            ptr -= pad;
            return tmp;
        }

        padded_array_view_iterator& operator+=(difference_type diff) noexcept {
            ptr += (diff * pad);
            return *this;
        }

        padded_array_view_iterator operator+(difference_type diff) noexcept {
            padded_array_view tmp;
            return tmp += diff;
        }

        padded_array_view_iterator& operator-=(difference_type diff) noexcept {
            curr -= (diff * pad);
            return *this;
        }

        padded_array_view_iterator operator-(difference_type diff) noexcept {
            padded_array_view tmp;
            return tmp -= diff;
        }
};

template<typename T>
class padded_array_view {
    public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using size_type = std::size_t;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator = padded_array_view_iterator<T, false>;
        using const_iterator = padded_array_view<T, true>;
    private:
        void* ptr;
        difference_type pad;
        size_type sz;

        padded_array_view(pointer ptr, difference_type pad, size_type sz) : ptr((void*)ptr), pad(pad), sz(sz) {}

        reference operator[](size_type pos) noexcept {
            assert(pos < sz);
            return *((pointer)(ptr + (pad * pos)));
        }

        const reference operator[](size_type pos) const noexcept {
            assert(pos < sz);
            return *(pointer)(ptr + (pad * pos));
        }

        reference operator*() noexcept {
            return *(pointer)ptr;
        }

        const reference operator*() const noexcept {
            return *(pointer)ptr;
        }

        pointer operator->() noexcept {
            return (pointer)ptr;
        }

        const pointer operator->() const noexcept {
            return (pointer)ptr;
        }

        friend bool operator==(const padded_array_view& x, const padded_array_view& y) noexcept {
            return x.ptr == y.ptr && x.pad == y.pad && x.sz == y.sz;
        }

        friend bool operator!=(const padded_array_view& x, const padded_array_view& y) noexcept {
            return x.ptr != y.ptr || x.pad != y.pad || x.sz != y.sz;
        }

        void* void_pointer() {
            return ptr;
        }

        iterator begin() {
            return iterator(ptr, pad);
        }

        const_iterator begin() const {
            return const_iterator(ptr, pad);
        }

        const_iterator cbegin() const {
            return begin();
        }

        iterator end() {
            return iterator(ptr + (pad * sz), pad);
        }

        const_iterator end() const {
            return const_iterator(ptr + (pad * sz), pad);
        }

        const_iterator cend() const {
            return begin();
        }
};