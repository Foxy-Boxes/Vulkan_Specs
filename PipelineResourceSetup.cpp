//
// Created by cacoethes on 8/10/20.
//

#include <cstring>
#include <iostream>
#include "global_inline.h"
#include "PipelineResourceSetup.h"

PipelineResourceSetup::PipelineResourceSetup() {
    renderPassBeginInfo.renderArea      = renderArea;
    renderPassBeginInfo.clearValueCount = 0;
    renderPassBeginInfo.pClearValues    = p_clearValues;
    renderPassBeginInfo.renderPass      = renderPass;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.levelCount = imageViewCreateInfo.subresourceRange.layerCount = 1;
    framebufferCreateInfo.attachmentCount = 1;
    framebufferCreateInfo.layers = 1;
}

void PipelineResourceSetup::setInitializer(Initializer *pInitializer) {
    II = pInitializer;
}

void PipelineResourceSetup::setExtent(uint32_t width, uint32_t height) {
    renderArea.extent.width  = width;
    renderArea.extent.height = height;
    renderPassBeginInfo.renderArea.extent.width  = width;
    renderPassBeginInfo.renderArea.extent.height = height;
    framebufferCreateInfo.width  = width;
    framebufferCreateInfo.height = height;
}

void PipelineResourceSetup::setOffset(int32_t x, int32_t y) {
    renderArea.offset.x = x;
    renderArea.offset.y = y;
    renderPassBeginInfo.renderArea.offset.x = x;
    renderPassBeginInfo.renderArea.offset.y = y;
}

void PipelineResourceSetup::createRenderPass() {
    VkAttachmentReference attachmentReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
    VkSubpassDescription subpassDescription = {};
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.flags = VK_SUBPASS_DESCRIPTION_PER_VIEW_ATTRIBUTES_BIT_NVX;
    subpassDescription.inputAttachmentCount = 0;
    subpassDescription.pColorAttachments = &attachmentReference;
    VkAttachmentDescription attachmentDescription = {};

    imageViewCreateInfo.format = attachmentDescription.format = II->BI->Swapchain_C_Info.imageFormat;
    attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachmentDescription.flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;

    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pAttachments = &attachmentDescription;
    renderPassCreateInfo.attachmentCount = 1;

    check_vk_result(vkCreateRenderPass(II->Device, &renderPassCreateInfo, NULL, &renderPass),
                    "Render Pass Creation");
    framebufferCreateInfo.renderPass = renderPass;
    renderPassBeginInfo.renderPass = renderPass;
}

void PipelineResourceSetup::createFramebuffers(uint32_t img_count) {
    framebuffers = (VkFramebuffer *)malloc(img_count * sizeof(VkFramebuffer));
    imgCount = img_count;
    if(!renderPassBeginInfos){
        renderPassBeginInfos = (VkRenderPassBeginInfo *)malloc(img_count * sizeof(VkRenderPassBeginInfo));
        for (uint32_t i = 0; i < img_count; ++i) {
            renderPassBeginInfos[i] = renderPassBeginInfo;
            renderPassBeginInfos[i].pClearValues = p_clearValues + (i * renderPassBeginInfos[i].clearValueCount);
        }
    }
    for (uint32_t i = 0; i < img_count; ++i) {
        framebufferCreateInfo.pAttachments    = imageViews + i;
        check_vk_result(vkCreateFramebuffer(II->Device, &framebufferCreateInfo, NULL, framebuffers + i),
                        "Framebuffer Creation");
        assert(framebuffers[i]);
        renderPassBeginInfos[i].framebuffer = framebuffers[i];
    }
}

void PipelineResourceSetup::createImageViews(uint32_t img_count, VkImage* images) {
    imageViews   = (VkImageView *)malloc(img_count * sizeof(VkImageView));
    for (uint32_t i = 0; i < img_count; ++i) {
        imageViewCreateInfo.image = images[i];
        check_vk_result(vkCreateImageView(II->Device, &imageViewCreateInfo, NULL, imageViews + i),
                        "Image View Creation");
    }
}

void PipelineResourceSetup::setClearValues(VkClearValue *p_clearVals, int num_of_clear_vals, uint32_t image_count) {
    p_clearValues = (VkClearValue*)malloc(sizeof(VkClearValue) * num_of_clear_vals);
    for (uint32_t j = 0; j < image_count; ++j) {
        for (uint32_t i = 0; i < num_of_clear_vals; ++i) {
            p_clearValues[j * num_of_clear_vals + i] = p_clearVals[i];
        }
    }
    renderPassBeginInfo.pClearValues    = p_clearVals;
    renderPassBeginInfo.clearValueCount = num_of_clear_vals;
}

void PipelineResourceSetup::setStageProps(uint32_t stage_count, VkShaderStageFlagBits *pStageFlags,
                                          VkShaderModule *pModules, VkPipelineShaderStageCreateFlags *pShaderStageCFlags) {
    stageCount                     = stage_count;
    shaderStageFlags               = pStageFlags;
    shaderModules                  = pModules;
    pipelineShaderStageCreateFlags = pShaderStageCFlags;
}

VkPipeline PipelineResourceSetup::CreatePipeline(const char *pipeline_type) {
    if (!strcmp(pipeline_type, "graphics")){
        pipelineCreateInfo.GraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.GraphicsPipelineCreateInfo.renderPass = renderPass;
        pipelineCreateInfo.GraphicsPipelineCreateInfo.stageCount = stageCount;
        VkPipelineShaderStageCreateInfo Stages[stageCount];
        for (int i = 0; i < stageCount; ++i) {
            Stages[i].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            Stages[i].stage  = shaderStageFlags[i];
            Stages[i].module = shaderModules[i];
            Stages[i].flags  = pipelineShaderStageCreateFlags[i];
            Stages[i].pName  = "main";
            Stages[i].pNext  = nullptr;
            Stages[i].pSpecializationInfo = nullptr;
        }
        pipelineCreateInfo.GraphicsPipelineCreateInfo.pStages = Stages;
        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        pipelineCreateInfo.GraphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
        VkPipelineInputAssemblyStateCreateInfo    inputAssemblyStateCreateInfo = {};
        inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        pipelineCreateInfo.GraphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;

//       TODO?: const VkPipelineTessellationStateCreateInfo*     pTessellationState;
        VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
        viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCreateInfo.viewportCount = 1;
        viewportStateCreateInfo.scissorCount  = 1;
        pipelineCreateInfo.GraphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;

        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
        rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateCreateInfo.lineWidth = 1.0f;
        pipelineCreateInfo.GraphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;

        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
        multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        pipelineCreateInfo.GraphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;

        VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
        depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        pipelineCreateInfo.GraphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;

        VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
        colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
        colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
        colorBlendStateCreateInfo.attachmentCount = 1;
        pipelineCreateInfo.GraphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;

        VkDynamicState dynamicStates[2];
        dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
        dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;
        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
        dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.pDynamicStates = dynamicStates;
        dynamicStateCreateInfo.dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
        pipelineCreateInfo.GraphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;

        VkPipelineLayoutCreateInfo layoutCreateInfo = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
        VkPipelineLayout layout = nullptr;
        check_vk_result(vkCreatePipelineLayout(II->Device, &layoutCreateInfo, NULL, &layout),
                        "Pipeline Layout Creation");
        pipelineCreateInfo.GraphicsPipelineCreateInfo.layout = layout;

        pipelineCreateInfo.GraphicsPipelineCreateInfo.renderPass = renderPass;
        VkPipeline pipeline = nullptr;
        check_vk_result(vkCreateGraphicsPipelines(II->Device, nullptr, 1, &pipelineCreateInfo.GraphicsPipelineCreateInfo, NULL, &pipeline),
                        "Graphics Pipeline Creation");
//        vkDestroyPipelineLayout(II->Device, layout, NULL);
        return pipeline;
    }
    else if (!strcmp(pipeline_type, "compute")){
        pipelineCreateInfo.ComputePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        VkPipeline pipeline = nullptr;
        //TODO: Compute Pipeline
        return pipeline;
    } else{
        std::cerr << "Pipeline type has typo" << std::endl;
        return nullptr;
    }
}

const VkRenderPassBeginInfo & PipelineResourceSetup::getRenderPassBeginInfo(uint32_t imageIndex) {
    return renderPassBeginInfos[imageIndex];
}

PipelineResourceSetup::~PipelineResourceSetup() {
    if(II -> Device){
        destroySwapchainResources();
        if (renderPass){
            vkDestroyRenderPass(II->Device, renderPass, NULL);
        }
        ifree(framebuffers);
        ifree(p_clearValues);
        ifree(imageViews);
        ifree(renderPassBeginInfos);
    }
}

void PipelineResourceSetup::destroySwapchainResources() {
    if(imageViews && framebuffers && II->Device){

        for (int i = 0; i < imgCount; ++i) {

            vkDestroyImageView(II->Device,imageViews[i],NULL);
            vkDestroyFramebuffer(II->Device, framebuffers[i], NULL);
        }
    }
}

void PipelineResourceSetup::recreateSwapchainResources(uint32_t img_count, VkImage *images, uint32_t w, uint32_t h) {
    new_imageViews   = (VkImageView *)malloc(img_count * sizeof(VkImageView));
    new_framebuffers = (VkFramebuffer *)malloc(img_count * sizeof(VkFramebuffer));
    imgCount = img_count;
    framebufferCreateInfo.width  = w;
    framebufferCreateInfo.height = h;
    for (uint32_t i = 0; i < img_count; ++i) {
        imageViewCreateInfo.image = images[i];
        check_vk_result(vkCreateImageView(II->Device, &imageViewCreateInfo, NULL, new_imageViews + i),
                        "Image View Creation");
        framebufferCreateInfo.pAttachments    = new_imageViews + i;
        check_vk_result(vkCreateFramebuffer(II->Device, &framebufferCreateInfo, NULL, new_framebuffers + i),
                        "Framebuffer Creation");
        assert(new_framebuffers[i]);
    }
    check_vk_result(vkDeviceWaitIdle(II->Device),
                    "Fucking Wait Failed");
    destroySwapchainResources();
    for (uint32_t i = 0; i < img_count; ++i) {
        renderPassBeginInfos[i].framebuffer = new_framebuffers[i];
        renderPassBeginInfos[i].renderArea.extent.width  = w;
        renderPassBeginInfos[i].renderArea.extent.height = h;
    }
    framebuffers = new_framebuffers;
    imageViews = new_imageViews;
}


