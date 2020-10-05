//
// Created by cacoethes on 7/28/20.
//

#ifndef VULKAN_SPECS_INITIALIZER_H
#define VULKAN_SPECS_INITIALIZER_H
#include "build_configs.h"
#include "BackgroundCheck.h"
#include "PipelineResourceSetup.h"
#include "Renderer.h"

#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <cassert>

namespace Render{
    void Resize(Initializer* p_init);
    void Render_some(Initializer* p_init);
    void Set_Env(Initializer* p_init);
    void Check_Resize(Initializer* p_init);
}

class Initializer {
private:
    BackgroundCheck* BI;
    VkInstance       Instance   = nullptr;
    VkPhysicalDevice PhDevice   = nullptr;
    VkDevice         Device     = nullptr;
    VkSurfaceKHR     Surface    = nullptr;
    VkSwapchainKHR   Swapchain  = nullptr;
    VkCommandPool    CmdPool    = nullptr;
    VkImage swapchain_images[16];
    VkQueue          Queues[3]; // 0th Graphic, 1st Transfer, 2nd Compute if All queues are exclusive
    VkResult         results[10];
    VkPipeline       pipeline[1];
    GLFWwindow* window_glfw = nullptr;
    VkSwapchainKHR oldSwapchain = nullptr;
    std::vector<VkSemaphore> acquire_Semaphores;
    std::vector<VkSemaphore> complete_Semaphores;
    std::vector<VkFence> Fences;
    std::vector<VkCommandBuffer> CmdBuffers;
    std::vector<VkShaderModule> shaderModules;
    uint32_t image_index;
    uint32_t width;
    uint32_t height;
    uint32_t swapchain_image_count;
    uint32_t pipeline_obj_count;
    PipelineResourceSetup** PIs;
    friend class BackgroundCheck;
    friend class PipelineResourceSetup;
public:
    explicit Initializer(BackgroundCheck *bi, PipelineResourceSetup **pis, int pipeline_count);

    ~Initializer();

    void Create_Instance(const char *, float);

    void Create_PDevice();

    void Create_Device();

    void Create_Queues();

    //VkInstance get_Instance(); we may need it someday.
    void Create_Window(const char* display_name,uint32_t w, uint32_t h);

    void Create_Surface();

    void Create_Swapchain();

    void Resize_Swapchain();

    void Resize_Swapchain_Threaded();

    void Create_Semaphore();

    void Create_PresentInfo();

    void Create_CommandPool();

    void Create_CommandBuffer();

    void loadShadermodule(const char *path);

    void Render();

    GLFWwindow *getWindowGlfw() const;

    friend void Render::Resize(Initializer* p_init);
    friend void Render::Render_some(Initializer* p_init);
    friend void Render::Set_Env(Initializer* p_init);
    friend void Render::Check_Resize(Initializer* p_init);

    void Prepeare_Swapchain_Recreate();
};

#endif //VULKAN_SPECS_INITIALIZER_H
