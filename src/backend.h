#ifndef __BACKEND_H__
#define __BACKEND_H__

#include "common.h"
#include <string>

typedef void (*MainLoopCallback)();

class Backend {
    public:
        Backend();
        virtual ~Backend();
        virtual void mainLoop();
        virtual void close();
        virtual void notClose();
        virtual void setWidth(u32 width);
        virtual void setHeight(u32 height);
        virtual void setName(const std::string& name);
        virtual void setMainLoopCallback(MainLoopCallback mainLoopCallback);
        virtual const std::string& getName() const;
        virtual u32 getWidth() const;
        virtual u32 getHeight() const;
        virtual MainLoopCallback getMainLoopCallback() const;

        //states
        virtual void setSwapInterval(int interval);
        virtual void setViewport(u32 width, u32 height);

        //utils
        virtual void clear();
};

extern Backend* currentBackendInstance;

#endif