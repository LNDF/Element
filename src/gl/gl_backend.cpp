#include "gl_backend.h"
#include "../delta_time.h"
#include "texture/gl_texture_image.h"
#include <stdexcept>


GLBackend::GLBackend(const std::string& name, u32 width, u32 height, u8 resizable) {
    //First start
    this->name = name;
    this->width = width;
    this->height = height;
    this->resizable = resizable;
    this->mainLoopCallback = nullptr;
    if (!glfwInit()) {
        throw std::runtime_error("Cannot init GLFW");
    }
    //init
    glfwWindowHint(GLFW_RESIZABLE, this->resizable);
    this->glfwWindowId = glfwCreateWindow(this->width, this->height, this->name.c_str(), NULL, NULL);
    if (this->glfwWindowId == 0) {
        glfwTerminate();
        throw std::runtime_error("Cannot create GLFW window");
    }
    currentBackendInstance = this;
    glfwMakeContextCurrent(this->glfwWindowId);
    this->setSwapInterval(1);
    //TODO: blend, depth test, culling
    glfwSetWindowSizeCallback(this->glfwWindowId, [](GLFWwindow* window, int width, int height) {
        if (currentBackendInstance != nullptr) {
            currentBackendInstance->setWidth(width);
            currentBackendInstance->setHeight(height);
        }
    });
}

GLBackend::~GLBackend() {
    glfwTerminate();
    currentBackendInstance = nullptr;
}

void GLBackend::mainLoop() {
    if (mainLoopCallback == nullptr) {
        throw std::runtime_error("MainLoopCallback is null");
    }
    deltatime::set();
    while (!glfwWindowShouldClose(this->glfwWindowId)) {
        deltatime::set();
        this->mainLoopCallback();
        glfwSwapBuffers(this->glfwWindowId);
        glfwPollEvents();
    }
}

void GLBackend::close() {
    glfwSetWindowShouldClose(this->glfwWindowId, 1);
}

void GLBackend::notClose() {
    glfwSetWindowShouldClose(this->glfwWindowId, 0);
}

void GLBackend::setWidth(u32 width) {
    this->width = width;
}

void GLBackend::setHeight(u32 height) {
    this->height = height;
}

void GLBackend::setName(const std::string& name) {
    this->name = name;
    glfwSetWindowTitle(this->glfwWindowId, name.c_str());
}

void GLBackend::setMainLoopCallback(MainLoopCallback mainLoopCallback) {
    this->mainLoopCallback = mainLoopCallback;
}

const std::string& GLBackend::getName() const {
    return this->name;
}

u32 GLBackend::getWidth() const {
    return this->width;
}

u32 GLBackend::getHeight() const {
    return this->height;
}

MainLoopCallback GLBackend::getMainLoopCallback() const {
    return this->mainLoopCallback;
}

//states

void GLBackend::setSwapInterval(int interval) {
    if (interval != this->swapInterval) {   
        this->swapInterval = interval;
        glfwSwapInterval(this->swapInterval);
    }
}

void GLBackend::setViewport(u32 width, u32 height) {
    if (width != this->vpWidth || height != this->vpHeight) {
        glViewport(0, 0, width, height);
        this->vpWidth = width;
        this->vpHeight = height;
    }
}

//utils

void GLBackend::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

//textures

TextureImage* GLBackend::createTextureImageFromFile(const std::string& path) {
    return new GLTextureImage(path);
}

TextureImage* GLBackend::createTextureImageFromMemory(const u8* data, u32 len) {
    return new GLTextureImage(data, len);
}

TextureImage* GLBackend::createTextureImageFromRawRGBA8Memory(const u8* data, u32 width, u32 height) {
    return new GLTextureImage(data, width, height);
}