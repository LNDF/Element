#include <iostream>

#include "ecs/scene.h"
#include "ecs/gameobject.h"
#include <string>

using namespace engine;

struct test_comp {
    int a, b;
    float c, d;
};

int main() {
    scene s;
    game_object* g = s.create_game_object();;
    std::cout << g->get_uuid().str() << std::endl;
    s.add_component<test_comp>(g, {1, 2, 4.5f, 23.24f});
    //test_comp* t = s.get_component<test_comp>(g);
    s.remove_component<test_comp>(g);
    return 0;
}