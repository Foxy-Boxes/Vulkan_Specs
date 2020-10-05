//
// Created by cacoethes on 8/19/20.
//

#include "ExtensionManager.h"
#include "../global_inline.h"

void ExtensionManager::Add_Extension(const char *PextensionName) {
    strcpy(nameContainer.extensionNames[nameContainer.extensionCount++], PextensionName);
    nameContainer.p_extensionNames[nameContainer.extensionCount - 1] = (const char*)nameContainer.extensionNames + nameContainer.extensionCount -1;
}

InstanceExtensionManager::InstanceExtensionManager() {
    nameContainer.extensionCount = 0;
}

void InstanceExtensionManager::Check_Extensions() {
    uint32_t i_extension_prop_count;
    if (!check_vk_available<const char*, VkExtensionProperties, extension_name_t>
            (&i_extension_prop_count, (&vkEnumerateInstanceExtensionProperties),
             "Instance Extension", nullptr,
             (&VkExtensionProperties::extensionName),
             nameContainer.extensionNames, nameContainer.extensionCount, true, sizeof("Instance Extension"))) {
        std::cout << "You are using some Instance Extension which is not available\n";
    }
}
DeviceExtensionManager::DeviceExtensionManager(VkPhysicalDevice PphysicalDevice) {
    physicalDevice = PphysicalDevice;
    nameContainer.extensionCount = 0;
}

void DeviceExtensionManager::Check_Extensions() {
    uint32_t d_extension_prop_count;
    if (!check_vk_available<VkPhysicalDevice, const char*, VkExtensionProperties, extension_name_t>
            (&d_extension_prop_count,(&vkEnumerateDeviceExtensionProperties),
             "Device Extension",physicalDevice,nullptr, &VkExtensionProperties::extensionName,
             nameContainer.extensionNames, nameContainer.extensionCount, true, sizeof("Device Extension"))) {
        std::cout << "You are using some Device Extension which is not available\n";
    }
}
void InstanceExtensionManager::Add_Extension_List(char c) {
    uint32_t count = 0;
    const char** p_string;
    switch (c) {
        case 'g':
            p_string = glfwGetRequiredInstanceExtensions(&count);
            for (uint32_t i = 0; i < count; ++i) {
                Add_Extension(p_string[i]);
            }
            return;
        case 'd':
            Add_Extension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            return;
        default:
            std::cerr << "list initial is not known" << std::endl;
            return;
    }
}
