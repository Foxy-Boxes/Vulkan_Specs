//
// Created by cacoethes on 8/19/20.
//

#include "LayerManager.h"
#include "../global_inline.h"

void LayerManager::Add_Layer(const char *PlayerName) {
    strcpy(nameContainer.layerNames[nameContainer.layerCount++], PlayerName);
    nameContainer.p_layerNames[nameContainer.layerCount - 1] = (const char*)nameContainer.layerNames + nameContainer.layerCount -1;
}

InstanceLayerManager::InstanceLayerManager() {
    nameContainer.layerCount = 0;
}

void InstanceLayerManager::Check_Layers() {
    uint32_t i_layer_prop_count;
    if (!check_vk_available<VkLayerProperties, layer_name_t>
            (&i_layer_prop_count, (&vkEnumerateInstanceLayerProperties),
             "Instance Layer", &VkLayerProperties::layerName, nameContainer.layerNames,
             nameContainer.layerCount, true,sizeof("Instance Layer"))){
        std::cerr << "You have missing Instance Layers" << std::endl;
    }
}

DeviceLayerManager::DeviceLayerManager(VkPhysicalDevice PpyhsicalDevice) {
    physicalDevice = PpyhsicalDevice;
    nameContainer.layerCount = 0;
}

void DeviceLayerManager::Check_Layers() {
    uint32_t d_layer_prop_count;
    if (!check_vk_available<VkPhysicalDevice, VkLayerProperties, layer_name_t>
            (&d_layer_prop_count, (&vkEnumerateDeviceLayerProperties),
             "Device Layer", physicalDevice, &VkLayerProperties::layerName,
             nameContainer.layerNames, nameContainer.layerCount, true,sizeof("Device Layer"))){
        std::cerr << "You have missing Device Layers" << std::endl;
    }
}
