//
// Created by cacoethes on 8/4/20.
//

#ifndef VULKAN_SPECS_BACKGROUNDCHECK_H
#define VULKAN_SPECS_BACKGROUNDCHECK_H
#include "build_configs.h"
#include "Initializer.h"
#include <vulkan/vulkan.h>
class BackgroundCheck {
private:
    typedef char extension_name_t[VK_MAX_EXTENSION_NAME_SIZE];
    typedef extension_name_t* extname_ptr;
    typedef extension_name_t layer_name_t;
    typedef layer_name_t* layname_ptr;
    layer_name_t Instance_layer_names[12]{};
    layer_name_t Device_layer_names[12]{};
    extension_name_t Instance_extension_names[12]{};
    extension_name_t Device_extension_names[12]{};
    uint8_t Instance_layer_count = 0, Instance_extension_count = 0,
    Device_layer_count = 0, Device_extension_count = 0;
    const char *arr[4][12]{};
    VkApplicationInfo           App_Info              = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    VkInstanceCreateInfo        Instance_C_Info       = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    VkDeviceCreateInfo          Device_C_Info         = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    VkDeviceQueueCreateInfo     Queue_C_Info_Graph    = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    VkDeviceQueueCreateInfo     Queue_C_Info_Transfer = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
//    VkXcbSurfaceCreateInfoKHR   Surface_C_Info        = {VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR};
    VkSwapchainCreateInfoKHR    Swapchain_C_Info      = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
public:
    const VkSwapchainCreateInfoKHR &getSwapchainCInfo() const;

private:
    VkCommandPoolCreateInfo     CmdPool_C_Info        = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    VkCommandBufferAllocateInfo CmdBuffer_A_Info      = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBufferBeginInfo    CmdBuffer_B_Info      = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
public:
    const VkCommandBufferBeginInfo &getCmdBufferBInfo() const;

private:
    VkFenceCreateInfo           Fence_C_Info          = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    VkPresentInfoKHR            Present_Info          = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
public:
    const VkPresentInfoKHR &getPresentInfo() const;

private:
    VkSurfaceFormatKHR          Surface_format        = {VK_FORMAT_A8B8G8R8_UNORM_PACK32, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    VkPresentModeKHR            Present_mode          = {VK_PRESENT_MODE_MAILBOX_KHR};
    VkDeviceQueueCreateInfo Queue_C_Infos[3]{};
    VkBool32 Surface_support{};
    float* Queue_Priorities[3]{};
    uint32_t qFamIndices[3]{};
    uint8_t seperate_queue_count{};
    VkDebugReportCallbackEXT d_callback = nullptr;
    Initializer* II;
    friend class Initializer;
    friend class PipelineResourceSetup;
    void setInitializer(Initializer *);
public:
    BackgroundCheck();

    void Setup_Device_Background();

    void Setup_Queue_Properties();

    void Setup_Queue_Priorities();

    void Setup_Surface_Background();

    void Surface_Support_Check();

    void Setup_Swapchain_Background();

    bool Check_Instance_Layers();

    bool Check_Instance_Extensions();

    bool Check_Device_Layers();

    bool Check_Device_Extensions();

    void Add_Instance_Layer(const char*);

    void Add_Instance_Extension(const char*);

    void Add_Device_Layer(const char*);

    void Add_Device_Extension(const char*);

    void Add_Instance_Extension_List(char);

    void DebugSetup();

    static VkResult CreateDebugReportCallbackEXT(
            VkInstance instance,
            const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
            const VkAllocationCallbacks *pAllocator,
            VkDebugReportCallbackEXT *pCallback);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugReportFlagsEXT flags,
            VkDebugReportObjectTypeEXT objType,
            uint64_t obj, size_t location,
            int32_t code, const char* layer_prefix,
            const char* msg, void* userData);

    static void DestroyDebugReportCallbackEXT(
            VkInstance instance,
            VkDebugReportCallbackEXT callback,
            const VkAllocationCallbacks *pAllocator);

    ~BackgroundCheck();
};


#endif //VULKAN_SPECS_BACKGROUNDCHECK_H
