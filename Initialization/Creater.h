//
// Created by cacoethes on 8/19/20.
//

#ifndef VULKAN_SPECS_CREATER_H
#define VULKAN_SPECS_CREATER_H

#include "../build_configs.h"
#include "Filler.h"
#include "Types.h"

class Filler;
class TrivialCreater{
protected:
    inline static VulkanConstructs vulkanConstructs{};
public:
    virtual ~TrivialCreater() = default;
    virtual void Create() = 0;
};
class Creater : public TrivialCreater {
protected:
    Filler* filler;
};
class InstanceCreater : public Creater{
private:
    ApplicationInfoFiller* applicationInfoFiller;
    InstanceCreateInfoFiller* instanceCreateInfoFiller;
public:
    InstanceCreater(Filler* Pfiller);

    void Create() override;
};

class DeviceCreater : public Creater{
private:
    DeviceQueueCreateInfoFiller* queueCreateInfoFiller;
    DeviceCreateInfoFiller* deviceCreateInfoFiller;
public:
    DeviceCreater(Filler* Pfiller);

    void Create() override;
};

class QueueCreater : public Creater{
private:
    DeviceQueueCreateInfoFiller* queueCreateInfoFiller;
public:
    QueueCreater(Filler* Pfiller);

    void Create() override;
};

class PhysicalDeviceCreater : public TrivialCreater{
public:
    PhysicalDeviceCreater() = default;

    void Create() override;
};

class WindowCreater : public TrivialCreater{
private:
    const char* displayName;
public:
    WindowCreater() = default;

    void Set_Window_Props(const char* PdisplayName, uint32_t width, uint32_t height);

    void Create() override;
};

class SurfaceCreater : public TrivialCreater{
public:
    SurfaceCreater() = default;

    void Check_Surface_Support(VkDeviceQueueCreateInfo *PgraphicsQueueInfo);

    void Create() override;
};

class SwapchainCreater : public Creater{
private:
    SwapchainCreateInfoFiller* swapchainCreateInfoFiller;
public:
    SwapchainCreater(Filler* Pfiller);

    void Create() override;
};

class SemaphoreCreater : public TrivialCreater{
public:
    SemaphoreCreater() = default;

    void Create() override;
};
class FenceCreater : public TrivialCreater{

};

#endif //VULKAN_SPECS_CREATER_H
