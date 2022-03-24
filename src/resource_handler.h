#ifndef __RESOURCEHANDLER_H__
#define __RESOURCEHANDLER_H__

#include <string>
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
        const std::string& getUUID() const;
        static u32 checkUUID(const std::string& uuid);
    private:
        T* ptr;
        std::string uuid;
};

#endif