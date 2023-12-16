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
#include <cstdint>

#include <utils/padded_array_view.h>

namespace element { 
    template<typename I, typename N>
    struct packed_map_insert_return_type {
        I position;
        bool inserted;
        N node;
    };

    template<typename V, bool is_const>
    class packed_map_local_iterator;

    template<typename V, bool is_const>
    class packed_map_iterator {
        private:
            friend class packed_map_local_iterator<V, is_const>;
            
            using container_type = std::conditional_t<is_const, const V, V>;
            using node_type = std::conditional_t<is_const,const typename container_type::value_type, typename container_type::value_type>;

            static constexpr std::size_t local_end = std::numeric_limits<std::size_t>::max();

            friend class packed_map_iterator<V, !is_const>;

            node_type* curr;
        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = decltype(std::declval<node_type>().data);
            using difference_type = std::ptrdiff_t;
            using pointer = std::conditional_t<is_const, const value_type*, value_type*>;
            using reference = std::conditional_t<is_const, const value_type&, value_type&>;

            packed_map_iterator(node_type* curr) : curr(curr) {}
            
            template<bool C = is_const>
            packed_map_iterator(const std::enable_if_t<C && C == is_const, packed_map_iterator<V, false>>& other) : curr(other.curr) {}

            explicit packed_map_iterator(packed_map_local_iterator<V, is_const> other) : curr(other.off == local_end ? (other.cont->data() + other.cont->size()) : (other.cont->data() + other.off)) {}

            reference operator*() const noexcept {
                return this->curr->data;
            }

            pointer operator->() const noexcept {
                return &this->curr->data;
            }

            friend bool operator==(const packed_map_iterator& x, const packed_map_iterator& y) noexcept {
                return x.curr == y.curr;
            }

            friend bool operator!=(const packed_map_iterator& x, const packed_map_iterator& y) noexcept {
                return x.curr != y.curr;
            }

            packed_map_iterator& operator++() noexcept {
                curr++;
                return *this;
            }

            packed_map_iterator operator++(int) noexcept {
                packed_map_iterator tmp(*this);
                curr++;
                return tmp;
            }

            packed_map_iterator& operator--() noexcept {
                curr--;
                return *this;
            }

            packed_map_iterator operator--(int) noexcept {
                packed_map_iterator tmp(*this);
                curr--;
                return tmp;
            }

            packed_map_iterator& operator+=(difference_type diff) noexcept {
                curr += diff;
                return *this;
            }

            packed_map_iterator operator+(difference_type diff) noexcept {
                packed_map_iterator tmp(curr);
                return tmp += diff;
            }

            packed_map_iterator& operator-=(difference_type diff) noexcept {
                curr -= diff;
                return *this;
            }

            packed_map_iterator operator-(difference_type diff) noexcept {
                packed_map_iterator tmp(curr);
                return tmp -= diff;
            }

            difference_type operator-(const packed_map_iterator& other) {
                return curr - other.curr;
            }
    };

    template<typename V, bool is_const>
    class packed_map_local_iterator {
        private:
            friend class packed_map_iterator<V, is_const>;

            using container_type = std::conditional_t<is_const, const V, V>;
            using node_type = std::conditional_t<is_const, const typename container_type::value_type, typename container_type::value_type>;

            static constexpr std::size_t local_end = std::numeric_limits<std::size_t>::max();

            friend class packed_map_local_iterator<V, !is_const>;

            std::size_t off;
            container_type* cont;
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = decltype(std::declval<node_type>().data);
            using difference_type = std::ptrdiff_t;
            using pointer = std::conditional_t<is_const, const value_type*, value_type*>;
            using reference = std::conditional_t<is_const, const value_type&, value_type&>;

            packed_map_local_iterator(std::size_t off, container_type* cont) : off(off), cont(cont) {}
            
            template<bool C = is_const>
            packed_map_local_iterator(const std::enable_if_t<C && C == is_const, packed_map_local_iterator<V, false>>& other) : off(other.off), cont(other.cont) {}

            explicit packed_map_local_iterator(packed_map_iterator<V, is_const> other, container_type* cont) : off(other.curr == (cont->data() + cont->size()) ? local_end : (other.curr - cont->data())), cont(cont) {}
            
            reference operator*() const noexcept {
                return (cont->data() + off)->data;
            }

            pointer operator->() const noexcept {
                return &(cont->data() + off)->data;
            }

            friend bool operator==(const packed_map_local_iterator& x, const packed_map_local_iterator& y) noexcept {
                return x.off == y.off;
            }

            friend bool operator!=(const packed_map_local_iterator& x, const packed_map_local_iterator& y) noexcept {
                return x.off != y.off;
            }

            packed_map_local_iterator& operator++()  noexcept {
                off = (*cont)[off].next;
                return *this;
            }

            packed_map_local_iterator operator++(int) noexcept {
                packed_map_local_iterator tmp(*this);
                off = (*cont)[off].next;
                return tmp;
            }
    };

    template<typename K, typename V>
    struct packed_map_node {
        using value_type = std::pair<K, V>;
        
        value_type data;
        std::size_t next;

        packed_map_node() = default;

        template<typename... Args>
        packed_map_node(std::size_t next, Args&&... args) : data(std::forward<Args>(args)...), next(next) {}
    };

    template<typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<K>, typename A = std::allocator<std::pair<K, V>>>
    class packed_map {
        public:
            using key_type = K;
            using mapped_type = V;
            using value_type = std::pair<const K, V>;
            using size_type = std::size_t;
            using difference_type = std::ptrdiff_t;
            using hasher = H;
            using key_equal = E;
            using allocator_type = A;
            using reference = value_type&;
            using const_reference = const value_type&;
            using node_type = packed_map_node<K, V>;
        private:
            using traits = std::allocator_traits<allocator_type>;
            using data_pointer = traits::template rebind_traits<node_type>::pointer;
            using index_container_type = std::vector<size_type, typename traits::template rebind_alloc<size_type>>;
            using data_container_type = std::vector<node_type, typename traits::template rebind_alloc<node_type>>;
        public:
            using pointer = traits::pointer;
            using const_pointer = traits::const_pointer;
            using iterator = packed_map_iterator<data_container_type, false>;
            using const_iterator = packed_map_iterator<data_container_type, true>;
            using local_iterator = packed_map_local_iterator<data_container_type, false>;
            using const_local_iterator = packed_map_local_iterator<data_container_type, true>;
            using insert_return_type = packed_map_insert_return_type<iterator, node_type>;
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
                size_type b = bucket(data[pos].data.first);
                if (pos == index[b]) index[b] = data[pos].next;
                if (pos != data.size() - 1) {
                    size_type* pointer = &index[bucket(data.back().data.first)];
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
                    if (eqf(b->first, key)) {
                        return b;
                    }
                }
                return e;
            }

            const_local_iterator bucket_find(const key_type& key, size_type bucket) const {
                const_local_iterator b = begin(bucket);
                const_local_iterator e = end(bucket);
                for (; b != e; b++) {
                    if (eqf(b->first, key)) {
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
            packed_map() : data{}, index(min_buckets, local_end) {}

            explicit packed_map(size_type bucket_count, const H& hash = hasher(), const E& equal = key_equal(), const A& alloc = allocator_type())
             : data{}, index(real_bucket_count(bucket_count), local_end), hashf(hash), eqf(equal), alloc(alloc) {}
            
            packed_map(size_type bucket_count, const A& alloc)
             : packed_map(bucket_count, hasher(), key_equal(), alloc) {}

            explicit packed_map(const A& alloc) : packed_map(min_buckets, alloc) {}

            template<typename I>
            packed_map(I first, I last, size_type bucket_count, const H& hash = hasher(), const E& equal = key_equal(), const A& alloc = allocator_type())
             : packed_map(bucket_count, hash, equal, alloc) {
                insert(first, last);
            }

            template<typename I>
            packed_map(I first, I last, size_type bucket_count, const A& alloc)
             : packed_map(first, last, bucket_count, hasher(), key_equal(), alloc) {}

            template<typename I>
            packed_map(I first, I last, size_type bucket_count, const H& hash, const A& alloc)
             : packed_map(first, last, bucket_count, hash, key_equal(), alloc) {}
            
            packed_map(const packed_map& other) = default;
            
            packed_map(const packed_map& other, A& alloc) noexcept (std::is_nothrow_copy_constructible<H>() && std::is_nothrow_copy_constructible<E>())
             : data(other.data), index(other.index), hashf(other.hashf), eqf(other.eqf), alloc(alloc), lfactor(other.lfactor) {}

            packed_map(packed_map&& other) noexcept = default;
            
            packed_map(packed_map&& other, A& alloc) noexcept (std::is_nothrow_move_constructible<H>() && std::is_nothrow_move_constructible<E>())
             : data(std::move(other.data)), index(std::move(other.index)), hashf(std::move(other.hashf)), eqf(std::move(other.eqf)), alloc(alloc), lfactor(other.lfactor) {}
            
            packed_map(std::initializer_list<value_type> init, size_type bucket_count = min_buckets, const H& hash = hasher(), const E& equal = key_equal(), const A& alloc = allocator_type())
             : packed_map(bucket_count, hash, equal, alloc) {
                insert(init);
             }

            packed_map(std::initializer_list<value_type> init, size_type bucket_count, const A& alloc)
             : packed_map(init, bucket_count, hasher(), key_equal(), alloc) {}
            
            packed_map(std::initializer_list<value_type> init, size_type bucket_count, const H& hash, const A& alloc)
             : packed_map(init, bucket_count, hash, key_equal(), alloc) {}

            packed_map& operator=(const packed_map& other) = default;

            packed_map& operator=(packed_map&& other) noexcept = default;

            packed_map& operator=(std::initializer_list<value_type> i) {
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
                return key_view_type((std::uint8_t*)data.data() + offsetof(node_type, data) + offsetof(value_type, first), sizeof(node_type), data.size());
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
                size_type b = bucket(new_node.data.first);
                local_iterator i = bucket_find(new_node.data.first, b);
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
                size_type b = bucket(node.data.first);
                local_iterator i = bucket_find(node.data.first, b);
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

            template<typename... Args>
            std::pair<iterator, bool> try_emplace(const key_type& key, Args&&... args) {
                size_type b = bucket(key);
                local_iterator i = bucket_find(key, b);
                if (i != end(b)) return std::make_pair(iterator(i), false);
                data.emplace_back(index[b], std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(std::forward<Args>(args)...));
                index[b] = data.size() - 1;
                maybe_grow_and_rehash();
                return std::make_pair(--end(), true);
            }

            template<typename... Args>
            std::pair<iterator, bool> try_emplace(key_type&& key, Args&... args) {
                size_type b = bucket(key);
                local_iterator i = bucket_find(key, b);
                if (i != end(b)) return std::make_pair(iterator(i), false);
                data.emplace_back(index[i], std::piecewise_construct, std::forward_as_tuple(std::move(key)), std::forward_as_tuple(std::forward<Args>(args)...));
                index[i] = data.size() - 1;
                maybe_grow_and_rehash();
                return std::make_pair(--end(), true);
            }

            template<typename... Args>
            iterator try_emplace(const_iterator pos, const key_type& key, Args&... args) {
                //forward for now
                return try_emplace(key, std::forward<Args>(args)...).first;
            }

            template<typename... Args>
            iterator try_emplace(const_iterator pos, key_type&& key, Args&... args) {
                //forward for now
                return try_emplace(std::move(key), std::forward<Args>(args)...).first;
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

            template<typename O>
            std::pair<iterator, bool> insert_or_assign(const key_type& k, O&& o) {
                auto r = try_emplace(k, std::forward<O>(o)); //TODO: hint
                if (!r.second) {
                    r.first->second = std::forward<O>(o);
                    r.second = true;
                }
                return r;
            }

            template<typename O>
            std::pair<iterator, bool> insert_or_assign(key_type&& k, O&& o) {
                auto r = try_emplace(std::move(k), std::forward<O>(o)); //TODO: hint
                if (!r.second) {
                    r.first->second = std::forward<O>(o);
                    r.second = true;
                }
                return r;
            }

            template<typename O>
            std::pair<iterator, bool> insert_or_assign(const_iterator hint, const key_type& k, O&& o) {
               //forward for now
                insert_or_assign(k, std::forward<O>(o));
            }

            template<typename O>
            std::pair<iterator, bool> insert_or_assign(const_iterator hint, key_type&& k, O&& o) {
                //forward for now
                insert_or_assign(std::move(k), std::forward<O>(o));
            }

            iterator erase(const_iterator pos) {
                const difference_type d = pos - cbegin();
                erase(pos->first);
                return begin() + d;
            }

            iterator erase(iterator pos) {
                erase(pos->first);
                return pos;
            }

            size_type erase(const key_type& key) {
                size_type f = index[bucket(key)];
                while (f != local_end && !eqf(data[f].data.first, key)) f = data[f].next;
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

            void swap(packed_map& other) noexcept(std::is_nothrow_swappable<H>() && std::is_nothrow_swappable<A>()) {
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

            mapped_type& operator[](const key_type& key) {
                iterator i = find(key);
                if (i == end()) i = emplace(std::piecewise_construct, std::forward_as_tuple(key), std::tuple<>()).first;
                return i->second;
            }

            mapped_type& operator[](key_type&& key) {
                iterator i = find(key);
                if (i == end()) i = emplace(std::piecewise_construct, std::forward_as_tuple(std::move(key)), std::tuple<>()).first;
                return i->second;
            }

            mapped_type& at(const key_type& key) {
                iterator i = find(key);
                if (i == end()) throw std::out_of_range("Key not found in map");
                return i->second;
            }

            mapped_type& at(key_type&& key) {
                iterator i = find(std::move(key));
                if (i == end()) throw std::out_of_range("Key not found in map");
                return i->second;
            }

            const mapped_type& at(const key_type& key) const {
                const_iterator i = find(key);
                if (i == end()) throw std::out_of_range("Key not found in map");
                return i->second;
            }

            const mapped_type& at(key_type&& key) const {
                const_iterator i = find(std::move(key));
                if (i == end()) throw std::out_of_range("Key not found in map");
                return i->second;
            }

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
                        size_type b = bucket(node.data.first);
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

