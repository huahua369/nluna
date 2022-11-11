
#ifdef _WIN32
#include <conio.h>

#endif
#include <thread>
//#include "rope.h"
//#include "anjn/anjn.h"
//#include <data/xlsx.h>
using namespace std::literals::chrono_literals;


#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
#include <ntype.h>
//#include "vkclass.h"
//#include "vk_Initializers.h"

#include <base/camera.h>
#include <data/json_helper.h>
#include "view_info.h"
#include "vk_cx.h"
#include "vk_cx1.h"
#include <base/promise_cx.h>
#include <base/Singleton.h>
#include <base/thread_pool.h>
using namespace std;
#include <vk_core/view.h>
#include <vk_core/bw_sdl.h>
#include <view/file.h>
#include <view/image.h>
#include <audio/audio.h>
#ifdef _WIN32
#include <platform/win32/DDSLoader.h>
#else

#endif
#include <base/print_time.h>

#include <shared.h>
#include "vkc.h"
namespace hz {
	std::string format(const char* format, ...);
	std::string log_format(const char* format, ...);
}
MC_EXPORT void dslog(const char* d, int n = 0);

// 内存泄漏检测
#ifdef _DEBUG
#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif
namespace hz {
	// todo vkimage info
#if 1
	//static VkImageLayout GetImageLayout(ImageLayoutBarrier target);
	static void ImagePipelineBarrier(VkCommandBuffer cmdBuffer, VkImage image, ImageLayoutBarrier source, ImageLayoutBarrier dest, const VkImageSubresourceRange& subresourceRange);
	static void SetImageBarrierInfo(ImageLayoutBarrier source, ImageLayoutBarrier dest, VkImageMemoryBarrier& inOutBarrier, VkPipelineStageFlags& inOutSourceStage, VkPipelineStageFlags& inOutDestStage);
	static VkPipelineStageFlags GetImageBarrierFlags(ImageLayoutBarrier target, VkAccessFlags& accessFlags, VkImageLayout& layout);

	void insertImageMemoryBarrier(
		VkCommandBuffer cmdbuffer,
		VkImage image,
		VkAccessFlags srcAccessMask,
		VkAccessFlags dstAccessMask,
		VkImageLayout oldImageLayout,
		VkImageLayout newImageLayout,
		VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask,
		VkImageSubresourceRange subresourceRange);

	VkBool32 getSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat* depthFormat);
	//毫秒
	uint64_t get_ms()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}
	//微秒
	uint64_t get_micro()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}
	//纳秒
	uint64_t get_nano()
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}
	//秒
	uint64_t get_seconds()
	{
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}
	//分
	uint64_t get_minutes()
	{
		return std::chrono::duration_cast<std::chrono::minutes>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}
	//时
	uint64_t get_hours()
	{
		return std::chrono::duration_cast<std::chrono::hours>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}
	uint64_t get_now()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}
	double get_secondsf()
	{
		double ret = get_micro();// get_ms();
		return ret * 0.001 * 0.001;
	}



	namespace cvk {
#ifdef _WIN32
		VkFormat TranslateDxgiFormatIntoVulkans(DXGI_FORMAT1 format);
#endif
	}
	namespace vkc {


		void setImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkImageSubresourceRange subresourceRange, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
		void setImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);

		VkSampler createSampler(dvk_device* dev, VkFilter filter, VkSamplerAddressMode addressMode);

		int64_t createImage(dvk_device* dev, VkImageCreateInfo* imageinfo, VkImageViewCreateInfo* viewinfo, dvk_texture* image, VkSampler* sampler, VkSamplerCreateInfo* info);

		VkCommandPool createCommandPool(VkDevice dev, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags);

		bool checkCommandBuffers(std::vector<VkCommandBuffer>& drawCmdBuffers);

		bool clearCommandBuffers(std::vector<VkCommandBuffer>& drawCmdBuffers);

		void createCommandBuffers(VkDevice dev, VkCommandPool commandPool, std::vector<VkCommandBuffer>& drawCmdBuffers);

		void destroyCommandBuffers(VkDevice dev, VkCommandPool commandPool, std::vector<VkCommandBuffer>& drawCmdBuffers);

		VkCommandBuffer createCommandBuffer1(VkDevice dev, VkCommandPool commandPool, VkCommandBufferLevel level, bool begin);
		bool flushCommandBuffer(VkDevice dev, VkCommandBuffer commandBuffer, VkCommandPool commandPool, VkQueue queue, bool free);

		VkPipelineShaderStageCreateInfo load_shader(dvk_device* dev, const char* data, size_t size, size_t stage, const char* pName = "main");

		uint32_t getMemoryType(dvk_device* dev, uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound);
	}
#endif
	int msg_box(const char* text, const char* caption, int mod = 0, void* hwnd = nullptr)
	{
		int ret = 0;
#ifdef _WIN32 
		ret = MessageBoxA((HWND)hwnd, text, caption, mod < 0 ? MB_OK : mod);
#endif
		return ret;
	}

	// todo gpu info
#if 1


	struct gpu_dev_inifo_t
	{
		VkPhysicalDeviceProperties pdp = {};
		int m = 0;
		VkPhysicalDevice pd = 0;
	};
	gpu_info::gpu_info()
	{
		//_gpu_infos = new std::vector<gpu_dev_inifo_t>();
	}
	gpu_info::~gpu_info()
	{
		//auto gis = (std::vector<gpu_dev_inifo_t>*)_gpu_infos;
		//if (gis)
		{
			//delete gis; _gpu_infos = nullptr;
		}
		for (auto& [k, v] : gpusname) { if (v.pdp)delete v.pdp; }
	}

	void gpu_info::init()
	{
		enum_gpus();
	}



	bool CheckExtensionAvailability(const char* extension_name, const std::vector<VkExtensionProperties>& available_extensions) {
		for (size_t i = 0; i < available_extensions.size(); ++i) {
			if (strcmp(available_extensions[i].extensionName, extension_name) == 0) {
				return true;
			}
		}
		return false;
	}
	bool CheckPhysicalDeviceProperties(VkPhysicalDevice physical_device, uint32_t& selected_graphics_queue_family_index
		, uint32_t& selected_present_queue_family_index, VkSurfaceKHR surface) {
		uint32_t extensions_count = 0;
		if ((vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensions_count, nullptr) != VK_SUCCESS) ||
			(extensions_count == 0)) {
			std::cout << "Error occurred during physical device " << physical_device << " extensions enumeration!" << std::endl;
			return false;
		}

		std::vector<VkExtensionProperties> available_extensions(extensions_count);
		if (vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensions_count, &available_extensions[0]) != VK_SUCCESS) {
			std::cout << "Error occurred during physical device " << physical_device << " extensions enumeration!" << std::endl;
			return false;
		}

		std::vector<const char*> device_extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		for (size_t i = 0; i < device_extensions.size(); ++i) {
			if (!CheckExtensionAvailability(device_extensions[i], available_extensions)) {
				std::cout << "Physical device " << physical_device << " doesn't support extension named \"" << device_extensions[i] << "\"!" << std::endl;
				return false;
			}
		}

		VkPhysicalDeviceProperties device_properties;
		VkPhysicalDeviceFeatures   device_features;

		vkGetPhysicalDeviceProperties(physical_device, &device_properties);
		vkGetPhysicalDeviceFeatures(physical_device, &device_features);

		uint32_t major_version = VK_VERSION_MAJOR(device_properties.apiVersion);
		//memcpy(maxComputeWorkGroupCount, device_properties.limits.maxComputeWorkGroupCount, sizeof(unsigned int) * 3);
		if ((major_version < 1) ||
			(device_properties.limits.maxImageDimension2D < 4096)) {
			std::cout << "Physical device " << physical_device << " doesn't support required parameters!" << std::endl;
			return false;
		}

		uint32_t queue_families_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, nullptr);
		if (queue_families_count == 0) {
			std::cout << "Physical device " << physical_device << " doesn't have any queue families!" << std::endl;
			return false;
		}
		std::vector<VkQueueFamilyProperties>  queue_family_properties(queue_families_count);
		std::vector<VkBool32>                 queue_present_support(queue_families_count);

		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, &queue_family_properties[0]);
		std::string kstr = std::to_string(queue_families_count);
#if 0
		uint32_t qfc2 = 0;
		std::vector<VkQueueFamilyProperties2> qfp2;
		vkGetPhysicalDeviceQueueFamilyProperties2(physical_device, &qfc2, 0);
		//VkVideoQueueFamilyProperties2KHR
		msg_box(kstr.c_str(), "vk", MB_OK);
		if (qfc2 > 0)
		{
			qfp2.resize(qfc2);
			vkGetPhysicalDeviceQueueFamilyProperties2(physical_device, &qfc2, qfp2.data());
		}
		int vd = -1;
		int i = 0;
		for (auto it : qfp2)
		{
			auto qf = it.queueFamilyProperties.queueFlags;
			if (qf & VK_QUEUE_VIDEO_DECODE_BIT_KHR)
			{
				vd = i;
				printf("VIDEO_DECODE\n");
			}
			i++;
		}
#endif
		//msg_box( kstr.c_str(), "vk", MB_OK);
		if (!surface)
			return true;

		uint32_t graphics_queue_family_index = UINT32_MAX;
		uint32_t present_queue_family_index = UINT32_MAX;
		for (uint32_t i = 0; i < queue_families_count; ++i) {
			vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &queue_present_support[i]);

			if ((queue_family_properties[i].queueCount > 0) &&
				(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
				// Select first queue that supports graphics
				if (graphics_queue_family_index == UINT32_MAX) {
					graphics_queue_family_index = i;
				}

				// If there is queue that supports both graphics and present - prefer it
				if (queue_present_support[i]) {
					selected_graphics_queue_family_index = i;
					selected_present_queue_family_index = i;
					return true;
				}
			}
		}

		// We don't have queue that supports both graphics and present so we have to use separate queues
		for (uint32_t i = 0; i < queue_families_count; ++i) {
			if (queue_present_support[i]) {
				present_queue_family_index = i;
				break;
			}
		}

		// If this device doesn't support queues with graphics and present capabilities don't use it
		if ((graphics_queue_family_index == UINT32_MAX) ||
			(present_queue_family_index == UINT32_MAX)) {
			std::cout << "Could not find queue families with required properties on physical device " << physical_device << "!" << std::endl;
			return false;
		}

		selected_graphics_queue_family_index = graphics_queue_family_index;
		selected_present_queue_family_index = present_queue_family_index;
		return true;
	}

	//枚举GPU
	int gpu_info::enum_gpus()
	{
		uint32_t num_devices = 0;
		if ((vkEnumeratePhysicalDevices(_instance, &num_devices, nullptr) != VK_SUCCESS) || (num_devices == 0))
		{
			//LOGI("Error occurred during physical devices enumeration!\n");
			return false;
		}
		VkPhysicalDevice g = 0, g1 = 0;
		//if (gpus.size())g = gpus[0];
		//gpus.clear();
		std::vector<VkPhysicalDevice> gpus;
		gpus.resize(num_devices);
		gpusname.clear();
		if (vkEnumeratePhysicalDevices(_instance, &num_devices, gpus.data()) != VK_SUCCESS)
		{
			//LOGI("Error occurred during physical devices enumeration!\n");
			return false;
		}
		g1 = gpus[0];
		uint32_t selected_graphics_queue_family_index = UINT32_MAX;
		uint32_t selected_present_queue_family_index = UINT32_MAX;

		std::vector<gpu_dev_inifo_t> gpu_infos;
		gpu_infos.resize(num_devices);
		// 优先级DISCRETE、INTEGRATED、VIRTUAL、CPU、OTHER
		int mt[5] = { 4, 1, 0, 2, 3 };
		/*
		VkPhysicalDeviceProperties.deviceType
		VK_PHYSICAL_DEVICE_TYPE_OTHER			其他
		VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU	集成
		VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU	独立
		VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU		虚拟
		VK_PHYSICAL_DEVICE_TYPE_CPU
		*/
		for (uint32_t i = 0; i < num_devices; ++i)
		{
			if (CheckPhysicalDeviceProperties(gpus[i], selected_graphics_queue_family_index, selected_present_queue_family_index, 0))
			{
				//查询显卡属性VkPhysicalDeviceProperties deviceProperties;
				vkGetPhysicalDeviceProperties(gpus[i], (VkPhysicalDeviceProperties*)&gpu_infos[i]);
				auto& gt = gpusname[gpu_infos[i].pdp.deviceName];
				gt.pdp = new VkPhysicalDeviceProperties(gpu_infos[i].pdp);
				gt.pd = gpus[i];
				gpu_infos[i].m = mt[gpu_infos[i].pdp.deviceType];
				gpu_infos[i].pd = gpus[i];
				//printf("%s\t\tmaxPushConstantsSize:%d\n", gpu_infos[i].deviceName, gpu_infos[i].limits.maxPushConstantsSize);
				//def_phydev.insert(std::pair<int, VkPhysicalDevice>(gpu_infos[i].m, gpus[i]));
			}
		}
		//msg_box( kstr.c_str(), "vk", MB_OK);
		std::sort(gpu_infos.begin(), gpu_infos.end(), [](const gpu_dev_inifo_t& d1, const gpu_dev_inifo_t& d2) { return d1.m < d2.m; });

		auto uboa0 = gpu_infos[0].pdp.limits.minUniformBufferOffsetAlignment;
		//auto uboa1 = gpu_infos[1].limits.minUniformBufferOffsetAlignment;
		//LOGI("vulkan GPU count:%d\n", (int)gpusname.size());
		printf("Available Vulkan devices\n");
		for (uint32_t i = 0; i < num_devices; i++) {
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(gpus[i], &deviceProperties);
			printf("Device [%d] : \n", i, deviceProperties.deviceName);
			// printf(" Type: %s" << physicalDeviceTypeString(deviceProperties.deviceType) << "\n";
			printf(" API: %d.%d.%d\n", (deviceProperties.apiVersion >> 22), ((deviceProperties.apiVersion >> 12) & 0x3ff), deviceProperties.apiVersion & 0xfff);
		}
		return num_devices;
	}
	//int gpu_info::get_push_constant_size(size_t idx)
	//{
	//	auto& gis = *((std::vector<gpu_dev_inifo_t>*)_gpu_infos);
	//	int ret = 0;
	//	int binding = 0;
	//	int binding_stride = 0;
	//	if (gis.size() && idx < gis.size())
	//	{
	//		auto& it = gis[idx].pdp;
	//		ret = it.limits.maxPushConstantsSize;
	//		binding = it.limits.maxVertexInputBindings;
	//		binding_stride = it.limits.maxVertexInputBindingStride;
	//	}
	//	return ret;
	//}

	//std::string gpu_info::get_name(size_t idx)
	//{
	//	auto& gis = *((std::vector<gpu_dev_inifo_t>*)_gpu_infos);
	//	return gis[idx].pdp.deviceName;
	//}

	std::string gpu_info::get_idx(const char* name, int& idx)
	{
		auto& gpn = gpusname;
		std::string ret;
		int i = 0;
		for (auto& [k, v] : gpn) {
			if (k.find(name) != std::string::npos)
			{
				ret = k;
				idx = i;
			}
			i++;
		}
		if (ret.empty())
		{
			ret = gpn.begin()->first;
		}
		return ret;
	}
	//std::vector<VkPhysicalDevice> gpu_info::get_physicals()
	//{
	//	auto& gis = *((std::vector<gpu_dev_inifo_t>*)_gpu_infos);
	//	std::vector<VkPhysicalDevice> ret;
	//	for (auto& it : gis)
	//	{
	//		ret.push_back(it.pd);
	//	}
	//	return ret;// gpus;
	//}

#endif // 1

	// todo dvk_cmd
#if 1
	dvk_cmd::dvk_cmd(dvk_device* d, VkCommandBuffer cb, bool is_compute) :_dev(d), cmdb(cb), _is_compute(is_compute)
	{
		bindPoint = uint32_t(!_is_compute ? VK_PIPELINE_BIND_POINT_GRAPHICS : VK_PIPELINE_BIND_POINT_COMPUTE);
	}

	dvk_cmd::~dvk_cmd()
	{
	}

	void dvk_cmd::begin()
	{
		if (is_begin) {
			return;
		}
		is_begin = true;
		VkCommandBufferBeginInfo cmdBufInfo = {};
		cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(cmdb, &cmdBufInfo);
	}

	void dvk_cmd::end()
	{
		if (!is_begin) {
			return;
		}
		is_begin = false;
		vkEndCommandBuffer(cmdb);
	}



	struct dvk_barrier
	{
		uint32_t memoryBarrierCount;
		const VkMemoryBarrier* pMemoryBarriers;
		uint32_t bufferMemoryBarrierCount;
		const VkBufferMemoryBarrier* pBufferMemoryBarriers;
		uint32_t imageMemoryBarrierCount;
		const VkImageMemoryBarrier* pImageMemoryBarriers;
	};


	void dvk_cmd::barrier_buffer(VkBuffer bufs, uint32_t size, int src_familyidx, int dst_familyidx, uint32_t srcStageMask, uint32_t dstStageMask, bool access_r2w)
	{
		auto dev = (dvk_device*)_dev;
		VkBufferMemoryBarrier bufferBarrier = {};
		bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		bufferBarrier.buffer = bufs;
		bufferBarrier.size = size;
		bufferBarrier.srcAccessMask = access_r2w ? VK_ACCESS_SHADER_READ_BIT : VK_ACCESS_SHADER_WRITE_BIT;
		bufferBarrier.dstAccessMask = access_r2w ? VK_ACCESS_SHADER_WRITE_BIT : VK_ACCESS_SHADER_READ_BIT;
		bufferBarrier.srcQueueFamilyIndex = dev->get_familyIndex(src_familyidx);
		bufferBarrier.dstQueueFamilyIndex = dev->get_familyIndex(dst_familyidx);

		dvk_barrier barrier = {};
		barrier.bufferMemoryBarrierCount = 1;
		barrier.pBufferMemoryBarriers = &bufferBarrier;
		vkCmdPipelineBarrier(
			cmdb,
			srcStageMask,//VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			dstStageMask,//VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			0,
			barrier.memoryBarrierCount, barrier.pMemoryBarriers,
			barrier.bufferMemoryBarrierCount, barrier.pBufferMemoryBarriers,
			barrier.imageMemoryBarrierCount, barrier.pImageMemoryBarriers
		);

	}
	void dvk_cmd::barrier_image(dvk_texture* img, int src_familyidx, int dst_familyidx
		, ImageLayoutBarrier oldLayout, ImageLayoutBarrier newLayout
		, bool access_r2w, uint32_t srcStageMask /*= 0*/, uint32_t dstStageMask /*= 0*/)
	{
		auto dev = (dvk_device*)_dev;
		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = img->mipLevels;
		subresourceRange.layerCount = 1;
		VkImageMemoryBarrier imgmem_barrier = {};
		imgmem_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		imgmem_barrier.image = img->_image;
		imgmem_barrier.subresourceRange = subresourceRange;
		imgmem_barrier.oldLayout = (VkImageLayout)GetImageLayout(oldLayout);//VK_IMAGE_LAYOUT_GENERAL
		imgmem_barrier.newLayout = (VkImageLayout)GetImageLayout(newLayout);
		if (!srcStageMask || !dstStageMask)
		{
			srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			dstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		}
		imgmem_barrier.srcAccessMask = access_r2w ? VK_ACCESS_SHADER_READ_BIT : VK_ACCESS_SHADER_WRITE_BIT;
		imgmem_barrier.dstAccessMask = access_r2w ? VK_ACCESS_SHADER_WRITE_BIT : VK_ACCESS_SHADER_READ_BIT;

		imgmem_barrier.srcQueueFamilyIndex = dev->get_familyIndex(src_familyidx);
		imgmem_barrier.dstQueueFamilyIndex = dev->get_familyIndex(dst_familyidx);

		dvk_barrier barrier = {};
		barrier.imageMemoryBarrierCount = 1;
		barrier.pImageMemoryBarriers = &imgmem_barrier;
		vkCmdPipelineBarrier(
			cmdb,
			srcStageMask,//VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			dstStageMask,//VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			0,
			barrier.memoryBarrierCount, barrier.pMemoryBarriers,
			barrier.bufferMemoryBarrierCount, barrier.pBufferMemoryBarriers,
			barrier.imageMemoryBarrierCount, barrier.pImageMemoryBarriers
		);

	}
	void dvk_cmd::barrier_mem_compute(bool src_write)
	{
		VkMemoryBarrier memoryBarrier = {};
		memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		memoryBarrier.srcAccessMask = src_write ? VK_ACCESS_SHADER_WRITE_BIT : VK_ACCESS_SHADER_READ_BIT;
		memoryBarrier.dstAccessMask = src_write ? VK_ACCESS_SHADER_READ_BIT : VK_ACCESS_SHADER_WRITE_BIT;

		dvk_barrier barrier = {};
		barrier.memoryBarrierCount = 1;
		barrier.pMemoryBarriers = &memoryBarrier;
		vkCmdPipelineBarrier(cmdb,
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, // srcStageMask
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, // dstStageMask
			0,
			barrier.memoryBarrierCount, barrier.pMemoryBarriers,
			barrier.bufferMemoryBarrierCount, barrier.pBufferMemoryBarriers,
			barrier.imageMemoryBarrierCount, barrier.pImageMemoryBarriers
		);
	}
	void dvk_cmd::blit_image(dvk_texture* dst, dvk_texture* src, bool is_linear)
	{
		int width = src->width, height = src->height;
		VkFilter filter = is_linear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
		VkImageBlit blit = {};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { (int)src->width, (int)src->height, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = src->mipLevels;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		//if (width > 1)width >>= 1;
		//if (height > 1)height >>= 1;
		blit.dstOffsets[1] = { (int)width, (int)height, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = dst->mipLevels;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(cmdb, src->_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst->_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, filter);
	}

	//void dvk_cmd::SetViewport(uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports)
	//{
	//	vkCmdSetViewport(cmdb, firstViewport, viewportCount, pViewports);
	//}
	//void dvk_cmd::SetScissor(uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors)
	//{
	//	vkCmdSetScissor(cmdb, firstScissor, scissorCount, pScissors);
	//}
	//void dvk_cmd::SetLineWidth(float lineWidth)
	//{
	//	vkCmdSetLineWidth(cmdb, lineWidth);
	//}
	void dvk_cmd::SetBlendConstants(const float blendConstants[4])
	{
		vkCmdSetBlendConstants(cmdb, blendConstants);
	}
	void dvk_cmd::SetDepthBiasBounds(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor, float minDepthBounds, float maxDepthBounds)
	{
		vkCmdSetDepthBias(cmdb, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
		vkCmdSetDepthBounds(cmdb, minDepthBounds, maxDepthBounds);
	}
	void dvk_cmd::SetStencilCompareMask(uint32_t faceMask, uint32_t compareMask)
	{
		vkCmdSetStencilCompareMask(cmdb, (VkStencilFaceFlags)faceMask, compareMask);
	}
	void dvk_cmd::SetStencilWriteMask(uint32_t faceMask, uint32_t writeMask)
	{
		vkCmdSetStencilWriteMask(cmdb, (VkStencilFaceFlags)faceMask, writeMask);
	}
	void dvk_cmd::SetStencilReference(uint32_t faceMask, uint32_t reference)
	{
		vkCmdSetStencilReference(cmdb, (VkStencilFaceFlags)faceMask, reference);
	}

	void dvk_cmd::set_viewport(const glm::vec4* s, uint32_t count, uint32_t first, float minDepth, float maxDepth)
	{
		if (count > 0)
		{
			std::vector<VkViewport> viewport(count);
			for (size_t i = 0; i < count; i++, s++)
			{
				auto& it = viewport[i];
				it.x = s->x;
				it.y = s->y;
				it.width = s->z;
				it.height = s->w;
				it.minDepth = minDepth;
				it.maxDepth = maxDepth;
			}
			vkCmdSetViewport(cmdb, first, count, viewport.data());
		}
	}

	void dvk_cmd::set_viewport(glm::ivec4 s, uint32_t first, float minDepth, float maxDepth)
	{
		VkViewport viewport[1];
		for (size_t i = 0; i < 1; i++, s++)
		{
			auto& it = viewport[i];
			it.x = s.x;
			it.y = s.y;
			it.width = s.z;
			it.height = s.w;
			it.minDepth = minDepth;
			it.maxDepth = maxDepth;
		}
		vkCmdSetViewport(cmdb, first, 1, viewport);
	}

	void dvk_cmd::set_scissor(const glm::vec4* pScissors, uint32_t count, uint32_t first)
	{
		if (count > 0)
		{
			std::vector<VkRect2D> scissor(count);
			for (size_t i = 0; i < count; i++, pScissors++)
			{
				auto& it = scissor[i];
				it.offset.x = pScissors->x;
				it.offset.y = pScissors->y;
				it.extent.width = pScissors->z;
				it.extent.height = pScissors->w;
			}
			vkCmdSetScissor(cmdb, first, count, scissor.data());
		}
	}

	void dvk_cmd::set_scissor(const glm::ivec4* pScissors, uint32_t first)
	{
		VkRect2D scissor[1] = {};
		auto& it = scissor[0];
		it.offset.x = pScissors->x;
		it.offset.y = pScissors->y;
		it.extent.width = pScissors->z;
		it.extent.height = pScissors->w;
		vkCmdSetScissor(cmdb, first, 1, scissor);
	}

	void dvk_cmd::set_scissor(const glm::ivec4& scissors, uint32_t first)
	{
		VkRect2D scissor[1] = {};
		auto& it = scissor[0];
		it.offset.x = scissors.x;
		it.offset.y = scissors.y;
		it.extent.width = scissors.z;
		it.extent.height = scissors.w;
		vkCmdSetScissor(cmdb, first, 1, scissor);
	}

	void dvk_cmd::set_state(uint32_t s, uint32_t cull_mod, uint32_t front_face, uint32_t primitive_topology)
	{
#if 1
		do
		{
			if (!_dev->ds_cb || !(s & (uint32_t)dynamic_state_e::enable))break;

			if (cull_mod < VK_CULL_MODE_FLAG_BITS_MAX_ENUM && (s & (uint32_t)dynamic_state_e::CullMode))
				_dev->ds_cb->CmdSetCullModeEXT(cmdb, cull_mod);
			//VK_FRONT_FACE_COUNTER_CLOCKWISE = 0,
			//VK_FRONT_FACE_CLOCKWISE = 1
			if (front_face < VK_FRONT_FACE_MAX_ENUM && (s & (uint32_t)dynamic_state_e::CullMode))
				_dev->ds_cb->CmdSetFrontFaceEXT(cmdb, (VkFrontFace)front_face);
			if (primitive_topology < VK_PRIMITIVE_TOPOLOGY_MAX_ENUM && (s & (uint32_t)dynamic_state_e::PrimitiveTopology))
				_dev->ds_cb->CmdSetPrimitiveTopologyEXT(cmdb, (VkPrimitiveTopology)primitive_topology);
			//vkCmdSetViewportWithCountEXT(cmdb); vkCmdSetScissorWithCountEXT(cmdb);
			//vkCmdBindVertexBuffers2EXT();
			_dev->ds_cb->CmdSetDepthTestEnableEXT(cmdb, (s & (uint32_t)dynamic_state_e::DepthTest));
			_dev->ds_cb->CmdSetDepthWriteEnableEXT(cmdb, (s & (uint32_t)dynamic_state_e::DepthWrite));
			_dev->ds_cb->CmdSetStencilTestEnableEXT(cmdb, (s & (uint32_t)dynamic_state_e::StencilTest));

		} while (0);
#endif
	}

	void dvk_cmd::bind_pipeline(shader_info* pipeline, bool depthTestEnable, bool depthWriteEnable)
	{
		_pipeline = pipeline;
		auto pipe = _pipeline->get_vkpipe(depthTestEnable, depthWriteEnable);
		vkCmdBindPipeline(cmdb, (VkPipelineBindPoint)bindPoint, pipe);

		if (_dev->ds_cb)
		{
			_dev->ds_cb->CmdSetDepthTestEnableEXT(cmdb, depthTestEnable ? VK_TRUE : VK_FALSE);
			_dev->ds_cb->CmdSetDepthWriteEnableEXT(cmdb, depthWriteEnable ? VK_TRUE : VK_FALSE);
		}
	}

	//void dvk_cmd::bind_set(dvk_set* dsp, uint32_t first_set)
	//{
	//	assert(_pipeline);
	//	auto dsptr = (VkDescriptorSet*)dsp->descriptorSets.data();
	//	VkPipelineLayout layout = _pipeline->get_pipelineLayout();
	//	uint32_t* dynOffsets = dsp->dynamicOffsets.data();
	//	uint32_t dynamicOffsetCount = dsp->dynamicOffsets.size();
	//	vkCmdBindDescriptorSets(cmdb, (VkPipelineBindPoint)bindPoint, layout, first_set, dsp->size(), dsptr, dynamicOffsetCount, dynOffsets);
	//}
	void dvk_cmd::bind_set(const VkDescriptorSet* sets, uint32_t count, uint32_t first_set, std::vector<uint32_t>* offsets) const
	{
		if (!sets)
			return;
		VkPipelineLayout layout = _pipeline->get_pipelineLayout();
		int dynamic_count = _pipeline->isDynamic(first_set, count);
		uint32_t offsetCount = dynamic_count;
		std::vector<uint32_t> toffsets;
		if (dynamic_count > 0)
		{
			if (!offsets)
			{
				offsets = &toffsets;
			}
			if (offsets->size() < dynamic_count)
			{
				offsets->resize(dynamic_count);
			}
		}
		const uint32_t* offsetData = (dynamic_count > 0 ? offsets->data() : nullptr);
		vkCmdBindDescriptorSets(cmdb, (VkPipelineBindPoint)bindPoint, layout, first_set, count, sets, offsetCount, offsetData);
	}

	void dvk_cmd::bind_set(const VkDescriptorSet* sets, uint32_t count, uint32_t first_set, uint32_t offsets) const
	{
		if (!sets)
			return;
		VkPipelineLayout layout = _pipeline->get_pipelineLayout();
		int dynamic_count = _pipeline->isDynamic(first_set, count);
		//assert(dynamic_count == 1);
		if (dynamic_count < 2)
		{
			uint32_t offsetCount = dynamic_count > 0 ? 1 : 0;
			const uint32_t* offsetData = (dynamic_count > 0 ? &offsets : nullptr);
			vkCmdBindDescriptorSets(cmdb, (VkPipelineBindPoint)bindPoint, layout, first_set, count, sets, offsetCount, offsetData);
		}
	}
	void dvk_cmd::bind_set(VkDescriptorSet sets, uint32_t first_set, uint32_t offsets) const
	{
		if (!sets)
			return;
		VkPipelineLayout layout = _pipeline->get_pipelineLayout();
		int dynamic_count = _pipeline->isDynamic(first_set, 1);
		//assert(dynamic_count == 1);
		uint32_t* offsetData = 0;
		uint32_t offsetCount = 0;
		if (dynamic_count > 0)
		{
			offsetCount = 1;
			offsetData = &offsets;
		}
		vkCmdBindDescriptorSets(cmdb, (VkPipelineBindPoint)bindPoint, layout, first_set, 1, &sets, offsetCount, offsetData);
	}
	void dvk_cmd::bind_set(VkWriteDescriptorSet* pds, uint32_t count, uint32_t setidx)
	{
		VkPipelineLayout layout = _pipeline->get_pipelineLayout();
		int dynamic_count = _pipeline->isDynamic(0, 1);
		//std::array<VkWriteDescriptorSet, 3> writeDescriptorSets{};

		//// Scene matrices
		//writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		//writeDescriptorSets[0].dstSet = 0;
		//writeDescriptorSets[0].dstBinding = 0;
		//writeDescriptorSets[0].descriptorCount = 1;
		//writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		//writeDescriptorSets[0].pBufferInfo = &uniformBuffers.scene.descriptor;

		//// Model matrices
		//writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		//writeDescriptorSets[1].dstSet = 0;
		//writeDescriptorSets[1].dstBinding = 1;
		//writeDescriptorSets[1].descriptorCount = 1;
		//writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		//writeDescriptorSets[1].pBufferInfo = &cube.uniformBuffer.descriptor;

		//// Texture
		//writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		//writeDescriptorSets[2].dstSet = 0;
		//writeDescriptorSets[2].dstBinding = 2;
		//writeDescriptorSets[2].descriptorCount = 1;
		//writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		//writeDescriptorSets[2].pImageInfo = &cube.texture.descriptor;

		PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR = {};
		VkPhysicalDevicePushDescriptorPropertiesKHR pushDescriptorProps{};
		vkCmdPushDescriptorSetKHR(cmdb, (VkPipelineBindPoint)bindPoint, layout, setidx, count, pds);

	}
	void dvk_cmd::bind_set(ubo_set_t* ubo) const
	{
		if (ubo && ubo->dset)
		{
			bind_set(ubo->dset, ubo->first_set, ubo->offsets);
		}
	}

	void dvk_cmd::bind_set(ubo_set_t ubo) const
	{
		if (ubo.dset)
		{
			bind_set(ubo.dset, ubo.first_set, ubo.offsets);
		}
	}

	void dvk_cmd::bind_ibo(VkBuffer buffer, VkDeviceSize offset, uint8_t index_type)
	{
		assert(index_type == 0 || index_type == 1);
		vkCmdBindIndexBuffer(cmdb, buffer, offset, (VkIndexType)index_type);
	}

	void dvk_cmd::bind_ibo(VkBuffer buffer, VkDeviceSize offset, IBO_type index_type)
	{
		vkCmdBindIndexBuffer(cmdb, buffer, offset, (VkIndexType)index_type);
	}

	void dvk_cmd::bind_ibo16(VkBuffer buffer, VkDeviceSize offset)
	{
		vkCmdBindIndexBuffer(cmdb, buffer, offset, VkIndexType::VK_INDEX_TYPE_UINT16);
	}

	void dvk_cmd::bind_ibo32(VkBuffer buffer, VkDeviceSize offset)
	{
		vkCmdBindIndexBuffer(cmdb, buffer, offset, VkIndexType::VK_INDEX_TYPE_UINT32);
	}

	void dvk_cmd::bind_vbos(uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets)
	{
		vkCmdBindVertexBuffers(cmdb, firstBinding, bindingCount, pBuffers, pOffsets);
	}

	void dvk_cmd::bind_vbo(VkBuffer pBuffer, VkDeviceSize offset, uint32_t firstBinding)
	{
		VkDeviceSize pOffsets[] = { offset };
		vkCmdBindVertexBuffers(cmdb, firstBinding, 1, &pBuffer, pOffsets);
	}

	void dvk_cmd::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
	{
		if (vertexCount > 0)
			vkCmdDraw(cmdb, vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void dvk_cmd::draw_indexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
	{
		if (indexCount > 0)
			vkCmdDrawIndexed(cmdb, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

	void dvk_cmd::draw(uint32_t vertexCount, uint32_t firstVertex)
	{
		if (vertexCount > 0)
			vkCmdDraw(cmdb, vertexCount, 1, firstVertex, 0);
	}

	void dvk_cmd::draw_indexed(uint32_t indexCount, uint32_t firstIndex, int32_t vertexOffset)
	{
		if (indexCount > 0)
			vkCmdDrawIndexed(cmdb, indexCount, 1, firstIndex, vertexOffset, 0);
	}

	void dvk_cmd::dispatch(uint32_t gcx, uint32_t gcy, uint32_t gcz)
	{
		vkCmdDispatch(cmdb, gcx, gcy, gcz);

	}

	void dvk_cmd::dispatchIndirect(VkBuffer buffer, VkDeviceSize offset)
	{
		vkCmdDispatchIndirect(cmdb, buffer, offset);
	}

	void dvk_cmd::exe_cmds(const VkCommandBuffer* pCommandBuffers, uint32_t count)
	{
		if (count && pCommandBuffers)
			vkCmdExecuteCommands(cmdb, count, pCommandBuffers);
	}

#endif
#if 1
	class easy_cmd_cx
	{
	public:
		dvk_queue* qctx = 0;
		VkQueue q = 0;
		VkDevice device = 0;
		dvk_cmd_pool* cp = 0;
		VkCommandBuffer vcb = 0;
		VkFence fence = 0;
		bool is_begin = false;
	public:
		easy_cmd_cx(dvk_device* dev);
		~easy_cmd_cx();
		void begin();
		void end();
		bool submit();
		VkCommandBuffer get();
	private:

	};

	VkCommandBuffer easy_cmd_cx::get() { return vcb; };
	easy_cmd_cx::easy_cmd_cx(dvk_device* dev)
	{
		// 获取0号图形列队
		qctx = dev->get_graphics_queue(1);
		cp = qctx->new_cmd_pool();
		q = qctx->get_vkptr();
		device = dev->device;
		vcb = vkc::createCommandBuffer1(dev->device, cp->command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

		// Create fence to ensure that the command buffer has finished executing
		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = 0;

		(vkCreateFence(device, &fenceInfo, nullptr, &fence));

	}

	easy_cmd_cx::~easy_cmd_cx()
	{
		vkFreeCommandBuffers(device, cp->command_pool, 1, &vcb);
		qctx->free_cmd_pool(cp);
		vkDestroyFence(device, fence, nullptr); fence = 0;
	}
	void easy_cmd_cx::begin()
	{
		if (is_begin)return;
		VkCommandBufferBeginInfo cmdBufInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, 0, 0, 0 };
		cmdBufInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		(vkBeginCommandBuffer(vcb, &cmdBufInfo));
	}
	void easy_cmd_cx::end()
	{
		if (is_begin)
		{
			is_begin = false;
			(vkEndCommandBuffer(vcb));
		}
	}

	bool easy_cmd_cx::submit()
	{
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &vcb;

		// Submit to the queue
		auto hr = (vkQueueSubmit(q, 1, &submitInfo, fence));
		if (hr == VK_SUCCESS)
		{
			// Wait for the fence to signal that command buffer has finished executing
			(vkWaitForFences(device, 1, &fence, VK_TRUE, 2000));
		}
		return hr == VK_SUCCESS;
	}

	bool memory_type_from_properties(VkPhysicalDeviceMemoryProperties* memory_properties, uint32_t typeBits, VkFlags requirements_mask, uint32_t* typeIndex) {
		// Search memtypes to find first index with those properties
		for (uint32_t i = 0; i < memory_properties->memoryTypeCount; i++) {
			if ((typeBits & 1) == 1) {
				// Type is available, does it match user properties?
				if ((memory_properties->memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
					*typeIndex = i;
					return true;
				}
			}
			typeBits >>= 1;
		}
		// No memory types matched, return failure
		return false;
	}

	upload_cx::upload_cx()
	{
	}

	upload_cx::~upload_cx()
	{
		delop(db);
	}
	void upload_cx::init(dvk_device* dev, size_t size, int idxqueue)
	{
		if (_dev)return;
		_dev = dev;
		assert(_dev);
		ncap = size;
		VkResult res;
		db = new dynamic_buffer_cx(dev, size, false, true);

		_queue = _dev->get_graphics_queue(idxqueue)->get_vkptr();
		// Create command list and allocators 
		{
			VkCommandPoolCreateInfo cmd_pool_info = {};
			cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			cmd_pool_info.queueFamilyIndex = _dev->get_familyIndex(0);// GetGraphicsQueueFamilyIndex();
			cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			res = vkCreateCommandPool(_dev->device, &cmd_pool_info, NULL, &_commandPool);
			assert(res == VK_SUCCESS);

			VkCommandBufferAllocateInfo cmd = {};
			cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			cmd.pNext = NULL;
			cmd.commandPool = _commandPool;
			cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			cmd.commandBufferCount = 1;
			res = vkAllocateCommandBuffers(_dev->device, &cmd, &_pCommandBuffer);
			assert(res == VK_SUCCESS);
		}
		// Create fence
		{
			VkFenceCreateInfo fence_ci = {};
			fence_ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			res = vkCreateFence(_dev->device, &fence_ci, NULL, &_fence);
			assert(res == VK_SUCCESS);
		}

		{
			//VkCommandBufferBeginInfo cmd_buf_info = {};
			//cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			//res = vkBeginCommandBuffer(_pCommandBuffer, &cmd_buf_info);
			//assert(res == VK_SUCCESS);
		}
	}
	void upload_cx::on_destroy()
	{
		delop(db);
		//vkDestroyBuffer(_dev->device, _buffer, NULL);
		//vkUnmapMemory(_dev->device, _deviceMemory);
		//vkFreeMemory(_dev->device, _deviceMemory, NULL);

		vkFreeCommandBuffers(_dev->device, _commandPool, 1, &_pCommandBuffer);
		vkDestroyCommandPool(_dev->device, _commandPool, NULL);

		vkDestroyFence(_dev->device, _fence, NULL);
	}
	char* upload_cx::get_tbuf(size_t size, size_t uAlign)
	{
		auto ns = ncap - (last_pos + last_size);
		flush();
		if (!uAlign)uAlign = 512;
		size = AlignUp(size, uAlign);
		if (ns < size)
		{
			flushAndFinish();
		}
		if (ncap < size)
		{
			db->append(size - ncap);
		}
		uint32_t ps = 0;
		auto d = db->alloc(size, &ps);
		last_pos = ps;
		last_size = size;
		ups = size;
		return d;
	}
	void upload_cx::AddPreBarrier(VkImageMemoryBarrier imb)
	{
		toPreBarrier.push_back(imb);
	}
	void upload_cx::addCopy(VkImage image, VkBufferImageCopy bic)
	{
		_copies.push_back({ image,bic });
	}
	void upload_cx::AddPostBarrier(VkImageMemoryBarrier imb)
	{
		toPostBarrier.push_back(imb);
	}
	void upload_cx::flush()
	{
		if (ups > 0)
		{
			db->flush(last_pos, last_size); ups = 0;
		}
	}
	int upload_cx::flushAndFinish(int wait_time)
	{
		if (toPreBarrier.empty())return 0;
		// Reset so it can be reused
		flush();	// 刷新数据缓存
		auto cbf = get_cmdbuf();
		auto bf = get_resource();
		VkResult res;

		vkResetFences(_dev->device, 1, &_fence);

		VkCommandBufferBeginInfo cmd_buf_info = {};
		cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		res = vkBeginCommandBuffer(_pCommandBuffer, &cmd_buf_info);
		assert(res == VK_SUCCESS);

		if (toPreBarrier.size() > 0)
		{
			vkCmdPipelineBarrier(cbf, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT
				, 0, 0, NULL, 0, NULL
				, (uint32_t)toPreBarrier.size(), toPreBarrier.data());

		}

		for (auto c : _copies)
		{
			vkCmdCopyBufferToImage(cbf, bf, c._image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				, 1, &c._bic);
		}

		//apply post barriers in one go
		if (toPostBarrier.size() > 0)
		{
			vkCmdPipelineBarrier(cbf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, 0, 0, NULL, 0, NULL
				, (uint32_t)toPostBarrier.size(), toPostBarrier.data());
		}
		// Close 
		res = vkEndCommandBuffer(_pCommandBuffer);
		assert(res == VK_SUCCESS);

		// Submit
		const VkCommandBuffer cmd_bufs[] = { _pCommandBuffer };
		VkSubmitInfo submit_info;
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.pNext = NULL;
		submit_info.waitSemaphoreCount = 0;
		submit_info.pWaitSemaphores = NULL;
		submit_info.pWaitDstStageMask = NULL;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = cmd_bufs;
		submit_info.signalSemaphoreCount = 0;
		submit_info.pSignalSemaphores = NULL;
		res = vkQueueSubmit(_queue, 1, &submit_info, _fence);
		int r = 0;
		do {
			assert(res == VK_SUCCESS);
			toPreBarrier.clear();
			_copies.clear();
			toPostBarrier.clear();
			if (res != VK_SUCCESS)
			{
				r = -6;
				break;
			}
			// 等待GPU返回
			if(isprint)
			{
				print_time Pt("vkQueueSubmit upload");
				res = vkWaitForFences(_dev->device, 1, &_fence, VK_TRUE, wait_time);
				assert(res == VK_SUCCESS);
				if (res != VK_SUCCESS)
				{
					r = -7;
					break;
				}
			}
			else {
				res = vkWaitForFences(_dev->device, 1, &_fence, VK_TRUE, wait_time);
				assert(res == VK_SUCCESS);
				if (res != VK_SUCCESS)
				{
					r = -7;
					break;
				}

			}

		} while (0);
		db->clear();
		last_size = last_pos = ups = 0;
		return res;
	}
	void upload_cx::free_buf()
	{
		ncap = 0;
		db->free_mem();
	}

	VkBuffer upload_cx::get_resource() { return db->_ubo->buffer; }
	VkCommandBuffer upload_cx::get_cmdbuf() { return _pCommandBuffer; }

	void upload_cx::add_pre(VkImage image, VkFormat format, uint32_t aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevel, uint32_t layerCount)
	{
		VkImageMemoryBarrier copy_barrier = {};
		copy_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		copy_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		copy_barrier.oldLayout = oldLayout;
		copy_barrier.newLayout = newLayout;
		copy_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		copy_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		copy_barrier.image = image;
		copy_barrier.subresourceRange.aspectMask = aspectMask;
		copy_barrier.subresourceRange.levelCount = mipLevel;
		copy_barrier.subresourceRange.layerCount = layerCount;

		toPreBarrier.push_back(copy_barrier);
	}

	void upload_cx::add_post(VkImage image, VkFormat format, uint32_t aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevel, uint32_t layerCount)
	{
		VkImageMemoryBarrier use_barrier = {};
		use_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		use_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		use_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		use_barrier.oldLayout = oldLayout;
		use_barrier.newLayout = newLayout;
		use_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		use_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		use_barrier.image = image;
		use_barrier.subresourceRange.aspectMask = aspectMask;
		use_barrier.subresourceRange.levelCount = mipLevel;
		use_barrier.subresourceRange.layerCount = layerCount;
		toPostBarrier.push_back(use_barrier);
	}





	// todo texture

	dvk_texture::dvk_texture()
	{
		descriptor = new VkDescriptorImageInfo();
		_info = new VkImageCreateInfo();
		//_header = new IMG_INFO();
	}

	dvk_texture::dvk_texture(dvk_device* dev) :_dev(dev)
	{
		descriptor = new VkDescriptorImageInfo();
		_info = new VkImageCreateInfo();
	}

	dvk_texture::~dvk_texture()
	{
		if (ycbcr_sampler_conversion)
		{
			vkDestroySamplerYcbcrConversion(_dev->device, ycbcr_sampler_conversion, 0);
		}
		free_image();
		//delop(_header);
		delop(_info);
		delop(descriptor);
	}

	void dvk_texture::OnDestroy()
	{
#ifdef USE_VMA
		if (m_pResource != VK_NULL_HANDLE)
		{
			vmaDestroyImage(m_pDevice->GetAllocator(), m_pResource, m_ImageAlloc);
			m_pResource = VK_NULL_HANDLE;
		}
#else
		if (image_memory != VK_NULL_HANDLE)
		{
			vkDestroyImage(_dev->device, _image, nullptr);
			vkFreeMemory(_dev->device, image_memory, nullptr);
			_image = VK_NULL_HANDLE;
			image_memory = VK_NULL_HANDLE;
		}
#endif

	}
	void dvk_texture::free_image()
	{
		if (_dev)
		{
			if (_view)
				vkDestroyImageView(_dev->device, _view, nullptr);
			if (_image)
				vkDestroyImage(_dev->device, _image, nullptr);
			if (sampler)
				vkDestroySampler(_dev->device, sampler, nullptr);
			if (image_memory)
				vkFreeMemory(_dev->device, image_memory, nullptr);
		}
		_view = nullptr;
		_image = nullptr;
		image_memory = nullptr;
		sampler = nullptr;
	}





	//dvk_texture* dvk_texture::new2d_priv(const std::string& filename, upload_cx*up, uint64_t cq, ImageLayoutBarrier imageLayout, VkImageUsageFlags imageUsageFlags)
	dvk_texture* dvk_texture::new2d_priv(const std::string& filename, upload_cx* up, ImageLayoutBarrier imageLayout, uint32_t imageUsageFlags)
	{
		VkImageLayout il = (VkImageLayout)GetImageLayout(imageLayout);
		std::vector<char> data;
		File::read_binary_file(filename.c_str(), data);
		if (data.empty())
		{
			return nullptr;
		}
		int comp = 0;
		int width = 0;
		int height = 0;
		uint8_t* pxdata = stbi_load_from_memory((stbi_uc*)data.data(), data.size(), &width, &height, &comp, 4);
		//float* pxdata1 = stbi_loadf_from_memory((stbi_uc*)data.data(), data.size(), &width, &height, &comp, 4);
		//float pxf[128] = {};
		//memcpy(pxf, pxdata1, sizeof(float) * 100);
		if (!pxdata)
		{
			return nullptr;
		}
		auto tex = new dvk_texture(up->_dev);
		if (tex)
		{
			//tex->upload_data();
			//tex->set_data(pxdata, (uint64_t)width * height * 4, VK_FORMAT_R8G8B8A8_UNORM, width, height, up, imageUsageFlags, il);
		}

		stbi_image_free(pxdata);
		return tex;
	}

	int dvk_texture::Init(dvk_device* pDevice, VkImageCreateInfo* pCreateInfo, const char* name)
	{
		_dev = pDevice;
		mipLevels = pCreateInfo->mipLevels;
		width = pCreateInfo->extent.width;
		height = pCreateInfo->extent.height;
		_depth = pCreateInfo->extent.depth;
		layerCount = pCreateInfo->arrayLayers;
		_format = pCreateInfo->format;
		*_info = *pCreateInfo;
		if (name)
			_name = name;

#ifdef USE_VMA
		VmaAllocationCreateInfo imageAllocCreateInfo = {};
		imageAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		imageAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT;
		imageAllocCreateInfo.pUserData = (void*)m_name.c_str();
		VmaAllocationInfo gpuImageAllocInfo = {};
		VkResult res = vmaCreateImage(m_pDevice->GetAllocator(), pCreateInfo, &imageAllocCreateInfo, &_image, &m_ImageAlloc, &gpuImageAllocInfo);
		assert(res == VK_SUCCESS);
		SetResourceName(pDevice->device, VK_OBJECT_TYPE_IMAGE, (uint64_t)_image, m_name.c_str());
#else
		/* Create image */
		VkResult res = vkCreateImage(_dev->device, pCreateInfo, NULL, &_image);
		assert(res == VK_SUCCESS);

		VkMemoryRequirements mem_reqs;
		vkGetImageMemoryRequirements(_dev->device, _image, &mem_reqs);

		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.pNext = NULL;
		alloc_info.allocationSize = 0;
		alloc_info.allocationSize = mem_reqs.size;
		alloc_info.memoryTypeIndex = 0;

		bool pass = memory_type_from_properties(_dev->get_dmp(), mem_reqs.memoryTypeBits,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&alloc_info.memoryTypeIndex);
		assert(pass);

		/* Allocate memory */
		res = vkAllocateMemory(_dev->device, &alloc_info, NULL, &image_memory);
		assert(res == VK_SUCCESS);

		/* bind memory */
		res = vkBindImageMemory(_dev->device, _image, image_memory, 0);
		assert(res == VK_SUCCESS);
#endif
		return 0;
	}

	int dvk_texture::InitRenderTarget(dvk_device* pDevice, uint32_t width, uint32_t height, VkFormat format, VkSampleCountFlagBits msaa, VkImageUsageFlags usage, bool bUAV, const char* name, VkImageCreateFlagBits flags)
	{
		VkImageCreateInfo image_info = {};
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.pNext = NULL;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.format = format;
		image_info.extent.width = width;
		image_info.extent.height = height;
		image_info.extent.depth = 1;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;
		image_info.samples = msaa;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.queueFamilyIndexCount = 0;
		image_info.pQueueFamilyIndices = NULL;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_info.usage = usage; //TODO    
		image_info.flags = flags;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;   // VK_IMAGE_TILING_LINEAR should never be used and will never be faster

		return Init(pDevice, &image_info, name);
	}

	VkImageView dvk_texture::CreateRTV(VkImageView* pImageView, int mipLevel, VkFormat format)
	{
		if (!pImageView)pImageView = &_view;
		VkImageViewCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.image = _image;
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		if (_info->arrayLayers > 1)
		{
			info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
			info.subresourceRange.layerCount = _info->arrayLayers;
		}
		else
		{
			info.subresourceRange.layerCount = 1;
		}
		if (format == VK_FORMAT_UNDEFINED)
			info.format = (VkFormat)_format;
		else
			info.format = format;
		if (_format == VK_FORMAT_D32_SFLOAT)
			info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		else
			info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		std::string ResourceName = _name.c_str();

		if (mipLevel == -1)
		{
			info.subresourceRange.baseMipLevel = 0;
			info.subresourceRange.levelCount = _info->mipLevels;
		}
		else
		{
			info.subresourceRange.baseMipLevel = mipLevel;
			info.subresourceRange.levelCount = 1;
			ResourceName += std::to_string(mipLevel);
		}

		info.subresourceRange.baseArrayLayer = 0;
		VkResult res = vkCreateImageView(_dev->device, &info, NULL, pImageView);
		assert(res == VK_SUCCESS);

		//SetResourceName(_dev->device, VK_OBJECT_TYPE_IMAGE_VIEW, (uint64_t)*pImageView, ResourceName.c_str());
		return *pImageView;
	}

	VkImageView dvk_texture::CreateSRV(VkImageView* pImageView, int mipLevel)
	{
		if (!pImageView)pImageView = &_view;
		VkImageViewCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.image = _image;
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		if (_info->arrayLayers > 1)
		{
			info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
			info.subresourceRange.layerCount = _info->arrayLayers;
		}
		else
		{
			info.subresourceRange.layerCount = 1;
		}
		info.format = (VkFormat)_format;
		if (_format == VK_FORMAT_D32_SFLOAT)
			info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		else
			info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		if (mipLevel == -1)
		{
			info.subresourceRange.baseMipLevel = 0;
			info.subresourceRange.levelCount = _info->mipLevels;
		}
		else
		{
			info.subresourceRange.baseMipLevel = mipLevel;
			info.subresourceRange.levelCount = 1;
		}

		info.subresourceRange.baseArrayLayer = 0;

		VkResult res = vkCreateImageView(_dev->device, &info, NULL, pImageView);
		assert(res == VK_SUCCESS);

		//SetResourceName(_dev->device, VK_OBJECT_TYPE_IMAGE_VIEW, (uint64_t)*pImageView, m_name.c_str());
		return *pImageView;
	}

	VkImageView dvk_texture::CreateCubeSRV(VkImageView* pImageView)
	{
		if (!pImageView)pImageView = &_view;
		VkImageViewCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.image = _image;
		info.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		info.format = (VkFormat)_format;
		info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		info.subresourceRange.baseMipLevel = 0;
		info.subresourceRange.levelCount = _info->mipLevels;
		info.subresourceRange.baseArrayLayer = 0;
		info.subresourceRange.layerCount = _info->arrayLayers;

		VkResult res = vkCreateImageView(_dev->device, &info, NULL, pImageView);
		assert(res == VK_SUCCESS);

		//SetResourceName(_dev->device, VK_OBJECT_TYPE_IMAGE_VIEW, (uint64_t)*pImageView, m_name.c_str());
		return *pImageView;
	}

	VkImageView dvk_texture::CreateDSV(VkImageView* pImageView)
	{
		if (!pImageView)pImageView = &_view;
		VkImageViewCreateInfo view_info = {};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.pNext = NULL;
		view_info.image = _image;
		view_info.format = (VkFormat)_format;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;

		if (_format == VK_FORMAT_D16_UNORM_S8_UINT || _format == VK_FORMAT_D24_UNORM_S8_UINT || _format == VK_FORMAT_D32_SFLOAT_S8_UINT)
		{
			view_info.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}

		_info->mipLevels = 1;

		VkResult res = vkCreateImageView(_dev->device, &view_info, NULL, pImageView);
		assert(res == VK_SUCCESS);

		//SetResourceName(_dev->device, VK_OBJECT_TYPE_IMAGE_VIEW, (uint64_t)*pImageView, m_name.c_str());
		return *pImageView;
	}

	int dvk_texture::InitDepthStencil(dvk_device* pDevice, uint32_t width, uint32_t height, VkFormat format, VkSampleCountFlagBits msaa, const char* name)
	{
		VkImageCreateInfo image_info = {};
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.pNext = NULL;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.format = format;
		image_info.extent.width = width;
		image_info.extent.height = height;
		image_info.extent.depth = 1;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;
		image_info.samples = msaa;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.queueFamilyIndexCount = 0;
		image_info.pQueueFamilyIndices = NULL;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT; //TODO    
		image_info.flags = 0;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;   // VK_IMAGE_TILING_LINEAR should never be used and will never be faster

		return Init(pDevice, &image_info, name);
	}

	//--------------------------------------------------------------------------------------
	// create a comitted resource using m_header
	//--------------------------------------------------------------------------------------
	VkImage dvk_texture::CreateTextureCommitted(upload_cx* pUploadHeap, const char* pName, bool useSRGB, VkImageUsageFlags usageFlags)
	{
		VkImageCreateInfo info = {};
		if (!_dev)_dev = pUploadHeap->_dev;
		if (pUploadHeap && _header) {
			dvk_device* pDevice = pUploadHeap->_dev;
			if (pName)
				_name = pName;

#ifdef _WIN32
			if (useSRGB && ((usageFlags & VK_IMAGE_USAGE_STORAGE_BIT) != 0))
			{
				// the storage bit is not supported for srgb formats
				// we can still use the srgb format on an image view if the access is read-only
				// for write access, we need to use an image view with unorm format
				// this is ok as srgb and unorm formats are compatible with each other
				VkImageFormatListCreateInfo formatListInfo = {};
				formatListInfo.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO;
				formatListInfo.viewFormatCount = 2;
				VkFormat list[2];
				list[0] = cvk::TranslateDxgiFormatIntoVulkans(_header->format);
				list[1] = cvk::TranslateDxgiFormatIntoVulkans(SetFormatGamma(_header->format, useSRGB));
				formatListInfo.pViewFormats = list;
				info.pNext = &formatListInfo;
			}
			else {
				_header->format = SetFormatGamma(_header->format, useSRGB);
			}
			if (_header->format)
				_format = cvk::TranslateDxgiFormatIntoVulkans((DXGI_FORMAT)_header->format);

#endif
			if (!_header->format && !_header->vkformat)
				_header->vkformat = useSRGB ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;

			if (_header->vkformat)
				_format = _header->vkformat;
			width = _header->width;
			height = _header->height;
			mipLevels = _header->mipMapCount;
			layerCount = _header->arraySize;
		}
		if (mipLevels < 1)mipLevels = 1;
		if (layerCount < 1)layerCount = 1;
		VkImage tex;

		// Create the Image:
		{
			info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			info.imageType = VK_IMAGE_TYPE_2D;
			info.format = (VkFormat)_format;
			info.extent.width = width;
			info.extent.height = height;
			info.extent.depth = 1;
			info.mipLevels = mipLevels;
			info.arrayLayers = layerCount;
			if (layerCount == 6)
				info.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
			info.samples = VK_SAMPLE_COUNT_1_BIT;
			info.tiling = VK_IMAGE_TILING_OPTIMAL;
			info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | usageFlags;
			info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			// allocate memory and bind the image to it
#ifdef USE_VMA
			VmaAllocationCreateInfo imageAllocCreateInfo = {};
			imageAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
			imageAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT;
			imageAllocCreateInfo.pUserData = (void*)m_name.c_str();
			VmaAllocationInfo gpuImageAllocInfo = {};
			VkResult res = vmaCreateImage(pDevice->GetAllocator(), &info, &imageAllocCreateInfo, &tex, &m_ImageAlloc, &gpuImageAllocInfo);
			assert(res == VK_SUCCESS);
			SetResourceName(pDevice->device, VK_OBJECT_TYPE_IMAGE, (uint64_t)tex, m_name.c_str());
#else
			VkResult res = vkCreateImage(_dev->device, &info, NULL, &tex);
			assert(res == VK_SUCCESS);
			*_info = info;
			VkMemoryRequirements mem_reqs;
			vkGetImageMemoryRequirements(_dev->device, tex, &mem_reqs);

			VkMemoryAllocateInfo alloc_info = {};
			alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			alloc_info.allocationSize = mem_reqs.size;
			alloc_info.memoryTypeIndex = 0;

			bool pass = memory_type_from_properties(_dev->get_dmp(), mem_reqs.memoryTypeBits,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&alloc_info.memoryTypeIndex);
			assert(pass && "No mappable, coherent memory");

			res = vkAllocateMemory(_dev->device, &alloc_info, NULL, &image_memory);
			assert(res == VK_SUCCESS);

			res = vkBindImageMemory(_dev->device, tex, image_memory, 0);
			assert(res == VK_SUCCESS);
#endif
		}
		_image = tex;
		return tex;
	}
	void dvk_texture::upload_data(upload_cx* up, IMG_INFO* info, uint32_t bufferOffset)
	{
		uint32_t imageUsageFlags = _info->usage | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT; //uint32_t imageLayout;
		{
			if (!_dev)_dev = up->_dev;
			//_dev = dev;
			bool isnew = false;
			if (width != info->width || height != info->height)
			{
				width = info->width;
				height = info->height;
				isnew = true;
			}
			VkSampler psampler = _dev->_sampler;
			auto dev = _dev;
			mipLevels = info->mipMapCount;
			//mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

			// Copy to Image:
			{
				VkImageMemoryBarrier copy_barrier = {};
				copy_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				copy_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				copy_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				copy_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				copy_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				copy_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				copy_barrier.image = _image;
				copy_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				copy_barrier.subresourceRange.levelCount = info->mipMapCount;
				copy_barrier.subresourceRange.layerCount = info->arraySize;
				//vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, copy_barrier);
				up->AddPreBarrier(copy_barrier);
				VkBufferImageCopy region = {};
				region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				region.imageSubresource.layerCount = info->arraySize;
				region.imageExtent.width = width;
				region.imageExtent.height = height;
				region.imageExtent.depth = info->depth;
				region.bufferOffset = bufferOffset;
				//vkCmdCopyBufferToImage(copyCmd, staging->buffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
				up->addCopy(_image, region);
				VkImageMemoryBarrier use_barrier = {};
				use_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				use_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				use_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				use_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				use_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				use_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				use_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				use_barrier.image = _image;
				use_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				use_barrier.subresourceRange.levelCount = info->mipMapCount;
				use_barrier.subresourceRange.layerCount = info->arraySize;
				//vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, use_barrier);
				up->AddPostBarrier(use_barrier);
			}


#if 0
			auto hr = vkc::flushCommandBuffer(_dev->device, copyCmd, cp->command_pool, copyQueue, true);
			if (!hr)
			{
				t_string s;
				s.resize(1024);
				sprintf(s.data(), "Image %p\twidth:%d,height:%d\n,thread id %d\n", _image, width, height, (int)get_tid());
				//msg_box(s.c_str(), "vkQueueSubmit错误", MB_OK);

				printf("失败\n");
			}

			qctx->free_cmd_pool(cp);
#endif
			if (uimg) {
				printf("上传纹理:Image %p\twidth:%d,height:%d\t,thread id %d\n", _image, width, height, (int)get_tid());
				uimg->get_crc();
				printf("crc:%ud\n", uimg->dcrc);
			}
			if (psampler && psampler != _dev->_sampler)
			{
				sampler = (VkSampler)psampler;;
			}
			else
			{
				psampler = _dev->_sampler;
			}
			// Update descriptor image info member that can be used for setting up descriptor sets
			//descriptor->imageView = _view;
			//descriptor->sampler = psampler;
			//descriptor->imageLayout = (VkImageLayout)imageLayout;

		}
		//catch (...)
		{
			//	std::string s = "out";
			//	LOGW(s.c_str());
		}
	}
	bool dvk_texture::InitFromFile(upload_cx* up, const char* pFilename, bool useSRGB, VkImageUsageFlags usageFlags, float cutOff)
	{
		_dev = up->_dev;
		assert(_image == NULL);
		IMG_INFO info = {};
		auto ext = File::getPath(pFilename);
		Image* img = 0;
		do {
#ifdef _WIN320
			DDSLoader loader;
			if (ext != ".dds")break;
			if (loader.Load(pFilename, 0, &info))
			{
				auto mp = up->get_tbuf(loader.get_size(), 512);

				load_px(info, &loader, mp);
				break;
			}
#endif
			img = Image::create(pFilename);
			if (img)
			{
				info.size = img->datasize();
				info.width = img->width;
				info.height = img->height;
				info.arraySize = img->layerCount;
				info.mipMapCount = img->mipLevels;
			}
		} while (0);
		_header = &info;
		_image = CreateTextureCommitted(up, pFilename, useSRGB, usageFlags);
		_info->usage |= usageFlags;
		if (!img)
		{
			upload_data(up, &info, up->last_pos);
		}
		else if (img)
		{
			set_data(img, up);
			delop(img);
		}
		else
		{
			//Trace("Error loading texture from file: %s", pFilename);
			assert("Could not load requested file. Please make sure it exists on disk.");
		}
		bool result = _image;

		return result;
	}

	bool dvk_texture::InitFromData(upload_cx* up, IMG_INFO& header, VkImageUsageFlags usageFlags, bool useSRGB, const char* name)
	{
		assert(!_image && !_dev);
		assert(header.arraySize == 1 && header.mipMapCount > 0);// == 1);

		_header = &header;
		_image = CreateTextureCommitted(up, name, useSRGB, usageFlags);
		_info->usage |= usageFlags;

		auto mp = up->get_tbuf(_header->size, 512);
		if (!mp)
		{
			mp = up->get_tbuf(_header->size, 512);
		}
		memcpy(mp, _header->data, _header->size);
		upload_data(up, _header, up->last_pos);

		return true;
		// Upload Image
		{
			VkImageMemoryBarrier copy_barrier = {};
			copy_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			copy_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			copy_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			copy_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			copy_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			copy_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			copy_barrier.image = _image;
			copy_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copy_barrier.subresourceRange.baseMipLevel = 0;
			copy_barrier.subresourceRange.levelCount = header.mipMapCount;
			copy_barrier.subresourceRange.layerCount = header.arraySize;
			vkCmdPipelineBarrier(up->get_cmdbuf(), VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &copy_barrier);
		}

		//compute pixel size
		//
		uint32_t bytePP = header.bitCount / 8;
#ifdef DXGI_FORMAT_BC1_TYPELESS
		if ((header.format >= DXGI_FORMAT_BC1_TYPELESS) && (header.format <= DXGI_FORMAT_BC5_SNORM))
		{
			bytePP = (uint32_t)GetPixelByteSize((DXGI_FORMAT1)header.format);
		}
#endif

		uint64_t UplHeapSize = header.width * header.height * 4;
		auto pixels = up->get_tbuf(UplHeapSize, 512);// .Suballocate(UplHeapSize, 512);
		assert(pixels != NULL);

		//CopyMemory(pixels, data, header.width * header.height * bytePP);

		VkBufferImageCopy region = {};
		region.bufferOffset = up->last_pos;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.layerCount = 1;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.mipLevel = 0;
		region.imageExtent.width = header.width;
		region.imageExtent.height = header.height;
		region.imageExtent.depth = 1;
		vkCmdCopyBufferToImage(up->get_cmdbuf(), up->get_resource(), _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		// prepare to shader read
		//
		{
			VkImageMemoryBarrier use_barrier = {};
			use_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			use_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			use_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			use_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			use_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			use_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			use_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			use_barrier.image = _image;
			use_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			use_barrier.subresourceRange.levelCount = header.mipMapCount;
			use_barrier.subresourceRange.layerCount = header.arraySize;
			vkCmdPipelineBarrier(up->get_cmdbuf(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &use_barrier);
		}

		return true;
	}
#if 0
	void dvk_texture::set_data(void* buffer, VkDeviceSize bufferSize, uint32_t format
		, uint32_t w, uint32_t h, upload_cx* up, uint32_t imageUsageFlags, uint32_t imageLayout)
		//void dvk_texture::set_data(void* buffer, VkDeviceSize bufferSize, uint32_t format, uint32_t w, uint32_t h, upload_cx*up, VkQueue copyQueue, VkImageUsageFlags imageUsageFlags, uint32_t  imageLayout)//VkImageLayout
	{
		//assert(buffer);
		//try
		{
			if (!_dev)_dev = up->_dev;
			auto dev = _dev;
			bool isnew = false;
			if (width != w || height != h)
			{
				width = w;
				height = h;
				isnew = true;
			}
			VkSampler psampler = dev->_sampler;
			mipLevels = 1;
			mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(w, h)))) + 1;
			_format = format;
			//if (!copyQueue)
			//{
			//	copyQueue = dev->GetGraphicsQueue();
			//}
			//VK_CHECK_RESULT(vkQueueWaitIdle(copyQueue));

			// 获取0号图形列队
			//auto qctx = dev->get_graphics_queue(1);
			////auto_destroy_cx dcx;
			//auto cp = qctx->new_cmd_pool();

			//auto copyQueue = qctx->get_vkptr();
			VkMemoryAllocateInfo memAllocInfo = {};
			memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			VkMemoryRequirements memReqs;
			// Use a separate command buffer for texture loading
			//VkCommandBuffer copyCmd = vkc::createCommandBuffer1(dev->device, cp->command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

			//LOGW("createTexture:w:%d,h:%d,size:%lld,memReqs.size:%d\n", width, height, p->size, width * height * 4);

			// Get device properites for the requested texture format
			VkFormatProperties formatProperties = {};
			vkGetPhysicalDeviceFormatProperties(dev->physicalDevice, VK_FORMAT_R32_UINT, &formatProperties);

			if (formatProperties.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT)
			{
				format = format;
			}
			dvk_staging_buffer staging;
			// Create a host-visible staging buffer that contains the raw image data
			staging.initBuffer(dev, bufferSize);

			// Copy texture data into staging buffer
			staging.copyToBuffer(buffer, bufferSize);
			//
			VkBufferImageCopy bufferCopyRegion = {};
			bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferCopyRegion.imageSubresource.mipLevel = 0;
			bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
			bufferCopyRegion.imageSubresource.layerCount = 1;
			bufferCopyRegion.imageExtent.width = width;
			bufferCopyRegion.imageExtent.height = height;
			bufferCopyRegion.imageExtent.depth = 1;
			bufferCopyRegion.bufferOffset = 0;


			// Create optimal tiled target image
			VkImageCreateInfo imageCreateInfo = {};
			imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
			imageCreateInfo.format = (VkFormat)format;
			imageCreateInfo.mipLevels = mipLevels;
			imageCreateInfo.arrayLayers = 1;
			imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageCreateInfo.extent = { width, height, 1 };
			imageCreateInfo.usage = imageUsageFlags;
			// Ensure that the TRANSFER_DST bit is set for staging
			if (!(imageCreateInfo.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
			{
				imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			}
			if (isnew)
			{
				if (_image)
				{
					vkDestroyImage(dev->device, _image, 0);
					_image = 0;
				}
				(vkCreateImage(dev->device, &imageCreateInfo, 0, &_image));
			}
			//	LOGW("createTexture:w:%d,h:%d,size:%d,memReqs.size:%d\n", width, height, bufferSize, width*height*4);
			vkGetImageMemoryRequirements(dev->device, _image, &memReqs);

			memAllocInfo.allocationSize = memReqs.size;
			VkBool32 memTypeFound = 0;
			uint32_t dh = imageCreateInfo.usage & VK_IMAGE_USAGE_STORAGE_BIT ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			memAllocInfo.memoryTypeIndex = vkc::getMemoryType(dev, memReqs.memoryTypeBits, dh, &memTypeFound);

			if (isnew)
			{
				if (deviceMemory)
				{
					vkFreeMemory(dev->device, deviceMemory, 0);
					deviceMemory = 0;
				}
				(vkAllocateMemory(dev->device, &memAllocInfo, nullptr, &deviceMemory));
				(vkBindImageMemory(dev->device, _image, deviceMemory, 0));
			}

			// Create image view
			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = (VkFormat)format;
			viewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.image = _image;
			if (isnew)
			{
				if (_view)
				{
					vkDestroyImageView(dev->device, _view, 0);
				}
				(vkCreateImageView(dev->device, &viewInfo, nullptr, &_view));
			}
#if 0
			VkImageSubresourceRange subresourceRange = {};
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = mipLevels;
			subresourceRange.layerCount = 1;

			// Image barrier for optimal image (target)
			// Optimal image will be used as destination for the copy
			hz::vkc::setImageLayout(copyCmd, _image
				, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				, subresourceRange, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

			// Copy mip levels from staging buffer
			vkCmdCopyBufferToImage(copyCmd, staging.buffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);

			// Change texture image layout to shader read after all mip levels have been copied
			hz::vkc::setImageLayout(copyCmd, _image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				, (VkImageLayout)imageLayout, subresourceRange, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);


#else
			// Copy to Image:
			{
				VkImageMemoryBarrier copy_barrier = {};
				copy_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				copy_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				copy_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				copy_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				copy_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				copy_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				copy_barrier.image = _image;
				copy_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				copy_barrier.subresourceRange.levelCount = 1;
				copy_barrier.subresourceRange.layerCount = 1;
				//vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, copy_barrier);
				up->AddPreBarrier(copy_barrier);
				VkBufferImageCopy region = {};
				region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				region.imageSubresource.layerCount = 1;
				region.imageExtent.width = width;
				region.imageExtent.height = height;
				region.imageExtent.depth = 1;
				//vkCmdCopyBufferToImage(copyCmd, staging.buffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
				up->addCopy(_image, region);
				VkImageMemoryBarrier use_barrier = {};
				use_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				use_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				use_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				use_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				use_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				use_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				use_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				use_barrier.image = _image;
				use_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				use_barrier.subresourceRange.levelCount = 1;
				use_barrier.subresourceRange.layerCount = 1;
				//vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, use_barrier);
				up->AddPostBarrier(use_barrier);
			}
#endif
#if 0
			auto hr = vkc::flushCommandBuffer(_dev->device, copyCmd, cp->command_pool, copyQueue, true);
			if (!hr)
			{
				t_string s;
				s.resize(1024);
				sprintf(s.data(), "Image %p\twidth:%d,height:%d\n,thread id %d\n", _image, width, height, (int)get_tid());
				//msg_box(s.c_str(), "vkQueueSubmit错误", MB_OK);

				printf("submit错误\n");
			}

			qctx->free_cmd_pool(cp);
#endif
			if (uimg) {
				printf("上传纹理:Image %p\twidth:%d,height:%d\t,thread id %d\n", _image, width, height, (int)get_tid());
				uimg->get_crc();
				printf("crc:%ud\n", uimg->dcrc);
			}
			if (psampler && psampler != _dev->_sampler)
			{
				sampler = (VkSampler)psampler;;
			}
			else
			{
				psampler = _dev->_sampler;
			}
			// Update descriptor image info member that can be used for setting up descriptor sets
			descriptor->imageView = _view;
			descriptor->sampler = psampler;
			descriptor->imageLayout = (VkImageLayout)imageLayout;

		}
		//catch (...)
		{
			//	std::string s = "out";
			//	LOGW(s.c_str());
		}
	}
	void dvk_texture::mkrt_image(int w, int h, uint32_t format, bool is_depth, uint32_t usage, ImageLayoutBarrier image_layout)
		//void dvk_texture::mkrt_image(int w, int h, uint32_t format, bool is_depth, VkImageUsageFlags usage, ImageLayoutBarrier image_layout)
	{
		// Color attachment
		VkImageCreateInfo image = {};
		VkImageViewCreateInfo colorImageView = {};
		auto imageLayout = (VkImageLayout)GetImageLayout(image_layout);
		_image_layout = image_layout;
		width = w;
		height = h;
		if (!format)
		{
			do
			{
				if (is_depth)
				{

					VkFormat depth_format = {};
					VkBool32 validDepthFormat = getSupportedDepthFormat(_dev->physicalDevice, &depth_format);
					if (validDepthFormat)
					{
						format = depth_format;
						break;
					}
					else {
						return;
					}
				}
				format = VK_FORMAT_R8G8B8A8_UNORM;
			} while (0);
		}
		_format = (VkFormat)format;
		image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image.imageType = VK_IMAGE_TYPE_2D;
		image.format = (VkFormat)format;
		image.extent.width = width;
		image.extent.height = height;
		image.extent.depth = 1;
		image.mipLevels = 1;
		image.arrayLayers = 1;
		image.samples = VK_SAMPLE_COUNT_1_BIT;
		image.tiling = VK_IMAGE_TILING_OPTIMAL;// VK_IMAGE_TILING_LINEAR;// 
		// We will sample directly from the color attachment
		image.usage = usage ? usage : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		image.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
		image.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
		//image.queueFamilyIndexCount = 1;
		uint32_t queue_family = 0;
		//image.pQueueFamilyIndices = &queue_family;
		//image.queueFamilyIndexCount = 1;
		colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colorImageView.format = (VkFormat)format;
		colorImageView.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
		colorImageView.subresourceRange = {};
		colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		colorImageView.subresourceRange.baseMipLevel = 0;
		colorImageView.subresourceRange.levelCount = 1;
		colorImageView.subresourceRange.baseArrayLayer = 0;
		colorImageView.subresourceRange.layerCount = 1;
		if (is_depth)
		{
			//image.format = depthFormat;
			image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;// | VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		_alloc_size = vkc::createImage(_dev, &image, &colorImageView, this, 0, 0);
		if (image.usage & VK_IMAGE_USAGE_STORAGE_BIT)
			map();
		auto qctx = _dev->get_graphics_queue(1);
		auto cp = qctx->new_cmd_pool();
		auto copyQueue = qctx->get_vkptr();
		VkCommandBuffer copyCmd = vkc::createCommandBuffer1(_dev->device, cp->command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);


		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.levelCount = mipLevels;
		subresourceRange.layerCount = 1;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		ImagePipelineBarrier(copyCmd, _image, ImageLayoutBarrier::UNDEFINED, image_layout, subresourceRange);
		auto subhr = vkc::flushCommandBuffer(_dev->device, copyCmd, cp->command_pool, copyQueue, true);
		if (!subhr)
		{
			printf("mkrt_image\n");
			printf("copyQueue %p\n", copyQueue);
			msg_box("vkQueueSubmit错误", "提交出错", MB_OK);
		}
		qctx->free_cmd_pool(cp);
		descriptor->imageView = _view;
		descriptor->sampler = _dev->_sampler;
		descriptor->imageLayout = imageLayout;
	}

	void dvk_texture::new_storage(int width, int height, uint32_t format, bool is_compute, bool sampled)
	{
		uint32_t usage = VK_IMAGE_USAGE_STORAGE_BIT;
		if (sampled)usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
		mkrt_image(width, height, format ? format : VK_FORMAT_R8G8B8A8_UNORM, false, usage
			, is_compute ? ImageLayoutBarrier::ComputeGeneralRW : ImageLayoutBarrier::PixelGeneralRW);
	}
#endif
	Image* dvk_texture::save2Image(Image* outimage, dvk_queue* q, bool unpm)
	{
		if (outimage)
		{
			outimage->resize(width, height);
			outimage->clearColor(0);
			get_buffer((char*)outimage->data(), q);
			outimage->pre((VK_FORMAT_B8G8R8A8_UNORM == _format), unpm);
		}
		return outimage;
	}
	bool dvk_texture::check_format()
	{
		bool supportsBlit = true;
		// Check blit support for source and destination
		VkFormatProperties formatProps = {};

		// Check if the device supports blitting from optimal images (the swapchain images are in optimal format)
		vkGetPhysicalDeviceFormatProperties(_dev->physicalDevice, (VkFormat)_format, &formatProps);
		if (!(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT)) {
			std::cerr << "Device does not support blitting from optimal tiled images, using copy instead of blit!" << std::endl;
			supportsBlit = false;
		}

		// Check if the device supports blitting to linear images
		vkGetPhysicalDeviceFormatProperties(_dev->physicalDevice, VK_FORMAT_R8G8B8A8_UNORM, &formatProps);
		if (!(formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT)) {
			std::cerr << "Device does not support blitting to linear tiled images, using copy instead of blit!" << std::endl;
			supportsBlit = false;
		}
		return supportsBlit;
	}
	bool dvk_texture::save2file(const char* fn0, dvk_queue* q, bool unpm)
	{
		bool supportsBlit = check_format();


		// Source for the copy is the last rendered swapchain image
		VkImage srcImage = _image;

		// Create the linear tiled destination image to copy to and to read the memory from
		VkImageCreateInfo imageCreateCI = {};
		imageCreateCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateCI.imageType = VK_IMAGE_TYPE_2D;
		// Note that vkCmdBlitImage (if supported) will also do format conversions if the swapchain color format would differ
		imageCreateCI.format = VK_FORMAT_R8G8B8A8_UNORM;
		imageCreateCI.extent.width = width;
		imageCreateCI.extent.height = height;
		imageCreateCI.extent.depth = 1;
		imageCreateCI.arrayLayers = 1;
		imageCreateCI.mipLevels = 1;
		imageCreateCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateCI.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateCI.tiling = VK_IMAGE_TILING_LINEAR;
		imageCreateCI.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		// Create the image
		VkImage dstImage;
		auto hr = (vkCreateImage(_dev->device, &imageCreateCI, nullptr, &dstImage));
		// Create memory to back up the image
		VkMemoryRequirements memRequirements = {};
		VkMemoryAllocateInfo memAllocInfo = {};
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		VkDeviceMemory dstImageMemory;
		vkGetImageMemoryRequirements(_dev->device, dstImage, &memRequirements);
		memAllocInfo.allocationSize = memRequirements.size;
		VkBool32 mtf[1] = {};
		// Memory must be host visible to copy from
		memAllocInfo.memoryTypeIndex = vkc::getMemoryType(_dev, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mtf);
		hr = (vkAllocateMemory(_dev->device, &memAllocInfo, nullptr, &dstImageMemory));
		hr = (vkBindImageMemory(_dev->device, dstImage, dstImageMemory, 0));

		// Do the actual blit from the swapchain image to our host visible destination image

		auto cmdBuffer = new easy_cmd_cx(_dev);
		std::shared_ptr<easy_cmd_cx> ecb1(cmdBuffer);
		// start record
		cmdBuffer->begin();

		VkCommandBuffer copyCmd = cmdBuffer->get();

		// Transition destination image to transfer destination layout
		insertImageMemoryBarrier(
			copyCmd,
			dstImage,
			0,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

		// Transition swapchain image from present to transfer source layout
		insertImageMemoryBarrier(
			copyCmd,
			srcImage,
			VK_ACCESS_MEMORY_READ_BIT,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

		// If source and destination support blit we'll blit as this also does automatic format conversion (e.g. from BGR to RGB)
		if (supportsBlit)
		{
			// Define the region to blit (we will blit the whole swapchain image)
			VkOffset3D blitSize;
			blitSize.x = width;
			blitSize.y = height;
			blitSize.z = 1;
			VkImageBlit imageBlitRegion{};
			imageBlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageBlitRegion.srcSubresource.layerCount = 1;
			imageBlitRegion.srcOffsets[1] = blitSize;
			imageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageBlitRegion.dstSubresource.layerCount = 1;
			imageBlitRegion.dstOffsets[1] = blitSize;

			// Issue the blit command
			vkCmdBlitImage(
				copyCmd,
				srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&imageBlitRegion,
				VK_FILTER_NEAREST);
		}
		else
		{
			// Otherwise use image copy (requires us to manually flip components)
			VkImageCopy imageCopyRegion{};
			imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageCopyRegion.srcSubresource.layerCount = 1;
			imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageCopyRegion.dstSubresource.layerCount = 1;
			imageCopyRegion.extent.width = width;
			imageCopyRegion.extent.height = height;
			imageCopyRegion.extent.depth = 1;

			// Issue the copy command
			vkCmdCopyImage(
				copyCmd,
				srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&imageCopyRegion);
		}

		// Transition destination image to general layout, which is the required layout for mapping the image memory later on
		insertImageMemoryBarrier(
			copyCmd,
			dstImage,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_MEMORY_READ_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

		// Transition back the swap chain image after the blit is done
		insertImageMemoryBarrier(
			copyCmd,
			srcImage,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_ACCESS_MEMORY_READ_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

		cmdBuffer->end();
		cmdBuffer->submit();

		// Get layout of the image (including row pitch)
		VkImageSubresource subResource{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0 };
		VkSubresourceLayout subResourceLayout;
		vkGetImageSubresourceLayout(_dev->device, dstImage, &subResource, &subResourceLayout);

		// Map image memory so we can start copying from it
		const char* data;
		vkMapMemory(_dev->device, dstImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&data);
		data += subResourceLayout.offset;
		// If source is BGR (destination is always RGB) and we can't use blit (which does automatic conversion), we'll have to manually swizzle color components
		bool colorSwizzle = false;
		// Check if source is BGR
		// Note: Not complete, only contains most common and basic BGR surface formats for demonstration purposes
		if (!supportsBlit)
		{
			std::vector<VkFormat> formatsBGR = { VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_SNORM };
			colorSwizzle = (std::find(formatsBGR.begin(), formatsBGR.end(), (VkFormat)_format) != formatsBGR.end());
		}
#if 0


		std::ofstream file(filename, std::ios::out | std::ios::binary);

		// ppm header
		file << "P6\n" << width << "\n" << height << "\n" << 255 << "\n";

		auto olddata = data;
		// ppm binary pixel data
		for (uint32_t y = 0; y < height; y++)
		{
			unsigned int* row = (unsigned int*)data;
			for (uint32_t x = 0; x < width; x++)
			{
				if (colorSwizzle)
				{
					file.write((char*)row + 2, 1);
					file.write((char*)row + 1, 1);
					file.write((char*)row, 1);
				}
				else
				{
					file.write((char*)row, 3);
				}
				row++;
			}
			data += subResourceLayout.rowPitch;
		}
		file.close();
#endif // 0
		std::string fn = fn0;
		fn.resize(fn.find('.'));
		fn += ".png";
		{
			Image::pre_a((unsigned int*)data, width, height, colorSwizzle, unpm);
		}
		int ret = stbi_write_png(fn.c_str(), width, height, 4, data, 0);
		//std::cout << "Screenshot saved to disk" << std::endl;

		// Clean up resources
		vkUnmapMemory(_dev->device, dstImageMemory);
		vkFreeMemory(_dev->device, dstImageMemory, nullptr);
		vkDestroyImage(_dev->device, dstImage, nullptr);
		return ret;
	}


	void dvk_texture::get_buffer(char* outbuf, dvk_queue* q)
	{

		dvk_staging_buffer staging;
		// Use a separate command buffer for texture loading
		auto qctx = q ? q : _dev->get_graphics_queue(1);
		auto cp = qctx->new_cmd_pool();
		auto copyQueue = qctx->get_vkptr();
		VkCommandBuffer copyCmd = vkc::createCommandBuffer1(_dev->device, cp->command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
		VkImageAspectFlags    aspectMask = (VkImageAspectFlags)(_format == VK_FORMAT_D32_SFLOAT_S8_UINT ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);
		VkBufferImageCopy bufferCopyRegion = {};
		bufferCopyRegion.imageSubresource.aspectMask = aspectMask;
		bufferCopyRegion.imageSubresource.mipLevel = 0;
		bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
		bufferCopyRegion.imageSubresource.layerCount = 1;
		bufferCopyRegion.imageExtent.width = width;
		bufferCopyRegion.imageExtent.height = height;
		bufferCopyRegion.imageExtent.depth = 1;
		bufferCopyRegion.bufferOffset = 0;
		bufferCopyRegion.bufferImageHeight = height;
		bufferCopyRegion.bufferRowLength = width;
		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.aspectMask = aspectMask;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = mipLevels;
		subresourceRange.layerCount = 1;
		staging.initBuffer(_dev, _alloc_size);
		// Image barrier for optimal image (target)
		// Optimal image will be used as destination for the copy
		hz::vkc::setImageLayout(copyCmd, _image, aspectMask, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			, subresourceRange, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
		// Copy mip levels from staging buffer
		vkCmdCopyImageToBuffer(copyCmd, _image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, staging.buffer, 1, &bufferCopyRegion);
		VkImageLayout il = (VkImageLayout)GetImageLayout(_image_layout);
		// Change texture image layout to shader read after all mip levels have been copied
		hz::vkc::setImageLayout(copyCmd, _image, aspectMask, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			, !descriptor ? il : descriptor->imageLayout, subresourceRange, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

		auto subhr = vkc::flushCommandBuffer(_dev->device, copyCmd, cp->command_pool, copyQueue, true);
		if (!subhr)
		{
#ifdef _WIN32
			printf("tex2cpu\n");
			printf("copyQueue %p\n", copyQueue);
			msg_box("vkQueueSubmit错误", "提交出错", MB_OK);
#endif
		}
		staging.getBuffer(outbuf, -1);
		qctx->free_cmd_pool(cp);

	}

	void dvk_texture::set_data(const void* data, int size, int width, int height, uint32_t format, upload_cx* up)
	{
		do {
			IMG_INFO info = {};

			if (!data || !size || !(width * height > 0))break;
			//dvk_staging_buffer staging;
			info.width = width;
			info.height = height;
			info.arraySize = 1;
			info.mipMapCount = 1;
			info.depth = 1;
			info.vkformat = format;
			auto mp = up->get_tbuf(size, 512);
			memcpy(mp, data, size);
			upload_data(up, &info, up->last_pos);

		} while (0);

		return;
	}

	void dvk_texture::set_data(Image* img, upload_cx* up)
	{
		VkImageLayout il = (VkImageLayout)GetImageLayout(_image_layout);
		if (!img || img->empty())
		{
			return;
		}
		int comp = 0;
		int width = img->width;
		int height = img->height;
		uint8_t* data = (uint8_t*)img->data();
		size_t size = width * height * 4 * img->layerCount;
		do {
			IMG_INFO info = {};
			if (!data || !size || !(width * height > 0))break;
			info.width = width;
			info.height = height;
			info.arraySize = img->layerCount;
			info.mipMapCount = 1;
			info.depth = 1;
			info.vkformat = VK_FORMAT_R8G8B8A8_UNORM;
			auto mp = up->get_tbuf(size, 512);
			memcpy(mp, data, size);
			upload_data(up, &info, up->last_pos);

		} while (0);
	}

	char* dvk_texture::map()
	{
		VkResult res = {};
		if (!mapped)
			res = (vkMapMemory(_dev->device, image_memory, 0, _alloc_size, 0, (void**)&mapped));
		return (char*)mapped;
	}
	void  dvk_texture::unmap()
	{
		assert(image_memory);
		if (!image_memory)return;
		// 如果没有请求主机一致性，请手动刷新以使写入可见
		// If host coherency hasn't been requested, do a manual flush to make writes visible
		//if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
		{
			VkMappedMemoryRange mappedRange = {};
			mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			mappedRange.memory = image_memory;
			mappedRange.offset = 0;
			mappedRange.size = _alloc_size;
			vkFlushMappedMemoryRanges(_dev->device, 1, &mappedRange);
		}
		vkUnmapMemory(_dev->device, image_memory);
	}

	uint32_t GetImageLayout(ImageLayoutBarrier target)
	{
		VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;

		switch (target)
		{
		case ImageLayoutBarrier::UNDEFINED:
		{
			layout = VK_IMAGE_LAYOUT_UNDEFINED;
		}
		break;

		case ImageLayoutBarrier::TRANSFER_DST:
		{
			layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		}
		break;

		case ImageLayoutBarrier::COLOR_ATTACHMENT:
		{
			layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
		break;

		case ImageLayoutBarrier::DEPTH_STENCIL_ATTACHMENT:
		{
			layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}
		break;

		case ImageLayoutBarrier::TRANSFER_SRC:
		{
			layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		}
		break;

		case ImageLayoutBarrier::PRESENT_SRC:
		{
			layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		}
		break;

		case ImageLayoutBarrier::SHADER_READ:
		{
			layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
		break;

		case ImageLayoutBarrier::DEPTH_STENCIL_READ:
		{
			layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		}
		break;

		case ImageLayoutBarrier::PixelGeneralRW:
		case ImageLayoutBarrier::ComputeGeneralRW:
		{
			layout = VK_IMAGE_LAYOUT_GENERAL;
		}
		break;
		default:
		{
			//MLOGE("Unknown ImageLayoutBarrier %d", (int32)target);
		}
		break;
		}

		return layout;
	}

	void ImagePipelineBarrier(VkCommandBuffer cmdBuffer, VkImage image, ImageLayoutBarrier source, ImageLayoutBarrier dest, const VkImageSubresourceRange& subresourceRange)
	{
		VkImageMemoryBarrier imageBarrier = {};
		imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageBarrier.image = image;
		imageBarrier.subresourceRange = subresourceRange;
		imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		VkPipelineStageFlags sourceStages = (VkPipelineStageFlags)0;
		VkPipelineStageFlags destStages = (VkPipelineStageFlags)0;
		SetImageBarrierInfo(source, dest, imageBarrier, sourceStages, destStages);

		if (source == ImageLayoutBarrier::PRESENT_SRC) {
			sourceStages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		}
		else if (dest == ImageLayoutBarrier::PRESENT_SRC) {
			destStages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		}

		vkCmdPipelineBarrier(cmdBuffer, sourceStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier);
	}

	void SetImageBarrierInfo(ImageLayoutBarrier source, ImageLayoutBarrier dest, VkImageMemoryBarrier& inOutBarrier, VkPipelineStageFlags& inOutSourceStage, VkPipelineStageFlags& inOutDestStage)
	{
		inOutSourceStage |= GetImageBarrierFlags(source, inOutBarrier.srcAccessMask, inOutBarrier.oldLayout);
		inOutDestStage |= GetImageBarrierFlags(dest, inOutBarrier.dstAccessMask, inOutBarrier.newLayout);
	}

	VkPipelineStageFlags GetImageBarrierFlags(ImageLayoutBarrier target, VkAccessFlags& accessFlags, VkImageLayout& layout)
	{
		VkPipelineStageFlags stageFlags = (VkPipelineStageFlags)0;
		switch (target)
		{
		case ImageLayoutBarrier::UNDEFINED:
		{
			accessFlags = 0;
			stageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			layout = VK_IMAGE_LAYOUT_UNDEFINED;
		}
		break;

		case ImageLayoutBarrier::TRANSFER_DST:
		{
			accessFlags = VK_ACCESS_TRANSFER_WRITE_BIT;
			stageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
			layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		}
		break;

		case ImageLayoutBarrier::COLOR_ATTACHMENT:
		{
			accessFlags = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			stageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
		break;

		case ImageLayoutBarrier::DEPTH_STENCIL_ATTACHMENT:
		{
			accessFlags = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			stageFlags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}
		break;

		case ImageLayoutBarrier::TRANSFER_SRC:
		{
			accessFlags = VK_ACCESS_TRANSFER_READ_BIT;
			stageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
			layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		}
		break;

		case ImageLayoutBarrier::PRESENT_SRC:
		{
			accessFlags = 0;
			stageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		}
		break;

		case ImageLayoutBarrier::SHADER_READ:
		{
			accessFlags = VK_ACCESS_SHADER_READ_BIT;
			stageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
		break;

		case ImageLayoutBarrier::DEPTH_STENCIL_READ:
		{
			accessFlags = VK_ACCESS_SHADER_READ_BIT;
			stageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		}
		break;

		case ImageLayoutBarrier::ComputeGeneralRW:
		{
			accessFlags = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
			stageFlags = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			layout = VK_IMAGE_LAYOUT_GENERAL;
		}
		break;

		case ImageLayoutBarrier::PixelGeneralRW:
		{
			accessFlags = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
			stageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			layout = VK_IMAGE_LAYOUT_GENERAL;
		}
		break;

		default:
		{
			//MLOGE("Unknown ImageLayoutBarrier %d", (int32)target);
		}
		break;
		}

		return stageFlags;
	}
	/*
			VkBufferMemoryBarrier bufferMemoryBarrier {};
			bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			*/
	void insertImageMemoryBarrier(
		VkCommandBuffer cmdbuffer,
		VkImage image,
		VkAccessFlags srcAccessMask,
		VkAccessFlags dstAccessMask,
		VkImageLayout oldImageLayout,
		VkImageLayout newImageLayout,
		VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask,
		VkImageSubresourceRange subresourceRange)
	{
		VkImageMemoryBarrier imageMemoryBarrier = {};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.srcAccessMask = srcAccessMask;
		imageMemoryBarrier.dstAccessMask = dstAccessMask;
		imageMemoryBarrier.oldLayout = oldImageLayout;
		imageMemoryBarrier.newLayout = newImageLayout;
		imageMemoryBarrier.image = image;
		imageMemoryBarrier.subresourceRange = subresourceRange;

		vkCmdPipelineBarrier(
			cmdbuffer,
			srcStageMask,
			dstStageMask,
			0,
			0, nullptr,
			0, nullptr,
			1, &imageMemoryBarrier);
	}



	dvk_texture* new_image2d0(const uint8_t* rgbaData, uint32_t size, VkFormat format, int32_t width, int32_t height
		, dvk_device* dev, VkImageUsageFlags imageUsageFlags, ImageLayoutBarrier imageLayout)
	{
		int32_t mipLevels = floor(log2(std::max(width, height))) + 1;

		VkDevice device = dev->device;
		auto staging = new dvk_staging_buffer();
		std::shared_ptr<dvk_staging_buffer> stt(staging);
		// Create a host-visible staging buffer that contains the raw image data
		staging->initBuffer(dev, size);

		// Copy texture data into staging buffer
		staging->copyToBuffer((void*)rgbaData, size);
		//( VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, size);


		uint32_t memoryTypeIndex = 0;
		VkMemoryRequirements memReqs = {};
		VkMemoryAllocateInfo memAllocInfo = {};
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

		// image info
		VkImage                         image = VK_NULL_HANDLE;
		VkDeviceMemory                  imageMemory = VK_NULL_HANDLE;
		VkImageView                     imageView = VK_NULL_HANDLE;
		VkSampler                       imageSampler = VK_NULL_HANDLE;
		VkDescriptorImageInfo           descriptorInfo = {};

		if (!(imageUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)) {
			imageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		if (!(imageUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)) {
			imageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		// 创建image
		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = format;
		imageCreateInfo.mipLevels = mipLevels;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.extent = { (uint32_t)width, (uint32_t)height, 1 };
		imageCreateInfo.usage = imageUsageFlags;
		auto hr = (vkCreateImage(device, &imageCreateInfo, 0, &image));

		// bind image buffer
		vkGetImageMemoryRequirements(device, image, &memReqs);
		VkBool32 memTypeFound = 0;
		memAllocInfo.memoryTypeIndex = vkc::getMemoryType(dev, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memTypeFound);
		memAllocInfo.allocationSize = memReqs.size;
		hr = (vkAllocateMemory(device, &memAllocInfo, 0, &imageMemory));
		hr = (vkBindImageMemory(device, image, imageMemory, 0));

		auto cmdBuffer = new easy_cmd_cx(dev);
		std::shared_ptr<easy_cmd_cx> ecb1(cmdBuffer);
		// start record
		cmdBuffer->begin();

		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.levelCount = 1;
		subresourceRange.layerCount = 1;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;

		// undefined to TransferDest
		ImagePipelineBarrier(cmdBuffer->get(), image, ImageLayoutBarrier::UNDEFINED, ImageLayoutBarrier::TRANSFER_DST, subresourceRange);

		VkBufferImageCopy bufferCopyRegion = {};
		bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bufferCopyRegion.imageSubresource.mipLevel = 0;
		bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
		bufferCopyRegion.imageSubresource.layerCount = 1;
		bufferCopyRegion.imageExtent.width = width;
		bufferCopyRegion.imageExtent.height = height;
		bufferCopyRegion.imageExtent.depth = 1;

		// copy buffer to image
		vkCmdCopyBufferToImage(cmdBuffer->get(), staging->buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);

		// TransferDest to TransferSrc
		ImagePipelineBarrier(cmdBuffer->get(), image, ImageLayoutBarrier::TRANSFER_DST, ImageLayoutBarrier::TRANSFER_SRC, subresourceRange);

		// Generate the mip chain
		for (uint32_t i = 1; i < mipLevels; i++)
		{
			VkImageBlit imageBlit = {};

			int32_t mip0Width = std::max(width >> (i - 1), 1);
			int32_t mip0Height = std::max(height >> (i - 1), 1);
			int32_t mip1Width = std::max(width >> (i - 0), 1);
			int32_t mip1Height = std::max(height >> (i - 0), 1);

			imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageBlit.srcSubresource.layerCount = 1;
			imageBlit.srcSubresource.mipLevel = i - 1;
			imageBlit.srcOffsets[1].x = int32_t(mip0Width);
			imageBlit.srcOffsets[1].y = int32_t(mip0Height);
			imageBlit.srcOffsets[1].z = 1;

			imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageBlit.dstSubresource.layerCount = 1;
			imageBlit.dstSubresource.mipLevel = i;
			imageBlit.dstOffsets[1].x = int32_t(mip1Width);
			imageBlit.dstOffsets[1].y = int32_t(mip1Height);
			imageBlit.dstOffsets[1].z = 1;

			VkImageSubresourceRange mipSubRange = {};
			mipSubRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			mipSubRange.baseMipLevel = i;
			mipSubRange.levelCount = 1;
			mipSubRange.layerCount = 1;
			mipSubRange.baseArrayLayer = 0;

			// undefined to dst
			ImagePipelineBarrier(cmdBuffer->get(), image, ImageLayoutBarrier::UNDEFINED, ImageLayoutBarrier::TRANSFER_DST, mipSubRange);
			// blit image
			vkCmdBlitImage(cmdBuffer->get(), image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlit, VK_FILTER_LINEAR);
			// dst to src
			ImagePipelineBarrier(cmdBuffer->get(), image, ImageLayoutBarrier::TRANSFER_DST, ImageLayoutBarrier::TRANSFER_SRC, mipSubRange);
		}

		subresourceRange.levelCount = mipLevels;

		// dst to layout
		ImagePipelineBarrier(cmdBuffer->get(), image, ImageLayoutBarrier::TRANSFER_SRC, imageLayout, subresourceRange);

		cmdBuffer->end();
		cmdBuffer->submit();

		VkSamplerCreateInfo samplerInfo;
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
		samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		samplerInfo.maxAnisotropy = 1.0;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxLod = mipLevels;
		samplerInfo.minLod = 0.0f;
		hr = (vkCreateSampler(device, &samplerInfo, 0, &imageSampler));

		VkImageViewCreateInfo viewInfo;
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.subresourceRange.levelCount = mipLevels;
		hr = (vkCreateImageView(device, &viewInfo, 0, &imageView));

		descriptorInfo.sampler = imageSampler;
		descriptorInfo.imageView = imageView;
		descriptorInfo.imageLayout = (VkImageLayout)GetImageLayout(imageLayout);

		dvk_texture* texture = new dvk_texture();
		texture->descriptor = new VkDescriptorImageInfo(descriptorInfo);
		texture->_format = format;
		texture->width = width;
		texture->height = height;
		texture->_image = image;
		//texture->imageLayout = GetImageLayout(imageLayout);
		texture->image_memory = imageMemory;
		texture->sampler = imageSampler;
		texture->_view = imageView;
		texture->_dev = dev;
		texture->mipLevels = mipLevels;
		texture->layerCount = 1;

		return texture;
	}

	//dvk_texture* dvk_texture::new_2d(const std::string& filename, upload_cx* up, ImageLayoutBarrier imageLayout)
	//{
	//	auto p = dvk_texture::new2d_priv(filename, (dvk_device*)dev, imageLayout, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
	//	return p;
	//}
	dvk_texture* dvk_texture::new_image2d(Image* img, dvk_device* dev, ImageLayoutBarrier imageLayout, bool storage)
	{
		VkImageLayout il = (VkImageLayout)GetImageLayout(imageLayout);
		if (!img || img->empty())
		{
			return nullptr;
		}
		int comp = 0;
		int width = img->width;
		int height = img->height;
		uint8_t* data = (uint8_t*)img->data();
		auto imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		if (storage)
			imageUsageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
		auto tex = new dvk_texture();
		do {
			auto size = width * height * sizeof(uint32_t);
			IMG_INFO info = {};

			if (!data || !size || !(width * height > 0))break;
			info.width = width;
			info.height = height;
			info.arraySize = 1;
			info.mipMapCount = 1;
			info.depth = 1;
#ifdef _WIN32
			info.format = (DXGI_FORMAT)DXGI_FORMAT_R8G8B8A8_UNORM;
#endif
			info.vkformat = VK_FORMAT_R8G8B8A8_UNORM;

			if (tex)
			{
				upload_cx* up = new upload_cx();
				up->init(dev, size, 1);
				tex->uimg = img;
				info.data = data;
				info.size = size;
				tex->InitFromData(up, info, imageUsageFlags, false, img->_filename.c_str());
				up->flushAndFinish();
				tex->CreateSRV(&tex->_view);
				delop(up);
			}
		} while (0);
		return tex;
	}

	dvk_texture* dvk_texture::new_image2d(Image* img, upload_cx* up, ImageLayoutBarrier imageLayout, bool storage)
	{
		VkImageLayout il = (VkImageLayout)GetImageLayout(imageLayout);
		if (!img || img->empty())
		{
			return nullptr;
		}
		int comp = 0;
		int width = img->width;
		int height = img->height;
		uint8_t* data = (uint8_t*)img->data();
		auto imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		if (storage)
			imageUsageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
		auto tex = new dvk_texture();
		do {
			auto size = width * height * sizeof(uint32_t);
			IMG_INFO info = {};

			if (!data || !size || !(width * height > 0))break;
			info.width = width;
			info.height = height;
			info.arraySize = 1;
			info.mipMapCount = 1;
			info.depth = 1;
#ifdef _WIN32
			info.format = (DXGI_FORMAT)DXGI_FORMAT_R8G8B8A8_UNORM;
#endif
			info.vkformat = VK_FORMAT_R8G8B8A8_UNORM;

			if (tex)
			{
				tex->uimg = img;
				info.data = (uint8_t*)data;
				info.size = size;
				tex->InitFromData(up, info, imageUsageFlags, false, img->_filename.c_str());
				up->flush();
				tex->CreateSRV(&tex->_view);
			}
		} while (0);
		return tex;
	}
	dvk_texture* dvk_texture::new_image2d(void* buffer, VkDeviceSize bufferSize, uint32_t format, uint32_t w, uint32_t h
		, upload_cx* up, uint32_t imageUsageFlags, ImageLayoutBarrier imageLayout)
	{
		VkImageLayout il = (VkImageLayout)GetImageLayout(imageLayout);
		if (!buffer || w * h <= 0)
		{
			return nullptr;
		}
		imageUsageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		//if (storage)
		//	imageUsageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
		if (format == 0)format = VK_FORMAT_R8G8B8A8_UNORM;
		auto tex = new dvk_texture();
		if (tex)
			do {
				IMG_INFO info = {};

				if (!buffer || !bufferSize || !(w * h > 0))break;
				info.width = w;
				info.height = h;
				info.arraySize = 1;
				info.mipMapCount = 1;
				info.depth = 1;
#ifdef _WIN32
				info.format = (DXGI_FORMAT)DXGI_FORMAT_R8G8B8A8_UNORM;
#endif
				info.vkformat = VK_FORMAT_R8G8B8A8_UNORM;
				info.data = (uint8_t*)buffer;
				info.size = bufferSize;
				tex->InitFromData(up, info, imageUsageFlags, false, "");
				up->flush();
				tex->CreateSRV(&tex->_view);
				//tex->uimg = img;
				//tex->set_data(buffer, bufferSize, format, w, h, dev, imageUsageFlags, il);

			} while (0);
			return tex;
	}
	dvk_texture* dvk_texture::new_image2d(upload_cx* up, const void* data, int size, int width, int height, bool useSRGB, uint32_t format, uint32_t dxformat)
	{
		dvk_texture* tex = 0;

		do {
			IMG_INFO info = {};

			if (!data || !size || !(width * height > 0))break;
			//dvk_staging_buffer staging;
			info.width = width;
			info.height = height;
			info.arraySize = 1;
			info.mipMapCount = 1;
			info.depth = 1;

#ifdef _WIN32
			info.format = (DXGI_FORMAT)dxformat;
#endif
			info.vkformat = format;
			info.data = (uint8_t*)data;
			info.size = size;

			// compute number of mips
			//
			uint32_t mipWidth = width;
			uint32_t mipHeight = height;
			uint32_t mipCount = 0;
			for (;;)
			{
				mipCount++;
				if (mipWidth > 1) mipWidth >>= 1;
				if (mipHeight > 1) mipHeight >>= 1;
				if (mipWidth == 1 && mipHeight == 1)
					break;
			}
			auto pInfo = &info;
			// fill img struct
			//
			pInfo->arraySize = 1;
			pInfo->width = width;
			pInfo->height = height;
			pInfo->depth = 1;
			pInfo->mipMapCount = 1;// mipCount;
			pInfo->bitCount = 32;
#ifdef _WIN32
			pInfo->format = DXGI_FORMAT_R8G8B8A8_UNORM;
#else
			pInfo->vkformat = VK_FORMAT_R8G8B8A8_UNORM;
#endif
			tex = new dvk_texture();
			if (tex)
			{
				tex->ad = (char*)data;
				size_t ad = (size_t)data;
				tex->ad1 = (void*)AlignUp(ad, up->_dev->get_align_texel_offset());
				if (tex->ad != tex->ad1)
				{
					printf("align error\n");
				}
				tex->InitFromData(up, info, VK_IMAGE_USAGE_SAMPLED_BIT, useSRGB, "");
				up->flush();
				tex->CreateSRV(&tex->_view);
			}
		} while (0);

		return tex;
	}
	dvk_texture* dvk_texture::new_image2d(const char* fn, upload_cx* up, bool useSRGB)
	{
		dvk_texture* tex = new dvk_texture(up->_dev);
		if (!tex->InitFromFile(up, fn, useSRGB))
			delop(tex);
		up->flush();
		//else
			//up->flushAndFinish(UINT64_MAX);
		return tex;
	}
	dvk_texture* dvk_texture::new_storage2d(Image* img, upload_cx* up, bool is_compute)
	{
		return new_image2d(img, up, is_compute ? ImageLayoutBarrier::ComputeGeneralRW : ImageLayoutBarrier::PixelGeneralRW, true);
	}
	dvk_texture* dvk_texture::new_storage2d(const std::string& filename, upload_cx* up, bool is_compute)
	{
		return dvk_texture::new2d_priv(filename, up, is_compute ? ImageLayoutBarrier::ComputeGeneralRW : ImageLayoutBarrier::PixelGeneralRW
			, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
	}

	dvk_texture* dvk_texture::new_storage2d(dvk_device* dev, int width, int height, uint32_t format, bool is_compute, bool sampled)
	{
		auto p = new dvk_texture(dev);
		if (p)
		{
			do {
				IMG_INFO info = {};
				if (!(width * height > 0))break;
				info.width = width;
				info.height = height;
				info.arraySize = 1;
				info.mipMapCount = 1;
				info.depth = 1;
				info.vkformat = VK_FORMAT_R8G8B8A8_UNORM;
				p->_header = &info;

				uint32_t usage = VK_IMAGE_USAGE_STORAGE_BIT;
				if (sampled)usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
				auto ilb = is_compute ? ImageLayoutBarrier::ComputeGeneralRW : ImageLayoutBarrier::PixelGeneralRW;
				p->_info->usage |= usage;
				p->CreateTextureCommitted(0, 0, false, usage);
				//p->new_storage(width, height, format, is_compute, sampled);
			} while (0);
		}
		return p;
	}
	dvk_texture* dvk_texture::new_render_target_color(dvk_device* dev, int width, int height, uint32_t format, uint32_t sampleCount)
	{
		auto p = new dvk_texture(dev);
		if (p)
		{
			p->InitRenderTarget(dev, width, height, (VkFormat)format, (VkSampleCountFlagBits)sampleCount
				, (VkImageUsageFlags)(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT), false, "");
			p->CreateSRV(&p->_view);
			//p->mkrt_image(width, height, format, false, 0, ImageLayoutBarrier::COLOR_ATTACHMENT);
		}
		return p;
	}

	dvk_texture* dvk_texture::new_render_target_depth(dvk_device* dev, int width, int height, uint32_t format, uint32_t sampleCount)
	{
		auto p = new dvk_texture();
		if (p)
		{
			p->InitDepthStencil(dev, width, height, (VkFormat)format, (VkSampleCountFlagBits)sampleCount, "DepthBuffer");
			//p->CreateDSV(&m_DepthBufferDSV);
			//p->CreateRTV(&m_DepthBufferSRV);
			//p->mkrt_image(width, height, format, false, 0, ImageLayoutBarrier::DEPTH_STENCIL_ATTACHMENT);
		}
		return p;
	}


	enum YCbCrStorageFormat
	{
		NV12
	};

	unsigned char* ReadYCbCrFile(const std::string& filename, YCbCrStorageFormat storage_format, VkFormat vulkan_format, uint32_t* buffer_size, uint32_t* buffer_offset_plane1, uint32_t* buffer_offset_plane2)
	{
		std::ifstream file;
		file.open(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
		if (!file.is_open()) { log_format("Failed to open YCbCr image"); }
		*buffer_size = file.tellg();
		file.seekg(0);

		unsigned char* data = 0;
		switch (storage_format)
		{
		case NV12:
		{
			if (vulkan_format != VK_FORMAT_G8_B8R8_2PLANE_420_UNORM)
			{
				log_format("A 1:1 relationship doesn't exist between NV12 and 420, exiting");
				exit(1);
			}
			*buffer_offset_plane1 = (*buffer_size / 3) * 2;
			*buffer_offset_plane2 = 0; //Not used
			data = new unsigned char[*buffer_size];
			file.read((char*)(data), *buffer_size);
			break;
		}
		default:
			log_format("A YCbCr storage format is required");
			break;
		}

		file.close();
		return data;
	}
	struct yuv_info_t0 {
		void* data[3] = {};
		uint32_t size[3] = {};
		uint32_t width = 0, height = 0;
		int8_t format;			// 0=420, 1=422, 2=444
		int8_t b = 8;			// 8,10, 12,16
		int8_t t = 0;			// 1plane时422才有0=gbr, 1=brg
		int8_t plane = 0;		// 1 2 3

	};
	VkFormat get_format_yuv(yuv_info_t& p)
	{
		VkFormat r = VK_FORMAT_UNDEFINED;
		int x[] = { 0,6,14 };
		int x1[] = { 4,5 };
		VkFormat f[] = {
			VK_FORMAT_G8_B8R8_2PLANE_420_UNORM /*= 1000156003*/,
			VK_FORMAT_G16_B16R16_2PLANE_420_UNORM /*= 1000156030*/,
			VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM /*= 1000156002*/,
			VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM /*= 1000156029*/,
			VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
			VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
			//6
			VK_FORMAT_G8B8G8R8_422_UNORM /*= 1000156000*/,
			VK_FORMAT_B8G8R8G8_422_UNORM /*= 1000156001*/,
			VK_FORMAT_G16B16G16R16_422_UNORM /*= 1000156027*/,
			VK_FORMAT_B16G16R16G16_422_UNORM /*= 1000156028*/,

			VK_FORMAT_G8_B8R8_2PLANE_422_UNORM /*= 1000156005*/,
			VK_FORMAT_G16_B16R16_2PLANE_422_UNORM /*= 1000156032*/,
			VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM /*= 1000156004*/,
			VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM /*= 1000156031*/,
			//14
			VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM /*= 1000156006*/,
			VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM /*= 1000156033*/,
		};

		p.plane = (p.data[0]) ? 1 : 0;
		if (p.data[1])
		{
			p.plane++;
			if (p.data[2])
			{
				p.plane++;
			}
		}
		assert(p.plane);
		int i = 0;
		i += ((p.b / 8) - 1);
		do {
			if (p.format == 1)
			{
				if (p.plane < 2)
				{
					i *= 2;
					i += p.t;
					break;
				}
				else
				{
					i += 4;
				}
			}
			if (p.format != 2)
			{
				i += (p.plane - 2) * 2;
			}
		} while (0);
		if (p.format == 0)
		{
			if (p.b == 10)
				i = x1[0];
			if (p.b == 12)
				i = x1[1];
		}
		i += x[p.format];
		assert(i < (sizeof(f) / sizeof(f[0])));
		r = f[i];
		return r;
	}

	dvk_texture* dvk_texture::new_yuv(yuv_info_t yuv, upload_cx* up)
	{
		assert(up && up->_dev);
		dvk_device* dev = up->_dev;
		dvk_texture* tex = 0;
		auto physical_device = dev->physicalDevice;
		auto logical_device = dev->device;

		uint32_t width = yuv.width, height = yuv.height;

		VkFormat format = get_format_yuv(yuv);

		VkImage image = {};
		VkDeviceMemory image_memory;
		//VkDeviceSize memory_offset_plane0, memory_offset_plane1, memory_offset_plane2;
		VkImageView image_view = {};


		// 创建采样器

		VkSamplerYcbcrConversion ycbcr_sampler_conversion;
		VkSamplerYcbcrConversionInfo ycbcr_info;
		VkSampler ycbcr_sampler;
		VkFormatProperties format_properties;
		vkGetPhysicalDeviceFormatProperties(physical_device, format, &format_properties);
		bool cosited = false, midpoint = false;
		if (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT)
		{
			cosited = true;
		}
		else if (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT)
		{
			midpoint = true;
		}
		if (!cosited && !midpoint)
		{
			log_format("Nither VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT nor VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT is supported for VK_FORMAT_G8_B8R8_2PLANE_420_UNORM");
		}
		const VkSamplerCreateInfo defaultSamplerInfo = {
		   VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO, NULL, 0, VK_FILTER_LINEAR,  VK_FILTER_LINEAR,  VK_SAMPLER_MIPMAP_MODE_NEAREST,
		   VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		   // mipLodBias  anisotropyEnable  maxAnisotropy  compareEnable      compareOp         minLod  maxLod          borderColor                   unnormalizedCoordinates
				0.0,          false,            0.00,         false,       VK_COMPARE_OP_NEVER,   0.0,   16.0,    VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,        false };

		VkSamplerYcbcrConversionCreateInfo conversion_info = {};
		conversion_info.sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO;
		conversion_info.pNext = NULL;
		conversion_info.format = format;
		conversion_info.ycbcrModel = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_709;
		conversion_info.ycbcrRange = VK_SAMPLER_YCBCR_RANGE_ITU_FULL;
		conversion_info.components.r =
			conversion_info.components.g =
			conversion_info.components.b =
			conversion_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		if (cosited)
		{
			conversion_info.xChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;
			conversion_info.yChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;
		}
		else
		{
			conversion_info.xChromaOffset = VK_CHROMA_LOCATION_MIDPOINT;
			conversion_info.yChromaOffset = VK_CHROMA_LOCATION_MIDPOINT;
		}
		conversion_info.chromaFilter = VK_FILTER_LINEAR;
		conversion_info.forceExplicitReconstruction = VK_FALSE;
		VkResult res = vkCreateSamplerYcbcrConversion(logical_device, &conversion_info, NULL, &ycbcr_sampler_conversion);
		//CHECK_RESULT(res, "Failed to create YCbCr conversion sampler");
		assert(res == VK_SUCCESS);
		log_format("Successfully created YCbCr conversion");

		ycbcr_info.sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO;
		ycbcr_info.pNext = NULL;
		ycbcr_info.conversion = ycbcr_sampler_conversion;

		VkSamplerCreateInfo sampler_info = {};
		sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_info.pNext = &ycbcr_info;
		sampler_info.flags = 0;
		sampler_info.magFilter = VK_FILTER_LINEAR;
		sampler_info.minFilter = VK_FILTER_LINEAR;
		sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler_info.mipLodBias = 0.0f;
		sampler_info.anisotropyEnable = VK_FALSE;
		//sampler_info.maxAnisotropy IGNORED
		sampler_info.compareEnable = VK_FALSE;
		//sampler_info.compareOp =  IGNORED
		sampler_info.minLod = 0.0f;
		sampler_info.maxLod = 1.0f;
		sampler_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		res = vkCreateSampler(logical_device, &sampler_info, NULL, &ycbcr_sampler);
		//CHECK_VK_RESULT(res, "Failed to create YUV sampler");
		assert(res == VK_SUCCESS);
		log_format("Successfully created sampler with YCbCr in pNext");

		//uint32_t buffer_size, buffer_offset_plane1, buffer_offset_plane2;
		//unsigned char* ycbcr_data = ReadYCbCrFile(filename, NV12, VK_FORMAT_G8_B8R8_2PLANE_420_UNORM, &buffer_size, &buffer_offset_plane1, &buffer_offset_plane2);

		//Load image into staging buffer
		//VkDeviceMemory stage_buffer_memory;
		//VkBuffer stage_buffer;// = create_vk_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stage_buffer_memory);
		//void* stage_memory_ptr;
		//vkMapMemory(logical_device, stage_buffer_memory, 0, buffer_size, 0, &stage_memory_ptr);
		//memcpy(stage_memory_ptr, ycbcr_data, buffer_size);
		//vkUnmapMemory(logical_device, stage_buffer_memory);
		//delete[] ycbcr_data;

		//Create image
		VkImageCreateInfo img_info = {};
		img_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		img_info.flags = VK_IMAGE_CREATE_DISJOINT_BIT;
		img_info.imageType = VK_IMAGE_TYPE_2D;
		img_info.extent.width = width;
		img_info.extent.height = height;
		img_info.extent.depth = 1;
		img_info.mipLevels = 1;
		img_info.arrayLayers = 1;
		img_info.format = format;
		img_info.tiling = VK_IMAGE_TILING_LINEAR;//VK_IMAGE_TILING_OPTIMAL;
		img_info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
		img_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		img_info.samples = VK_SAMPLE_COUNT_1_BIT;
		img_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VkResult result = vkCreateImage(logical_device, &img_info, NULL, &image);
		//VK_CHECK_RESULT(result, "vkCreateImage failed to create image handle");
		log_format("Image created!");

		//Get memory requirements for each plane and combine
		//Plane 0
		VkImagePlaneMemoryRequirementsInfo image_plane_info = {};
		image_plane_info.sType = VK_STRUCTURE_TYPE_IMAGE_PLANE_MEMORY_REQUIREMENTS_INFO;
		image_plane_info.pNext = NULL;
		image_plane_info.planeAspect = VK_IMAGE_ASPECT_PLANE_0_BIT;
		VkImageMemoryRequirementsInfo2 image_info2 = {};
		image_info2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2;
		image_info2.pNext = &image_plane_info;
		image_info2.image = image;
		VkImagePlaneMemoryRequirementsInfo memory_plane_requirements = {};
		memory_plane_requirements.sType = VK_STRUCTURE_TYPE_IMAGE_PLANE_MEMORY_REQUIREMENTS_INFO;
		memory_plane_requirements.pNext = NULL;
		memory_plane_requirements.planeAspect = VK_IMAGE_ASPECT_PLANE_0_BIT;
		VkMemoryRequirements2 memory_requirements2 = {};
		memory_requirements2.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
		memory_requirements2.pNext = &memory_plane_requirements;
		vkGetImageMemoryRequirements2(logical_device, &image_info2, &memory_requirements2);
		VkDeviceSize image_pos[3] = {};
		VkDeviceSize image_size = memory_requirements2.memoryRequirements.size;
		uint32_t image_bits = memory_requirements2.memoryRequirements.memoryTypeBits;
		//Set offsets
		image_pos[0] = 0;
		//Plane 1
		image_plane_info.planeAspect = VK_IMAGE_ASPECT_PLANE_1_BIT;
		memory_plane_requirements.planeAspect = VK_IMAGE_ASPECT_PLANE_1_BIT;
		vkGetImageMemoryRequirements2(logical_device, &image_info2, &memory_requirements2);
		image_pos[1] = image_size;
		image_size += memory_requirements2.memoryRequirements.size;
		image_bits = image_bits | memory_requirements2.memoryRequirements.memoryTypeBits;

		//Plane 2
		if (yuv.size[2])
		{
			image_pos[2] = image_size;
			image_plane_info.planeAspect = VK_IMAGE_ASPECT_PLANE_2_BIT;
			memory_plane_requirements.planeAspect = VK_IMAGE_ASPECT_PLANE_2_BIT;
			vkGetImageMemoryRequirements2(logical_device, &image_info2, &memory_requirements2);
			image_size += memory_requirements2.memoryRequirements.size;
			image_bits = image_bits | memory_requirements2.memoryRequirements.memoryTypeBits;

		}
		uint32_t buffer_size = image_size;
		//Allocate image memory
		VkMemoryAllocateInfo allocate_info = {};
		allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocate_info.allocationSize = image_size;

		allocate_info.memoryTypeIndex = dev->get_device_memory_type(image_bits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		result = vkAllocateMemory(logical_device, &allocate_info, NULL, &image_memory);
		//CHECK_VK_RESULT(result, "vkAllocateMemory failed to allocate image memory");
		assert(result == VK_SUCCESS);
		//Bind each image plane to memory
		std::vector<VkBindImageMemoryInfo> bind_image_memory_infos(3);
		//Plane 0
		VkBindImagePlaneMemoryInfo bind_image_plane0_info = {};
		bind_image_plane0_info.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_PLANE_MEMORY_INFO;
		bind_image_plane0_info.pNext = NULL;
		bind_image_plane0_info.planeAspect = VK_IMAGE_ASPECT_PLANE_0_BIT;
		VkBindImageMemoryInfo& bind_image_memory_plane0_info = bind_image_memory_infos[0];
		bind_image_memory_plane0_info.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
		bind_image_memory_plane0_info.pNext = &bind_image_plane0_info;
		bind_image_memory_plane0_info.image = image;
		bind_image_memory_plane0_info.memory = image_memory;
		bind_image_memory_plane0_info.memoryOffset = image_pos[0];
		//Plane 1
		VkBindImagePlaneMemoryInfo bind_image_plane1_info = {};
		bind_image_plane1_info.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_PLANE_MEMORY_INFO;
		bind_image_plane1_info.pNext = NULL;
		bind_image_plane1_info.planeAspect = VK_IMAGE_ASPECT_PLANE_1_BIT;
		VkBindImageMemoryInfo& bind_image_memory_plane1_info = bind_image_memory_infos[1];
		bind_image_memory_plane1_info.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
		bind_image_memory_plane1_info.pNext = &bind_image_plane1_info;
		bind_image_memory_plane1_info.image = image;
		bind_image_memory_plane1_info.memory = image_memory;
		bind_image_memory_plane1_info.memoryOffset = image_pos[1];
		//Plane 2
		VkBindImagePlaneMemoryInfo p2 = {};

		if (yuv.size[2])
		{
			p2.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_PLANE_MEMORY_INFO;
			p2.pNext = NULL;
			p2.planeAspect = VK_IMAGE_ASPECT_PLANE_2_BIT;
			VkBindImageMemoryInfo& bind_image_memory_plane2_info = bind_image_memory_infos[2];
			bind_image_memory_plane2_info.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
			bind_image_memory_plane2_info.pNext = &p2;
			bind_image_memory_plane2_info.image = image;
			bind_image_memory_plane2_info.memory = image_memory;
			bind_image_memory_plane2_info.memoryOffset = image_pos[2];
		}
		else {
			bind_image_memory_infos.pop_back();
		}
		vkBindImageMemory2(logical_device, bind_image_memory_infos.size(), bind_image_memory_infos.data());

		VkImageViewCreateInfo image_view_info = {};
		image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		image_view_info.pNext = &ycbcr_info;
		image_view_info.flags = 0;
		image_view_info.image = image;
		image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		image_view_info.format = format;
		image_view_info.components.r = VK_COMPONENT_SWIZZLE_R;// VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_info.components.g = VK_COMPONENT_SWIZZLE_G;
		image_view_info.components.b = VK_COMPONENT_SWIZZLE_B;
		image_view_info.components.a = VK_COMPONENT_SWIZZLE_A;
		image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_view_info.subresourceRange.baseMipLevel = 0;
		image_view_info.subresourceRange.levelCount = 1;
		image_view_info.subresourceRange.baseArrayLayer = 0;
		image_view_info.subresourceRange.layerCount = 1;
		res = vkCreateImageView(logical_device, &image_view_info, NULL, &image_view);
		//CHECK_VK_RESULT(res, "Failed to create image view");
		assert(res == VK_SUCCESS);
		tex = new dvk_texture(up->_dev);
		if (tex)
		{
			tex->_image = image;
			tex->_view = image_view;
			tex->_format = format;
			tex->width = width;
			tex->height = height;
			//tex->imageLayout = GetImageLayout(imageLayout);
			tex->image_memory = image_memory;
			tex->sampler = ycbcr_sampler;
			tex->_dev = dev;
			tex->mipLevels = 1;
			tex->layerCount = 1;
			tex->_info->usage = img_info.usage;
			tex->ycbcr_sampler_conversion = ycbcr_sampler_conversion;
			tex->up_yuv(yuv, up);
		}
		return tex;
	}

	void dvk_texture::up_yuv(yuv_info_t yi, upload_cx* up)
	{
		if (!yi.data[0] || !up)return;
		assert(up && up->_dev);
		dvk_device* dev = up->_dev;
		dvk_texture* tex = 0;
		auto physical_device = dev->physicalDevice;
		auto logical_device = dev->device;
		//assert(yuv.width == yi.width);
		yuv = yi;
		uint32_t width = yuv.width, height = yuv.height;

		//Get memory requirements for each plane and combine
//Plane 0
		VkImagePlaneMemoryRequirementsInfo image_plane_info = {};
		image_plane_info.sType = VK_STRUCTURE_TYPE_IMAGE_PLANE_MEMORY_REQUIREMENTS_INFO;
		image_plane_info.pNext = NULL;
		image_plane_info.planeAspect = VK_IMAGE_ASPECT_PLANE_0_BIT;
		VkImageMemoryRequirementsInfo2 image_info2 = {};
		image_info2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2;
		image_info2.pNext = &image_plane_info;
		image_info2.image = _image;
		VkImagePlaneMemoryRequirementsInfo memory_plane_requirements = {};
		memory_plane_requirements.sType = VK_STRUCTURE_TYPE_IMAGE_PLANE_MEMORY_REQUIREMENTS_INFO;
		memory_plane_requirements.pNext = NULL;
		memory_plane_requirements.planeAspect = VK_IMAGE_ASPECT_PLANE_0_BIT;
		VkMemoryRequirements2 memory_requirements2 = {};
		memory_requirements2.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
		memory_requirements2.pNext = &memory_plane_requirements;
		vkGetImageMemoryRequirements2(logical_device, &image_info2, &memory_requirements2);
		VkDeviceSize image_pos[3] = {};
		VkDeviceSize image_size = memory_requirements2.memoryRequirements.size;
		uint32_t image_bits = memory_requirements2.memoryRequirements.memoryTypeBits;
		//Set offsets
		image_pos[0] = 0;
		//Plane 1
		image_plane_info.planeAspect = VK_IMAGE_ASPECT_PLANE_1_BIT;
		memory_plane_requirements.planeAspect = VK_IMAGE_ASPECT_PLANE_1_BIT;
		vkGetImageMemoryRequirements2(logical_device, &image_info2, &memory_requirements2);
		image_pos[1] = image_size;
		image_size += memory_requirements2.memoryRequirements.size;
		image_bits = image_bits | memory_requirements2.memoryRequirements.memoryTypeBits;

		//Plane 2
		if (yuv.size[2])
		{
			image_pos[2] = image_size;
			image_plane_info.planeAspect = VK_IMAGE_ASPECT_PLANE_2_BIT;
			memory_plane_requirements.planeAspect = VK_IMAGE_ASPECT_PLANE_2_BIT;
			vkGetImageMemoryRequirements2(logical_device, &image_info2, &memory_requirements2);
			image_size += memory_requirements2.memoryRequirements.size;
			image_bits = image_bits | memory_requirements2.memoryRequirements.memoryTypeBits;

		}

		uint32_t buffer_size = image_size;
		auto mp = up->get_tbuf(buffer_size, 512);
		if (!mp)
		{
			mp = up->get_tbuf(buffer_size, 512);
		}
		auto lasize = up->last_pos;
		for (size_t i = 0; i < 3; i++)
		{
			if (yuv.data[i])
				memcpy(mp + image_pos[i], yuv.data[i], yuv.size[i]);
		}

		up->add_pre(_image, (VkFormat)_format, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		//Copy staging buffer to device local buffer
		//VkCommandBuffer tmp_cmd_buffer = begin_tmp_vk_cmd_buffer();
		VkImageAspectFlags atm[] = { VK_IMAGE_ASPECT_PLANE_0_BIT, VK_IMAGE_ASPECT_PLANE_1_BIT,VK_IMAGE_ASPECT_PLANE_2_BIT };
		std::vector<VkBufferImageCopy> plane_regions(1);
		plane_regions[0].bufferOffset = up->last_pos;
		plane_regions[0].bufferRowLength = 0;
		plane_regions[0].bufferImageHeight = 0;
		plane_regions[0].imageSubresource.aspectMask = VK_IMAGE_ASPECT_PLANE_0_BIT;
		plane_regions[0].imageSubresource.mipLevel = 0;
		plane_regions[0].imageSubresource.baseArrayLayer = 0;
		plane_regions[0].imageSubresource.layerCount = 1;
		plane_regions[0].imageOffset = { 0, 0, 0 };
		plane_regions[0].imageExtent = { width, height, 1 };
		plane_regions.resize(yuv.size[2] ? 3 : 2);
		int n = plane_regions.size();
		for (int i = 1; i < n; i++)
		{
			plane_regions[i].bufferOffset = up->last_pos + image_pos[i];
			plane_regions[i].bufferRowLength = 0;
			plane_regions[i].bufferImageHeight = 0;
			plane_regions[i].imageSubresource.aspectMask = atm[i];
			plane_regions[i].imageSubresource.mipLevel = 0;
			plane_regions[i].imageSubresource.baseArrayLayer = 0;
			plane_regions[i].imageSubresource.layerCount = 1;
			plane_regions[i].imageOffset = { 0, 0, 0 };
			plane_regions[i].imageExtent = { width / 2, height / 2, 1 };
		}
		for (auto& it : plane_regions)
		{
			up->addCopy(_image, it);
		}
		//vkCmdCopyBufferToImage(tmp_cmd_buffer, stage_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, plane_regions.size(), plane_regions.data());

		//end_tmp_vk_cmd_buffer(tmp_cmd_buffer); //Submit and waits

		up->add_post(_image, (VkFormat)_format, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		up->flush();
	}


	VkDescriptorImageInfo* dvk_texture::get_descriptor_image_info()
	{
		if (!descriptor->imageView)descriptor->imageView = _view;
		if (!descriptor->sampler)descriptor->sampler = sampler ? sampler : _dev->_sampler;
		assert(descriptor->imageView);
		assert(descriptor->sampler);
		return descriptor;
	}

#endif // 1

#ifndef no_vkqueue

	submit_info::submit_info()
	{
	}
	submit_info::~submit_info()
	{
	}

	void submit_info::clear()
	{
		pCommandBuffers.clear();
		pSignalSemaphores.clear();
		pWaitSemaphores.clear();
		pWaitDstStageMask.clear();
	}

	void submit_info::push_cmd(VkCommandBuffer cmd, VkSemaphore signal /*= 0*/, VkSemaphore wait /*= 0*/, uint32_t wait_dst_stage_mask/* = 0*/)
	{
		if (cmd)
			pCommandBuffers.push_back(cmd);
		if (signal)
			pSignalSemaphores.push_back(signal);
		if (wait)
			pWaitSemaphores.push_back(wait);
		if (wait_dst_stage_mask)
			pWaitDstStageMask.push_back(wait_dst_stage_mask);
	}

	VkSemaphore submit_info::get_idx_sem(size_t idx)
	{
		if (idx < pSignalSemaphores.size())
		{
			return pSignalSemaphores[idx];
		}
		return 0;
	}



	present_info::present_info(bool is_rts)
	{
		if (is_rts)
			pResults = new std::vector<VkResult>();
	}
	present_info::~present_info()
	{
		if (pResults)
		{
			auto pr = (std::vector<VkResult>*)pResults;
			delete pr; pResults = 0;
		}
	}
	void present_info::push_swapchain(VkSwapchainKHR swapchain, VkSemaphore WaitSemaphores)
	{
		pSwapchains.push_back(swapchain);
		pImageIndices.push_back(0);
		//if (WaitSemaphores)
		{
			pWaitSemaphores.push_back(WaitSemaphores);
		}
	}

	void present_info::push(dvk_swapchain* sp)
	{
		//push_swapchain(sp->swapChain, 0);
		_swapchains[sp];
	}

	void present_info::update()
	{
		reset();
		size_t i = 0;
		for (auto& [k, v] : _swapchains)
		{
			if (!k || !k->swapChain)continue;
			pSwapchains[i] = (k->swapChain);
			pImageIndices[i] = (v.idx);
			pWaitSemaphores[i] = (v.sem);
			i++;
		}
	}

	void present_info::set_idx(dvk_swapchain* sp, uint32_t image_indices, VkSemaphore WaitSemaphores)
	{
		_swapchains[sp].idx = image_indices;
		_swapchains[sp].sem = WaitSemaphores;
	}

	void present_info::erase(dvk_swapchain* sp)
	{
		_swapchains.erase(sp);
	}

	size_t present_info::size()
	{
		return _swapchains.size();
	}

	void present_info::reset()
	{
		auto scs = _swapchains.size();
		if (scs != pSwapchains.size())
		{
			pSwapchains.resize(scs);
			pImageIndices.resize(scs);
			pWaitSemaphores.resize(scs);
			if (pResults)
			{
				auto pr = (std::vector<VkResult>*)pResults;
				pr->resize(scs);
			}
		}
	}


	dvk_queue::dvk_queue(dvk_device* dev, VkQueue q, uint32_t fmily_indices, dvk_queue_bit t) : _queue(q), _fmily_indices(fmily_indices), _t(t)
		, _dev(dev)
	{

	}

	dvk_queue::~dvk_queue()
	{

	}

	dvk_cmd_pool* dvk_queue::new_cmd_pool()
	{
		auto cp = _dev->new_cmd_pool(_fmily_indices);
		auto p = new dvk_cmd_pool(cp, _dev);
		return p;
	}

	void dvk_queue::free_cmd_pool(dvk_cmd_pool* cp)
	{
		if (cp)
		{
			delete cp;
		}
	}


	uint32_t dvk_queue::submit(submit_infos* sub, dvk_fence* fence, bool is_wait)
	{
		assert(sub);
		uint32_t ret = 0;
		sub->update_sub();
		if (sub->n)
		{
			if (fence)
			{
				fence->wait_for();
				fence->reset();
			}
			VkFence vkf = fence ? fence->get() : nullptr;
			auto rt = vkQueueSubmit(_queue, sub->n, sub->data(), vkf);
			if (rt)
			{
				printf("submit: %d\terrorid:%d\n", sub->size(), rt);
			}
			int fs = 1;
			if (fence)
				fs = fence->get_status();
			if (is_wait && fence)
				fence->wait_for();
			ret = rt;
		}
		return ret;
	}
	uint32_t dvk_queue::bind_sparse(bind_sparses* bs, dvk_fence* fence, bool is_wait)
	{
		assert(bs);
		uint32_t ret = 0;
		auto submitInfo = (std::vector<VkBindSparseInfo>*)bs;
		if (submitInfo->size())
		{
			fence->reset();
			auto rt = vkQueueBindSparse(_queue, submitInfo->size(), submitInfo->data(), fence->get());
			if (is_wait)
				fence->wait_for();
			ret = rt;
		}
		return ret;
	}
	uint32_t dvk_queue::present(present_info* p)
	{
		assert(p && p->pSwapchains.size() > 0 && p->pSwapchains.size() == p->pImageIndices.size());

		if (!p || p->pSwapchains.empty() || p->pSwapchains.size() != p->pImageIndices.size())
		{
			return VK_ERROR_UNKNOWN;
		}
		// 如果有fence则等待提交完成
		//if (p->submit_fence)
		//{
		//	p->submit_fence->wait_for();
		//	p->submit_fence->reset();
		//}
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = NULL;
		presentInfo.waitSemaphoreCount = p->pWaitSemaphores.size();
		presentInfo.pWaitSemaphores = p->pWaitSemaphores.size() ? p->pWaitSemaphores.data() : nullptr;
		presentInfo.swapchainCount = p->pSwapchains.size();
		presentInfo.pSwapchains = p->pSwapchains.data();
		presentInfo.pImageIndices = p->pImageIndices.size() ? p->pImageIndices.data() : nullptr;
		auto pResults = (std::vector<VkResult>*)p->pResults;
		presentInfo.pResults = pResults && pResults->size() ? pResults->data() : nullptr;
		auto r = vkQueuePresentKHR(_queue, &presentInfo);

		if (p->submit_fence)
		{
			p->submit_fence->wait_for();
		}
		if (r == VK_ERROR_OUT_OF_DATE_KHR) {
			std::string logstr = hz::format("vkQueuePresentKHR:<VK_ERROR_OUT_OF_DATE_KHR>\n");
			dslog(logstr.c_str(), logstr.size());
		}
		else if (r) {
			std::string logstr = hz::format("vkQueuePresentKHR:<%s>%d\n", vkc::errorString(r).c_str(), r);
			dslog(logstr.c_str(), logstr.size());
			//assert(r != VK_ERROR_DEVICE_LOST);
		}
		//r = vkQueueWaitIdle(_queue);
		return r;
	}
#endif
	submit_infos::submit_infos()
	{
	}
	submit_infos::~submit_infos()
	{
	}

	void submit_infos::push(submit_info* psi)
	{
		auto r = st.insert(psi).second;

		return;
	}

	void submit_infos::update_sub()
	{
		if (st.empty())
		{
			return;
		}
		if (cap < st.size())
		{
			if (v)
				uac.free_mem(v, cap);
			cap = st.size();
			uac.new_mem(cap, v);
		}
		auto t = v;
		n = 0;
		for (auto it : st)
		{
			if (it)
			{
				n++;
				VkSubmitInfo& dt = *t;
				uint32_t waitSemaphoreCount = it->pWaitSemaphores.size();
				uint32_t commandBufferCount = it->pCommandBuffers.size();
				uint32_t signalSemaphoreCount = it->pSignalSemaphores.size();
				dt.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				dt.commandBufferCount = commandBufferCount;
				dt.pCommandBuffers = it->pCommandBuffers.data();
				dt.signalSemaphoreCount = signalSemaphoreCount;
				dt.pSignalSemaphores = it->pSignalSemaphores.data();
				if (waitSemaphoreCount > 0)
				{
					dt.waitSemaphoreCount = waitSemaphoreCount;
					dt.pWaitSemaphores = it->pWaitSemaphores.data();
					dt.pWaitDstStageMask = it->pWaitDstStageMask.data();
				}
				t++;
			}
		}
	}
	void submit_infos::clear()
	{
		st.clear();
	}
	VkSubmitInfo* submit_infos::data()
	{
		return v;
	}
	size_t submit_infos::size()
	{
		return st.size();
	}
	game_time::game_time()
	{
	}
	game_time::~game_time()
	{
	}

	void game_time::init()
	{
		g_CurrTime = get_secondsf();
		_tid = hz::get_tid();
	}
	// 每帧绘制前调用
	void game_time::update()
	{
		// 获取时间(秒)
		double nowT = get_secondsf();
		double delta = nowT - g_LastTime;
		g_LastTime = nowT;
		auto ct = g_CurrTime;
		//auto ti = ctu.elapsed();
		//delta = 0.001 * ti;
		UpdateFPS(ct, delta);
		uptime(ct, delta);
		//sleep(128);

		g_CurrTime = g_CurrTime + delta;
	}

	float game_time::get_fps()
	{
		return _LastFPS;
	}
	void game_time::UpdateFPS(double time, double delta)
	{
		_FrameCounter++;
		_LastFrameTime += delta;
		if (_LastFrameTime >= 1.0f)
		{
			_LastFPS = _FrameCounter;
			_FrameCounter = 0;
			_LastFrameTime = 0.0f;
		}
	}
	void game_time::uptime(double time, double delta)
	{
		_gtime.captureFramerate = _temgt.captureFramerate;
		_gtime.timeScale = _temgt.timeScale;
		_gtime.fixedDeltaTime = _temgt.fixedDeltaTime;
		_gtime.inFixedTimeStep = _temgt.inFixedTimeStep;
		_gtime.maximumDeltaTime = _temgt.maximumDeltaTime;
		_gtime.maximumParticleDeltaTime = _temgt.maximumParticleDeltaTime;

		double st = delta * _gtime.timeScale;
		_gtime.deltaTime = st;
		_gtime.deltaTimei = ctu.elapsed();
		ctu.reset();
		if (!_pause)
		{
			_gtime.time += st;
			_gtime.timeSinceLevelLoad += st;
			_gtime.unscaledDeltaTime = delta;
			_gtime.unscaledTime += delta;
		}
		_gtime.realtimeSinceStartup += delta;
		if (_FrameCounter == 1) {
			_gtime.smoothDeltaTime = st;
		}
		else {
			_gtime.smoothDeltaTime *= (1.0 - alpha) + st * alpha;
		}
		_gtime.frameCount = _FrameCounter;
		_gtime.fps = _LastFPS;
		_temgt = _gtime;
	}
	int game_time::fpsup(double deltaTime) // ms
	{
		++frameCount;
		int fps = 0;
		if (1 == frameCount)
		{
			avgDuration = static_cast<double>(deltaTime);
		}
		else
		{
			avgDuration = avgDuration * (1 - alpha) + deltaTime * alpha;
		}
		_LastFPS1 = (1.f / avgDuration * 1000);
		return _LastFPS1;
	}
	float game_time::get_fps1()
	{
		return _LastFPS1;
	}
	bool game_time::nc_tid(uint64_t t)
	{
		return _tid != t;
	}
	void game_time::set_pause()
	{
		_pause = true;
	}
	void game_time::set_play()
	{
		_pause = false;
	}
	cpg_time* game_time::get_gt()
	{
		return &_temgt;
	}
	// !no_vkqueue

	// queue_ctx

	queue_ctx::queue_ctx(bool onethread) :_onethread(onethread)
	{
		pgt = &gt;
		if (onethread)
		{
			std::thread renderthr([=](bool* isexits)
				{
					hz::mp_timer tc;
					int tk = 0, dif = 0, ec = 0;
					while (*isexits)
					{
						loop();
						//int tk1 = tc.elapsed();
						//dif = tk1 - tk;
						//if (dif > 1000)
						//{
						//	tk = tk1;
						//	int fps = pgt->get_fps();
						//	printf("\x1b]0;fps:%d \x07", fps);
						//}
					}
					_end = true;
					printf("exit render thread\n");
				}, &isrun);
			_rthr.swap(renderthr);
		}
	}

	queue_ctx::~queue_ctx()
	{
		stop();
		if (_onethread)
			_rthr.join();
		if (present.submit_fence)
		{
			delete present.submit_fence;
			present.submit_fence = 0;
		}
	}

	void queue_ctx::stop()
	{
		isrun = false;
	}

	void queue_ctx::begin_thr()
	{
		isbegin = true;
	}

	bool* queue_ctx::get_rts()
	{
		return &isrun;
	}

	game_time* queue_ctx::get_pgt()
	{
		return &gt;
	}

	void queue_ctx::init(dvk_device* dev, size_t qidx)
	{
		_dev = dev;
		_qctx = dev->get_graphics_queue(qidx);
		present.submit_fence = new dvk_fence(_dev->device);
	}

	void queue_ctx::push(sdl_form* form1)
	{
		if (form1)
		{
			temqueue.push(form1);
		}
	}

	void queue_ctx::set_waitms(int ms, int minfps)
	{
		if (ms < 0)
		{
			ms = 0;
		}
		if (minfps < 1)
		{
			minfps = 1;
		}
		_waitms = ms;
		_minfps = minfps;
		_gfps = 1000.0 / minfps;
	}

	void queue_ctx::FixedUpdate(cpg_time* t)
	{

	}

	void queue_ctx::LateUpdate(cpg_time* t)
	{

	}

	void queue_ctx::game_logic(cpg_time* t)
	{
		if (update_cb1)
		{
			update_cb1(t);
		}
		LateUpdate(t);
	}

	void queue_ctx::game_update()
	{
		auto t = gt.get_gt();
		FixedUpdate(t);
		game_logic(t);
	}

	int queue_ctx::frame_it(sdl_form* it, fbo_cx* fbo1, glm::ivec2 fws, uint32_t* image_idx)
	{

		bool has_outofdate = false;
		int isc = 0;
		do {

#if 1
			if (fbo1->is_gs() || has_outofdate)
			{
				printf("resize\n");
				auto t = gt.get_gt();
				int w = 0, h = 0;
				{
					fbo1->_gstime = t->unscaledTime;
					w = fws.x;
					h = fws.y;
					fbo1->resize(w, h);
					fbo1->close_gs();
				}
#ifdef _WIN32
				if (fbo1->dev_resize(has_outofdate))
				{
					it->set_ptr(0);
					//it->set_viewport(-1,-1,w, h);
					//fbo1->inc_update();
				}
#endif
				fbo1->_upinc++;
			}
#endif
			isc = fbo1->acquire(image_idx);

			auto swap1 = fbo1->get_swapchain();
			if (isc == VK_ERROR_SURFACE_LOST_KHR)
			{
				present.erase(swap1);
				it->free_surface();
				it->set_draw();
				has_outofdate = true;
				break;
			}
			if (*image_idx > fbo1->count())
			{
				present.erase(swap1);
				it->free_surface();
				it->set_draw();
				it->reset_dev();
				has_outofdate = true;
				//assert(!isc);
				break;
			}

			int fup = fbo1->is_update();
			//log_format("frame_begin: %d,%d\n", isc, fup);
			if (isc == 0 && fbo1->is_update()) {
				fbo1->build_cmd_cb();
				fbo1->dec_update();
			}
			break;
		} while (1);
		return isc;
	}
	bool queue_ctx::frame_begin()
	{
		bool ret = false;
		auto form1 = temqueue.get_wait_one(1);
		if (form1)
		{
			// 添加窗口到显示
			_vforms.push_back(form1);
			auto fbo1 = form1->get_fbo();
			if (fbo1)
			{
				fbo1->set_queue(_qctx, &gt);
				if (!is_clear)
				{
					auto swap1 = fbo1->get_swapchain();
					present.push(swap1);
					//sub.insert_sub(fbo1->get_cur_submit());

				}
				fbo1->inc_update();
			}
		}
		if (_vforms.size())
			game_update();		// 更新数据
		// 更新交窗口换链相关
		for (size_t i = 0; i < _vforms.size(); i++)
		{
			uint32_t image_idx = 0;
			auto& it = _vforms[i];
			auto fbo1 = it->get_fbo();
			if (is_outofdate)
			{
				//it->set_nodraw();
				//it->set_draw();
			}
			it->reset_dev();
			auto swap1 = fbo1 ? fbo1->get_swapchain() : (dvk_swapchain*)0;
			if (it->is_close())
			{
				auto pit = it;
				_vforms.erase(_vforms.begin() + i);
				present.erase(swap1);
				pit->set_stop_draw();
				pit->is_free = true;
				printf("stop %p\n", pit);
				i--;
				continue;
			}
			if (!it->get_visible())
			{
				present.erase(swap1);
				//sub.erase(fbo1->get_cur_submit());
				continue;
			}


			if (it->is_draw() && fbo1)
			{
				auto fs1 = it->get_fsize();
				auto fws = it->get_size();
				if (fs1 != fws)
				{
					fbo1->set_gs();
				}
				int isc = frame_it(it, fbo1, fws, &image_idx);

				if (isc != 0)
					continue;
				auto sp = fbo1->get_cur_submit();
				sub.push(sp);
				if (is_clear)
				{
					subform.push_back(it);
					present.push(swap1);
				}
				present.set_idx(swap1, image_idx, sp->get_idx_sem(0));
				ret = true;
			}
			else
			{
				present.erase(swap1);
			}
		}
		present.update();
		if (_subsize != present.size())
		{
			is_clear = true;
			_subsize = present.size();
		}
		return ret;
	}

	void queue_ctx::frame_end()
	{
		if (_vforms.size())
		{
			uint32_t sr = 0;
			if (sub.size())
				sr = _qctx->submit(&sub, present.submit_fence, true);

			auto tid = get_tid();
			if (sr)
			{
				printf("submit: %d\terrorid:%d%lld\n", sub.size(), sr, tid);
			}

			if (sr == 0 && present.size())
			{
				VkResult hr = (VkResult)_qctx->present(&present);
				if (hr != 0)
				{
					log_format("present: %d\t%lld\t spresent.size()%d\n", hr, tid, (int)present.size());
					if (hr == VK_ERROR_OUT_OF_DATE_KHR)
					{
						is_outofdate = true;
					}
					hr = hr;
				}
				else {
					is_outofdate = false;
					// todo save 2file
					bool is = false;
					if (is)
					{
						for (size_t i = 0; i < _vforms.size(); i++)
						{
							auto& it = _vforms[i];
							if (it->get_visible())
							{
								auto fbo1 = it->get_fbo();
								fbo1->save_image("temp/form_fbo" + std::to_string(i), 0);
							}
						}
					}
				}
			}
			if (is_clear)
			{
				for (size_t i = 0; i < subform.size(); i++)
				{
					auto& it = subform[i];
					it->end_cb();
				}
				sub.clear();
				//present.clear();
				subform.clear();
				is_clear = false;
			}
		}
	}
	void queue_ctx::loop()
	{
		if (!isbegin) {
			sleep(8);
			return;
		}
		if (isinit)
		{
			isinit = false;
			if (_onethread)
				set_thr_name_cur("queue_ctx");
			pgt->init();
			if (init_cb)
				init_cb();
		}
		if (isrun)
		{
			pgt->update();

			if (frame_begin())
				frame_end();
			else
			{
				sleep(8);
			}
			int lms = _waitms;
			do {
				if (_minfps > 0)
				{
					auto gk = pgt->get_gt();
					double ms = gk->smoothDeltaTime * 1000;
					double dms = gk->deltaTime * 1000;
					if (ms < _gfps /*|| gk->fps > _minfps*/)
					{
						_minfps_ms = _gfps - (_gfps - ms) * 0.5;
						if (_minfps_ms < 2)
							_minfps_ms = 2;
						sleep(_minfps_ms);
						break;
					}
					else {
						lms = _gfps - ms;
						_minfps_ms = _minfps_ms;
					}
				}
				{
					if (lms < _waitms)lms = _waitms;
					sleep(lms);
				}
			} while (0);

		}
	}

	// !queue_ctx

	uint32_t GetFormat(const std::string& str, int id)
	{
		if (str == "SCALAR")
		{
			switch (id)
			{
			case 5120: return VK_FORMAT_R8_SINT; //(BYTE)
			case 5121: return VK_FORMAT_R8_UINT; //(UNSIGNED_BYTE)1
			case 5122: return VK_FORMAT_R16_SINT; //(SHORT)2
			case 5123: return VK_FORMAT_R16_UINT; //(UNSIGNED_SHORT)2
			case 5124: return VK_FORMAT_R32_SINT; //(SIGNED_INT)4
			case 5125: return VK_FORMAT_R32_UINT; //(UNSIGNED_INT)4
			case 5126: return VK_FORMAT_R32_SFLOAT; //(FLOAT)
			}
		}
		else if (str == "VEC2")
		{
			switch (id)
			{
			case 5120: return VK_FORMAT_R8G8_SINT; //(BYTE)
			case 5121: return VK_FORMAT_R8G8_UINT; //(UNSIGNED_BYTE)1
			case 5122: return VK_FORMAT_R16G16_SINT; //(SHORT)2
			case 5123: return VK_FORMAT_R16G16_UINT; //(UNSIGNED_SHORT)2
			case 5124: return VK_FORMAT_R32G32_SINT; //(SIGNED_INT)4
			case 5125: return VK_FORMAT_R32G32_UINT; //(UNSIGNED_INT)4
			case 5126: return VK_FORMAT_R32G32_SFLOAT; //(FLOAT)
			}
		}
		else if (str == "VEC3")
		{
			switch (id)
			{
			case 5120: return VK_FORMAT_UNDEFINED; //(BYTE)
			case 5121: return VK_FORMAT_UNDEFINED; //(UNSIGNED_BYTE)1
			case 5122: return VK_FORMAT_UNDEFINED; //(SHORT)2
			case 5123: return VK_FORMAT_UNDEFINED; //(UNSIGNED_SHORT)2
			case 5124: return VK_FORMAT_R32G32B32_SINT; //(SIGNED_INT)4
			case 5125: return VK_FORMAT_R32G32B32_UINT; //(UNSIGNED_INT)4
			case 5126: return VK_FORMAT_R32G32B32_SFLOAT; //(FLOAT)
			}
		}
		else if (str == "VEC4")
		{
			switch (id)
			{
			case 5120: return VK_FORMAT_R8G8B8A8_SINT; //(BYTE)
			case 5121: return VK_FORMAT_R8G8B8A8_UINT; //(UNSIGNED_BYTE)1
			case 5122: return VK_FORMAT_R16G16B16A16_SINT; //(SHORT)2
			case 5123: return VK_FORMAT_R16G16B16A16_UINT; //(UNSIGNED_SHORT)2
			case 5124: return VK_FORMAT_R32G32B32A32_SINT; //(SIGNED_INT)4
			case 5125: return VK_FORMAT_R32G32B32A32_UINT; //(UNSIGNED_INT)4
			case 5126: return VK_FORMAT_R32G32B32A32_SFLOAT; //(FLOAT)
			}
		}

		return VK_FORMAT_UNDEFINED;
	}

	uint32_t SizeOfFormat(uint32_t format)
	{
		switch (format)
		{
		case VK_FORMAT_R8_SINT: return 1;//(BYTE)
		case VK_FORMAT_R8_UINT: return 1;//(UNSIGNED_BYTE)1
		case VK_FORMAT_R16_SINT: return 2;//(SHORT)2
		case VK_FORMAT_R16_UINT: return 2;//(UNSIGNED_SHORT)2
		case VK_FORMAT_R32_SINT: return 4;//(SIGNED_INT)4
		case VK_FORMAT_R32_UINT: return 4;//(UNSIGNED_INT)4
		case VK_FORMAT_R32_SFLOAT: return 4;//(FLOAT)

		case VK_FORMAT_R8G8_SINT: return 2 * 1;//(BYTE)
		case VK_FORMAT_R8G8_UINT: return 2 * 1;//(UNSIGNED_BYTE)1
		case VK_FORMAT_R16G16_SINT: return 2 * 2;//(SHORT)2
		case VK_FORMAT_R16G16_UINT: return 2 * 2; // (UNSIGNED_SHORT)2
		case VK_FORMAT_R32G32_SINT: return 2 * 4;//(SIGNED_INT)4
		case VK_FORMAT_R32G32_UINT: return 2 * 4;//(UNSIGNED_INT)4
		case VK_FORMAT_R32G32_SFLOAT: return 2 * 4;//(FLOAT)

		case VK_FORMAT_UNDEFINED: return 0;//(BYTE) (UNSIGNED_BYTE) (SHORT) (UNSIGNED_SHORT)
		case VK_FORMAT_R32G32B32_SINT: return 3 * 4;//(SIGNED_INT)4
		case VK_FORMAT_R32G32B32_UINT: return 3 * 4;//(UNSIGNED_INT)4
		case VK_FORMAT_R32G32B32_SFLOAT: return 3 * 4;//(FLOAT)

		case VK_FORMAT_R8G8B8A8_SINT: return 4 * 1;//(BYTE)
		case VK_FORMAT_R8G8B8A8_UINT: return 4 * 1;//(UNSIGNED_BYTE)1
		case VK_FORMAT_R16G16B16A16_SINT: return 4 * 2;//(SHORT)2
		case VK_FORMAT_R16G16B16A16_UINT: return 4 * 2;//(UNSIGNED_SHORT)2
		case VK_FORMAT_R32G32B32A32_SINT: return 4 * 4;//(SIGNED_INT)4
		case VK_FORMAT_R32G32B32A32_UINT: return 4 * 4;//(UNSIGNED_INT)4
		case VK_FORMAT_R32G32B32A32_SFLOAT: return 4 * 4;//(FLOAT)
		}

		return 0;
	}

	dynamic_buffer_cx::dynamic_buffer_cx(dvk_device* d, size_t acsize, bool vibo, bool transfer) :dev(d)
	{
		assert(dev);
		if (dev)
		{
			_ubo_align = dev->get_ubo_align();
			if (!acsize)
			{
				acsize = dev->get_ubo_range();
			}
			uint32_t usage = 0;
			if (vibo)
			{
				usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			}
			if (transfer)
			{
				usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			}
			size_t a = 512;
			acsize = AlignUp(acsize, a);
			_ubo = dvk_buffer::new_ubo(dev, acsize, 0, usage);
			clear();
		}
	}

	dynamic_buffer_cx::~dynamic_buffer_cx()
	{
		delop(_ubo);
	}

	void dynamic_buffer_cx::append(size_t size)
	{
		size_t a = 512;
		size = AlignUp(size, a);
		_ubo->resize(_ubo->_size + size);
		mdata = (char*)_ubo->map(-1);
		assert(mdata);
	}

	char* dynamic_buffer_cx::get_ptr(uint32_t offset)
	{
		assert(mdata);
		return mdata + offset;
	}

	void dynamic_buffer_cx::flush(size_t pos, size_t size)
	{
		VkResult res = {};
		VkMappedMemoryRange range[1] = {};
		range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range[0].memory = _ubo->memory;
		range[0].size = size;
		res = vkFlushMappedMemoryRanges(dev->device, 1, range);
		assert(res == VK_SUCCESS);
	}

	void dynamic_buffer_cx::clear()
	{
		last = 0;
		if (!mdata)
			mdata = (char*)_ubo->map(-1);
	}

	void dynamic_buffer_cx::free_mem()
	{
		_ubo->destroybuf();
		mdata = 0;
	}

	char* dynamic_buffer_cx::alloc(size_t size, uint32_t* offset)
	{
		assert(mdata);
		uint32_t r = last;
		size_t a = _ubo_align;
		alignUp(size, a);
		auto nac = r + size;
		if (nac < 0 || nac > _ubo->_size)
		{
			assert("'dynamic' buffers空间不足!");
			return 0;
		}
		auto ret = mdata + r;
		if (offset)
		{
			*offset = r;
		}
		last = nac;
		return ret;
	}

	bool dynamic_buffer_cx::AllocConstantBuffer(uint32_t size, void** pData, VkDescriptorBufferInfo* pOut)
	{
		//size = AlignUp(size, 256u);

		uint32_t memOffset = 0;
		*pData = alloc(size, &memOffset);
		if (!*pData)
		{
			assert("Ran out of mem for 'dynamic' buffers, please increase the allocated size");
			return false;
		}

		pOut->buffer = _ubo->buffer;
		pOut->offset = memOffset;
		pOut->range = size;

		return true;
	}



	//--------------------------------------------------------------------------------------
	//
	// OnCreate
	//
	//--------------------------------------------------------------------------------------
	void UploadHeap::OnCreate(dvk_device* pDevice, size_t uSize)
	{
		dev = pDevice;

		VkResult res;

		// Create command list and allocators 
		{
			VkCommandPoolCreateInfo cmd_pool_info = {};
			cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			cmd_pool_info.queueFamilyIndex = dev->get_familyIndex(0);// GetGraphicsQueueFamilyIndex();
			cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			res = vkCreateCommandPool(dev->device, &cmd_pool_info, NULL, &m_commandPool);
			assert(res == VK_SUCCESS);

			VkCommandBufferAllocateInfo cmd = {};
			cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			cmd.pNext = NULL;
			cmd.commandPool = m_commandPool;
			cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			cmd.commandBufferCount = 1;
			res = vkAllocateCommandBuffers(dev->device, &cmd, &m_pCommandBuffer);
			assert(res == VK_SUCCESS);
		}

		// Create buffer to suballocate
		{
			VkBufferCreateInfo buffer_info = {};
			buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			buffer_info.size = uSize;
			buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			res = vkCreateBuffer(dev->device, &buffer_info, NULL, &m_buffer);
			assert(res == VK_SUCCESS);

			VkMemoryRequirements mem_reqs;
			vkGetBufferMemoryRequirements(dev->device, m_buffer, &mem_reqs);

			VkMemoryAllocateInfo alloc_info = {};
			alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			alloc_info.allocationSize = mem_reqs.size;
			alloc_info.memoryTypeIndex = 0;

			bool pass = memory_type_from_properties(dev->get_dmp(), mem_reqs.memoryTypeBits,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				&alloc_info.memoryTypeIndex);
			assert(pass && "No mappable, coherent memory");

			res = vkAllocateMemory(dev->device, &alloc_info, NULL, &m_deviceMemory);
			assert(res == VK_SUCCESS);

			res = vkBindBufferMemory(dev->device, m_buffer, m_deviceMemory, 0);
			assert(res == VK_SUCCESS);

			res = vkMapMemory(dev->device, m_deviceMemory, 0, mem_reqs.size, 0, (void**)&m_pDataBegin);
			assert(res == VK_SUCCESS);

			m_pDataCur = m_pDataBegin;
			m_pDataEnd = m_pDataBegin + mem_reqs.size;
		}

		// Create fence
		{
			VkFenceCreateInfo fence_ci = {};
			fence_ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

			res = vkCreateFence(dev->device, &fence_ci, NULL, &m_fence);
			assert(res == VK_SUCCESS);
		}

		// Begin Command Buffer
		{
			VkCommandBufferBeginInfo cmd_buf_info = {};
			cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			res = vkBeginCommandBuffer(m_pCommandBuffer, &cmd_buf_info);
			assert(res == VK_SUCCESS);
		}
	}

	//--------------------------------------------------------------------------------------
	//
	// OnDestroy
	//
	//--------------------------------------------------------------------------------------
	void UploadHeap::OnDestroy()
	{
		vkDestroyBuffer(dev->device, m_buffer, NULL);
		vkUnmapMemory(dev->device, m_deviceMemory);
		vkFreeMemory(dev->device, m_deviceMemory, NULL);

		vkFreeCommandBuffers(dev->device, m_commandPool, 1, &m_pCommandBuffer);
		vkDestroyCommandPool(dev->device, m_commandPool, NULL);

		vkDestroyFence(dev->device, m_fence, NULL);
	}

	//--------------------------------------------------------------------------------------
	//
	// SuballocateFromUploadHeap
	//
	//--------------------------------------------------------------------------------------
	uint8_t* UploadHeap::Suballocate(size_t uSize, uint64_t uAlign)
	{
		// wait until we are done flusing the heap
		flushing.Wait();

		uint8_t* pRet = NULL;

		{
			std::unique_lock<std::mutex> lock(m_mutex);

			// make sure resource (and its mips) would fit the upload heap, if not please make the upload heap bigger
			assert(uSize < (size_t)(m_pDataBegin - m_pDataEnd));

			m_pDataCur = reinterpret_cast<uint8_t*>(AlignUp(reinterpret_cast<size_t>(m_pDataCur), uAlign));
			uSize = AlignUp(uSize, uAlign);

			// return NULL if we ran out of space in the heap
			if ((m_pDataCur >= m_pDataEnd) || (m_pDataCur + uSize >= m_pDataEnd))
			{
				return NULL;
			}

			pRet = m_pDataCur;
			m_pDataCur += uSize;
		}

		return pRet;
	}

	uint8_t* UploadHeap::BeginSuballocate(size_t uSize, uint64_t uAlign)
	{
		uint8_t* pRes = NULL;

		for (;;) {
			pRes = Suballocate(uSize, uAlign);
			if (pRes != NULL)
			{
				break;
			}

			FlushAndFinish();
		}

		allocating.Inc();

		return pRes;
	}

	void UploadHeap::EndSuballocate()
	{
		allocating.Dec();
	}


	void UploadHeap::AddCopy(VkImage image, VkBufferImageCopy bufferImageCopy)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_copies.push_back({ image, bufferImageCopy });
	}

	void UploadHeap::AddPreBarrier(VkImageMemoryBarrier imageMemoryBarrier)
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_toPreBarrier.push_back(imageMemoryBarrier);
	}


	void UploadHeap::AddPostBarrier(VkImageMemoryBarrier imageMemoryBarrier)
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_toPostBarrier.push_back(imageMemoryBarrier);
	}

	void UploadHeap::Flush()
	{
		VkResult res;

		VkMappedMemoryRange range[1] = {};
		range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range[0].memory = m_deviceMemory;
		range[0].size = m_pDataCur - m_pDataBegin;
		res = vkFlushMappedMemoryRanges(dev->device, 1, range);
		assert(res == VK_SUCCESS);
	}

	//--------------------------------------------------------------------------------------
	//
	// FlushAndFinish
	//
	//--------------------------------------------------------------------------------------
	void UploadHeap::FlushAndFinish(bool bDoBarriers)
	{
		// make sure another thread is not already flushing
		flushing.Wait();

		// begins a critical section, and make sure no allocations happen while a thread is inside it
		flushing.Inc();

		// wait for pending allocations to finish
		allocating.Wait();

		std::unique_lock<std::mutex> lock(m_mutex);
		Flush();
		//Trace("flushing %i", m_copies.size());

		//apply pre barriers in one go
		if (m_toPreBarrier.size() > 0)
		{
			vkCmdPipelineBarrier(GetCommandList(), VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, (uint32_t)m_toPreBarrier.size(), m_toPreBarrier.data());
			m_toPreBarrier.clear();
		}

		for (COPY c : m_copies)
		{
			vkCmdCopyBufferToImage(GetCommandList(), GetResource(), c.m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &c.m_bufferImageCopy);
		}
		m_copies.clear();

		//apply post barriers in one go
		if (m_toPostBarrier.size() > 0)
		{
			vkCmdPipelineBarrier(GetCommandList(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, (uint32_t)m_toPostBarrier.size(), m_toPostBarrier.data());
			m_toPostBarrier.clear();
		}

		// Close 
		VkResult res = vkEndCommandBuffer(m_pCommandBuffer);
		assert(res == VK_SUCCESS);

		// Submit
		const VkCommandBuffer cmd_bufs[] = { m_pCommandBuffer };
		VkSubmitInfo submit_info;
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.pNext = NULL;
		submit_info.waitSemaphoreCount = 0;
		submit_info.pWaitSemaphores = NULL;
		submit_info.pWaitDstStageMask = NULL;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = cmd_bufs;
		submit_info.signalSemaphoreCount = 0;
		submit_info.pSignalSemaphores = NULL;

		res = vkQueueSubmit(dev->graphics_queues[0], 1, &submit_info, m_fence);
		assert(res == VK_SUCCESS);

		// Make sure it's been processed by the GPU

		res = vkWaitForFences(dev->device, 1, &m_fence, VK_TRUE, 2000);
		assert(res == VK_SUCCESS);

		vkResetFences(dev->device, 1, &m_fence);

		// Reset so it can be reused
		VkCommandBufferBeginInfo cmd_buf_info = {};
		cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		res = vkBeginCommandBuffer(m_pCommandBuffer, &cmd_buf_info);
		assert(res == VK_SUCCESS);

		m_pDataCur = m_pDataBegin;

		flushing.Dec();
	}


	namespace cvk
	{
#ifdef _WIN32

		VkFormat TranslateDxgiFormatIntoVulkans(DXGI_FORMAT1 format)
		{
			switch (format)
			{
			case DXGI_FORMAT_B8G8R8A8_UNORM: return VK_FORMAT_B8G8R8A8_UNORM;
			case DXGI_FORMAT_R8G8B8A8_UNORM: return VK_FORMAT_R8G8B8A8_UNORM;
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return VK_FORMAT_R8G8B8A8_SRGB;
			case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return VK_FORMAT_B8G8R8A8_SRGB;
			case DXGI_FORMAT_BC1_UNORM:         return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
			case DXGI_FORMAT_BC1_UNORM_SRGB:    return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
			case DXGI_FORMAT_BC2_UNORM:         return VK_FORMAT_BC2_UNORM_BLOCK;
			case DXGI_FORMAT_BC2_UNORM_SRGB:    return VK_FORMAT_BC2_SRGB_BLOCK;
			case DXGI_FORMAT_BC3_UNORM:         return VK_FORMAT_BC3_UNORM_BLOCK;
			case DXGI_FORMAT_BC3_UNORM_SRGB:    return VK_FORMAT_BC3_SRGB_BLOCK;
			case DXGI_FORMAT_BC4_UNORM:         return VK_FORMAT_BC4_UNORM_BLOCK;
			case DXGI_FORMAT_BC4_SNORM:         return VK_FORMAT_BC4_UNORM_BLOCK;
			case DXGI_FORMAT_BC5_UNORM:         return VK_FORMAT_BC5_UNORM_BLOCK;
			case DXGI_FORMAT_BC5_SNORM:         return VK_FORMAT_BC5_UNORM_BLOCK;
			case DXGI_FORMAT_B5G6R5_UNORM:      return VK_FORMAT_B5G6R5_UNORM_PACK16;
			case DXGI_FORMAT_B5G5R5A1_UNORM:    return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
			case DXGI_FORMAT_BC6H_UF16:         return VK_FORMAT_BC6H_UFLOAT_BLOCK;
			case DXGI_FORMAT_BC6H_SF16:         return VK_FORMAT_BC6H_SFLOAT_BLOCK;
			case DXGI_FORMAT_BC7_UNORM:         return VK_FORMAT_BC7_UNORM_BLOCK;
			case DXGI_FORMAT_BC7_UNORM_SRGB:    return VK_FORMAT_BC7_SRGB_BLOCK;
			case DXGI_FORMAT_R10G10B10A2_UNORM: return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
			case DXGI_FORMAT_R16G16B16A16_FLOAT: return VK_FORMAT_R16G16B16A16_SFLOAT;
			case DXGI_FORMAT_R32G32B32A32_FLOAT: return VK_FORMAT_R32G32B32A32_SFLOAT;
			case DXGI_FORMAT_A8_UNORM:          return VK_FORMAT_R8_UNORM;
			default: assert(false);  return VK_FORMAT_UNDEFINED;
			}
		}
#endif
	}


}
// !hz
