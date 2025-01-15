#include "../include/headers/Engine.h"


int main() {
    Engine& engine = Engine::getInstance();

    if (!engine.initialize("Game Project", 800, 600)) {
        return -1;
    }

    engine.run();
    return 0;
}