//
// Created by cacoethes on 8/10/20.
//

#ifndef VULKAN_SPECS_PIPELINERESOURCESETUP_H
#define VULKAN_SPECS_PIPELINERESOURCESETUP_H
#include "build_configs.h"
#include "Initializer.h"
#include <vulkan/vulkan.h>


class PipelineResourceSetup {
private:
    VkRenderPassBeginInfo   renderPassBeginInfo   = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    VkRenderPassCreateInfo  renderPassCreateInfo  = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    VkFramebufferCreateInfo framebufferCreateInfo = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    VkImageViewCreateInfo   imageViewCreateInfo   = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    VkRect2D renderArea = {{0 , 0 },{400, 600}};
    VkRenderPassBeginInfo* renderPassBeginInfos = nullptr;
    VkRenderPass   renderPass      = nullptr;
    VkFramebuffer* framebuffers    = nullptr;
    VkImageView*   imageViews      = nullptr;
    VkClearValue*  p_clearValues   = nullptr;
    VkFramebuffer* new_framebuffers    = nullptr;
    VkImageView*   new_imageViews      = nullptr;
    uint32_t stageCount = 0;
    uint32_t imgCount   = 0;
    VkShaderStageFlagBits* shaderStageFlags;
    VkShaderModule* shaderModules;
    VkPipelineShaderStageCreateFlags* pipelineShaderStageCreateFlags;
    typedef union PipelineCreateInfo {
        VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
        VkComputePipelineCreateInfo  ComputePipelineCreateInfo;
    } PipelineCreateInfo;
    PipelineCreateInfo pipelineCreateInfo;
    Initializer* II;
public:
    PipelineResourceSetup();

    void setInitializer(Initializer* pInitializer);

    void setExtent(uint32_t width,uint32_t height);

    void setOffset(int32_t x, int32_t y);

    void createRenderPass();

    void createFramebuffers(uint32_t img_count);

    void createImageViews(uint32_t img_count, VkImage* images);

    void recreateSwapchainResources(uint32_t img_count, VkImage* images, uint32_t w, uint32_t h);

    void destroySwapchainResources();

    void setClearValues(VkClearValue* p_clearVals, int num_of_clear_vals, uint32_t image_count);

    void setStageProps(uint32_t stage_count, VkShaderStageFlagBits* pStageFlags,
            VkShaderModule* pModules, VkPipelineShaderStageCreateFlags* pShaderStageCFlags);

    VkPipeline CreatePipeline(const char* pipeline_type);

    const VkRenderPassBeginInfo & getRenderPassBeginInfo(uint32_t imageIndex);

    ~PipelineResourceSetup();
};



#endif //VULKAN_SPECS_PIPELINERESOURCESETUP_H
