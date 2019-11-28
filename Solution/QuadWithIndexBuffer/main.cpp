#include "App.h"
#include "Utils/SystemInitializer.h"

int main() {
    vk::system_initializer::initialize();

    {
        App app;
        app.run();
    }

    vk::system_initializer::finalize();
}