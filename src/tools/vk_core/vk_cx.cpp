#include <stdlib.h>
#include <stdio.h>
#include <tools.h>
#include <libuv/uv.h>
#include <libuv/uvw/util.hpp>
#include <base/ecc_sv.h>
#include <base/sem.h>
#include <sem_async_uv.h>
#include <palloc/palloc.h>
#include <base/print_time.h>


#include <ntype.h>
#include <iconv.h>
#include <base/smq.h>
#include <base/hlUtil.h>
#include "sqlt.h"
#include <base/promise_cx.h>
#include <http/http.h>
#include <net/curl_cx.h>
#include <view/utils_bin.h>
#include <view/image.h>
//#include <view/Canvas.h>

#ifdef _WIN32
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#endif /*_WIN32*/
#ifdef __ANDROID__
#ifndef VK_USE_PLATFORM_ANDROID_KHR
#define VK_USE_PLATFORM_ANDROID_KHR
#endif
#endif /*__ANDROID__*/

#ifndef USE_SWAPCHAIN_EXTENSIONS
#define USE_SWAPCHAIN_EXTENSIONS
#define VK_NO_PROTOTYPES0
#endif /*USE_SWAPCHAIN_EXTENSIONS*/

#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>

#include <spirv_cross.hpp>

#ifdef _WIN32
#define LOGIf printf
#define LOGI
#define LOGW LOGI
#define LOGD LOGI
#define LOGE LOGI
#define LOGv LOGI
#endif
//#include "vkclass.h"
//#include "vk_Initializers.h"
#include "vk_cx.h"

#include "vk_cx1.h"
using namespace std;
#include "view.h"
#include "bw_sdl.h"

#include <base/promise_cx.h>
#if 1


#ifndef HZ_VERSION
#define HZ_VERSION "nluna6.0"
#define HZ_APPname "nluna6.0app"
#endif // !HZ_VERSION
#ifndef VK_TRUE
#define VK_TRUE 1
#define VK_FALSE 0
#endif // !VK_TRUE
#ifndef VK_MAKE_VERSION
#define VK_MAKE_VERSION(major, minor, patch) \
    (((major) << 22) | ((minor) << 12) | (patch))
#endif // !VK_MAKE_VERSION
#ifndef VK_KHR_SURFACE_EXTENSION_NAME
#define VK_KHR_SURFACE_EXTENSION_NAME     "VK_KHR_surface"
#endif // !VK_KHR_SURFACE_EXTENSION_NAME
#ifdef _WIN32
#ifndef VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#define VK_KHR_WIN32_SURFACE_EXTENSION_NAME "VK_KHR_win32_surface"
#endif // !VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif // _WIN32

#ifndef VK_QUEUE_FAMILY_IGNORED
#define VK_QUEUE_FAMILY_IGNORED           (~0U)
#endif
#ifndef VKAPI_ATTR
#if defined(_WIN32)
// On Windows, Vulkan commands use the stdcall convention
#define VKAPI_ATTR
#define VKAPI_CALL __stdcall
#define VKAPI_PTR  VKAPI_CALL
#elif defined(__ANDROID__) && defined(__ARM_ARCH) && __ARM_ARCH < 7
#error "Vulkan isn't supported for the 'armeabi' NDK ABI"
#elif defined(__ANDROID__) && defined(__ARM_ARCH) && __ARM_ARCH >= 7 && defined(__ARM_32BIT_STATE)
// On Android 32-bit ARM targets, Vulkan functions use the "hardfloat"
// calling convention, i.e. float parameters are passed in registers. This
// is true even if the rest of the application passes floats on the stack,
// as it does by default when compiling for the armeabi-v7a NDK ABI.
#define VKAPI_ATTR __attribute__((pcs("aapcs-vfp")))
#define VKAPI_CALL
#define VKAPI_PTR  VKAPI_ATTR
#else
// On other platforms, use the default calling convention
#define VKAPI_ATTR
#define VKAPI_CALL
#define VKAPI_PTR
#endif
#endif
#if defined(__ANDROID__)
#define VK_CHECK_RESULT(f)																				\
{																										\
	VkResult res = (f);																					\
	if (res != VK_SUCCESS)																				\
	{																									\
		LOGE("Fatal : VkResult is \" %s \" in %s at line %d", hz::vkc::errorString(res).c_str(), __FILE__, __LINE__); \
		assert(res == VK_SUCCESS);																		\
	}																									\
}
#else
#define VK_CHECK_RESULT(f) { VkResult res = (f); if (res != VK_SUCCESS)	{ std::cout << "Fatal : VkResult is \"" << hz::vkc::errorString(res) << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl; assert(res == VK_SUCCESS);}}
#define VK_CHECK_RESULT0(f) { VkResult res = (f); if (res != VK_SUCCESS)	{ std::cout << "Fatal : VkResult is \"" << hz::vkc::errorString(res) << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl; (res == VK_SUCCESS);}}
#endif

#ifndef VK_EXT_DEBUG_MARKER_EXTENSION_NAME
#define VK_EXT_DEBUG_MARKER_EXTENSION_NAME "VK_EXT_debug_marker"
#endif // !VK_EXT_DEBUG_MARKER_EXTENSION_NAME

#ifndef VK_KHR_SWAPCHAIN_EXTENSION_NAME
#define VK_KHR_SWAPCHAIN_EXTENSION_NAME   "VK_KHR_swapchain"
#endif // !VK_KHR_SWAPCHAIN_EXTENSION_NAME

#ifndef VK_NULL_HANDLE
#define VK_NULL_HANDLE 0
#endif // !VK_NULL_HANDLE
#ifndef VK_SUBPASS_EXTERNAL
#define VK_SUBPASS_EXTERNAL               (~0U)
#endif // !VK_SUBPASS_EXTERNAL


// 内存泄漏检测
#ifdef _DEBUG
#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif
namespace hz
{
	class dev_info
	{
	public:
		VkInstance instance;
		VkPhysicalDeviceMemoryProperties memoryProperties = {};
		std::vector<VkQueueFamilyProperties> queueFamilyProperties;
		VkPhysicalDeviceFeatures enabledFeatures = {};
		std::vector<const char*> enabledExtensions;
		VkPhysicalDeviceProperties properties = {};
		VkPhysicalDeviceFeatures features = {};
	public:
		dev_info()
		{
		}

		~dev_info()
		{
		}

	private:

	};
#define DSEXT(n) PFN_vk##n vk##n
	struct dynamic_state_ext_cb {
		DSEXT(CmdSetCullModeEXT);
		DSEXT(CmdSetFrontFaceEXT);
		DSEXT(CmdSetPrimitiveTopologyEXT);
		DSEXT(CmdSetViewportWithCountEXT);
		DSEXT(CmdSetScissorWithCountEXT);
		DSEXT(CmdBindVertexBuffers2EXT);
		DSEXT(CmdSetDepthTestEnableEXT);
		DSEXT(CmdSetDepthWriteEnableEXT);
		DSEXT(CmdSetDepthCompareOpEXT);
		DSEXT(CmdSetDepthBoundsTestEnableEXT);
		DSEXT(CmdSetStencilTestEnableEXT);
		DSEXT(CmdSetStencilOpEXT);
	};
#undef DSEXT
	namespace vkc
	{
		VkSampler createSampler(dvk_device* dev, VkFilter filter, VkSamplerAddressMode addressMode);
		std::string errorString(VkResult errorCode)
		{
			switch (errorCode)
			{
#define STR(r) case VK_ ##r: return #r
				STR(NOT_READY);
				STR(TIMEOUT);
				STR(EVENT_SET);
				STR(EVENT_RESET);
				STR(INCOMPLETE);
				STR(ERROR_OUT_OF_HOST_MEMORY);
				STR(ERROR_OUT_OF_DEVICE_MEMORY);
				STR(ERROR_INITIALIZATION_FAILED);
				STR(ERROR_DEVICE_LOST);
				STR(ERROR_MEMORY_MAP_FAILED);
				STR(ERROR_LAYER_NOT_PRESENT);
				STR(ERROR_EXTENSION_NOT_PRESENT);
				STR(ERROR_FEATURE_NOT_PRESENT);
				STR(ERROR_INCOMPATIBLE_DRIVER);
				STR(ERROR_TOO_MANY_OBJECTS);
				STR(ERROR_FORMAT_NOT_SUPPORTED);
				STR(ERROR_SURFACE_LOST_KHR);
				STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
				STR(SUBOPTIMAL_KHR);
				STR(ERROR_OUT_OF_DATE_KHR);
				STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
				STR(ERROR_VALIDATION_FAILED_EXT);
				STR(ERROR_INVALID_SHADER_NV);
#undef STR
			default:
				return "UNKNOWN_ERROR";
			}
		}


		void exitFatal(std::string message, std::string caption)
		{
#if defined(_WIN32)
			MessageBoxA(NULL, message.c_str(), caption.c_str(), MB_OK | MB_ICONERROR);
#elif defined(__ANDROID__)
			LOGE("Fatal error: %s", message.c_str());
#else
			std::cerr << message << "\n";
#endif
			exit(1);
		}

		void setImageLayout(
			VkCommandBuffer cmdbuffer,
			VkImage image,
			VkImageAspectFlags aspectMask,
			VkImageLayout oldImageLayout,
			VkImageLayout newImageLayout,
			VkImageSubresourceRange subresourceRange,
			VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT)
		{
			// Create an image barrier object
			VkImageMemoryBarrier imageMemoryBarrier = {};
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.oldLayout = oldImageLayout;
			imageMemoryBarrier.newLayout = newImageLayout;
			imageMemoryBarrier.image = image;
			imageMemoryBarrier.subresourceRange = subresourceRange;

			// Source layouts (old)
			// Source access mask controls actions that have to be finished on the old layout
			// before it will be transitioned to the new layout
			switch (oldImageLayout)
			{
			case VK_IMAGE_LAYOUT_UNDEFINED:
				// Image layout is undefined (or does not matter)
				// Only valid as initial layout
				// No flags required, listed only for completeness
				imageMemoryBarrier.srcAccessMask = 0;
				break;

			case VK_IMAGE_LAYOUT_PREINITIALIZED:
				// Image is preinitialized
				// Only valid as initial layout for linear images, preserves memory contents
				// Make sure host writes have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				// Image is a color attachment
				// Make sure any writes to the color buffer have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				// Image is a depth/stencil attachment
				// Make sure any writes to the depth/stencil buffer have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				// Image is a transfer source
				// Make sure any reads from the image have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				// Image is a transfer destination
				// Make sure any writes to the image have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				// Image is read by a shader
				// Make sure any shader reads from the image have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;
			}

			// Target layouts (new)
			// Destination access mask controls the dependency for the new image layout
			switch (newImageLayout)
			{
			case VK_IMAGE_LAYOUT_GENERAL:
				// Image will be used as a transfer destination
				// Make sure any writes to the image have been finished
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;;
				break;
			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				// Image will be used as a transfer destination
				// Make sure any writes to the image have been finished
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				// Image will be used as a transfer source
				// Make sure any reads from and writes to the image have been finished
				imageMemoryBarrier.srcAccessMask = imageMemoryBarrier.srcAccessMask | VK_ACCESS_TRANSFER_READ_BIT;
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				// Image will be used as a color attachment
				// Make sure any writes to the color buffer have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				// Image layout will be used as a depth/stencil attachment
				// Make sure any writes to depth/stencil buffer have been finished
				imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				// Image will be read in a shader (sampler, input attachment)
				// Make sure any writes to the image have been finished
				if (imageMemoryBarrier.srcAccessMask == 0)
				{
					imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
				}
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;
			}

			// Put barrier inside setup command buffer
			vkCmdPipelineBarrier(
				cmdbuffer,
				srcStageMask,
				dstStageMask,
				0,
				0, nullptr,
				0, nullptr,
				1, &imageMemoryBarrier);
		}

		// Fixed sub resource on first mip level and layer
		void setImageLayout(
			VkCommandBuffer cmdbuffer,
			VkImage image,
			VkImageAspectFlags aspectMask,
			VkImageLayout oldImageLayout,
			VkImageLayout newImageLayout,
			VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT)
		{
			VkImageSubresourceRange subresourceRange = {};
			subresourceRange.aspectMask = aspectMask;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.layerCount = 1;
			setImageLayout(cmdbuffer, image, aspectMask, oldImageLayout, newImageLayout, subresourceRange, srcStageMask, dstStageMask);
		}



#ifdef _DEBUG
		static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
		{
			if (flags != 8)
			{
				(void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
				fprintf(stderr, "[vulkan] ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);

			}
			return 0;
		}
#endif
		bool CheckExtensionAvailability(const char* extension_name, const std::vector<VkExtensionProperties>& available_extensions) {
			std::string en;
			for (size_t i = 0; i < available_extensions.size(); ++i) {
				en = available_extensions[i].extensionName;
				if (en == extension_name)
					return true;
				//if (strcmp(available_extensions[i].extensionName, extension_name) == 0) { return true; }
			}
			return false;
		}

#ifdef GLFWAPI
		VkInstance glfw_get_instance()
		{
			VkResult err;
			VkInstance Instance = 0;
			// Create Vulkan Instance
			{
				uint32_t extensions_count = 0;
				const char** extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
				VkInstanceCreateInfo create_info = {};
				create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
				create_info.enabledExtensionCount = extensions_count;
				create_info.ppEnabledExtensionNames = &extensions[0];

#ifdef _DEBUG
				// Enabling multiple validation layers grouped as LunarG standard validation
				const char* layers[] = {"VK_LAYER_LUNARG_standard_validation"};
				create_info.enabledLayerCount = 1;
				create_info.ppEnabledLayerNames = layers;

				// Enable debug report extension (we need additional storage, so we duplicate the user array to add our new extension to it)
				//const char** extensions_ext = (const char**)malloc(sizeof(const char*) * (extensions_count + 1));
				std::vector<const char*> extensions_ext;
				extensions_ext.resize(extensions_count + 1);
				memcpy(extensions_ext.data(), extensions, extensions_count * sizeof(const char*));
				extensions_ext[extensions_count] = "VK_EXT_debug_report";
				create_info.enabledExtensionCount = extensions_count + 1;
				create_info.ppEnabledExtensionNames = extensions_ext.data();

				// Create Vulkan Instance
				err = vkCreateInstance(&create_info, nullptr, &Instance);
				//check_vk_result(err);
				//free(extensions_ext);

				// Get the function pointer (required for any extensions)
				auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugReportCallbackEXT");
				// IM_ASSERT(vkCreateDebugReportCallbackEXT != NULL);

				// Setup the debug report callback
				VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
				debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
				debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
				debug_report_ci.pfnCallback = debug_report;
				debug_report_ci.pUserData = NULL;
				err = vkCreateDebugReportCallbackEXT(Instance, &debug_report_ci, nullptr, &_DebugReport);
				//check_vk_result(err);
#else
				// Create Vulkan Instance without any debug feature
				err = vkCreateInstance(&create_info, nullptr, &Instance);
				//check_vk_result(err);
				//UNUSED(g_DebugReport);
#endif
			}
			return Instance;
		}
#endif
		VkInstance CreateInstance()
		{
			uint32_t extensions_count = 0;

			if ((!vkEnumerateInstanceExtensionProperties) || (vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr) != VK_SUCCESS) ||
				(extensions_count == 0)) {
				//LOGE("Error occurred during instance extensions enumeration!\n");
				return 0;
			}

			std::vector<VkExtensionProperties> available_extensions(extensions_count);
			if (!vkEnumerateInstanceExtensionProperties || vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, &available_extensions[0]) != VK_SUCCESS) {
				//LOGE("Error occurred during instance extensions enumeration!");
				return 0;
			}

			std::vector<const char*> extensions = {
				VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_XLIB_KHR)
				VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#elif defined(VK_USE_PLATFORM_XCB_KHR)
				VK_KHR_XCB_SURFACE_EXTENSION_NAME
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
				VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
#elif defined(VK_USE_PLATFORM_MIR_KHR)
				VK_KHR_MIR_SURFACE_EXTENSION_NAME
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
				VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
				VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif
			};
			for (size_t i = 0; i < extensions.size(); ++i) {
				if (!CheckExtensionAvailability(extensions[i], available_extensions)) {
					std::cout << "Could not find instance extension named \"" << extensions[i] << "\"!" << std::endl;
					return 0;
				}
			}
			VkApplicationInfo application_info = {
				VK_STRUCTURE_TYPE_APPLICATION_INFO,             // VkStructureType            sType
				nullptr,                                        // const void                *pNext
				HZ_APPname,  // const char                *pApplicationName
				VK_MAKE_VERSION(1, 0, 0),                     // uint32_t                   applicationVersion
				HZ_VERSION,                     // const char                *pEngineName
				VK_MAKE_VERSION(1, 0, 0),                     // uint32_t                   engineVersion
				0//VK_MAKE_VERSION(1, 0, 0)                      // uint32_t                   apiVersion
			};

			VkInstanceCreateInfo instance_create_info = {
				VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,         // VkStructureType            sType
				nullptr,                                        // const void                *pNext
				0,                                              // VkInstanceCreateFlags      flags
				&application_info,                              // const VkApplicationInfo   *pApplicationInfo
				0,                                              // uint32_t                   enabledLayerCount
				nullptr,                                        // const char * const        *ppEnabledLayerNames
				static_cast<uint32_t>(extensions.size()),       // uint32_t                   enabledExtensionCount
				&extensions[0]                                  // const char * const        *ppEnabledExtensionNames
			};
			VkInstance Instance = nullptr;
#ifdef GLFWAPI
			Instance = glfw_get_instance();
#endif // GLFWAPI
			if (!Instance && vkCreateInstance(&instance_create_info, nullptr, &Instance) != VK_SUCCESS) {
				std::cout << "Could not create Vulkan instance!" << std::endl;
				return 0;
			}
			uint32_t iv = 0;
			vkEnumerateInstanceVersion(&iv);
			//printf("%d\n", iv);
			return Instance;
		}

		VkQueueFamilyProperties* get_queue_fp(dvk_device* dev, VkQueueFlagBits queueFlags)
		{
			VkQueueFamilyProperties* ret = 0;
			auto& queueFamilyProperties = dev->_info->queueFamilyProperties;
			do {
				if (queueFlags & VK_QUEUE_COMPUTE_BIT)
				{
					for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
					{
						if ((queueFamilyProperties[i].queueFlags & queueFlags) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
						{
							ret = &queueFamilyProperties[i];
							break;
						}
					}
					if (ret)
					{
						break;
					}
				}

				// Dedicated queue for transfer
				// Try to find a queue family index that supports transfer but not graphics and compute
				if (queueFlags & VK_QUEUE_TRANSFER_BIT)
				{
					for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
					{
						if ((queueFamilyProperties[i].queueFlags & queueFlags) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
						{
							ret = &queueFamilyProperties[i];
							break;
						}
					}
					if (ret)
					{
						break;
					}
				}

				// For other queue types or if no separate compute queue is present, return the first one to support the requested flags
				for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
				{
					if (queueFamilyProperties[i].queueFlags & queueFlags)
					{
						ret = &queueFamilyProperties[i];
						break;
					}
				}
			} while (0);
			return ret;
		}
		uint32_t getQueueFamilyIndex(dvk_device* dev, VkQueueFlagBits queueFlags)
		{
			auto& queueFamilyProperties = dev->_info->queueFamilyProperties;
			// Dedicated queue for compute
			// Try to find a queue family index that supports compute but not graphics
			if (queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
				{
					if ((queueFamilyProperties[i].queueFlags & queueFlags) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
					{
						return i;
						break;
					}
				}
			}

			// Dedicated queue for transfer
			// Try to find a queue family index that supports transfer but not graphics and compute
			if (queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
				{
					if ((queueFamilyProperties[i].queueFlags & queueFlags) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
					{
						return i;
						break;
					}
				}
			}

			// For other queue types or if no separate compute queue is present, return the first one to support the requested flags
			for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
			{
				if (queueFamilyProperties[i].queueFlags & queueFlags)
				{
					return i;
					break;
				}
			}

#if defined(__ANDROID__)
			//todo : Exceptions are disabled by default on Android (need to add LOCAL_CPP_FEATURES += exceptions to Android.mk), so for now just return zero
			return 0;
#else
			throw std::runtime_error("Could not find a matching queue family index");
#endif
		}


		uint32_t getMemoryType(dvk_device* dev, uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound)
		{
			auto& memoryProperties = dev->_info->memoryProperties;
			for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
			{
				if ((typeBits & 1) == 1)
				{
					if ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
					{
						if (memTypeFound)
						{
							*memTypeFound = true;
						}
						return i;
					}
				}
				typeBits >>= 1;
			}

#if defined(__ANDROID__)
			//todo : Exceptions are disabled by default on Android (need to add LOCAL_CPP_FEATURES += exceptions to Android.mk), so for now just return zero
			if (memTypeFound)
			{
				*memTypeFound = false;
			}
			return 0;
#else
			if (memTypeFound)
			{
				*memTypeFound = false;
				return 0;
			}
			else
			{
				throw std::runtime_error("Could not find a matching memory type");
			}
#endif
		}


		VkResult createLogicalDevice(dvk_device* dev, VkPhysicalDeviceFeatures enabledFeatures, std::vector<const char*> enabledExtensions, bool useSwapChain = true
									 , VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT)
		{
			// Desired queues need to be requested upon logical device creation
			// Due to differing queue family configurations of Vulkan implementations this can be a bit tricky, especially if the application
			// requests different queue types
			assert(dev && dev->physicalDevice);
			if (dev && dev->_dev)
			{
				return VK_SUCCESS;
			}
			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
			auto& queueFamilyIndices = dev->queueFamilyIndices;
			// Get queue family indices for the requested queue family types
			// Note that the indices may overlap depending on the implementation

			const float defaultQueuePriority(0.0f);

			// Graphics queue
			if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
			{
				auto gp = get_queue_fp(dev, VK_QUEUE_GRAPHICS_BIT);
				queueFamilyIndices.graphics = getQueueFamilyIndex(dev, VK_QUEUE_GRAPHICS_BIT);
				VkDeviceQueueCreateInfo queueInfo{};
				queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueInfo.queueFamilyIndex = queueFamilyIndices.graphics;
				queueInfo.queueCount = gp->queueCount;
				queueInfo.pQueuePriorities = &defaultQueuePriority;
				queueCreateInfos.push_back(queueInfo);
			}
			else
			{
				queueFamilyIndices.graphics = 0;// VK_NULL_HANDLE;
			}

			// Dedicated compute queue
			if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
			{
				queueFamilyIndices.compute = getQueueFamilyIndex(dev, VK_QUEUE_COMPUTE_BIT);
				if (queueFamilyIndices.compute != queueFamilyIndices.graphics)
				{
					// If compute family index differs, we need an additional queue create info for the compute queue
					VkDeviceQueueCreateInfo queueInfo{};
					auto gp = get_queue_fp(dev, VK_QUEUE_COMPUTE_BIT);
					queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
					queueInfo.queueFamilyIndex = queueFamilyIndices.compute;
					queueInfo.queueCount = gp->queueCount;
					queueInfo.pQueuePriorities = &defaultQueuePriority;
					queueCreateInfos.push_back(queueInfo);
				}
			}
			else
			{
				// Else we use the same queue
				queueFamilyIndices.compute = queueFamilyIndices.graphics;
			}

			// Dedicated transfer queue
			if (requestedQueueTypes & VK_QUEUE_TRANSFER_BIT)
			{
				queueFamilyIndices.transfer = getQueueFamilyIndex(dev, VK_QUEUE_TRANSFER_BIT);
				if ((queueFamilyIndices.transfer != queueFamilyIndices.graphics) && (queueFamilyIndices.transfer != queueFamilyIndices.compute))
				{
					// If compute family index differs, we need an additional queue create info for the compute queue
					VkDeviceQueueCreateInfo queueInfo{};
					auto gp = get_queue_fp(dev, VK_QUEUE_TRANSFER_BIT);
					queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
					queueInfo.queueFamilyIndex = queueFamilyIndices.transfer;
					queueInfo.queueCount = gp->queueCount;
					queueInfo.pQueuePriorities = &defaultQueuePriority;
					queueCreateInfos.push_back(queueInfo);
				}
			}
			else
			{
				// Else we use the same queue
				queueFamilyIndices.transfer = queueFamilyIndices.graphics;
			}

			// Dedicated sparse_binding queue
			if (requestedQueueTypes & VK_QUEUE_SPARSE_BINDING_BIT)
			{
				queueFamilyIndices.sparse_binding = getQueueFamilyIndex(dev, VK_QUEUE_SPARSE_BINDING_BIT);
				if ((queueFamilyIndices.sparse_binding != queueFamilyIndices.graphics) && (queueFamilyIndices.sparse_binding != queueFamilyIndices.compute))
				{
					// If compute family index differs, we need an additional queue create info for the compute queue
					VkDeviceQueueCreateInfo queueInfo{};
					auto gp = get_queue_fp(dev, VK_QUEUE_SPARSE_BINDING_BIT);
					queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
					queueInfo.queueFamilyIndex = queueFamilyIndices.sparse_binding;
					queueInfo.queueCount = gp->queueCount;
					queueInfo.pQueuePriorities = &defaultQueuePriority;
					queueCreateInfos.push_back(queueInfo);
				}
			}
			else
			{
				// Else we use the same queue
				queueFamilyIndices.sparse_binding = queueFamilyIndices.graphics;
			}


			// Create the logical device representation
			std::vector<const char*> deviceExtensions(enabledExtensions);
			if (useSwapChain)
			{
				// If the device will be used for presenting to a display via a swapchain we need to request the swapchain extension
				deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
			}

			VkDeviceCreateInfo deviceCreateInfo = {};
			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());;
			deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
			//deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

			// Enable the debug marker extension if it is present (likely meaning a debugging tool is present)
			if (dev->extensionSupported(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
			{
				deviceExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
				dev->enableDebugMarkers = true;
			}

			if (deviceExtensions.size() > 0)
			{
				deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
				deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
			}
			VkResult result;
			{
				print_time ftpt("vkCreateDevice");
				result = vkCreateDevice(dev->physicalDevice, &deviceCreateInfo, nullptr, &dev->_dev);
			}
			if (result == VK_SUCCESS)
			{
				// Create a default command pool for graphics command buffers
				//commandPool = createCommandPool(queueFamilyIndices.graphics);
			/*	compute_commandPool = createCommandPool(queueFamilyIndices.compute ? queueFamilyIndices.compute : queueFamilyIndices.graphics);

				GetDeviceQueue();*/
				dev->_sampler = createSampler(dev, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT);
			}
			return result;
		}


		VkResult create_buffer(dvk_device* dev, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags
							   , size_t size, VkBuffer* buffer, VkDeviceMemory* memory, void* data, size_t* cap_size)
		{

			// Create the buffer handle
			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.usage = usageFlags;
			bufferCreateInfo.size = size;
			bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			VK_CHECK_RESULT(vkCreateBuffer(dev->_dev, &bufferCreateInfo, nullptr, buffer));

			// Create the memory backing up the buffer handle
			VkMemoryRequirements memReqs;
			VkMemoryAllocateInfo memAlloc = {};
			memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			vkGetBufferMemoryRequirements(dev->_dev, *buffer, &memReqs);
			//if (memReqs.size < 4096)
			//{
			//	memAlloc.allocationSize = 4096;
			//}
			//else
			{
				memAlloc.allocationSize = memReqs.size;
			}
			// Find a memory type index that fits the properties of the buffer
			VkBool32 memTypeFound = 0;
			memAlloc.memoryTypeIndex = vkc::getMemoryType(dev, memReqs.memoryTypeBits, memoryPropertyFlags, &memTypeFound);
			printf("create_buffer\t%d\t%d\n", (int)memAlloc.allocationSize, size);
			if (size == 284908)
			{
				size = size;
			}
			VK_CHECK_RESULT(vkAllocateMemory(dev->_dev, &memAlloc, nullptr, memory));

			if (cap_size)
			{
				*cap_size = size;// memAlloc.allocationSize;
			}
			// If a pointer to the buffer data has been passed, map the buffer and copy over the data
			// 如果已传递指向缓冲区数据的指针，请映射缓冲区并在数据上进行复制
			if (data != nullptr && !(memoryPropertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
			{
				void* mapped = 0;
				VK_CHECK_RESULT(vkMapMemory(dev->_dev, *memory, 0, size, 0, &mapped));
				memcpy(mapped, data, size);
				// 如果没有请求主机一致性，请手动刷新以使写入可见
				// If host coherency hasn't been requested, do a manual flush to make writes visible
				if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
				{
					VkMappedMemoryRange mappedRange = {};
					mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
					mappedRange.memory = *memory;
					mappedRange.offset = 0;
					mappedRange.size = size;
					vkFlushMappedMemoryRanges(dev->_dev, 1, &mappedRange);
				}
				vkUnmapMemory(dev->_dev, *memory);
			}

			// Attach the memory to the buffer object
			VK_CHECK_RESULT(vkBindBufferMemory(dev->_dev, *buffer, *memory, 0));

			return VK_SUCCESS;
		}
		//---------------------------------------------------------------------------------------------
		//创建采样器
		bool createSampler(dvk_device* dev, VkSampler* sampler, VkSamplerCreateInfo* info)
		{
			VkSamplerCreateInfo sampler_create_info = {
				VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,                // VkStructureType            sType
				nullptr,                                              // const void*                pNext
				0,                                                    // VkSamplerCreateFlags       flags
				VK_FILTER_LINEAR,                                     // VkFilter                   magFilter
				VK_FILTER_LINEAR,                                     // VkFilter                   minFilter
				VK_SAMPLER_MIPMAP_MODE_LINEAR,                       // VkSamplerMipmapMode        mipmapMode
				VK_SAMPLER_ADDRESS_MODE_REPEAT,                // VkSamplerAddressMode       addressModeU
				VK_SAMPLER_ADDRESS_MODE_REPEAT,                // VkSamplerAddressMode       addressModeV
				VK_SAMPLER_ADDRESS_MODE_REPEAT,                // VkSamplerAddressMode       addressModeW
				0.0f,                                                 // float                      mipLodBias
				VK_FALSE,                                             // VkBool32                   anisotropyEnable
				1.0f,                                                 // float                      maxAnisotropy
				VK_FALSE,                                             // VkBool32                   compareEnable
				VK_COMPARE_OP_ALWAYS,                                 // VkCompareOp                compareOp
				0.0f,                                                 // float                      minLod
				0.0f,                                                 // float                      maxLod
				VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,              // VkBorderColor              borderColor
				VK_FALSE                                              // VkBool32                   unnormalizedCoordinates
			};
			return vkCreateSampler(dev->_dev, info ? info : &sampler_create_info, nullptr, sampler) == VK_SUCCESS;
		}

		VkSampler createSampler(dvk_device* dev, VkFilter filter = VK_FILTER_LINEAR, VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT)
		{
			VkSampler sampler = 0;
			// Create sampler
			VkSamplerCreateInfo samplerCreateInfo = {};
			samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerCreateInfo.magFilter = filter;
			samplerCreateInfo.minFilter = filter;
			samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			//samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			//samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			//samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerCreateInfo.addressModeU = samplerCreateInfo.addressModeV = samplerCreateInfo.addressModeW = addressMode;
			samplerCreateInfo.mipLodBias = 0.0f;
			samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
			samplerCreateInfo.minLod = 0.0f;
			samplerCreateInfo.maxLod = 0.0f;
			samplerCreateInfo.compareEnable = VK_FALSE;
			// Enable anisotropic filtering
			samplerCreateInfo.maxAnisotropy = 8;
			samplerCreateInfo.anisotropyEnable = VK_FALSE;
			samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;// VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

			VkSamplerCreateInfo sampler_create_info = {
				VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,                // VkStructureType            sType
				nullptr,                                              // const void*                pNext
				0,                                                    // VkSamplerCreateFlags       flags
				VK_FILTER_LINEAR,                                     // VkFilter                   magFilter
				VK_FILTER_LINEAR,                                     // VkFilter                   minFilter
				VK_SAMPLER_MIPMAP_MODE_LINEAR,                       // VkSamplerMipmapMode        mipmapMode
				VK_SAMPLER_ADDRESS_MODE_REPEAT,						  // VkSamplerAddressMode       addressModeU
				VK_SAMPLER_ADDRESS_MODE_REPEAT,						  // VkSamplerAddressMode       addressModeV
				VK_SAMPLER_ADDRESS_MODE_REPEAT,						  // VkSamplerAddressMode       addressModeW
				0.0f,                                                 // float                      mipLodBias
				VK_FALSE,                                             // VkBool32                   anisotropyEnable
				1.0f,                                                 // float                      maxAnisotropy
				VK_FALSE,                                             // VkBool32                   compareEnable
				VK_COMPARE_OP_ALWAYS,                                 // VkCompareOp                compareOp
				0.0f,                                                 // float                      minLod
				0.0f,                                                 // float                      maxLod
				VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,              // VkBorderColor              borderColor
				VK_FALSE                                              // VkBool32                   unnormalizedCoordinates
			};
			VK_CHECK_RESULT(vkCreateSampler(dev->_dev, &sampler_create_info, nullptr, &sampler));
			return sampler;
		}
		//创建图像
		int64_t createImage(dvk_device* dev, VkImageCreateInfo* imageinfo, VkImageViewCreateInfo* viewinfo
							, dvk_texture* texture, VkSampler* sampler = nullptr, VkSamplerCreateInfo* info = nullptr)
		{
			VkImage* image = &texture->_image;
			VkDeviceMemory mem = texture->deviceMemory;
			VkImageView* imageview;
			VkMemoryAllocateInfo memAlloc = {};
			memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			VkMemoryRequirements memReqs;
			if (texture->_image)
			{
				vkDestroyImage(dev->_dev, texture->_image, 0); texture->_image = 0;
			}
			VK_CHECK_RESULT(vkCreateImage(dev->_dev, imageinfo, nullptr, image));
			vkGetImageMemoryRequirements(dev->_dev, texture->_image, &memReqs);
			// 设备内存分配空间小于需求的重新申请内存
			if (texture->cap_device_mem_size < memReqs.size || (texture->caps < texture->cap_inc))
			{
				texture->cap_inc = 0;
				if (texture->deviceMemory)
				{
					vkFreeMemory(dev->_dev, texture->deviceMemory, 0);
					texture->deviceMemory = 0;
				}
				memAlloc.allocationSize = memReqs.size;
				texture->cap_device_mem_size = memReqs.size;
				VkBool32 memTypeFound = 0;
				memAlloc.memoryTypeIndex = vkc::getMemoryType(dev, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memTypeFound);
				auto hr = vkAllocateMemory(dev->_dev, &memAlloc, nullptr, &mem);
				assert(hr == VK_SUCCESS);
				texture->deviceMemory = mem;
			}
			texture->cap_inc++;
			// 绑定显存
			VK_CHECK_RESULT(vkBindImageMemory(dev->_dev, texture->_image, mem, 0));

			viewinfo->image = texture->_image;
			if (texture->view)
			{
				vkDestroyImageView(dev->_dev, texture->view, 0);
			}
			VK_CHECK_RESULT(vkCreateImageView(dev->_dev, viewinfo, nullptr, &texture->view));
			if (sampler)
				createSampler(dev, sampler, info);
			return memAlloc.allocationSize;
		}


		//---------------------------------------------------------------------------------------------
		//创建信号
		void createSemaphore(dvk_device* dev, VkSemaphore* semaphore, VkSemaphoreCreateInfo* semaphoreCreateInfo)
		{
			VkSemaphoreCreateInfo semaphore_create_info = {
				VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,      // VkStructureType          sType
				nullptr,                                      // const void*              pNext
				0                                             // VkSemaphoreCreateFlags   flags
			};
			if (!semaphoreCreateInfo)
				semaphoreCreateInfo = &semaphore_create_info;
			VK_CHECK_RESULT(vkCreateSemaphore(dev->_dev, semaphoreCreateInfo, nullptr, semaphore));
		}

		VkFence createFence(dvk_device* dev, VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT)
		{
			VkFenceCreateInfo fence_create_info = {
				VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,              // VkStructureType                sType
				nullptr,                                          // const void                    *pNext
				flags                      // VkFenceCreateFlags             flags
			};

			VkFence fence = 0;
			VK_CHECK_RESULT(vkCreateFence(dev->_dev, &fence_create_info, nullptr, &fence));
			return fence;
		}
		// 创建命令池
		VkCommandPool createCommandPool(VkDevice dev, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
		{
			VkCommandPoolCreateInfo cmdPoolInfo = {};
			cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			cmdPoolInfo.queueFamilyIndex = queueFamilyIndex;
			cmdPoolInfo.flags = createFlags;
			VkCommandPool cmdPool;
			VK_CHECK_RESULT(vkCreateCommandPool(dev, &cmdPoolInfo, nullptr, &cmdPool));
			if (cmdPool)
			{
				//_cmdpools.insert(cmdPool);
			}
			return cmdPool;
		}

		bool checkCommandBuffers(std::vector<VkCommandBuffer>& drawCmdBuffers)
		{
			for (auto& cmdBuffer : drawCmdBuffers)
			{
				if (cmdBuffer == VK_NULL_HANDLE)
				{
					return false;
				}
			}
			return true;
		}

		bool clearCommandBuffers(std::vector<VkCommandBuffer>& drawCmdBuffers)
		{
			for (auto& cmdBuffer : drawCmdBuffers)
			{
				cmdBuffer = VK_NULL_HANDLE;
			}
			return true;
		}
		void createCommandBuffers(VkDevice dev, VkCommandPool commandPool, std::vector<VkCommandBuffer>& drawCmdBuffers)
		{
			// Create one command buffer for each swap chain image and reuse for rendering
			//drawCmdBuffers.resize(swapChain.imageCount);
			if (drawCmdBuffers.empty())return;
			clearCommandBuffers(drawCmdBuffers);
			/*if (!checkCommandBuffers(drawCmdBuffers))
				destroyCommandBuffers(drawCmdBuffers);*/
			VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
			cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			cmdBufAllocateInfo.commandPool = commandPool;
			cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			cmdBufAllocateInfo.commandBufferCount = drawCmdBuffers.size();
			VK_CHECK_RESULT(vkAllocateCommandBuffers(dev, &cmdBufAllocateInfo, drawCmdBuffers.data()));
		}

		void destroyCommandBuffers(VkDevice dev, VkCommandPool commandPool, std::vector<VkCommandBuffer>& drawCmdBuffers)
		{
			vkFreeCommandBuffers(dev, commandPool, static_cast<uint32_t>(drawCmdBuffers.size()), drawCmdBuffers.data());
			int n = drawCmdBuffers.size();
			for (size_t i = 0; i < drawCmdBuffers.size(); i++)
			{
				VkCommandBuffer c = drawCmdBuffers[i];
				drawCmdBuffers[i] = 0;
			}
		}
		VkCommandBuffer createCommandBuffer1(VkDevice dev, VkCommandPool commandPool, VkCommandBufferLevel level, bool begin)
		{
			VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};

			cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			cmdBufAllocateInfo.commandPool = commandPool;
			cmdBufAllocateInfo.level = level;
			cmdBufAllocateInfo.commandBufferCount = 1;
			VkCommandBuffer cmdBuffer = 0;
			VK_CHECK_RESULT(vkAllocateCommandBuffers(dev, &cmdBufAllocateInfo, &cmdBuffer));

			// If requested, also start recording for the new command buffer
			if (begin)
			{
				VkCommandBufferBeginInfo cmdBufInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, 0, 0, 0};// hz::initializers::commandBufferBeginInfo();
				VK_CHECK_RESULT(vkBeginCommandBuffer(cmdBuffer, &cmdBufInfo));
			}

			return cmdBuffer;
		}

		void flushCommandBuffer(VkDevice dev, VkCommandBuffer commandBuffer, VkCommandPool commandPool, VkQueue queue, bool free)
		{
			if (commandBuffer == VK_NULL_HANDLE)
			{
				return;
			}

			VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));
			//waitIdle();
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			// Create fence to ensure that the command buffer has finished executing
			VkFenceCreateInfo fenceInfo = {};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = 0;
			VkFence fence;
			VK_CHECK_RESULT(vkCreateFence(dev, &fenceInfo, nullptr, &fence));

			// Submit to the queue
			VK_CHECK_RESULT0(vkQueueSubmit(queue, 1, &submitInfo, fence));


			// Wait for the fence to signal that command buffer has finished executing
			VK_CHECK_RESULT(vkWaitForFences(dev, 1, &fence, VK_TRUE, 100000000000));

			vkDestroyFence(dev, fence, nullptr);

			if (free)
			{
				vkFreeCommandBuffers(dev, commandPool, 1, &commandBuffer);
				commandBuffer = 0;
			}
		}

		VkPipelineShaderStageCreateInfo load_shader(dvk_device* dev, const char* data, size_t size, size_t stage, const char* pName)
		{
			VkShaderModule shader_module = dev->CreateShaderModule((char*)data, size);
			VkPipelineShaderStageCreateInfo shader_stage_create_infos =
			{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,        // VkStructureType                                sType
				nullptr,                                                    // const void                                    *pNext
				0,                                                          // VkPipelineShaderStageCreateFlags               flags
				(VkShaderStageFlagBits)stage,                                 // VkShaderStageFlagBits                          stage
				shader_module,                                 // VkShaderModule                                 module
				pName,                                                     // const char                                    *pName
				nullptr                                                     // const VkSpecializationInfo                    *pSpecializationInfo
			};
			return shader_stage_create_infos;
		}


		void get_blend(bool blend, VkPipelineColorBlendAttachmentState& out)
		{
			VkPipelineColorBlendAttachmentState color_blend =
			{
				VK_TRUE,                                                     // VkBool32                                       blendEnable
				VK_BLEND_FACTOR_SRC_ALPHA,                                    // VkBlendFactor                                  srcColorBlendFactor
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,                          // VkBlendFactor                                  dstColorBlendFactor
				VK_BLEND_OP_ADD,                                              // VkBlendOp                                      colorBlendOp
				VK_BLEND_FACTOR_ONE,                                          // VkBlendFactor                                  srcAlphaBlendFactor
				VK_BLEND_FACTOR_ZERO,                                         // VkBlendFactor                                  dstAlphaBlendFactor
				VK_BLEND_OP_ADD,                                              // VkBlendOp                                      alphaBlendOp
				VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |         // VkColorComponentFlags                          colorWriteMask
				VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
			};
			VkPipelineColorBlendAttachmentState color_no_blend = {
				VK_FALSE,                                                     // VkBool32                                       blendEnable
				VK_BLEND_FACTOR_ONE,                                          // VkBlendFactor                                  srcColorBlendFactor
				VK_BLEND_FACTOR_ZERO,                                         // VkBlendFactor                                  dstColorBlendFactor
				VK_BLEND_OP_ADD,                                              // VkBlendOp                                      colorBlendOp
				VK_BLEND_FACTOR_ONE,                                          // VkBlendFactor                                  srcAlphaBlendFactor
				VK_BLEND_FACTOR_ZERO,                                         // VkBlendFactor                                  dstAlphaBlendFactor
				VK_BLEND_OP_ADD,                                              // VkBlendOp                                      alphaBlendOp
				VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |         // VkColorComponentFlags                          colorWriteMask
				VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
			};
			out = blend ? color_blend : color_no_blend;
		}

	};
	//!vkc


/*
vulkan 渲染器


vulkan流程:
1. 创建VkInstance。
2. 选择一个显卡(VkPhysicalDevice)
3. 为绘画和显示创建一个VkDevice和VkQueue。
4. 创建一个window、window_surface 和 swap_chain。
5. 用image_view 包裹swap_chain里的image。
6. 创建一个render_pass ,用它来定义渲染目标和目标的用法。
7. 为render_pass 创建一个frameBuffer。(需要的image_view来自swap_chain或纹理的)
8. 创建graphics pipeline。
9. 为每一个可能的swap_chain_image 绘画命令分配和记录command_buffer。
10.通过获取的image 来draw_frame,提交正确的绘画command_buffer，最后将绘画结果(image)返回到swap_chain。
*/
	class physical_dev_info
	{
	public:
		// 渲染表面
		VkSurfaceKHR surfaces;
		// 设备信息
		VkFormat colorFormat;
		VkColorSpaceKHR colorSpace;
		uint32_t queueNodeIndex = UINT32_MAX;
		uint32_t desiredNumberOfSwapchainImages;
		VkSurfaceTransformFlagsKHR preTransform;
		VkCompositeAlphaFlagBitsKHR compositeAlpha;
		VkPresentModeKHR present_mode[2] = {};
		// 支持的刷新模式
		std::set<VkPresentModeKHR> presentModes;
	};

	// todo vk_info
	class vk_base_info
	{
	public:
		VkInstance _instance = nullptr;
		//dQueue* dqueue = 0;
		//GPU gpu;
		std::vector<dvk_device> devs;
		std::vector<VkLayerProperties> layer_properties;
		std::vector<std::vector<VkLayerProperties>> physical_layer_properties;

		std::vector<physical_dev_info> physical_infos;
		bool _is_one = false;
		gpu_info* _gpu = 0;
	public:
		vk_base_info()
		{
		}

		~vk_base_info()
		{
		}
		void enum_lp()
		{
			uint32_t nc = 0;
			vkEnumerateInstanceLayerProperties(&nc, nullptr);
			if (nc > 0)
			{
				layer_properties.resize(nc);
				vkEnumerateInstanceLayerProperties(&nc, layer_properties.data());
			}
		}
		void get_dlp(VkPhysicalDevice pd, std::vector<VkLayerProperties>& out)
		{
			uint32_t ncd = 0;
			vkEnumerateDeviceLayerProperties(pd, &ncd, nullptr);
			if (ncd > 0)
			{
				out.resize(ncd);
				vkEnumerateDeviceLayerProperties(pd, &ncd, out.data());
			}
		}
		int init(gpu_info* gpu)
		{
			_gpu = gpu;
			//_is_one = is_one;
			_instance = vkc::CreateInstance();
			enum_lp();
			gpu->init(_instance);
			//auto& gis = *((std::vector<VkPhysicalDeviceProperties>*)gpu->_gpu_infos);
			auto gpsn = gpu->gpusname.size();
			if (gpsn)
			{
				physical_layer_properties.resize(gpsn);
				auto& gpn = gpu->gpusname;
				printf("所有显卡：");
				int idx = 0, ic = 0;
				auto n = gpsn;
				printf("%d\n", n);
				devs.resize(n);
				for (auto& [k, v] : gpn) {
					printf("%s\n", k.c_str());
					devs[ic].physicalDevice = v.pd;
					ic++;
				}

			}
			else {
				return -2;
			}
			return 0;
			//dqueue->createDevice(_instance, gpu.selectedDevice(""));
			//dev = dqueue->device[0];
			//dqueue->setSurface(surface);// , gpu.selectedDevice(""));
			//dqueue->createSwapChain(width_, height_);

			//VkSwapchainKHR sc = dqueue->get_swapChain();
			// 交换链视图
			//std::vector<dvk_swapchain::SwapChainBuffer>* psc = dqueue->swapChain.getSwapChainVs();
			//v.main(glfw_, surface, instance, dev->_dev, sc, psc);
			//return 0;

		}

		int init_dev(const std::string n, int idx, bool one_compute)
		{
			//auto gps = _gpu->get_physicals();
			//auto& gis = *((std::vector<VkPhysicalDeviceProperties>*)_gpu->_gpu_infos);
			auto& gpn = _gpu->gpusname;

			if (!devs[idx]._dev)
			{
				auto& it = gpn[n];
				devs[idx].init(_instance, one_compute);
				devs[idx]._limits = new VkPhysicalDeviceLimits(it.pdp->limits);
				get_dlp(it.pd, physical_layer_properties[idx]);
			}
			return idx;
		}
		VkSurfaceKHR newSwapchain(void* platformWindow_, void* glfw_)
		{
			VkResult err;
			VkSurfaceKHR surface = 0;
#ifdef GLFWAPI
#if (HZ_TARGET_PLATFORM==HZ_PLATFORM_WIN32)
			if (glfw_)
			{
				if (glfwCreateWindowSurface(_instance, (GLFWwindow*)glfw_, 0, &surface) != VK_SUCCESS)
				{
					return;
				}
			}
			else
#endif
#endif
			{
				// Create the os-specific surface
#ifdef _WIN32
				VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
				surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
				surfaceCreateInfo.hinstance = (HINSTANCE)GetModuleHandleW(NULL);
				surfaceCreateInfo.hwnd = (HWND)platformWindow_;
				err = vkCreateWin32SurfaceKHR(_instance, &surfaceCreateInfo, nullptr, &surface);

#endif
#ifdef __ANDROID__
				ANativeWindow* window = (ANativeWindow*)platformWindow_;
				VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = {};
				surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
				surfaceCreateInfo.window = window;
				//if (surface)
				//	vkDestroySurfaceKHR(instance, surface, 0);
				surface = 0;
				err = vkCreateAndroidSurfaceKHR(_instance, &surfaceCreateInfo, NULL, &surface);
				LOGE("vkCreateAndroidSurfaceKHR: err:%d,s %p,w %p, p %p,flags:%d\n", (int)err, surface, window, (int)surfaceCreateInfo.flags);
#endif
			}
			return surface;
		}
		bool initSurface(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, physical_dev_info* opd)
		{
			VkResult err;
			// Get available queue family properties
			uint32_t queueCount;

			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, NULL);
			assert(queueCount >= 1);

			std::vector<VkQueueFamilyProperties> queueProps(queueCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queueProps.data());

			// Iterate over each queue to learn whether it supports presenting:
			// Find a queue with present support
			// Will be used to present the swap chain images to the windowing system
			std::vector<VkBool32> supportsPresent(queueCount);
			for (uint32_t i = 0; i < queueCount; i++)
			{
				vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportsPresent[i]);
			}

			// Search for a graphics and a present queue in the array of queue
			// families, try to find one that supports both
			uint32_t graphicsQueueNodeIndex = UINT32_MAX;
			uint32_t presentQueueNodeIndex = UINT32_MAX;
			for (uint32_t i = 0; i < queueCount; i++)
			{
				if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
				{
					if (graphicsQueueNodeIndex == UINT32_MAX)
					{
						graphicsQueueNodeIndex = i;
					}

					if (supportsPresent[i] == VK_TRUE)
					{
						graphicsQueueNodeIndex = i;
						presentQueueNodeIndex = i;
						break;
					}
				}
			}
			if (presentQueueNodeIndex == UINT32_MAX)
			{
				// If there's no queue that supports both present and graphics
				// try to find a separate present queue
				for (uint32_t i = 0; i < queueCount; ++i)
				{
					if (supportsPresent[i] == VK_TRUE)
					{
						presentQueueNodeIndex = i;
						break;
					}
				}
			}

			// Exit if either a graphics or a presenting queue hasn't been found
			if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX)
			{
				hz::vkc::exitFatal("Could not find a graphics and/or presenting queue!", "Fatal error");
			}

			// todo : Add support for separate graphics and presenting queue
			if (graphicsQueueNodeIndex != presentQueueNodeIndex)
			{
				hz::vkc::exitFatal("Separate graphics and presenting queues are not supported yet!", "Fatal error");
			}

			opd->queueNodeIndex = graphicsQueueNodeIndex;

			// Get list of supported surface formats
			uint32_t formatCount;
			err = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);
			assert(!err);
			assert(formatCount > 0);

			std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
			err = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());
			assert(!err);

			// If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
			// there is no preferered format, so we assume VK_FORMAT_B8G8R8A8_UNORM
			if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
			{
				opd->colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
				opd->colorSpace = surfaceFormats[0].colorSpace;
			}
			else
			{
				// iterate over the list of available surface format and
				// check for the presence of VK_FORMAT_B8G8R8A8_UNORM
				bool found_B8G8R8A8_UNORM = false;
				for (auto&& surfaceFormat : surfaceFormats)
				{
					if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
					{
						opd->colorFormat = surfaceFormat.format;
						opd->colorSpace = surfaceFormat.colorSpace;
						found_B8G8R8A8_UNORM = true;
						break;
					}
				}

				// in case VK_FORMAT_B8G8R8A8_UNORM is not available
				// select the first available color format
				if (!found_B8G8R8A8_UNORM)
				{
					opd->colorFormat = surfaceFormats[0].format;
					opd->colorSpace = surfaceFormats[0].colorSpace;
				}
			}
			if (!surface /*|| _width == *width && _height == *height*/)
				return false;

			if (!physicalDevice)return false;
			// Get physical device surface properties and formats
			VkSurfaceCapabilitiesKHR surfCaps;
			VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfCaps));
			//assert(!err);

			// Get available present modes
			uint32_t presentModeCount;
			err = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);
			assert(!err);
			assert(presentModeCount > 0);

			std::vector<VkPresentModeKHR> presentModes(presentModeCount);

			err = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
			assert(!err);
			for (auto it : presentModes)
			{
				opd->presentModes.insert(it);
			}
			//bool ret = false;
			//VkExtent2D swapchainExtent = {};
			//// If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be set by the swapchain
			//if (surfCaps.currentExtent.width == (uint32_t)-1)
			//{
			//	// If the surface size is undefined, the size is set to
			//	// the size of the images requested.
			//	swapchainExtent.width = *width;
			//	swapchainExtent.height = *height;
			//}
			//else
			//{
			//	// If the surface size is defined, the swap chain size must match
			//	swapchainExtent = surfCaps.currentExtent;
			//	*width = surfCaps.currentExtent.width;
			//	*height = surfCaps.currentExtent.height;
			//	ret = true;
			//}

			// Select a present mode for the swapchain
			{
				// The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
				// This mode waits for the vertical blank ("v-sync")
				VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
				opd->present_mode[1] = VK_PRESENT_MODE_FIFO_KHR;
				// If v-sync is not requested, try to find a mailbox mode
				// It's the lowest latency non-tearing present mode available
				//if (!vsync)
				{
					for (size_t i = 0; i < presentModeCount; i++)
					{
						if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
						{
							swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
							break;
						}
						if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
						{
							swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
						}
					}
				}
				opd->present_mode[0] = swapchainPresentMode;
				// Determine the number of images
				auto& desiredNumberOfSwapchainImages = opd->desiredNumberOfSwapchainImages;
				desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
				if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
				{
					desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
				}

				// Find the transformation of the surface
				VkSurfaceTransformFlagsKHR& preTransform = opd->preTransform;
				if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
				{
					// We prefer a non-rotated transform
					preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
				}
				else
				{
					preTransform = surfCaps.currentTransform;
				}

				// Find a supported composite alpha format (not all devices support alpha opaque)
				auto& compositeAlpha = opd->compositeAlpha;
				compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
				// Simply select the first composite alpha format available
				std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
					VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
					VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
					VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
					VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
				};
				for (auto& compositeAlphaFlag : compositeAlphaFlags) {
					if (surfCaps.supportedCompositeAlpha & compositeAlphaFlag) {
						compositeAlpha = compositeAlphaFlag;
						break;
					};
				}
			}
			return true;
		}
		void padd_binding(int idx, int binding, uint32_t descriptor_type, uint8_t count, uint8_t stage_flags
						  , std::map<int, std::map<int, desc_type>>& bindings, BindInfo* bindinfo)
		{
			desc_type dt = {};
			dt.descriptorType = descriptor_type;
			dt.descriptorCount = count;
			dt.stageFlags = stage_flags;
			if (bindinfo)
			{
				if (bindinfo->dtype.stageFlags)
				{
					dt.stageFlags |= bindinfo->dtype.stageFlags;
				}
				bindinfo->binding = binding;
				bindinfo->set = idx;
				bindinfo->dtype = dt;
			}
			auto& setm = bindings[idx];
			setm[binding] = dt;
		}
		uint32_t spirv2type(spirv_cross::SPIRType::BaseType a)
		{
			uint32_t ret = 0;
			switch (a)
			{
			case spirv_cross::SPIRType::Unknown:
				break;
			case spirv_cross::SPIRType::Void:
				break;
			case spirv_cross::SPIRType::Boolean:
				break;
			case spirv_cross::SPIRType::SByte:
				break;
			case spirv_cross::SPIRType::UByte:
				break;
			case spirv_cross::SPIRType::Short:
				break;
			case spirv_cross::SPIRType::UShort:
				break;
			case spirv_cross::SPIRType::Int:
				break;
			case spirv_cross::SPIRType::UInt:
				break;
			case spirv_cross::SPIRType::Int64:
				break;
			case spirv_cross::SPIRType::UInt64:
				break;
			case spirv_cross::SPIRType::AtomicCounter:
				break;
			case spirv_cross::SPIRType::Half:
				break;
			case spirv_cross::SPIRType::Float:
				break;
			case spirv_cross::SPIRType::Double:
				break;
			case spirv_cross::SPIRType::Struct:
				break;
			case spirv_cross::SPIRType::Image:
				ret = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				break;
			case spirv_cross::SPIRType::SampledImage:
				ret = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				break;
			case spirv_cross::SPIRType::Sampler:
				ret = VK_DESCRIPTOR_TYPE_SAMPLER;
				break;
			case spirv_cross::SPIRType::AccelerationStructureNV:
				break;
			case spirv_cross::SPIRType::ControlPointArray:
				break;
			case spirv_cross::SPIRType::Char:
				break;
			default:
				break;
			}
			return ret;
		}
		uint32_t get_format(spirv_cross::SPIRType::BaseType basetype, int vecsize)
		{
			static uint32_t _unorm8[4] = {VK_FORMAT_R8_UNORM, VK_FORMAT_R8G8_UNORM, VK_FORMAT_R8G8B8_UNORM, VK_FORMAT_R8G8B8A8_UNORM};
			static uint32_t _uint32[4] = {VK_FORMAT_R32_UINT, VK_FORMAT_R32G32_UINT, VK_FORMAT_R32G32B32_UINT, VK_FORMAT_R32G32B32A32_UINT};
			static uint32_t _sint32[4] = {VK_FORMAT_R32_SINT, VK_FORMAT_R32G32_SINT, VK_FORMAT_R32G32B32_SINT, VK_FORMAT_R32G32B32A32_SINT};
			static uint32_t _uint64[4] = {VK_FORMAT_R64_UINT, VK_FORMAT_R64G64_UINT, VK_FORMAT_R64G64B64_UINT, VK_FORMAT_R64G64B64A64_UINT};
			static uint32_t _sint64[4] = {VK_FORMAT_R64_SINT, VK_FORMAT_R64G64_SINT, VK_FORMAT_R64G64B64_SINT, VK_FORMAT_R64G64B64A64_SINT};
			static uint32_t _float32[4] = {VK_FORMAT_R32_SFLOAT, VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32B32A32_SFLOAT};
			static uint32_t _float64[4] = {VK_FORMAT_R64_SFLOAT, VK_FORMAT_R64G64_SFLOAT, VK_FORMAT_R64G64B64_SFLOAT, VK_FORMAT_R64G64B64A64_SFLOAT};

			uint32_t ret = 0;
			uint32_t* p = nullptr;
			switch (basetype)
			{
			case spirv_cross::SPIRType::Void:
				p = _unorm8;
				break;
			case spirv_cross::SPIRType::Boolean:
				break;
			case spirv_cross::SPIRType::SByte:
				break;
			case spirv_cross::SPIRType::UByte:
				break;
			case spirv_cross::SPIRType::Short:
				break;
			case spirv_cross::SPIRType::UShort:
				break;
			case spirv_cross::SPIRType::Int:
				p = _sint32;
				break;
			case spirv_cross::SPIRType::UInt:
				p = _uint32;
				break;
			case spirv_cross::SPIRType::Int64:
				p = _sint64;
				break;
			case spirv_cross::SPIRType::UInt64:
				p = _uint64;
				break;
			case spirv_cross::SPIRType::Float:
				p = _float32;
				break;
			case spirv_cross::SPIRType::Double:
				p = _float64;
				break;
			case spirv_cross::SPIRType::Char:
				break;
			default:
				break;
			}
			if (p && vecsize > 0 && vecsize < 5)
			{
				ret = p[vecsize - 1];
			}
			return ret;
		}

		void Process(spirv_cross::Compiler& compiler, spirv_cross::ShaderResources& resources, VkShaderStageFlags stageFlags
					 , spv_res_info* spvi)
		{
			if (!spvi) {
				assert(spvi);
				return;
			}
			auto& input_info = spvi->input_info;
			auto& bindings = spvi->bindings;
			auto& paramsMap = spvi->paramsMap;
			auto& bufferParams = spvi->bufferParams;
			auto& imageParams = spvi->imageParams;
			if (stageFlags & VK_SHADER_STAGE_VERTEX_BIT) {
				// 获取input信息
				for (int i = 0; i < resources.stage_inputs.size(); ++i)
				{
					spirv_cross::Resource& res = resources.stage_inputs[i];
					spirv_cross::SPIRType type = compiler.get_type(res.type_id);
					auto varName = compiler.get_name(res.id);
					int inputAttributeSize = type.vecsize;
					double stride_size = type.width / 8;
					stride_size *= type.vecsize;
					// location必须连续
					int location = compiler.get_decoration(res.id, spv::DecorationLocation);
					int binding = compiler.get_decoration(res.id, spv::DecorationBinding);
					int didx = compiler.get_decoration(res.id, spv::DecorationIndex);
					auto& it = input_info[location];
					it.location = location;
					it.vecsize = type.vecsize;
					auto tbt = type.basetype;
					// 4字节颜色则用uint32_t
					if ((varName == "col" || (varName.find("Color") != std::string::npos) || (varName.find("color") != std::string::npos))
						&& spvi->color_unorm && type.vecsize == 4)
					{
						tbt = spirv_cross::SPIRType::Void;
						stride_size = type.vecsize;
					}
					it.stride_size = stride_size;
					it.name = varName;

					//if ((varName.find("pos") != std::string::npos))
					//{
					//	tbt = spirv_cross::SPIRType::Double;
					//	it.stride_size = sizeof(double) * 3;
					//	//tbt = spirv_cross::SPIRType::Float;
					//}
					it.format = get_format(tbt, type.vecsize);
				}
			}

			// 获取attachment信息
			for (int i = 0; i < resources.subpass_inputs.size(); ++i)
			{
				spirv_cross::Resource& res = resources.subpass_inputs[i];
				spirv_cross::SPIRType type = compiler.get_type(res.type_id);
				spirv_cross::SPIRType base_type = compiler.get_type(res.base_type_id);
				auto varName = compiler.get_name(res.id);

				int sets = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
				int binding = compiler.get_decoration(res.id, spv::DecorationBinding);
				uint32_t descriptor_type = spirv2type(type.basetype);
				uint32_t dt = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
				auto nt = paramsMap[varName];
				padd_binding(sets, binding, dt, 1, stageFlags, bindings, &nt);

				auto it = imageParams.find(varName);
				if (it == imageParams.end())
				{
					spv_res_info::image_info imageInfo = {};
					imageInfo.set = sets;
					imageInfo.binding = binding;
					imageInfo.stageFlags = stageFlags;
					imageInfo.descriptorType = dt;
					imageParams.insert(std::make_pair(varName, imageInfo));
				}
				else
				{
					it->second.stageFlags |= stageFlags;
				}
			}
			// 获取Uniform Buffer信息
			for (int i = 0; i < resources.uniform_buffers.size(); ++i)
			{
				spirv_cross::Resource& res = resources.uniform_buffers[i];
				spirv_cross::SPIRType type = compiler.get_type(res.type_id);
				spirv_cross::SPIRType base_type = compiler.get_type(res.base_type_id);
				auto varName = compiler.get_name(res.id);
				auto typeName = compiler.get_name(res.base_type_id);
				uint32_t uniformBufferStructSize = compiler.get_declared_struct_size(type);

				int sets = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
				int binding = compiler.get_decoration(res.id, spv::DecorationBinding);

				uint32_t descriptor_type = spirv2type(type.basetype);
				auto kn1 = compiler.get_member_name(res.base_type_id, 0);
				auto kn2 = compiler.get_member_name(res.base_type_id, 1);
				auto kn3 = compiler.get_member_name(res.base_type_id, 2);
				auto kn4 = compiler.get_member_name(res.base_type_id, 3);
				auto kn5 = compiler.get_member_name(res.base_type_id, 4);

				// [layout (binding = 0) uniform MVPDynamicBlock] 标记为Dynamic的buffer
				descriptor_type = (typeName.find("u_") != std::string::npos) ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				auto nt = paramsMap[varName];
				padd_binding(sets, binding, descriptor_type, 1, stageFlags, bindings, &nt);

				auto it = bufferParams.find(varName);
				if (it == bufferParams.end())
				{
					spv_res_info::buffer_info info = {};
					info.set = sets;
					info.binding = binding;
					info.stageFlags = stageFlags;
					info.descriptorType = descriptor_type;
					info.bufferSize = uniformBufferStructSize;
					bufferParams.insert(std::make_pair(varName, info));
				}
				else
				{
					it->second.stageFlags |= stageFlags;
				}
			}
			// 获取Texture
			for (int i = 0; i < resources.sampled_images.size(); ++i)
			{
				spirv_cross::Resource& res = resources.sampled_images[i];
				spirv_cross::SPIRType type = compiler.get_type(res.type_id);
				spirv_cross::SPIRType base_type = compiler.get_type(res.base_type_id);
				auto varName = compiler.get_name(res.id);

				int sets = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
				int binding = compiler.get_decoration(res.id, spv::DecorationBinding);
				uint32_t descriptor_type = spirv2type(type.basetype); VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				auto nt = paramsMap[varName];
				padd_binding(sets, binding, descriptor_type, 1, stageFlags, bindings, &nt);

				auto it = imageParams.find(varName);
				if (it == imageParams.end())
				{
					spv_res_info::image_info imageInfo = {};
					imageInfo.set = sets;
					imageInfo.binding = binding;
					imageInfo.stageFlags = stageFlags;
					imageInfo.descriptorType = descriptor_type;
					imageParams.insert(std::make_pair(varName, imageInfo));
				}
				else
				{
					it->second.stageFlags |= stageFlags;
				}
			}



			for (int i = 0; i < resources.storage_images.size(); ++i)
			{
				spirv_cross::Resource& res = resources.storage_images[i];
				spirv_cross::SPIRType type = compiler.get_type(res.type_id);
				spirv_cross::SPIRType base_type = compiler.get_type(res.base_type_id);
				auto varName = compiler.get_name(res.id);

				int sets = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
				int binding = compiler.get_decoration(res.id, spv::DecorationBinding);

				uint32_t descriptor_type = spirv2type(type.basetype); VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				auto nt = paramsMap[varName];
				padd_binding(sets, binding, descriptor_type, 1, stageFlags, bindings, &nt);

				auto it = imageParams.find(varName);
				if (it == imageParams.end())
				{
					spv_res_info::image_info imageInfo = {};
					imageInfo.set = sets;
					imageInfo.binding = binding;
					imageInfo.stageFlags = stageFlags;
					imageInfo.descriptorType = descriptor_type;
					imageParams.insert(std::make_pair(varName, imageInfo));
				}
				else
				{
					it->second.stageFlags |= stageFlags;
				}
			}


			for (int i = 0; i < resources.storage_buffers.size(); ++i)
			{
				spirv_cross::Resource& res = resources.storage_buffers[i];
				spirv_cross::SPIRType type = compiler.get_type(res.type_id);
				spirv_cross::SPIRType base_type = compiler.get_type(res.base_type_id);
				auto varName = compiler.get_name(res.id);
				auto typeName = compiler.get_name(res.base_type_id);
				uint32_t uniformBufferStructSize = compiler.get_declared_struct_size(type);

				int sets = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
				int binding = compiler.get_decoration(res.id, spv::DecorationBinding);

				uint32_t descriptor_type = (typeName.find("u_") != std::string::npos) ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				auto nt = paramsMap[varName];
				padd_binding(sets, binding, descriptor_type, 1, stageFlags, bindings, &nt);
				auto it = bufferParams.find(varName);
				if (it == bufferParams.end())
				{
					spv_res_info::buffer_info info = {};
					info.set = sets;
					info.binding = binding;
					info.stageFlags = stageFlags;
					info.descriptorType = descriptor_type;
					info.bufferSize = uniformBufferStructSize;
					bufferParams.insert(std::make_pair(varName, info));
				}
				else
				{
					it->second.stageFlags |= stageFlags;
				}
			}
			for (int i = 0; i < resources.push_constant_buffers.size(); ++i)
			{
				spirv_cross::Resource& res = resources.push_constant_buffers[i];
				spirv_cross::SPIRType type = compiler.get_type(res.type_id);
				spirv_cross::SPIRType base_type = compiler.get_type(res.base_type_id);
				auto varName = compiler.get_name(res.id);

				printf("");
			}
		}
		// bindings=>set,bind,type
		spv_res_info* get_shader_info(const void* data, size_t size, size_t stage)
		{
			// 反编译Shader获取相关信息
			spirv_cross::Compiler compiler((uint32_t*)data, size / sizeof(uint32_t));
			spirv_cross::ShaderResources resources = compiler.get_shader_resources();
			auto spvinfo = new spv_res_info();
			Process(compiler, resources, stage, spvinfo);
			return spvinfo;
		}
		void load_pipe(void* dev_, pipe_create_info* info, uint16_t count, std::vector<VkPipelineShaderStageCreateInfo>* out)
		{
			auto dev = (dvk_device*)dev_;
			assert(dev && out);
			//dev->new_set_pool();
			for (size_t i = 0; i < count; i++)
			{
				if (info[i].stage == 0)
				{
					info[i].stage = get_stage(info[i].fn);
				}
				auto p = vkc::load_shader(dev, info[i].data, info[i].size, info[i].stage, info[i].pName);
				if (p.module)
				{
					info[i]._module = (uint64_t)p.module;
					info[i].spv_info = get_shader_info(info[i].data, info[i].size, info[i].stage);
					out->push_back(p);
				}
			}
		}
		static uint32_t get_stage(const std::string& str)
		{
			uint32_t ret = 0;
			if (str.find(".vert") != std::string::npos)
			{
				ret = VK_SHADER_STAGE_VERTEX_BIT;
			}
			if (str.find(".frag") != std::string::npos)
			{
				ret = VK_SHADER_STAGE_FRAGMENT_BIT;
			}
			if (str.find(".comp") != std::string::npos)
			{
				ret = VK_SHADER_STAGE_COMPUTE_BIT;
			}
			if (str.find(".geom") != std::string::npos)
			{
				ret = VK_SHADER_STAGE_GEOMETRY_BIT;
			}
			if (str.find(".tese") != std::string::npos)
			{
				ret = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			}
			if (str.find(".tesc") != std::string::npos)
			{
				ret = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			}
			return ret;
		}
	private:

	};
	// todo vk_render_cx
#if 1
	vk_render_cx::vk_render_cx()
	{
		auto pvk = dc.ac<vk_base_info>();
		if (pvk)
		{
			_gpu = dc.ac<gpu_info>();
			pvk->init(_gpu);
			_instance = _gpu->_instance;
		}
		ctx = pvk;

	}
	vk_render_cx::~vk_render_cx()
	{
		ctx = nullptr; _gpu = nullptr;
	}

	fbo_cx* vk_render_cx::new_fbo_swapchain(int width, int height, int* count, VkSurfaceKHR surface, unsigned int clear_color, dvk_device* dev, void* pass)
	{
		fbo_cx* p = dc.ac<fbo_cx>(width, height, clear_color);
		if (p)
		{
			dvk_swapchain* swapchain = dc.ac<dvk_swapchain>(dev, surface);
			uint32_t uw = width, uh = height;
			swapchain->create(&uw, &uh, false);
			p->init(dev, *count, pass, swapchain);
		}
		return p;
	}
	fbo_cx* vk_render_cx::new_fbo_image(int width, int height, int count, unsigned int clear_color, dvk_device* dev, void* pass)
	{
		fbo_cx* p = dc.ac<fbo_cx>(width, height, clear_color);
		if (p)
		{
			p->init(dev, count, pass, nullptr);
		}
		return p;
	}
	dvk_device* vk_render_cx::get_dev(unsigned int idx)
	{
		if (ctx->devs.empty())return nullptr;
		if (idx >= ctx->devs.size())
		{
			idx = ctx->devs.size() - 1;
		}
		//ctx->init_dev(idx, true);
		return &ctx->devs[idx];
	}

	dvk_device* vk_render_cx::get_dev(const char* name)
	{
		int idx = 0;
		auto n = _gpu->get_idx(name, idx);
		ctx->init_dev(n, idx, true);
		return &ctx->devs[idx];
	}

	void vk_render_cx::load_pipe(dvk_device* dev_, pipe_create_info* info, uint16_t count, pipe_info_s* out)
	{
		auto p = (vk_base_info*)ctx;
		auto dev = dev_;
		assert(dev_);
		//auto v = dc.ac<std::vector<VkPipelineShaderStageCreateInfo>>();
		auto v = new std::vector<VkPipelineShaderStageCreateInfo>();
		out->v = v;
		out->dev = dev;
		out->n = count;
		p->load_pipe(dev_, info, count, v);
	}

	bool vk_render_cx::new_pipeline(dvk_device* dev, pipeline_ptr_info* info)
	{
		assert(dev);
		if (!dev)
			return false;

		auto ret = dev->new_pipeline(info);
		return ret;
	}

	void vk_render_cx::free_pipe_info(pipe_info_s* p)
	{
		auto dev = p->dev;
		assert(dev && p);
		if (p && dev)
		{
			auto v = (std::vector<VkPipelineShaderStageCreateInfo>*)p->v;
			for (auto& it : *v)
			{
				dev->free_shader(it.module);
			}
			delete v;
			p->v = 0;
			p->n = 0;
		}
	}

	int vk_render_cx::get_push_constant_size(size_t idx)
	{
		return 128;// _gpu->get_push_constant_size(idx);
	}
	VkInstance vk_render_cx::get_instance()
	{
		return _instance;
	}

#endif
#if 1
	// 2d
#if 0
	class BindPass
	{
	public:
		VkCommandBuffer cmdb = nullptr;
		//PassObj* pao = nullptr;
		//Pipeline* pe = nullptr;
	public:
		BindPass(VkCommandBuffer command_buffer, PassObj* po) :cmdb(command_buffer), pao(po)
		{
			//if (pao)
			//	pe = (Pipeline*)pao->_shader;
		}

		~BindPass()
		{
		}

		void pass_bind()
		{
			if (pao->_shader)
			{
				//pe->bind(cmdb);
			}
		}
		// ubo/纹理
		void bind_dset(VkDescriptorSet* descriptorSet)
		{
			//pe->bind_set(cmdb, descriptorSet);
		}
		void bind_tex(dvk_texture* tex)
		{
			//if (tex && tex->dset)
			//	pe->bind_set(cmdb, &tex->dset);
		}
		void bind_vbo()
		{
			uint32_t idx = 0;
			if (pao->_vbos.size())
			{
				VkBuffer* vbo = (VkBuffer*)pao->_vbos.data();
				while (pao->_offset.size() < pao->_vbos.size())
				{
					pao->_offset.push_back(0);
				}
				vkCmdBindVertexBuffers(cmdb, idx, pao->_vbos.size(), vbo, pao->_offset.data());
			}
		}
		void bind_ibo()
		{
			VkBuffer ibo = (VkBuffer)pao->_ibo;
			if (ibo)
				vkCmdBindIndexBuffer(cmdb, ibo, pao->_ibo_offset, (VkIndexType)pao->_ibo_type);
		}
	private:
	};
#endif

	void mkr2(VkRect2D& s, VkViewport* vp)
	{
		if (s.offset.x < 0)
		{
			s.offset.x = 0;
		}
		if (s.offset.y < 0)
		{
			s.offset.y = 0;
		}
		if (s.extent.width > vp->width)
		{
			s.extent.width = vp->width;
		}
		if (s.extent.height > vp->height)
		{
			s.extent.height = vp->height;
		}
	}

#endif // 1
	// todo fbo
#if 1

	/*
	渲染缓冲
	包含了renderPass创建
	*/
	class Command_guard
	{
	public:
		Command_guard(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo) :
			_commandBuffer(commandBuffer)
		{
			void* bcb = (void*)vkBeginCommandBuffer;
			VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, pBeginInfo));
		}

		~Command_guard()
		{
			VK_CHECK_RESULT(vkEndCommandBuffer(_commandBuffer));
		}
	private:
		VkCommandBuffer _commandBuffer = nullptr;
	};
	class RenderPass_guard
	{
	public:
		RenderPass_guard(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE) :
			_commandBuffer(commandBuffer)
		{
			vkCmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
		}

		~RenderPass_guard()
		{
			vkCmdEndRenderPass(_commandBuffer);
		}

	private:
		VkCommandBuffer _commandBuffer = nullptr;
	};

	class fbo_info_cx
	{
	public:
		class FBO
		{
		public:
			VkFramebuffer framebuffer = 0;
			//深度、模板缓冲
			dvk_texture color;
			dvk_texture depth_stencil;
			VkDescriptorImageInfo descriptor = {};
			// Semaphore used to synchronize between offscreen and final scene rendering
			//信号量用于在屏幕外和最终场景渲染之间进行同步
			VkSemaphore semaphore = VK_NULL_HANDLE;
		public:
			FBO() {}
			~FBO() {}
		};
		size_t count_ = 0;
		int _width = 0, _height = 0;
		dvk_swapchain* _swapc = 0;
		VkRenderPass renderPass = 0, nrp = 0;
		//采样器
		VkSampler sampler = 0;
		// 渲染到纹理同步cpu
		VkFence _fence = 0;
		// 缓冲区列表
		std::vector<FBO> framebuffers;

		dvk_device* _dev = nullptr;
		VkClearValue clearValues[2] = {};
		// VK_FORMAT_B8G8R8A8_UNORM, 浮点纹理 VK_FORMAT_R32G32B32A32_SFLOAT;
		VkFormat depthFormat = VK_FORMAT_D32_SFLOAT_S8_UINT, colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
		bool isColor = false;	//渲染到纹理则为true

		int cmdcount = 0;
		// Command buffers used for rendering
		std::vector<VkCommandBuffer> drawCmdBuffers;
	public:
		fbo_info_cx()
		{
		}

		~fbo_info_cx()
		{
		}

		void setClearValues(uint32_t color, float depth = 1.0f, uint32_t Stencil = 0)
		{
			//float r[] = { vk_R(color) / 255.0f,  vk_G(color) / 255.0f,  vk_B(color) / 255.0f,  vk_A(color) / 255.0f };
			//memcpy(&clearValues[0].color, r, sizeof(float) * 4);
			unsigned char* uc = (unsigned char*)&color;
			clearValues[0].color = {uc[0] / 255.0f, uc[1] / 255.0f, uc[2] / 255.0f, uc[3] / 255.0f};
			clearValues[1].depthStencil = {depth, Stencil};
		}

		void setClearValues(float* color, float depth = 1.0f, uint32_t Stencil = 0)
		{
			clearValues[0].color = {{color[0], color[1], color[2], color[3]}};
			clearValues[1].depthStencil = {depth, Stencil};
		}



		//swapchainbuffers交换链
		void initFBO(int width, int height, VkRenderPass rp, dvk_swapchain* swapc = nullptr)
		{
			//LOGW("FBO:w:%d,h:%d", width, height);
			std::vector<dvk_swapchain::scb_t>* swapchainbuffers = nullptr;
			if (swapc)
			{
				swapchainbuffers = &swapc->buffers;
				colorFormat = (VkFormat)(*swapchainbuffers)[0].colorFormat;
				_swapc = swapc;
			}
			_width = width; _height = height;
			//是否创建颜色缓冲纹理
			isColor = !swapchainbuffers || swapchainbuffers->empty();
			// Create a separate render pass for the offscreen rendering as it may differ from the one used for scene rendering
			resetCommandBuffers();
			//LOGE("func=%s,line=%d,file=%s\n", __FUNCTION__, __LINE__, __FILE__);

			if (!renderPass)
			{
				if (!rp)
				{
					nrp = rp = _dev->new_render_pass(colorFormat, depthFormat);
				}
				renderPass = rp;
			}
			// Create sampler to sample from the color attachments
			VkSamplerCreateInfo samplerinfo = {};
			samplerinfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerinfo.magFilter = VK_FILTER_LINEAR;
			samplerinfo.minFilter = VK_FILTER_LINEAR;
			samplerinfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerinfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerinfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerinfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerinfo.mipLodBias = 0.0f;
			samplerinfo.maxAnisotropy = 0;
			samplerinfo.minLod = 0.0f;
			samplerinfo.maxLod = 1.0f;
			samplerinfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			vkc::createSampler(_dev, &sampler, &samplerinfo);

			//LOGE("func=%s,line=%d,file=%s\n", __FUNCTION__, __LINE__, __FILE__);
			// Create num frame buffers
			resetFramebuffer(width, height, swapchainbuffers);
			//LOGE("func=%s,line=%d,file=%s\n", __FUNCTION__, __LINE__, __FILE__);
		}

		//窗口大小改变时需要重新创建image,如果是交换链则传swapchainbuffers
		void resetFramebuffer(int width, int height, std::vector<dvk_swapchain::scb_t>* swapchainbuffers)
		{
			if (width & 1)
				width++;
			if (height & 1)
				height++;
			_width = width;
			_height = height;
#ifdef _WIN32
			destroyImage();
#endif
			// Color attachment
			VkImageCreateInfo image = {};
			image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image.imageType = VK_IMAGE_TYPE_2D;
			image.format = colorFormat;
			image.extent.width = width;
			image.extent.height = height;
			image.extent.depth = 1;
			image.mipLevels = 1;
			image.arrayLayers = 1;
			image.samples = VK_SAMPLE_COUNT_1_BIT;
			image.tiling = VK_IMAGE_TILING_OPTIMAL;
			// We will sample directly from the color attachment
			image.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

			VkImageViewCreateInfo colorImageView = {};
			colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
			colorImageView.format = colorFormat;
			colorImageView.flags = 0;
			colorImageView.subresourceRange = {};
			colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			colorImageView.subresourceRange.baseMipLevel = 0;
			colorImageView.subresourceRange.levelCount = 1;
			colorImageView.subresourceRange.baseArrayLayer = 0;
			colorImageView.subresourceRange.layerCount = 1;
			if (isColor)
			{
				for (auto& it : framebuffers)
				{
					vkc::createImage(_dev, &image, &colorImageView, &it.color, 0);
					it.color.width = width;
					it.color.height = height;
					it.color._format = colorFormat;
				}
				if (!_fence)
				{
					_fence = vkc::createFence(_dev);
				}
			}
			else
			{
				int i = 0;
				for (auto it : *swapchainbuffers)
				{
					if (framebuffers.size() > i)
					{
						framebuffers[i].color._dev = _dev;
						framebuffers[i].color._image = it.image;
						framebuffers[i].color.view = it.view;
						framebuffers[i].color.width = width;
						framebuffers[i].color.height = height;
						framebuffers[i].color._format = (VkFormat)it.colorFormat;
						i++;
					}
				}
			}
			// Depth stencil attachment
			image.format = depthFormat;
			image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

			VkImageViewCreateInfo depthStencilView = {};
			depthStencilView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
			depthStencilView.format = depthFormat;
			depthStencilView.flags = 0;
			depthStencilView.subresourceRange = {};
			depthStencilView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			depthStencilView.subresourceRange.baseMipLevel = 0;
			depthStencilView.subresourceRange.levelCount = 1;
			depthStencilView.subresourceRange.baseArrayLayer = 0;
			depthStencilView.subresourceRange.layerCount = 1;
			if (depthFormat >= VK_FORMAT_D16_UNORM_S8_UINT) {
				depthStencilView.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
			for (auto& it : framebuffers)
			{
				vkc::createImage(_dev, &image, &depthStencilView, &it.depth_stencil, 0);
				it.depth_stencil.width = width; it.depth_stencil.height = height; it.depth_stencil._format = depthFormat;
			}

			VkImageView attachments[2];
			int inc = 0;
			for (auto& it : framebuffers)
			{
				inc++;
				attachments[0] = it.color.view;
				attachments[1] = it.depth_stencil.view;
				VkFramebufferCreateInfo fbufCreateInfo = {};
				fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				fbufCreateInfo.renderPass = renderPass;
				fbufCreateInfo.attachmentCount = 2;
				fbufCreateInfo.pAttachments = attachments;
				fbufCreateInfo.width = width;
				fbufCreateInfo.height = height;
				fbufCreateInfo.layers = 1;
				VK_CHECK_RESULT(vkCreateFramebuffer(_dev->_dev, &fbufCreateInfo, nullptr, &it.framebuffer));
				// Fill a descriptor for later use in a descriptor set
				it.descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				it.descriptor.imageView = it.color.view;
				it.descriptor.sampler = sampler;
			}
		}
		void reset_fbo(int width, int height)
		{
			resetFramebuffer(width, height, _swapc ? &_swapc->buffers : nullptr);
		}
		void resetCommandBuffers()
		{
			if (count_ < 1)
				return;
			destroy_all();
			framebuffers.resize(count_);
			drawCmdBuffers.resize(count_);
			for (auto& it : framebuffers)
			{
				it.depth_stencil._dev = _dev;
				if (it.semaphore == VK_NULL_HANDLE)
				{
					vkc::createSemaphore(_dev, &it.semaphore, 0);
				}
			}
			if (isColor)
			{
				for (auto& it : framebuffers)
					it.color._dev = _dev;
			}
		}
		// 支持普通全局函数、静态函数、以及lambda表达式void(VkCommandBuffer commandBuffer, size_t index)
		template<typename Function, typename... Args>
		void build_cmd(int idx, const Function& dc_func, Args... args)
		{
			VkCommandBufferBeginInfo cmdBufInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, 0, 0, 0};

			VkRenderPassBeginInfo renderPassBeginInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, 0, 0, 0, 0, 0, 0};
			renderPassBeginInfo.renderPass = renderPass;
			renderPassBeginInfo.renderArea.extent.width = _width;
			renderPassBeginInfo.renderArea.extent.height = _height;
			renderPassBeginInfo.clearValueCount = 2;
			renderPassBeginInfo.pClearValues = clearValues;
			VkViewport viewport{};
			viewport.width = _width;
			viewport.height = _height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor{};
			scissor.extent.width = _width;
			scissor.extent.height = _height;
			scissor.offset.x = 0;
			scissor.offset.y = 0;
			int dcbcount = drawCmdBuffers.size();
			if (/*dc_func &&*/ dcbcount)
			{
				if (idx < 0)
				{
					for (size_t i = 0; i < framebuffers.size(); ++i)
					{
						dvk_cmd dcmd(_dev, drawCmdBuffers[i], false);
						dcmd._viewport = {_width, _height};
						Command_guard _cmd(dcmd.cmdb, &cmdBufInfo);
						renderPassBeginInfo.framebuffer = framebuffers[i].framebuffer;
						{
							RenderPass_guard _rp(dcmd.cmdb, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
							//vkCmdSetViewport(dcmd.cmdb, 0, 1, &viewport);
							//vkCmdSetLineWidth(dcmd.cmdb, 1.0f);
							dc_func(&dcmd, args...);
						}
						//end cmd
					}
				}
				else
				{
					idx = std::min(idx, dcbcount - 1);
					dvk_cmd dcmd(_dev, drawCmdBuffers[idx], false);
					dcmd._viewport = {_width, _height};
					Command_guard _cmd(dcmd.cmdb, &cmdBufInfo);
					renderPassBeginInfo.framebuffer = framebuffers[idx].framebuffer;
					{
						RenderPass_guard _rp(dcmd.cmdb, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
						//vkCmdSetViewport(dcmd.cmdb, 0, 1, &viewport);
						//vkCmdSetLineWidth(dcmd.cmdb, 1.0f);
						dc_func(&dcmd, args...);
					}
					//end cmd
				}
			}
			else
			{
				VkRenderPassBeginInfo renderPassBeginInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, 0, 0, 0, 0, 0, 0};
				renderPassBeginInfo.renderPass = renderPass;
				renderPassBeginInfo.renderArea.offset.x = 0;
				renderPassBeginInfo.renderArea.offset.y = 0;
				renderPassBeginInfo.renderArea.extent.width = _width;
				renderPassBeginInfo.renderArea.extent.height = _height;
				renderPassBeginInfo.clearValueCount = 2;
				renderPassBeginInfo.pClearValues = clearValues;

				for (size_t i = 0; i < framebuffers.size(); ++i)
				{
					// Set target frame buffer
					renderPassBeginInfo.framebuffer = framebuffers[i].framebuffer;
					VK_CHECK_RESULT(vkBeginCommandBuffer(drawCmdBuffers[i], &cmdBufInfo));
					// Draw the particle system using the update vertex buffer
					vkCmdBeginRenderPass(drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);


					vkCmdEndRenderPass(drawCmdBuffers[i]);
					VK_CHECK_RESULT(vkEndCommandBuffer(drawCmdBuffers[i]));
				}
			}

			cmdcount = 1;
		}

		void build_cmd_empty()
		{
			{
				VkCommandBufferBeginInfo cmdBufInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, 0, 0, 0};
				VkRenderPassBeginInfo renderPassBeginInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, 0, 0, 0, 0, 0, 0};
				renderPassBeginInfo.renderPass = renderPass;
				renderPassBeginInfo.renderArea.offset.x = 0;
				renderPassBeginInfo.renderArea.offset.y = 0;
				renderPassBeginInfo.renderArea.extent.width = _width;
				renderPassBeginInfo.renderArea.extent.height = _height;
				renderPassBeginInfo.clearValueCount = 2;
				renderPassBeginInfo.pClearValues = clearValues;

				for (size_t i = 0; i < framebuffers.size(); ++i)
				{

					// Set target frame buffer
					renderPassBeginInfo.framebuffer = framebuffers[i].framebuffer;

					VK_CHECK_RESULT(vkBeginCommandBuffer(drawCmdBuffers[i], &cmdBufInfo));

					// Draw the particle system using the update vertex buffer

					vkCmdBeginRenderPass(drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);


					vkCmdEndRenderPass(drawCmdBuffers[i]);

					VK_CHECK_RESULT(vkEndCommandBuffer(drawCmdBuffers[i]));
				}
			}
		}
	public:

		void destroyImage()
		{
#if __ANDROID__
			_dev->destroyCommandBuffers(drawCmdBuffers);
			//_dev->destroySemaphore(semaphore); semaphore = 0;
			_dev->destroyRenderPass(renderPass); renderPass = 0;
			_dev->destroySampler(sampler); sampler = 0;
#endif // __ANDROID__

			for (auto& it : framebuffers)
			{
				//it.color.destroyBase(isColor);
				//it.depth_stencil.destroyBase(true);
				if (it.framebuffer)
					vkDestroyFramebuffer(_dev->_dev, it.framebuffer, 0);
				it.framebuffer = 0;
			}
		}
		void destroy_all()
		{
			destroyImage();

#if _WIN32
			//vkc::destroyCommandBuffers(_dev,drawCmdBuffers);
			if (nrp)
			{
				//_dev->destroyRenderPass(nrp); nrp = 0;
			}
			renderPass = 0;
			//_dev->destroySampler(sampler); sampler = 0;
#endif // __ANDROID__
			for (auto& it : framebuffers)
			{
				//_dev->destroySemaphore(it.semaphore); it.semaphore = 0;
			}
		}
	private:

	};

	fbo_cx::fbo_cx(int width, int height, unsigned int clear_color) :_width(width), _height(height)
		, _clear_color(clear_color)//, _fixed_gs_time(.60)
	{
		_submitinfo = new submit_info();
		dc.push(_submitinfo);
		//camera = hz::Camera::create();
		// dx 11 需要true
		// vk 要用false
		//camera->set_top(false);
	}
	fbo_cx::~fbo_cx()
	{
		_submitinfo = 0;
		ctx = 0;
	}
	//获取深度类型
	VkBool32 getSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat* depthFormat)
	{
		// Since all depth formats may be optional, we need to find a suitable depth format to use
		// Start with the highest precision packed format
		std::vector<VkFormat> depthFormats = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM
		};

		for (auto& format : depthFormats)
		{
			VkFormatProperties formatProps;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);
			// Format must support depth stencil attachment for optimal tiling
			if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				*depthFormat = format;
				return true;
			}
		}

		return false;
	}

	void fbo_cx::init(dvk_device* dev, int count, void* pass, dvk_swapchain* swapchain)
	{
		int k = sizeof(VkPipelineShaderStageCreateInfo);
		auto scp = (dvk_swapchain*)swapchain;
		_swapcain = swapchain;
		ctx = new fbo_info_cx();
		dc.push(ctx);
		ctx->_dev = (dvk_device*)dev;
		_dev = dev;
		// Find a suitable depth format
		VkBool32 validDepthFormat = getSupportedDepthFormat(ctx->_dev->physicalDevice, &ctx->depthFormat);
		assert(validDepthFormat);
		//默认颜色
		ctx->clearValues[0].color = {{0.0f, 0.0f, 0.0f, 0.0f}};
		//默认深度、模板
		ctx->clearValues[1].depthStencil = {1.0f, 0};
		if (scp)
		{
			count = scp->imageCount;
		}

		{
			ctx->count_ = count;
		}

		vkc::createSemaphore(_dev, &_presentComplete, 0);
		ctx->initFBO(_width, _height, (VkRenderPass)pass, scp);
		if (camera)
			camera->setOrtho(_width, _height);

		ctx->setClearValues(_clear_color);

	}
	void fbo_cx::set_queue(dvk_queue* qp, game_time* gt)
	{
		_gt = gt;
		_queue = qp;
		cmd_pool = _queue->new_cmd_pool();
		cmd_pool->new_cmd(ctx->drawCmdBuffers, false);
		dc.push(cmd_pool);
	}
	void fbo_cx::set_clear_color(unsigned int clear_color)
	{
		_clear_color = clear_color;
		ctx->setClearValues(_clear_color);
	}
	/*
	ve创建VBO、纹理、shader之类
	ve->set_fbo(fbo);
	ve->set_clear_values(0x00ff5020);
	ve->buildCommandBuffers();	//生成渲染命令\执行vkCmdDrawIndexed

	*/
	void fbo_cx::build_cmd(int idx, std::function<void(dvk_cmd* cmd)> cb)
	{
		ctx->build_cmd(idx, cb);
	}
	void fbo_cx::build_cmd_cb()
	{
		ctx->build_cmd(-1, [=](dvk_cmd* cmd) { draw_call_cb(cmd); });
	}
	void fbo_cx::submit(int idx)
	{
		//ctx->submit(idx);	//提交GPU执行渲染
	}

	void fbo_cx::save_image(const std::string& fn)
	{
		//ctx->save_image(fn);
	}
	void fbo_cx::to_image(Image* img, int idx)
	{
		//ctx->to_image(img, idx);
	}

	dvk_device* fbo_cx::get_dev()
	{
		return _dev;
	}

	VkRenderPass fbo_cx::get_render_pass()
	{
		return  ctx->renderPass;
	}
	dvk_swapchain* fbo_cx::get_swapchain()
	{
		return _swapcain;
	}
	glm::vec2 fbo_cx::get_size()
	{
		return glm::vec2(_width, _height);
	}
	glm::vec2 fbo_cx::get_size2()
	{
		return glm::vec2(_width & 1 ? _width + 1 : _width, _height & 1 ? _height + 1 : _height);
	}
	glm::mat4* fbo_cx::get_ortho_graphic()
	{
		return camera->getOrthographic();
	}
	//void fbo_cx::set_drawcb(std::function<void(dvk_cmd* cmd)> cb)
	//{
	//	draw_call_cbs.push_back(cb);
	//}
	void fbo_cx::draw_call_cb(dvk_cmd* cmd)
	{
		for (auto it = draw_call_cbs.begin(); it != draw_call_cbs.end(); it++)
		{
			if (*it)
			{
				(*it)(cmd);
			}
		}
	}
	void fbo_cx::set_ud(int idx, void* ud)
	{
		uds[idx] = ud;
	}
	void* fbo_cx::get_ud(int idx)
	{
		return uds[idx];
	}
	void fbo_cx::set_ortho_ubo(dvk_buffer* ubo, uint32_t offset)
	{
		_ortho_ubo = ubo; _offset = offset;
	}
	void fbo_cx::acquire(uint32_t* imageIndex)
	{
		_swapcain->acquireNextImage(_presentComplete, imageIndex);
	}
	submit_info* fbo_cx::get_cur_submit()
	{
		auto idx = _swapcain->_curr_imageIndex;
		auto& fb = ctx->framebuffers[idx];
		_submitinfo->clear();
		_submitinfo->push_cmd(ctx->drawCmdBuffers[idx], fb.semaphore, _presentComplete, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
		return _submitinfo;
	}
	bool fbo_cx::resize(int width, int height)
	{
		if (width > 0 && height > 0)
		{
			r_width = width;
			r_height = height;
		}
		_isreset++;
		// todo 判断是否同一线程
		return (_gt && _gt->nc_tid(hz::get_tid()));
	}
	bool fbo_cx::resize_view(int width, int height)
	{
		return false;
	}
	void fbo_cx::begin()
	{
	}
	void fbo_cx::end()
	{
	}
	bool fbo_cx::dev_resize(bool isout)
	{
		if (!(r_width * r_height > 0))
		{
			return false;
		}
		if (isout || _isreset > 0 && (r_width != _width || r_height != _height))
		{
			uint32_t uw = r_width, uh = r_height;
#ifdef _DEBUG
			print_time dwi("vkDeviceWaitIdle");
#endif
			_dev->wait_idle();
			_swapcain->create(&uw, &uh);
			_width = _swapcain->_width;
			_height = _swapcain->_height;
			if (_width > 0)
			{

				assert(_width > 0);
				ctx->reset_fbo(_width, _height);
				//camera->setOrtho(_width, _height);
				// 更新ubo
				_isreset--; inc_update();
			}
			return true;
		}
		return false;
	}
	void fbo_cx::update_ubo(glm::mat4& m)
	{
		_ortho_ubo->set_data(&m, sizeof(glm::mat4), _offset, true);
	}
	bool fbo_cx::is_update()
	{
		return 0 < _incup;
	}
	void fbo_cx::inc_update()
	{
		_incup++;
	}
	void fbo_cx::dec_update()
	{
		_incup = 0;
	}
	void fbo_cx::set_gs()
	{
		_is_getsize = true;
	}
	void fbo_cx::close_gs()
	{
		_is_getsize = false;
	}
	bool fbo_cx::is_gs()
	{
		bool ret = _is_getsize;
		return ret;
	}
#endif

	// todo set_pool_info
#if 1
	set_pool_info::set_pool_info()
	{
	}
	set_pool_info::~set_pool_info()
	{
		if (_dev && _dev->_dev)
		{
			for (auto it : set_ptr) {
				if (it)
					vkDestroyDescriptorSetLayout(_dev->_dev, it, 0);
			}
			for (auto it : pools) {
				if (it)
					vkDestroyDescriptorPool(_dev->_dev, it, 0);
			}
			if (pipelineLayout)
				vkDestroyPipelineLayout(_dev->_dev, pipelineLayout, 0);
		}
	}
	void set_pool_info::set_push_constant_size(int vert_size, int frag_size)
	{
		pct_vert = vert_size;
		pct_frag = frag_size;
		push_constant_size = vert_size + frag_size;
		if (push_constant_size < 128)
		{
			push_constant_size = 128;
		}
	}
	// VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_FRAGMENT_BIT|VK_SHADER_STAGE_COMPUTE_BIT
	void set_pool_info::add_set(uint8_t stage_flags)
	{
		sets.push_back(get_stage(stage_flags));
	}
	uint32_t set_pool_info::get_stage(uint8_t stage_flags)
	{
		uint32_t stage = 0;
		if (stage_flags & 1)
		{
			stage |= VK_SHADER_STAGE_VERTEX_BIT;
		}
		if (stage_flags & 2)
		{
			stage |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		if (stage_flags & 4)
		{
			stage |= VK_SHADER_STAGE_COMPUTE_BIT;
		}
		return stage;
	}
	void set_pool_info::add_ubo(int idx, int binding, uint8_t count, uint8_t stage_flags, bool dynamic)
	{
		add_binding(idx, binding, dynamic ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, count, stage_flags);
	}
	void set_pool_info::add_image(int idx, int binding, uint8_t count, uint8_t stage_flags)
	{
		add_binding(idx, binding, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, count, stage_flags);
	}
	// VkDescriptorType
	void set_pool_info::add_binding(int idx, int binding, uint32_t descriptor_type, uint8_t count, uint8_t stage_flags)
	{
		desc_type dt = {};
		dt.descriptorType = descriptor_type;
		dt.descriptorCount = count;
		dt.stageFlags = get_stage(stage_flags);
		auto& setm = _binding[idx];
		setm[binding] = dt;
	}

	void set_pool_info::set_binding(std::map<int, std::map<int, desc_type>>& binding)
	{
		// set,bind,type
		if (binding.empty())return;
		sets.clear();
		_binding = binding;
		for (auto& [st, bt] : binding)
		{
			uint32_t stage = 0;
			for (auto& [b, dt] : bt)
			{
				stage |= dt.stageFlags;
			}
			sets.push_back(stage);
		}
	}

	void set_pool_info::make_set_pool(dvk_device* dev)
	{
		_dev = dev;
		assert(dev);
		dev->new_set_pool(this);
	}

	std::vector<VkDescriptorSet> set_pool_info::new_sets()
	{
		std::vector<VkDescriptorSet> out;
		new_sets(out, 0, set_ptr.size());
		return out;
	}

	size_t set_pool_info::new_sets(std::vector<VkDescriptorSet>& out)
	{
		return new_sets(out, 0, set_ptr.size());
	}

	size_t set_pool_info::new_sets(std::vector<VkDescriptorSet>& out, int idx, int n)
	{
		auto dev = _dev;
		assert(dev && idx >= 0);
		std::vector<VkDescriptorSet> dset;
		int spsize = set_ptr.size();
		int count = n;
		bool isfull = false;
		if (idx == 0 && n < 1)
		{
			count = spsize;
			isfull = true;
		}
		if (idx > 0 && n < 1)
		{
			count = n = 1;
		}
		if (set_ptr.size() > 0 && idx < set_ptr.size())
		{
			do
			{
				if (pools_idx == pools.size())
				{
					pools.push_back(dev->new_desc_pool(_ps, max_sets * spsize));// 32 * 2
				}
				if (pools_idx < pools.size())
				{
					if (dev->alloc_set((VkDescriptorPool)pools[pools_idx], (VkDescriptorSetLayout*)&set_ptr[idx], isfull ? spsize : 1, dset))
					{
						n -= spsize;
						for (auto it : dset)
						{
							out.push_back(it);
						}
						dset.clear();
					}
					else if (dset.size())
					{
						pools_idx++;
					}
					else
					{
						dset.clear();
						break;
					}
				}
			} while (n > 0);
		}
		return out.size();
	}

	void set_pool_info::test()
	{
		set_pool_info spool[4];
		{
			// 布局设置
			for (size_t i = 0; i < 3; i++)
			{
				auto sps = &spool[i];
				// vertex set=0，如果1|2则vert\frag都可访问
				sps->add_set((uint8_t)stage_bit::vertex);
				// fragment set=1只在片断访问
				sps->add_set((uint8_t)stage_bit::fragment);
				// layout(set=0, binding=0) uniform ubo;
				sps->add_ubo(0, 0, 1, (uint8_t)stage_bit::vertex, false);
				// layout(set=1, binding=0) uniform sampler2D srcSampler;
				sps->add_image(1, 0, 1, (uint8_t)stage_bit::fragment);
			}
			// layout(set=1, binding=1) uniform sampler2D srcSampler1;
			int sets = 1, binging = 1, count = 1;
			spool[2].add_image(sets, binging, count, (uint8_t)stage_bit::fragment);
		}
		{
			spool[3].add_set((uint8_t)stage_bit::vertex | (uint8_t)stage_bit::fragment);
			// layout(set=0, binding=0)
			spool[3].add_ubo(0, 0, 1, (uint8_t)stage_bit::vertex, false);
			spool[3].add_image(0, 0, 1, (uint8_t)stage_bit::fragment);
		}
	}

#endif // 1

	pipeline_ptr_info::pipeline_ptr_info()
	{
	}

	pipeline_ptr_info::~pipeline_ptr_info()
	{
	}

	void pipeline_ptr_info::init(VkRenderPass rpass)
	{
		if (rpass)
			render_pass = rpass;
	}

	void pipeline_ptr_info::add_input_vbo(int n, int t)
	{
		vinput_info.push_back({n, t});
	}


	dvk_cmd_pool::dvk_cmd_pool()
	{
	}

	dvk_cmd_pool::dvk_cmd_pool(VkCommandPool cp, dvk_device* dev) :command_pool(cp), _dev(dev)
	{

	}
	dvk_cmd_pool::~dvk_cmd_pool()
	{
		auto ps = cmdv.data();
		for (auto n : _count)
		{
			_dev->free_cmd(command_pool, ps, n);
			ps += n;
		}
		_count.clear();
		cmdv.clear();
		vkDestroyCommandPool(_dev->_dev, command_pool, 0);
		command_pool = 0;
	}

	void dvk_cmd_pool::init(VkCommandPool cp, dvk_device* dev)
	{
		command_pool = cp; _dev = dev;
	}
	void dvk_cmd_pool::new_cmd(std::vector<VkCommandBuffer>& out, bool secondary)
	{
		//VK_COMMAND_BUFFER_LEVEL_PRIMARY: 可以提交到队列执行，但不能从其他的命令缓冲区调用。
		//	VK_COMMAND_BUFFER_LEVEL_SECONDARY : 无法直接提交，但是可以从主命令缓冲区调用。
		auto dev = _dev;
		if (out.empty())out.resize(1);
		dev->new_cmd((VkCommandPool)command_pool, secondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY, (VkCommandBuffer*)out.data(), out.size());
		if (out.size())
		{
			cmdv.insert(cmdv.end(), out.begin(), out.end());
			_count.push_back(out.size());
		}
	}
	dvk_cmd* dvk_cmd_pool::new_cmd(bool secondary, bool isc)
	{
		std::vector<VkCommandBuffer> rs;
		new_cmd(rs, secondary);
		if (rs.empty() || !rs[0])
		{
			return nullptr;
		}
		auto p = new dvk_cmd(_dev, rs[0], isc);
		return p;
	}
	// -------------------------------------------------------------------------------------------------------------------
	// todo compute 着色器
	class dvk_compute
	{
	public:

		struct simulateBuffer
		{
			std::vector<uint8_t>		dataContent;
			bool                    global = false;
			uint32_t					dataSize = 0;
			uint32_t					set = 0;
			uint32_t					binding = 0;
			uint32_t					dynamicIndex = 0;
			VkDescriptorType		descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			VkShaderStageFlags		stageFlags = 0;
			VkDescriptorBufferInfo	bufferInfo;
		};

		struct simulateTexture
		{
			uint32_t              set = 0;
			uint32_t              binding = 0;
			VkDescriptorType    descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			VkShaderStageFlags  stageFlags = 0;
			dvk_texture* texture = nullptr;
		};
		typedef std::unordered_map<std::string, simulateBuffer>    BuffersMap;
		typedef std::unordered_map<std::string, simulateTexture>   TexturesMap;
	public:

		dvk_device* _dev = nullptr;
		shader_info* _shader = nullptr;

		VkPipelineCache             pipelineCache = 0;
		VkPipeline                  pipeline = 0;

		//dvk_set* descriptorSet = nullptr;

		uint32_t                      dynamicOffsetCount = 0;
		std::vector<uint32_t>         dynamicOffsets;

		BuffersMap					uniformBuffers;
		BuffersMap					storageBuffers;
		TexturesMap                 textures;


	private:
		dvk_buffer* ringBuffer = nullptr;
		//DVKRingBuffer* ringBuffer;
	public:
		dvk_compute(dvk_device* dev) :_dev(dev)
		{
		}

		~dvk_compute()
		{
		}

		static dvk_compute* create(dvk_device* dev, VkPipelineCache pipelineCache, shader_info* shader)
		{
			auto processor = new dvk_compute(dev);
			processor->_shader = shader;
			processor->pipelineCache = pipelineCache;
			processor->Prepare();
			processor->PreparePipeline();
			return processor;
		}

		void bind(dvk_cmd* cmd, dvk_set* dsp)
		{
			assert(pipeline);
			bool is_compute = true;
			std::vector<VkDescriptorSet> ds;
			for (auto& [k, v] : dsp->_dset)
			{
				ds.push_back(v[0]);
			}
			auto dsptr = (VkDescriptorSet*)ds.data();// dsp->descriptorSets.data();
			uint32_t* dynOffsets = dsp->dynamicOffsets.data();
			uint32_t dynamicOffsetCount = dsp->dynamicOffsets.size();
			uint32_t bindPoint = uint32_t(!is_compute ? VK_PIPELINE_BIND_POINT_GRAPHICS : VK_PIPELINE_BIND_POINT_COMPUTE);
			auto c = (VkCommandBuffer)cmd->ptr();
			vkCmdBindPipeline(c, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
			vkCmdBindDescriptorSets(c, (VkPipelineBindPoint)bindPoint, GetPipelineLayout(), 0, dsp->_dset.size(), dsptr, dynamicOffsetCount, dynOffsets);
		}
		void push_constants(VkCommandBuffer command_buffer, const void* pValues, uint32_t size, uint32_t offset)
		{
			VkShaderStageFlags stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
			vkCmdPushConstants(command_buffer, GetPipelineLayout(), stageFlags, offset, size, pValues);
		}
#if 0
		void SetUniform(const std::string& name, void* dataPtr, uint32_t size)
		{

			auto it = uniformBuffers.find(name);
			if (it == uniformBuffers.end())
			{
				// MLOGE("Uniform %s not found.", name.c_str());
				return;
			}

			if (it->second.dataSize != size)
			{
				// MLOGE("Uniform %s size not match, dst=%ud src=%ud", name.c_str(), it->second.dataSize, size);
				return;
			}
			// 拷贝数据至ringbuffer
			//uint8_t* ringCPUData = (uint8_t*)(ringBuffer->GetMappedPointer());
			//uint64_t ringOffset = ringBuffer->AllocateMemory(it->second.dataSize);
			//uint64_t bufferSize = it->second.dataSize;

			// 拷贝数据
			//memcpy(ringCPUData + ringOffset, dataPtr, bufferSize);

			// 记录Offset
			//dynamicOffsets[it->second.dynamicIndex] = ringOffset;
		}
		void SetTexture(const std::string& name, dvk_texture* texture)
		{

			auto it = textures.find(name);
			if (it == textures.end())
			{
				// MLOGE("Texture %s not found.", name.c_str());
				return;
			}

			if (texture == nullptr)
			{
				// MLOGE("Texture %s can't be null.", name.c_str());
				return;
			}

			if (it->second.texture != texture)
			{
				it->second.texture = texture;
				descriptorSet->write_image(name, texture);
			}
		}


		void SetStorageTexture(const std::string& name, dvk_texture* texture)
		{
			SetTexture(name, texture);

		}
		void SetStorageBuffer(const std::string& name, Buffer* buffer)
		{
			auto it = storageBuffers.find(name);
			if (it == storageBuffers.end())
			{
				//// MLOGE("StorageBuffer %s not found.", name.c_str());
				return;
			}

			if (buffer == nullptr)
			{
				//// MLOGE("StorageBuffer %s can't be null.", name.c_str());
				return;
			}

			if (it->second.bufferInfo.buffer != buffer->buffer[0])
			{
				it->second.dataSize = buffer->size;
				it->second.bufferInfo.buffer = buffer->buffer[0];
				it->second.bufferInfo.offset = 0;
				it->second.bufferInfo.range = buffer->size;
				descriptorSet->write_buffer(name, buffer);
			}

		}
		VkDescriptorSet* GetDescriptorSets() const
		{
			return (VkDescriptorSet*)descriptorSet->descriptorSets.data();
		}
		size_t get_set_count() {
			return descriptorSet->descriptorSets.size();
		}
#endif
		VkPipeline GetPipeline() const
		{
			return pipeline;
		}

		VkPipelineLayout GetPipelineLayout() const
		{
			return (VkPipelineLayout)_shader->_spinfo.pipelineLayout;
		}


		dvk_set* new_set()
		{
			return  _shader->new_dvkset();
		}
	private:
		void InitRingBuffer(dvk_device* dev)
		{
			_dev = (dvk_device*)dev;
			//ringBuffer = _dev->new_buffer(dvkObjType::E_UBO, 8 * 1024 * 1024, nullptr);
		}
		void DestroyRingBuffer()
		{
			delete ringBuffer;
			ringBuffer = nullptr;
		}
		void Prepare()
		{
			// 创建descriptorSet
			//descriptorSet = _shader->new_dvkset();
#if 1
			auto& bufferParams = _shader->_srinfo.bufferParams;
			auto& imageParams = _shader->_srinfo.imageParams;
			// 从Shader获取Buffer信息
#if 0
			for (auto it = bufferParams.begin(); it != bufferParams.end(); ++it)
			{
				simulateBuffer uboBuffer = {};
				uboBuffer.binding = it->second.binding;
				uboBuffer.descriptorType = (VkDescriptorType)it->second.descriptorType;
				uboBuffer.set = it->second.set;
				uboBuffer.stageFlags = it->second.stageFlags;
				uboBuffer.dataSize = it->second.bufferSize;
				uboBuffer.bufferInfo = {};
				uboBuffer.bufferInfo.buffer = ringBuffer->buffer[0];
				uboBuffer.bufferInfo.offset = 0;
				uboBuffer.bufferInfo.range = uboBuffer.dataSize;

				if (it->second.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
					it->second.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
				{
					uniformBuffers.insert(std::make_pair(it->first, uboBuffer));
					descriptorSet->write_buffer(it->first, &(uboBuffer.bufferInfo));
				}
				else if (it->second.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
						 it->second.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC)
				{
					storageBuffers.insert(std::make_pair(it->first, uboBuffer));
				}
			}
#endif
			// 设置Offset的索引,DynamicOffset的顺序跟set和binding顺序相关
			dynamicOffsetCount = 0;
			//std::vector<DVKDescriptorSetLayoutInfo>& setLayouts = _shader->setLayoutsInfo.setLayouts;
			auto& setlayouts = _shader->_srinfo.bindings;
			//std::vector<std::vector<VkDescriptorSetLayoutBinding>> set_layout_binding;
			for (auto it = uniformBuffers.begin(); it != uniformBuffers.end(); ++it)
			{
				auto st = _shader->_srinfo.get_dt(it->second.set, it->second.binding);
				if (it->second.descriptorType == st.descriptorType && it->second.stageFlags == st.stageFlags)
				{
					it->second.dynamicIndex = dynamicOffsetCount;
					dynamicOffsetCount += 1;
					break;
				}
			}
#if 0
			for (int i = 0; i < setLayouts.size(); ++i)
			{
				std::vector<VkDescriptorSetLayoutBinding>& bindings = setLayouts[i].bindings;
				for (int j = 0; j < bindings.size(); ++j)
				{
					for (auto it = uniformBuffers.begin(); it != uniformBuffers.end(); ++it)
					{
						if (it->second.set == setLayouts[i].set &&
							it->second.binding == bindings[j].binding &&
							it->second.descriptorType == bindings[j].descriptorType &&
							it->second.stageFlags == bindings[j].stageFlags
							)
						{
							it->second.dynamicIndex = dynamicOffsetCount;
							dynamicOffsetCount += 1;
							break;
						}
					}
				}
			}
#endif
			dynamicOffsets.resize(dynamicOffsetCount);

			// 从Shader中获取Texture信息，包含attachment信息
			for (auto it = imageParams.begin(); it != imageParams.end(); ++it)
			{
				simulateTexture texture = {};
				texture.texture = nullptr;
				texture.binding = it->second.binding;
				texture.descriptorType = (VkDescriptorType)it->second.descriptorType;
				texture.set = it->second.set;
				texture.stageFlags = it->second.stageFlags;
				textures.insert(std::make_pair(it->first, texture));
			}
#endif
		}
		void PreparePipeline()
		{
			VkDevice device = _dev->_dev;
			if (pipeline != 0)
			{
				vkDestroyPipeline(device, pipeline, nullptr);
				pipeline = 0;// VK_NULL_HANDLE;
			}

			std::vector<VkPipelineShaderStageCreateInfo> shader_infos;
			auto& sv = *((std::vector<VkPipelineShaderStageCreateInfo>*)_shader->shader_data->v);
			assert(sv.size() > 0);
			for (auto it : _shader->shader_idx) {
				if (it >= 0 && it < sv.size())
					shader_infos.push_back(sv[it]);
			}
			if (shader_infos.empty())
			{
				return;
			}
			VkComputePipelineCreateInfo computeCreateInfo = {};
			computeCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
			computeCreateInfo.layout = (VkPipelineLayout)_shader->get_pipelineLayout();
			computeCreateInfo.stage = shader_infos[0];
			auto hr = vkCreateComputePipelines(device, pipelineCache, 1, &computeCreateInfo, nullptr, &pipeline);
			if (pipeline)
			{
				_shader->_pipe[0] = pipeline;
			}
		}
	public:

	};

	dvk_set::dvk_set(dvk_device* dev, spv_res_info* p, shader_info* ps) :_dev(dev), setLayoutsInfo(p), _pipe(ps)
	{
	}

	//dvk_set::dvk_set()
	//{
	//}

	dvk_set::~dvk_set()
	{

	}

	ubo_set_t dvk_set::write_image(const std::string& name, dvk_texture* tex, VkDescriptorSet dstSet)
	{
		assert(tex);
		ubo_set_t ret = {};
		auto it = setLayoutsInfo->imageParams.find(name);
		if (it != setLayoutsInfo->imageParams.end() || !tex)
		{
			//// MLOGE("Failed write buffer, %s not found!", name.c_str());
			auto bindInfo = it->second;
			ret.dset = write_image(bindInfo.set, bindInfo.binding, bindInfo.descriptorType, tex, dstSet);
			ret.first_set = bindInfo.set;
		}
		return ret;
	}

	VkDescriptorSet dvk_set::write_image(int set, int binding, uint32_t descriptorType, dvk_texture* tex, VkDescriptorSet dstSet)
	{
		auto dev = _dev;
		auto& it = _dset[set];
		auto ac = setLayoutsInfo->bindings.size();
		// set为一个时创建新set,或者多个set时每次创建新set
		if (!dstSet) {
			if (ac == 1 && it.empty() || ac > 1)
			{
				std::vector<VkDescriptorSet> vd;
				_pipe->new_sets(vd, set, 1);
				if (vd.empty())
				{
					return 0;
				}
				it.push_back(vd[0]);
			}
			dstSet = *it.rbegin();
		}
		VkWriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = dstSet;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.descriptorType = (VkDescriptorType)descriptorType;
		writeDescriptorSet.pBufferInfo = nullptr;
		writeDescriptorSet.pImageInfo = (VkDescriptorImageInfo*)tex->get_descriptor_image_info();
		writeDescriptorSet.dstBinding = binding;
		writeDescriptorSet.dstArrayElement = 0;
		_cy_info.push_back(bset_info{set, binding, descriptorType, tex});
		_first_set[dstSet] = set;
		vkUpdateDescriptorSets(dev->_dev, 1, &writeDescriptorSet, 0, nullptr);
		return  dstSet;
	}
#if 0
	void dvk_set::write_buffer(const std::string& name, const void* bufferInfo)
	{
		auto it = setLayoutsInfo->bufferParams.find(name);
		if (it == setLayoutsInfo->bufferParams.end())
		{
			//// MLOGE("Failed write buffer, %s not found!", name.c_str());
			return;
		}

		auto bindInfo = it->second;
		auto dt = bindInfo.descriptorType;
		auto dev = (dvk_device*)_dev;
		VkWriteDescriptorSet writeDescriptorSet;
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = (VkDescriptorSet)descriptorSets[bindInfo.set];
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.descriptorType = (VkDescriptorType)dt;
		writeDescriptorSet.pBufferInfo = (VkDescriptorBufferInfo*)bufferInfo;
		writeDescriptorSet.dstBinding = bindInfo.binding;
		vkUpdateDescriptorSets(dev->_dev, 1, &writeDescriptorSet, 0, nullptr);
	}
#endif

	ubo_set_t dvk_set::write_buffer(const std::string& name, dvk_buffer* buffer_, uint64_t size, uint64_t offset, VkDescriptorSet dstSet)
	{
		ubo_set_t ret = {};
		auto it = setLayoutsInfo->bufferParams.find(name);
		if (it != setLayoutsInfo->bufferParams.end())
		{
			//// MLOGE("Failed write buffer, %s not found!", name.c_str());
			auto bindInfo = it->second;
			ret.dset = write_buffer(bindInfo.set, bindInfo.binding, bindInfo.descriptorType, buffer_, size, offset, dstSet);
			ret.first_set = bindInfo.set;
		}
		return ret;
	}

	VkDescriptorSet dvk_set::write_buffer(int set, int binding, uint32_t descriptorType, dvk_buffer* buffer, uint64_t size, uint64_t offset, VkDescriptorSet dstSet)
	{
		auto dev = (dvk_device*)_dev;

		auto& it = _dset[set];
		auto ac = setLayoutsInfo->bindings.size();
		// set为一个时创建新set,或者多个set时每次创建新set
		if (!dstSet)
		{
			if (ac == 1 && it.empty() || ac > 1)
			{
				std::vector<VkDescriptorSet> vd;
				_pipe->new_sets(vd, set, 1);
				if (vd.empty())
				{
					return 0;
				}
				it.push_back(vd[0]);
			}
			dstSet = *it.rbegin();
		}
		VkDescriptorBufferInfo dp[1] = {};
		dp->buffer = buffer->buffer;
		dp->offset = offset;
		dp->range = size;
		VkWriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = dstSet;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.descriptorType = (VkDescriptorType)descriptorType;
		writeDescriptorSet.pBufferInfo = dp;
		writeDescriptorSet.dstBinding = binding;
		_cy_info.push_back(bset_info{set, binding, descriptorType, buffer, size, offset});
		_first_set[dstSet] = set;
		vkUpdateDescriptorSets(dev->_dev, 1, &writeDescriptorSet, 0, nullptr);
		return dstSet;
	}

	std::vector<VkDescriptorSet> dvk_set::get_ubos(const std::string& name, dvk_buffer* buffer, int count, uint64_t size, uint64_t first_offset)
	{
		std::vector<VkDescriptorSet> ret;
		auto ft = setLayoutsInfo->bufferParams.find(name);
		if (count < 1)return ret;
		if (ft == setLayoutsInfo->bufferParams.end())
		{
			auto set_ = ft->second.set;
			auto binding = ft->second.binding;
			auto dt = ft->second.descriptorType;

			auto& it = _dset[set_];
			auto ac = setLayoutsInfo->bindings.size();
			// set为一个时创建新set,或者多个set时每次创建新set
			if (ac == 1 && it.empty() || ac > 1)
			{
				std::vector<VkDescriptorSet>& vd = ret;
				_pipe->new_sets(vd, set_, count);
				if (vd.empty())
				{
					return ret;
				}
				for (auto vt : vd)
				{
					it.push_back(vt);
				}
			}
			std::vector<VkDescriptorBufferInfo> dp = {};
			dp.resize(count);
			for (size_t i = 0; i < count; i++)
			{
				dp[i].buffer = buffer->buffer;
				dp[i].offset = first_offset + i * size;
				dp[i].range = size;
			}
			std::vector<VkWriteDescriptorSet> upset;
			for (int i = 0; i < count; i++)
			{
				VkWriteDescriptorSet writeDescriptorSet = {};
				writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeDescriptorSet.dstSet = it[i];
				writeDescriptorSet.descriptorCount = 1;
				writeDescriptorSet.descriptorType = (VkDescriptorType)dt;
				writeDescriptorSet.pBufferInfo = &dp[i];
				writeDescriptorSet.dstBinding = binding;
				_cy_info.push_back(bset_info{(int)set_, (int)binding, dt, buffer, size, first_offset});
				_first_set[it[i]] = set_;
				upset.push_back(writeDescriptorSet);
			}
			vkUpdateDescriptorSets(_dev->_dev, upset.size(), upset.data(), 0, nullptr);
		}
		return ret;
	}

	void dvk_set::set_info(dvk_set* ds)
	{
		if (ds && ds->_cy_info.size())
		{
			auto& cyi = ds->_cy_info;
			for (auto& it : cyi)
			{
				if (it.buffer)
				{
					write_buffer(it.set, it.binding, it.descriptorType, it.buffer, it.size, it.offset, 0);
				}
				else if (it.texture)
				{
					write_image(it.set, it.binding, it.descriptorType, it.texture, 0);
				}
			}
		}
	}

	std::vector<VkDescriptorSet> dvk_set::set_one(dvk_texture* texture, dvk_buffer* buffer, uint64_t size, uint64_t offset)
	{
		std::vector<VkDescriptorSet> ret;
		if (setLayoutsInfo->imageParams.size() && texture)
		{
			auto bindInfo = setLayoutsInfo->imageParams.begin()->second;
			auto dset = write_image(bindInfo.set, bindInfo.binding, bindInfo.descriptorType, texture, 0);
			if (dset)
			{
				ret.push_back(dset);
			}
		}
		if (setLayoutsInfo->bufferParams.size() && buffer)
		{
			auto bindInfo = setLayoutsInfo->bufferParams.begin()->second;
			auto dset = write_buffer(bindInfo.set, bindInfo.binding, bindInfo.descriptorType, buffer, size, offset, 0);
			if (dset)
			{
				ret.push_back(dset);
			}
		}
		return ret;
	}

	uint32_t dvk_set::get_setid(VkDescriptorSet st)
	{
		auto it = _first_set.find(st);
		return it != _first_set.end() ? it->second : 0;
	}

	uint32_t dvk_set::get_set_maxs()
	{
		return setLayoutsInfo->bindings.size();
	}

	desc_type spv_res_info::get_dt(int set, int binding)
	{
		desc_type ret = {};
		do
		{
			auto bt = bindings.find(set);
			if (bt == bindings.end())break;
			auto it = bt->second.find(binding);
			if (it == bt->second.end())break;
			ret = it->second;
		} while (0);
		return ret;
	}

	size_t spv_res_info::get_ubosize(const std::string n)
	{
		assert(bufferParams.size());
		if (bufferParams.empty())
		{
			return 0;
		}
		auto it = bufferParams.find(n);
		return it != bufferParams.end() ? it->second.bufferSize : bufferParams.begin()->second.bufferSize;
	}

	std::string spv_res_info::get_uboname0()
	{
		return bufferParams.begin()->first;
	}

	std::vector<std::string> spv_res_info::get_uboname()
	{
		std::vector<std::string> ret; ret.reserve(bufferParams.size());
		for (auto& [k, v] : bufferParams)
		{
			ret.push_back(k);
		}
		return ret;
	}

	std::vector<std::string> spv_res_info::get_tex_name()
	{
		std::vector<std::string> ret; ret.reserve(imageParams.size());
		for (auto& [k, v] : imageParams)
		{
			ret.push_back(k);
		}
		return ret;
	}

	shader_info::~shader_info()
	{
		if (_dev && _dev->_dev)
		{
			int n = 0;
			if (_pipe[0])
			{
				n++;
				if (_pipe[1])
				{
					n++;
				}
			}
			for (int i = 0; i < n; i++)
				vkDestroyPipeline(_dev->_dev, _pipe[i], 0);
		}
	}

	int shader_info::isDynamic(int first, int n)
	{
		int ret = 0;
		for (size_t i = 0; i < n; i++)
		{
			auto it = _spinfo._binding.find(first + i);
			if (it != _spinfo._binding.end())
			{
				for (auto& [k, v] : it->second)
				{
					if (v.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC || v.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC)
					{
						ret++;
					}
				}
			}
		}
		return ret;
	}

	VkPipelineLayout shader_info::get_pipelineLayout()
	{
		return _spinfo.pipelineLayout;
	}

	VkPipeline shader_info::get_vkpipe(bool depthWriteEnable)
	{
		return !depthWriteEnable && _pipe[1] ? _pipe[1] : _pipe[0];
	}

	std::vector<VkDescriptorSet> shader_info::new_sets()
	{
		return _spinfo.new_sets();
	}

	size_t shader_info::new_sets(std::vector<VkDescriptorSet>& out)
	{
		return _spinfo.new_sets(out);
	}

	size_t shader_info::new_sets(std::vector<VkDescriptorSet>& out, int idx, int n)
	{
		return _spinfo.new_sets(out, idx, n);
	}

	dvk_set* shader_info::new_dvkset()
	{
		// todo new
		auto p = new dvk_set(_dev, &_srinfo, this);
		if (p)
		{
			//p->descriptorSets = new_sets();
		}
		return p;
	}

	//dvk_set* shader_info::new_dvkset(int n)
	//{
	//	dvk_set* p = nullptr;
	//	do {
	//		if (n > 0)
	//			p = new dvk_set[n];
	//		if (!p)break;
	//		for (size_t i = 0; i < n; i++)
	//		{
	//			p[i]._dev = _dev;
	//			p[i].setLayoutsInfo = &_srinfo;
	//			p[i].descriptorSets = new_sets();
	//			p[i].mkfs();
	//		}
	//	} while (0);
	//	return p;
	//}

	shader_hp::shader_hp()
	{
	}

	shader_hp::~shader_hp()
	{
		if (_ctx)
			clear();
	}

	void shader_hp::init(vk_render_cx* ctx, dvk_device* dev, const t_vector<t_string>& shader_path_fn)
	{
		_ctx = ctx;
		_dev = dev;
		for (auto& it : shader_path_fn)
		{
			auto ks = access(it.c_str(), 0);
			auto ns = File::is_path2file(it.c_str());
			if (ns == 1)
			{
				load_shader_zip(it.c_str(), sod, pci);
			}
			else if (ns == 2)
			{
				load_file(it.c_str(), sod, pci);
			}
		}
		ctx->load_pipe(dev, pci.data(), pci.size(), ps);
	}

	void shader_hp::clear()
	{
		assert(_ctx);
		for (auto& it : pci)
		{
			if (it.spv_info)
			{
				delete it.spv_info; it.spv_info = nullptr;
			}
		}
		_ctx->free_pipe_info(ps);
	}

	// todo layout(set=0, binding=0) uniform、uniform sampler2D、uniform sampler2DArray
	// idx空的话，则选择fns全部

	shader_info* shader_hp::init_pipe(shader_info* sp, const std::vector<std::string>& fns, const std::vector<int>& idx)
	{
		if (!sp || fns.empty())
		{
			return nullptr;
		}
		if (idx.size())
		{
			for (auto it : idx)
			{
				auto n = fns[it];
				sp->_spinfo.names.push_back(n);
			}
		}
		else {
			sp->_spinfo.names = fns;
		}
		auto ks = pci.size();
		for (size_t i = 0; i < ks; i++)
		{
			std::string pcin = pci[i].fn;
			for (auto& str : sp->_spinfo.names)
			{
				if (pcin.find(str) != std::string::npos)
				{
					sp->shader_idx.push_back(i);
					sp->_srinfo.union_binding(pci[i].spv_info);
				}
			}
		}
		auto dev = sp->_dev;
		if (!dev)
		{
			dev = _dev;
			sp->_dev = (dev);
		}
		sp->shader_data = ps;
		sp->_srinfo.make_input();
		sp->_spinfo.set_binding(sp->_srinfo.bindings);
		sp->_spinfo.make_set_pool(dev);
		return sp;
	}

	pipeline_ptr_info* shader_hp::new_pipe(pipeline_ptr_info* sp, const std::vector<std::string>& fns, const std::vector<int>& idx)
	{
		pipeline_ptr_info* ret = nullptr;
		do
		{
			auto tp = init_pipe(sp, fns, idx);
			if (!tp)break;
			if (_ctx->new_pipeline(sp->_dev, sp))ret = sp;
		} while (0);
		return ret;
	}

	ut_compute* shader_hp::new_compute(dvk_device* dev_, const std::vector<std::string>& fns, const std::vector<int>& idx)
	{
		auto dev = (dvk_device*)dev_;
		ut_compute* ret = nullptr;
		dvk_compute* dc = nullptr;
		VkPipelineCache pipelineCache = nullptr;
		auto shader = new shader_info();
		do
		{
			auto tp = init_pipe(shader, fns, idx);
			if (!tp) {
				break;
			}
			dc = dvk_compute::create(dev, pipelineCache, shader);
		} while (0);
		if (!dc) {
			delete shader;
		}
		else {
			ret = new ut_compute(dc);
			auto cpl = dev->new_cmd_pool(1);
			auto cp = new dvk_cmd_pool();
			cp->init(cpl, dev_);
			ret->set_cp(cp);
		}
		return ret;
	}
	int shader_hp::new_pipe(const njson& info, VkRenderPass render_pass)
	{
		int ret = 0;
		if (info.find("pipe_info") == info.end())
		{
			return ret;
		}
		auto item = info["pipe_info"];
		if (item.size() > 0)
		{
			for (auto& [k, v] : item.items())
			{
				pipeline_ptr_info* pt = dc.ac<pipeline_ptr_info>();
				pt->init(render_pass);
				int inps = v["input"].size();
				if (inps > 0)
				{
					auto& vi = v["input"];
					for (int k = 0; k < inps; k += 2)
					{
						pt->add_input_vbo(vi[k], vi[k + 1]);
					}
				}
				bool isdisable = toBool(v["disable"], false);
				if (isdisable)continue;
				// 填充方式VK_POLYGON_MODE_FILL = 0, VK_POLYGON_MODE_LINE = 1, VK_POLYGON_MODE_POINT
				pt->pso.polygon_mode = toInt(v["polygon_mode"], 0);
				// 剔除模式 VK_CULL_MODE_NONE = 0, VK_CULL_MODE_FRONT_BIT = 1, VK_CULL_MODE_BACK_BIT = 2, VK_CULL_MODE_FRONT_AND_BACK = 3,
				pt->pso.cull_mode = toInt(v["cull_mode"], 0);
				//表示顺时针方向为正面（VK_FRONT_FACE_CLOCKWISE=1）和逆时针方向为正面（VK_FRONT_FACE_COUNTER_CLOCKWISE=0）
				pt->pso.front_face = toInt(v["front_face"], 0);
				pt->pso._blend_type = toInt(v["blend_type"], 0);
				pt->pso.depthTestEnable = toBool(v["depthTestEnable"], true);
				pt->name = k;
				if (mpipe.find(k) != mpipe.end())
				{
					auto tp = mpipe[k];
					dc.pop(tp, 0);
				}
				auto p = new_pipe(pt, v["file"]);
				assert(p);
				if (p)
					mpipe[k] = p;
			}
		}
		return 0;
	}
	std::map<std::string, pipeline_ptr_info*>& shader_hp::mk_pipe(VkRenderPass render_pass)
	{
		// TODO: 在此处插入 return 语句
		new_pipe(_jnd[pipeidx], render_pass);
		return mpipe;
	}
	void shader_hp::load_shader_zip(const std::string& zfn, std::vector<pipe_od>& sod, std::vector<pipe_create_info>& out)
	{
		auto zd = Zip::create(zfn);
		dc.push(zd);
		size_t pos = sod.size();
		zd->enum_file("*.spv", [&sod](const std::string& fn, std::vector<char>* data)
		{
			sod.push_back({fn, std::move(*data)});
			return 0;
		});
		zd->enum_file("*.json", [&](const std::string& fn, std::vector<char>* data)
		{
			try
			{
				njson nd = njson::parse(data->begin(), data->end());
				if (nd.size())
				{
					if (fn == "pipe_info.json")
						pipeidx = _jnd.size();
					_jnd.push_back(std::move(nd));
				}
			}
			catch (const std::exception&)
			{

			}
			return 0;
		});
		if (sod.empty())
		{
			return;
		}
		pci.reserve(pci.size() + sod.size());
		auto& o = pci;
		for (size_t i = pos; i < sod.size(); i++)
		{
			pipe_create_info pc = {};
			pc.data = sod[i].data.data();
			pc.size = sod[i].data.size();
			pc.fn = zfn + "/" + sod[i].fn.c_str();
			pc.fn = hstring::replace(pc.fn.c_str(), "\\", "/");
			o.push_back(pc);
		}
	}
	void enum_read_file(const std::string rfn, std::vector<std::string>& fns, std::vector<shader_hp::pipe_od>& sod)
	{
		std::vector<char> data;
		std::string s = rfn;
		hstring::replace_ds(s, "*", R"([\w|\W]*)");
		std::regex rg1(s + "$");
		std::smatch r1;
		for (auto& k : fns)
		{
			if (std::regex_match(k, r1, rg1))
			{
				data.clear();
				File::read_binary_file(k.c_str(), data);
				if (data.size())
				{
					sod.push_back({k, std::move(data)});
				}
			}
		}
	}
	void shader_hp::load_file(const std::string& path, std::vector<pipe_od>& sod, std::vector<pipe_create_info>& out)
	{
		std::vector<std::string> fns;
		hz::File::IterFiles(path, &fns);
		size_t pos = sod.size();
		enum_read_file("*.spv", fns, sod);
		if (sod.empty())
		{
			return;
		}
		pci.reserve(pci.size() + sod.size());
		auto& o = pci;
		for (size_t i = pos; i < sod.size(); i++)
		{
			pipe_create_info pc = {};
			pc.data = sod[i].data.data();
			pc.size = sod[i].data.size();
			pc.fn = sod[i].fn.c_str();
			pc.fn = hstring::replace(pc.fn.c_str(), "\\", "/");
			o.push_back(pc);
		}
	}

	ut_compute::ut_compute(void* p) :_compute(p)
	{
	}

	ut_compute::~ut_compute()
	{
	}

	void ut_compute::set_cp(dvk_cmd_pool* p)
	{
		if (p)
			cmd_pool = p;
	}

	dvk_cmd* ut_compute::new_cmd()
	{
		return cmd_pool->new_cmd(false, true);
	}


	void ut_compute::bind(dvk_cmd* cmd, dvk_set* dsp)
	{
		auto p = (dvk_compute*)_compute; assert(p);
		p->bind(cmd, dsp);
	}

	dvk_set* ut_compute::new_set()
	{
		auto p = (dvk_compute*)_compute; assert(p);
		return p->new_set();
	}
	void ut_compute::set_push_constant(dvk_cmd* commandBuffer, void* data, uint32_t size, uint32_t offset)
	{
		auto p = (dvk_compute*)_compute; assert(p);
		p->push_constants(commandBuffer->ptr(), data, size, offset);
	}
#if 0
	void ut_compute::setUniform(const std::string& name, void* dataPtr, uint32_t size)
	{
		auto p = (dvk_compute*)_compute; assert(p);
		p->SetUniform(name, dataPtr, size);
	}

	void ut_compute::setTexture(const std::string& name, dvk_texture* texture)
	{
		auto p = (dvk_compute*)_compute; assert(p);
		p->SetTexture(name, texture);
	}

	void ut_compute::setStorageTexture(const std::string& name, dvk_texture* texture)
	{
		auto p = (dvk_compute*)_compute; assert(p);
		p->SetStorageTexture(name, texture);
	}

	void ut_compute::setStorageBuffer(const std::string& name, void* buffer)
	{
		auto p = (dvk_compute*)_compute; assert(p);
		p->SetStorageBuffer(name, (Buffer*)buffer);
	}
#endif



	dvk_fence::dvk_fence(VkDevice d) :dev(d)
	{
		init(d);
	}
	void dvk_fence::init(VkDevice d)
	{
		if (!dev)
			dev = d;
		if (!dev)
		{
			return;
		}
		VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT;
		VkFenceCreateInfo fence_create_info = {
			VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,              // VkStructureType                sType
			nullptr,                                          // const void                    *pNext
			flags                      // VkFenceCreateFlags             flags
		};
		auto hr = vkCreateFence(dev, &fence_create_info, nullptr, &fence);
		if (hr != 0)
		{
			assert(hr);
		}

	}

	dvk_fence::~dvk_fence()
	{
		if (fence && dev) {
			vkDestroyFence(dev, fence, nullptr);
		}
	}

	void dvk_fence::reset()
	{
		if (fence)
			vkResetFences(dev, 1, &fence);
	}
	void dvk_fence::wait_for()
	{
		if (fence)
			vkWaitForFences(dev, 1, &fence, true, MAXUINT64);
	}
	int dvk_fence::get_status()
	{
		return vkGetFenceStatus(dev, fence);
	}
#ifndef dvkbuffer

	dvk_buffer::dvk_buffer(dvk_device* dev, uint32_t usage, uint32_t mempro, uint32_t size, void* data) :_dev(dev), _size(size)
		, usageFlags(usage), memoryPropertyFlags(mempro)
	{
		assert(dev);
		if (dev)
		{
			device = *((VkDevice*)dev);
			auto d = (dvk_device*)dev;
			vkc::create_buffer(dev, usage, (VkMemoryPropertyFlags)mempro, size, &buffer, &memory, data, &_capacity);
		}
	}

	dvk_buffer::~dvk_buffer()
	{
		destroybuf();
	}
	void dvk_buffer::destroybuf()
	{
		if (buffer)
		{
			vkDestroyBuffer(device, buffer, nullptr);
			buffer = 0;
		}
		if (memory)
		{
			vkFreeMemory(device, memory, nullptr);
			memory = 0;
		}
		//buffer.clear();
		descriptors.clear();
	}

	void* dvk_buffer::get_dbi()
	{
		return descriptors.size() ? descriptors.data() : nullptr;
	}
	uint32_t dvk_buffer::get_dbi_count()
	{
		auto s = descriptors.size() / sizeof(VkDescriptorBufferInfo);
		return s;
	}
	void dvk_buffer::resize(size_t size)
	{
		if (size > _capacity)
		{
			destroybuf();
			vkc::create_buffer(_dev, usageFlags, (VkMemoryPropertyFlags)memoryPropertyFlags, size, &buffer, &memory, nullptr, &_capacity);
		}
		_size = size;
		mapped = 0;
	}
#if 0
	{
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT = 0x00000001,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT = 0x00000002,
			VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT = 0x00000004,
			VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT = 0x00000008,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT = 0x00000010,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT = 0x00000020,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT = 0x00000040,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT = 0x00000080,
			VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT = 0x00000100,
			VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT = 0x00000800,
			VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT = 0x00001000,
			VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT = 0x00000200,
			VK_BUFFER_USAGE_RAY_TRACING_BIT_NV = 0x00000400,
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_EXT = 0x00020000,
	}
#endif


	VkMemoryPropertyFlags get_mpflags(bool device_local, uint32_t* usage)
	{
		VkMemoryPropertyFlags mempro = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		if (device_local)
		{
			mempro = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			if (usage)
				*usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}
		return mempro;
	}
	dvk_buffer* dvk_buffer::create(dvk_device* dev, uint32_t usage, uint32_t mempro, uint32_t size, void* data)
	{
		return new dvk_buffer(dev, usage, mempro, size, data);
	}
	dvk_buffer* dvk_buffer::new_texel(dvk_device* dev, bool storage, uint32_t size, void* data)
	{
		uint32_t usage = (storage ? (uint32_t)VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT : (uint32_t)VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT);
		VkMemoryPropertyFlags mempro = get_mpflags(false, &usage);
		auto p = new dvk_buffer(dev, usage, mempro, size, data);

		return p;
	}
	dvk_buffer* dvk_buffer::new_staging(dvk_device* dev, uint32_t size, void* data)
	{
		uint32_t usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		VkMemoryPropertyFlags mempro = get_mpflags(false, &usage);
		auto p = new dvk_buffer(dev, usage, mempro, size, data);

		return p;
	}
	dvk_buffer* dvk_buffer::new_vbo(dvk_device* dev, bool device_local, bool compute_rw, uint32_t size, void* data)
	{
		uint32_t usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		VkMemoryPropertyFlags mempro = get_mpflags(device_local, &usage);
		if (compute_rw)
		{
			usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}
		auto p = new dvk_buffer(dev, usage, mempro, size, data);

		return p;
	}
	dvk_buffer* dvk_buffer::new_indirect(dvk_device* dev, bool device_local, uint32_t size, void* data)
	{
		uint32_t usage = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		VkMemoryPropertyFlags mempro = get_mpflags(device_local, &usage);
		auto p = new dvk_buffer(dev, usage, mempro, size, data);

		return p;
	}
	dvk_buffer* dvk_buffer::new_ibo(dvk_device* dev, int type, bool device_local, uint32_t count, void* data)
	{
		uint32_t usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		uint32_t indexBufferSize = count * (type ? sizeof(uint32_t) : sizeof(uint16_t));
		VkMemoryPropertyFlags mempro = get_mpflags(device_local, &usage);
		// todo new
		auto p = new dvk_buffer(dev, usage, mempro, indexBufferSize, data);

		return p;
	}
	dvk_buffer* dvk_buffer::new_ubo(dvk_device* dev, bool storage, uint32_t size)
	{
		uint32_t usage = (storage ? VK_BUFFER_USAGE_STORAGE_BUFFER_BIT : VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		VkMemoryPropertyFlags mempro = get_mpflags(false, &usage);
		auto p = new dvk_buffer(dev, usage, mempro, size, nullptr);

		return p;
	}

	void dvk_buffer::copy_buffer(dvk_buffer* dst, dvk_buffer* src, uint64_t dst_offset, uint64_t src_offset, int64_t size)
	{
		assert(dst->_size <= src->_size);
		auto _dev = (dvk_device*)dst->_dev;
		auto devs = (dvk_device*)src->_dev;
		if (_dev != devs)
		{
			assert(_dev == devs);
			return;
		}
		auto qctx = _dev->get_graphics_queue(0);
		auto cp = qctx->new_cmd_pool();
		auto copyQueue = qctx->get_vkptr();
		VkCommandBuffer copyCmd = vkc::createCommandBuffer1(_dev->_dev, cp->command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

		VkBufferCopy bufferCopy = {};

		bufferCopy.size = (size > 0) ? size : src->_size;
		if (dst->_size < bufferCopy.size)
		{
			bufferCopy.size = dst->_size;
			assert(dst->_size >= bufferCopy.size);
		}
		bufferCopy.dstOffset = dst_offset;
		bufferCopy.srcOffset = src_offset;

		vkCmdCopyBuffer(copyCmd, src->buffer, dst->buffer, 1, &bufferCopy);
		vkc::flushCommandBuffer(_dev->_dev, copyCmd, cp->command_pool, copyQueue, true);
		qctx->free_cmd_pool(cp);
	}
	void dvk_buffer::setDesType(uint32_t dt)
	{
		dtype = (VkDescriptorType)dt;
	}

	bool dvk_buffer::make_data(void* data, size_t size, size_t offset, bool isun)
	{
		map(size, offset);
		if (mapped)
		{
			memcpy(mapped, data, size);
			if (isun)unmap();
			return true;
		}
		return false;
	}
	size_t dvk_buffer::set_data(void* data, size_t size, size_t offset, bool is_flush)
	{
		if (!mapped)map(_size, 0);
		assert(mapped);
		memcpy((char*)mapped + offset, data, size);
		if (is_flush)
		{
			flush(size, offset);
		}
		if (fpos > offset)
		{
			fpos = offset;
		}
		if (fsize < size + offset)
		{
			fsize = size + offset;
		}
		return size + offset;
	}

	//uint32_t dvk_buffer::bind(VkDeviceSize offset)
	//{
	//	return vkBindBufferMemory(device, buffer, memory, offset);
	//}


	void* dvk_buffer::map(VkDeviceSize m_size, VkDeviceSize offset)
	{
		if (mapped)return mapped;
		if (m_size > _size || m_size == 0)
		{
			m_size = _size;
		}
		VkResult r = vkMapMemory(device, memory, offset, m_size, 0, &mapped);
		assert(r == VK_SUCCESS);
		return mapped;
	}
	void* dvk_buffer::get_map(VkDeviceSize offset)
	{
		char* p = (char*)mapped;
		assert(p);
		return p + offset;
	}
	void dvk_buffer::unmap()
	{
		if (mapped)
		{
			vkUnmapMemory(device, memory);
			mapped = nullptr;
		}
	}
	// todo new_descriptor有问题
	//void* dvk_buffer::new_descriptor(VkDeviceSize s, VkDeviceSize offset)
	//{
	//	auto pos = descriptors.size();
	//	descriptors.resize(pos + sizeof(VkDescriptorBufferInfo));
	//	auto p = (VkDescriptorBufferInfo*)&descriptors[pos];
	//	p->offset = offset;
	//	p->buffer = buffer;
	//	p->range = s;
	//	return p;
	//}


	uint32_t dvk_buffer::flush(VkDeviceSize size_, VkDeviceSize offset)
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = memory;
		mappedRange.offset = offset;
		mappedRange.size = size_ > 0 ? size_ : -1;
		return vkFlushMappedMemoryRanges(device, 1, &mappedRange);
	}
	uint32_t dvk_buffer::flush()
	{
		auto ret = flush(fsize, fpos);
		fpos = -1;
		fsize = 0;
		return ret;
	}
	uint32_t dvk_buffer::invalidate(VkDeviceSize size_, VkDeviceSize offset)
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = memory;
		mappedRange.offset = offset;
		mappedRange.size = size_;
		return vkInvalidateMappedMemoryRanges(device, 1, &mappedRange);
	}

#endif // !dvkbuffer


#if 1
	dvk_swapchain::dvk_swapchain(dvk_device* dev, VkSurfaceKHR surfacekhr) {
		initSurface(dev, surfacekhr);
	}

	dvk_swapchain::~dvk_swapchain() {}

	void dvk_swapchain::initSurface(dvk_device* devp, VkSurfaceKHR sf)
	{
		if (!devp || !sf || surface || _dev)
		{
			return;
		}
		VkResult err;
		// Get available queue family properties
		uint32_t queueCount = 0;
		surface = sf;
		_dev = devp;
		auto dev = (dvk_device*)_dev;
		assert(dev);


		vkGetPhysicalDeviceQueueFamilyProperties(dev->physicalDevice, &queueCount, NULL);
		assert(queueCount >= 1);

		std::vector<VkQueueFamilyProperties> queueProps(queueCount);
		vkGetPhysicalDeviceQueueFamilyProperties(dev->physicalDevice, &queueCount, queueProps.data());

		// Iterate over each queue to learn whether it supports presenting:
		// Find a queue with present support
		// Will be used to present the swap chain images to the windowing system
		std::vector<VkBool32> supportsPresent(queueCount);
		for (uint32_t i = 0; i < queueCount; i++)
		{
			vkGetPhysicalDeviceSurfaceSupportKHR(dev->physicalDevice, i, surface, &supportsPresent[i]);
		}

		// Search for a graphics and a present queue in the array of queue
		// families, try to find one that supports both
		uint32_t graphicsQueueNodeIndex = UINT32_MAX;
		uint32_t presentQueueNodeIndex = UINT32_MAX;
		for (uint32_t i = 0; i < queueCount; i++)
		{
			if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
			{
				if (graphicsQueueNodeIndex == UINT32_MAX)
				{
					graphicsQueueNodeIndex = i;
				}

				if (supportsPresent[i] == VK_TRUE)
				{
					graphicsQueueNodeIndex = i;
					presentQueueNodeIndex = i;
					break;
				}
			}
		}
		if (presentQueueNodeIndex == UINT32_MAX)
		{
			// If there's no queue that supports both present and graphics
			// try to find a separate present queue
			for (uint32_t i = 0; i < queueCount; ++i)
			{
				if (supportsPresent[i] == VK_TRUE)
				{
					presentQueueNodeIndex = i;
					break;
				}
			}
		}

		// Exit if either a graphics or a presenting queue hasn't been found
		if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX)
		{
			//hz::tools::exitFatal("Could not find a graphics and/or presenting queue!", "Fatal error");
		}

		// todo : Add support for separate graphics and presenting queue
		if (graphicsQueueNodeIndex != presentQueueNodeIndex)
		{
			//hz::tools::exitFatal("Separate graphics and presenting queues are not supported yet!", "Fatal error");
		}

		queueNodeIndex = graphicsQueueNodeIndex;

		// Get list of supported surface formats
		uint32_t formatCount;
		err = vkGetPhysicalDeviceSurfaceFormatsKHR(dev->physicalDevice, surface, &formatCount, NULL);
		assert(!err);
		assert(formatCount > 0);

		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		err = vkGetPhysicalDeviceSurfaceFormatsKHR(dev->physicalDevice, surface, &formatCount, surfaceFormats.data());
		assert(!err);

		// If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
		// there is no preferered format, so we assume VK_FORMAT_B8G8R8A8_UNORM
		if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
		{
			colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
			colorSpace = surfaceFormats[0].colorSpace;
		}
		else
		{
			// iterate over the list of available surface format and
			// check for the presence of VK_FORMAT_B8G8R8A8_UNORM
			bool found_B8G8R8A8_UNORM = false;
			for (auto&& surfaceFormat : surfaceFormats)
			{
				if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
				{
					colorFormat = surfaceFormat.format;
					colorSpace = surfaceFormat.colorSpace;
					found_B8G8R8A8_UNORM = true;
					break;
				}
			}

			// in case VK_FORMAT_B8G8R8A8_UNORM is not available
			// select the first available color format
			if (!found_B8G8R8A8_UNORM)
			{
				colorFormat = surfaceFormats[0].format;
				colorSpace = surfaceFormats[0].colorSpace;
			}
		}

	}


	/**
	* Create the swapchain and get it's images with given width and height
	*
	* @param width Pointer to the width of the swapchain (may be adjusted to fit the requirements of the swapchain)
	* @param height Pointer to the height of the swapchain (may be adjusted to fit the requirements of the swapchain)
	* @param vsync (Optional) Can be used to force vsync'd rendering (by using VK_PRESENT_MODE_FIFO_KHR as presentation mode)
	*/

	bool dvk_swapchain::create(uint32_t* width, uint32_t* height, bool vsync)
	{
		//print_time ftpt("dev_resize");
		if (!surface /*|| _width == *width && _height == *height*/)
			return false;
		if (*width != c_width || *height != c_height)
		{
			c_width = *width;
			c_height = *height;
		}
		else
		{
			//printf("大小一样:%d,%d\t%d,%d\n", (int)*width, (int)*height, (int)c_width, (int)c_height);
			//return true;
		}

		VkResult err;
		auto dev = (dvk_device*)_dev;
		VkSwapchainKHR oldSwapchain = swapChain;
		if (!dev->physicalDevice)return false;
		// Get physical device surface properties and formats
		VkSurfaceCapabilitiesKHR surfCaps;
		err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev->physicalDevice, surface, &surfCaps);
		assert(!err);

		// Get available present modes
		uint32_t presentModeCount;
		err = vkGetPhysicalDeviceSurfacePresentModesKHR(dev->physicalDevice, surface, &presentModeCount, NULL);
		assert(!err);
		assert(presentModeCount > 0);

		std::vector<VkPresentModeKHR> presentModes(presentModeCount);

		err = vkGetPhysicalDeviceSurfacePresentModesKHR(dev->physicalDevice, surface, &presentModeCount, presentModes.data());
		assert(!err);

		bool ret = false;
		VkExtent2D swapchainExtent = {};
		// If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be set by the swapchain
		if (surfCaps.currentExtent.width == (uint32_t)-1)
		{
			// If the surface size is undefined, the size is set to
			// the size of the images requested.
			swapchainExtent.width = *width;
			swapchainExtent.height = *height;
		}
		else
		{
			//	// If the surface size is defined, the swap chain size must match
			swapchainExtent = surfCaps.currentExtent;
			*width = surfCaps.currentExtent.width;
			*height = surfCaps.currentExtent.height;
			ret = true;
		}
		_width = *width;
		_height = *height;
		// Select a present mode for the swapchain
		{
			// The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
			// This mode waits for the vertical blank ("v-sync")
			VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

			// If v-sync is not requested, try to find a mailbox mode
			// It's the lowest latency non-tearing present mode available
			if (!vsync)
			{
				for (size_t i = 0; i < presentModeCount; i++)
				{
					if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
					{
						swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
						break;
					}
					if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
					{
						swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
					}
				}
			}

			// Determine the number of images
			uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
			if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
			{
				desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
			}

			// Find the transformation of the surface
			VkSurfaceTransformFlagsKHR preTransform;
			if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
			{
				// We prefer a non-rotated transform
				preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
			}
			else
			{
				preTransform = surfCaps.currentTransform;
			}

			// Find a supported composite alpha format (not all devices support alpha opaque)
			VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			// Simply select the first composite alpha format available
			std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
				VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
				VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
				VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
				VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
			};
			for (auto& compositeAlphaFlag : compositeAlphaFlags) {
				if (surfCaps.supportedCompositeAlpha & compositeAlphaFlag) {
					compositeAlpha = compositeAlphaFlag;
					break;
				};
			}

			VkSwapchainCreateInfoKHR swapchainCI = {};
			swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapchainCI.pNext = NULL;
			swapchainCI.surface = surface;
			swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
			swapchainCI.imageFormat = (VkFormat)colorFormat;
			swapchainCI.imageColorSpace = (VkColorSpaceKHR)colorSpace;
			swapchainCI.imageExtent = {swapchainExtent.width, swapchainExtent.height};
			swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			swapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
			swapchainCI.imageArrayLayers = 1;
			swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchainCI.queueFamilyIndexCount = 0;
			swapchainCI.pQueueFamilyIndices = NULL;
			swapchainCI.presentMode = swapchainPresentMode;
			swapchainCI.oldSwapchain = oldSwapchain;
			// Setting clipped to VK_TRUE allows the implementation to discard rendering outside of the surface area
			swapchainCI.clipped = VK_TRUE;
			swapchainCI.compositeAlpha = compositeAlpha;

			// Set additional usage flag for blitting from the swapchain images if supported
			VkFormatProperties formatProps;
			vkGetPhysicalDeviceFormatProperties(dev->physicalDevice, (VkFormat)colorFormat, &formatProps);
			if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT) {
				swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			}

			err = vkCreateSwapchainKHR(dev->_dev, &swapchainCI, nullptr, &swapChain);
			assert(!err);
			//return ret;
		}
		// If an existing swap chain is re-created, destroy the old swap chain
		// This also cleans up all the presentable images
		if (oldSwapchain)
		{
			for (uint32_t i = 0; i < imageCount; i++)
			{
				vkDestroyImageView(dev->_dev, buffers[i].view, nullptr);
			}
			vkDestroySwapchainKHR(dev->_dev, oldSwapchain, nullptr);
		}

		err = vkGetSwapchainImagesKHR(dev->_dev, swapChain, &imageCount, NULL);
		assert(!err);

		// Get the swap chain images
		images.resize(imageCount);
		err = vkGetSwapchainImagesKHR(dev->_dev, swapChain, &imageCount, images.data());
		assert(!err);

		// Get the swap chain buffers containing the image and imageview
		buffers.resize(imageCount);

		for (uint32_t i = 0; i < imageCount; i++)
		{
			VkImageViewCreateInfo colorAttachmentView = {};
			colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			colorAttachmentView.pNext = NULL;
			colorAttachmentView.format = (VkFormat)colorFormat;
			colorAttachmentView.components = {
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A
			};
			colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			colorAttachmentView.subresourceRange.baseMipLevel = 0;
			colorAttachmentView.subresourceRange.levelCount = 1;
			colorAttachmentView.subresourceRange.baseArrayLayer = 0;
			colorAttachmentView.subresourceRange.layerCount = 1;
			colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
			colorAttachmentView.flags = 0;

			buffers[i].image = images[i];
			buffers[i].colorFormat = colorFormat;
			colorAttachmentView.image = buffers[i].image;

			err = vkCreateImageView(dev->_dev, &colorAttachmentView, nullptr, &buffers[i].view);
			assert(!err);
		}
		return ret;
	}

	std::vector<dvk_swapchain::scb_t>* dvk_swapchain::getSwapChainVs()
	{
		return buffers.empty() ? nullptr : &buffers;
	}

	/**
	* Acquires the next image in the swap chain
	*
	* @param presentCompleteSemaphore (Optional) Semaphore that is signaled when the image is ready for use
	* @param imageIndex Pointer to the image index that will be increased if the next image could be acquired
	*
	* @note The function will always wait until the next image has been acquired by setting timeout to UINT64_MAX
	*
	* @return VkResult of the image acquisition
	*/

	uint32_t dvk_swapchain::acquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t* imageIndex)
	{
		VkDevice device = _dev->_dev;
		const uint32_t prev = _semaphoreIndex;
		_semaphoreIndex = (_semaphoreIndex + 1) % imageCount;
		uint32_t simageIndex = 0;
		if (!imageIndex)
		{
			imageIndex = &simageIndex;
		}
		VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, presentCompleteSemaphore, VK_NULL_HANDLE, imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_ERROR_SURFACE_LOST_KHR) {
			_semaphoreIndex = prev;
			return result;
		}
		_NumAcquireCalls += 1;
		//*outSemaphore = semaphore[_semaphoreIndex];
		_curr_imageIndex = *imageIndex;
		return _curr_imageIndex;
	}

	/**
	* Destroy and free Vulkan resources used for the swapchain
	*/

	void dvk_swapchain::cleanup()
	{
		auto dev = (dvk_device*)_dev;
		if (swapChain)
		{
			for (uint32_t i = 0; i < imageCount; i++)
			{
				vkDestroyImageView(dev->_dev, buffers[i].view, nullptr);
			}
		}
		if (surface)
		{
			vkDestroySwapchainKHR(dev->_dev, swapChain, nullptr);
			//vkDestroySurfaceKHR(dev->instance, surface, nullptr);
		}
		surface = VK_NULL_HANDLE;
		swapChain = VK_NULL_HANDLE;
	}

	/**
	* Create direct to display surface
	*/

#if defined(_DIRECT2DISPLAY)
	void dvk_swapchain::createDirect2DisplaySurface(uint32_t width, uint32_t height)
	{
		uint32_t displayPropertyCount;

		// Get display property
		vkGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, &displayPropertyCount, NULL);
		VkDisplayPropertiesKHR* pDisplayProperties = new VkDisplayPropertiesKHR[displayPropertyCount];
		vkGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, &displayPropertyCount, pDisplayProperties);

		// Get plane property
		uint32_t planePropertyCount;
		vkGetPhysicalDeviceDisplayPlanePropertiesKHR(physicalDevice, &planePropertyCount, NULL);
		VkDisplayPlanePropertiesKHR* pPlaneProperties = new VkDisplayPlanePropertiesKHR[planePropertyCount];
		vkGetPhysicalDeviceDisplayPlanePropertiesKHR(physicalDevice, &planePropertyCount, pPlaneProperties);

		VkDisplayKHR display = VK_NULL_HANDLE;
		VkDisplayModeKHR displayMode;
		VkDisplayModePropertiesKHR* pModeProperties;
		bool foundMode = false;

		for (uint32_t i = 0; i < displayPropertyCount; ++i)
		{
			display = pDisplayProperties[i].display;
			uint32_t modeCount;
			vkGetDisplayModePropertiesKHR(physicalDevice, display, &modeCount, NULL);
			pModeProperties = new VkDisplayModePropertiesKHR[modeCount];
			vkGetDisplayModePropertiesKHR(physicalDevice, display, &modeCount, pModeProperties);

			for (uint32_t j = 0; j < modeCount; ++j)
			{
				const VkDisplayModePropertiesKHR* mode = &pModeProperties[j];

				if (mode->parameters.visibleRegion.width == width && mode->parameters.visibleRegion.height == height)
				{
					displayMode = mode->displayMode;
					foundMode = true;
					break;
				}
			}
			if (foundMode)
			{
				break;
			}
			delete[] pModeProperties;
		}

		if (!foundMode)
		{
			hz::tools::exitFatal("Can't find a display and a display mode!", "Fatal error");
			return;
		}

		// Search for a best plane we can use
		uint32_t bestPlaneIndex = UINT32_MAX;
		VkDisplayKHR* pDisplays = NULL;
		for (uint32_t i = 0; i < planePropertyCount; i++)
		{
			uint32_t planeIndex = i;
			uint32_t displayCount;
			vkGetDisplayPlaneSupportedDisplaysKHR(physicalDevice, planeIndex, &displayCount, NULL);
			if (pDisplays)
			{
				delete[] pDisplays;
			}
			pDisplays = new VkDisplayKHR[displayCount];
			vkGetDisplayPlaneSupportedDisplaysKHR(physicalDevice, planeIndex, &displayCount, pDisplays);

			// Find a display that matches the current plane
			bestPlaneIndex = UINT32_MAX;
			for (uint32_t j = 0; j < displayCount; j++)
			{
				if (display == pDisplays[j])
				{
					bestPlaneIndex = i;
					break;
				}
			}
			if (bestPlaneIndex != UINT32_MAX)
			{
				break;
			}
		}

		if (bestPlaneIndex == UINT32_MAX)
		{
			hz::tools::exitFatal("Can't find a plane for displaying!", "Fatal error");
			return;
		}

		VkDisplayPlaneCapabilitiesKHR planeCap;
		vkGetDisplayPlaneCapabilitiesKHR(physicalDevice, displayMode, bestPlaneIndex, &planeCap);
		VkDisplayPlaneAlphaFlagBitsKHR alphaMode;

		if (planeCap.supportedAlpha & VK_DISPLAY_PLANE_ALPHA_PER_PIXEL_PREMULTIPLIED_BIT_KHR)
		{
			alphaMode = VK_DISPLAY_PLANE_ALPHA_PER_PIXEL_PREMULTIPLIED_BIT_KHR;
		}
		else if (planeCap.supportedAlpha & VK_DISPLAY_PLANE_ALPHA_PER_PIXEL_BIT_KHR)
		{

			alphaMode = VK_DISPLAY_PLANE_ALPHA_PER_PIXEL_BIT_KHR;
		}
		else
		{
			alphaMode = VK_DISPLAY_PLANE_ALPHA_GLOBAL_BIT_KHR;
		}

		VkDisplaySurfaceCreateInfoKHR surfaceInfo{};
		surfaceInfo.sType = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR;
		surfaceInfo.pNext = NULL;
		surfaceInfo.flags = 0;
		surfaceInfo.displayMode = displayMode;
		surfaceInfo.planeIndex = bestPlaneIndex;
		surfaceInfo.planeStackIndex = pPlaneProperties[bestPlaneIndex].currentStackIndex;
		surfaceInfo.transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		surfaceInfo.globalAlpha = 1.0;
		surfaceInfo.alphaMode = alphaMode;
		surfaceInfo.imageExtent.width = width;
		surfaceInfo.imageExtent.height = height;

		VkResult result = vkCreateDisplayPlaneSurfaceKHR(instance, &surfaceInfo, NULL, &surface);
		if (result != VK_SUCCESS)
		{
			hz::tools::exitFatal("Failed to create surface!", "Fatal error");
		}

		delete[] pDisplays;
		delete[] pModeProperties;
		delete[] pDisplayProperties;
		delete[] pPlaneProperties;
	}
#endif



#endif


	dvk_device::dvk_device()
	{
		_info = new dev_info();
	}

	dvk_device::~dvk_device()
	{
		for (auto it : graphics_queue_cxs)
		{
			if (it)
			{
				delete it;
			}
		}
		graphics_queue_cxs.clear();
		for (auto it : compute_queue_cxs)
		{
			if (it)
			{
				delete it;
			}
		}
		compute_queue_cxs.clear();
		if (_info)
		{
			delete _info; _info = 0;
		}
		if (ds_cb)
		{
			delete ds_cb; ds_cb = 0;
		}
		if (_limits)
		{
			delete _limits; _limits = 0;
		}
		if (_dev)
			vkDestroyDevice(_dev, 0);
	}

	void dvk_device::init(VkInstance inst, bool iscompute)
	{
		assert(physicalDevice);
		//if(physicalDevice)
		//this->physicalDevice = physicalDevice;
		_info->instance = inst;
		// Store Properties features, limits and properties of the physical device for later use
		// Device properties also contain limits and sparse properties
		vkGetPhysicalDeviceProperties(physicalDevice, &_info->properties);
		// Features should be checked by the examples before using them
		vkGetPhysicalDeviceFeatures(physicalDevice, &_info->features);
		// Memory properties are used regularly for creating all kinds of buffers
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &_info->memoryProperties);
		// Queue family properties, used for setting up requested queues upon device creation
		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		assert(queueFamilyCount > 0);
		//auto qfp = new std::vector<VkQueueFamilyProperties>();
		_info->queueFamilyProperties.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, _info->queueFamilyProperties.data());

		// Get list of supported extensions
		uint32_t extCount = 0;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, nullptr);
		bool isray = false;
		if (extCount > 0)
		{
			std::vector<VkExtensionProperties> extensions(extCount);
			if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
			{
				for (auto ext : extensions)
				{
					auto it = supportedExtensions.insert(ext.extensionName);
					//LOGI("extensionName: %s\n", ext.extensionName);
				}
			}
		}

		//LOGI("extensionName: %d\n", (int)supportedExtensions.size());
		//framebufferDepthSampleCounts = getMaxUsableSampleCount();
		VkQueueFlags qe = VK_QUEUE_GRAPHICS_BIT;
		if (iscompute)qe |= VK_QUEUE_COMPUTE_BIT;
		vkc::createLogicalDevice(this, _info->enabledFeatures, _info->enabledExtensions, true, qe);
		bool nv_mesh_shader = extensionSupported("VK_NV_mesh_shader");
		bool nv_ray_tracing = extensionSupported("VK_NV_ray_tracing");
		bool ray_tracing = extensionSupported("VK_KHR_ray_tracing_pipeline");
		bool is_dynamic_state = extensionSupported("VK_EXT_extended_dynamic_state");
		printf("ray_tracing: %s\n", ray_tracing || nv_ray_tracing ? "true" : "false");
		printf("is_dynamic_state: %s\n", is_dynamic_state ? "true" : "false");
		for (auto it : supportedExtensions)
		{
			if (it.find("h264") != std::string::npos)
			{
				printf("%s\n", it.c_str());
			}
		}
		//VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME;
		if (is_dynamic_state)
		{

			if (!ds_cb)
				ds_cb = new dynamic_state_ext_cb();
			auto cbk1 = (PFN_vkCmdSetCullModeEXT)vkGetInstanceProcAddr(_info->instance, "vkCmdSetCullModeEXT");
			//auto cbk1 = vkGetDeviceProcAddr(_dev, "vkCmdSetCullModeEXT");
			auto cbk2 = (PFN_vkQueueSubmit)vkGetDeviceProcAddr(_dev, "vkQueueSubmit");
			printf("vkCmdSetCullModeEXT: %p\n", cbk1);
			auto qs = vkQueueSubmit;
			printf("vkQueueSubmit: %p\told:%p\n", cbk2, qs);
#define DSEXT(n) ds_cb->##n=(PFN_##n) vkGetInstanceProcAddr(_info->instance,#n)
			DSEXT(vkCmdSetCullModeEXT);
			DSEXT(vkCmdSetFrontFaceEXT);
			DSEXT(vkCmdSetPrimitiveTopologyEXT);
			DSEXT(vkCmdSetViewportWithCountEXT);
			DSEXT(vkCmdSetScissorWithCountEXT);
			DSEXT(vkCmdBindVertexBuffers2EXT);
			DSEXT(vkCmdSetDepthTestEnableEXT);
			DSEXT(vkCmdSetDepthWriteEnableEXT);
			DSEXT(vkCmdSetDepthCompareOpEXT);
			DSEXT(vkCmdSetDepthBoundsTestEnableEXT);
			DSEXT(vkCmdSetStencilTestEnableEXT);
			DSEXT(vkCmdSetStencilOpEXT);
#undef DSEXT
		}
		mkDeviceQueue();
	}

	void dvk_device::mkDeviceQueue()
	{
		auto gp = vkc::get_queue_fp(this, VK_QUEUE_GRAPHICS_BIT);
		graphics_queues.resize(gp->queueCount);
		for (size_t i = 0; i < gp->queueCount; i++)
		{
			vkGetDeviceQueue(_dev, queueFamilyIndices.graphics, i, &graphics_queues[i]);
		}
		graphics_queue_cxs.resize(gp->queueCount);
		gp = vkc::get_queue_fp(this, VK_QUEUE_COMPUTE_BIT);
		compute_queues.resize(gp->queueCount);
		for (size_t i = 0; i < gp->queueCount; i++)
		{
			vkGetDeviceQueue(_dev, queueFamilyIndices.compute, i, &compute_queues[i]);
		}
		compute_queue_cxs.resize(gp->queueCount);
		return;
	}

	// new


	void dvk_device::free_shader(VkShaderModule s)
	{
		vkDestroyShaderModule(_dev, s, nullptr);
	}

	bool dvk_device::new_pipeline(pipeline_ptr_info* info)
	{
		pipeline_ptr_info* p = (pipeline_ptr_info*)info;
		if (!p || !p->render_pass || p->_srinfo.input_info.empty())
			return false;
		std::vector<VkVertexInputBindingDescription> bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> vertex_attribute_descriptions;
		uint32_t n = 0;
		//顶点属性
		uint32_t in_offset = 0, stride = 0;
		int in_binding = 0;
		auto vii = p->vinput_info;
		auto pi = vii.begin();
		std::vector<inbind_desc> input_binding;
		for (auto& [k, it] : p->_srinfo.input_info)
		{
			vertex_attribute_descriptions.push_back({(uint32_t)it.location, (uint32_t)in_binding, (VkFormat)it.format, (uint32_t)in_offset});
			in_offset += it.stride_size;
			if (pi != vii.end())
			{
				pi->x--;
				if (pi->x == 0)
				{
					input_binding.push_back({0, (uint16_t)in_binding, (uint16_t)in_offset, (uint8_t)pi->y});
					in_binding++;
					in_offset = 0;
					pi++;
				}
			}
		}
		stride = in_offset;
		// VBO绑定
		if (input_binding.size())
		{
			bindingDescriptions.resize(input_binding.size());
			for (size_t i = 0; i < input_binding.size(); i++)
			{
				auto& it = input_binding[i];
				bindingDescriptions[i].binding = it.binding;
				bindingDescriptions[i].stride = it.stride;
				bindingDescriptions[i].inputRate = (VkVertexInputRate)it.inputRate; //VK_VERTEX_INPUT_RATE_VERTEX,VK_VERTEX_INPUT_RATE_INSTANCE
			}
		}
		else
		{
			for (; stride % p->align; stride++);
			bindingDescriptions.push_back({(uint32_t)0, (uint32_t)stride, VK_VERTEX_INPUT_RATE_VERTEX});
		}


		VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,    // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineVertexInputStateCreateFlags          flags;
			(uint32_t)bindingDescriptions.size(),                                   // uint32_t  vertexBindingDescriptionCount
			bindingDescriptions.data(),                                  // const VkVertexInputBindingDescription         *pVertexBindingDescriptions
			(uint32_t)vertex_attribute_descriptions.size(),               // uint32_t                                       vertexAttributeDescriptionCount
			vertex_attribute_descriptions.data()                          // const VkVertexInputAttributeDescription       *pVertexAttributeDescriptions
		};
		//vertex_input_state_create_info.vertexBindingDescriptionCount = bindingDescriptions.size();
		VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,  // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineInputAssemblyStateCreateFlags        flags
			(VkPrimitiveTopology)p->topology,							  // VkPrimitiveTopology                            topology
			VK_FALSE                                                      // VkBool32                                       primitiveRestartEnable
		};

		VkPipelineViewportStateCreateInfo viewport_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,        // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineViewportStateCreateFlags             flags
			p->viewportCount,                                             // uint32_t                                       viewportCount
			nullptr,                                                      // const VkViewport                              *pViewports
			p->scissorCount,                                              // uint32_t                                       scissorCount
			nullptr                                                       // const VkRect2D                                *pScissors
		};
		//polygon_mode\cull_mode\front_face\line_width
		VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,   // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineRasterizationStateCreateFlags        flags
			VK_TRUE,                                                      // VkBool32                                       depthClampEnable
			VK_FALSE,                                                     // VkBool32                                       rasterizerDiscardEnable
			(VkPolygonMode)p->pso.polygon_mode,// VK_POLYGON_MODE_FILL,                                         // VkPolygonMode                                  polygonMode
			(VkCullModeFlags)p->pso.cull_mode, //VK_CULL_MODE_BACK_BIT,										  // VkCullModeFlags                                cullMode背面剔除
			(VkFrontFace)p->pso.front_face, //VK_FRONT_FACE_COUNTER_CLOCKWISE,                              // VkFrontFace                                    frontFace逆时针旋转
			VK_FALSE,														// VkBool32                                       depthBiasEnable
			0.0f,															// float                                          depthBiasConstantFactor
			0.0f,															// float                                          depthBiasClamp
			0.0f,															// float                                          depthBiasSlopeFactor
			1.0f															// float                                          lineWidth
		};

		VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,     // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineMultisampleStateCreateFlags          flags
			VK_SAMPLE_COUNT_1_BIT,                                        // VkSampleCountFlagBits                          rasterizationSamples
			VK_FALSE,                                                     // VkBool32                                       sampleShadingEnable
			1.0f,                                                         // float                                          minSampleShading
			nullptr,                                                      // const VkSampleMask                            *pSampleMask
			VK_FALSE,                                                     // VkBool32                                       alphaToCoverageEnable
			VK_FALSE                                                      // VkBool32                                       alphaToOneEnable
		};

		VkPipelineColorBlendAttachmentState* cbas = 0;

		VkPipelineColorBlendAttachmentState userblend = {
			p->pso.blend.blendEnable,
			(VkBlendFactor)p->pso.blend.srcColorBlendFactor,
			(VkBlendFactor)p->pso.blend.dstColorBlendFactor,
			(VkBlendOp)p->pso.blend.colorBlendOp,
			(VkBlendFactor)p->pso.blend.srcAlphaBlendFactor,
			(VkBlendFactor)p->pso.blend.dstAlphaBlendFactor,
			(VkBlendOp)p->pso.blend.alphaBlendOp,
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |         // VkColorComponentFlags                          colorWriteMask
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};
		if (p->pso._blend_type < 2)
		{
			vkc::get_blend(p->pso._blend_type ? false : true, userblend);
		}
		cbas = &userblend;

		VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,     // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineColorBlendStateCreateFlags           flags
			VK_FALSE,                                                     // VkBool32                                       logicOpEnable
			VK_LOGIC_OP_COPY,                                             // VkLogicOp                                      logicOp
			1,                                                            // uint32_t                                       attachmentCount
			cbas,                                // const VkPipelineColorBlendAttachmentState     *pAttachments
												 // float                                          blendConstants[4]
		};
		memcpy(color_blend_state_create_info.blendConstants, p->blendConstants, sizeof(float) * 4);

		{
			p->dynamic_states.insert(VK_DYNAMIC_STATE_VIEWPORT);
			p->dynamic_states.insert(VK_DYNAMIC_STATE_SCISSOR);
			p->dynamic_states.insert(VK_DYNAMIC_STATE_BLEND_CONSTANTS);
		}
		std::vector<VkDynamicState> dynamic_states;
		for (auto it : p->dynamic_states) {
			dynamic_states.push_back((VkDynamicState)it);
		}

		VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,         // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineDynamicStateCreateFlags              flags
			(uint32_t)dynamic_states.size(),                                                            // uint32_t                                       dynamicStateCount
			(VkDynamicState*)dynamic_states.data()                                                // const VkDynamicState                          *pDynamicStates
		};
		VkStencilOpState back;
		back.failOp = back.passOp = back.depthFailOp = VK_STENCIL_OP_KEEP;
		back.compareOp = VK_COMPARE_OP_ALWAYS;
		back.compareMask = 0;
		back.writeMask = 0;
		back.reference = 0;
		//VkStencilOpState back0;
		//back0.failOp = back0.passOp = back0.depthFailOp = VK_STENCIL_OP_REPLACE;
		//back0.compareOp = VK_COMPARE_OP_ALWAYS;
		//back0.compareMask = 0xff;
		//back0.writeMask = 0xff;
		//back0.reference = 1;
		//VkStencilOpState back1;
		//back1.failOp = back1.depthFailOp = VK_STENCIL_OP_KEEP;
		//back1.passOp = VK_STENCIL_OP_REPLACE;
		//back1.compareOp = VK_COMPARE_OP_NOT_EQUAL;
		//back1.compareMask = 0xff;
		//back1.writeMask = 0xff;
		//back1.reference = 1;
		VkPipelineDepthStencilStateCreateInfo depth_stencil_state[2] =
		{
			{
				VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,	//	VkStructureType                           sType;
				nullptr,													//	const void*                               pNext;
				0,															//	VkPipelineDepthStencilStateCreateFlags    flags;
				(VkBool32)(p->pso.depthTestEnable ? VK_TRUE : VK_FALSE),	//	VkBool32                                  depthTestEnable;
				VK_TRUE,													//	VkBool32                                  depthWriteEnable;
				VK_COMPARE_OP_LESS_OR_EQUAL,								//	VkCompareOp                               depthCompareOp;
				VK_FALSE,													//	VkBool32                                  depthBoundsTestEnable;
				VK_FALSE,													//	VkBool32                                  stencilTestEnable;
				back,															//	VkStencilOpState                          front;
				back,															//	VkStencilOpState                          back;
				0.0f,														//	float                                     minDepthBounds;
				1.0f														//	float                                     maxDepthBounds;
			}
			, {
				VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,	//	VkStructureType                           sType;
				nullptr,													//	const void*                               pNext;
				0,															//	VkPipelineDepthStencilStateCreateFlags    flags;
				(VkBool32)(p->pso.depthTestEnable ? VK_TRUE : VK_FALSE),	//	VkBool32                                  depthTestEnable;
				VK_FALSE,													//	VkBool32                                  depthWriteEnable;
				VK_COMPARE_OP_LESS_OR_EQUAL,								//	VkCompareOp                               depthCompareOp;
				VK_FALSE,													//	VkBool32                                  depthBoundsTestEnable;
				VK_FALSE,													//	VkBool32                                  stencilTestEnable;
				back,														//	VkStencilOpState                          front;
				back,														//	VkStencilOpState                          back;
				0.0f,														//	float                                     minDepthBounds;
				1.0f														//	float                                     maxDepthBounds;
			}
		};

		std::vector<VkPipelineShaderStageCreateInfo> shader_infos;
		auto& sv = *((std::vector<VkPipelineShaderStageCreateInfo>*)p->shader_data->v);
		assert(sv.size() > 0);
		for (auto it : p->shader_idx) {
			if (it >= 0 && it < sv.size())
				shader_infos.push_back(sv[it]);
		}
		if (shader_infos.empty())
		{
			return false;
		}
		auto pipeline_layout = (VkPipelineLayout)p->_spinfo.pipelineLayout;
		VkGraphicsPipelineCreateInfo pipeline_create_info[2] = {{
				VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,              // VkStructureType                                sType
				nullptr,                                                      // const void                                    *pNext
				0,                                                            // VkPipelineCreateFlags                          flags
				static_cast<uint32_t>(shader_infos.size()),					  // uint32_t                                       stageCount
				shader_infos.data(),										  // const VkPipelineShaderStageCreateInfo         *pStages
				&vertex_input_state_create_info,                              // const VkPipelineVertexInputStateCreateInfo    *pVertexInputState;
				&input_assembly_state_create_info,                            // const VkPipelineInputAssemblyStateCreateInfo  *pInputAssemblyState
				nullptr,                                                      // const VkPipelineTessellationStateCreateInfo   *pTessellationState
				&viewport_state_create_info,                                  // const VkPipelineViewportStateCreateInfo       *pViewportState
				&rasterization_state_create_info,                             // const VkPipelineRasterizationStateCreateInfo  *pRasterizationState
				&multisample_state_create_info,                               // const VkPipelineMultisampleStateCreateInfo    *pMultisampleState
				depth_stencil_state,                                          // const VkPipelineDepthStencilStateCreateInfo   *pDepthStencilState
				&color_blend_state_create_info,                               // const VkPipelineColorBlendStateCreateInfo     *pColorBlendState
				&dynamic_state_create_info,                                   // const VkPipelineDynamicStateCreateInfo        *pDynamicState
				pipeline_layout,											  // VkPipelineLayout                               layout
				(VkRenderPass)p->render_pass,                                 // VkRenderPass                                   renderPass
				0,                                                            // uint32_t                                       subpass
				VK_NULL_HANDLE,                                               // VkPipeline                                     basePipelineHandle
				-1                                                            // int32_t                                        basePipelineIndex
			}};
		uint32_t count = 1;
		if (p->pso.depthTestEnable)
		{
			memcpy(&pipeline_create_info[1], &pipeline_create_info[0], sizeof(VkGraphicsPipelineCreateInfo));
			pipeline_create_info[1].pDepthStencilState = &depth_stencil_state[1];
			count++;
		}
		if (vkCreateGraphicsPipelines(_dev, p->pipelineCache, count, pipeline_create_info, nullptr, (VkPipeline*)p->_pipe) != VK_SUCCESS) {
			std::cout << "Could not create graphics pipeline!" << std::endl;
			return false;
		}
		return true;

	}

	VkCommandPool dvk_device::new_cmd_pool(uint32_t idx)
	{
		return vkc::createCommandPool(_dev, idx, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	}

	VkCommandPool dvk_device::new_graphics_cmd_pool()
	{
		return vkc::createCommandPool(_dev, queueFamilyIndices.graphics, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	}

	VkCommandPool dvk_device::new_compute_cmd_pool()
	{
		return vkc::createCommandPool(_dev, queueFamilyIndices.compute, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	}


	bool dvk_device::new_set_pool(set_pool_info* sp)
	{
		std::vector<std::vector<VkDescriptorSetLayoutBinding>> set_layout_binding;
		if (sp->_binding.size())
		{
			set_layout_binding.resize(sp->_binding.size());
		}
		std::map<uint32_t, uint32_t>& ps = sp->_ps;
		auto& spb = sp->_binding;
		int ki = 0;
		for (auto& [k, v] : spb)
		{
			//if (k < set_layout_binding.size())
			{
				auto& slb = set_layout_binding[ki++];
				for (auto& [k1, v1] : v)
				{
					desc_type dt;
					dt = v1;
					slb.push_back({(uint32_t)k1, (VkDescriptorType)dt.descriptorType, (uint32_t)dt.descriptorCount, (VkShaderStageFlags)dt.stageFlags, nullptr});
					ps[dt.descriptorType] += dt.descriptorCount;
				}
			}
		}
		std::vector<VkDescriptorSetLayout> setLayouts;
		for (auto& it : set_layout_binding)
		{
			VkDescriptorSetLayout temp = 0;
			VkDescriptorSetLayoutCreateInfo ninfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, nullptr, 0, (uint32_t)it.size(), it.data()};
			vkCreateDescriptorSetLayout(_dev, &ninfo, nullptr, &temp);
			setLayouts.push_back(temp);
			sp->set_ptr.push_back(temp);
		}
		//VkPushConstantRange pcs[2] = { {VK_SHADER_STAGE_VERTEX_BIT,0,64} , {VK_SHADER_STAGE_FRAGMENT_BIT,64,64} };
		VkPushConstantRange pconstant[2] = {{VK_SHADER_STAGE_VERTEX_BIT, 0, sp->pct_vert}, {VK_SHADER_STAGE_FRAGMENT_BIT, sp->pct_vert, sp->pct_frag}};
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, nullptr, 0,
			(uint32_t)setLayouts.size(), setLayouts.data(),
			2, pconstant
		};
		// Now create the pipeline layout.
		vkCreatePipelineLayout(_dev, &pipelineLayoutCreateInfo, nullptr, (VkPipelineLayout*)&sp->pipelineLayout);
		return true;
	}

	VkDescriptorPool dvk_device::new_desc_pool(std::map<uint32_t, uint32_t>& mps, uint32_t maxSets)
	{
		std::vector<VkDescriptorPoolSize> pool_sizes;
		for (auto& [k, v] : mps)
		{
			pool_sizes.push_back({(VkDescriptorType)k, v});
		}
		// todo 创建set分配池子vkCreateDescriptorPool
		if (maxSets < 8)
			maxSets = 8;
		VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,  // VkStructureType                sType
			nullptr,                                        // const void                    *pNext
			0,                                              // VkDescriptorPoolCreateFlags    flags
			maxSets,                                              //
			static_cast<uint32_t>(pool_sizes.size()),       // uint32_t                       poolSizeCount
			pool_sizes.data()                                  // const VkDescriptorPoolSize    *pPoolSizes
		};
		VkDescriptorPool ret = {};
		if (vkCreateDescriptorPool(_dev, &descriptor_pool_create_info, nullptr, &ret) != VK_SUCCESS) {
			std::cout << "Could not create descriptor pool!" << std::endl;
			return 0;
		}

		return ret;
	}

	bool dvk_device::alloc_set(VkDescriptorPool pool, VkDescriptorSetLayout* pSetLayouts, uint32_t descriptorSetCount, std::vector<VkDescriptorSet>& dset)
	{
		if (!pool || !pSetLayouts)
		{
			return false;
		}
		dset.resize(descriptorSetCount);
		VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, // VkStructureType                sType
			nullptr,                                        // const void                    *pNext
			pool,											// VkDescriptorPool               descriptorPool
			descriptorSetCount,                             // uint32_t                       descriptorSetCount
			pSetLayouts										// const VkDescriptorSetLayout   *pSetLayouts
		};
		auto hr = vkAllocateDescriptorSets(_dev, &descriptor_set_allocate_info, dset.data());
		if (hr != VK_SUCCESS || dset[0] == 0)
		{
			std::cout << "Could not allocate descriptor set!" << std::endl;
			return false;
		}

		return true;
	}

	int dvk_device::new_cmd(VkCommandPool pool, uint32_t level, VkCommandBuffer* outptr, uint32_t count)
	{
		if (count > 0 && outptr)
		{
			VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
			cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			cmdBufAllocateInfo.commandPool = pool;
			cmdBufAllocateInfo.level = (VkCommandBufferLevel)level;
			cmdBufAllocateInfo.commandBufferCount = count;
			VK_CHECK_RESULT(vkAllocateCommandBuffers(_dev, &cmdBufAllocateInfo, outptr));
		}
		return count;
	}

	void dvk_device::free_cmd(VkCommandPool pool, VkCommandBuffer* pcbs, uint32_t count)
	{
		if (pool && pcbs && count > 0)
		{
			vkFreeCommandBuffers(_dev, pool, count, pcbs);
		}
	}



	// 创建RenderPass

	VkRenderPass dvk_device::new_render_pass(uint32_t color_format1, uint32_t depth_format1)
	{
		VkFormat color_format = (VkFormat)color_format1, depth_format = (VkFormat)depth_format1;
		std::array<VkAttachmentDescription, 2> attchmentDescriptions = {};
		// Color attachment
		attchmentDescriptions[0].format = color_format;
		attchmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attchmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attchmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attchmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attchmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attchmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attchmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		// Depth attachment
		attchmentDescriptions[1].format = depth_format;
		attchmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
		attchmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attchmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attchmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attchmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attchmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attchmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		VkAttachmentReference colorReference = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
		VkAttachmentReference depthReference = {1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorReference;
		subpassDescription.pDepthStencilAttachment = &depthReference;
		// Use subpass dependencies for layout transitions
		std::array<VkSubpassDependency, 2> dependencies;
		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		// Create the actual renderpass
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attchmentDescriptions.size());
		renderPassInfo.pAttachments = attchmentDescriptions.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDescription;
		renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassInfo.pDependencies = dependencies.data();
		VkRenderPass pass = 0;
		VK_CHECK_RESULT(vkCreateRenderPass(_dev, &renderPassInfo, nullptr, &pass));
		return pass;
	}

	std::string dvk_device::get_name()
	{
		std::string ret = _info->properties.deviceName;
		return ret;
	}

	uint32_t dvk_device::get_familyIndex(int idx)
	{
		uint32_t family = ((uint32_t*)&queueFamilyIndices)[idx];
		return family;
	}

	dvk_queue* dvk_device::get_graphics_queue(unsigned int idx)
	{
		dvk_queue* ret = nullptr;
		if (idx < graphics_queues.size())
		{
			auto& p = graphics_queue_cxs[idx];
			if (!p)
			{
				p = new dvk_queue(this, graphics_queues[idx], queueFamilyIndices.graphics, dvk_queue_bit::GRAPHICS_BIT);
			}
			ret = p;
		}
		return ret;
	}

	dvk_queue* dvk_device::get_compute_queue(unsigned int idx)
	{
		dvk_queue* ret = nullptr;
		if (queueFamilyIndices.compute == queueFamilyIndices.graphics)
		{
			return get_graphics_queue(idx);
		}
		if (idx < compute_queues.size())
		{
			auto& p = compute_queue_cxs[idx];
			if (!p)
			{
				p = new dvk_queue(this, compute_queues[idx], queueFamilyIndices.compute, dvk_queue_bit::GRAPHICS_BIT);
			}
			ret = p;
		}
		return ret;
	}

	void dvk_device::free_gqp(unsigned int idx)
	{
		if ((idx < compute_queue_cxs.size()))
		{
			auto& cq = compute_queue_cxs[idx];
			delete cq; cq = 0;
		}
	}

	void dvk_device::free_cqp(unsigned int idx)
	{
		if (queueFamilyIndices.compute == queueFamilyIndices.graphics || !(idx < graphics_queue_cxs.size()))
		{
			return;
		}
		auto& cq = graphics_queue_cxs[idx];
		delete cq; cq = 0;
	}

	uint32_t dvk_device::get_gqueue_size()
	{
		return graphics_queues.size();
	}

	uint32_t dvk_device::get_cqueue_size()
	{
		return compute_queues.size();
	}

	void dvk_device::wait_idle()
	{
		vkDeviceWaitIdle(_dev);
	}

	VkShaderModule dvk_device::CreateShaderModule(const char* filename, size_t len)
	{
		std::vector<char> code;
		if (len > 0)
		{
			code.resize(len);
			memcpy(&code[0], filename, len);
		}
		else
		{
			//File::read_binary_file(filename, code);
		}
		if (code.size() == 0) {
			return VK_NULL_HANDLE;
		}

		VkShaderModuleCreateInfo shader_module_create_info = {
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,    // VkStructureType                sType
			nullptr,                                        // const void                    *pNext
			0,                                              // VkShaderModuleCreateFlags      flags
			code.size(),                                    // size_t                         codeSize
			reinterpret_cast<const uint32_t*>(&code[0])     // const uint32_t                *pCode
		};

		VkShaderModule shader_module;
		if (vkCreateShaderModule(_dev, &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS) {
			std::cout << "Could not create shader module from a \"" << filename << "\" file!" << std::endl;
			return VK_NULL_HANDLE;
		}
		return shader_module;
	}

	bool dvk_device::extensionSupported(const std::string& extension)
	{
		return supportedExtensions.find(extension) != supportedExtensions.end();
#if 0
		//ndef std::find
		for (auto it : supportedExtensions)
		{
			if (it != extension) {
				return false;
			}
		}
		return true;
		//#else
		return (std::find(supportedExtensions.begin(), supportedExtensions.end(), extension) != supportedExtensions.end());
#endif
	}

	dvk_staging_buffer::dvk_staging_buffer()
	{
	}

	dvk_staging_buffer::~dvk_staging_buffer()
	{
		freeBuffer();
	}

	void dvk_staging_buffer::freeBuffer()
	{
		if (isd)
		{
			// Clean up staging resources
			if (mem)
				vkFreeMemory(_dev, mem, nullptr);
			if (buffer)
				vkDestroyBuffer(_dev, buffer, nullptr);
		}
		mem = 0;
		buffer = 0;
	}

	void dvk_staging_buffer::initBuffer(dvk_device* dev, VkDeviceSize size)
	{
		_dev = dev->_dev;
		if (bufferSize != size)
		{
			bufferSize = size;
		}
		if (size > memSize)
		{
			freeBuffer();
		}
		// Create a host-visible staging buffer that contains the raw image data
		if (!buffer)
		{
			VkMemoryAllocateInfo memAllocInfo = {};
			memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			VkMemoryRequirements memReqs;

			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.size = bufferSize;
			// This buffer is used as a transfer source for the buffer copy
			bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VK_CHECK_RESULT(vkCreateBuffer(_dev, &bufferCreateInfo, nullptr, &buffer));

			// Get memory requirements for the staging buffer (alignment, memory type bits)
			vkGetBufferMemoryRequirements(_dev, buffer, &memReqs);
			memSize = memReqs.size;
			memAllocInfo.allocationSize = memReqs.size;
			// Get memory type index for a host visible buffer
			VkBool32 memTypeFound = 0;
			memAllocInfo.memoryTypeIndex = vkc::getMemoryType(dev, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &memTypeFound);

			VK_CHECK_RESULT(vkAllocateMemory(_dev, &memAllocInfo, nullptr, &mem));
			VK_CHECK_RESULT(vkBindBufferMemory(_dev, buffer, mem, 0));

		}
	}

	void dvk_staging_buffer::copyToBuffer(void* data, size_t bsize)
	{
		// Copy texture data into staging buffer
		if (data)
		{
			//LOGW("Texture:format:%d,%d\n", (int)buffer, (int)memSize);
			VK_CHECK_RESULT(vkMapMemory(_dev, mem, 0, memSize, 0, (void**)&mapped));
			memcpy(mapped, data, bsize);
			vkUnmapMemory(_dev, mem);
		}
	}

	void dvk_staging_buffer::copyGetBuffer(std::vector<char>& outbuf)
	{
		outbuf.resize(bufferSize);
		// Copy texture data into staging buffer
		uint8_t* data;
		VK_CHECK_RESULT(vkMapMemory(_dev, mem, 0, memSize, 0, (void**)&data));
		memcpy(outbuf.data(), data, bufferSize);
		vkUnmapMemory(_dev, mem);
	}

	size_t dvk_staging_buffer::getBufSize()
	{
		return memSize;
	}

	void dvk_staging_buffer::getBuffer(char* outbuf, size_t len)
	{
		len = std::min((size_t)bufferSize, len);
		// Copy texture data into staging buffer
		uint8_t* data;
		VK_CHECK_RESULT(vkMapMemory(_dev, mem, 0, memSize, 0, (void**)&data));
		memcpy(outbuf, data, len);
		vkUnmapMemory(_dev, mem);
	}


}
//!hz
#endif
