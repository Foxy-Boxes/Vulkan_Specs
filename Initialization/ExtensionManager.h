//
// Created by cacoethes on 8/19/20.
//

#ifndef VULKAN_SPECS_EXTENSIONMANAGER_H
#define VULKAN_SPECS_EXTENSIONMANAGER_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

// Forward Declarations
class InstanceCreateInfoFiller;
class DeviceCreateInfoFiller;

namespace {
    typedef char extension_name_t[VK_MAX_EXTENSION_NAME_SIZE];
    typedef struct ExtensionNameContainer {
        const char *p_extensionNames[12];
        extension_name_t extensionNames[12];
        uint32_t extensionCount;
    } ExtensionNameContainer;
}

class ExtensionManager {
protected:
    ExtensionNameContainer nameContainer{};
public:
    virtual ~ExtensionManager() = default;

    virtual void Add_Extension(const extension_name_t PextensionName);

    virtual void Check_Extensions() = 0;
};
class InstanceExtensionManager : public ExtensionManager {
public:
    ~InstanceExtensionManager() override = default;

    InstanceExtensionManager();

    void Check_Extensions() override;

    friend class InstanceCreateInfoFiller;

    void Add_Extension_List(char c);
};

class DeviceExtensionManager : public ExtensionManager {
private:
    VkPhysicalDevice physicalDevice;
public:
    ~DeviceExtensionManager() override = default;

    DeviceExtensionManager(VkPhysicalDevice PphysicalDevice);

    void Check_Extensions() override;

    friend class DeviceCreateInfoFiller;
};

#endif //VULKAN_SPECS_EXTENSIONMANAGER_H
