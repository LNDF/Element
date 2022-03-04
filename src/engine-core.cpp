#include <iostream>

#include "gl/gl_backend.h"
#include "delta_time.h"

int main(){
    GLBackend b("This is a test", 800, 600, 1);
    b.setMainLoopCallback([]() {
        std::cout << deltatime::time << std::endl;
    });
    b.mainLoop();
    return 0;
}
