#pragma once

#include <vector>
#include <functional>
#include <utility>
#include <memory>
#include <stdexcept>
#include <initializer_list>
#include <unordered_map>
#include <unordered_map>

namespace engine { 
    template<typename I, typename N>
    struct packed_map_insert_return_type {
        I position;
        bool inserted;
        N node;
    };

    template<typename W>
    class packed_map_iterator {

    };

    template<typename W>
    class packed_map_local_iterator {
        
    };

    template<typename K, typename V>
    struct packed_map_node {
        std::pair<K, V> data;
        packed_map_node* next;

        template<typename... Args>
        packed_map_node(packed_map_node* next, Args&&... args) : data(std::forward<Args>(args)...), next(next) {}
    };

    template<typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<K>, typename A = std::allocator<packed_map_node<K, V>>>
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
            using index_container_type = std::vector<data_pointer, typename traits::template rebind_alloc<data_pointer>>;
            using data_container_type = std::vector<node_type, typename traits::template rebind_alloc<node_type>>;
        public:
            using pointer = traits::pointer;
            using const_pointer = traits::const_pointer;
            using iterator = packed_map_iterator<data_container_type>;
            using const_iterator = const packed_map_iterator<data_container_type>;
            using local_iterator = packed_map_local_iterator<data_container_type>;
            using const_local_iterator = const packed_map_local_iterator<data_container_type>;
            using insert_return_type = packed_map_insert_return_type<iterator, node_type>;
            static constexpr std::size_t starting_capacity = 16;
            static constexpr size_type growth = 2;
        private:
            data_container_type data;
            index_container_type index;
            hasher hashf;
            key_equal eqf;
            allocator_type alloc;
            float lfactor = 0.875f;

            void remove_data(node_type* ptr) {
                size_type b = bucket(ptr->data.first);
                if (ptr == index[b]) index[b] = ptr->next;
                if (ptr != &data.back()) {
                    node_type** pointer = &index[bucket(data.back().data.first)];
                    node_type* target = &data.back();
                    while (*pointer != target) pointer = &(*pointer)->next;
                    *pointer = ptr;
                    *ptr = std::move(data.back());
                }
                data.pop_back();
            }

            void maybe_grow_and_rehash() {
                if (size() > (max_load_factor() * bucket_count())) {
                    rehash(bucket_count() * growth);
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

        public:
            packed_map() : data{data_container_type(starting_capacity)}, index(index_container_type(starting_capacity)) {}

            explicit packed_map(size_type bucket_count, const H& hash = hasher(), const E& equal = key_equal(), const A& alloc = allocator_type())
             : data(data_container_type(bucket_count)), index(index_container_type(bucket_count)), hashf(hash), eqf(equal), alloc(alloc) {}
            
            packed_map(size_type bucket_count, const A& alloc)
             : packed_map(bucket_count, hasher(), key_equal(), alloc) {}

            explicit packed_map(const A& alloc) : packed_map(starting_capacity, alloc) {}

            template<typename I>
            packed_map(I first, I last, size_type bucket_count, const H& hash = hasher(), const E& qual = key_equal(), const A& alloc = allocator_type())
             : packed_map(bucket_count, hash, equal, alloc) {
                //TODO: import from iter
            }

            template<typename I>
            packed_map(I first, I last, size_type bucket_count, const A& alloc)
             : packed_map(first, last, bucket_count, hasher(), key_equal(), alloc) {}

            template<typename I>
            packed_map(I first, I last, size_type bucket_count, const H& hash, const &A alloc)
             : packed_map(first, last, bucket_count, hash, key_equal(), alloc) {}

            packed_map(const packed_map& other) = default;
            
            packed_map(const packed_map& other, A& alloc)
             : data(other.data), index(other.index), hashf(other.hashf), eqf(other.eqf), alloc(alloc) {}

            packed_map(packed_map&& other) = default;
            
            packed_map(packed_map&& other, A& alloc)
             : data(std::move(other.data)), index(std::move(other.index)), hashf(std::move(other.hashf)), eqf(std::move(other.eqf)), alloc(alloc) {}
            
            packed_map(std::initializer_list<value_type> init, size_type bucket_count = starting_capacity, const H& hash = hasher(), const E& equal = key_equal(), const A& alloc = allocator_type())
             : packed_map(bucket_count, hash, equal, alloc) {
                //TODO: initlist
             }

            packed_map(std::initializer_list<value_type> init, size_type bucket_count, const A& alloc)
             : packed_map(bucket_count, hasher(), key_equal() alloc) {}
            
            packed_map(std::initializer_list<value_type> init, size_type bucket_count, const H& hash, const A& alloc)
             : packed_map(bucket_count, hash, key_equal(), alloc) {}

            packed_map& operator=(const packed_map& other) = default;

            packed_map& operator=(packed_map&& other) = default;

            packed_map& operator=(std::initializer_list<value_type> i) {
                //TODO: do init list
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

            iterator begin() noexcept {
                return iterator(data.begin());
            }

            const_iterator begin() const noexcept {
                return iterator(data.begin());
            }

            const_iterator cbegin() const noexcept {
                return begin();
            }

            iterator end() noexcept {
                return iterator(data.end());
            }

            const_iterator end() const noexcept {
                return iterator(data.end());
            }

            const_iterator cend() const noexcept {
                return end();
            }

            template<typename... Args>
            std::pair<iterator, bool> emplace(Args&&... args) {
                node_type& new_node = data.emplace_back(nullptr, std::forward<Args>(args)...);
                size_type b = bucket(new_node.data.first);
                iterator i = iterator(bucket_find(new_node.data.first));
                if (i != end()) {
                    data.pop_back();
                    return std::make_pair(i, false);
                }
                new_node.next = index[b];
                index[b] = &new_node;
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
                iterator i = iterator(bucket_find(node.data.first, b));
                if (i != end()) return {i, false, std::move(node)};
                node_type& inserted = data.push_back(std::move(node));
                inserted.next = index[b];
                index[b] = &inserted;
                maybe_grow_and_rehash();
                return {--end(), true, inserted};
            }

            iterator insert(const_iterator, node_type&& node) {
                return insert(std::move(node));
            }

            template<typename... Args>
            std::pair<iterator, bool> try_emplace(const key_type& key, Args&... args) {
                size_type b = bucket(key);
                iterator i = iterator(bucket_find(key, b));
                if (i != end()) return std::make_pair(i, false);
                data.emplace_back(index[i], std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(std::forward<Args>(args)...));
                index[i] = &data.back();
                maybe_grow_and_rehash();
                return std::make_pair(--end(), true);
            }

            template<typename... Args>
            std::pair<iterator, bool> try_emplace(key_type&& key, Args&... args) {
                size_type b = bucket(key);
                iterator i = iterator(bucket_find(key, b));
                if (i != end()) return std::make_pair(i, false);
                data.emplace_back(index[i], std::piecewise_construct, std::forward_as_tuple(std::move(key)), std::forward_as_tuple(std::forward<Args>(args)...));
                index[i] = &data.back();
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
                node_type* f = index[bucket(key)];
                while (f != nullptr && !eqf(f->data.first, key)) f = f->next;
                if (f == nullptr) return 0;
                remove_data(f);
                return 1;
            }

            iterator erase(const_iterator first, const_iterator last) {
                const difference_type d = first - cbegin();
                for (; first != last; last--) erase(last);
                return begin() dist;
            }

            void clear() noexcept {
                data.clear();
                index.clear();
                //TODO: rehash???
            }

            void swap(packed_map& other) { //TODO: noexcept
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

            iterator find(cosnt key_type& key) {
                size_type b = bucket(key);
                local_iterator l = bucket_find(key, bucket);
                return iterator(l);
            }

            cosnt_iterator find(cosnt key_type& key) const {
                size_type b = bucket(key);
                const_local_iterator l = bucket_find(key, bucket);
                return iterator(l);
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
                return std::make_pair<iterator, iterator>(it, it); //maybe it + 1???
            }

            std::pair<const_iterator, const_iterator> eqal_range(const key_type& key) const {
                const_iterator it = find(key);
                return std::make_pair<iterator, iterator>(it, it); //maybe it + 1???
            }

            //TODO: eqal_range...

            mapped_type& operator[](const key_type& key) {
                return at(key);
            }

            mapped_type& operator[](key_type&& key) {
                return at(std::move(key));
            }

            mapped_type& at(const key_type& key) {
                iterator i = find(key);
                if (i == end()) return mapped_type();
                return i->second;
            }

            mapped_type& at(key_type&& key) {
                iterator i = find(std::move(key));
                if (i == end()) return mapped_type();
                return i->second;
            }

            const mapped_type& at(const key_type& key) const {
                const_iterator i = find(key);
                if (i == end()) return mapped_type();
                return i->second;
            }

            const mapped_type& at(key_type&& key) const {
                const_iterator i = find(std::move(key));
                if (i == end()) return mapped_type();
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

            size_type bucket(const key_type& key) {
                return hashf(key) % bucket_count();
            }

            local_iterator begin(size_type bucket) {
                //TODO: local iter begin
            }

            const_local_iterator begin(size_type bucket) const {
                //TODO: local iter begin
            }

            const_local_iterator cbegin(size_type bucket) const {
                return begin(bucket);
            }

            local_iterator end(size_type bucket) {
                //TODO: local iter end
            }

            const_local_iterator end(size_type bucket) const {
                //TODO: local iter end
            }

            cosnt_local_iterator cend(size_type bucket) const {
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
                //TODO: rehash???
            }

            void rehash(size_type cap) {
                //TODO: rehash
            }

            void reserve(size_type elems) {
                //TODO: reserve
            }

    };
}

