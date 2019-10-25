#include "App.h"
#include "RenderPassCreator.h"

int main() {
    App app(800,
            600,
            "Simple Triangle App",
            renderPassCreator());

    app.run();
}