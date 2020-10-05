//
// Created by cacoethes on 8/4/20.
//

#include "BackgroundCheck.h"
#include "global_inline.h"
#include <cstring>
#include <iostream>


BackgroundCheck::BackgroundCheck() {
    for (int i = 0; i < 12; ++i) {
        arr[0][i] = arr[1][i] = arr[2][i] = arr[3][i] = nullptr;
    }
    int res;
    if ((res = glfwInit()) != GLFW_TRUE)
    {
        std::cerr << "GLFW returned : "<< res << std::endl;
//         Initialization failed
    }
}
void BackgroundCheck::setInitializer(Initializer * Initializer_Instance) {
    II = Initializer_Instance;
}
bool BackgroundCheck::Check_Instance_Layers() {
    uint32_t i_layer_prop_count;
    if (!check_vk_available<VkLayerProperties, layer_name_t>
            (&i_layer_prop_count, (&vkEnumerateInstanceLayerProperties),
             "Instance Layer", &VkLayerProperties::layerName, Instance_layer_names,
             Instance_layer_count, true,sizeof("Instance Layer"))){
        std::cerr << "You have missing Instance Layers" << std::endl;
        return false;
    }
    Instance_C_Info.enabledLayerCount = Instance_layer_count;
    Instance_C_Info.ppEnabledLayerNames = arr[0];
    return true;
}

bool BackgroundCheck::Check_Instance_Extensions() {
    uint32_t i_extension_prop_count;
    if (!check_vk_available<const char*, VkExtensionProperties, extension_name_t>
            (&i_extension_prop_count, (&vkEnumerateInstanceExtensionProperties),
             "Instance Extension", nullptr,
             (&VkExtensionProperties::extensionName),
             Instance_extension_names, Instance_extension_count, true, sizeof("Instance Extension"))){
        std::cout << "You are using some Instance Extension which is not available\n";
        return false;
    }
    Instance_C_Info.enabledExtensionCount = Instance_extension_count;
    Instance_C_Info.ppEnabledExtensionNames = arr[1];
    return true;
}

bool BackgroundCheck::Check_Device_Layers() {
    uint32_t d_layer_prop_count;
    if (!check_vk_available<VkPhysicalDevice, VkLayerProperties, layer_name_t>
            (&d_layer_prop_count, (&vkEnumerateDeviceLayerProperties),
            "Device Layer",II->PhDevice, &VkLayerProperties::layerName,
            Device_layer_names, Device_layer_count, true,sizeof("Device Layer"))){
        std::cerr << "You have missing Device Layers" << std::endl;
        return false;
    }
    Device_C_Info.enabledLayerCount = Device_layer_count;
    Device_C_Info.ppEnabledLayerNames = arr[2];
    return true;
}

bool BackgroundCheck::Check_Device_Extensions() {
    uint32_t d_extension_prop_count;
    if (!check_vk_available<VkPhysicalDevice, const char*, VkExtensionProperties, extension_name_t>
            (&d_extension_prop_count,(&vkEnumerateDeviceExtensionProperties),
            "Device Extension",II->PhDevice,nullptr, &VkExtensionProperties::extensionName,
            Device_extension_names, Device_extension_count, true, sizeof("Device Extension"))){
        std::cout << "You are using some Device Extension which is not available\n";
        return false;
    }
    Device_C_Info.enabledExtensionCount = Device_extension_count;
    Device_C_Info.ppEnabledExtensionNames = arr[3];
    return true;
}

void BackgroundCheck::Add_Instance_Layer(const char * string) {
    arr[0][Instance_layer_count] = string;
    strcpy(Instance_layer_names[Instance_layer_count++],string);
}

void BackgroundCheck::Add_Instance_Extension(const char * string) {
    arr[1][Instance_extension_count] = string;
    strcpy(Instance_extension_names[Instance_extension_count++],string);
}

void BackgroundCheck::Add_Device_Layer(const char * string) {
    arr[2][Device_layer_count] = string;
    strcpy(Device_layer_names[Device_layer_count++],string);
}

void BackgroundCheck::Add_Device_Extension(const char * string) {
    arr[3][Device_extension_count] = string;
    strcpy(Device_extension_names[Device_extension_count++],string);
}

void BackgroundCheck::Add_Instance_Extension_List(char c) {
    uint32_t count = 0;
    const char** p_string;
    switch (c) {
        case 'g':
            p_string = glfwGetRequiredInstanceExtensions(&count);
            for (uint32_t i = 0; i < count; ++i) {
                arr[1][Instance_extension_count] = p_string[i];
                strcpy(Instance_extension_names[Instance_extension_count++],p_string[i]);
            }
            return;
        case 'd':
            Add_Instance_Extension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            return;
    }
}
VkBool32 BackgroundCheck::debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj,
                                        size_t location, int32_t code, const char *layer_prefix, const char *msg,
                                        void *userData) {
    static const char* msgs[20];
    static int bottom = 0;
    bool found = false;
    for (int i = 0; i < 20; ++i) {
        if (msgs[i]==msg){
            found = true;
        }
    }
    if(!found) {
        msgs[bottom++] = msg;
        std::cerr << msg << std::endl;
    }
    return VK_FALSE;
}
void BackgroundCheck::DebugSetup() {
    VkDebugReportCallbackCreateInfoEXT DRC_C_Info = {VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT};
    DRC_C_Info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    DRC_C_Info.pfnCallback = debugCallback;
    if (d_callback){
        DestroyDebugReportCallbackEXT(II->Instance, d_callback, NULL);
    }
    check_vk_result(CreateDebugReportCallbackEXT(II->Instance, &DRC_C_Info, NULL, &d_callback),
                    "Debug Report Callback");
}
void BackgroundCheck::DestroyDebugReportCallbackEXT(
        VkInstance instance,
        VkDebugReportCallbackEXT callback,
        const VkAllocationCallbacks *pAllocator){
    static int time = 0;
    static PFN_vkDestroyDebugReportCallbackEXT func;
    if(!time){
        func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
        time++;
    }
    if(func){
        func(instance, callback, pAllocator);
    }
}

VkResult BackgroundCheck::CreateDebugReportCallbackEXT(
        VkInstance instance,
        const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
        const VkAllocationCallbacks *pAllocator,
        VkDebugReportCallbackEXT *pCallback) {
    static int time = 0;
    static PFN_vkCreateDebugReportCallbackEXT func;
    if (!time) {
        func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
        time++;
    }
    if (func) {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void BackgroundCheck::Setup_Queue_Properties() {
    Device_C_Info.pQueueCreateInfos = Queue_C_Infos;
    uint32_t family_props_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties2(II->PhDevice, &family_props_count, NULL);
    check_vk_count(family_props_count,
                   "Device Queue Family Prop");
    VkQueueFamilyProperties Q_Family_Props[family_props_count];
    vkGetPhysicalDeviceQueueFamilyProperties(II->PhDevice, &family_props_count, Q_Family_Props);
    seperate_queue_count = 0;
    uint8_t flag = VK_QUEUE_TRANSFER_BIT | VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, suitable = 0;
    bool found = false;
    for (uint32_t i = 0; i < family_props_count; i++) {
        ((Q_Family_Props[i].queueFlags | flag) == Q_Family_Props[i].queueFlags && (found = seperate_queue_count = 1)
        && (qFamIndices[0] = Queue_C_Infos[0].queueFamilyIndex = i));
        suitable |= Q_Family_Props[i].queueFlags;
        if (found) {
            Queue_C_Infos[0].queueCount = Q_Family_Props[i].queueCount;
            Device_C_Info.queueCreateInfoCount = seperate_queue_count;
            break;
        }
    }
    if ((suitable & flag) != flag){
        Device_C_Info.queueCreateInfoCount = seperate_queue_count;
        std::cerr << "Your Queue Families Do not meet the needs of this application" << std::endl;
        return;
    }
    if(!found){
        uint8_t complete = 0;
        for (uint32_t i = 0; i < family_props_count; i++) {
            ((Q_Family_Props[i].queueFlags | flag) == (Q_Family_Props[i].queueFlags | VK_QUEUE_COMPUTE_BIT) &&
             (seperate_queue_count = 2) && (complete |= 1) && ((Queue_C_Infos[0].queueCount = Q_Family_Props[i].queueCount) + 1)
             && (qFamIndices[0] = Queue_C_Infos[0].queueFamilyIndex = i));
            (Q_Family_Props[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && ((Queue_C_Infos[1].queueCount = Q_Family_Props[i].queueCount) + 1) &&
            (complete |= 2) && (qFamIndices[1] = Queue_C_Infos[1].queueFamilyIndex = i);
        }
        if (complete == 3){
            Device_C_Info.queueCreateInfoCount = seperate_queue_count;
            return;
        }
        for (uint32_t i = 0; i < family_props_count; i++) {
            ((Q_Family_Props[i].queueFlags | flag) == (Q_Family_Props[i].queueFlags | VK_QUEUE_TRANSFER_BIT) &&
             (seperate_queue_count = 2) && (complete |= 1) && ((Queue_C_Infos[0].queueCount = Q_Family_Props[i].queueCount) + 1)
             && (qFamIndices[0] = Queue_C_Infos[0].queueFamilyIndex = i));
            (Q_Family_Props[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && ((Queue_C_Infos[1].queueCount = Q_Family_Props[i].queueCount) + 1) &&
            (complete |= 2) && (qFamIndices[1] = Queue_C_Infos[1].queueFamilyIndex = i);
        }
        if (complete == 3){
            Device_C_Info.queueCreateInfoCount = seperate_queue_count;
            return;
        }
        for (uint32_t i = 0; i < family_props_count; i++) {
            ((Q_Family_Props[i].queueFlags | flag) == (Q_Family_Props[i].queueFlags | VK_QUEUE_GRAPHICS_BIT) &&
             (seperate_queue_count = 2) && (complete |= 1) && ((Queue_C_Infos[0].queueCount = Q_Family_Props[i].queueCount) + 1)
             && (qFamIndices[0] = Queue_C_Infos[0].queueFamilyIndex = i));
            (Q_Family_Props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && ((Queue_C_Infos[1].queueCount = Q_Family_Props[i].queueCount) + 1) &&
            (complete |= 2) && (qFamIndices[1] = Queue_C_Infos[1].queueFamilyIndex = i);
        }
        if (complete == 3){
            Device_C_Info.queueCreateInfoCount = seperate_queue_count;
            return;
        }
        if (seperate_queue_count != 2){
            seperate_queue_count = 3;
            for (uint32_t i = 0; i < family_props_count; i++) {
                (Q_Family_Props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && ((Queue_C_Infos[0].queueCount = Q_Family_Props[i].queueCount) + 1) && (qFamIndices[0] = Queue_C_Infos[0].queueFamilyIndex = i);
                (Q_Family_Props[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && ((Queue_C_Infos[1].queueCount = Q_Family_Props[i].queueCount) + 1) && (qFamIndices[1] = Queue_C_Infos[1].queueFamilyIndex = i);
                (Q_Family_Props[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && ((Queue_C_Infos[2].queueCount = Q_Family_Props[i].queueCount) + 1) && (qFamIndices[2] = Queue_C_Infos[2].queueFamilyIndex = i);
            }
            Device_C_Info.queueCreateInfoCount = seperate_queue_count;
        }
    }
}

void BackgroundCheck::Setup_Queue_Priorities() {
    if(seperate_queue_count == 3){
        Queue_Priorities[0] = (float *)malloc(sizeof(float) * (Queue_C_Infos[0].queueCount + Queue_C_Infos[1].queueCount + Queue_C_Infos[2].queueCount));
        Queue_Priorities[1] = Queue_Priorities[0] + Queue_C_Infos[0].queueCount;
        Queue_Priorities[2] = Queue_Priorities[1] + Queue_C_Infos[1].queueCount;
        for (uint8_t j = 0; j < 3; ++j){
            for (uint32_t i = 0; i < Queue_C_Infos[j].queueCount; ++i){
                Queue_Priorities[j][i] = 1.0f - j * 0.1f;
            }
        }
        Queue_C_Infos[0].pQueuePriorities = Queue_Priorities[0];
        Queue_C_Infos[1].pQueuePriorities = Queue_Priorities[1];
        Queue_C_Infos[2].pQueuePriorities = Queue_Priorities[2];
    }
    else if(seperate_queue_count == 2){
        Queue_Priorities[0] = (float *)malloc(sizeof(float) * (Queue_C_Infos[0].queueCount + Queue_C_Infos[1].queueCount));
        Queue_Priorities[1] = Queue_Priorities[0] + Queue_C_Infos[0].queueCount;
        for (uint8_t j = 0; j < 2; ++j){
            for (uint32_t i = 0; i < Queue_C_Infos[j].queueCount; ++i){
                Queue_Priorities[j][i] = 1.0f - j * 0.1f;
            }
        }
        Queue_C_Infos[0].pQueuePriorities = Queue_Priorities[0];
        Queue_C_Infos[1].pQueuePriorities = Queue_Priorities[1];
    }
    else{
        Queue_Priorities[0] = (float *)malloc(sizeof(float) * (Queue_C_Infos[0].queueCount));
        for (uint32_t i = 0; i < Queue_C_Infos[0].queueCount; ++i){
            Queue_Priorities[0][i] = 1.0f;
        }
        Queue_C_Infos[0].pQueuePriorities = Queue_Priorities[0];
    }
}
void BackgroundCheck::Setup_Device_Background() {
    Setup_Queue_Properties();
    Setup_Queue_Priorities();
    for (uint8_t j = 0; j < seperate_queue_count; ++j){
        Queue_C_Infos[j].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    }
}
BackgroundCheck::~BackgroundCheck(){
    free(Queue_Priorities[0]);
}

void BackgroundCheck::Surface_Support_Check() {
    check_vk_result(vkGetPhysicalDeviceSurfaceSupportKHR(II->PhDevice, Queue_C_Infos[0].queueFamilyIndex, II->Surface, &Surface_support),
                    "Physical Device Surface Support");
    if (Surface_support != VK_TRUE){
        throw std::runtime_error("Surface is not Supported\n");
    }
}

void BackgroundCheck::Setup_Swapchain_Background() {
    VkSurfaceCapabilitiesKHR capabilitiesKhr = {};
    check_vk_result(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(II->PhDevice, II->Surface, &capabilitiesKhr),
                    "Getting Capabilities Failed");
    if (capabilitiesKhr.maxImageCount){
        Swapchain_C_Info.minImageCount = Min(capabilitiesKhr.maxImageCount,3);
    } else {
        Swapchain_C_Info.minImageCount = Max(capabilitiesKhr.minImageCount,3);
    }
    VkCompositeAlphaFlagBitsKHR compAlphaFlag;
    compAlphaFlag = static_cast<VkCompositeAlphaFlagBitsKHR>(
            (capabilitiesKhr.supportedCompositeAlpha &
            ~(capabilitiesKhr.supportedCompositeAlpha << 1)));
    Swapchain_C_Info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    Swapchain_C_Info.compositeAlpha = compAlphaFlag;
    Swapchain_C_Info.queueFamilyIndexCount = seperate_queue_count;
    Swapchain_C_Info.pQueueFamilyIndices = qFamIndices;
    Swapchain_C_Info.presentMode = Present_mode;
}

const VkSwapchainCreateInfoKHR &BackgroundCheck::getSwapchainCInfo() const {
    return Swapchain_C_Info;
}

const VkCommandBufferBeginInfo &BackgroundCheck::getCmdBufferBInfo() const {
    return CmdBuffer_B_Info;
}

const VkPresentInfoKHR &BackgroundCheck::getPresentInfo() const {
    return Present_Info;
}
