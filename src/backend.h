#ifndef __BACKEND_H__
#define __BACKEND_H__

#include "common.h"
#include <string>

#include "texture/texture_image.h"

typedef void (*MainLoopCallback)();

class Backend {
    public:
        Backend();
        virtual ~Backend();
        virtual void mainLoop() = 0;
        virtual void close() = 0;
        virtual void notClose() = 0;
        virtual void setWidth(u32 width) = 0;
        virtual void setHeight(u32 height) = 0;
        virtual void setName(const std::string& name) = 0;
        virtual void setMainLoopCallback(MainLoopCallback mainLoopCallback) = 0;
        virtual const std::string& getName() const = 0;
        virtual u32 getWidth() const = 0;
        virtual u32 getHeight() const = 0;
        virtual MainLoopCallback getMainLoopCallback() const = 0;

        //states
        virtual void setSwapInterval(int interval) = 0;
        virtual void setViewport(u32 width, u32 height) = 0;

        //utils
        virtual void clear() = 0;

        //textures
        virtual TextureImage* createTextureImageFromFile(const std::string& path) = 0;
        virtual TextureImage* createTextureImageFromMemory(const u8* data, u32 len) = 0;
        virtual TextureImage* createTextureImageFromRawRGBA8Memory(const u8* data, u32 width, u32 length) = 0;
};

extern Backend* currentBackendInstance;

#endif