#include "loader.h"

#include <core/log.h>
#include <GL/glew.h>

static bool is_opengl_loaded = false;

void element::opengl_load() {
    if (is_opengl_loaded) return;
    is_opengl_loaded = true;
    glewExperimental = GL_TRUE;
    ELM_INFO("Setting up OpenGl...");
    GLenum ret = glewInit();
    if (ret != GLEW_OK) {
        ELM_CRITICAL("GLEW could not be started. Error: {}", (const char*) glewGetErrorString(ret));
        return;
    }
    ELM_DEBUG("GLEW started.");
    ELM_INFO("OpenGL info:");
    ELM_INFO("  Renderer:     {}", (const char*) glGetString(GL_RENDERER));
    ELM_INFO("  Vendor:       {}", (const char*) glGetString(GL_VENDOR));
    ELM_INFO("  Version:      {}", (const char*) glGetString(GL_VERSION));
    ELM_INFO("  GLSL version: {}", (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION));
    GLint exts;
    glGetIntegerv(GL_NUM_EXTENSIONS, &exts);
    ELM_DEBUG("OpenGL supported extensions ({} supported):", exts);
    for (int i = 0; i < exts; i++) {
        ELM_DEBUG("  {}",  (const char*) glGetStringi(GL_EXTENSIONS, i));
    }
}