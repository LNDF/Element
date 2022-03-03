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
        GLBackend(const std::string& title, u32 width, u32 height, u8 resizable);
        ~GLBackend();
        void mainLoop();
        void close();
        void notClose();
        void setWidth(u32 width);
        void setHeight(u32 height);
        void setName(const std::string& name);
        void setMainLoopCallback(MainLoopCallback mainLoopCallback);
        const std::string& getName() const;
        u32 getWidth() const;
        u32 getHeight() const;
        MainLoopCallback getMainLoopCallback() const;

        //states
        void setSwapInterval(int interval);
        void setViewport(u32 width, u32 height);

        //utils
        void clear();
};

#endif
