#include <iostream>
#include <thread>
#include "Initializer.h"

int main() {
    BackgroundCheck B_Instance;
    PipelineResourceSetup *Pipelines[1];
    PipelineResourceSetup trianglePipeline;
    Pipelines[0] = &trianglePipeline;
    Initializer Renderer_Instance(&B_Instance, Pipelines, 1);

    Renderer_Instance.Create_Instance("MY DEMO",1.2f);
    Renderer_Instance.Create_PDevice();
    Renderer_Instance.Create_Device();
    Renderer_Instance.Create_Queues();
    Renderer_Instance.Create_Window("Display",654,704);
    Renderer_Instance.Create_Surface();
    Renderer_Instance.Create_Swapchain();
    Renderer_Instance.Create_Semaphore();
    Renderer_Instance.Create_CommandPool();
    Renderer_Instance.Create_CommandBuffer();
    Renderer_Instance.loadShadermodule("/home/cacoethes/CLionProjects/Vulkan_Specs/shaders/triangle.vert.spv");
    Renderer_Instance.loadShadermodule("/home/cacoethes/CLionProjects/Vulkan_Specs/shaders/triangle.frag.spv");
    Renderer_Instance.Create_PresentInfo();

    std::cout << "Building as : " << BUILD_TYPE << std::endl;
//    glfwHideWindow(Renderer_Instance.getWindowGlfw());
    Render::Set_Env(&Renderer_Instance);
    std::thread th(Render::Check_Resize, &Renderer_Instance);
    Render::Render_start();

    std::this_thread::sleep_for(std::chrono::seconds(4));
    th.join();
    Render::End_Rendering();

    return 0;
}
