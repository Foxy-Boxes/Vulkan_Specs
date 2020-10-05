//
// Created by cacoethes on 8/18/20.
//

#include <cstring>
#include "Filler.h"
#include "../global_inline.h"


void ApplicationInfoFiller::Fill_Structure() {
    vulkanStructures.applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vulkanStructures.applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    vulkanStructures.applicationInfo.pEngineName   = "Sigma-Vk Rendering Engine";
}

ApplicationInfoFiller::ApplicationInfoFiller(const char *PappName, float PappVersion, float PvulkanAPIVersion) {
    vulkanStructures.applicationInfo = {};
    vulkanStructures.applicationInfo.apiVersion =
            ((PvulkanAPIVersion == 1.0f) * (VK_API_VERSION_1_0))
            + ((PvulkanAPIVersion == 1.1f) * (VK_API_VERSION_1_1))
            + ((PvulkanAPIVersion == 1.2f) * (VK_API_VERSION_1_2));
    vulkanStructures.applicationInfo.pApplicationName   = PappName;
    uint32_t full, major, minor, patch;
    full = (uint32_t)(PappVersion * 1000);
    major = full/1000;
    minor = (full - major)/100;
    patch = (((full - major - minor)%10 > 8) + (full - major - minor)/10);
    vulkanStructures.applicationInfo.applicationVersion = VK_MAKE_VERSION(major, minor, patch);

}

const VulkanStructure &ApplicationInfoFiller::Get_Structure() const {
    vulkanStructure.applicationInfo = &vulkanStructures.applicationInfo;
    return vulkanStructure;
}

InstanceCreateInfoFiller::InstanceCreateInfoFiller(InstanceExtensionManager &PextensionManager,
                                                   InstanceLayerManager &PlayerManager) {
    layerManager = PlayerManager;
    extensionManager = PextensionManager;
}

void InstanceCreateInfoFiller::Fill_Structure() {
    vulkanStructures.instanceCreateInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    vulkanStructures.instanceCreateInfo.pApplicationInfo = &(vulkanStructures.applicationInfo);
    extensionManager.Check_Extensions();
    vulkanStructures.instanceCreateInfo.enabledExtensionCount = extensionManager.nameContainer.extensionCount;
    vulkanStructures.instanceCreateInfo.ppEnabledExtensionNames = extensionManager.nameContainer.p_extensionNames;
    layerManager.Check_Layers();
    vulkanStructures.instanceCreateInfo.enabledLayerCount = layerManager.nameContainer.layerCount;
    vulkanStructures.instanceCreateInfo.ppEnabledLayerNames = layerManager.nameContainer.p_layerNames;
}

const VulkanStructure &InstanceCreateInfoFiller::Get_Structure() const {
    vulkanStructure.instanceCreateInfo = &vulkanStructures.instanceCreateInfo;
    return vulkanStructure;
}

const InstanceExtensionManager &InstanceCreateInfoFiller::getExtensionManager() const {
    return extensionManager;
}

const InstanceLayerManager &InstanceCreateInfoFiller::getLayerManager() const {
    return layerManager;
}

DeviceQueueCreateInfoFiller::DeviceQueueCreateInfoFiller(VulkanConstructs *Pconstructs) {
    vulkanConstructs = Pconstructs;
}

void DeviceQueueCreateInfoFiller::Fill_Structure() {
    Setup_Properties();
    Setup_Priorities();
    for (int i = 0; i < vulkanStructures.queueCreateInfoCount; ++i) {
        vulkanStructures.deviceQueueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    }
}

void DeviceQueueCreateInfoFiller::Setup_Properties() {
    uint32_t family_props_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties2(vulkanConstructs -> physicalDevice, &family_props_count, NULL);
    check_vk_count(family_props_count,
                   "Device Queue Family Prop");
    VkQueueFamilyProperties Q_Family_Props[family_props_count];
    vkGetPhysicalDeviceQueueFamilyProperties(vulkanConstructs -> physicalDevice, &family_props_count, Q_Family_Props);
    uint32_t *queueFamilyIndices = new uint32_t [family_props_count];
    VkDeviceQueueCreateInfo* queueCreateInfos = new VkDeviceQueueCreateInfo [family_props_count];
    uint8_t seperate_queue_count = 0;
    uint8_t graphicsQueueIndex, transferQueueIndex, computeQueueIndex;
    auto setup = [&](){
        vulkanStructures.queueFamilyIndices = new uint32_t [seperate_queue_count];
        vulkanStructures.deviceQueueCreateInfos = new VkDeviceQueueCreateInfo [seperate_queue_count];
        vulkanStructures.queueCreateInfoCount = seperate_queue_count;
        for (int i = 0; i < seperate_queue_count; ++i) {
            vulkanStructures.queueFamilyIndices[i] = queueFamilyIndices[i];
            vulkanStructures.deviceQueueCreateInfos[i] = queueCreateInfos[i];
        }
        vulkanStructures.queueStructureHandles.graphicsCreateInfo = vulkanStructures.deviceQueueCreateInfos + graphicsQueueIndex;
        vulkanStructures.queueStructureHandles.transferCreateInfo = vulkanStructures.deviceQueueCreateInfos + transferQueueIndex;
        vulkanStructures.queueStructureHandles.computeCreateInfo  = vulkanStructures.deviceQueueCreateInfos + computeQueueIndex;
        vulkanConstructs->queueObjectIndices.graphicsQueueIndex = graphicsQueueIndex;
        vulkanConstructs->queueObjectIndices.transferQueueIndex = transferQueueIndex;
        vulkanConstructs->queueObjectIndices.computeQueueIndex  = computeQueueIndex;
        delete[] queueFamilyIndices;
        delete[] queueCreateInfos;
    };
    uint8_t flag = VK_QUEUE_TRANSFER_BIT | VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, suitable = 0;
    bool found = false;
    for (uint32_t i = 0; i < family_props_count; i++) {
        ((Q_Family_Props[i].queueFlags | flag) == Q_Family_Props[i].queueFlags && (found = seperate_queue_count = 1)
         && (queueFamilyIndices[0] = queueCreateInfos[0].queueFamilyIndex = i));
        suitable |= Q_Family_Props[i].queueFlags;
        if (found) {
            graphicsQueueIndex = transferQueueIndex = computeQueueIndex = 0;
            queueCreateInfos[0].queueCount = Q_Family_Props[i].queueCount;
            break;
        }
    }
    if ((suitable & flag) != flag){
        std::cerr << "Your Queue Families Do not meet the needs of this application" << std::endl
                  << "Try Installing Another Driver" << std::endl;
        return;
    }
    if(!found){
        uint8_t complete = 0;
        for (uint32_t i = 0; i < family_props_count; i++) {
            ((Q_Family_Props[i].queueFlags | flag) == (Q_Family_Props[i].queueFlags | VK_QUEUE_COMPUTE_BIT) &&
             (seperate_queue_count = 2) && (complete |= 1) && ((queueCreateInfos[0].queueCount = Q_Family_Props[i].queueCount) + 1)
             && (queueFamilyIndices[0] = queueCreateInfos[0].queueFamilyIndex = i));
            (Q_Family_Props[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && ((queueCreateInfos[1].queueCount = Q_Family_Props[i].queueCount) + 1) &&
            (complete |= 2) && (queueFamilyIndices[1] = queueCreateInfos[1].queueFamilyIndex = i);
        }
        if (complete == 3){
            graphicsQueueIndex = transferQueueIndex = 0;
            computeQueueIndex  = 1;
            setup();
            return;
        }
        complete = 0;
        for (uint32_t i = 0; i < family_props_count; i++) {
            ((Q_Family_Props[i].queueFlags | flag) == (Q_Family_Props[i].queueFlags | VK_QUEUE_TRANSFER_BIT) &&
             (seperate_queue_count = 2) && (complete |= 1) && ((queueCreateInfos[0].queueCount = Q_Family_Props[i].queueCount) + 1)
             && (queueFamilyIndices[0] = queueCreateInfos[0].queueFamilyIndex = i));
            (Q_Family_Props[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && ((queueCreateInfos[1].queueCount = Q_Family_Props[i].queueCount) + 1) &&
            (complete |= 2) && (queueFamilyIndices[1] = queueCreateInfos[1].queueFamilyIndex = i);
        }
        if (complete == 3){
            graphicsQueueIndex = computeQueueIndex = 0;
            transferQueueIndex = 1;
            setup();
            return;
        }
        complete = 0;
        for (uint32_t i = 0; i < family_props_count; i++) {
            ((Q_Family_Props[i].queueFlags | flag) == (Q_Family_Props[i].queueFlags | VK_QUEUE_GRAPHICS_BIT) &&
             (seperate_queue_count = 2) && (complete |= 1) && ((queueCreateInfos[0].queueCount = Q_Family_Props[i].queueCount) + 1)
             && (queueFamilyIndices[0] = queueCreateInfos[0].queueFamilyIndex = i));
            (Q_Family_Props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && ((queueCreateInfos[1].queueCount = Q_Family_Props[i].queueCount) + 1) &&
            (complete |= 2) && (queueFamilyIndices[1] = queueCreateInfos[1].queueFamilyIndex = i);
        }
        if (complete == 3){
            transferQueueIndex = computeQueueIndex = 0;
            graphicsQueueIndex = 1;
            setup();
            return;
        }
        if (seperate_queue_count != 2){
            seperate_queue_count = 3;
            for (uint32_t i = 0; i < family_props_count; i++) {
                (Q_Family_Props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && ((queueCreateInfos[0].queueCount = Q_Family_Props[i].queueCount) + 1) && (queueFamilyIndices[0] = queueCreateInfos[0].queueFamilyIndex = i);
                (Q_Family_Props[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && ((queueCreateInfos[1].queueCount = Q_Family_Props[i].queueCount) + 1) && (queueFamilyIndices[1] = queueCreateInfos[1].queueFamilyIndex = i);
                (Q_Family_Props[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && ((queueCreateInfos[2].queueCount = Q_Family_Props[i].queueCount) + 1) && (queueFamilyIndices[2] = queueCreateInfos[2].queueFamilyIndex = i);
            }
            graphicsQueueIndex = 0, transferQueueIndex = 1, computeQueueIndex = 2;
        }
    }
    setup();
}

void DeviceQueueCreateInfoFiller::Setup_Priorities() {
    auto& queueCreateInfos = vulkanStructures.deviceQueueCreateInfos;
    if (vulkanStructures.queueCreateInfoCount == 3){
        vulkanStructures.queuePriorities = new float*[3];
        vulkanStructures.queuePriorities[0] = new float [queueCreateInfos[0].queueCount + queueCreateInfos[1].queueCount + queueCreateInfos[2].queueCount];
        vulkanStructures.queuePriorities[1] = vulkanStructures.queuePriorities[0] + queueCreateInfos[0].queueCount;
        vulkanStructures.queuePriorities[2] = vulkanStructures.queuePriorities[1] + queueCreateInfos[1].queueCount;
        for (uint8_t j = 0; j < 3; ++j){
            for (uint32_t i = 0; i < queueCreateInfos[j].queueCount; ++i){
                vulkanStructures.queuePriorities[j][i] = 1.0f - j * 0.1f;
            }
        }
        queueCreateInfos[0].pQueuePriorities = vulkanStructures.queuePriorities[0];
        queueCreateInfos[1].pQueuePriorities = vulkanStructures.queuePriorities[1];
        queueCreateInfos[2].pQueuePriorities = vulkanStructures.queuePriorities[2];
    }
    else if (vulkanStructures.queueCreateInfoCount == 2){
        vulkanStructures.queuePriorities = new float*[2];
        vulkanStructures.queuePriorities[0] = new float [queueCreateInfos[0].queueCount + queueCreateInfos[1].queueCount];
        vulkanStructures.queuePriorities[1] = vulkanStructures.queuePriorities[0] + queueCreateInfos[0].queueCount;
        for (uint8_t j = 0; j < 2; ++j){
            for (uint32_t i = 0; i < queueCreateInfos[j].queueCount; ++i){
                vulkanStructures.queuePriorities[j][i] = 1.0f - j * 0.1f;
            }
        }
        queueCreateInfos[0].pQueuePriorities = vulkanStructures.queuePriorities[0];
        queueCreateInfos[1].pQueuePriorities = vulkanStructures.queuePriorities[1];
    }
    else {
        vulkanStructures.queuePriorities = new float*[1];
        vulkanStructures.queuePriorities[0] = new float [queueCreateInfos[0].queueCount];
        for (uint32_t i = 0; i < queueCreateInfos[0].queueCount; ++i){
            vulkanStructures.queuePriorities[0][i] = 1.0f;
        }
        queueCreateInfos[0].pQueuePriorities = vulkanStructures.queuePriorities[0];
    }
}

 const VulkanStructure &DeviceQueueCreateInfoFiller::Get_Structure() const {
    vulkanStructure.deviceQueueCreateInfos = &vulkanStructures.deviceQueueCreateInfos;
    return vulkanStructure;
}

uint32_t DeviceQueueCreateInfoFiller::Get_Queue_Count() const {
    return vulkanStructures.queueCreateInfoCount;

}

DeviceCreateInfoFiller::DeviceCreateInfoFiller(VulkanConstructs *Pconstructs,
                                               DeviceExtensionManager &extensionManager,
                                               DeviceLayerManager &layerManager)
        : extensionManager(extensionManager), layerManager(layerManager) {
    vulkanConstructs = Pconstructs;
}

void DeviceCreateInfoFiller::Fill_Structure() {
    vulkanStructures.deviceCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    vulkanStructures.deviceCreateInfo.pQueueCreateInfos = vulkanStructures.deviceQueueCreateInfos;
    vulkanStructures.deviceCreateInfo.queueCreateInfoCount = vulkanStructures.queueCreateInfoCount;
    vulkanStructures.deviceCreateInfo.enabledLayerCount = layerManager.nameContainer.layerCount;
    vulkanStructures.deviceCreateInfo.ppEnabledLayerNames = layerManager.nameContainer.p_layerNames;
    vulkanStructures.deviceCreateInfo.enabledExtensionCount = extensionManager.nameContainer.extensionCount;
    vulkanStructures.deviceCreateInfo.ppEnabledExtensionNames = extensionManager.nameContainer.p_extensionNames;
}

const VulkanStructure &DeviceCreateInfoFiller::Get_Structure() const {
    vulkanStructure.deviceCreateInfo = &vulkanStructures.deviceCreateInfo;
    return vulkanStructure;
}

const DeviceExtensionManager &DeviceCreateInfoFiller::getExtensionManager() const {
    return extensionManager;
}

const DeviceLayerManager &DeviceCreateInfoFiller::getLayerManager() const {
    return layerManager;
}

void SwapchainCreateInfoFiller::Fill_Structure() {
    VkSurfaceCapabilitiesKHR capabilitiesKhr = {};
    check_vk_result(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkanConstructs->physicalDevice, vulkanConstructs->displayConstructs.surfaceKhr, &capabilitiesKhr),
                    "Getting Capabilities Failed");
    if (capabilitiesKhr.maxImageCount){
        vulkanStructures.swapchainCreateInfoKhr.minImageCount = Min(capabilitiesKhr.maxImageCount,3);
    } else {
        vulkanStructures.swapchainCreateInfoKhr.minImageCount = Max(capabilitiesKhr.minImageCount,3);
    }
    VkCompositeAlphaFlagBitsKHR compAlphaFlag;
    compAlphaFlag = static_cast<VkCompositeAlphaFlagBitsKHR>(
            (capabilitiesKhr.supportedCompositeAlpha &
             ~(capabilitiesKhr.supportedCompositeAlpha << 1)));
    vulkanStructures.swapchainCreateInfoKhr.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    vulkanStructures.swapchainCreateInfoKhr.compositeAlpha = compAlphaFlag;
    vulkanStructures.swapchainCreateInfoKhr.queueFamilyIndexCount = vulkanStructures.queueCreateInfoCount;
    vulkanStructures.swapchainCreateInfoKhr.pQueueFamilyIndices = vulkanStructures.queueFamilyIndices;

    VkSurfaceFormatKHR surfaceFormatKhr = {VK_FORMAT_A8B8G8R8_UNORM_PACK32, VK_COLORSPACE_SRGB_NONLINEAR_KHR};
    VkPresentModeKHR   presentModeKhr   = VK_PRESENT_MODE_MAILBOX_KHR;
    uint32_t s_format_count;
    check_vk_result(vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanConstructs->physicalDevice, vulkanConstructs->displayConstructs.surfaceKhr, &s_format_count, NULL),
                    "Retrieving Surface Format Count Failed");
    check_vk_count(s_format_count,
                   "Surface Formats");
    VkSurfaceFormatKHR available_formats[s_format_count];
    check_vk_result(vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanConstructs->physicalDevice, vulkanConstructs->displayConstructs.surfaceKhr, &s_format_count, available_formats),
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
                || available_formats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR){
                surfaceFormatKhr.format = available_formats[i].format;
                surfaceFormatKhr.colorSpace = available_formats[i].colorSpace;
                break;
            }
        }
    }
    if (s_format_count == 1 && available_formats[0].format == VK_FORMAT_UNDEFINED){
        surfaceFormatKhr.format = VK_FORMAT_A8B8G8R8_UNORM_PACK32;
    }
    find_preffered = false;
    uint32_t p_mode_count;
    check_vk_result(vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanConstructs->physicalDevice, vulkanConstructs->displayConstructs.surfaceKhr, &p_mode_count, NULL),
                    "Retrieving Present Mode Count Failed");
    check_vk_count(p_mode_count,
                   "Present Modes");
    VkPresentModeKHR available_modes[p_mode_count];
    check_vk_result(vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanConstructs->physicalDevice, vulkanConstructs->displayConstructs.surfaceKhr, &p_mode_count, available_modes),
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
                presentModeKhr = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
                break;
            }
            else if (available_modes[j] == VK_PRESENT_MODE_FIFO_KHR){
                presentModeKhr = VK_PRESENT_MODE_FIFO_KHR;
            }
        }
        if (presentModeKhr == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentModeKhr = available_modes[0];
        }
    }

    vulkanStructures.swapchainCreateInfoKhr.surface = vulkanConstructs->displayConstructs.surfaceKhr;
    vulkanStructures.swapchainCreateInfoKhr.imageFormat = surfaceFormatKhr.format;
    vulkanStructures.swapchainCreateInfoKhr.imageColorSpace = surfaceFormatKhr.colorSpace;
    vulkanStructures.swapchainCreateInfoKhr.presentMode = presentModeKhr;
    vulkanStructures.swapchainCreateInfoKhr.imageExtent.width = vulkanConstructs->displayConstructs.width;
    vulkanStructures.swapchainCreateInfoKhr.imageExtent.height = vulkanConstructs->displayConstructs.height;
    vulkanStructures.swapchainCreateInfoKhr.imageArrayLayers = 1;
    vulkanStructures.swapchainCreateInfoKhr.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
}

const VulkanStructure &SwapchainCreateInfoFiller::Get_Structure() const {
    vulkanStructure.swapchainCreateInfoKhr = &vulkanStructures.swapchainCreateInfoKhr;
    return vulkanStructure;
}

void SemaphoreCreateInfoFiller::Fill_Structure() {
    vulkanStructures.semaphoreInfo.createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    vulkanStructures.semaphoreInfo.purpose = "generic";
    vulkanStructures.semaphoreInfo.purpose_id = -1;
}

void SemaphoreCreateInfoFiller::Fill_Structure(const char *Ppurpose, int PpurposeId) {
    vulkanStructures.semaphoreInfo.createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    vulkanStructures.semaphoreInfo.purpose = Ppurpose;
    vulkanStructures.semaphoreInfo.purpose_id = PpurposeId;
}

const VulkanStructure &SemaphoreCreateInfoFiller::Get_Structure() const {
    vulkanStructure.semaphoreInfo = &vulkanStructures.semaphoreInfo;
    return vulkanStructure;
}
