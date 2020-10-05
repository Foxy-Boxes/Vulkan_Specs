//
// Created by cacoethes on 8/19/20.
//

#ifndef VULKAN_SPECS_TYPES_H
#define VULKAN_SPECS_TYPES_H
#include <vulkan/vulkan.h>
#include <vector>

namespace {
    typedef VkDeviceQueueCreateInfo* DeviceQueueCreateInfos;
}
typedef struct QueueObjectIndices{
    uint8_t graphicsQueueIndex;
    uint8_t transferQueueIndex;
    uint8_t computeQueueIndex;
} QueueObjectIndices;
typedef struct QueueStructureHandles{
    VkDeviceQueueCreateInfo* graphicsCreateInfo;
    VkDeviceQueueCreateInfo* transferCreateInfo;
    VkDeviceQueueCreateInfo* computeCreateInfo;
}QueueStructureHandles;
typedef struct SigmaSemaphoreInfo{
    VkSemaphoreCreateInfo createInfo;
    const char* purpose;
    int purpose_id;
};
typedef struct SigmaFenceInfo{
    VkFenceCreateInfo createInfo;
    const char* purpose;
    int purpose_id;
};
typedef struct SigmaSemaphoreVector{
    std::vector<VkSemaphore> semaphoreVector;
    const char* purpose;
    int purposeId;
}SigmaSemaphoreVector;
typedef struct SigmaFenceVector{
    std::vector<VkFence> fenceVector;
    const char* purpose;
    int purposeId;
}SigmaFenceVector;
typedef struct SynchPrimitives {
    std::vector<SigmaSemaphoreVector> semaphoreVectorVector;
    std::vector<SigmaFenceVector> fenceVectorVector;
} SynchPrimitives;
typedef struct DisplayConstructs {
    GLFWwindow* window;
    VkSurfaceKHR surfaceKhr;
    VkSwapchainKHR swapchainKhr;
    uint32_t width;
    uint32_t height;
} DisplayConstructs;
typedef struct VulkanConstructs {
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue* queues;
    QueueObjectIndices queueObjectIndices;
    DisplayConstructs displayConstructs;
    SynchPrimitives synchPrimitives;
} VulkanConstructs;
typedef struct SpecifiedVulkanStructures {
    VkApplicationInfo applicationInfo;
    VkInstanceCreateInfo instanceCreateInfo;
    VkDeviceCreateInfo deviceCreateInfo;
    VkDeviceQueueCreateInfo *deviceQueueCreateInfos;
    VkSwapchainCreateInfoKHR swapchainCreateInfoKhr;
    SigmaFenceInfo fenceInfo;
    SigmaSemaphoreInfo semaphoreInfo;
    uint32_t *queueFamilyIndices;
    float **queuePriorities;
    uint32_t queueCreateInfoCount;
    VkPresentInfoKHR presentInfoKhr;
    QueueStructureHandles queueStructureHandles;
} SpecifiedVulkanStructures;
typedef union VulkanStructure {
    VkApplicationInfo* applicationInfo;
    VkInstanceCreateInfo* instanceCreateInfo;
    VkDeviceCreateInfo* deviceCreateInfo;
    VkSwapchainCreateInfoKHR* swapchainCreateInfoKhr;
    DeviceQueueCreateInfos* deviceQueueCreateInfos;
    SigmaSemaphoreInfo* semaphoreInfo;
    SigmaFenceInfo* fenceInfo;
    uint32_t **queueFamilyIndices;
    float ***queuePriorities;
    uint32_t *queueCreateInfoCount;
    VkPresentInfoKHR *presentInfoKhr;
} VulkanStructure;
#endif //VULKAN_SPECS_TYPES_H
