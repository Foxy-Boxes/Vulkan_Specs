//
// Created by cacoethes on 7/28/20.
//

#include <iostream>
#include <cstring>
#include <fstream>
#include "global_inline.h"
#include "Initializer.h"

void Initializer::Create_Instance(const char *AppNAME, float Vulkan_API_Version) {
    BI->App_Info.apiVersion = ((Vulkan_API_Version == 1.0f) * (VK_API_VERSION_1_0))
                          + ((Vulkan_API_Version == 1.1f) * (VK_API_VERSION_1_1))
                          + ((Vulkan_API_Version == 1.2f) * (VK_API_VERSION_1_2));
    BI->App_Info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    BI->App_Info.pApplicationName = AppNAME;
    BI->App_Info.pEngineName = "Sigma-Vk Rendering Engine";
    BI->Instance_C_Info.pApplicationInfo = &BI->App_Info;
#if defined(DEBUG)
    BI->Add_Instance_Layer("VK_LAYER_KHRONOS_validation");
    BI->Add_Instance_Extension_List('d');
#endif
#if defined(VK_USE_PLATFORM_XCB_KHR)
    BI->Add_Instance_Extension(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
    BI->Add_Instance_Extension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif 1
    BI->Add_Instance_Extension_List('g');
#else
#error UNKNOWN Platform
#endif
#ifndef GLFW_INCLUDE_VULKAN
//    BI->Add_Instance_Extension(VK_KHR_SURFACE_EXTENSION_NAME);
#endif
    BI->Check_Instance_Extensions();
    BI->Check_Instance_Layers();
    check_vk_result(vkCreateInstance(&BI->Instance_C_Info, nullptr, &Instance),
                 "CANNOT Create Instance");
#if defined(DEBUG)
    BI->DebugSetup();
#endif
}

void Initializer::Create_PDevice() {
    uint32_t device_count = 0;
    check_vk_result(vkEnumeratePhysicalDevices(Instance, &device_count, NULL),
                    "Enumeration Failed");
    check_vk_count(device_count,
                   "Physical Devices");
    VkPhysicalDevice PhDevices[device_count];
    check_vk_result(vkEnumeratePhysicalDevices(Instance, &device_count, PhDevices),
                    "Problem Retrieving Physical Devices");
    VkPhysicalDeviceProperties deviceProperties[device_count];
    for (uint32_t i = 0; i < device_count; ++i) {
        vkGetPhysicalDeviceProperties(PhDevices[i], deviceProperties);
        if (deviceProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            std::cout << "Detected GPU : " << deviceProperties[i].deviceName << std::endl;
            PhDevice = PhDevices[i];
            break;
        }
    }
    if (!PhDevice && device_count) {
        std::cout << "No Gpu Available Falling-back to a : " << DeviceTypeStr(deviceProperties[0].deviceType) <<
                  ", which has the name : " << deviceProperties[0].deviceName << std::endl;
        PhDevice = PhDevices[0];
    }
}

void Initializer::Create_Device() {
    BI->Setup_Device_Background();
    BI->Add_Device_Extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    BI->Check_Device_Extensions();
    check_vk_result(vkCreateDevice(PhDevice, &BI->Device_C_Info, NULL, &Device),
                 "CANNOT Create Device");

    if(!glfwGetPhysicalDevicePresentationSupport(Instance, PhDevice, BI->Queue_C_Infos[0].queueFamilyIndex)){
        std::cerr << "Not Good\n";
    }
}


void Initializer::Create_Queues() {
    for (uint32_t i = 0; i < BI->seperate_queue_count; ++i) {
        for (int j = 0; j < BI->Queue_C_Infos[i].queueCount; ++j) {
            vkGetDeviceQueue(Device, BI->Queue_C_Infos[i].queueFamilyIndex, j, &Queues[i]);
        }
    }
}


void Initializer::Create_Window(const char* display_name,uint32_t w, uint32_t h) {
    width = w;
    height = h;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window_glfw = glfwCreateWindow(width, height, display_name, NULL, NULL);

    assert(window_glfw);
}

void Initializer::Create_Surface() {
    check_vk_result(glfwCreateWindowSurface(Instance, window_glfw, nullptr, &Surface),
                    "CANNOT Create Surface");
//    std::cout << Surface << std::endl;
    uint32_t s_format_count;
    check_vk_result(vkGetPhysicalDeviceSurfaceFormatsKHR(PhDevice, Surface, &s_format_count, NULL),
                 "Retrieving Surface Format Count Failed");
    check_vk_count(s_format_count,
                   "Surface Formats");
    VkSurfaceFormatKHR available_formats[s_format_count];
    check_vk_result(vkGetPhysicalDeviceSurfaceFormatsKHR(PhDevice, Surface, &s_format_count, available_formats),
                    "Error in Available Format retrieval");
    bool find_preffered = false;
    for (uint32_t i = 0; i < s_format_count; ++i) {
        if (available_formats[i].format == VK_FORMAT_A8B8G8R8_UNORM_PACK32
        && available_formats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR){
            find_preffered = true;
            break;
        }
    }
    if (!find_preffered){
        for (uint32_t i = 0; i < s_format_count; ++i) {
            if (available_formats[i].format == VK_FORMAT_A8B8G8R8_UNORM_PACK32
               || available_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
                BI->Surface_format.format = available_formats[i].format;
                BI->Surface_format.colorSpace = available_formats[i].colorSpace;
                break;
            }
        }
    }
    if (s_format_count == 1 && available_formats[0].format == VK_FORMAT_UNDEFINED){
        BI->Surface_format.format = VK_FORMAT_A8B8G8R8_UNORM_PACK32;
    }
    find_preffered = false;
    uint32_t p_mode_count;
    check_vk_result(vkGetPhysicalDeviceSurfacePresentModesKHR(PhDevice, Surface, &p_mode_count, NULL),
                    "Retrieving Present Mode Count Failed");
    check_vk_count(p_mode_count,
                   "Present Modes");
    VkPresentModeKHR available_modes[p_mode_count];
    check_vk_result(vkGetPhysicalDeviceSurfacePresentModesKHR(PhDevice, Surface, &p_mode_count, available_modes),
                    "Error in Available Mode retrieval");
    for (int j = 0; j < p_mode_count; ++j) {
        if (available_modes[j] == VK_PRESENT_MODE_MAILBOX_KHR){
            find_preffered = true;
            break;
        }
    }
    if (!find_preffered) {
        for (int j = 0; j < p_mode_count; ++j) {
            if (available_modes[j] == VK_PRESENT_MODE_FIFO_RELAXED_KHR) {
                BI->Present_mode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
                break;
            }
            else if (available_modes[j] == VK_PRESENT_MODE_FIFO_KHR){
                BI->Present_mode = VK_PRESENT_MODE_FIFO_KHR;
            }
        }
        if (BI->Present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            BI->Present_mode = available_modes[0];
        }
    }
    BI->Surface_Support_Check();
}

#ifdef VK_USE_PLATFORM_WIN32_KHR
void Initializer::Create_Window() {
}
void Initializer::Create_Surface() {
    return;
}

#endif
void Initializer::Create_Swapchain() {
    BI->Setup_Swapchain_Background();
    BI->Swapchain_C_Info.surface = Surface;
//    BI->Swapchain_C_Info.minImageCount = 2;
    BI->Swapchain_C_Info.imageFormat = BI->Surface_format.format;
    BI->Swapchain_C_Info.imageColorSpace = BI->Surface_format.colorSpace;
    BI->Swapchain_C_Info.imageExtent.width = width;
    BI->Swapchain_C_Info.imageExtent.height = height;
    BI->Swapchain_C_Info.imageArrayLayers = 1;
    BI->Swapchain_C_Info.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    check_vk_result(vkCreateSwapchainKHR(Device, &BI->Swapchain_C_Info, nullptr, &Swapchain),
                    "CANNOT Create Swapchain");

}


void Initializer::Create_Semaphore() {
    VkSemaphoreCreateInfo S_Create_Info = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    for (uint32_t i = 0; i < BI->Swapchain_C_Info.minImageCount; ++i) {
        acquire_Semaphores.push_back(nullptr);
        check_vk_result(vkCreateSemaphore(Device, &S_Create_Info, NULL, &(*(acquire_Semaphores.end() - 1))),
                        "CANNOT Create Semaphore");
        complete_Semaphores.push_back(nullptr);
        check_vk_result(vkCreateSemaphore(Device, &S_Create_Info, NULL, &(*(complete_Semaphores.end() - 1))),
                        "CANNOT Create Semaphore");
    }
}

void Initializer::Create_PresentInfo() {
    BI->CmdBuffer_B_Info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    swapchain_image_count = 0;
    vkGetSwapchainImagesKHR(Device, Swapchain, &swapchain_image_count, NULL);
    check_vk_count(swapchain_image_count,
                   "Swapchain Images");
    check_vk_result(vkGetSwapchainImagesKHR(Device, Swapchain, &swapchain_image_count, swapchain_images),
                    "Problem getting images");
    BI->Present_Info.swapchainCount = 1;
    BI->Present_Info.pSwapchains = &Swapchain;
    BI->Present_Info.pImageIndices = &image_index;
    BI->Present_Info.waitSemaphoreCount = 1;
    BI->Present_Info.pWaitSemaphores = &complete_Semaphores[0];
    BI->Present_Info.pResults = results;
    VkClearColorValue color = {100.0f/255.0f,0.6f,1.0f,1.0f};
    VkClearValue clearValue = {color};
    PIs[0]->setOffset(0, 0);
    PIs[0]->setExtent(width, height);
    PIs[0]->setClearValues(&clearValue,1,swapchain_image_count);
    PIs[0]->createRenderPass();
    PIs[0]->createImageViews(swapchain_image_count, swapchain_images);
    PIs[0]->createFramebuffers(swapchain_image_count);
    VkShaderStageFlagBits stageFlagBits[] = {VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT};

    VkPipelineShaderStageCreateFlags stageCreateFlags[] = {0, 0};
    PIs[0]->setStageProps(2, stageFlagBits, shaderModules.data(), stageCreateFlags);
    pipeline[0] = nullptr;
    pipeline[0] = PIs[0]->CreatePipeline("graphics");
    check_vk_result(vkDeviceWaitIdle(Device),
                    "Fucking Wait Failed");
}

void Initializer::Create_CommandPool() {
    BI->CmdPool_C_Info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    BI->CmdPool_C_Info.queueFamilyIndex = BI->Queue_C_Infos[0].queueFamilyIndex;
    check_vk_result(vkCreateCommandPool(Device, &BI->CmdPool_C_Info, NULL, &CmdPool),
                    "CANNOT Create Command Pool");
}

void Initializer::Create_CommandBuffer() {
    BI->CmdBuffer_A_Info.commandBufferCount = BI->Swapchain_C_Info.minImageCount;
    BI->CmdBuffer_A_Info.commandPool = CmdPool;
    BI->CmdBuffer_A_Info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    CmdBuffers.push_back(nullptr);
    check_vk_result(vkAllocateCommandBuffers(Device, &BI->CmdBuffer_A_Info, &(*(CmdBuffers.end() - 1))),
                    "CANNOT Allocate Command Buffers");
    for (uint32_t i = 0; i < BI->Swapchain_C_Info.minImageCount; ++i) {
        Fences.push_back(nullptr);
        check_vk_result(vkCreateFence(Device, &BI->Fence_C_Info, NULL, &(*(Fences.end() - 1))),
                        "CANNOT Create Fence");
    }

}

Initializer::Initializer(BackgroundCheck *bi, PipelineResourceSetup **pis, int pipeline_count) : BI(bi), PIs(pis) {
    BI->setInitializer(this);
    for (int i = 0; i < pipeline_count; ++i) {
        PIs[i]->setInitializer(this);
    }
    pipeline_obj_count = pipeline_count;
    for (auto & swapchain_image : swapchain_images) {
        swapchain_image = VK_NULL_HANDLE;
    }
}

GLFWwindow *Initializer::getWindowGlfw() const {
    return window_glfw;
}

Initializer::~Initializer() {
    if (Instance && Device) {
        check_vk_result(vkDeviceWaitIdle(Device),
                        "Fucking Wait Failed");
        vkFreeCommandBuffers(Device, CmdPool, CmdBuffers.size(), CmdBuffers.data());
        vkDestroyCommandPool(Device,CmdPool,NULL);
        for (int k = 0; k < pipeline_obj_count; ++k) {
            PIs[k]->~PipelineResourceSetup();
        }
        for (auto & i : pipeline) {
            vkDestroyPipeline(Device, i, NULL);
        }
        for (auto & acquire_Semaphore : acquire_Semaphores) {
            vkDestroySemaphore(Device, acquire_Semaphore,NULL);
        }
        for (auto & complete_Semaphore : complete_Semaphores) {
            vkDestroySemaphore(Device, complete_Semaphore,NULL);
        }
        vkDestroySwapchainKHR(Device, Swapchain, NULL);
        for (auto & Fence : Fences) {
            vkDestroyFence(Device,Fence,NULL);
        }
        vkDestroySurfaceKHR(Instance, Surface, NULL);
        vkDestroyDevice(Device, NULL);
        Device = nullptr;
        BackgroundCheck::DestroyDebugReportCallbackEXT(Instance, BI->d_callback,NULL);
        vkDestroyInstance(Instance, NULL);
        Instance = nullptr;
        glfwDestroyWindow(window_glfw);
        glfwTerminate();
    }
}

void Initializer::loadShadermodule(const char *path) {
    std::ifstream file;
    file.open(path, std::ios::in |std::ios::binary);
    long length = 0;
    char* buffer = nullptr;
    length = file.is_open();
    if(file.is_open()){
        file.seekg(0, std::ios::end);
        length = file.tellg();
        file.seekg(0, std::ios::beg);
        buffer = (char *)malloc(length * sizeof(char));
        file.read(buffer, length);
        file.close();
    }

    VkShaderModuleCreateInfo createInfo = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    createInfo.codeSize = length;
    createInfo.pCode    = reinterpret_cast<const uint32_t*>(buffer);
    shaderModules.push_back(nullptr);
    check_vk_result(vkCreateShaderModule(Device, &createInfo, NULL, &*(shaderModules.end() - 1)),
                    "Shader Module Creation");
    ifree(buffer);
}
void Initializer::Prepeare_Swapchain_Recreate() {
    BI->Swapchain_C_Info.imageExtent.width  = width;
    BI->Swapchain_C_Info.imageExtent.height = height;
    BI->Swapchain_C_Info.oldSwapchain       = Swapchain;
}


