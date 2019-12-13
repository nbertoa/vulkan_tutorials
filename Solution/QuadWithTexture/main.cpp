#include "App.h"
#include "Utils/SystemInitializer.h"

int main() {
    vulkan::system_initializer::initialize();

    {
        App app;
        app.run();
    }

    vulkan::system_initializer::finalize();
}