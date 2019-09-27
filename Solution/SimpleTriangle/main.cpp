#include "SimpleTriangleApp.h"
#include "GraphicsPipelineCreator.h"
#include "RenderPassCreator.h"

int main() {
    SimpleTriangleApp app(800,
                          600,
                          "Simple Triangle App",
                          renderPassCreator(),
                          graphicsPipelineCreator());

    app.run();
}