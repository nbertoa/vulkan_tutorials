#include "App.h"
#include "GraphicsPipelineCreator.h"
#include "RenderPassCreator.h"

int main() {
    App app(800,
            600,
            "Quad With Uniform Buffer",
            renderPassCreator(),
            graphicsPipelineCreator());

    app.run();
}