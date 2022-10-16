#pragma once

#include <unordered_map>
#include <vector>
#include <utility>

namespace engine {
    
    template<typename K, typename V>
    class packed_map {
        public:
            void insert(const K& k, const V& v) {
                if (indexes.find(k) == indexes.end()) {
                    indexes[k] = packed.size();
                    keys.push_back(k);
                    packed.push_back(v);
                }
            }

            void insert(const K& k, V&& v) {
                if (indexes.find(k) == indexes.end()) {
                    indexes[k] = packed.size();
                    keys.push_back(k);
                    packed.push_back(std::forward<V&&>(v));
                }
            }

            void insert(K&& k, const V& v) {
                if (indexes.find(std::forward<K&&>(k)) == indexes.end()) {
                    indexes[std::forward<K&&>(k)] = packed.size();
                    keys.push_back(std::forward<K&&>(k));
                    packed.push_back(v);
                }
            }

            void insert(K&& k, V&& v) {
                if (indexes.find(std::forward<K&&>(k)) == indexes.end()) {
                    indexes[std::forward<V&&>(k)] = packed.size();
                    keys.push_back(std::forward<K&&>(k));
                    packed.push_back(std::forward<V&&>(v));
                }
            }

            template<typename... A>
            void emplace(const K& k, A&&... args) {
                if (indexes.find(k) == indexes.end()) {
                    indexes[k] = packed.size();
                    keys.push_back(k);
                    packed.emplace_back(std::forward<A>(args)...);
                }
            }

            template<typename... A>
            void emplace(K&& k, A&&... args) {
                if (indexes.find(std::forward<K&&>(k)) == indexes.end()) {
                    indexes[std::forward<K&&>(k)] = packed.size();
                    keys.push_back(std::forward<K&&>(k));
                    packed.emplace_back(std::forward<A>(args)...);
                }
            }

            void erase(const K& key) {
                if (indexes.find(key) != indexes.end()) {
                    std::size_t i = indexes[key];
                    if (packed.size() - 1 != i) {
                        packed[i].~V();
                        packed[i] = std::move(packed[packed.size() - 1]);
                        indexes[keys[packed.size() - 1]] = i;
                        keys[i].~K();
                        keys[i] = std::move(keys[keys.size() - 1]);
                    }
                    packed.pop_back();
                    keys.pop_back();
                    indexes.erase(key);
                }
            }

            inline bool contains(const K& k) const {
                return indexes.find(k) != indexes.end();
            }

            V& operator [](const K& k) {
                return packed[indexes[k]];
            }

            const V& operator [](const K& k) const {
                return packed[indexes[k]];
            }

            inline std::vector<V>::size_type size() {return packed.size();}

            inline auto begin() {
                return packed.begin();
            }

            inline auto end() {
                return packed.end();
            }

            inline auto begin() const {
                return packed.begin();
            }

            inline auto end() const {
                return packed.end();
            }

            inline auto cbegin() const {
                return packed.cbegin();
            }

            inline auto cend() const {
                return packed.cend();
            }

            inline auto rbegin() {
                return packed.rbegin();
            }

            inline auto rend() {
                return packed.rend();
            }

            inline auto rbegin() const {
                return packed.rbegin();
            }

            inline auto rend() const {
                return packed.rend();
            }

            inline auto crbegin() const {
                return packed.crbegin();
            }

            inline auto crend() const {
                return packed.crend();
            }

            inline const auto& get_indexes() const {return indexes;}
            inline const auto& get_keys() const {return keys;}
            inline const auto& get_packed() const {return packed;}
        private:
            std::unordered_map<K, std::size_t> indexes;
            std::vector<K> keys;
            std::vector<V> packed;
    };

} // namespace engine