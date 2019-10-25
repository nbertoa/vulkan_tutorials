#include "App.h"
#include "RenderPassCreator.h"

int main() {
    App app(800,
            600,
            "Simple Triangle With Index Buffer App",
            renderPassCreator());

    app.run();
}