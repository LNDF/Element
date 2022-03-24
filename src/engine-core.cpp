#include <iostream>

#include "gl/gl_backend.h"
#include "delta_time.h"

#include "resource_handler.h"

class a {
    public:
        a() {
            std::cout << "hello" << std::endl;
        }
        ~a() {
            std::cout << "good bye" << std::endl;
        }
};

typedef ReferenceHandle<a> aa;

int main(){
    aa* p1 = new aa(new a(), "id1");
    aa* p2 = new aa(new a(), "id1");
    aa* p3 = new aa(new a(), "id1");
    aa* p4 = new aa(new a(), "id1");
    delete p4;
    delete p3;
    aa* p5 = new aa(new a(), "id1");
    delete p1;
    delete p2;
    delete p5;
    return 0;
    GLBackend b("This is a test", 800, 600, 1);
    b.setMainLoopCallback([]() {
        std::cout << deltatime::time << std::endl;
    });
    b.mainLoop();
    return 0;
}
