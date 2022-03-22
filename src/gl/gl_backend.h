#ifndef __GLBACKEND_H__
#define __GLBACKEND_H__

#include "../common.h"
#include "gl_include.h"
#include "../backend.h"
#include <string>

class GLBackend : public Backend {
    private:
        std::string name;
        u32 width;
        u32 height;
        GLFWwindow* glfwWindowId;
        u8 resizable;
        MainLoopCallback mainLoopCallback;

        //states
        int swapInterval;
        u32 vpWidth;
        u32 vpHeight;
    public:
        GLBackend(const std::string& name, u32 width, u32 height, u8 resizable);
        virtual ~GLBackend();
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

#endif
