#include "App.h"
#include "Utils/SystemInitializer.h"

int main() {
    vk2::system_initializer::initialize();

    {
        App app;
        app.run();
    }

    vk2::system_initializer::finalize();
}