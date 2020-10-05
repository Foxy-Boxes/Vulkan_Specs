//
// Created by cacoethes on 8/18/20.
//

#ifndef VULKAN_SPECS_FILLER_H
#define VULKAN_SPECS_FILLER_H

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "../build_configs.h"
#include "Types.h"
#include "LayerManager.h"
#include "ExtensionManager.h"


class Filler {
protected:
    inline static VulkanStructure vulkanStructure{};
    inline static SpecifiedVulkanStructures vulkanStructures{};
    VulkanConstructs *vulkanConstructs;
public:
    virtual ~Filler() = default;

    virtual void Fill_Structure() = 0;

    virtual const VulkanStructure& Get_Structure() const = 0;
};

class ApplicationInfoFiller : public Filler {
public:

    ApplicationInfoFiller(const char *PappName, float PappVersion, float PvulkanAPIVersion);

    ~ApplicationInfoFiller() override = 0;

    void Fill_Structure() override;

    const VulkanStructure& Get_Structure() const override;
};

class InstanceCreateInfoFiller : public Filler {
private:
    InstanceExtensionManager extensionManager;
    InstanceLayerManager layerManager;
public:
    InstanceCreateInfoFiller(InstanceExtensionManager &PextensionManager, InstanceLayerManager &PlayerManager);

    ~InstanceCreateInfoFiller() override = 0;

    void Fill_Structure() override;

    const VulkanStructure& Get_Structure() const override;

    const InstanceExtensionManager &getExtensionManager() const;

    const InstanceLayerManager &getLayerManager() const;

};

class DeviceQueueCreateInfoFiller : public Filler {
private:
    void Setup_Properties();

    void Setup_Priorities();
public:
    DeviceQueueCreateInfoFiller(VulkanConstructs *Pconstructs);

    ~DeviceQueueCreateInfoFiller() override = 0;

    void Fill_Structure() override;

    const VulkanStructure &Get_Structure() const override;

    uint32_t Get_Queue_Count() const;
};

class DeviceCreateInfoFiller : public Filler {
private:
    DeviceExtensionManager extensionManager;
    DeviceLayerManager layerManager;
public:
    DeviceCreateInfoFiller(VulkanConstructs *Pconstructs,
                           DeviceExtensionManager& extensionManager, DeviceLayerManager& layerManager);

    ~DeviceCreateInfoFiller() override = 0;

    void Fill_Structure() override;

    const VulkanStructure& Get_Structure() const override;

    const DeviceExtensionManager &getExtensionManager() const;

    const DeviceLayerManager &getLayerManager() const;
};

class SwapchainCreateInfoFiller : public Filler{
public:
    void Fill_Structure() override;

    const VulkanStructure& Get_Structure() const override;
};

class SemaphoreCreateInfoFiller : public Filler{
public:
    void Fill_Structure() override;

    void Fill_Structure(const char* Ppurpose, int PpurposeId);

    const VulkanStructure& Get_Structure() const override;
};
#endif //VULKAN_SPECS_FILLER_H
