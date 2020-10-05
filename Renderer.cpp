//
// Created by cacoethes on 8/17/20.
//

#include "Renderer.h"
#include "global_inline.h"
#include <future>
#include <zconf.h>

class Initializer;
namespace Render{
    int CUSTOM_SIG_RESIZE;
    int CUSTOM_SIG_RESTART_RENDER;
    int CUSTOM_SIG_CLOSE_WINDOW;
    Initializer* g_p_init;
    std::atomic<bool> stop_check(false);
    struct StoppingThread {
        std::promise<void> exitSignal = std::promise<void>();
        std::future<void> futureObj;
        std::future<void> resizeFuture;
        std::thread s_thread;
        sig_atomic_t resizeFuture_isValid;
        sig_atomic_t s_thread_isValid;

        StoppingThread() {
            futureObj = exitSignal.get_future();
            s_thread_isValid = false;
            resizeFuture_isValid = false;
        }

        void ResetStopping() {
            exitSignal = std::promise<void>();
            futureObj = exitSignal.get_future();
        }

        void SetThread(std::thread *pnewThread) {
            s_thread = static_cast<std::thread &&>(*pnewThread);
            s_thread_isValid = true;
        }
    };
    StoppingThread* stoppingThread;

    void Should_Resize(int num){
        stoppingThread -> resizeFuture = std::async(std::launch::async,Render::Resize,g_p_init);
        stoppingThread -> resizeFuture_isValid = true;
        std::raise(CUSTOM_SIG_RESTART_RENDER);
    }
    void Window_Close(int num){
        if(stoppingThread -> s_thread_isValid) {
            stoppingThread -> exitSignal.set_value();
            stoppingThread -> s_thread.join();
        }
        stop_check.store(true);
        char buf[] ="Window Closed";
        write(STDOUT_FILENO,buf, sizeof(buf) / sizeof(buf[0]));
    }
    void Render_thread(std::future<void> futureObj){
        if (stoppingThread -> resizeFuture_isValid) {
            stoppingThread->resizeFuture.get();
            stoppingThread->resizeFuture_isValid = false;
        }
        while (futureObj.wait_for(std::chrono::microseconds(0)) != std::future_status::ready){
            Render_some(g_p_init);
        }
    }
    void Render_start(){
        std::promise<void> exitSignal;
        stoppingThread -> ResetStopping();
        std::thread th(&Render_thread, std::move(stoppingThread -> futureObj));
        stoppingThread -> SetThread(&th);
    }
    void Render_wrapper(int num){
        Render_start();
    }
    void Resize(Initializer* p_init){
        p_init -> Prepeare_Swapchain_Recreate();
        VkSwapchainKHR newSwapchain = nullptr;
        check_vk_result(vkCreateSwapchainKHR(p_init -> Device, &p_init -> BI -> getSwapchainCInfo(), nullptr, &newSwapchain),
                        "CANNOT Create Swapchain");
        uint32_t swap_img_count = 0;
        vkGetSwapchainImagesKHR(p_init -> Device, newSwapchain, &swap_img_count, NULL);
        check_vk_count(swap_img_count,
                       "Swapchain Images");
        VkImage swap_img[swap_img_count];
        check_vk_result(vkGetSwapchainImagesKHR(p_init -> Device, newSwapchain, &swap_img_count, swap_img),
                        "Problem getting images");
        p_init -> PIs[0] -> recreateSwapchainResources(swap_img_count, swap_img, p_init -> width, p_init -> height);
        p_init -> Swapchain = newSwapchain;
        for(uint32_t i = 0; i < swap_img_count; i++){
            p_init -> swapchain_images[i] = swap_img[i];
        }
        p_init -> swapchain_image_count = swap_img_count;
        vkDestroySwapchainKHR(p_init -> Device, p_init -> BI -> getSwapchainCInfo().oldSwapchain, NULL);
    }
    void Render_some(Initializer* p_init) {
        check_vk_result(vkAcquireNextImageKHR(p_init -> Device, p_init -> Swapchain, ~1ull, p_init -> acquire_Semaphores[0], VK_NULL_HANDLE, &p_init -> image_index),
                        "can't acq next image");

        check_vk_result(vkResetCommandPool(p_init -> Device, p_init -> CmdPool, 1),
                        "Reset on Command Pool Failed");
        check_vk_result(vkBeginCommandBuffer(p_init -> CmdBuffers[0], &p_init -> BI -> getCmdBufferBInfo()),
                        "Could not Begin Buffer");

        VkRenderPassBeginInfo *pRenderPassBegin;
        pRenderPassBegin = (VkRenderPassBeginInfo *) &p_init -> PIs[0]->getRenderPassBeginInfo(p_init -> image_index);
        vkCmdBeginRenderPass(p_init -> CmdBuffers[0], pRenderPassBegin, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport = { 0.0f, 0.0f, float (p_init -> width), float (p_init -> height), 0.0, 1.0};
        VkRect2D scissor = {{0, 0},{p_init -> width, p_init -> height}};
        vkCmdSetViewport(p_init -> CmdBuffers[0], 0, 1, &viewport);
        vkCmdSetScissor(p_init -> CmdBuffers[0], 0, 1, &scissor);
        vkCmdBindPipeline(p_init -> CmdBuffers[0], VK_PIPELINE_BIND_POINT_GRAPHICS, p_init -> pipeline[0]);
        vkCmdDraw(p_init -> CmdBuffers[0], 3, 1, 0, 0);

        vkCmdEndRenderPass(p_init -> CmdBuffers[0]);

        VkPipelineStageFlags submit_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
        submit_info.commandBufferCount   = 1;
        submit_info.pCommandBuffers      = p_init -> CmdBuffers.data();
        submit_info.waitSemaphoreCount   = 1;
        submit_info.pWaitSemaphores      = &p_init -> acquire_Semaphores[0];
        submit_info.pWaitDstStageMask    = &submit_stage_mask;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores    = &p_init -> complete_Semaphores[0];

        check_vk_result(vkEndCommandBuffer(p_init -> CmdBuffers[0]),
                        "Could not End Buffer");
        check_vk_result(vkQueueSubmit(p_init -> Queues[0], 1, &submit_info, NULL),
                        "Could not Submit");
        check_vk_result(vkQueuePresentKHR(p_init->Queues[0], &p_init->BI->getPresentInfo()),
                        "Cannot get queue to present");

        check_vk_result(vkDeviceWaitIdle(p_init -> Device),
                        "Fucking Wait Failed");
    }
    void Set_Env(Initializer* p_init){
        CUSTOM_SIG_CLOSE_WINDOW =
                (CUSTOM_SIG_RESTART_RENDER =
                (CUSTOM_SIG_RESIZE = SIGRTMIN + 1) + 1) + 1;
        std::signal(CUSTOM_SIG_RESIZE, Should_Resize);
        std::signal(CUSTOM_SIG_RESTART_RENDER, Render_wrapper);
        std::signal(CUSTOM_SIG_CLOSE_WINDOW, Window_Close);

        stoppingThread = new StoppingThread();
        g_p_init = p_init;
    }
    void Check_Resize(Initializer* p_init){
        while (!glfwWindowShouldClose(p_init->window_glfw) && !stop_check){
            VkSurfaceCapabilitiesKHR capabilitiesKhr = {};
            check_vk_result(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p_init->PhDevice, p_init->Surface, &capabilitiesKhr),
                            "Getting Capabilities Failed");
            if (capabilitiesKhr.currentExtent.width  != p_init->width ||
                capabilitiesKhr.currentExtent.height != p_init->height) {
                //terminate Render
                if(stoppingThread->s_thread_isValid){
                    stoppingThread -> exitSignal.set_value();
                    stoppingThread -> s_thread.join();
                }
                //done
                p_init -> width  = capabilitiesKhr.currentExtent.width;
                p_init -> height = capabilitiesKhr.currentExtent.height;
                auto _ign = std::async(std::launch::async,std::raise,CUSTOM_SIG_RESIZE);
            }
            check_vk_result(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p_init->PhDevice, p_init->Surface, &capabilitiesKhr),
                        "Getting Capabilities Failed");
        }
        if (glfwWindowShouldClose(p_init->window_glfw)){
            std::raise(CUSTOM_SIG_CLOSE_WINDOW);
        }
    }
    void End_Rendering(){
        delete stoppingThread;
    }
}
