#include "SimpleTriangleWithIndexBufferApp.h"
#include "GraphicsPipelineCreator.h"
#include "RenderPassCreator.h"

int main() {
    SimpleTriangleWithIndexBufferApp app(800,
                                         600,
                                         "Simple Triangle With Index Buffer App",
                                         renderPassCreator(),
                                         graphicsPipelineCreator());

    app.run();
}