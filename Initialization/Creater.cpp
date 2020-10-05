//
// Created by cacoethes on 8/19/20.
//

#include "Creater.h"
#include "../global_inline.h"

InstanceCreater::InstanceCreater(Filler *Pfiller) {
    filler = Pfiller;
    applicationInfoFiller = dynamic_cast<ApplicationInfoFiller *>(filler);
    instanceCreateInfoFiller = dynamic_cast<InstanceCreateInfoFiller *>(filler + 1);
}

void InstanceCreater::Create() {
    applicationInfoFiller->Fill_Structure();
    InstanceExtensionManager extensionManager = instanceCreateInfoFiller->getExtensionManager();
    InstanceLayerManager layerManager = instanceCreateInfoFiller->getLayerManager();
#if defined(DEBUG)
    layerManager.Add_Layer("VK_LAYER_KHRONOS_validation");
    extensionManager.Add_Extension_List('d');
#endif
#ifdef GLFW_INCLUDE_VULKAN
    extensionManager.Add_Extension_List('g');
#endif
#ifndef GLFW_INCLUDE_VULKAN
    extensionManager.Add_Extension(VK_KHR_SURFACE_EXTENSION_NAME);
#endif
    instanceCreateInfoFiller->Fill_Structure();
    check_vk_result(vkCreateInstance(instanceCreateInfoFiller->Get_Structure().instanceCreateInfo, nullptr, &vulkanConstructs.instance),
                    "CANNOT Create Instance");
#if defined(DEBUG)
    //setupDebugCallbacks.
#endif
}

DeviceCreater::DeviceCreater(Filler *Pfiller) {
    filler = Pfiller;
    queueCreateInfoFiller  = dynamic_cast<DeviceQueueCreateInfoFiller *>(filler);
    deviceCreateInfoFiller = dynamic_cast<DeviceCreateInfoFiller *>(filler + 1);
}

void DeviceCreater::Create() {
    queueCreateInfoFiller -> Fill_Structure();
    DeviceExtensionManager extensionManager = deviceCreateInfoFiller -> getExtensionManager();
    extensionManager.Add_Extension("VK_KHR_SWAPCHAIN_EXTENSION_NAME");
    extensionManager.Check_Extensions();
    check_vk_result(vkCreateDevice(vulkanConstructs.physicalDevice, deviceCreateInfoFiller -> Get_Structure().deviceCreateInfo, NULL, &(vulkanConstructs.device)),
                    "CANNOT Create Device");
}

QueueCreater::QueueCreater(Filler *Pfiller) {
    filler = Pfiller;
    queueCreateInfoFiller = dynamic_cast<DeviceQueueCreateInfoFiller *>(filler);
}

void QueueCreater::Create() {
    auto queueCreateInfos = queueCreateInfoFiller->Get_Structure().deviceQueueCreateInfos;
    vulkanConstructs.queues = new VkQueue[queueCreateInfoFiller->Get_Queue_Count()];
    for (uint32_t i = 0; i < queueCreateInfoFiller->Get_Queue_Count(); ++i) {
        for (int j = 0; j < queueCreateInfos[i]->queueCount; ++j) {
            vkGetDeviceQueue(vulkanConstructs.device, queueCreateInfos[i]->queueFamilyIndex, j, &vulkanConstructs.queues[i]);
        }
    }
}

void PhysicalDeviceCreater::Create() {
    uint32_t device_count = 0;
    check_vk_result(vkEnumeratePhysicalDevices(vulkanConstructs.instance, &device_count, NULL),
                    "Enumeration Failed");
    check_vk_count(device_count,
                   "Physical Devices");
    VkPhysicalDevice PhDevices[device_count];
    check_vk_result(vkEnumeratePhysicalDevices(vulkanConstructs.instance, &device_count, PhDevices),
                    "Problem Retrieving Physical Devices");
    VkPhysicalDeviceProperties deviceProperties[device_count];
    for (uint32_t i = 0; i < device_count; ++i) {
        vkGetPhysicalDeviceProperties(PhDevices[i], deviceProperties);
        if (deviceProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            std::cout << "Detected GPU : " << deviceProperties[i].deviceName << std::endl;
            vulkanConstructs.physicalDevice = PhDevices[i];
            break;
        }
    }
    if (!vulkanConstructs.physicalDevice && device_count) {
        std::cout << "No Gpu Available Falling-back to a : " << DeviceTypeStr(deviceProperties[0].deviceType) <<
                  ", which has the name : " << deviceProperties[0].deviceName << std::endl;
        vulkanConstructs.physicalDevice = PhDevices[0];
    }
}

void SurfaceCreater::Check_Surface_Support(VkDeviceQueueCreateInfo *PgraphicsQueueInfo) {
    VkBool32 Surface_support;
    check_vk_result(vkGetPhysicalDeviceSurfaceSupportKHR(vulkanConstructs.physicalDevice, PgraphicsQueueInfo->queueFamilyIndex, vulkanConstructs.displayConstructs.surfaceKhr, &Surface_support),
                    "Physical Device Surface Support");
    if (Surface_support != VK_TRUE){
        throw std::runtime_error("Surface is not Supported\n");
    }
}

void SurfaceCreater::Create() {
    check_vk_result(glfwCreateWindowSurface(vulkanConstructs.instance, vulkanConstructs.displayConstructs.window, nullptr, &vulkanConstructs.displayConstructs.surfaceKhr),
                    "CANNOT Create Surface");
}

void WindowCreater::Set_Window_Props(const char *PdisplayName, uint32_t width, uint32_t height) {
    displayName = PdisplayName;
    vulkanConstructs.displayConstructs.width  = width;
    vulkanConstructs.displayConstructs.height = height;
}

void WindowCreater::Create() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    vulkanConstructs.displayConstructs.window = glfwCreateWindow(vulkanConstructs.displayConstructs.width,vulkanConstructs.displayConstructs.height, displayName, NULL, NULL);
}

SwapchainCreater::SwapchainCreater(Filler *Pfiller) {
    filler = Pfiller;
    swapchainCreateInfoFiller = dynamic_cast<SwapchainCreateInfoFiller *>(filler);
}

void SwapchainCreater::Create() {
    swapchainCreateInfoFiller->Fill_Structure();
    check_vk_result(vkCreateSwapchainKHR(vulkanConstructs.device, swapchainCreateInfoFiller->Get_Structure().swapchainCreateInfoKhr, nullptr, &vulkanConstructs.displayConstructs.swapchainKhr),
                    "CANNOT Create Swapchain");
}

void SemaphoreCreater::Create() {


}
