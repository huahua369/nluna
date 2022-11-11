#include <stdlib.h>
#include <stdio.h>
//#include <tools.h>
//#include <libuv/uv.h>
//#include <libuv/uvw/util.hpp>
#include <base/ecc_sv.h>
#include <base/sem.h>
#include <sem_async_uv.h>
#include <palloc/palloc.h>
#include <base/print_time.h>


#include <ntype.h>
#include <iconv.h>
#include <base/smq.h>
#include <base/hlUtil.h>
//#include "sqlt.h"
#include <base/promise_cx.h>
//#include <http/http.h>
//#include <net/curl_cx.h>
#include <view/utils_bin.h>
#include <view/image.h>
//#include <view/Canvas.h>
#include <view/file.h>
#include <libzip/libzip.h>

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
#ifdef VK_NO_PROTOTYPES

#endif // VK_NO_PROTOTYPES

#include <vulkan/vulkan.h>

#include <SPIRV-Cross/spirv_cross.hpp>

#ifdef _WIN32
#define LOGIf printf
#else
#define LOGIf 
#endif
#define LOGI
#define LOGW LOGI
#define LOGD LOGI
#define LOGE LOGI
#define LOGv LOGI

#include <data/json_helper.h>
//#include "vkclass.h"
//#include "vk_Initializers.h"
#include "vk_cx.h"

#include "vk_cx1.h"

#include "vkc.h"
using namespace std;
#include "view.h"
#include "bw_sdl.h"

#include <base/promise_cx.h>

#include <shared.h>
MC_EXPORT void dslog(const char* d, int n = 0);

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
		VkInstance instance = 0;
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
	namespace vkc
	{
		VkSampler createSampler(dvk_device* dev, VkFilter filter, VkSamplerAddressMode addressMode);
		std::string errorString(uint32_t errorCode)
		{
			//VkResult
			int e = errorCode;
			switch (e)
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
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData) {

			std::cout << "validation layer: " << pCallbackData->pMessage << std::endl;

			return VK_FALSE;
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
				const char* layers[] = { "VK_LAYER_LUNARG_standard_validation" };
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
		VkInstance CreateInstance(std::set<std::string>* ext)
		{
			uint32_t extensions_count = 0;

			if (/*(!vkEnumerateInstanceExtensionProperties) ||*/ vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr) != VK_SUCCESS)
			{
				if (extensions_count == 0)
				{
					//LOGE("Error occurred during instance extensions enumeration!\n");
					return 0;
				}
			}

			std::vector<VkExtensionProperties> available_extensions(extensions_count);
			if (/*!vkEnumerateInstanceExtensionProperties || */vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, &available_extensions[0]) != VK_SUCCESS) {
				//LOGE("Error occurred during instance extensions enumeration!");
				return 0;
			}
			if (ext)
			{
				for (auto& it : available_extensions)
				{
					ext->insert(it.extensionName);
				}
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
				,"VK_KHR_get_physical_device_properties2"
			};
			std::vector<const char*> extensions1;
			for (auto it : extensions)
			{
				if (ext->find(it) != ext->end())
				{
					extensions1.push_back(it);
				}
				else {
					std::cout << "Could not find instance extension named \"" << it << "\"!" << std::endl;
				}
			}
			extensions.swap(extensions1);
			//for (size_t i = 0; i < extensions.size(); ++i) {
			//	if (!CheckExtensionAvailability(extensions[i], available_extensions)) {
			//		return 0;
			//	}
			//}
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

#if 1
			bool validation = false;
#ifdef _DEBUG
			validation = true;
#endif // _DEBUG


			if (extensions.size() > 0)
			{
#ifdef VK_EXT_DEBUG_UTILS_EXTENSION_NAME
				if (validation)
				{
					extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
					extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
				}
#endif
				instance_create_info.enabledExtensionCount = (uint32_t)extensions.size();
				instance_create_info.ppEnabledExtensionNames = extensions.data();
			}

			// The VK_LAYER_KHRONOS_validation contains all current validation functionality.
			// Note that on Android this layer requires at least NDK r20
			const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
			if (validation)
			{
				// Check if this layer is available at instance level
				uint32_t instanceLayerCount;
				vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
				std::vector<VkLayerProperties> instanceLayerProperties(instanceLayerCount);
				vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProperties.data());
				bool validationLayerPresent = false;
				for (VkLayerProperties layer : instanceLayerProperties) {
					if (strcmp(layer.layerName, validationLayerName) == 0) {
						validationLayerPresent = true;
						break;
					}
				}
				if (validationLayerPresent) {
					instance_create_info.ppEnabledLayerNames = &validationLayerName;
					//instance_create_info.enabledLayerCount = 1;
				}
				else {
					std::cerr << "Validation layer VK_LAYER_KHRONOS_validation not present, validation is disabled";
				}


			}
#endif

			if (!Instance && vkCreateInstance(&instance_create_info, nullptr, &Instance) != VK_SUCCESS) {
				std::cout << "Could not create Vulkan instance!" << std::endl;
				return 0;
			}

#ifdef _DEBUG
			if (validation)
			{
				VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				createInfo.messageSeverity =
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				createInfo.messageType =
					VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				// 指定消息回调函数
				createInfo.pfnUserCallback = debugCallback;
				createInfo.pUserData = nullptr; // 可选
				VkDebugUtilsMessengerEXT um = {};

				//auto cbk = vkCreateDebugUtilsMessengerEXT;
				auto kkf = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT");
				auto dcb = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT");
				if (kkf) {
					kkf(Instance, &createInfo, 0, &um);
				}
			}
#endif
			uint32_t iv = 0;
			//vkEnumerateInstanceVersion(&iv);
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


		VkResult createLogicalDevice(dvk_device* dev, VkPhysicalDeviceFeatures enabledFeatures, std::vector<const char*> enabledExtensions, void* pNextChain, bool useSwapChain = true
			, VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT)
		{
			// Desired queues need to be requested upon logical device creation
			// Due to differing queue family configurations of Vulkan implementations this can be a bit tricky, especially if the application
			// requests different queue types
			assert(dev && dev->physicalDevice);
			if (dev && dev->device)
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

		// If a pNext(Chain) has been passed, we need to add it to the device creation info
			VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{};
			if (pNextChain) {
				physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
				physicalDeviceFeatures2.features = enabledFeatures;
				physicalDeviceFeatures2.pNext = pNextChain;
				deviceCreateInfo.pEnabledFeatures = nullptr;
				deviceCreateInfo.pNext = &physicalDeviceFeatures2;
			}
			// Enable the debug marker extension if it is present (likely meaning a debugging tool is present)
#ifdef _DEBUG
			if (dev->extensionSupported(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
			{
				deviceExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
				dev->enableDebugMarkers = true;
			}

#endif
			if (deviceExtensions.size() > 0)
			{
				deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
				deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
				printf("enabledExtensionCount：%d\n", deviceCreateInfo.enabledExtensionCount);
				for (auto it : deviceExtensions)
					printf("\t%s\n", it);
			}
			VkResult result;
			{
				print_time ftpt("vkCreateDevice");
				result = vkCreateDevice(dev->physicalDevice, &deviceCreateInfo, nullptr, &dev->device);
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
			, size_t size, VkBuffer* buffer, VkDeviceMemory* memory, void* data, size_t* cap_size, void* _this)
		{

			// Create the buffer handle
			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.usage = usageFlags;
			bufferCreateInfo.size = size;
			bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			if (!(*buffer))
				VK_CHECK_RESULT(vkCreateBuffer(dev->device, &bufferCreateInfo, nullptr, buffer));

			// Create the memory backing up the buffer handle
			VkMemoryRequirements memReqs;
			VkMemoryAllocateInfo memAlloc = {};
			memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			vkGetBufferMemoryRequirements(dev->device, *buffer, &memReqs);
			//if (memReqs.size < 4096)
			//{
			//	memAlloc.allocationSize = 4096;
			//}
			//else
			{
				memAlloc.allocationSize = memReqs.size;
			}
			assert(memAlloc.allocationSize >= size);
			// Find a memory type index that fits the properties of the buffer
			VkBool32 memTypeFound = 0;
			memAlloc.memoryTypeIndex = vkc::getMemoryType(dev, memReqs.memoryTypeBits, memoryPropertyFlags, &memTypeFound);
#ifdef _WIN32
			printf("create_buffer\t[%p]\tallocationSize:%d\tsize:%d\n", _this, (int)memAlloc.allocationSize, (int)size);
#endif
			if (*memory)
				assert(0);
			VK_CHECK_RESULT(vkAllocateMemory(dev->device, &memAlloc, nullptr, memory));

			if (cap_size)
			{
				*cap_size = size;// memAlloc.allocationSize;
			}
			// If a pointer to the buffer data has been passed, map the buffer and copy over the data
			// 如果已传递指向缓冲区数据的指针，请映射缓冲区并在数据上进行复制
			if (data != nullptr && !(memoryPropertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
			{
				void* mapped = 0;
				VK_CHECK_RESULT(vkMapMemory(dev->device, *memory, 0, size, 0, &mapped));
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
					vkFlushMappedMemoryRanges(dev->device, 1, &mappedRange);
				}
				vkUnmapMemory(dev->device, *memory);
			}
			// Attach the memory to the buffer object
			VK_CHECK_RESULT(vkBindBufferMemory(dev->device, *buffer, *memory, 0));

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
			return vkCreateSampler(dev->device, info ? info : &sampler_create_info, nullptr, sampler) == VK_SUCCESS;
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
			VK_CHECK_RESULT(vkCreateSampler(dev->device, &sampler_create_info, nullptr, &sampler));
			return sampler;
		}
		//创建图像
		int64_t createImage(dvk_device* dev, VkImageCreateInfo* imageinfo, VkImageViewCreateInfo* viewinfo
			, dvk_texture* texture, VkSampler* sampler = nullptr, VkSamplerCreateInfo* info = nullptr)
		{
			VkImage* image = &texture->_image;
			VkDeviceMemory mem = texture->image_memory;
			VkImageView* imageview;
			VkMemoryAllocateInfo memAlloc = {};
			memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			VkMemoryRequirements memReqs;
			if (texture->_image)
			{
				vkDestroyImage(dev->device, texture->_image, 0); texture->_image = 0;
			}
			auto hr = (vkCreateImage(dev->device, imageinfo, nullptr, image));
			vkGetImageMemoryRequirements(dev->device, texture->_image, &memReqs);
			// 设备内存分配空间小于需求的重新申请内存
			if (texture->cap_device_mem_size < memReqs.size || (texture->caps < texture->cap_inc))
			{
				texture->cap_inc = 0;
				if (texture->image_memory)
				{
					vkFreeMemory(dev->device, texture->image_memory, 0);
					texture->image_memory = 0;
				}
				memAlloc.allocationSize = memReqs.size;
				texture->cap_device_mem_size = memReqs.size;
				VkBool32 memTypeFound = 0;
				uint32_t dh = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
				if ((imageinfo->usage & VK_IMAGE_USAGE_STORAGE_BIT))
				{
					dh = (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
				}
				do {
					memAlloc.memoryTypeIndex = vkc::getMemoryType(dev, memReqs.memoryTypeBits, dh, &memTypeFound);
					if (!memTypeFound && dh != VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
					{
						dh = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
						continue;
					}
					break;
				} while (1);

				auto hr = vkAllocateMemory(dev->device, &memAlloc, nullptr, &mem);
				assert(hr == VK_SUCCESS);
				texture->image_memory = mem;
			}
			texture->cap_inc++;
			// 绑定显存
			VK_CHECK_RESULT(vkBindImageMemory(dev->device, texture->_image, mem, 0));

			viewinfo->image = texture->_image;
			if (texture->_view)
			{
				vkDestroyImageView(dev->device, texture->_view, 0);
			}
			VK_CHECK_RESULT(vkCreateImageView(dev->device, viewinfo, nullptr, &texture->_view));
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
			VK_CHECK_RESULT(vkCreateSemaphore(dev->device, semaphoreCreateInfo, nullptr, semaphore));
		}

		VkFence createFence(dvk_device* dev, VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT)
		{
			VkFenceCreateInfo fence_create_info = {
				VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,              // VkStructureType                sType
				nullptr,                                          // const void                    *pNext
				flags                      // VkFenceCreateFlags             flags
			};

			VkFence fence = 0;
			VK_CHECK_RESULT(vkCreateFence(dev->device, &fence_create_info, nullptr, &fence));
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
			int err = 0;
			// If requested, also start recording for the new command buffer
			if (begin)
			{
				err = vkResetCommandPool(dev, commandPool, 0);
				VkCommandBufferBeginInfo cmdBufInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, 0, 0, 0 };// hz::initializers::commandBufferBeginInfo();
				VK_CHECK_RESULT(vkBeginCommandBuffer(cmdBuffer, &cmdBufInfo));
			}

			return cmdBuffer;
		}

		bool flushCommandBuffer(VkDevice dev, VkCommandBuffer commandBuffer, VkCommandPool commandPool, VkQueue queue, bool free)
		{
			if (commandBuffer == VK_NULL_HANDLE)
			{
				return false;
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
			VkFence fence = 0;
			VK_CHECK_RESULT(vkCreateFence(dev, &fenceInfo, nullptr, &fence));

			// Submit to the queue
			auto hr = (vkQueueSubmit(queue, 1, &submitInfo, fence));
			if (hr == VK_SUCCESS)
			{

				// Wait for the fence to signal that command buffer has finished executing
				hr = (vkWaitForFences(dev, 1, &fence, VK_TRUE, 1000));

			}
			vkDestroyFence(dev, fence, nullptr);

			if (free)
			{
				vkFreeCommandBuffers(dev, commandPool, 1, &commandBuffer);
				commandBuffer = 0;
			}
			return hr == VK_SUCCESS;
		}

		VkPipelineShaderStageCreateInfo load_shader(dvk_device* dev, const char* data, size_t size, size_t stage, const char* pName)
		{
			VkShaderModule shader_module = dev->CreateShaderModule((char*)data, size);
			VkPipelineShaderStageCreateInfo shader_stage_create_infos =
			{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,        // VkStructureType                                sType
				nullptr,                                                    // const void                                    *pNext
				0,                                                          // VkPipelineShaderStageCreateFlags               flags
				(VkShaderStageFlagBits)stage,                               // VkShaderStageFlagBits                          stage
				shader_module,												// VkShaderModule                                 module
				pName,														// const char                                    *pName
				nullptr                                                     // const VkSpecializationInfo                    *pSpecializationInfo
			};
			return shader_stage_create_infos;
		}


		void get_blend(bool blend, VkPipelineColorBlendAttachmentState& out)
		{
			VkPipelineColorBlendAttachmentState color_blend =
			{
				VK_TRUE,                                                      // VkBool32                                       blendEnable
				VK_BLEND_FACTOR_SRC_ALPHA,                                    // VkBlendFactor                                  srcColorBlendFactor
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,                          // VkBlendFactor                                  dstColorBlendFactor
				VK_BLEND_OP_ADD,                                              // VkBlendOp                                      colorBlendOp
				VK_BLEND_FACTOR_ONE,                                          // VkBlendFactor                                  srcAlphaBlendFactor
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,                          // VkBlendFactor                                  dstAlphaBlendFactor
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

		void Trace(const std::string& str)
		{
			std::mutex mutex;
			std::unique_lock<std::mutex> lock(mutex);

			// Output to attached debugger
#ifdef _WIN32
			OutputDebugStringA(str.c_str());
#endif
			// Also log to file
			//Log::Trace(str.c_str());
		}

		void Trace(const char* pFormat, ...)
		{
			std::mutex mutex;
			std::unique_lock<std::mutex> lock(mutex);

			va_list args;

			// generate formatted string
			va_start(args, pFormat);
			//MessageBuffer buf(bufLen);
			t_string buf;
#ifdef _WIN32
			const size_t bufLen = (size_t)_vscprintf(pFormat, args) + 2;
			buf.resize(bufLen);
			vsnprintf_s(buf.data(), bufLen, bufLen, pFormat, args);
#else
			buf.resize(2048);
			vsprintf(buf.data(), pFormat, args);
#endif
			va_end(args);
			//strcat_s(buf.data(), bufLen, "\n");
			buf = buf.c_str();
			buf.push_back('\n');
			// Output to attached debugger
#ifdef _WIN32
			OutputDebugStringA(buf.data());
#endif
			// Also log to file
			//Log::Trace(buf.Data());
		}


		class Device
		{
		public:
			Device();
			~Device();
			void OnCreate(const char* pAppName, const char* pEngineName, bool cpuValidationLayerEnabled, bool gpuValidationLayerEnabled, HWND hWnd);
			void SetEssentialInstanceExtensions(bool cpuValidationLayerEnabled, bool gpuValidationLayerEnabled, InstanceProperties* pIp);
			void SetEssentialDeviceExtensions(DeviceProperties* pDp);
			void OnCreateEx(VkInstance vulkanInstance, VkPhysicalDevice physicalDevice, HWND hWnd, DeviceProperties* pDp);
			void OnDestroy();
			VkDevice GetDevice() { return m_device; }
			VkQueue GetGraphicsQueue() { return graphics_queue; }
			uint32_t GetGraphicsQueueFamilyIndex() { return present_queue_family_index; }
			VkQueue GetPresentQueue() { return present_queue; }
			uint32_t GetPresentQueueFamilyIndex() { return graphics_queue_family_index; }
			VkQueue GetComputeQueue() { return compute_queue; }
			uint32_t GetComputeQueueFamilyIndex() { return compute_queue_family_index; }
			VkPhysicalDevice GetPhysicalDevice() { return m_physicaldevice; }
			VkSurfaceKHR GetSurface() { return m_surface; }
			void GetDeviceInfo(std::string* deviceName, std::string* driverVersion);
#ifdef USE_VMA
			VmaAllocator GetAllocator() { return m_hAllocator; }
#endif
			VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties() { return m_memoryProperties; }
			VkPhysicalDeviceProperties GetPhysicalDeviceProperries() { return m_deviceProperties; }
			VkPhysicalDeviceSubgroupProperties GetPhysicalDeviceSubgroupProperties() { return m_subgroupProperties; }

			bool IsFp16Supported() { return m_usingFp16; };
			bool IsRT10Supported() { return m_rt10Supported; }
			bool IsRT11Supported() { return m_rt11Supported; }
			bool IsVRSTier1Supported() { return m_vrs1Supported; }
			bool IsVRSTier2Supported() { return m_vrs2Supported; }

			// pipeline cache
			VkPipelineCache m_pipelineCache;
			void CreatePipelineCache();
			void DestroyPipelineCache();
			VkPipelineCache GetPipelineCache();

			void CreateShaderCache() {};
			void DestroyShaderCache() {};

			void GPUFlush();

		private:
			VkInstance m_instance;
			VkDevice m_device;
			VkPhysicalDevice m_physicaldevice;
			VkPhysicalDeviceMemoryProperties m_memoryProperties;
			VkPhysicalDeviceProperties m_deviceProperties;
			VkPhysicalDeviceProperties2 m_deviceProperties2;
			VkPhysicalDeviceSubgroupProperties m_subgroupProperties;
			VkSurfaceKHR m_surface;

			VkQueue present_queue;
			uint32_t present_queue_family_index;
			VkQueue graphics_queue;
			uint32_t graphics_queue_family_index;
			VkQueue compute_queue;
			uint32_t compute_queue_family_index;

			bool m_usingValidationLayer = false;
			bool m_usingFp16 = false;
			bool m_rt10Supported = false;
			bool m_rt11Supported = false;
			bool m_vrs1Supported = false;
			bool m_vrs2Supported = false;
#ifdef USE_VMA
			VmaAllocator m_hAllocator = NULL;
#endif
		};



		bool DeviceProperties::IsExtensionPresent(const char* pExtName)
		{
			return std::find_if(
				m_deviceExtensionProperties.begin(),
				m_deviceExtensionProperties.end(),
				[pExtName](const VkExtensionProperties& extensionProps) -> bool {
					return strcmp(extensionProps.extensionName, pExtName) == 0;
				}) != m_deviceExtensionProperties.end();
		}

		uint32_t DeviceProperties::Init(VkPhysicalDevice physicaldevice)
		{
			m_physicaldevice = physicaldevice;

			// Enumerate device extensions
			//
			uint32_t extensionCount;
			VkResult res = vkEnumerateDeviceExtensionProperties(physicaldevice, nullptr, &extensionCount, NULL);
			m_deviceExtensionProperties.resize(extensionCount);
			res = vkEnumerateDeviceExtensionProperties(physicaldevice, nullptr, &extensionCount, m_deviceExtensionProperties.data());

			return res;
		}

		bool DeviceProperties::AddDeviceExtensionName(const char* deviceExtensionName)
		{
			if (IsExtensionPresent(deviceExtensionName))
			{
				m_device_extension_names.push_back(deviceExtensionName);
				return true;
			}

			Trace("The device extension '%s' has not been found", deviceExtensionName);

			return false;
		}

		void  DeviceProperties::GetExtensionNamesAndConfigs(std::vector<const char*>* pDevice_extension_names)
		{
			for (auto& name : m_device_extension_names)
				pDevice_extension_names->push_back(name);
		}

		bool InstanceProperties::IsLayerPresent(const char* pExtName)
		{
			return std::find_if(
				m_instanceLayerProperties.begin(),
				m_instanceLayerProperties.end(),
				[pExtName](const VkLayerProperties& layerProps) -> bool {
					return strcmp(layerProps.layerName, pExtName) == 0;
				}) != m_instanceLayerProperties.end();
		}

		bool InstanceProperties::IsExtensionPresent(const char* pExtName)
		{
			return std::find_if(
				m_instanceExtensionProperties.begin(),
				m_instanceExtensionProperties.end(),
				[pExtName](const VkExtensionProperties& extensionProps) -> bool {
					return strcmp(extensionProps.extensionName, pExtName) == 0;
				}) != m_instanceExtensionProperties.end();
		}

		uint32_t InstanceProperties::Init()
		{
			// Query instance layers.
			//
			uint32_t instanceLayerPropertyCount = 0;
			VkResult res = vkEnumerateInstanceLayerProperties(&instanceLayerPropertyCount, nullptr);
			m_instanceLayerProperties.resize(instanceLayerPropertyCount);
			assert(res == VK_SUCCESS);
			if (instanceLayerPropertyCount > 0)
			{
				res = vkEnumerateInstanceLayerProperties(&instanceLayerPropertyCount, m_instanceLayerProperties.data());
				assert(res == VK_SUCCESS);
			}

			// Query instance extensions.
			//
			uint32_t instanceExtensionPropertyCount = 0;
			res = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionPropertyCount, nullptr);
			assert(res == VK_SUCCESS);
			m_instanceExtensionProperties.resize(instanceExtensionPropertyCount);
			if (instanceExtensionPropertyCount > 0)
			{
				res = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionPropertyCount, m_instanceExtensionProperties.data());
				assert(res == VK_SUCCESS);
			}

			return res;
		}

		bool InstanceProperties::AddInstanceLayerName(const char* instanceLayerName)
		{
			if (IsLayerPresent(instanceLayerName))
			{
				m_instance_layer_names.push_back(instanceLayerName);
				return true;
			}

			Trace("The instance layer '%s' has not been found\n", instanceLayerName);

			return false;
		}

		bool InstanceProperties::AddInstanceExtensionName(const char* instanceExtensionName)
		{
			if (IsExtensionPresent(instanceExtensionName))
			{
				m_instance_extension_names.push_back(instanceExtensionName);
				return true;
			}

			Trace("The instance extension '%s' has not been found\n", instanceExtensionName);

			return false;
		}

		void  InstanceProperties::GetExtensionNamesAndConfigs(std::vector<const char*>* pInstance_layer_names, std::vector<const char*>* pInstance_extension_names)
		{
			for (auto& name : m_instance_layer_names)
				pInstance_layer_names->push_back(name);

			for (auto& name : m_instance_extension_names)
				pInstance_extension_names->push_back(name);
		}

#if 0
		Device::Device()
		{
		}

		Device::~Device()
		{
		}

		void Device::OnCreate(const char* pAppName, const char* pEngineName, bool cpuValidationLayerEnabled, bool gpuValidationLayerEnabled, HWND hWnd)
		{
			InstanceProperties ip;
			ip.Init();
			SetEssentialInstanceExtensions(cpuValidationLayerEnabled, gpuValidationLayerEnabled, &ip);

			// Create instance
			VkInstance vulkanInstance;
			VkPhysicalDevice physicalDevice;
			CreateInstance(pAppName, pEngineName, &vulkanInstance, &physicalDevice, &ip);

			DeviceProperties dp;
			dp.Init(physicalDevice);
			SetEssentialDeviceExtensions(&dp);

			// Create device
			OnCreateEx(vulkanInstance, physicalDevice, hWnd, &dp);
		}

		void Device::SetEssentialInstanceExtensions(bool cpuValidationLayerEnabled, bool gpuValidationLayerEnabled, InstanceProperties* pIp)
		{
#ifdef VK_KHR_WIN32_SURFACE_EXTENSION_NAME
			pIp->AddInstanceExtensionName(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
#ifdef VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
			pIp->AddInstanceExtensionName(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif
			pIp->AddInstanceExtensionName(VK_KHR_SURFACE_EXTENSION_NAME);
			ExtCheckHDRInstanceExtensions(pIp);
			ExtDebugUtilsCheckInstanceExtensions(pIp);
			if (cpuValidationLayerEnabled)
			{
				ExtDebugReportCheckInstanceExtensions(pIp, gpuValidationLayerEnabled);
			}
		}

		void Device::SetEssentialDeviceExtensions(DeviceProperties* pDp)
		{
			m_usingFp16 = ExtFp16CheckExtensions(pDp);
			ExtRTCheckExtensions(pDp, m_rt10Supported, m_rt11Supported);
			ExtVRSCheckExtensions(pDp, m_vrs1Supported, m_vrs2Supported);
			ExtCheckHDRDeviceExtensions(pDp);
			ExtCheckFSEDeviceExtensions(pDp);
			ExtCheckFreeSyncHDRDeviceExtensions(pDp);
			pDp->AddDeviceExtensionName(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
			pDp->AddDeviceExtensionName(VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME);
		}

		void Device::OnCreateEx(VkInstance vulkanInstance, VkPhysicalDevice physicalDevice, HWND hWnd, DeviceProperties* pDp)
		{
			VkResult res;

			m_instance = vulkanInstance;
			m_physicaldevice = physicalDevice;

			// Get queue/memory/device properties
			//
			uint32_t queue_family_count;
			vkGetPhysicalDeviceQueueFamilyProperties(m_physicaldevice, &queue_family_count, NULL);
			assert(queue_family_count >= 1);

			std::vector<VkQueueFamilyProperties> queue_props;
			queue_props.resize(queue_family_count);
			vkGetPhysicalDeviceQueueFamilyProperties(m_physicaldevice, &queue_family_count, queue_props.data());
			assert(queue_family_count >= 1);

			vkGetPhysicalDeviceMemoryProperties(m_physicaldevice, &m_memoryProperties);
			vkGetPhysicalDeviceProperties(m_physicaldevice, &m_deviceProperties);

			// Get subgroup properties to check if subgroup operations are supported
			//
			m_subgroupProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES;
			m_subgroupProperties.pNext = NULL;

			m_deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
			m_deviceProperties2.pNext = &m_subgroupProperties;

			vkGetPhysicalDeviceProperties2(m_physicaldevice, &m_deviceProperties2);

#if defined(_WIN32)
			// Crate a Win32 Surface
			//
			VkWin32SurfaceCreateInfoKHR createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			createInfo.pNext = NULL;
			createInfo.hinstance = NULL;
			createInfo.hwnd = hWnd;
			res = vkCreateWin32SurfaceKHR(m_instance, &createInfo, NULL, &m_surface);
#else
			//#error platform not supported
#endif
			// Find a graphics device and a queue that can present to the above surface
			//
			graphics_queue_family_index = UINT32_MAX;
			present_queue_family_index = UINT32_MAX;
			for (uint32_t i = 0; i < queue_family_count; ++i)
			{
				if ((queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
				{
					if (graphics_queue_family_index == UINT32_MAX) graphics_queue_family_index = i;

					VkBool32 supportsPresent;
					vkGetPhysicalDeviceSurfaceSupportKHR(m_physicaldevice, i, m_surface, &supportsPresent);
					if (supportsPresent == VK_TRUE)
					{
						graphics_queue_family_index = i;
						present_queue_family_index = i;
						break;
					}
				}
			}

			// If didn't find a queue that supports both graphics and present, then
			// find a separate present queue.
			if (present_queue_family_index == UINT32_MAX)
			{
				for (uint32_t i = 0; i < queue_family_count; ++i)
				{
					VkBool32 supportsPresent;
					vkGetPhysicalDeviceSurfaceSupportKHR(m_physicaldevice, i, m_surface, &supportsPresent);
					if (supportsPresent == VK_TRUE)
					{
						present_queue_family_index = (uint32_t)i;
						break;
					}
				}
			}

			compute_queue_family_index = UINT32_MAX;

			for (uint32_t i = 0; i < queue_family_count; ++i)
			{
				if ((queue_props[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
				{
					if (compute_queue_family_index == UINT32_MAX)
						compute_queue_family_index = i;
					if (i != graphics_queue_family_index) {
						compute_queue_family_index = i;
						break;
					}
				}
			}

			// prepare existing extensions names into a buffer for vkCreateDevice
			std::vector<const char*> extension_names;
			pDp->GetExtensionNamesAndConfigs(&extension_names);

			// Create device 
			//
			float queue_priorities[1] = { 0.0 };
			VkDeviceQueueCreateInfo queue_info[2] = {};
			queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_info[0].pNext = NULL;
			queue_info[0].queueCount = 1;
			queue_info[0].pQueuePriorities = queue_priorities;
			queue_info[0].queueFamilyIndex = graphics_queue_family_index;
			queue_info[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_info[1].pNext = NULL;
			queue_info[1].queueCount = 1;
			queue_info[1].pQueuePriorities = queue_priorities;
			queue_info[1].queueFamilyIndex = compute_queue_family_index;

			VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
			physicalDeviceFeatures.fillModeNonSolid = true;
			physicalDeviceFeatures.pipelineStatisticsQuery = true;
			physicalDeviceFeatures.fragmentStoresAndAtomics = true;
			physicalDeviceFeatures.vertexPipelineStoresAndAtomics = true;
			physicalDeviceFeatures.shaderImageGatherExtended = true;
			physicalDeviceFeatures.wideLines = true; //needed for drawing lines with a specific width.
			physicalDeviceFeatures.independentBlend = true; // needed for having different blend for each render target 

			// enable feature to support fp16 with subgroup operations
			//
			VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR shaderSubgroupExtendedType = {};
			shaderSubgroupExtendedType.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR;
			shaderSubgroupExtendedType.pNext = pDp->GetNext(); //used to be pNext of VkDeviceCreateInfo
			shaderSubgroupExtendedType.shaderSubgroupExtendedTypes = VK_TRUE;

			VkPhysicalDeviceRobustness2FeaturesEXT robustness2 = {};
			robustness2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
			robustness2.pNext = &shaderSubgroupExtendedType;
			robustness2.nullDescriptor = VK_TRUE;

			// to be able to bind NULL views
			VkPhysicalDeviceFeatures2 physicalDeviceFeatures2 = {};
			physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
			physicalDeviceFeatures2.features = physicalDeviceFeatures;
			physicalDeviceFeatures2.pNext = &robustness2;

			VkDeviceCreateInfo device_info = {};
			device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			device_info.pNext = &physicalDeviceFeatures2;
			device_info.queueCreateInfoCount = 2;
			device_info.pQueueCreateInfos = queue_info;
			device_info.enabledExtensionCount = (uint32_t)extension_names.size();
			device_info.ppEnabledExtensionNames = device_info.enabledExtensionCount ? extension_names.data() : NULL;
			device_info.pEnabledFeatures = NULL;
			res = vkCreateDevice(m_physicaldevice, &device_info, NULL, &m_device);
			assert(res == VK_SUCCESS);

#ifdef USE_VMA
			VmaAllocatorCreateInfo allocatorInfo = {};
			allocatorInfo.physicalDevice = GetPhysicalDevice();
			allocatorInfo.device = GetDevice();
			allocatorInfo.instance = m_instance;
			vmaCreateAllocator(&allocatorInfo, &m_hAllocator);
#endif

			// create queues
			//
			vkGetDeviceQueue(m_device, graphics_queue_family_index, 0, &graphics_queue);
			if (graphics_queue_family_index == present_queue_family_index)
			{
				present_queue = graphics_queue;
			}
			else
			{
				vkGetDeviceQueue(m_device, present_queue_family_index, 0, &present_queue);
			}
			if (compute_queue_family_index != UINT32_MAX)
			{
				vkGetDeviceQueue(m_device, compute_queue_family_index, 0, &compute_queue);
			}

			// Init the extensions (if they have been enabled successfuly)
			//
			ExtDebugUtilsGetProcAddresses(m_device);
			ExtGetHDRFSEFreesyncHDRProcAddresses(m_instance, m_device);
		}

		void Device::GetDeviceInfo(std::string* deviceName, std::string* driverVersion)
		{
#define EXTRACT(v,offset, length) ((v>>offset) & ((1<<length)-1))
			* deviceName = m_deviceProperties.deviceName;
			*driverVersion = format("%i.%i.%i", EXTRACT(m_deviceProperties.driverVersion, 22, 10), EXTRACT(m_deviceProperties.driverVersion, 14, 8), EXTRACT(m_deviceProperties.driverVersion, 0, 16));
		}

		void Device::CreatePipelineCache()
		{
			// create pipeline cache

			VkPipelineCacheCreateInfo pipelineCache;
			pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			pipelineCache.pNext = NULL;
			pipelineCache.initialDataSize = 0;
			pipelineCache.pInitialData = NULL;
			pipelineCache.flags = 0;
			VkResult res = vkCreatePipelineCache(m_device, &pipelineCache, NULL, &m_pipelineCache);
			assert(res == VK_SUCCESS);
		}

		void Device::DestroyPipelineCache()
		{
			vkDestroyPipelineCache(m_device, m_pipelineCache, NULL);
		}

		VkPipelineCache Device::GetPipelineCache()
		{
			return m_pipelineCache;
		}

		void Device::OnDestroy()
		{
			if (m_surface != VK_NULL_HANDLE)
			{
				vkDestroySurfaceKHR(m_instance, m_surface, NULL);
			}

#ifdef USE_VMA
			vmaDestroyAllocator(m_hAllocator);
			m_hAllocator = NULL;
#endif

			if (m_device != VK_NULL_HANDLE)
			{
				vkDestroyDevice(m_device, nullptr);
				m_device = VK_NULL_HANDLE;
			}

			ExtDebugReportOnDestroy(m_instance);

			DestroyInstance(m_instance);

			m_instance = VK_NULL_HANDLE;
		}

		void Device::GPUFlush()
		{
			vkDeviceWaitIdle(m_device);
		}
#endif
		bool memory_type_from_properties(VkPhysicalDeviceMemoryProperties& memory_properties, uint32_t typeBits, VkFlags requirements_mask, uint32_t* typeIndex) {
			// Search memtypes to find first index with those properties
			for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
				if ((typeBits & 1) == 1) {
					// Type is available, does it match user properties?
					if ((memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
						*typeIndex = i;
						return true;
					}
				}
				typeBits >>= 1;
			}
			// No memory types matched, return failure
			return false;
		}
#ifdef _WIN32


		extern PFN_vkGetDeviceProcAddr                        g_vkGetDeviceProcAddr = NULL;

		// Functions for regular HDR ex: HDR10
		extern PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR g_vkGetPhysicalDeviceSurfaceCapabilities2KHR = NULL;
		extern PFN_vkGetPhysicalDeviceSurfaceFormats2KHR      g_vkGetPhysicalDeviceSurfaceFormats2KHR = NULL;
		extern PFN_vkSetHdrMetadataEXT                        g_vkSetHdrMetadataEXT = NULL;

		// Functions for FSE required if trying to use Freesync HDR
		extern PFN_vkAcquireFullScreenExclusiveModeEXT        g_vkAcquireFullScreenExclusiveModeEXT = NULL;
		extern PFN_vkReleaseFullScreenExclusiveModeEXT        g_vkReleaseFullScreenExclusiveModeEXT = NULL;

		// Functions for Freesync HDR
		extern PFN_vkSetLocalDimmingAMD                       g_vkSetLocalDimmingAMD = NULL;

		//static VkPhysicalDeviceSurfaceInfo2KHR s_PhysicalDeviceSurfaceInfo2KHR;

		//static VkSurfaceFullScreenExclusiveWin32InfoEXT s_SurfaceFullScreenExclusiveWin32InfoEXT;
		//static VkSurfaceFullScreenExclusiveInfoEXT s_SurfaceFullScreenExclusiveInfoEXT;

		//static VkDisplayNativeHdrSurfaceCapabilitiesAMD s_DisplayNativeHdrSurfaceCapabilitiesAMD;

		static bool s_isHdrInstanceExtensionPresent = false;
		static bool s_isHdrDeviceExtensionsPresent = false;
		static bool s_isFSEDeviceExtensionsPresent = false;
		static bool s_isFSHDRDeviceExtensionsPresent = false;

		void ExtCheckHDRInstanceExtensions(InstanceProperties* pIP)
		{
			s_isHdrInstanceExtensionPresent = pIP->AddInstanceExtensionName(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);
		}

		void ExtCheckHDRDeviceExtensions(DeviceProperties* pDP)
		{
			s_isHdrDeviceExtensionsPresent = pDP->AddDeviceExtensionName(VK_EXT_HDR_METADATA_EXTENSION_NAME);
		}

		void ExtCheckFSEDeviceExtensions(DeviceProperties* pDP)
		{
			s_isFSEDeviceExtensionsPresent = pDP->AddDeviceExtensionName(VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME);
		}

		void ExtCheckFreeSyncHDRDeviceExtensions(DeviceProperties* pDP)
		{
			s_isFSHDRDeviceExtensionsPresent = pDP->AddDeviceExtensionName(VK_AMD_DISPLAY_NATIVE_HDR_EXTENSION_NAME);
		}

		void ExtGetHDRFSEFreesyncHDRProcAddresses(VkInstance instance, VkDevice device)
		{
			if (s_isHdrInstanceExtensionPresent)
			{
				g_vkGetPhysicalDeviceSurfaceCapabilities2KHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilities2KHR");
				assert(g_vkGetPhysicalDeviceSurfaceCapabilities2KHR);

				g_vkGetPhysicalDeviceSurfaceFormats2KHR = (PFN_vkGetPhysicalDeviceSurfaceFormats2KHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormats2KHR");
				assert(g_vkGetPhysicalDeviceSurfaceFormats2KHR);
			}

			g_vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(instance, "vkGetDeviceProcAddr");
			assert(g_vkGetDeviceProcAddr);

			if (s_isHdrDeviceExtensionsPresent)
			{
				g_vkSetHdrMetadataEXT = (PFN_vkSetHdrMetadataEXT)g_vkGetDeviceProcAddr(device, "vkSetHdrMetadataEXT");
				assert(g_vkSetHdrMetadataEXT);
			}

			if (s_isFSEDeviceExtensionsPresent)
			{
				g_vkAcquireFullScreenExclusiveModeEXT = (PFN_vkAcquireFullScreenExclusiveModeEXT)g_vkGetDeviceProcAddr(device, "vkAcquireFullScreenExclusiveModeEXT");
				assert(g_vkAcquireFullScreenExclusiveModeEXT);

				g_vkReleaseFullScreenExclusiveModeEXT = (PFN_vkReleaseFullScreenExclusiveModeEXT)g_vkGetDeviceProcAddr(device, "vkReleaseFullScreenExclusiveModeEXT");
				assert(g_vkReleaseFullScreenExclusiveModeEXT);
			}

			if (s_isFSHDRDeviceExtensionsPresent)
			{
				g_vkSetLocalDimmingAMD = (PFN_vkSetLocalDimmingAMD)g_vkGetDeviceProcAddr(device, "vkSetLocalDimmingAMD");
				assert(g_vkSetLocalDimmingAMD);
			}
		}

		bool ExtAreHDRExtensionsPresent()
		{
			return s_isHdrInstanceExtensionPresent && s_isHdrDeviceExtensionsPresent;
		}

		bool ExtAreFSEExtensionsPresent()
		{
			return s_isFSEDeviceExtensionsPresent;
		}

		bool ExtAreFreeSyncHDRExtensionsPresent()
		{
			return s_isHdrInstanceExtensionPresent &&
				s_isHdrDeviceExtensionsPresent &&
				s_isFSEDeviceExtensionsPresent &&
				s_isFSHDRDeviceExtensionsPresent;
		}

#endif // _WIN32

		uint32_t GetScore(VkPhysicalDevice physicalDevice)
		{
			uint32_t score = 0;

			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
			// Use the features for a more precise way to select the GPU
			//VkPhysicalDeviceFeatures deviceFeatures;
			//vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
			switch (deviceProperties.deviceType)
			{
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				score += 1000;
				break;
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				score += 10000;
				break;
			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
				score += 100;
				break;
			case VK_PHYSICAL_DEVICE_TYPE_CPU:
				score += 10;
				break;
			default:
				break;
			}

			// TODO: add other constraints

			return score;
		}

		// Select the best physical device.
		// For now, the code just gets the first discrete GPU.
		// If none is found, it default to an integrated then virtual then cpu one
		VkPhysicalDevice SelectPhysicalDevice(std::vector<VkPhysicalDevice>& physicalDevices)
		{
			assert(physicalDevices.size() > 0 && "No GPU found");

			std::multimap<uint32_t, VkPhysicalDevice> ratings;

			for (auto it = physicalDevices.begin(); it != physicalDevices.end(); ++it)
				ratings.insert(std::make_pair(GetScore(*it), *it));

			return ratings.rbegin()->second;
		}

		bool CreateInstance(const char* pAppName, const char* pEngineName, VkInstance* pVulkanInstance, VkPhysicalDevice* pPhysicalDevice, InstanceProperties* pIp)
		{
			VkApplicationInfo app_info = {};
			app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			app_info.pNext = NULL;
			app_info.pApplicationName = pAppName;
			app_info.applicationVersion = 1;
			app_info.pEngineName = pEngineName;
			app_info.engineVersion = 1;
			app_info.apiVersion = VK_API_VERSION_1_1;
			VkInstance instance = CreateInstance(app_info, pIp);

			// Enumerate physical devices
			uint32_t physicalDeviceCount = 1;
			uint32_t const req_count = physicalDeviceCount;
			VkResult res = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
			assert(physicalDeviceCount > 0 && "No GPU found");

			std::vector<VkPhysicalDevice> physicalDevices;
			physicalDevices.resize(physicalDeviceCount);
			res = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
			assert(res == VK_SUCCESS && physicalDeviceCount >= req_count && "Unable to enumerate physical devices");

			// get the best available gpu
			*pPhysicalDevice = SelectPhysicalDevice(physicalDevices);
			*pVulkanInstance = instance;

			return true;
		}

		VkInstance CreateInstance(VkApplicationInfo app_info, InstanceProperties* pIp)
		{
			VkInstance instance;

			// prepare existing extensions and layer names into a buffer for vkCreateInstance
			std::vector<const char*> instance_layer_names;
			std::vector<const char*> instance_extension_names;
			pIp->GetExtensionNamesAndConfigs(&instance_layer_names, &instance_extension_names);

			// do create the instance
			VkInstanceCreateInfo inst_info = {};
			inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			inst_info.pNext = pIp->GetNext();
			inst_info.flags = 0;
			inst_info.pApplicationInfo = &app_info;
			inst_info.enabledLayerCount = (uint32_t)instance_layer_names.size();
			inst_info.ppEnabledLayerNames = (uint32_t)instance_layer_names.size() ? instance_layer_names.data() : NULL;
			inst_info.enabledExtensionCount = (uint32_t)instance_extension_names.size();
			inst_info.ppEnabledExtensionNames = instance_extension_names.data();
			VkResult res = vkCreateInstance(&inst_info, NULL, &instance);
			assert(res == VK_SUCCESS);

			// Init the extensions (if they have been enabled successfuly)
			//
			//
#ifdef _WIN32
			ExtDebugReportGetProcAddresses(instance);
			ExtDebugReportOnCreate(instance);
#endif
			return instance;
		}

		void DestroyInstance(VkInstance instance)
		{
			vkDestroyInstance(instance, nullptr);
		}

		static VkPhysicalDeviceRayTracingPipelineFeaturesKHR RTPipelinesFeatures = {};
		static VkPhysicalDeviceRayQueryFeaturesKHR RTQueryFeatures = {};
		static VkPhysicalDeviceAccelerationStructureFeaturesKHR AccelerationStructureFeatures = {};
		static VkPhysicalDeviceBufferDeviceAddressFeatures BufferDeviceAddressFeatures = {};
		static VkPhysicalDeviceDescriptorIndexingFeatures DescriptorIndexingFeatures = {};
		void ExtRTCheckExtensions(DeviceProperties* pDP, bool& RT10Supported, bool& RT11Supported)
		{
#ifdef _WIN32
#ifdef VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME
			bool RT10ExtensionPresent = pDP->AddDeviceExtensionName(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
			bool RT11ExtensionPresent = pDP->AddDeviceExtensionName(VK_KHR_RAY_QUERY_EXTENSION_NAME);
			bool AccelerationStructureExtensionPresent = pDP->AddDeviceExtensionName(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
			bool BufferDeviceAddressPresent = pDP->AddDeviceExtensionName(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
			bool DescriptorIndexingExtensionPresent = pDP->AddDeviceExtensionName(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
			pDP->AddDeviceExtensionName(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
			pDP->AddDeviceExtensionName(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
			pDP->AddDeviceExtensionName(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);

			if (RT10ExtensionPresent)
			{
				RTPipelinesFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
				VkPhysicalDeviceFeatures2 features = {};
				features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
				features.pNext = &RTPipelinesFeatures;
				vkGetPhysicalDeviceFeatures2(pDP->GetPhysicalDevice(), &features);

				RT10Supported = RTPipelinesFeatures.rayTracingPipeline;

				if (RT10Supported)
				{
					RTPipelinesFeatures.pNext = pDP->GetNext();
					pDP->SetNewNext(&RTPipelinesFeatures);
				}
			}

			if (RT11ExtensionPresent)
			{
				RTQueryFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
				VkPhysicalDeviceFeatures2 features = {};
				features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
				features.pNext = &RTQueryFeatures;
				vkGetPhysicalDeviceFeatures2(pDP->GetPhysicalDevice(), &features);

				RT11Supported = RTQueryFeatures.rayQuery;

				if (RT11Supported)
				{
					RTQueryFeatures.pNext = pDP->GetNext();
					pDP->SetNewNext(&RTQueryFeatures);
				}
			}

			if (AccelerationStructureExtensionPresent)
			{
				AccelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
				VkPhysicalDeviceFeatures2 features = {};
				features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
				features.pNext = &AccelerationStructureFeatures;
				vkGetPhysicalDeviceFeatures2(pDP->GetPhysicalDevice(), &features);

				if (AccelerationStructureFeatures.accelerationStructure == VK_TRUE)
				{
					// we already have 2 feature structures chained, so we aren't setting pNext to the same structure that will become the Next one
					AccelerationStructureFeatures.pNext = pDP->GetNext();
					pDP->SetNewNext(&AccelerationStructureFeatures);
				}
			}

			if (DescriptorIndexingExtensionPresent)
			{
				DescriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
				VkPhysicalDeviceFeatures2 features = {};
				features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
				features.pNext = &DescriptorIndexingFeatures;
				vkGetPhysicalDeviceFeatures2(pDP->GetPhysicalDevice(), &features);

				DescriptorIndexingFeatures.pNext = pDP->GetNext();
				pDP->SetNewNext(&DescriptorIndexingFeatures);
			}

			if (BufferDeviceAddressPresent)
			{
				BufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
				VkPhysicalDeviceFeatures2 features = {};
				features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
				features.pNext = &BufferDeviceAddressFeatures;
				vkGetPhysicalDeviceFeatures2(pDP->GetPhysicalDevice(), &features);

				if (BufferDeviceAddressFeatures.bufferDeviceAddress == VK_TRUE)
				{
					BufferDeviceAddressFeatures.pNext = pDP->GetNext();
					pDP->SetNewNext(&BufferDeviceAddressFeatures);
				}
			}
#endif
#endif
		}
#if 1

#ifdef _WIN32
		static PFN_vkCreateDebugReportCallbackEXT g_vkCreateDebugReportCallbackEXT = NULL;
		static PFN_vkDebugReportMessageEXT g_vkDebugReportMessageEXT = NULL;
		static PFN_vkDestroyDebugReportCallbackEXT g_vkDestroyDebugReportCallbackEXT = NULL;
		static VkDebugReportCallbackEXT g_DebugReportCallback = NULL;

		static bool s_bCanUseDebugReport = false;

		static VKAPI_ATTR VkBool32 VKAPI_CALL MyDebugReportCallback(
			VkDebugReportFlagsEXT flags,
			VkDebugReportObjectTypeEXT objectType,
			uint64_t object,
			size_t location,
			int32_t messageCode,
			const char* pLayerPrefix,
			const char* pMessage,
			void* pUserData)
		{
#ifdef _WIN32
			OutputDebugStringA(pMessage);
			OutputDebugStringA("\n");
#endif
			return VK_FALSE;
		}
#ifndef _countof
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif
		const VkValidationFeatureEnableEXT featuresRequested[] = { VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT, VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT, VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT };
		VkValidationFeaturesEXT features = {};

		const char instanceExtensionName[] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
		const char instanceLayerName[] = "VK_LAYER_KHRONOS_validation";

		bool ExtDebugReportCheckInstanceExtensions(InstanceProperties* pIP, bool gpuValidation)
		{
			s_bCanUseDebugReport = pIP->AddInstanceLayerName(instanceLayerName) && pIP->AddInstanceExtensionName(instanceExtensionName);
			if (s_bCanUseDebugReport && gpuValidation)
			{
				features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
				features.pNext = pIP->GetNext();
				features.enabledValidationFeatureCount = _countof(featuresRequested);
				features.pEnabledValidationFeatures = featuresRequested;

				pIP->SetNewNext(&features);
			}

			return s_bCanUseDebugReport;
		}

		void ExtDebugReportGetProcAddresses(VkInstance instance)
		{
			if (s_bCanUseDebugReport)
			{
				g_vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
				g_vkDebugReportMessageEXT = (PFN_vkDebugReportMessageEXT)vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT");
				g_vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
				assert(g_vkCreateDebugReportCallbackEXT);
				assert(g_vkDebugReportMessageEXT);
				assert(g_vkDestroyDebugReportCallbackEXT);
			}
		}

		void ExtDebugReportOnCreate(VkInstance instance)
		{
			if (g_vkCreateDebugReportCallbackEXT)
			{
				VkDebugReportCallbackCreateInfoEXT debugReportCallbackInfo = { VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT };
				debugReportCallbackInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
				debugReportCallbackInfo.pfnCallback = MyDebugReportCallback;
				VkResult res = g_vkCreateDebugReportCallbackEXT(instance, &debugReportCallbackInfo, nullptr, &g_DebugReportCallback);
				assert(res == VK_SUCCESS);
			}
		}

		void ExtDebugReportOnDestroy(VkInstance instance)
		{
			// It should happen after destroing device, before destroying instance.
			if (g_DebugReportCallback)
			{
				g_vkDestroyDebugReportCallbackEXT(instance, g_DebugReportCallback, nullptr);
				g_DebugReportCallback = nullptr;
			}
		}


		static PFN_vkSetDebugUtilsObjectNameEXT     s_vkSetDebugUtilsObjectName = nullptr;
		static PFN_vkCmdBeginDebugUtilsLabelEXT     s_vkCmdBeginDebugUtilsLabel = nullptr;
		static PFN_vkCmdEndDebugUtilsLabelEXT       s_vkCmdEndDebugUtilsLabel = nullptr;
		static bool s_bCanUseDebugUtils = false;
		static std::mutex s_mutex;

		bool ExtDebugUtilsCheckInstanceExtensions(InstanceProperties* pDP)
		{
			s_bCanUseDebugUtils = pDP->AddInstanceExtensionName("VK_EXT_debug_utils");
			return s_bCanUseDebugUtils;
		}
		bool ExtDebugUtilsCheckInstanceExtensions(bool is_debug_utils)
		{
			s_bCanUseDebugUtils = is_debug_utils;// pDP->AddInstanceExtensionName("VK_EXT_debug_utils");
			return s_bCanUseDebugUtils;
		}

		//
		//
		void ExtDebugUtilsGetProcAddresses(VkDevice device)
		{
			if (s_bCanUseDebugUtils)
			{
				s_vkSetDebugUtilsObjectName = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(device, "vkSetDebugUtilsObjectNameEXT");
				s_vkCmdBeginDebugUtilsLabel = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetDeviceProcAddr(device, "vkCmdBeginDebugUtilsLabelEXT");
				s_vkCmdEndDebugUtilsLabel = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetDeviceProcAddr(device, "vkCmdEndDebugUtilsLabelEXT");
			}
		}

		void SetResourceName(VkDevice device, VkObjectType objectType, uint64_t handle, const char* name)
		{
			if (s_vkSetDebugUtilsObjectName && handle && name)
			{
				std::unique_lock<std::mutex> lock(s_mutex);

				VkDebugUtilsObjectNameInfoEXT nameInfo = {};
				nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
				nameInfo.objectType = objectType;
				nameInfo.objectHandle = handle;
				nameInfo.pObjectName = name;
				s_vkSetDebugUtilsObjectName(device, &nameInfo);
			}
		}

		void SetPerfMarkerBegin(VkCommandBuffer cmd_buf, const char* name)
		{
			if (s_vkCmdBeginDebugUtilsLabel)
			{
				VkDebugUtilsLabelEXT label = {};
				label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
				label.pLabelName = name;
				const float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
				memcpy(label.color, color, sizeof(color));
				s_vkCmdBeginDebugUtilsLabel(cmd_buf, &label);
			}
		}

		void SetPerfMarkerEnd(VkCommandBuffer cmd_buf)
		{
			if (s_vkCmdEndDebugUtilsLabel)
			{
				s_vkCmdEndDebugUtilsLabel(cmd_buf);
			}
		}
		// vrs
		static VkPhysicalDeviceFragmentShadingRateFeaturesKHR VRSQueryFeatures = {};

		void ExtVRSCheckExtensions(DeviceProperties* pDP, bool& Tier1Supported, bool& Tier2Supported)
		{
#ifdef VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME
			if (pDP->AddDeviceExtensionName(VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME))
			{
				pDP->AddDeviceExtensionName(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);

				VRSQueryFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
				VkPhysicalDeviceFeatures2 features = {};
				features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
				features.pNext = &VRSQueryFeatures;
				vkGetPhysicalDeviceFeatures2(pDP->GetPhysicalDevice(), &features);

				Tier1Supported = VRSQueryFeatures.pipelineFragmentShadingRate;
				Tier2Supported = VRSQueryFeatures.attachmentFragmentShadingRate && VRSQueryFeatures.primitiveFragmentShadingRate;
			}
#endif
		}


		static VkPhysicalDeviceFloat16Int8FeaturesKHR FP16Features = {};
		static VkPhysicalDevice16BitStorageFeatures Storage16BitFeatures = {};

		bool ExtFp16CheckExtensions(DeviceProperties* pDP)
		{
			std::vector<const char*> required_extension_names = {
#ifdef VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME
				VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME
#endif
			};
			if (required_extension_names.empty())
			{
				return false;
			}
			bool bFp16Enabled = true;
			for (auto& ext : required_extension_names)
			{
				if (pDP->AddDeviceExtensionName(ext) == false)
				{
					Trace(format("FP16 disabled, missing extension: %s\n", ext));
					bFp16Enabled = false;
				}

			}

			if (bFp16Enabled)
			{
				// Query 16 bit storage
				Storage16BitFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES;

				VkPhysicalDeviceFeatures2 features = {};
				features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
				features.pNext = &Storage16BitFeatures;
				vkGetPhysicalDeviceFeatures2(pDP->GetPhysicalDevice(), &features);

				bFp16Enabled = bFp16Enabled && Storage16BitFeatures.storageBuffer16BitAccess;

				// Query 16 bit ops
				FP16Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR;

				features.pNext = &FP16Features;
				vkGetPhysicalDeviceFeatures2(pDP->GetPhysicalDevice(), &features);

				bFp16Enabled = bFp16Enabled && FP16Features.shaderFloat16;
			}

			if (bFp16Enabled)
			{
				// Query 16 bit storage
				Storage16BitFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES;
				Storage16BitFeatures.pNext = pDP->GetNext();

				// Query 16 bit ops
				FP16Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR;
				FP16Features.pNext = &Storage16BitFeatures;

				pDP->SetNewNext(&FP16Features);
			}

			return bFp16Enabled;
		}
		// todo hdr


		static VkSurfaceFullScreenExclusiveWin32InfoEXT s_SurfaceFullScreenExclusiveWin32InfoEXT;
		static VkSurfaceFullScreenExclusiveInfoEXT s_SurfaceFullScreenExclusiveInfoEXT;
		static VkPhysicalDeviceSurfaceInfo2KHR s_PhysicalDeviceSurfaceInfo2KHR;
		static VkDisplayNativeHdrSurfaceCapabilitiesAMD s_DisplayNativeHdrSurfaceCapabilitiesAMD;

		static VkHdrMetadataEXT s_HdrMetadataEXT;
		static VkSurfaceCapabilities2KHR s_SurfaceCapabilities2KHR;
		static VkSwapchainDisplayNativeHdrCreateInfoAMD s_SwapchainDisplayNativeHdrCreateInfoAMD;

		static VkDevice s_device;
		static VkSurfaceKHR s_surface;
		static VkPhysicalDevice s_physicalDevice;
		static HWND s_hWnd = NULL;

		static std::unordered_map<DisplayMode, VkSurfaceFormatKHR> availableDisplayModeSurfaceformats;

		bool s_windowsHdrtoggle = false;

		void SetHDRStructs(VkSurfaceKHR surface)
		{
			s_PhysicalDeviceSurfaceInfo2KHR.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
			s_PhysicalDeviceSurfaceInfo2KHR.pNext = nullptr;
			s_PhysicalDeviceSurfaceInfo2KHR.surface = surface;

			s_HdrMetadataEXT.sType = VK_STRUCTURE_TYPE_HDR_METADATA_EXT;
			s_HdrMetadataEXT.pNext = nullptr;
		}

		void SetFSEStructures(HWND hWnd, PresentationMode fullscreenMode)
		{
			// Required final chaining order
			// VkPhysicalDeviceSurfaceInfo2KHR -> VkSurfaceFullScreenExclusiveInfoEXT -> VkSurfaceFullScreenExclusiveWin32InfoEXT

			s_PhysicalDeviceSurfaceInfo2KHR.pNext = &s_SurfaceFullScreenExclusiveInfoEXT;
			s_SurfaceFullScreenExclusiveInfoEXT.pNext = &s_SurfaceFullScreenExclusiveWin32InfoEXT;

			s_SurfaceFullScreenExclusiveInfoEXT.sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_INFO_EXT;
			if (fullscreenMode == PRESENTATIONMODE_WINDOWED)
				s_SurfaceFullScreenExclusiveInfoEXT.fullScreenExclusive = VK_FULL_SCREEN_EXCLUSIVE_DISALLOWED_EXT;
			else if (fullscreenMode == PRESENTATIONMODE_BORDERLESS_FULLSCREEN)
				s_SurfaceFullScreenExclusiveInfoEXT.fullScreenExclusive = VK_FULL_SCREEN_EXCLUSIVE_ALLOWED_EXT;
			else if (fullscreenMode == PRESENTATIONMODE_EXCLUSIVE_FULLSCREEN)
				s_SurfaceFullScreenExclusiveInfoEXT.fullScreenExclusive = VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT;

			s_SurfaceFullScreenExclusiveWin32InfoEXT.sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_WIN32_INFO_EXT;
			s_SurfaceFullScreenExclusiveWin32InfoEXT.pNext = nullptr;
			s_SurfaceFullScreenExclusiveWin32InfoEXT.hmonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		}

		void SetFreesyncHDRStructures()
		{
			// Required final chaning order
			// VkSurfaceCapabilities2KHR -> VkDisplayNativeHdrSurfaceCapabilitiesAMD -> VkHdrMetadataEXT

			s_SurfaceCapabilities2KHR.sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR;
			s_SurfaceCapabilities2KHR.pNext = &s_DisplayNativeHdrSurfaceCapabilitiesAMD;

			// This will cause validation error and complain as invalid structure attached.
			// But we are hijacking hdr metadata struct and attaching it here to fill it with monitor primaries
			// When getting surface capabilities
			s_DisplayNativeHdrSurfaceCapabilitiesAMD.sType = VK_STRUCTURE_TYPE_DISPLAY_NATIVE_HDR_SURFACE_CAPABILITIES_AMD;
			s_DisplayNativeHdrSurfaceCapabilitiesAMD.pNext = &s_HdrMetadataEXT;
		}

		void SetSwapchainFreesyncHDRStructures(bool enableLocalDimming)
		{
			// Required final chaning order
			// VkSurfaceFullScreenExclusiveInfoEXT -> VkSurfaceFullScreenExclusiveWin32InfoEXT -> VkSwapchainDisplayNativeHdrCreateInfoAMD
			s_SurfaceFullScreenExclusiveWin32InfoEXT.pNext = &s_SwapchainDisplayNativeHdrCreateInfoAMD;

			s_SwapchainDisplayNativeHdrCreateInfoAMD.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_DISPLAY_NATIVE_HDR_CREATE_INFO_AMD;
			s_SwapchainDisplayNativeHdrCreateInfoAMD.pNext = nullptr;
			// Enable local dimming if supported
			// Must requry FS HDR display capabilities
			// After value is set through swapchain creation when attached to swapchain pnext.
			s_SwapchainDisplayNativeHdrCreateInfoAMD.localDimmingEnable = s_DisplayNativeHdrSurfaceCapabilitiesAMD.localDimmingSupport && enableLocalDimming;
		}

		VkSurfaceFullScreenExclusiveInfoEXT* GetVkSurfaceFullScreenExclusiveInfoEXT()
		{
			return &s_SurfaceFullScreenExclusiveInfoEXT;
		}

		void GetSurfaceFormats(uint32_t* pFormatCount, std::vector<VkSurfaceFormat2KHR>* surfFormats)
		{
			// Get the list of formats
			//
			VkResult res = g_vkGetPhysicalDeviceSurfaceFormats2KHR(s_physicalDevice, &s_PhysicalDeviceSurfaceInfo2KHR, pFormatCount, NULL);
			assert(res == VK_SUCCESS);

			uint32_t formatCount = *pFormatCount;
			surfFormats->resize(formatCount);
			for (UINT i = 0; i < formatCount; ++i)
			{
				(*surfFormats)[i].sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
			}

			res = g_vkGetPhysicalDeviceSurfaceFormats2KHR(s_physicalDevice, &s_PhysicalDeviceSurfaceInfo2KHR, &formatCount, (*surfFormats).data());
			assert(res == VK_SUCCESS);
		}

		//--------------------------------------------------------------------------------------
		//
		// fsHdrInit, if it returns false the fsHdr extensions are not present
		//
		//--------------------------------------------------------------------------------------
		bool fsHdrInit(VkDevice device, VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, HWND hWnd)
		{
			s_hWnd = hWnd;
			s_device = device;
			s_surface = surface;
			s_physicalDevice = physicalDevice;

			return ExtAreFreeSyncHDRExtensionsPresent();
		}

		//--------------------------------------------------------------------------------------
		//
		// fsHdrEnumerateDisplayModes, enumerates availabe modes
		//
		//--------------------------------------------------------------------------------------
		bool fsHdrEnumerateDisplayModes(std::vector<DisplayMode>* pModes, bool includeFreesyncHDR, PresentationMode fullscreenMode, bool enableLocalDimming)
		{
			pModes->clear();
			availableDisplayModeSurfaceformats.clear();

			VkSurfaceFormatKHR surfaceFormat;
			surfaceFormat.format = VK_FORMAT_R8G8B8A8_UNORM;
			surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

			pModes->push_back(DISPLAYMODE_SDR);
			availableDisplayModeSurfaceformats[DISPLAYMODE_SDR] = surfaceFormat;

			if (ExtAreHDRExtensionsPresent())
			{
				SetHDRStructs(s_surface);

				uint32_t formatCount;
				std::vector<VkSurfaceFormat2KHR> surfFormats;
				GetSurfaceFormats(&formatCount, &surfFormats);

				for (uint32_t i = 0; i < formatCount; i++)
				{
					if ((surfFormats[i].surfaceFormat.format == VK_FORMAT_A2R10G10B10_UNORM_PACK32 &&
						surfFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_HDR10_ST2084_EXT)
						||
						(surfFormats[i].surfaceFormat.format == VK_FORMAT_A2B10G10R10_UNORM_PACK32 &&
							surfFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_HDR10_ST2084_EXT))
					{
						// If surface formats have HDR10 format even before fullscreen surface is attached, it can only mean windows hdr toggle is on
						s_windowsHdrtoggle = true;
						break;
					}
				}
			}
			else
			{
				s_PhysicalDeviceSurfaceInfo2KHR.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
				s_PhysicalDeviceSurfaceInfo2KHR.pNext = nullptr;
				s_PhysicalDeviceSurfaceInfo2KHR.surface = s_surface;
			}

			if (ExtAreFSEExtensionsPresent())
			{
				SetFSEStructures(s_hWnd, fullscreenMode);
			}

			if (includeFreesyncHDR && ExtAreFreeSyncHDRExtensionsPresent())
			{
				SetFreesyncHDRStructures();

				// Calling get capabilities here to query for local dimming support
				fsHdrGetPhysicalDeviceSurfaceCapabilities2KHR(s_physicalDevice, s_surface, NULL);

				SetSwapchainFreesyncHDRStructures(enableLocalDimming);
			}

			uint32_t formatCount;
			std::vector<VkSurfaceFormat2KHR> surfFormats;
			GetSurfaceFormats(&formatCount, &surfFormats);

			for (uint32_t i = 0; i < formatCount; i++)
			{
				if (surfFormats[i].surfaceFormat.format == VK_FORMAT_A2R10G10B10_UNORM_PACK32 &&
					surfFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_DISPLAY_NATIVE_AMD)
				{
					pModes->push_back(DISPLAYMODE_FSHDR_Gamma22);
					availableDisplayModeSurfaceformats[DISPLAYMODE_FSHDR_Gamma22] = surfFormats[i].surfaceFormat;
				}
				else if (surfFormats[i].surfaceFormat.format == VK_FORMAT_R16G16B16A16_SFLOAT &&
					surfFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_DISPLAY_NATIVE_AMD)
				{
					pModes->push_back(DISPLAYMODE_FSHDR_SCRGB);
					availableDisplayModeSurfaceformats[DISPLAYMODE_FSHDR_SCRGB] = surfFormats[i].surfaceFormat;
				}
			}

			for (uint32_t i = 0; i < formatCount; i++)
			{
				if ((surfFormats[i].surfaceFormat.format == VK_FORMAT_A2R10G10B10_UNORM_PACK32 &&
					surfFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_HDR10_ST2084_EXT)
					||
					(surfFormats[i].surfaceFormat.format == VK_FORMAT_A2B10G10R10_UNORM_PACK32 &&
						surfFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_HDR10_ST2084_EXT))
				{
					if (availableDisplayModeSurfaceformats.find(DISPLAYMODE_HDR10_2084) == availableDisplayModeSurfaceformats.end())
					{
						pModes->push_back(DISPLAYMODE_HDR10_2084);
						availableDisplayModeSurfaceformats[DISPLAYMODE_HDR10_2084] = surfFormats[i].surfaceFormat;
					}
				}
				else if (surfFormats[i].surfaceFormat.format == VK_FORMAT_R16G16B16A16_SFLOAT &&
					surfFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT)
				{
					pModes->push_back(DISPLAYMODE_HDR10_SCRGB);
					availableDisplayModeSurfaceformats[DISPLAYMODE_HDR10_SCRGB] = surfFormats[i].surfaceFormat;
				}
			}

			return true;
		}

		//--------------------------------------------------------------------------------------
		//
		// fsHdrGetDisplayModeString
		//
		//--------------------------------------------------------------------------------------
		const char* fsHdrGetDisplayModeString(DisplayMode displayMode)
		{
			// note that these string must match the order of the DisplayModes enum
			const char* DisplayModesStrings[]
			{
				"SDR",
				"FSHDR_Gamma22",
				"FSHDR_SCRGB",
				"HDR10_2084",
				"HDR10_SCRGB"
			};

			return DisplayModesStrings[displayMode];
		}

		//--------------------------------------------------------------------------------------
		//
		// fsHdrGetFormat
		//
		//--------------------------------------------------------------------------------------
		VkSurfaceFormatKHR fsHdrGetFormat(DisplayMode displayMode)
		{
			VkSurfaceFormatKHR surfaceFormat;

			surfaceFormat = availableDisplayModeSurfaceformats[displayMode];

			return surfaceFormat;
		}

		//--------------------------------------------------------------------------------------
		//
		// fsHdrSetDisplayMode
		//
		//--------------------------------------------------------------------------------------
		bool fsHdrSetDisplayMode(DisplayMode displayMode, VkSwapchainKHR swapChain)
		{
			if (!ExtAreHDRExtensionsPresent())
				return false;

			switch (displayMode)
			{
			case DISPLAYMODE_SDR:
				// rec 709 primaries
				s_HdrMetadataEXT.displayPrimaryRed.x = 0.64f;
				s_HdrMetadataEXT.displayPrimaryRed.y = 0.33f;
				s_HdrMetadataEXT.displayPrimaryGreen.x = 0.30f;
				s_HdrMetadataEXT.displayPrimaryGreen.y = 0.60f;
				s_HdrMetadataEXT.displayPrimaryBlue.x = 0.15f;
				s_HdrMetadataEXT.displayPrimaryBlue.y = 0.06f;
				s_HdrMetadataEXT.whitePoint.x = 0.3127f;
				s_HdrMetadataEXT.whitePoint.y = 0.3290f;
				s_HdrMetadataEXT.minLuminance = 0.0f;
				s_HdrMetadataEXT.maxLuminance = 300.0f;
				break;
			case DISPLAYMODE_FSHDR_Gamma22:
				// use the values that we queried using VkGetPhysicalDeviceSurfaceCapabilities2KHR
				break;
			case DISPLAYMODE_FSHDR_SCRGB:
				// use the values that we queried using VkGetPhysicalDeviceSurfaceCapabilities2KHR
				break;
			case DISPLAYMODE_HDR10_2084:
				// rec 2020 primaries
				s_HdrMetadataEXT.displayPrimaryRed.x = 0.708f;
				s_HdrMetadataEXT.displayPrimaryRed.y = 0.292f;
				s_HdrMetadataEXT.displayPrimaryGreen.x = 0.170f;
				s_HdrMetadataEXT.displayPrimaryGreen.y = 0.797f;
				s_HdrMetadataEXT.displayPrimaryBlue.x = 0.131f;
				s_HdrMetadataEXT.displayPrimaryBlue.y = 0.046f;
				s_HdrMetadataEXT.whitePoint.x = 0.3127f;
				s_HdrMetadataEXT.whitePoint.y = 0.3290f;
				s_HdrMetadataEXT.minLuminance = 0.0f;
				s_HdrMetadataEXT.maxLuminance = 1000.0f; // This will cause tonemapping to happen on display end as long as it's greater than display's actual queried max luminance. The look will change and it will be display dependent!
				s_HdrMetadataEXT.maxContentLightLevel = 1000.0f;
				s_HdrMetadataEXT.maxFrameAverageLightLevel = 400.0f; // max and average content light level data will be used to do tonemapping on display
				break;
			case DISPLAYMODE_HDR10_SCRGB:
				// rec 709 primaries
				s_HdrMetadataEXT.displayPrimaryRed.x = 0.64f;
				s_HdrMetadataEXT.displayPrimaryRed.y = 0.33f;
				s_HdrMetadataEXT.displayPrimaryGreen.x = 0.30f;
				s_HdrMetadataEXT.displayPrimaryGreen.y = 0.60f;
				s_HdrMetadataEXT.displayPrimaryBlue.x = 0.15f;
				s_HdrMetadataEXT.displayPrimaryBlue.y = 0.06f;
				s_HdrMetadataEXT.whitePoint.x = 0.3127f;
				s_HdrMetadataEXT.whitePoint.y = 0.3290f;
				s_HdrMetadataEXT.minLuminance = 0.0f;
				s_HdrMetadataEXT.maxLuminance = 1000.0f; // This will cause tonemapping to happen on display end as long as it's greater than display's actual queried max luminance. The look will change and it will be display dependent!
				s_HdrMetadataEXT.maxContentLightLevel = 1000.0f;
				s_HdrMetadataEXT.maxFrameAverageLightLevel = 400.0f; // max and average content light level data will be used to do tonemapping on display
				break;
			}

			g_vkSetHdrMetadataEXT(s_device, 1, &swapChain, &s_HdrMetadataEXT);

			return true;
		}

		//--------------------------------------------------------------------------------------
		//
		// fsHdrSetLocalDimmingMode
		//
		//--------------------------------------------------------------------------------------
		void fsHdrSetLocalDimmingMode(VkSwapchainKHR swapchain, VkBool32 localDimmingEnable)
		{
			g_vkSetLocalDimmingAMD(s_device, swapchain, localDimmingEnable);
			VkResult res = g_vkGetPhysicalDeviceSurfaceCapabilities2KHR(s_physicalDevice, &s_PhysicalDeviceSurfaceInfo2KHR, &s_SurfaceCapabilities2KHR);
			assert(res == VK_SUCCESS);
			g_vkSetHdrMetadataEXT(s_device, 1, &swapchain, &s_HdrMetadataEXT);
		}

		//--------------------------------------------------------------------------------------
		//
		// fsHdrSetFullscreenState
		//
		//--------------------------------------------------------------------------------------
		void fsHdrSetFullscreenState(bool fullscreen, VkSwapchainKHR swapchain)
		{
			if (fullscreen)
			{
				VkResult res = g_vkAcquireFullScreenExclusiveModeEXT(s_device, swapchain);
				assert(res == VK_SUCCESS);
			}
			else
			{
				VkResult res = g_vkReleaseFullScreenExclusiveModeEXT(s_device, swapchain);
				assert(res == VK_SUCCESS);
			}
		}

		//--------------------------------------------------------------------------------------
		//
		// fsHdrGetDisplayInfo
		//
		//--------------------------------------------------------------------------------------
		const VkHdrMetadataEXT* fsHdrGetDisplayInfo()
		{
			return &s_HdrMetadataEXT;
		}

		void fsHdrGetPhysicalDeviceSurfaceCapabilities2KHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* pSurfCapabilities)
		{
			assert(surface == s_surface);
			assert(physicalDevice == s_physicalDevice);

			VkResult res = g_vkGetPhysicalDeviceSurfaceCapabilities2KHR(s_physicalDevice, &s_PhysicalDeviceSurfaceInfo2KHR, &s_SurfaceCapabilities2KHR);
			assert(res == VK_SUCCESS);

			if (pSurfCapabilities)
				*pSurfCapabilities = s_SurfaceCapabilities2KHR.surfaceCapabilities;
		}
#else

		void SetResourceName(VkDevice device, VkObjectType objectType, uint64_t handle, const char* name)
		{
		}

		void SetPerfMarkerBegin(VkCommandBuffer cmd_buf, const char* name)
		{

		}

		void SetPerfMarkerEnd(VkCommandBuffer cmd_buf)
		{}
#endif

		// We are turning off this HDR path for now
		// Driver update to get this path working is coming soon.
#define WINDOW_HDR_PATH 0
		const bool CheckIfWindowModeHdrOn()
		{
#if WINDOW_HDR_PATH
			return s_windowsHdrtoggle;
#else
			return false;
#endif
		}

#endif // 1

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
			_instance = gpu->_instance;
			enum_lp();
			gpu->init();
			//auto& gis = *((std::vector<VkPhysicalDeviceProperties>*)gpu->_gpu_infos);
			auto gpsn = gpu->gpusname.size();
			if (gpsn)
			{
				physical_layer_properties.resize(gpsn);
				auto& gpn = gpu->gpusname;
				//printf("所有显卡：");
				int idx = 0, ic = 0;
				int n = gpsn;
				//printf("%d\n", n);
				devs.resize(n);
				for (auto& [k, v] : gpn) {
					//printf("%s\n", k.c_str());
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
			//v.main(glfw_, surface, instance, dev->device, sc, psc);
			//return 0;

		}

		int init_dev(const std::string n, int idx, bool one_compute)
		{
			//auto gps = _gpu->get_physicals();
			//auto& gis = *((std::vector<VkPhysicalDeviceProperties>*)_gpu->_gpu_infos);
			auto& gpn = _gpu->gpusname;

			if (!devs[idx].device)
			{
				auto& it = gpn[n];
				devs[idx].init(_instance, one_compute);
				devs[idx]._limits = new VkPhysicalDeviceLimits(it.pdp->limits);
				get_dlp(it.pd, physical_layer_properties[idx]);
			}
			return idx;
		}
		VkSurfaceKHR new_surface(void* platformWindow_, void* glfw_)
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
				//LOGE("vkCreateAndroidSurfaceKHR: err:%d,s %p,w %p, p %p,flags:%d\n", (int)err, surface, window, (int)surfaceCreateInfo.flags);
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
			static uint32_t _unorm8[4] = { VK_FORMAT_R8_UNORM, VK_FORMAT_R8G8_UNORM, VK_FORMAT_R8G8B8_UNORM, VK_FORMAT_R8G8B8A8_UNORM };
			static uint32_t _uint32[4] = { VK_FORMAT_R32_UINT, VK_FORMAT_R32G32_UINT, VK_FORMAT_R32G32B32_UINT, VK_FORMAT_R32G32B32A32_UINT };
			static uint32_t _sint32[4] = { VK_FORMAT_R32_SINT, VK_FORMAT_R32G32_SINT, VK_FORMAT_R32G32B32_SINT, VK_FORMAT_R32G32B32A32_SINT };
			static uint32_t _uint64[4] = { VK_FORMAT_R64_UINT, VK_FORMAT_R64G64_UINT, VK_FORMAT_R64G64B64_UINT, VK_FORMAT_R64G64B64A64_UINT };
			static uint32_t _sint64[4] = { VK_FORMAT_R64_SINT, VK_FORMAT_R64G64_SINT, VK_FORMAT_R64G64B64_SINT, VK_FORMAT_R64G64B64A64_SINT };
			static uint32_t _float32[4] = { VK_FORMAT_R32_SFLOAT, VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32B32A32_SFLOAT };
			static uint32_t _float64[4] = { VK_FORMAT_R64_SFLOAT, VK_FORMAT_R64G64_SFLOAT, VK_FORMAT_R64G64B64_SFLOAT, VK_FORMAT_R64G64B64A64_SFLOAT };

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
					if ((varName == "col" || (varName.find("Color") != std::string::npos) || (varName == "color"))
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
				auto typeName = compiler.get_name(res.base_type_id);
				int sets = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
				int binding = compiler.get_decoration(res.id, spv::DecorationBinding);
				uint32_t descriptor_type = spirv2type(type.basetype);
				uint32_t dt = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
				BindInfo bnt = {};
				padd_binding(sets, binding, dt, 1, stageFlags, bindings, &bnt);

				if (varName.empty())
					varName = typeName;
				if (varName.empty())
				{
					varName = "lsb_" + std::to_string(sets) + "_" + std::to_string(binding);
				}
				paramsMap[varName] = bnt;
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
				descriptor_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				if ((typeName.find("u_") != std::string::npos) || typeName.find("dynamic") != std::string::npos || typeName.find("DYNAMIC") != std::string::npos)
				{
					descriptor_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
				}

				BindInfo bnt = {};
				padd_binding(sets, binding, descriptor_type, 1, stageFlags, bindings, &bnt);

				if (varName.empty())
					varName = typeName;
				if (varName.empty())
				{
					varName = "lsb_" + std::to_string(sets) + "_" + std::to_string(binding);
				}
				auto nt = paramsMap[varName];
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
				uint32_t descriptor_type = spirv2type(type.basetype); //VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

				BindInfo bnt = {};
				padd_binding(sets, binding, descriptor_type, 1, stageFlags, bindings, &bnt);

				if (varName.empty())
				{
					varName = "lsb_" + std::to_string(sets) + "_" + std::to_string(binding);
				}
				auto nt = paramsMap[varName];
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
				auto varNamefb = compiler.get_fallback_name(res.id);
				if (varName.empty())
					varName = varNamefb;
				int sets = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
				int binding = compiler.get_decoration(res.id, spv::DecorationBinding);

				uint32_t descriptor_type = spirv2type(type.basetype); //VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

				BindInfo bnt = {};
				padd_binding(sets, binding, descriptor_type, 1, stageFlags, bindings, &bnt);

				if (varName.empty())
				{
					varName = "lsb_" + std::to_string(sets) + "_" + std::to_string(binding);
				}
				auto nt = paramsMap[varName];
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

				uint32_t descriptor_type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				if ((typeName.find("u_") != std::string::npos) || typeName.find("dynamic") != std::string::npos || typeName.find("DYNAMIC") != std::string::npos)
				{
					descriptor_type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
				}

				BindInfo bnt = {};
				padd_binding(sets, binding, descriptor_type, 1, stageFlags, bindings, &bnt);

				if (varName.empty())
					varName = typeName;
				if (varName.empty())
				{
					varName = "lsb_" + std::to_string(sets) + "_" + std::to_string(binding);
				}
				auto nt = paramsMap[varName];
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
					if (uniformBufferStructSize > it->second.bufferSize)
					{
						it->second.bufferSize = uniformBufferStructSize;
					}
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
		spv_res_info* get_shader_info(const void* data, size_t size, pipe_create_info* ot)
		{
			// 反编译Shader获取相关信息
			spirv_cross::Compiler compiler((uint32_t*)data, size / sizeof(uint32_t));
			spirv_cross::ShaderResources resources = compiler.get_shader_resources();
			auto spvinfo = new spv_res_info();
			auto eps = compiler.get_entry_points_and_stages();
			auto m = compiler.get_execution_mode_bitset();
			for (auto& it : eps)
			{
				ot->eps.push_back({ it.name, get_stages(it.execution_model) });
			}
			if (eps.size())
			{
				auto ep0 = ot->eps[0];
				ot->name = ep0.name;
				//VkShaderStageFlagBits stage = (VkShaderStageFlagBits)ep0.stage;
				ot->stage = ep0.stage;
				// 解析信息
				Process(compiler, resources, ep0.stage, spvinfo);
			}
			return spvinfo;
		}
		int load_pipe(void* dev_, pipe_create_info** info, uint16_t count, std::vector<VkPipelineShaderStageCreateInfo*>* out)
		{
			int ret = 0;
			auto dev = (dvk_device*)dev_;
			assert(dev && out);
			//dev->new_set_pool();
			for (size_t i = 0; i < count; i++)
			{
				auto& it = info[i];

				it->spv_info = get_shader_info(it->data, it->size, it);
				if (it->stage == 0)
				{
					it->stage = get_stage(it->fn, it->is_comp);
				}
				auto p = vkc::load_shader(dev, it->data, it->size, it->stage, it->name.c_str());
				if (p.module)
				{
					it->_module = (uint64_t)p.module;
					auto p1 = new VkPipelineShaderStageCreateInfo();
					*p1 = p;
					out->push_back(p1);
					ret++;
				}
			}
			return ret;
		}
		static uint32_t get_stages(spv::ExecutionModel s)
		{
			uint32_t ret = 0;
			switch (s)
			{
			case spv::ExecutionModelVertex:
				ret = VK_SHADER_STAGE_VERTEX_BIT;
				break;
			case spv::ExecutionModelTessellationControl:
				ret = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
				break;
			case spv::ExecutionModelTessellationEvaluation:
				ret = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
				break;
			case spv::ExecutionModelGeometry:
				ret = VK_SHADER_STAGE_GEOMETRY_BIT;
				break;
			case spv::ExecutionModelFragment:
				ret = VK_SHADER_STAGE_FRAGMENT_BIT;
				break;
			case spv::ExecutionModelGLCompute:
				ret = VK_SHADER_STAGE_COMPUTE_BIT;
				break;
			case spv::ExecutionModelKernel:
				// todo kernel
				break;
			case spv::ExecutionModelTaskNV:
				ret = VK_SHADER_STAGE_TASK_BIT_NV;
				break;
			case spv::ExecutionModelMeshNV:
				ret = VK_SHADER_STAGE_MISS_BIT_KHR;
				break;
			case spv::ExecutionModelRayGenerationNV:
				ret = VK_SHADER_STAGE_RAYGEN_BIT_NV;
				break;
			case spv::ExecutionModelIntersectionNV:
				ret = VK_SHADER_STAGE_INTERSECTION_BIT_NV;
				break;
			case spv::ExecutionModelAnyHitNV:
				ret = VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
				break;
			case spv::ExecutionModelClosestHitNV:
				ret = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
				break;
			case spv::ExecutionModelMissNV:
				ret = VK_SHADER_STAGE_MISS_BIT_NV;
				break;
			case spv::ExecutionModelCallableNV:
				ret = VK_SHADER_STAGE_CALLABLE_BIT_NV;
				break;
			case spv::ExecutionModelMax:
				break;
			default:
				break;
			}
			return ret;
		}
		static uint32_t get_stage(const std::string& str, bool is_comp)
		{
			uint32_t ret = 0;
			if (str.find(".comp") != std::string::npos || is_comp)
			{
				ret = VK_SHADER_STAGE_COMPUTE_BIT;
			}
			else if (str.find(".vert") != std::string::npos)
			{
				ret = VK_SHADER_STAGE_VERTEX_BIT;
			}
			else if (str.find(".frag") != std::string::npos)
			{
				ret = VK_SHADER_STAGE_FRAGMENT_BIT;
			}
			else if (str.find(".geom") != std::string::npos)
			{
				ret = VK_SHADER_STAGE_GEOMETRY_BIT;
			}
			else if (str.find(".tese") != std::string::npos)
			{
				ret = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			}
			else if (str.find(".tesc") != std::string::npos)
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
			_instance = vkc::CreateInstance(&ext);
			_gpu->_instance = _instance;
			pvk->init(_gpu);
		}
		ctx = pvk;
	}
	vk_render_cx::~vk_render_cx()
	{
		ctx = nullptr; _gpu = nullptr;
	}

	fbo_cx* vk_render_cx::new_fbo_swapchain(int width, int height, int* count, VkSurfaceKHR surface, unsigned int clear_color, dvk_device* dev, void* pass)
	{
		//fbo_cx* p = dc.ac<fbo_cx>(width, height, clear_color);
		fbo_cx* p = new fbo_cx(width, height, clear_color);
		if (p)
		{
			//dvk_swapchain* swapchain = dc.ac<dvk_swapchain>(dev, surface);
			dvk_swapchain* swapchain = new dvk_swapchain(dev, surface);
			uint32_t uw = width, uh = height;
			swapchain->create(&uw, &uh, false);
			p->init(dev, *count, pass, swapchain);
		}
		return p;
	}
	fbo_cx* vk_render_cx::reset_fbo_swapchain(fbo_cx* p, int width, int height, int* count, VkSurfaceKHR surface, unsigned int clear_color, dvk_device* dev, void* pass)
	{
		fbo_cx* rt = 0;
		if (!p)
		{
			p = new fbo_cx(width, height, clear_color);
			rt = p;
		}
		else
		{
			p->_width = (width);
			p->_height = (height);
			p->_clear_color = (clear_color);
		}
		{
			if (p->_swapcain)
			{
				delop(p->_swapcain);
			}
			//dvk_swapchain* swapchain = dc.ac<dvk_swapchain>(dev, surface);
			dvk_swapchain* swapchain = new dvk_swapchain(dev, surface);
			uint32_t uw = width, uh = height;
			swapchain->create(&uw, &uh, false);
			p->init(dev, *count, pass, swapchain);
		}
		return rt;
	}
	fbo_cx* vk_render_cx::new_fbo_image(int width, int height, int count, unsigned int clear_color, dvk_device* dev, void* pass)
	{
		//fbo_cx* p = dc.ac<fbo_cx>(width, height, clear_color);
		fbo_cx* p = new fbo_cx(width, height, clear_color);
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
		auto p = &ctx->devs[idx];
		p->instanceExtensions = &ext;
		return p;
	}

	dvk_device* vk_render_cx::get_dev(const std::string& name)
	{
		int idx = 0;
		auto n = _gpu->get_idx(name.c_str(), idx);
		//ctx->init_dev(n, idx, true);
		auto p = &ctx->devs[idx];
		p->instanceExtensions = &ext;
		return p;
	}
	void vk_render_cx::init_dev(const std::string& name)
	{
		int idx = 0;
		auto n = _gpu->get_idx(name.c_str(), idx);
		ctx->init_dev(n, idx, true);
	}

	int vk_render_cx::load_pipe(dvk_device* dev_, pipe_create_info** info, uint16_t count, pipe_info_s* out)
	{
		auto p = (vk_base_info*)ctx;
		auto dev = dev_;
		assert(dev_);
		out->dev = dev;
		int r = p->load_pipe(dev_, info, count, &out->v);
		return r;
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
			for (auto& it : p->v)
			{
				dev->free_shader(it->module);
				delop(it);
			}
		}
	}

	VkSurfaceKHR vk_render_cx::new_surface(void* d)
	{
		return ctx->new_surface(d, 0);
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

			auto kr = (vkBeginCommandBuffer(commandBuffer, pBeginInfo));
			assert(kr == 0);
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
		std::vector<dvk_cmd> dcmds;
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
			clearValues[0].color = { uc[0] / 255.0f, uc[1] / 255.0f, uc[2] / 255.0f, uc[3] / 255.0f };
			clearValues[1].depthStencil = { depth, Stencil };
		}

		void setClearValues(float* color, float depth = 1.0f, uint32_t Stencil = 0)
		{
			clearValues[0].color = { {color[0], color[1], color[2], color[3]} };
			clearValues[1].depthStencil = { depth, Stencil };
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
			samplerinfo.maxAnisotropy = 1;
			samplerinfo.minLod = 0.0f;
			samplerinfo.maxLod = 1.0f;
			samplerinfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			if (!sampler)
				vkc::createSampler(_dev, &sampler, &samplerinfo);

			//LOGE("func=%s,line=%d,file=%s\n", __FUNCTION__, __LINE__, __FILE__);
			// Create num frame buffers
			resetFramebuffer(width, height, swapchainbuffers);
			//LOGE("func=%s,line=%d,file=%s\n", __FUNCTION__, __LINE__, __FILE__);
		}

		//窗口大小改变时需要重新创建image,如果是交换链则传swapchainbuffers
		void resetFramebuffer(int width, int height, std::vector<dvk_swapchain::scb_t>* swapchainbuffers)
		{
			/*if (width & 1)
				width++;
			if (height & 1)
				height++;*/
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
						framebuffers[i].color._view = it._view;
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
				attachments[0] = it.color._view;
				attachments[1] = it.depth_stencil._view;
				VkFramebufferCreateInfo fbufCreateInfo = {};
				fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				fbufCreateInfo.renderPass = renderPass;
				fbufCreateInfo.attachmentCount = 2;
				fbufCreateInfo.pAttachments = attachments;
				fbufCreateInfo.width = width;
				fbufCreateInfo.height = height;
				fbufCreateInfo.layers = 1;
				VK_CHECK_RESULT(vkCreateFramebuffer(_dev->device, &fbufCreateInfo, nullptr, &it.framebuffer));
				// Fill a descriptor for later use in a descriptor set
				it.descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				it.descriptor.imageView = it.color._view;
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
			dcmds.resize(count_);
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
			VkCommandBufferBeginInfo cmdBufInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, 0, 0, 0 };

			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, 0, 0, 0, 0, 0, 0 };
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

			auto tid = get_tid();
			//log_format("build_cmd: %d\t%lld\n", dcbcount, tid);
			//return;
			if (/*dc_func &&*/ dcbcount)
			{
				if (idx < 0)
				{
					for (size_t i = 0; i < framebuffers.size(); ++i)
					{
						auto& dcmd = dcmds[i];
						dcmd._dev = _dev; dcmd.cmdb = drawCmdBuffers[i]; dcmd._is_compute = false;
						dcmd._viewport = { _width, _height };
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
					dcmd._viewport = { _width, _height };
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
				VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, 0, 0, 0, 0, 0, 0 };
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
				VkCommandBufferBeginInfo cmdBufInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, 0, 0, 0 };
				VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, 0, 0, 0, 0, 0, 0 };
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
			//_dev->destroyCommandBuffers(drawCmdBuffers);
			//device->destroySemaphore(semaphore); semaphore = 0;
			//_dev->destroyRenderPass(renderPass); renderPass = 0;
			//_dev->destroySampler(sampler); sampler = 0;
#endif // __ANDROID__

			for (auto& it : framebuffers)
			{
				//it.color.destroyBase(isColor);
				//it.depth_stencil.destroyBase(true);
				if (it.framebuffer)
					vkDestroyFramebuffer(_dev->device, it.framebuffer, 0);
				it.framebuffer = 0;
			}
		}
		void destroy_all()
		{
			destroyImage();

#if _WIN32
			//vkc::destroyCommandBuffers(device,drawCmdBuffers);
			if (nrp)
			{
				//device->destroyRenderPass(nrp); nrp = 0;
			}
			renderPass = 0;
			//device->destroySampler(sampler); sampler = 0;
#endif // __ANDROID__
			for (auto& it : framebuffers)
			{
				//device->destroySemaphore(it.semaphore); it.semaphore = 0;
			}
		}
	private:

	};

	fbo_cx::fbo_cx(int width, int height, unsigned int clear_color) :_width(width), _height(height)
		, _clear_color(clear_color)//, _fixed_gs_time(.60)
	{
		_incup = 0;
		_submitinfo = new submit_info();
		dc.push(_submitinfo);
		//camera = hz::Camera::create();
		// dx 11 需要true
		// vk 要用false
		//camera->set_top(false);
	}
	fbo_cx::~fbo_cx()
	{
		if (_swapcain)
		{
			delop(_swapcain);
		}
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
		if (!ctx)
		{
			ctx = new fbo_info_cx();
			dc.push(ctx);
		}
		ctx->_dev = (dvk_device*)dev;
		_dev = dev;
		// Find a suitable depth format
		VkBool32 validDepthFormat = getSupportedDepthFormat(ctx->_dev->physicalDevice, &ctx->depthFormat);
		assert(validDepthFormat);
		//默认颜色
		ctx->clearValues[0].color = { {0.0f, 0.0f, 0.0f, 0.0f} };
		//默认深度、模板
		ctx->clearValues[1].depthStencil = { 1.0f, 0 };
		if (scp)
		{
			count = scp->imageCount;
		}

		{
			ctx->count_ = count;
		}
		//if (!_presentComplete)
		//	vkc::createSemaphore(_dev, &_presentComplete, 0);
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
	//void fbo_cx::submit(int idx)
	//{
	//	//ctx->submit(idx);	//提交GPU执行渲染
	//}

	void fbo_cx::save_image(const std::string& fn, int idx)
	{
		//auto rt = Image::create();
		int i = 0;
		int ic = 1;
		int ac = ctx->framebuffers.size();
		if (idx < 0)
		{
			ic = ac;
		}
		for (int i = idx; i < ac; i++)
		{
			auto& t = ctx->framebuffers[i];
			auto fn1 = fn + ".png";
			if (ic > 1)
			{
				fn1 = (fn + std::to_string(i) + ".png");
			}
			auto img = t.color.save2file(fn1.c_str(), _queue, unm);
			if (ic == 1)break;
		}
		//Image::destroy(rt);
	}
	void fbo_cx::to_image(Image* img, int idx)
	{
		int ic = ctx->framebuffers.size();
		if (ic < 0 || idx > ic || !img)
		{
			return;
		}
		auto& t = ctx->framebuffers[idx];
		t.color.save2Image(img, _queue, true);
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
	int fbo_cx::acquire(uint32_t* imageIndex)
	{
		int ret = 0;
		if (_swapcain)
			ret = _swapcain->acquireNextImage(imageIndex);
		return ret;
	}
	submit_info* fbo_cx::get_submit(int idx)
	{
		auto& fb = ctx->framebuffers[idx];
		_submitinfo->clear();
		_submitinfo->push_cmd(ctx->drawCmdBuffers[idx], fb.semaphore, _swapcain->get_sem(), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
		return _submitinfo;
	}
	submit_info* fbo_cx::get_cur_submit()
	{
		assert(_swapcain);
		int idx = 0;
		if (_swapcain)
			idx = _swapcain->_curr_imageIndex;
		auto& fb = ctx->framebuffers[idx];
		_submitinfo->clear();
		_submitinfo->push_cmd(ctx->drawCmdBuffers[idx], fb.semaphore, _swapcain->get_sem()/*_presentComplete*/, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
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
			_incup = 0;
			return false;
		}

#ifdef _WIN32
#else
		isout = false;
#endif // _WIN32
		if (isout || (_isreset > 0 && (r_width * r_height != _width * _height)))
		{
			uint32_t uw = r_width, uh = r_height;
#ifdef _DEBUG
			print_time dwi("vkDeviceWaitIdle");
#endif
			//_dev->wait_idle();
			if (_swapcain)
			{
				_swapcain->create(&uw, &uh);
				_width = _swapcain->_width;
				_height = _swapcain->_height;
			}
			else {
				_width = r_width, _height = r_height;
			}
			if (_width > 0)
			{

				assert(_width > 0);
				ctx->reset_fbo(_width, _height);
				//camera->setOrtho(_width, _height);
				// 更新ubo
				_isreset--; inc_update();
				return true;
			}
			else {
				_incup = 0;
			}
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
		int a = _incup;
		//log_format("_fbo inc update %p %d\n", this, a);
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
	size_t fbo_cx::count()
	{
		return ctx->count_;
	}
#endif

	// todo set_pool_info
#if 1
	set_pool_info::set_pool_info()
	{
	}
	set_pool_info::~set_pool_info()
	{
		if (_dev && _dev->device)
		{
			for (auto it : set_ptr) {
				if (it)
					vkDestroyDescriptorSetLayout(_dev->device, it, 0);
			}
			for (auto it : pools) {
				if (it)
					vkDestroyDescriptorPool(_dev->device, it, 0);
			}
			if (pipelineLayout)
				vkDestroyPipelineLayout(_dev->device, pipelineLayout, 0);
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
		int spsize = set_ptr.size();		// 获取pipe的set数量
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
					pools.push_back(dev->new_desc_pool(_ps, max_sets * spsize)); // 分配完就创建新的池
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
	// todo pipeline_ptr_info
	pipeline_ptr_info::pipeline_ptr_info()
	{
	}

	pipeline_ptr_info::~pipeline_ptr_info()
	{
		if (pipelineCache && ismepc && _dev && _dev->device)
		{
			vkDestroyPipelineCache(_dev->device, pipelineCache, 0);
		}
	}

	void pipeline_ptr_info::init(VkRenderPass rpass, VkPipelineCache pipeline_cache)
	{
		if (rpass)
			render_pass = rpass;
		pipelineCache = pipeline_cache;
	}

	void pipeline_ptr_info::add_input_vbo(int n, int t)
	{
		vinput_info.push_back({ n, t });
	}


	dvk_cmd_pool::dvk_cmd_pool()
	{
	}

	dvk_cmd_pool::dvk_cmd_pool(VkCommandPool cp, dvk_device* dev) :command_pool(cp), _dev(dev)
	{

	}
	dvk_cmd_pool::~dvk_cmd_pool()
	{
		for (auto it : _fdc) { if (it)delete it; }
		_fdc.clear();
		auto ps = cmdv.data();
		for (auto n : _count)
		{
			_dev->free_cmd(command_pool, ps, n);
			ps += n;
		}
		_count.clear();
		cmdv.clear();
		vkDestroyCommandPool(_dev->device, command_pool, 0);
		command_pool = 0;
	}

	void dvk_cmd_pool::init(VkCommandPool cp, dvk_device* dev)
	{
		command_pool = cp; _dev = dev;
	}
	void dvk_cmd_pool::new_cmd(std::vector<VkCommandBuffer>& out, bool secondary)
	{
		//  VK_COMMAND_BUFFER_LEVEL_PRIMARY: 可以提交到队列执行，但不能从其他的命令缓冲区调用。
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
	void dvk_cmd_pool::new_cmd_secondary(VkCommandBuffer* buf, uint32_t count)
	{
		if (buf && count)
		{
			_dev->new_cmd((VkCommandPool)command_pool, VK_COMMAND_BUFFER_LEVEL_SECONDARY, buf, count);
		}
	}
	void dvk_cmd_pool::free_cmd(VkCommandBuffer* buf, uint32_t count)
	{
		if (buf && count)
			_dev->free_cmd(command_pool, buf, count);
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
		if (p)
		{
			_fdc.push_back(p);
		}
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
			if (!processor->pipeline)
			{
				delete processor; processor = 0;
			}
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
			//ringBuffer = device->new_buffer(dvkObjType::E_UBO, 8 * 1024 * 1024, nullptr);
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
			VkDevice device = _dev->device;
			if (pipeline != 0)
			{
				vkDestroyPipeline(device, pipeline, nullptr);
				pipeline = 0;// VK_NULL_HANDLE;
			}

			std::vector<VkPipelineShaderStageCreateInfo*> shader_infos;
			auto& sv = _shader->shader_data->v;
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
			computeCreateInfo.stage = *shader_infos[0];
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

	ubo_set_t dvk_set::get_binding(const std::string& name)
	{
		return ubo_set_t();
	}

	ubo_set_t dvk_set::write_image(const std::string& name, dvk_texture* tex, VkDescriptorSet dstSet)
	{
		assert(tex);
		ubo_set_t ret = {};
		do
		{
			if (!tex)break;
			auto it = setLayoutsInfo->imageParams.find(name);
			if (it != setLayoutsInfo->imageParams.end())
			{
				//// MLOGE("Failed write buffer, %s not found!", name.c_str());
				auto bindInfo = it->second;
				ret.dset = write_image(bindInfo.set, bindInfo.binding, bindInfo.descriptorType, tex, dstSet);
				ret.first_set = bindInfo.set;
				break;
			}
			auto n1 = hstring::toLower(name);
			for (auto& [k, v] : setLayoutsInfo->imageParams)
			{
				auto k1 = hstring::toLower(k);
				if (k1.find(n1) != std::string::npos)
				{
					auto bindInfo = v;
					ret.dset = write_image(bindInfo.set, bindInfo.binding, bindInfo.descriptorType, tex, dstSet);
					ret.first_set = bindInfo.set;
					break;
				}
			}
		} while (0);
		return ret;
	}

	VkDescriptorSet dvk_set::write_image(int set, int binding, uint32_t descriptorType, dvk_texture* tex, VkDescriptorSet dstSet)
	{
		auto dev = _dev;
		auto& it = _dset[set];
		auto ac = setLayoutsInfo->bindings.size();
		// set为一个时创建新set,或者多个set时每次创建新set
		if (!dstSet) {
			if ((ac == 1 && it.empty()) || ac > 1)
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
		_cy_info.push_back(bset_info{ set, binding, descriptorType, tex });
		_first_set[dstSet] = set;
		vkUpdateDescriptorSets(dev->device, 1, &writeDescriptorSet, 0, nullptr);
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
		do
		{
			if (!buffer_)break;
			auto it = setLayoutsInfo->bufferParams.find(name);
			if (it != setLayoutsInfo->bufferParams.end())
			{
				//// MLOGE("Failed write buffer, %s not found!", name.c_str());
				auto bindInfo = it->second;
				ret.dset = write_buffer(bindInfo.set, bindInfo.binding, bindInfo.descriptorType, buffer_, size, offset, dstSet);
				ret.first_set = bindInfo.set;
				ret.offsets = offset;
				break;
			}
			auto n1 = hstring::toLower(name);
			for (auto& [k, v] : setLayoutsInfo->bufferParams)
			{
				auto k1 = hstring::toLower(k);
				if (k1.find(n1) != std::string::npos)
				{
					auto bindInfo = v;
					ret.dset = write_buffer(bindInfo.set, bindInfo.binding, bindInfo.descriptorType, buffer_, size, offset, dstSet);
					ret.first_set = bindInfo.set;
					ret.offsets = offset;
					break;
				}
			}

		} while (0);
		return ret;
	}

	VkDescriptorSet dvk_set::write_buffer(int set, int binding, uint32_t descriptorType, dvk_buffer* buffer, uint64_t size, uint64_t offset, VkDescriptorSet dstSet)
	{
		auto dev = (dvk_device*)_dev;
		assert(buffer);
		auto& it = _dset[set];
		auto ac = setLayoutsInfo->bindings.size();
		// set为一个时创建新set,或者多个set时每次创建新set
		if (!dstSet)
		{
			if ((ac == 1 && it.empty()) || ac > 1)
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
		_cy_info.push_back(bset_info{ set, binding, descriptorType, buffer, size, offset });
		_first_set[dstSet] = set;
		vkUpdateDescriptorSets(dev->device, 1, &writeDescriptorSet, 0, nullptr);
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
			if ((ac == 1 && it.empty()) || ac > 1)
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
				_cy_info.push_back(bset_info{ (int)set_, (int)binding, dt, buffer, size, first_offset });
				_first_set[it[i]] = set_;
				upset.push_back(writeDescriptorSet);
			}
			vkUpdateDescriptorSets(_dev->device, upset.size(), upset.data(), 0, nullptr);
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

	// 合并set绑定


	// 设置多个VBO绑定

	void spv_res_info::add_input_binding(int location, bool instance, uint16_t stride)
	{
		inbind_desc d = {};
		d.location = location;
		d.binding = input_binding.size();
		d.stride = stride;
		d.inputRate = instance ? 1 : 0;
		input_binding.push_back(d);
	}

	size_t spv_res_info::union_binding(spv_res_info* sp)
	{
		size_t c = 0;
		for (auto& [s, v] : sp->bindings)
		{
			auto& dst = bindings[s];
			for (auto& [b, t] : v)
			{
				auto ts = t.stageFlags;
				//ts |= dst[b].descriptorType;
				ts |= dst[b].stageFlags;
				t.stageFlags = ts;
				dst[b] = t;
				c++;
			}
		}
		for (auto& [k, v] : sp->input_info)
		{
			input_info[k] = v;
		}
		for (auto& [k, v] : sp->bufferParams)
		{
			bufferParams[k] = v;
		}
		for (auto& [k, v] : sp->imageParams)
		{
			imageParams[k] = v;
		}
		for (auto& [k, v] : sp->paramsMap)
		{
			paramsMap[k] = v;
		}
		return c;
	}

	void spv_res_info::make_input()
	{
		if (input_binding.empty())
		{
			return;
		}
		int stride = 0;
		size_t i = 0;
		auto it = input_binding.begin();
		for (auto& [k, v] : input_info)
		{
			if (it->location > v.location)
			{
				i++;
				it++;
				it->stride = stride;
				stride = 0;
			}
			v.binding = i;
			stride += v.stride_size;
		}
	}

	// todo shader_info
	shader_info::~shader_info()
	{
		if (_dev && _dev->device)
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
				vkDestroyPipeline(_dev->device, _pipe[i], 0);
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

	VkPipeline shader_info::get_vkpipe(bool depthTestEnable, bool depthWriteEnable)
	{
		auto ret = _pipe[0];
		if (!_dev->ds_cb)
		{
			if (!depthWriteEnable && _pipe[1] && depthTestEnable)
				ret = _pipe[1];
			if (!depthTestEnable && no_depth_write)
			{
				ret = no_depth_write;
			}
		}
		return ret;
	}

	VkPipeline shader_info::get_vkpipe()
	{
		return  _pipe[0];
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

	size_t shader_info::new_sets(int idx, int n, VkDescriptorSet* out)
	{
		size_t ret = 0;
		if (n > 0 && out)
		{
			std::vector<VkDescriptorSet> dv;
			ret = _spinfo.new_sets(dv, idx, n);
			for (auto it : dv)
			{
				*out = it; out++;
			}
		}
		return ret;
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
	//			p[i].device = device;
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
		{
			clear();
			for (auto& it : sod)
				delop(it);
		}
	}
	void shader_hp::init(vk_render_cx* ctx, dvk_device* dev, const t_vector<t_string>& shader_path_fn)
	{
		_ctx = ctx;
		_dev = dev;
		for (auto& it : shader_path_fn)
		{
#ifndef __ANDROID__
			auto ks = access(it.c_str(), 0);
#endif
			auto ns = File::is_path2file(it.c_str());
			if (ns == 1 || it.find("zip") != std::string::npos)
			{
				load_shader_zip(it.c_str(), sod, pci);
			}
			else if (ns == 2)
			{
				load_file(it.c_str(), sod, pci);
			}
		}
		ctx->load_pipe(dev, pci.data(), pci.size(), _pis);
	}

	bool shader_hp::add(const char* n, const char* path, int len, bool is_comp)
	{
		bool ret = false;
		pipe_create_info pc = {};
		if (n && *n)
			pc.fn = n;
		pc.is_comp = is_comp;
		if (!len)
		{
			std::vector<char> data;
			File::read_binary_file(path, data);
			int pos = sod.size();
			if (data.size())
			{
				auto ps = new pipe_od();
				ps->fn = path;
				ps->data = std::move(data);
				sod.push_back(ps);
			}
			else {
				return ret;
			}
			if (!is_comp)
			{
				pc.is_comp = vk_base_info::get_stage(path, is_comp) == VK_SHADER_STAGE_COMPUTE_BIT;
			}
			pc.data = sod[pos]->data.data();
			pc.size = sod[pos]->data.size();
			if (!n || !*n)
			{
				pc.fn = sod[pos]->fn.c_str();
				pc.fn = hstring::replace(pc.fn.c_str(), "\\", "/");
			}
		}
		else {
			if (!is_comp)
			{
				pc.is_comp = vk_base_info::get_stage(n, is_comp) == VK_SHADER_STAGE_COMPUTE_BIT;
			}
			pc.data = path;
			pc.size = len;
		}
		size_t pidx = pci.size();
		auto pc1 = new pipe_create_info();
		*pc1 = pc;
		pci.push_back(pc1);
		ret = _ctx->load_pipe(_dev, pci.data() + pidx, pci.size() - pidx, _pis);
		return ret;
	}

	void shader_hp::clear()
	{
		assert(_ctx);
		for (auto& it : pci)
		{
			if (it->spv_info)
			{
				delop(it->spv_info);
			}
			delop(it);
		}
		pci.clear();
		_ctx->free_pipe_info(_pis);
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
			std::string pcin = pci[i]->fn;
			for (auto& str : sp->_spinfo.names)
			{
				if (pcin.find(str) != std::string::npos)
				{
					sp->shader_idx.push_back(i);
					sp->_srinfo.union_binding(pci[i]->spv_info);
				}
			}
		}
		auto dev = sp->_dev;
		if (!dev)
		{
			dev = _dev;
			sp->_dev = (dev);
		}
		sp->shader_data = _pis;
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

	ut_compute* shader_hp::new_compute(const std::vector<std::string>& fns, const std::vector<int>& idx)
	{
		auto dev = (dvk_device*)_dev;
		assert(dev);
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
			cp->init(cpl, _dev);
			ret->set_cp(cp);
		}
		return ret;
	}
	pipeline_ptr_info* shader_hp::new_pipe(const char* k, const njson& info, VkRenderPass render_pass, VkSampler immutableSamplers)
	{
		pipeline_ptr_info* ret = 0;
		auto v = info;
		do {
			pipeline_ptr_info* pt = dc.ac<pipeline_ptr_info>();
			pt->init(render_pass, 0);
			pt->_spinfo.immutableSamplers = immutableSamplers;
			int inps = v["input"].size();
			if (inps > 0 && v["input"].is_array())
			{
				auto& vi = v["input"];
				for (int i = 0; i < inps; i += 2)
				{
					pt->add_input_vbo(vi[i], vi[i + 1]);
				}
			}
			bool isdisable = toBool(v["disable"], false);
			if (isdisable)break;
			// 顶点属性名带_ins
			// 填充方式VK_POLYGON_MODE_FILL = 0, VK_POLYGON_MODE_LINE = 1, VK_POLYGON_MODE_POINT
			pt->pso.polygon_mode = toInt(v["polygon_mode"], 0);
			// 剔除模式 VK_CULL_MODE_NONE = 0, VK_CULL_MODE_FRONT_BIT = 1, VK_CULL_MODE_BACK_BIT = 2, VK_CULL_MODE_FRONT_AND_BACK = 3,
			pt->pso.cull_mode = toInt(v["cull_mode"], 0);
			//表示顺时针方向为正面（VK_FRONT_FACE_CLOCKWISE=1）和逆时针方向为正面（VK_FRONT_FACE_COUNTER_CLOCKWISE=0）
			pt->pso.front_face = toInt(v["front_face"], 0);
			pt->pso._blend_type = toInt(v["blend_type"], 0);
			//pt->pso.blendConstants = toVec4(v["blendConstants"]);
			pt->pso.depthTestEnable = toBool(v["depthTestEnable"], true);
			pt->pso.depthRW = toBool(v["depthRW"], true);
			pt->pso.fillModeNonSolid = toBool(v["fillModeNonSolid"], false);
			// 顶点属性为列数组，input为自定义分组
			pt->pso.input_cut = toBool(v["input_cut"], false);
			if (v.find("input_fs") != v.end() && v["input_fs"].is_array())
			{
				auto& vi = v["input_fs"];
				auto it = &pt->pso.input_info[0];
				for (int i = 0; i < 32 && i < vi.size(); it++)
				{
					*it = { vi[i],vi[i + 1] }; i += 2;
					pt->pso.icount++;
				}
			}
			pt->name = k;
			if (mpipe.find(k) != mpipe.end())
			{
				auto tp = mpipe[k];
				dc.pop(tp, 0);
			}
			auto p = new_pipe(pt, v["file"]);
			if (!p) {
				printf("not load: %s\n", v.dump(2).c_str());

			}
			//assert(p);
			if (p)
				mpipe[k] = p;
			ret = p;
		} while (0);
		return ret;
	}
	int shader_hp::new_pipe(const njson& info, VkRenderPass render_pass, VkSampler immutableSamplers)
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
				if (new_pipe(k.c_str(), v, render_pass, immutableSamplers))ret++;
			}
		}
		return 0;
	}
	pipeline_ptr_info* shader_hp::new_pipe1(const std::string& newname, const std::string& k, VkRenderPass render_pass, VkSampler immutableSamplers)
	{
		pipeline_ptr_info* r = 0;
		auto nk = _jnd[pipeidx];
		nk = nk["pipe_info"];
		if (nk.find(k) != nk.end())
		{
			r = new_pipe(newname.c_str(), nk[k], render_pass, immutableSamplers);
		}
		return r;
	}
	std::map<std::string, pipeline_ptr_info*>& shader_hp::mk_pipe(VkRenderPass render_pass, VkSampler immutableSamplers)
	{
		// TODO: 在此处插入 return 语句
		new_pipe(_jnd[pipeidx], render_pass, immutableSamplers);
		return mpipe;
	}
	dvk_device* shader_hp::get_dev()
	{
		return _dev;
	}
	void shader_hp::load_shader_zip(const std::string& zfn, std::vector<pipe_od*>& sod, std::vector<pipe_create_info*>& out)
	{
		auto zd = Zip::create(zfn);
		dc.push(zd);
		size_t pos = sod.size();
		zd->enum_file("*.spv", [&sod](const std::string& fn, std::vector<char>* data)
			{
				auto po = new pipe_od();
				po->fn = fn; po->data = std::move(*data);
				sod.push_back(po);
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
			pc.data = sod[i]->data.data();
			pc.size = sod[i]->data.size();
			pc.fn = zfn + "/" + sod[i]->fn.c_str();
			pc.fn = hstring::replace(pc.fn.c_str(), "\\", "/");
			auto pc1 = new pipe_create_info(); *pc1 = pc;
			o.push_back(pc1);
		}
	}
	void shader_hp::enum_read_file(const std::string rfn, std::vector<std::string>& fns, std::vector<shader_hp::pipe_od*>& sod)
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
					auto ps = new pipe_od();
					ps->fn = k;
					ps->data = std::move(data);
					sod.push_back(ps);
				}
			}
		}
	}
	void shader_hp::load_file(const std::string& path, std::vector<pipe_od*>& sod, std::vector<pipe_create_info*>& out)
	{
		std::vector<std::string> fns;
		hz::File::IterFiles(path, &fns);
		size_t pos = sod.size();
		enum_read_file("*.spv", fns, sod);
		if (sod.empty())
		{
			return;
		}
		out.reserve(out.size() + sod.size());
		auto& o = out;
		for (size_t i = pos; i < sod.size(); i++)
		{
			pipe_create_info pc = {};
			pc.data = sod[i]->data.data();
			pc.size = sod[i]->data.size();
			pc.fn = sod[i]->fn.c_str();
			pc.fn = hstring::replace(pc.fn.c_str(), "\\", "/");
			auto pc1 = new pipe_create_info(); *pc1 = pc;
			o.push_back(pc1);
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
	// todo fence
	int dvk_fence::wait_for()
	{
		int ret = 0;
		if (fence)
			vkWaitForFences(dev, 1, &fence, true, 2000);
		ret = get_status();
		return ret;
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
			descriptor = new VkDescriptorBufferInfo();
			device = *((VkDevice*)dev);
			auto d = (dvk_device*)dev;
			vkc::create_buffer(dev, usage, (VkMemoryPropertyFlags)mempro, size, &buffer, &memory, data, &_capacity, this);
		}
	}

	dvk_buffer::~dvk_buffer()
	{
		delop(descriptor);
		destroybuf();
	}
	void dvk_buffer::destroybuf()
	{
		unmap();
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
		//descriptors.clear();
	}

	void dvk_buffer::resize(size_t size)
	{
		if (size > _capacity)
		{
			destroybuf();
			vkc::create_buffer(_dev, usageFlags, (VkMemoryPropertyFlags)memoryPropertyFlags, size, &buffer, &memory, nullptr, &_capacity, this);
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

		p->descriptor->buffer = p->buffer;
		p->descriptor->offset = 0;
		p->descriptor->range = -1;
		return p;
	}
	dvk_buffer* dvk_buffer::new_indirect(dvk_device* dev, bool device_local, uint32_t size, void* data)
	{
		uint32_t usage = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		VkMemoryPropertyFlags mempro = get_mpflags(device_local, &usage);
		auto p = new dvk_buffer(dev, usage, mempro, size, data);

		p->descriptor->buffer = p->buffer;
		p->descriptor->offset = 0;
		p->descriptor->range = -1;
		return p;
	}
	dvk_buffer* dvk_buffer::new_ibo(dvk_device* dev, int type, bool device_local, uint32_t count, void* data)
	{
		uint32_t usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		uint32_t indexBufferSize = count * (type ? sizeof(uint32_t) : sizeof(uint16_t));
		VkMemoryPropertyFlags mempro = get_mpflags(device_local, &usage);
		// todo new
		auto p = new dvk_buffer(dev, usage, mempro, indexBufferSize, data);
		p->descriptor->buffer = p->buffer;
		p->descriptor->offset = 0;
		p->descriptor->range = -1;

		return p;
	}
	dvk_buffer* dvk_buffer::new_ubo(dvk_device* dev, uint32_t size, void* data, uint32_t usage1)
	{
		uint32_t usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | usage1;
		VkMemoryPropertyFlags mempro = get_mpflags(false, &usage);
		auto p = new dvk_buffer(dev, usage, mempro, size, data);
		p->descriptor->buffer = p->buffer;
		p->descriptor->offset = 0;
		p->descriptor->range = -1;

		return p;
	}

	dvk_buffer* dvk_buffer::new_ssbo(dvk_device* dev, uint32_t size, void* data)
	{
		uint32_t usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		VkMemoryPropertyFlags mempro = get_mpflags(false, &usage);
		auto p = new dvk_buffer(dev, usage, mempro, size, data);
		p->descriptor->buffer = p->buffer;
		p->descriptor->offset = 0;
		p->descriptor->range = -1;
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
		auto qctx = _dev->get_graphics_queue(1);
		auto cp = qctx->new_cmd_pool();
		auto copyQueue = qctx->get_vkptr();
		VkCommandBuffer copyCmd = vkc::createCommandBuffer1(_dev->device, cp->command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

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
		vkc::flushCommandBuffer(_dev->device, copyCmd, cp->command_pool, copyQueue, true);
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
	size_t dvk_buffer::set_data(void* data, size_t size, size_t offset, bool is_flush, bool iscp)
	{
		if (_size < size)
			return 0;
		if (!mapped)map(_size, 0);
		assert(mapped);
		if (iscp)
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

	void dvk_buffer::copy_to(void* data, size_t size)
	{
		set_data(data, size, 0, false);
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

	void* dvk_buffer::get_map(VkDeviceSize offset)
	{
		char* p = (char*)mapped;
		assert(p);
		return p + offset;
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

	dvk_swapchain::~dvk_swapchain() {
		for (auto it : semaphore)
		{
			if (it)
				vkDestroySemaphore(_dev->device, it, 0);
		}
	}

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

		VkResult err = {};
		auto dev = (dvk_device*)_dev;
		VkSwapchainKHR oldSwapchain = swapChain;
		if (!dev->physicalDevice)return false;
		// Get physical device surface properties and formats
		VkSurfaceCapabilitiesKHR surfCaps = {};
		try {
			//err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev->physicalDevice, surface, &surfCaps);

		}
		catch (const std::exception&)
		{

		}
		///assert(!err);

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
		if (surfCaps.currentExtent.width == (uint32_t)-1 || surfCaps.currentExtent.width == 0)
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
			swapchainCI.imageExtent = { swapchainExtent.width, swapchainExtent.height };
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
			VkSwapchainKHR csc = {};
			err = vkCreateSwapchainKHR(dev->device, &swapchainCI, nullptr, &csc);
			if (err)
			{
				auto er = vkc::errorString(err);
				auto estr = hz::format("vkCreateSwapchainKHR: %d\t%s\n", err, er.c_str());
				dslog(estr.c_str());
				if (err == VK_ERROR_NATIVE_WINDOW_IN_USE_KHR)
				{
					return true;
				}
			}
			else {
				swapChain = csc;
			}
			assert(!err);
			//return ret;
		}
		// If an existing swap chain is re-created, destroy the old swap chain
		// This also cleans up all the presentable images
		if (oldSwapchain)
		{
			for (auto it : semaphore)
			{
				if (it)
					vkDestroySemaphore(_dev->device, it, 0);
			}
			semaphore.clear();
			for (uint32_t i = 0; i < imageCount; i++)
			{
				vkDestroyImageView(dev->device, buffers[i]._view, nullptr);
			}
			vkDestroySwapchainKHR(dev->device, oldSwapchain, nullptr);
		}

		err = vkGetSwapchainImagesKHR(dev->device, swapChain, &imageCount, NULL);
		assert(!err);

		// Get the swap chain images
		images.resize(imageCount);
		err = vkGetSwapchainImagesKHR(dev->device, swapChain, &imageCount, images.data());
		assert(!err);

		// Get the swap chain buffers containing the image and imageview
		buffers.resize(imageCount);
		//m_ImageAvailableSemaphores
		semaphore.resize(imageCount);
		for (uint32_t i = 0; i < imageCount; i++)
		{
			if (semaphore[i] == VK_NULL_HANDLE)
			{
				vkc::createSemaphore(_dev, &semaphore[i], 0);
			}
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

			err = vkCreateImageView(dev->device, &colorAttachmentView, nullptr, &buffers[i]._view);
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

	uint32_t dvk_swapchain::acquireNextImage(uint32_t* imageIndex)
	{
		VkDevice device = _dev->device;
		//const uint32_t prev = _semaphoreIndex;
		//_semaphoreIndex = (_semaphoreIndex + 1) % imageCount;
		uint32_t simageIndex = 0;
		if (!imageIndex)
		{
			imageIndex = &simageIndex;
		}
		VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, semaphore[_semaphoreIndex], VK_NULL_HANDLE, imageIndex);

		_prevSemaphoreIndex = _semaphoreIndex;
		_semaphoreIndex++;
		if (_semaphoreIndex >= imageCount)
			_semaphoreIndex = 0;
		// todo wait fences rest
		do
		{
			if (result == VK_SUCCESS)
				break;
			auto ks = vkc::errorString(result);
			std::string logstr = hz::format("vkAcquireNextImageKHR:<%s>%d\tdo you resize swapchain\n", ks.c_str(), result);
			dslog(logstr.c_str(), logstr.size());

			if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR) || result == VK_ERROR_SURFACE_LOST_KHR) {

				c_width = c_height = 0;
				*imageIndex = -1;

				return result;
			}
		} while (0);
		_NumAcquireCalls += 1;
		//*outSemaphore = semaphore[_semaphoreIndex];
		_curr_imageIndex = *imageIndex;
		return result;
	}

	//void SwapChain::GetSemaphores(VkSemaphore* pImageAvailableSemaphore, VkSemaphore* pRenderFinishedSemaphores, VkFence* pCmdBufExecutedFences)
	//{
	//	*pImageAvailableSemaphore = m_ImageAvailableSemaphores[m_prevSemaphoreIndex];
	//	*pRenderFinishedSemaphores = m_RenderFinishedSemaphores[m_semaphoreIndex];
	//	*pCmdBufExecutedFences = m_cmdBufExecutedFences[m_semaphoreIndex];
	//}
	/**
	* Destroy and free Vulkan resources used for the swapchain
	*/

	VkSemaphore dvk_swapchain::get_sem()
	{
		return semaphore[_prevSemaphoreIndex];
	}

	void dvk_swapchain::cleanup()
	{
		auto dev = (dvk_device*)_dev;
		if (swapChain)
		{
			for (uint32_t i = 0; i < imageCount; i++)
			{
				vkDestroyImageView(dev->device, buffers[i]._view, nullptr);
			}
		}
		if (surface)
		{
			vkDestroySwapchainKHR(dev->device, swapChain, nullptr);
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
		freeres();

	}

	void dvk_device::init_info(VkInstance inst)
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
	}
	uint32_t dvk_device::get_ubo_range()
	{
		assert(_limits);
		return _limits->maxUniformBufferRange;
	}
	VkDeviceSize dvk_device::get_ubo_align()
	{
		assert(_limits);
		return _limits->minUniformBufferOffsetAlignment;
	}
	uint32_t dvk_device::get_ssbo_range()
	{
		assert(_limits);
		return _limits->maxStorageBufferRange;
	}
	VkDeviceSize dvk_device::get_ssbo_align()
	{
		assert(_limits);
		return _limits->minStorageBufferOffsetAlignment;
	}
	uint32_t dvk_device::get_constant_size()
	{
		assert(_limits);
		return _limits->maxPushConstantsSize;
	}
	VkDeviceSize dvk_device::get_align_memory_map()
	{
		assert(_limits);
		return _limits->minMemoryMapAlignment;
	}
	VkDeviceSize dvk_device::get_align_texel_offset()
	{
		assert(_limits);
		return _limits->minTexelBufferOffsetAlignment;
	}
	VkDeviceSize dvk_device::get_align_optimal_offset()
	{
		assert(_limits);
		return _limits->optimalBufferCopyOffsetAlignment;
	}
	VkDeviceSize dvk_device::get_align_optimal_rowpitch_offset()
	{
		assert(_limits);
		return _limits->optimalBufferCopyRowPitchAlignment;
	}
	void dvk_device::reset0()
	{
		device = 0;
		freeres();
		init(0, _iscompute);
		//Image img[1];
		//img->resize(512, 512);
		//auto k = dvk_texture::new_image2d(img, this);
		printf("reset dev\n");
	}
	void dvk_device::freeres()
	{
		DestroyPipelineCache();
		for (auto it : graphics_queue_cxs)
		{
			delop(it);
		}
		graphics_queue_cxs.clear();
		for (auto it : compute_queue_cxs)
		{
			delop(it);
		}
		compute_queue_cxs.clear();
		delop(_info);
		delop(ds_cb);
		delop(_limits);
		if (device)
			vkDestroyDevice(device, 0);
		device = 0;
	}
	void dvk_device::init(VkInstance inst, bool iscompute)
	{
		assert(physicalDevice);
		//if(physicalDevice)
		//this->physicalDevice = physicalDevice;
		if (inst)
			_info->instance = inst;
		_iscompute = iscompute;
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
		if (enabled_features_cb)
			enabled_features_cb(&_info->features, &_info->enabledFeatures);
		//LOGI("extensionName: %d\n", (int)supportedExtensions.size());
		//framebufferDepthSampleCounts = getMaxUsableSampleCount();
		VkQueueFlags qe = VK_QUEUE_GRAPHICS_BIT;
		if (iscompute)qe |= VK_QUEUE_COMPUTE_BIT;
		bool dynamic_rendering = extensionSupported("VK_KHR_dynamic_rendering");
		bool physical_device_properties2 = extensionSupported("VK_KHR_get_physical_device_properties2");
		bool nv_mesh_shader = extensionSupported("VK_NV_mesh_shader");
		bool nv_ray_tracing = extensionSupported("VK_NV_ray_tracing");
		bool ray_tracing = extensionSupported("VK_KHR_ray_tracing_pipeline");
		bool is_dynamic_state = extensionSupported("VK_EXT_extended_dynamic_state");
		bool is_dynamic_state1 = extensionSupported(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME);
		bool is_debug_utils = extensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		bool is_line = extensionSupported(VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME);

		printf("ray_tracing: %s\n", ray_tracing || nv_ray_tracing ? "true" : "false");
		printf("is_dynamic_state: %s\n", is_dynamic_state ? "true" : "false");
		for (auto it : supportedExtensions)
		{
			if (it.find("h264") != std::string::npos)
			{
				printf("%s\n", it.c_str());
			}
		}
		void* pNextChain = 0;
#if 1
		//def PFN_vkCmdBeginRenderingKHR
		VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeaturesKHR = {};
		if (dynamic_rendering)
		{
			// Enable anisotropic filtering if supported
			if (_info->features.samplerAnisotropy) {
				_info->enabledFeatures.samplerAnisotropy = VK_TRUE;
			}
			dynamicRenderingFeaturesKHR.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
			dynamicRenderingFeaturesKHR.dynamicRendering = VK_TRUE;

			pNextChain = &dynamicRenderingFeaturesKHR;

			_info->enabledExtensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
		}
		if (extensionSupported(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME))
			_info->enabledExtensions.push_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME);
		//if (deviceFeatures.samplerAnisotropy) {
		//	enabledFeatures.samplerAnisotropy = VK_TRUE;
		//};
#endif
		//	dynamic_state需要两个扩展支持VK_KHR_get_physical_device_properties2\VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME
		if (is_dynamic_state && physical_device_properties2)
			_info->enabledExtensions.push_back(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME);
		vkc::createLogicalDevice(this, _info->enabledFeatures, _info->enabledExtensions, pNextChain, true, qe);

#ifdef PFN_vkCmdBeginRenderingKHR
		//VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME;
		//reinterpret_cast<PFN_vkCmdBeginRenderingKHR>reinterpret_cast<PFN_vkCmdEndRenderingKHR>
		auto CmdBeginRenderingKHR = (PFN_vkCmdBeginRenderingKHR*)(vkGetDeviceProcAddr(_dev, "vkCmdBeginRenderingKHR"));
		auto CmdEndRenderingKHR = (PFN_vkCmdEndRenderingKHR*)(vkGetDeviceProcAddr(_dev, "vkCmdEndRenderingKHR"));
#endif
		//auto brcb = (PFN_vkCmdBeginRenderingKHR)(vkGetInstanceProcAddr(_info->instance, "vkCmdBeginRenderingKHR"));
		auto cbk12 = vkGetDeviceProcAddr(device, "vkCmdBeginRenderingKHR");
		if (is_dynamic_state && physical_device_properties2)
		{
			VkPhysicalDeviceProperties2 dp2[1] = {};
			dp2->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR;
			//auto pdp2 = (PFN_vkGetPhysicalDeviceProperties2KHR)(vkGetDeviceProcAddr(device, "vkGetPhysicalDeviceProperties2KHR"));
			auto pdp20 = (PFN_vkGetPhysicalDeviceProperties2KHR)(vkGetInstanceProcAddr(_info->instance, "vkGetPhysicalDeviceProperties2KHR"));
			//pdp2 = (PFN_vkGetPhysicalDeviceProperties2KHR)(vkGetDeviceProcAddr(device, "vkGetPhysicalDeviceProperties2"));
			if (!pdp20)
				pdp20 = (PFN_vkGetPhysicalDeviceProperties2KHR)(vkGetInstanceProcAddr(_info->instance, "vkGetPhysicalDeviceProperties2"));
			VkPhysicalDeviceRayTracingPipelinePropertiesKHR m_rtProperties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR };
			dp2->pNext = &m_rtProperties;
			if (pdp20)
				pdp20(physicalDevice, dp2);

			if (!ds_cb)
				ds_cb = new dynamic_state_ext_cb();
			if (0)
			{
				auto cbk1 = (PFN_vkCmdSetCullModeEXT)vkGetInstanceProcAddr(_info->instance, "vkCmdSetCullModeEXT");
				auto cbk12 = vkGetDeviceProcAddr(device, "vkCmdSetCullModeEXT");
				assert(printf("vkCmdSetCullModeEXT: %p\n", cbk1));
			}
#define DSEXT(n) (#n)
			const char* dystr[] = {
			DSEXT(vkCmdSetCullModeEXT),
			DSEXT(vkCmdSetFrontFaceEXT),
			DSEXT(vkCmdSetPrimitiveTopologyEXT),
			DSEXT(vkCmdSetViewportWithCountEXT),
			DSEXT(vkCmdSetScissorWithCountEXT),
			DSEXT(vkCmdBindVertexBuffers2EXT),
			DSEXT(vkCmdSetDepthTestEnableEXT),
			DSEXT(vkCmdSetDepthWriteEnableEXT),
			DSEXT(vkCmdSetDepthCompareOpEXT),
			DSEXT(vkCmdSetDepthBoundsTestEnableEXT),
			DSEXT(vkCmdSetStencilTestEnableEXT),
			DSEXT(vkCmdSetStencilOpEXT),
#undef DSEXT
			};
			int dyn = sizeof(dystr) / sizeof(char*);
			void** dst = (void**)ds_cb;
			t_vector<void*> dv2;
			for (int i = 0; i < dyn; i++, dst++)
				*dst = (void*)vkGetInstanceProcAddr(_info->instance, dystr[i]);
			for (int i = 0; i < dyn; i++)
				dv2.push_back((void*)vkGetDeviceProcAddr(device, dystr[i]));
		}
		void* setls = 0;
		if (is_line)
		{
			setls = (void*)vkGetInstanceProcAddr(_info->instance, "vkCmdSetLineStippleEXT");
		}
		mkDeviceQueue();


		VkPhysicalDevicePushDescriptorPropertiesKHR pushDescriptorProps{};
		// The push descriptor update function is part of an extension so it has to be manually loaded
		vkCmdPushDescriptorSet = (PFN_vkCmdPushDescriptorSetKHR)vkGetDeviceProcAddr(device, "vkCmdPushDescriptorSetKHR");
		if (!vkCmdPushDescriptorSet) {
			//vks::tools::exitFatal("Could not get a valid function pointer for vkCmdPushDescriptorSetKHR", -1);
		}

		// Get device push descriptor properties (to display them)
		PFN_vkGetPhysicalDeviceProperties2KHR vkGetPhysicalDeviceProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2KHR>(vkGetInstanceProcAddr(_info->instance, "vkGetPhysicalDeviceProperties2KHR"));
		if (vkGetPhysicalDeviceProperties2KHR) {
			//vks::tools::exitFatal("Could not get a valid function pointer for vkGetPhysicalDeviceProperties2KHR", -1);

			VkPhysicalDeviceProperties2KHR deviceProps2{};
			pushDescriptorProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR;
			deviceProps2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR;
			deviceProps2.pNext = &pushDescriptorProps;
			vkGetPhysicalDeviceProperties2KHR(physicalDevice, &deviceProps2);
			pushDescriptorProps.maxPushDescriptors;
		}


#ifdef _WIN32
		vkc::ExtDebugUtilsCheckInstanceExtensions(true);
		vkc::ExtDebugUtilsGetProcAddresses(device);
#endif
	}

	void dvk_device::mkDeviceQueue()
	{
		auto gp = vkc::get_queue_fp(this, VK_QUEUE_GRAPHICS_BIT);
		graphics_queues.resize(gp->queueCount);
		for (size_t i = 0; i < gp->queueCount; i++)
		{
			vkGetDeviceQueue(device, queueFamilyIndices.graphics, i, &graphics_queues[i]);
		}
		graphics_queue_cxs.resize(gp->queueCount);
		gp = vkc::get_queue_fp(this, VK_QUEUE_COMPUTE_BIT);
		compute_queues.resize(gp->queueCount);
		for (size_t i = 0; i < gp->queueCount; i++)
		{
			vkGetDeviceQueue(device, queueFamilyIndices.compute, i, &compute_queues[i]);
		}
		compute_queue_cxs.resize(gp->queueCount);
		return;
	}


	// new


	void dvk_device::free_shader(VkShaderModule s)
	{
		vkDestroyShaderModule(device, s, nullptr);
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
		bool input_cut = p->pso.input_cut;
		if (input_cut && vii.empty())
		{
			for (auto& it : p->_srinfo.input_info)
			{
				vii.push_back({ 1, 0 });
			}
		}
		auto pi = vii.begin();
		std::vector<inbind_desc> input_binding;
		glm::uvec2* iot = 0;
		int ic = p->pso.icount;
		if (ic > 0)
			iot = &p->pso.input_info[0];
		for (auto& [k, it] : p->_srinfo.input_info)
		{
			if (iot && ic > 0)
			{
				vertex_attribute_descriptions.push_back({ (uint32_t)it.location, (uint32_t)in_binding, (VkFormat)iot->x, (uint32_t)in_offset });
				in_offset += iot->y;
				iot++; ic--;
			}
			else
			{
				vertex_attribute_descriptions.push_back({ (uint32_t)it.location, (uint32_t)in_binding, (VkFormat)it.format, (uint32_t)in_offset });
				in_offset += it.stride_size;
			}
			if (pi != vii.end())
			{
				pi->x--;
				if (pi->x == 0)
				{
					uint8_t rate = (uint8_t)pi->y;
					if (it.name.find("_inst") != std::string::npos || it.name.find("inst_") != std::string::npos)
						rate = VK_VERTEX_INPUT_RATE_INSTANCE;
					inbind_desc id = {};
					id.binding = (uint16_t)in_binding;
					id.inputRate = rate;
					id.stride = in_offset;
					input_binding.push_back(id);
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
			bindingDescriptions.push_back({ (uint32_t)0, (uint32_t)stride, VK_VERTEX_INPUT_RATE_VERTEX });
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
			VK_FALSE,                                                      // VkBool32                                       depthClampEnable
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
		VkPipelineColorBlendAdvancedStateCreateInfoEXT bas = {
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_ADVANCED_STATE_CREATE_INFO_EXT,//VkStructureType      sType;
			0,//const void*          pNext;
			VK_FALSE,//VkBool32             srcPremultiplied;
			VK_FALSE,//VkBool32             dstPremultiplied;
			//VkBlendOverlapEXT    blendOverlap;
			VK_BLEND_OVERLAP_UNCORRELATED_EXT
			//VK_BLEND_OVERLAP_DISJOINT_EXT
			//VK_BLEND_OVERLAP_CONJOINT_EXT
		};
		//color_blend_state_create_info.pNext = &bas;
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
		auto& sv = p->shader_data->v;
		assert(sv.size() > 0);
		for (auto it : p->shader_idx) {
			if (it >= 0 && it < sv.size())
				shader_infos.push_back(*sv[it]);
		}
		if (shader_infos.empty())
		{
			return false;
		}
		auto pipeline_layout = (VkPipelineLayout)p->_spinfo.pipelineLayout;
		VkGraphicsPipelineCreateInfo pipeline_create_info[2] = { {
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
			} };
		uint32_t count = 1;
		if (!ds_cb && p->pso.depthTestEnable && p->pso.depthRW)
		{
			memcpy(&pipeline_create_info[1], &pipeline_create_info[0], sizeof(VkGraphicsPipelineCreateInfo));
			pipeline_create_info[1].pDepthStencilState = &depth_stencil_state[1];
			count++;
		}
		if (!p->pipelineCache)
		{
			p->pipelineCache = load_pipeline_cache(0, 0);
			p->ismepc = true;
		}
		if (vkCreateGraphicsPipelines(device, p->pipelineCache, count, pipeline_create_info, nullptr, (VkPipeline*)p->_pipe) != VK_SUCCESS) {
			std::cout << "Could not create graphics pipeline!" << std::endl;
			return false;
		}
		uint32_t hr = 0;
		// 不支持动态时创建新pipe
		if (!ds_cb)
		{
			// 关闭深度测试
			if (p->pso.depthTestEnable && p->pso.depthRW)
			{
				memcpy(&pipeline_create_info[1], &pipeline_create_info[0], sizeof(VkGraphicsPipelineCreateInfo));
				pipeline_create_info[1].pDepthStencilState = &depth_stencil_state[1];
				depth_stencil_state[1].depthTestEnable = VK_FALSE;
				depth_stencil_state[1].depthWriteEnable = VK_FALSE;
				hr = (vkCreateGraphicsPipelines(device, p->pipelineCache, 1, &pipeline_create_info[1], nullptr, &p->no_depth_write));
			}
		}
		// 线框Wire frame rendering pipeline
		if (p->pso.fillModeNonSolid && p->pso.polygon_mode != VK_POLYGON_MODE_LINE)
		{
			rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_LINE;
			rasterization_state_create_info.lineWidth = 1.0f;
			hr = (vkCreateGraphicsPipelines(device, p->pipelineCache, 1, &pipeline_create_info[1], nullptr, &p->wireframe));
		}
		get_pipeline_cache(p->pipelineCache, 0);
		return true;

	}
	VkPipelineCache dvk_device::load_pipeline_cache(const void* d, size_t size)
	{
		VkPipelineCacheCreateInfo pio = {};
		pio.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		pio.initialDataSize = size;
		pio.pInitialData = d;
		VkPipelineCache ret = 0;
		vkCreatePipelineCache(device, &pio, 0, &ret);
		return ret;
	}
	size_t dvk_device::get_pipeline_cache(VkPipelineCache pc, std::vector<char>* dst)
	{
		size_t ret = 0;
		if (pc)
		{
			vkGetPipelineCacheData(device, pc, &ret, 0);
			if (ret > 0 && dst)
			{
				dst->resize(ret);
				vkGetPipelineCacheData(device, pc, &ret, dst->data());
			}
		}
		return ret;
	}

	bool dvk_device::merge_pipeline_cache(VkPipelineCache pc, VkPipelineCache* src_pc, uint32_t n)
	{
		size_t ret = 0;
		if (pc && src_pc)
		{
			ret = vkMergePipelineCaches(device, pc, n, src_pc) == VK_SUCCESS;
		}
		return ret;
	}

	void dvk_device::CreatePipelineCache()
	{
		// create pipeline cache

		VkPipelineCacheCreateInfo pipelineCache;
		pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		pipelineCache.pNext = NULL;
		pipelineCache.initialDataSize = 0;
		pipelineCache.pInitialData = NULL;
		pipelineCache.flags = 0;
		VkResult res = vkCreatePipelineCache(device, &pipelineCache, NULL, &_pipelineCache);
		assert(res == VK_SUCCESS);
	}

	void dvk_device::DestroyPipelineCache()
	{
		if (_pipelineCache)
			vkDestroyPipelineCache(device, _pipelineCache, NULL);
		_pipelineCache = 0;
	}

	VkPipelineCache dvk_device::GetPipelineCache()
	{
		if (!_pipelineCache)
			CreatePipelineCache();
		return _pipelineCache;
	}



	VkCommandPool dvk_device::new_cmd_pool(uint32_t idx)
	{
		return vkc::createCommandPool(device, idx, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	}

	VkCommandPool dvk_device::new_graphics_cmd_pool()
	{
		return vkc::createCommandPool(device, queueFamilyIndices.graphics, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	}

	VkCommandPool dvk_device::new_compute_cmd_pool()
	{
		return vkc::createCommandPool(device, queueFamilyIndices.compute, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
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
					VkSampler* pImmutableSamplers = nullptr;
					if (sp->immutableSamplers && VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER == (VkDescriptorType)dt.descriptorType)
					{
						pImmutableSamplers = &sp->immutableSamplers;
					}
					slb.push_back({ (uint32_t)k1, (VkDescriptorType)dt.descriptorType, (uint32_t)dt.descriptorCount, (VkShaderStageFlags)dt.stageFlags, pImmutableSamplers });
					ps[dt.descriptorType] += dt.descriptorCount;
				}
			}
		}
		std::vector<VkDescriptorSetLayout> setLayouts;
		for (auto& it : set_layout_binding)
		{
			VkDescriptorSetLayout temp = 0;

			VkDescriptorSetLayoutCreateInfo ninfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, nullptr, 0, (uint32_t)it.size(), it.data() };
			vkCreateDescriptorSetLayout(device, &ninfo, nullptr, &temp);
			setLayouts.push_back(temp);
			sp->set_ptr.push_back(temp);
		}
		//VkPushConstantRange pcs[2] = { {VK_SHADER_STAGE_VERTEX_BIT,0,64} , {VK_SHADER_STAGE_FRAGMENT_BIT,64,64} };
		VkPushConstantRange pconstant[2] = { {VK_SHADER_STAGE_VERTEX_BIT, 0, sp->pct_vert}, {VK_SHADER_STAGE_FRAGMENT_BIT, sp->pct_vert, sp->pct_frag} };
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, nullptr, 0,
			(uint32_t)setLayouts.size(), setLayouts.data(),
			2, pconstant
		};
		// Now create the pipeline layout.
		vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, (VkPipelineLayout*)&sp->pipelineLayout);
		return true;
	}

	VkDescriptorPool dvk_device::new_desc_pool(std::map<uint32_t, uint32_t>& mps, uint32_t maxSets)
	{
		std::vector<VkDescriptorPoolSize> pool_sizes;
		for (auto& [k, v] : mps)
		{
			pool_sizes.push_back({ (VkDescriptorType)k, v });
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
		if (vkCreateDescriptorPool(device, &descriptor_pool_create_info, nullptr, &ret) != VK_SUCCESS) {
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
		auto hr = vkAllocateDescriptorSets(device, &descriptor_set_allocate_info, dset.data());
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
			VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, outptr));
		}
		return count;
	}

	void dvk_device::free_cmd(VkCommandPool pool, VkCommandBuffer* pcbs, uint32_t count)
	{
		if (pool && pcbs && count > 0)
		{
			vkFreeCommandBuffers(device, pool, count, pcbs);
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
		VkAttachmentReference colorReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
		VkAttachmentReference depthReference = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
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
		VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassInfo, nullptr, &pass));
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
		auto gqs = graphics_queues.size();
		if (idx >= gqs)
		{
			idx = 0;
		}
		if (idx < gqs && gqs>0)
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
				p = new dvk_queue(this, compute_queues[idx], queueFamilyIndices.compute, dvk_queue_bit::COMPUTE_BIT);
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
		log_format("vkDeviceWaitIdle\n");
		vkDeviceWaitIdle(device);
	}

	VkPhysicalDeviceMemoryProperties* dvk_device::get_dmp()
	{
		return &_info->memoryProperties;
	}

	uint32_t dvk_device::get_device_memory_type(uint32_t image_bits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound)
	{
		uint32_t r = -1;
		VkBool32 b = false;
		if (!memTypeFound)
		{
			memTypeFound = &b;
		}
		*memTypeFound = false;
		auto& memoryProperties = _info->memoryProperties;
		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
		{
			if ((image_bits & 1) == 1)
			{
				if ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
				{
					if (memTypeFound)
					{
						*memTypeFound = true;
					}
					r = i;
					break;
				}
			}
			image_bits >>= 1;
		}

#if defined(__ANDROID__)
		//todo : Exceptions are disabled by default on Android (need to add LOCAL_CPP_FEATURES += exceptions to Android.mk), so for now just return zero

#endif
		return r;
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
		if (vkCreateShaderModule(device, &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS) {
			std::cout << "Could not create shader module from a \"" << filename << "\" file!" << std::endl;
			return VK_NULL_HANDLE;
		}
		return shader_module;
	}

	bool dvk_device::extensionSupported(const std::string& extension)
	{
		return supportedExtensions.find(extension) != supportedExtensions.end() || (instanceExtensions && instanceExtensions->find(extension) != instanceExtensions->end());
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
		_dev = dev->device;
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

	char* dvk_staging_buffer::map()
	{
		if (!mapped)
			VK_CHECK_RESULT(vkMapMemory(_dev, mem, 0, memSize, 0, (void**)&mapped));
		return (char*)mapped;
	}
	void  dvk_staging_buffer::unmap()
	{
		assert(mem);
		if (!mem)return;
		// 如果没有请求主机一致性，请手动刷新以使写入可见
		// If host coherency hasn't been requested, do a manual flush to make writes visible
		//if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
		{
			VkMappedMemoryRange mappedRange = {};
			mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			mappedRange.memory = mem;
			mappedRange.offset = 0;
			mappedRange.size = memSize;
			vkFlushMappedMemoryRanges(_dev, 1, &mappedRange);
		}
		vkUnmapMemory(_dev, mem);
	}

	void dvk_staging_buffer::copyToBuffer(void* data, size_t bsize)
	{
		// Copy texture data into staging buffer
		if (data)
		{
			VK_CHECK_RESULT(vkMapMemory(_dev, mem, 0, memSize, 0, (void**)&mapped));
			memcpy(mapped, data, bsize);
			// 如果没有请求主机一致性，请手动刷新以使写入可见
			// If host coherency hasn't been requested, do a manual flush to make writes visible
			//if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
			{
				VkMappedMemoryRange mappedRange = {};
				mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
				mappedRange.memory = mem;
				mappedRange.offset = 0;
				mappedRange.size = bsize;
				vkFlushMappedMemoryRanges(_dev, 1, &mappedRange);
			}
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
		uint8_t* data;
		VK_CHECK_RESULT(vkMapMemory(_dev, mem, 0, memSize, 0, (void**)&data));
		memcpy(outbuf, data, len);
		vkUnmapMemory(_dev, mem);
	}


	std::string format(const char* format1, ...)
	{
		va_list args;
		va_start(args, format1);
#if 0
		ndef _MSC_VER
			size_t size = std::snprintf(nullptr, 0, format, args) + 1; // Extra space for '\0'
		//MessageBuffer buf(size);
		std::string buf; buf.resize(size);
		std::vsnprintf(buf.data(), size, format, args);
		va_end(args);
		return buf;// std::string(buf.data(), buf.Data() + size - 1); // We don't want the '\0' inside
#else
		t_string buf;
		//MessageBuffer buf(size);
#ifdef _WIN32
		const size_t size = (size_t)_vscprintf(format1, args) + 1;
		buf.resize(size);
		vsnprintf_s(buf.data(), size, _TRUNCATE, format1, args);
#else
		buf.resize(2048);
		vsprintf(buf.data(), format1, args);
#endif
		va_end(args);
		return buf.c_str();
#endif
	}
	std::string log_format(const char* format1, ...)
	{
		va_list args;
		va_start(args, format1);
#if 0
		ndef _MSC_VER
			size_t size = std::snprintf(nullptr, 0, format, args) + 1; // Extra space for '\0'
		//MessageBuffer buf(size);
		std::string buf; buf.resize(size);
		std::vsnprintf(buf.data(), size, format, args);
		va_end(args);
		return buf;// std::string(buf.data(), buf.Data() + size - 1); // We don't want the '\0' inside
#else
		t_string buf;
		//MessageBuffer buf(size);
#ifdef _WIN32
		const size_t size = (size_t)_vscprintf(format1, args) + 1;
		buf.resize(size);
		vsnprintf_s(buf.data(), size, _TRUNCATE, format1, args);
#else
		buf.resize(2048);
		vsprintf(buf.data(), format1, args);
#endif
		va_end(args);
		dslog(buf.c_str());
		return buf.c_str();
#endif
	}

}
//!hz
#endif
