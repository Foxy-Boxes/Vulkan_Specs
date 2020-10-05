//
// Created by cacoethes on 7/28/20.
//

#ifndef VULKAN_SPECS_BUILD_CONFIGS_H
#define VULKAN_SPECS_BUILD_CONFIGS_H
class Initializer;
class BackgroundCheck;
class PipelineResourceSetup;
#if BUILD_TYPE
#define DEBUG
#else
#define RELEASE
#endif
#endif //VULKAN_SPECS_BUILD_CONFIGS_H
