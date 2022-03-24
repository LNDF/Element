#include "resource_handler.h"
#include <unordered_map>

std::unordered_map<std::string, void*> resourceHandlePointers;
std::unordered_map<std::string, u32> resourceHandleCounts;

template<typename T>
ReferenceHandle<T>::ReferenceHandle(T* item, const std::string& uuid) {
    if (resourceHandleCounts.count(uuid)) {
        this->ptr = static_cast<T>(resourceHandlePointers[uuid]);
        resourceHandleCounts[uuid]++;
    } else {
        this->ptr = item;
        resourceHandlePointers[uuid] = item;
        resourceHandleCounts[uuid] = 1;
    }
    this->uuid = uuid;
}

template<typename T>
ReferenceHandle<T>::ReferenceHandle(const ReferenceHandle<T>& p) {
    resourceHandleCounts[p.uuid]++;
    this->ptr = static_cast<T>(resourceHandlePointers[p.uuid]);
    this->uuid = p.uuid;
}

template<typename T>
ReferenceHandle<T>::~ReferenceHandle() {
    if (--resourceHandleCounts[this->uuid] == 0) {
        delete this->ptr;
        resourceHandleCounts.erase(this->uuid);
        resourceHandlePointers.erase(this->uuid);
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
const std::string& ReferenceHandle<T>::getUUID() const {
    return this->uuid;
}

template<typename T>
u32 ReferenceHandle<T>::checkUUID(const std::string& uuid) {
    if (resourceHandleCounts.count(uuid)) return resourceHandleCounts[uuid];
    return 0;
}