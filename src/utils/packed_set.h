#pragma once

#include <cmath>
#include <bit>
#include <vector>
#include <functional>
#include <utility>
#include <limits>
#include <algorithm>
#include <memory>
#include <type_traits>
#include <iterator>
#include <stdexcept>
#include <initializer_list>

#include <utils/padded_array_view.h>

namespace element { 
    template<typename I, typename N>
    struct packed_set_insert_return_type {
        I position;
        bool inserted;
        N node;
    };

    template<typename V, bool is_const>
    class packed_set_local_iterator;

    template<typename V, bool is_const>
    class packed_set_iterator {
        private:
            friend class packed_set_local_iterator<V, is_const>;
            
            using container_type = std::conditional_t<is_const, const V, V>;
            using node_type = std::conditional_t<is_const,const typename container_type::value_type, typename container_type::value_type>;

            static constexpr std::size_t local_end = std::numeric_limits<std::size_t>::max();

            node_type* curr;
        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = decltype(std::declval<node_type>().data);
            using difference_type = std::ptrdiff_t;
            using pointer = std::conditional_t<is_const, const value_type*, value_type*>;
            using reference = std::conditional_t<is_const, const value_type&, value_type&>;

            packed_set_iterator(node_type* curr) : curr(curr) {}

            template<bool C = is_const>
            packed_set_iterator(const std::enable_if_t<C && C == is_const, packed_set_iterator<V, false>>& other) : curr(other.curr) {}

            explicit packed_set_iterator(packed_set_local_iterator<V, is_const> other) : curr(other.off == local_end ? (other.cont->data() + other.cont->size()) : (other.cont->data() + other.off)) {}

            reference operator*() const noexcept {
                return this->curr->data;
            }

            pointer operator->() const noexcept {
                return &this->curr->data;
            }

            friend bool operator==(const packed_set_iterator& x, const packed_set_iterator& y) noexcept {
                return x.curr == y.curr;
            }

            friend bool operator!=(const packed_set_iterator& x, const packed_set_iterator& y) noexcept {
                return x.curr != y.curr;
            }

            packed_set_iterator& operator++() noexcept {
                curr++;
                return *this;
            }

            packed_set_iterator operator++(int) noexcept {
                packed_set_iterator tmp(*this);
                curr++;
                return tmp;
            }

            packed_set_iterator& operator--() noexcept {
                curr--;
                return *this;
            }

            packed_set_iterator operator--(int) noexcept {
                packed_set_iterator tmp(*this);
                curr--;
                return tmp;
            }

            packed_set_iterator& operator+=(difference_type diff) noexcept {
                curr += diff;
                return *this;
            }

            packed_set_iterator operator+(difference_type diff) noexcept {
                packed_set_iterator tmp;
                return tmp += diff;
            }

            packed_set_iterator& operator-=(difference_type diff) noexcept {
                curr -= diff;
                return *this;
            }

            packed_set_iterator operator-(difference_type diff) noexcept {
                packed_set_iterator tmp;
                return tmp -= diff;
            }
    };

    template<typename V, bool is_const>
    class packed_set_local_iterator {
        private:
            friend class packed_set_iterator<V, is_const>;

            using container_type = std::conditional_t<is_const, const V, V>;
            using node_type = std::conditional_t<is_const, const typename container_type::value_type, typename container_type::value_type>;

            static constexpr std::size_t local_end = std::numeric_limits<std::size_t>::max();

            std::size_t off;
            container_type* cont;
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = decltype(std::declval<node_type>().data);
            using difference_type = std::ptrdiff_t;
            using pointer = std::conditional_t<is_const, const value_type*, value_type*>;
            using reference = std::conditional_t<is_const, const value_type&, value_type&>;

            packed_set_local_iterator(std::size_t off, container_type* cont) : off(off), cont(cont) {}

            template<bool C = is_const>
            packed_set_local_iterator(const std::enable_if_t<C && C == is_const, packed_set_local_iterator<V, false>>& other) : off(other.off), cont(other.cont) {}

            explicit packed_set_local_iterator(packed_set_iterator<V, is_const> other, container_type* cont) : off(other.curr == (cont->data() + cont->size()) ? local_end : (other.curr - cont->data())), cont(cont) {}
            
            reference operator*() const noexcept {
                return (cont->data() + off)->data;
            }

            pointer operator->() const noexcept {
                return &(cont->data() + off)->data;
            }

            friend bool operator==(const packed_set_local_iterator& x, const packed_set_local_iterator& y) noexcept {
                return x.off == y.off;
            }

            friend bool operator!=(const packed_set_local_iterator& x, const packed_set_local_iterator& y) noexcept {
                return x.off != y.off;
            }

            packed_set_local_iterator& operator++()  noexcept {
                off = (*cont)[off].next;
                return *this;
            }

            packed_set_local_iterator operator++(int) noexcept {
                packed_set_local_iterator tmp(*this);
                off = (*cont)[off].next;
                return tmp;
            }
    };

    template<typename K>
    struct packed_set_node {
        using value_type = K;
        
        value_type data;
        std::size_t next;

        packed_set_node() = default;

        template<typename... Args>
        packed_set_node(std::size_t next, Args&&... args) : data(std::forward<Args>(args)...), next(next) {}
    };

    template<typename K, typename H = std::hash<K>, typename E = std::equal_to<K>, typename A = std::allocator<K>>
    class packed_set {
        public:
            using key_type = K;
            using value_type = K;
            using size_type = std::size_t;
            using difference_type = std::ptrdiff_t;
            using hasher = H;
            using key_equal = E;
            using allocator_type = A;
            using reference = value_type&;
            using const_reference = const value_type&;
            using node_type = packed_set_node<K>;
        private:
            using traits = std::allocator_traits<allocator_type>;
            using data_pointer = traits::template rebind_traits<node_type>::pointer;
            using index_container_type = std::vector<size_type, typename traits::template rebind_alloc<size_type>>;
            using data_container_type = std::vector<node_type, typename traits::template rebind_alloc<node_type>>;
        public:
            using pointer = traits::pointer;
            using const_pointer = traits::const_pointer;
            using iterator = packed_set_iterator<data_container_type, false>;
            using const_iterator = packed_set_iterator<data_container_type, true>;
            using local_iterator = packed_set_local_iterator<data_container_type, false>;
            using const_local_iterator = packed_set_local_iterator<data_container_type, true>;
            using insert_return_type = packed_set_insert_return_type<iterator, node_type>;
            using key_view_type = padded_array_view<key_type>;
        private:
            static constexpr std::size_t min_buckets = 16;
            static constexpr std::size_t local_end = std::numeric_limits<size_type>::max();
            data_container_type data;
            index_container_type index;
            hasher hashf;
            key_equal eqf;
            allocator_type alloc;
            float lfactor = 0.875f;

            void remove_data(size_type pos) {
                size_type b = bucket(data[pos].data);
                if (pos == index[b]) index[b] = data[pos].next;
                if (pos != data.size() - 1) {
                    size_type* pointer = &index[bucket(data.back().data)];
                    size_type target = data.size() - 1;
                    while (*pointer != target) pointer = &data[*pointer].next;
                    *pointer = pos;
                    data[pos] = std::move(data.back());
                }
                data.pop_back();
            }

            void maybe_grow_and_rehash() {
                if (size() > (max_load_factor() * bucket_count())) {
                    rehash(bucket_count() * 2);
                }
            }

            local_iterator bucket_find(const key_type& key, size_type bucket) {
                local_iterator b = begin(bucket);
                local_iterator e = end(bucket);
                for (; b != e; b++) {
                    if (eqf(*b, key)) {
                        return b;
                    }
                }
                return e;
            }

            const_local_iterator bucket_find(const key_type& key, size_type bucket) const {
                const_local_iterator b = begin(bucket);
                const_local_iterator e = end(bucket);
                for (; b != e; b++) {
                    if (eqf(*b, key)) {
                        return b;
                    }
                }
                return e;
            }

            bool bucket_contains(const key_type& key, size_type bucket) const {
                return bucket_find(key, bucket) != end();
            }

            size_type real_bucket_count(size_type count) const {
                return std::bit_ceil(std::max(std::max(count, min_buckets), (size_type)(std::ceil(size() / max_load_factor()))));
            }

        public:
            packed_set() : data{}, index(min_buckets, local_end) {}

            explicit packed_set(size_type bucket_count, const H& hash = hasher(), const E& equal = key_equal(), const A& alloc = allocator_type())
             : data{}, index(real_bucket_count(bucket_count), local_end), hashf(hash), eqf(equal), alloc(alloc) {}
            
            packed_set(size_type bucket_count, const A& alloc)
             : packed_set(bucket_count, hasher(), key_equal(), alloc) {}

            explicit packed_set(const A& alloc) : packed_set(min_buckets, alloc) {}

            template<typename I>
            packed_set(I first, I last, size_type bucket_count, const H& hash = hasher(), const E& equal = key_equal(), const A& alloc = allocator_type())
             : packed_set(bucket_count, hash, equal, alloc) {
                insert(first, last);
            }

            template<typename I>
            packed_set(I first, I last, size_type bucket_count, const A& alloc)
             : packed_set(first, last, bucket_count, hasher(), key_equal(), alloc) {}

            template<typename I>
            packed_set(I first, I last, size_type bucket_count, const H& hash, const A& alloc)
             : packed_set(first, last, bucket_count, hash, key_equal(), alloc) {}

            packed_set(const packed_set& other) = default;
            
            packed_set(const packed_set& other, A& alloc) noexcept (std::is_nothrow_copy_constructible<H>() && std::is_nothrow_copy_constructible<E>())
             : data(other.data), index(other.index), hashf(other.hashf), eqf(other.eqf), alloc(alloc), lfactor(other.lfactor) {}

            packed_set(packed_set&& other) = default;
            
            packed_set(packed_set&& other, A& alloc) noexcept (std::is_nothrow_move_constructible<H>() && std::is_nothrow_move_constructible<E>())
             : data(std::move(other.data)), index(std::move(other.index)), hashf(std::move(other.hashf)), eqf(std::move(other.eqf)), alloc(alloc), lfactor(other.lfactor) {}
            
            packed_set(std::initializer_list<value_type> init, size_type bucket_count = min_buckets, const H& hash = hasher(), const E& equal = key_equal(), const A& alloc = allocator_type())
             : packed_set(bucket_count, hash, equal, alloc) {
                insert(init);
             }

            packed_set(std::initializer_list<value_type> init, size_type bucket_count, const A& alloc)
             : packed_set(init, bucket_count, hasher(), key_equal(), alloc) {}
            
            packed_set(std::initializer_list<value_type> init, size_type bucket_count, const H& hash, const A& alloc)
             : packed_set(init, bucket_count, hash, key_equal(), alloc) {}

            packed_set& operator=(const packed_set& other) = default;

            packed_set& operator=(packed_set&& other) = default;

            packed_set& operator=(std::initializer_list<value_type> i) {
                clear();
                insert(i);
                return *this;
            }

            allocator_type get_allocator() const noexcept {
                return alloc;
            }

            [[nodiscard]] bool empty() const noexcept {
                return data.empty();
            }

            size_type size() const noexcept {
                return data.size();
            }

            size_type max_size() const noexcept {
                return data.max_size();
            }

            key_view_type key_view() {
                return key_view_type((std::uint8_t*)data.data() + offsetof(node_type, data), sizeof(node_type), data.size());
            }

            iterator begin() noexcept {
                return iterator(data.data());
            }

            const_iterator begin() const noexcept {
                return const_iterator(data.data());
            }

            const_iterator cbegin() const noexcept {
                return begin();
            }

            iterator end() noexcept {
                return iterator(data.data() + size());
            }

            const_iterator end() const noexcept {
                return const_iterator(data.data() + size());
            }

            const_iterator cend() const noexcept {
                return end();
            }

            template<typename... Args>
            std::pair<iterator, bool> emplace(Args&&... args) {
                node_type& new_node = data.emplace_back(local_end, std::forward<Args>(args)...);
                size_type b = bucket(new_node.data);
                local_iterator i = bucket_find(new_node.data, b);
                if (i != end(b)) {
                    data.pop_back();
                    return std::make_pair(iterator(i), false);
                }
                new_node.next = index[b];
                index[b] = data.size() - 1;
                maybe_grow_and_rehash();
                return std::make_pair(--end(), true);
            }

            template<typename... Args>
            iterator emplace_hint(const_iterator pos, Args&&... args) {
                //forward for now
                return emplace(std::forward<Args>(args)...).first;
            }

            node_type extract(const_iterator pos) {
                if (pos == end()) return node_type();
                return *pos;
            }

            node_type extract(const key_type& key) {
                return extract(find(key));
            }

            insert_return_type insert(node_type&& node) {
                size_type b = bucket(node.data);
                local_iterator i = bucket_find(node.data, b);
                if (i != end(b)) return {iterator(i), false, std::move(node)};
                node_type& inserted = data.push_back(std::move(node));
                inserted.next = index[b];
                index[b] = data.size() - 1;
                maybe_grow_and_rehash();
                return {--end(), true, inserted};
            }

            iterator insert(const_iterator, node_type&& node) {
                return insert(std::move(node));
            }

            std::pair<iterator, bool> insert(const value_type& val) {
                return emplace(val);
            }

            std::pair<iterator, bool> insert(value_type&& val) {
                return emplace(std::move(val));
            }

            template<typename P>
            std::enable_if_t<std::is_constructible<value_type, P&&>::value, std::pair<iterator, bool>> insert(P&& p) {
                return emplace(std::forward<P>(p));
            }

            iterator insert(const_iterator hint, const value_type& val) {
                //forward for now
                return insert(val);
            }

            iterator insert(const_iterator hint, value_type& val) {
                //forward for now
                return insert(std::move(val));
            }

            template<typename P>
            std::enable_if_t<std::is_constructible<value_type, P&&>::value, iterator> insert(const_iterator hint, P&& p) {
                //forward for now
                return insert(std::forward<P>(p));
            }

            template<typename I>
            void insert(I first, I last) {
                for (; first != last; first++) insert(*first);
            }


            void insert(std::initializer_list<value_type> i) {
                for (const value_type& v : i) insert(v);
            }

            iterator erase(const_iterator pos) const {
                const difference_type d = pos - cbegin();
                erase(*pos);
                return begin() + d;
            }

            iterator erase(iterator pos) {
                erase(*pos);
                return pos;
            }

            size_type erase(const key_type& key) {
                size_type f = index[bucket(key)];
                while (f != local_end && !eqf(data[f].data, key)) f = data[f].next;
                if (f == local_end) return 0;
                remove_data(f);
                return 1;
            }

            iterator erase(const_iterator first, const_iterator last) {
                const difference_type d = first - cbegin();
                for (; first != last; last--) erase(last);
                return begin() + d;
            }

            void clear() noexcept {
                data.clear();
                index.clear();
                rehash(0);
            }

            void swap(packed_set& other) noexcept(std::is_nothrow_swappable<H>() && std::is_nothrow_swappable<A>()) {
                data.swap(other.data);
                index.swap(other.index);
                std::swap(lfactor, other.lfactor);
            }

            //TODO: merge

            hasher hash_function() const {
                return hashf;
            }

            key_equal key_eq() const {
                return eqf;
            }

            iterator find(const key_type& key) {
                size_type b = bucket(key);
                local_iterator l = bucket_find(key, b);
                return iterator(l);
            }

            const_iterator find(const key_type& key) const {
                size_type b = bucket(key);
                const_local_iterator l = bucket_find(key, b);
                return const_iterator(l);
            }

            //TODO: find...

            size_type count(const key_type& key) const {
                return contains(key) ? 1 : 0;
            }

            //TODO: count...

            bool contains(const key_type& key) const {
                return find(key) != end();
            }

            std::pair<iterator, iterator> eqal_range(const key_type& key) {
                iterator it = find(key);
                if (it == end()) return std::make_pair<iterator, iterator>(it, it);
                return std::make_pair<iterator, iterator>(it, it + 1);
            }

            std::pair<const_iterator, const_iterator> eqal_range(const key_type& key) const {
                const_iterator it = find(key);
                if (it == end()) return std::make_pair<const_iterator, const_iterator>(it, it);
                return std::make_pair<const_iterator, const_iterator>(it, it + 1);
            }

            //TODO: eqal_range...

            size_type bucket_count() const noexcept {
                return index.size();
            }

            size_type max_bucket_count() const noexcept {
                return index.max_size();
            }

            size_type bucket_size(size_type index) const {
                const_local_iterator f = cbegin(index);
                const_local_iterator l = cend(index);
                size_type count = 0;
                for (; f != l; f++) count++;
                return count;
            }

            size_type bucket(const key_type& key) const {
                return hashf(key) % bucket_count();
            }

            local_iterator begin(size_type bucket) {
                return local_iterator(index[bucket], &data);
            }

            const_local_iterator begin(size_type bucket) const {
                return const_local_iterator(index[bucket], &data);
            }

            const_local_iterator cbegin(size_type bucket) const {
                return begin(bucket);
            }

            local_iterator end(size_type bucket) {
                return local_iterator(local_end, &data);
            }

            const_local_iterator end(size_type bucket) const {
                return const_local_iterator(local_end, &data);
            }

            const_local_iterator cend(size_type bucket) const {
                return end(bucket);
            }

            float load_factor() const noexcept {
                return size() / (float) bucket_count();
            }

            float max_load_factor() const noexcept {
                return lfactor;
            }

            void max_load_factor(float factor) {
                lfactor = factor;
                rehash(0);
            }

            void rehash(size_type c) {
                c = real_bucket_count(c);
                if (c != bucket_count()) {
                    index.resize(c);
                    std::fill(index.begin(), index.end(), local_end);
                    for (node_type& node : data) {
                        size_type b = bucket(node.data);
                        node.next = index[b];
                        index[b] = &node - data.data();
                    }
                }
            }

            void reserve(size_type elems) {
                rehash(std::ceil(elems / max_load_factor()));
            }

    };
}