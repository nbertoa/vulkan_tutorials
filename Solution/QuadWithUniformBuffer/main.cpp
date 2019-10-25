#include "App.h"
#include "RenderPassCreator.h"

int main() {
    App app(800,
            600,
            "Quad With Uniform Buffer",
            renderPassCreator());

    app.run();
}