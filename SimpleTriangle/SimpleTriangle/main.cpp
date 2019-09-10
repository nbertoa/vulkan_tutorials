#include "App.h"
#include "GraphicsPipelineCreator.h"
#include "RenderPassCreator.h"

int main() {
    App app(800, 
            600, 
            "Simple Triangle App",
            renderPassCreator(),
            graphicsPipelineCreator());

    app.run();
}