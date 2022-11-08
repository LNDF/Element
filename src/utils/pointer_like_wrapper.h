#pragma once

#include <utility>

template<typename T>
class pointer_like_wrapper {
    public:
        using value_type = T;
        using pointer = value_type*;
        using reference = value_type&;
    private:
        value_type value;
    public:
        pointer_like_wrapper(value_type&& value) : value(std::move(value)) {}

        pointer operator->() const noexcept {
            return std::addressof(value);
        }

        reference operator*() const noexcept {
            return value;
        }

};