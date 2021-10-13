#pragma once

#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CTS_DEFINE_HANDLE_CASTS(__cts_type, __VkType)                                              \
   static inline struct __cts_type *__cts_type##FromHandle(__VkType _handle)                       \
   {                                                                                               \
      return (struct __cts_type *)_handle;                                                         \
   }                                                                                               \
                                                                                                   \
   static inline __VkType __cts_type##ToHandle(struct __cts_type *_obj)                            \
   {                                                                                               \
      return (__VkType)_obj;                                                                       \
   }

#define CTS_DEFINE_NONDISP_HANDLE_CASTS(__cts_type, __VkType)                                      \
   static inline struct __cts_type *__cts_type##FromHandle(__VkType _handle)                       \
   {                                                                                               \
      return (struct __cts_type *)(uintptr_t)_handle;                                              \
   }                                                                                               \
                                                                                                   \
   static inline __VkType __cts_type##ToHandle(struct __cts_type *_obj)                            \
   {                                                                                               \
      return (__VkType)(uintptr_t)_obj;                                                            \
   }

CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsBuffer, VkBuffer)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsImage, VkImage)
CTS_DEFINE_HANDLE_CASTS(CtsInstance, VkInstance)
CTS_DEFINE_HANDLE_CASTS(CtsPhysicalDevice, VkPhysicalDevice)
CTS_DEFINE_HANDLE_CASTS(CtsDevice, VkDevice)
CTS_DEFINE_HANDLE_CASTS(CtsQueue, VkQueue)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsSemaphore, VkSemaphore)
CTS_DEFINE_HANDLE_CASTS(CtsCommandBuffer, VkCommandBuffer)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsFence, VkFence)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsDeviceMemory, VkDeviceMemory)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsEvent, VkEvent)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsQueryPool, VkQueryPool)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsBufferView, VkBufferView)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsImageView, VkImageView);
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsShaderModule, VkShaderModule)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsPipelineCache, VkPipelineCache)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsPipelineLayout, VkPipelineLayout)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsPipeline, VkPipeline)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsRenderPass, VkRenderPass)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsDescriptorSetLayout, VkDescriptorSetLayout)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsSampler, VkSampler)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsDescriptorSet, VkDescriptorSet)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsDescriptorPool, VkDescriptorPool)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsFramebuffer, VkFramebuffer)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsCommandPool, VkCommandPool)
//CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsDescriptorUpdateTemplate, VkDescriptorUpdateTemplate)
//CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsSamplerYcbcrConversion, VkSamplerYcbcrConversion)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsSurface, VkSurfaceKHR)
CTS_DEFINE_NONDISP_HANDLE_CASTS(CtsSwapchain, VkSwapchainKHR)

#ifdef __cplusplus
}
#endif
