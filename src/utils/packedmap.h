#pragma once

#include <vector>
#include <functional>
#include <utility>
#include <memory>
#include <stdexcept>
#include <initializer_list>
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
    class packed_map_node {

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
            static constexpr float threshhold = 0.875f;
            static constexpr std::size_t starting_capacity = 16;
        private:
            data_container_type data;
            index_container_type index;
            hasher hashf;
            key_equal eqf;
            allocator_type alloc;
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
                //TODO: function
            }

            template<typename... Args>
            iterator emplace_hint(const_iterator pos, Args&&... args) {
                //TODO: function
            }

            node_type extract(const_iterator pos) {
                //TODO: assert and get
            }

            node_type extract(const key_type& key) {
                //TODO: extract
            }

            insert_return_type insert(node_type&& node) {
                //TODO: insert
            }

            iterator insert(const_iterator, node_type&& node) {
                //TODO: insert
            }

            template<typename... Args>
            std::pair<iterator, bool> try_emplace(const key_type& key, Args&... args) {
                //TODO: try to emplace
            }

            template<typename... Args>
            std::pair<iterator, bool> try_emplace(key_type&& key, Args&... args) {
                //TODO: try to emplace
            }

            template<typename... Args>
            std::pair<iterator, bool> try_emplace(const_iterator pos, const key_type& key, Args&... args) {
                //TODO: try to emplace
            }

            template<typename... Args>
            std::pair<iterator, bool> try_emplace(const_iterator pos, key_type&& key, Args&... args) {
                //TODO: try to emplace
            }

            std::pair<iterator, bool> insert(const value_type& val) {
                //TODO: insert
            }

            std::pair<iterator, bool> insert(value_type& val) {
                //TODO: insert
            }

            template<typename P>
            std::enable_if_t<std::is_constructible<value_type, P&&>::value, std::pair<iterator, bool>> insert(P&& p) {
                //TODO: insert
            }

            iterator insert(const_iterator hint, const value_type& val) {
                //TODO: insert
            }

            iterator insert(const_iterator hint, value_type& val) {
                //TODO: insert
            }

            template<typename P>
            std::enable_if_t<std::is_constructible<value_type, P&&>::value, iterator> insert(const_iterator hint, P&& p) {
                //TODO: insert
            }

            template<typename I>
            void insert(I first, I last) {
                //TODO: insert using iterators
            }

            void insert(std::initializer_list<value_type> i) {
                //TODO: insert using inti list
            }

            template<typename O>
            std::pair<iterator, bool> insert_or_assign(const key_type& k, O&& o) {
                auto r = try_emplace(cend(), k, std::forward<O>(o));
                if (!r.second) {
                    //TODO: assign
                }
                return r;
            }

            template<typename O>
            std::pair<iterator, bool> insert_or_assign(key_type&& k, O&& o) {
                auto r = try_emplace(cend(), std::move(k), std::forward<O>(o));
                if (!r.second) {
                    //TODO: assign
                }
                return r;
            }

            template<typename O>
            std::pair<iterator, bool> insert_or_assign(const_iterator hint, const key_type& k, O&& o) {
                auto r = try_emplace(hint, k, std::forward<O>(o));
                if (!r.second) {
                    //TODO: assign
                }
                return r;
            }

            template<typename O>
            std::pair<iterator, bool> insert_or_assign(const_iterator hint, key_type&& k, O&& o) {
                auto r = try_emplace(hint, std::move(k), std::forward<O>(o));
                if (!r.second) {
                    //TODO: assign
                }
                return r;
            }

            iterator erase(const_iterator pos) {
                //TODO erase
            }

            iterator erase(iterator pos) {
                //TODO erase
            }

            size_type erase(const key_type& key) {
                //TODO: erase
            }

            iterator erase(const_iterator first, const_iterator last) {
                //TODO: erase
            }

            void clear() noexcept {
                data.clear();
                index.clear();
                //TODO: rehash???
            }

            void swap(packed_map& other) { //TODO: noexcept
                data.swap(other.data);
                index.swap(other.index);
            }

            //TODO: merge

            hasher hash_function() const {
                return hashf;
            }

            key_equal key_eq() const {
                return eqf;
            }

            iterator find(cosnt key_type& key) {
                //TODO: find
            }

            cosnt_iterator find(cosnt key_type& key) const {
                //TODO: find
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
                //TODO: at
            }

            mapped_type& at(key_type&& key) {
                //TODO: at
            }

            size_type bucket_count() const noexcept {
                return index.size();
            }

            size_type max_bucket_count() const noexcept {
                return index.max_size();
            }

            size_type bucket_size() const {
                //TODO: bucket_size
            }

            size_type bucket(const key_type& key) {
                //TODO: bucket
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

    };
}

