//
// Created by cacoethes on 8/19/20.
//

#ifndef VULKAN_SPECS_LAYERMANAGER_H
#define VULKAN_SPECS_LAYERMANAGER_H

#include <vulkan/vulkan.h>

// Forward Declarations
class InstanceCreateInfoFiller;
class DeviceCreateInfoFiller;

namespace {
    typedef char layer_name_t[VK_MAX_EXTENSION_NAME_SIZE];
    typedef struct LayerNameContainer {
        const char *p_layerNames[12];
        layer_name_t layerNames[12];
        uint32_t layerCount;
    } LayerNameContainer;
}

class LayerManager {
protected:
    LayerNameContainer nameContainer;
public:
    virtual ~LayerManager() = default;

    virtual void Add_Layer(const layer_name_t PlayerName);

    virtual void Check_Layers() = 0;
};
class InstanceLayerManager : public LayerManager {
public:
    ~InstanceLayerManager() override = default;

    InstanceLayerManager();

    void Check_Layers() override;

    friend class InstanceCreateInfoFiller;
};

class DeviceLayerManager : public LayerManager {
    VkPhysicalDevice physicalDevice;
public:
    ~DeviceLayerManager() override = default;

    DeviceLayerManager(VkPhysicalDevice PphysicalDevice);

    void Check_Layers() override;

    friend class DeviceCreateInfoFiller;
};

#endif //VULKAN_SPECS_LAYERMANAGER_H
