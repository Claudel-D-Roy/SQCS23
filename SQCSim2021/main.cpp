#include <SFML/Window.hpp>
#include <iostream>
#include "engine.h"

int main() {
    Engine engine;
    engine.SetMaxFps(60);
    engine.Start("Syndicat Quebecois de la Construction Simulator 2023", BASE_WIDTH, BASE_HEIGHT, false);
}
