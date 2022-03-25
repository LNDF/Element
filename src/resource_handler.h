#ifndef __RESOURCEHANDLER_H__
#define __RESOURCEHANDLER_H__

#include <string>
#include <unordered_map>
#include "common.h"

template<typename T>
class ReferenceHandle {
    public:
        T* operator->();
        T& operator*();
        ReferenceHandle<T>& operator=(const ReferenceHandle<T>& p);
        ReferenceHandle(T* item, const std::string& uuid);
        ReferenceHandle(const ReferenceHandle<T>& p);
        ~ReferenceHandle();
        static u32 checkUUID(const std::string& uuid);
    private:
        static std::unordered_map<std::string, T*> pointers;
        static std::unordered_map<std::string, u32> counts;
        T* ptr;
        std::string uuid;
};

template<typename T>
std::unordered_map<std::string, T*> ReferenceHandle<T>::pointers;

template<typename T>
std::unordered_map<std::string, u32> ReferenceHandle<T>::counts;

template<typename T>
ReferenceHandle<T>::ReferenceHandle(T* item, const std::string& uuid) {
    if (ReferenceHandle<T>::counts.count(uuid)) {
        this->ptr = ReferenceHandle<T>::pointers[uuid];
        ReferenceHandle<T>::counts[uuid]++;
    } else {
        this->ptr = item;
        ReferenceHandle<T>::pointers[uuid] = item;
        ReferenceHandle<T>::counts[uuid] = 1;
    }
    this->uuid = uuid;
}

template<typename T>
ReferenceHandle<T>::ReferenceHandle(const ReferenceHandle<T>& p) {
    ReferenceHandle<T>::counts[p.uuid]++;
    this->ptr = ReferenceHandle<T>::pointers[p.uuid];
    this->uuid = p.uuid;
}

template<typename T>
ReferenceHandle<T>::~ReferenceHandle() {
    if (--ReferenceHandle<T>::counts[this->uuid] == 0) {
        delete this->ptr;
        ReferenceHandle<T>::counts.erase(this->uuid);
        ReferenceHandle<T>::counts.erase(this->uuid);
    }
}

template<typename T>
T* ReferenceHandle<T>::operator->() {
    return this->ptr;
}

template<typename T>
T& ReferenceHandle<T>::operator*() {
    return *this->ptr;
}

template<typename T>
u32 ReferenceHandle<T>::checkUUID(const std::string& uuid) {
    if (ReferenceHandle<T>::counts.count(uuid)) return ReferenceHandle<T>::counts[uuid];
    return 0;
}

#endif