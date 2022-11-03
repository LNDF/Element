#include <iostream>

#include "ecs/scene.h"
#include "ecs/game_object.h"
#include "utils/packed_map.h"
#include "utils/padded_array_view.h"
#include <string>

using namespace engine;

struct test_comp {
    int a, b;
    float c, d;
};

struct t2 {
    bool a;
};

struct test {
    int a;
    float t;
};

int main() {
    /*
    scene s;
    game_object* g = s.create_game_object();
    game_object* g2 = s.create_game_object();
    game_object* g3 = s.create_game_object();
    std::cout << g->get_uuid().str() << std::endl;
    std::cout << g2->get_uuid().str() << std::endl;
    std::cout << g3->get_uuid().str() << std::endl;
    s.add_component<test_comp>(g, {1, 2, 4.5f, 23.24f});
    s.add_component<test_comp>(g2, {2, 3, 4.4f, 12.1f});
    s.add_component<t2>(g2, {false});
    std::cout << std::endl;
    s.view<test_comp>().each([](game_object* obj, test_comp* c) {
        std::cout << obj->get_uuid().str() << " " << c->a << std::endl;
    });
    std::cout << std::endl;
    s.view<t2>().each([](game_object* obj, t2* c) {
        std::cout << obj->get_uuid().str() << " " << c->a << std::endl;
    });
    std::cout << std::endl;
    s.view<test_comp, t2>().each([](game_object* obj, test_comp* tc, t2* c) {
        std::cout << obj->get_uuid().str() << " " << tc->a << " " << c->a << std::endl;
    });
    */
    std::vector<test> tv;
    tv.push_back({1, 34.43f});
    tv.push_back({2, 334.4f});
    tv.push_back({3, 1.23f});
    padded_array_view<float> mv(tv.data() + offsetof(test, t), sizeof(test), tv.size());
    return 0;
}