#pragma once

#include <stdio.h>
#include <stdbool.h>
#include "vulkan/vulkan_core.h"
#include "cts/icd.h"
#include "cts/renderer.h"
#include "private/surface_private.h"

#define GETPROCADDR(func) if(!strcmp(pName, "vk" #func)) return (PFN_vkVoidFunction)&cts##func

static PFN_vkVoidFunction ctsGetInstanceProcAddr(VkInstance instance, const char* pName);
static PFN_vkVoidFunction ctsGetDeviceProcAddr(VkDevice device, const char *pName);
static bool isExtension(const char* pName);

LIBRARY_API PFN_vkVoidFunction VKAPI_CALL vk_icdGetInstanceProcAddr(VkInstance instance, const char* pName)
{
	return ctsGetInstanceProcAddr(instance, pName);
}

LIBRARY_API VkResult VKAPI_CALL vk_icdNegotiateLoaderICDInterfaceVersion(uint32_t* pSupportedVersion)
{
	*pSupportedVersion = 3;
	return VK_SUCCESS;
}

static PFN_vkVoidFunction ctsGetInstanceProcAddr(VkInstance instance, const char* pName)
{
	GETPROCADDR(GetInstanceProcAddr);
	GETPROCADDR(GetDeviceProcAddr);
	GETPROCADDR(CreateInstance);
	GETPROCADDR(DestroyInstance);
	GETPROCADDR(CreateDevice);
	GETPROCADDR(EnumeratePhysicalDevices);
	GETPROCADDR(EnumerateInstanceVersion);
	GETPROCADDR(EnumerateInstanceExtensionProperties);
	GETPROCADDR(EnumerateInstanceLayerProperties);
	GETPROCADDR(EnumerateDeviceExtensionProperties);
	GETPROCADDR(GetPhysicalDeviceFeatures);
	GETPROCADDR(GetPhysicalDeviceProperties);
	GETPROCADDR(GetPhysicalDeviceFormatProperties);
	GETPROCADDR(GetPhysicalDeviceImageFormatProperties);
	GETPROCADDR(GetPhysicalDeviceQueueFamilyProperties);
	GETPROCADDR(GetPhysicalDeviceMemoryProperties);
	GETPROCADDR(GetPhysicalDeviceSparseImageFormatProperties);
	
	GETPROCADDR(CreateSwapchainKHR);
	GETPROCADDR(GetSwapchainImagesKHR);
	GETPROCADDR(AcquireNextImageKHR);
	GETPROCADDR(QueuePresentKHR);
	GETPROCADDR(DestroySwapchainKHR);

	GETPROCADDR(DestroySurfaceKHR);
	GETPROCADDR(GetPhysicalDeviceSurfaceSupportKHR);
	GETPROCADDR(GetPhysicalDeviceSurfaceCapabilitiesKHR);
	GETPROCADDR(GetPhysicalDeviceSurfaceFormatsKHR);
	GETPROCADDR(GetPhysicalDeviceSurfacePresentModesKHR);

	GETPROCADDR(CreateWin32SurfaceKHR);
	GETPROCADDR(GetPhysicalDeviceWin32PresentationSupportKHR);
	
	if (!isExtension(pName)) {
		//printf("ctsGetInstanceProcAddr is missing export for: %s\n", pName);
	}

	return NULL;
}

static PFN_vkVoidFunction ctsGetDeviceProcAddr(VkDevice device, const char *pName)
{
	GETPROCADDR(CreateBuffer);
	GETPROCADDR(DestroyBuffer);

	//GETPROCADDR(CreateBufferView);
	//GETPROCADDR(DestroyBufferView);

	GETPROCADDR(BeginCommandBuffer);
	GETPROCADDR(ResetCommandBuffer);
	GETPROCADDR(EndCommandBuffer);
	GETPROCADDR(QueueSubmit);
	GETPROCADDR(CmdBeginQuery);
	GETPROCADDR(CmdBeginRenderPass);
	GETPROCADDR(CmdBindDescriptorSets);
	GETPROCADDR(CmdBindIndexBuffer);
	GETPROCADDR(CmdBindPipeline);
	GETPROCADDR(CmdBindVertexBuffers);
	GETPROCADDR(CmdBlitImage);
	GETPROCADDR(CmdClearAttachments);
	GETPROCADDR(CmdClearColorImage);
	GETPROCADDR(CmdClearDepthStencilImage);
	GETPROCADDR(CmdCopyBuffer);
	GETPROCADDR(CmdCopyBufferToImage);
	GETPROCADDR(CmdCopyImage);
	GETPROCADDR(CmdCopyImageToBuffer);
	GETPROCADDR(CmdCopyQueryPoolResults);
	GETPROCADDR(CmdDispatch);
	GETPROCADDR(CmdDispatchIndirect);
	GETPROCADDR(CmdDraw);
	GETPROCADDR(CmdDrawIndexed);
	GETPROCADDR(CmdDrawIndexedIndirect);
	GETPROCADDR(CmdDrawIndirect);
	GETPROCADDR(CmdEndQuery);
	GETPROCADDR(CmdEndRenderPass);
	GETPROCADDR(CmdExecuteCommands);
	GETPROCADDR(CmdFillBuffer);
	GETPROCADDR(CmdNextSubpass);
	GETPROCADDR(CmdPipelineBarrier);
	GETPROCADDR(CmdPushConstants);
	GETPROCADDR(CmdResetEvent);
	GETPROCADDR(CmdResetQueryPool);
	GETPROCADDR(CmdResolveImage);
	GETPROCADDR(CmdSetBlendConstants);
	GETPROCADDR(CmdSetDepthBias);
	GETPROCADDR(CmdSetDepthBounds);
	GETPROCADDR(CmdSetDeviceMask);
	GETPROCADDR(CmdSetEvent);
	GETPROCADDR(CmdSetLineWidth);
	GETPROCADDR(CmdSetScissor);
	GETPROCADDR(CmdSetStencilCompareMask);
	GETPROCADDR(CmdSetStencilReference);
	GETPROCADDR(CmdSetStencilWriteMask);
	GETPROCADDR(CmdSetViewport);
	GETPROCADDR(CmdUpdateBuffer);
	GETPROCADDR(CmdWaitEvents);
	GETPROCADDR(CmdWriteTimestamp);
	
	GETPROCADDR(CreateCommandPool);
	//GETPROCADDR(ResetCommandPool);
	GETPROCADDR(DestroyCommandPool);

	GETPROCADDR(AllocateCommandBuffers);
	GETPROCADDR(FreeCommandBuffers);
	
	GETPROCADDR(CreateDescriptorPool);
	GETPROCADDR(DestroyDescriptorPool);
	
	GETPROCADDR(CreateDescriptorSetLayout);
	GETPROCADDR(DestroyDescriptorSetLayout);
	
	GETPROCADDR(AllocateDescriptorSets);
	GETPROCADDR(UpdateDescriptorSets);
	GETPROCADDR(FreeDescriptorSets);
	
	GETPROCADDR(AllocateMemory);
	GETPROCADDR(MapMemory);
	GETPROCADDR(UnmapMemory);
	GETPROCADDR(FlushMappedMemoryRanges);
	GETPROCADDR(FreeMemory);
	GETPROCADDR(BindBufferMemory);
	GETPROCADDR(BindImageMemory);
	GETPROCADDR(GetBufferMemoryRequirements);
	GETPROCADDR(GetImageMemoryRequirements);
	
	// CreateDevice is part of the Instance chain
	GETPROCADDR(GetDeviceQueue);
	GETPROCADDR(DestroyDevice);
	
	GETPROCADDR(CreateFence);
	GETPROCADDR(ResetFences);
	GETPROCADDR(GetFenceStatus);
	GETPROCADDR(WaitForFences);
	GETPROCADDR(DestroyFence);
	
	GETPROCADDR(CreateFramebuffer);
	GETPROCADDR(DestroyFramebuffer);
	
	GETPROCADDR(CreateImageView);
	GETPROCADDR(DestroyImageView);
	
	GETPROCADDR(CreateImage);
	GETPROCADDR(DestroyImage);
	
	GETPROCADDR(GetPhysicalDeviceQueueFamilyProperties);
	GETPROCADDR(EnumerateDeviceExtensionProperties);
	GETPROCADDR(GetPhysicalDeviceProperties);
	GETPROCADDR(GetPhysicalDeviceMemoryProperties);
	GETPROCADDR(GetPhysicalDeviceFormatProperties);
	
	GETPROCADDR(CreatePipelineLayout);
	GETPROCADDR(DestroyPipelineLayout);
	
	GETPROCADDR(CreateGraphicsPipelines);
	GETPROCADDR(DestroyPipeline);
	
	GETPROCADDR(CreateQueryPool);
	GETPROCADDR(DestroyQueryPool);
	
	GETPROCADDR(QueueWaitIdle);
	
	GETPROCADDR(CreateRenderPass);
	GETPROCADDR(DestroyRenderPass);
	
	GETPROCADDR(CreateSampler);
	GETPROCADDR(DestroySampler);
	
	GETPROCADDR(CreateSemaphore);
	GETPROCADDR(DestroySemaphore);
	
	GETPROCADDR(CreateShaderModule);
	GETPROCADDR(DestroyShaderModule);
	
	GETPROCADDR(CreateSwapchainKHR);
	GETPROCADDR(GetSwapchainImagesKHR);
	GETPROCADDR(AcquireNextImageKHR);
	GETPROCADDR(QueuePresentKHR);
	GETPROCADDR(DestroySwapchainKHR);

	GETPROCADDR(DestroySurfaceKHR);
	GETPROCADDR(GetPhysicalDeviceSurfaceSupportKHR);
	GETPROCADDR(GetPhysicalDeviceSurfaceCapabilitiesKHR);
	GETPROCADDR(GetPhysicalDeviceSurfaceFormatsKHR);
	GETPROCADDR(GetPhysicalDeviceSurfacePresentModesKHR);

	GETPROCADDR(CreateWin32SurfaceKHR);
	GETPROCADDR(GetPhysicalDeviceWin32PresentationSupportKHR);
    
	if (!isExtension(pName)) {
		//printf("ctsGetDeviceProcAddr is missing export for: %s\n", pName);
	}

	return NULL;
}

static bool isExtension(const char* pName)
{
	return (strstr(pName, "EXT")    != NULL || 
			strstr(pName, "NV")     != NULL || 
			strstr(pName, "NVX")    != NULL || 
			strstr(pName, "AMD")    != NULL ||
			strstr(pName, "EXT")    != NULL ||
			strstr(pName, "HUAWEI") != NULL ||
			strstr(pName, "GOOGLE") != NULL ||
			strstr(pName, "INTEL")  != NULL);
}
