
#include <conio.h>
#include <thread>
//#include "rope.h"
#include "anjn/anjn.h"
#include "tools/data/xlsx.h"
using namespace std::literals::chrono_literals;

#include <tools/audio/audio.h>
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
#include <ntype.h>
//#include "vkclass.h"
//#include "vk_Initializers.h"
#include "vk_cx.h"
#include "vk_cx1.h"
#include <base/promise_cx.h>
#include <base/Singleton.h>
#include <base/thread_pool.h>
using namespace std;
#include <vk_core/view.h>
#include <vk_core/bw_sdl.h>

// 内存泄漏检测
#ifdef _DEBUG
#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif
namespace hz {
	// todo vkimage info
#if 1
	static VkImageLayout GetImageLayout(ImageLayoutBarrier target);
	static void ImagePipelineBarrier(VkCommandBuffer cmdBuffer, VkImage image, ImageLayoutBarrier source, ImageLayoutBarrier dest, const VkImageSubresourceRange& subresourceRange);
	static void SetImageBarrierInfo(ImageLayoutBarrier source, ImageLayoutBarrier dest, VkImageMemoryBarrier& inOutBarrier, VkPipelineStageFlags& inOutSourceStage, VkPipelineStageFlags& inOutDestStage);
	static VkPipelineStageFlags GetImageBarrierFlags(ImageLayoutBarrier target, VkAccessFlags& accessFlags, VkImageLayout& layout);

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
		double ret = get_ms();
		return ret * 0.001;
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
		void flushCommandBuffer(VkDevice dev, VkCommandBuffer commandBuffer, VkCommandPool commandPool, VkQueue queue, bool free);
		VkPipelineShaderStageCreateInfo load_shader(dvk_device* dev, const char* data, size_t size, size_t stage, const char* pName = "main");

		uint32_t getMemoryType(dvk_device* dev, uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound);
	}
#endif


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

	void gpu_info::init(VkInstance ip)
	{
		_instance = ip;
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
		MessageBoxA(0, kstr.c_str(), "vk", MB_OK);
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
		//MessageBoxA(0, kstr.c_str(), "vk", MB_OK);
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
		int mt[5] = {4, 1, 0, 2, 3};
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
		//MessageBoxA(0, kstr.c_str(), "vk", MB_OK);
		std::sort(gpu_infos.begin(), gpu_infos.end(), [](const gpu_dev_inifo_t& d1, const gpu_dev_inifo_t& d2) { return d1.m < d2.m; });

		auto uboa0 = gpu_infos[0].pdp.limits.minUniformBufferOffsetAlignment;
		//auto uboa1 = gpu_infos[1].limits.minUniformBufferOffsetAlignment;
		//LOGI("vulkan GPU count:%d\n", (int)gpusname.size());
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


	void dvk_cmd::dispatch(int groupX, int groupY, int groupZ)
	{
		vkCmdDispatch(cmdb, groupX, groupY, groupZ);
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

		VkFilter filter = is_linear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
		VkImageBlit blit = {};
		blit.srcOffsets[0] = {0, 0, 0};
		blit.srcOffsets[1] = {(int)src->width, (int)src->height, 1};
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = src->mipLevels;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = {0, 0, 0};
		blit.dstOffsets[1] = {(int)dst->width, (int)dst->height, 1};
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

	void dvk_cmd::set_state(uint32_t cull_mod, uint32_t front_face, uint32_t primitive_topology, int depth_test_enable, int depth_write_enable)
	{
#if 0
		do
		{
			if (cull_mod < VK_CULL_MODE_FLAG_BITS_MAX_ENUM)
				vkCmdSetCullModeEXT(cmdb, cull_mod);
			//VK_FRONT_FACE_COUNTER_CLOCKWISE = 0,
			//VK_FRONT_FACE_CLOCKWISE = 1
			if (front_face < VK_FRONT_FACE_MAX_ENUM)
				vkCmdSetFrontFaceEXT(cmdb, (VkFrontFace)front_face);
			if (primitive_topology < VK_PRIMITIVE_TOPOLOGY_MAX_ENUM)
				vkCmdSetPrimitiveTopologyEXT(cmdb, (VkPrimitiveTopology)primitive_topology);
			//vkCmdSetViewportWithCountEXT(cmdb); vkCmdSetScissorWithCountEXT(cmdb);
			//vkCmdBindVertexBuffers2EXT();
			vkCmdSetDepthTestEnableEXT(cmdb, depth_test_enable);
			vkCmdSetDepthWriteEnableEXT(cmdb, depth_write_enable);

		} while (0);
#endif
	}

	void dvk_cmd::bind_pipeline(shader_info* pipeline, bool depthWriteEnable)
	{
		_pipeline = pipeline;
		vkCmdBindPipeline(cmdb, (VkPipelineBindPoint)bindPoint, _pipeline->get_vkpipe(depthWriteEnable));
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

	void dvk_cmd::bind_ibo(VkBuffer buffer, VkDeviceSize offset, vkm::IBO_type index_type)
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
		VkDeviceSize pOffsets[] = {offset};
		vkCmdBindVertexBuffers(cmdb, firstBinding, 1, &pBuffer, pOffsets);
	}

	void dvk_cmd::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
	{
		vkCmdDraw(cmdb, vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void dvk_cmd::draw_indexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
	{
		vkCmdDrawIndexed(cmdb, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

	void dvk_cmd::draw(uint32_t vertexCount, uint32_t firstVertex)
	{
		vkCmdDraw(cmdb, vertexCount, 1, firstVertex, 0);
	}

	void dvk_cmd::draw_indexed(uint32_t indexCount, uint32_t firstIndex, int32_t vertexOffset)
	{
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

	void dvk_cmd::ExecuteCommands(const VkCommandBuffer* pCommandBuffers, uint32_t count)
	{
		vkCmdExecuteCommands(cmdb, count, pCommandBuffers);
	}

#endif
#if 1


	dvk_texture::dvk_texture()
	{
	}

	dvk_texture::dvk_texture(dvk_device* dev) :_dev(dev)
	{
		descriptor = new VkDescriptorImageInfo();
	}

	dvk_texture::~dvk_texture()
	{
		free_image();
		if (descriptor)
		{
			delete descriptor; descriptor = 0;
		}
	}

	void dvk_texture::free_image()
	{
		if (_dev)
		{
			if (view)
				vkDestroyImageView(_dev->_dev, view, nullptr);
			if (_image)
				vkDestroyImage(_dev->_dev, _image, nullptr);
			if (sampler)
				vkDestroySampler(_dev->_dev, sampler, nullptr);
			if (deviceMemory)
				vkFreeMemory(_dev->_dev, deviceMemory, nullptr);
		}
		view = nullptr;
		_image = nullptr;
		deviceMemory = nullptr;
		sampler = nullptr;
	}





	//dvk_texture* dvk_texture::new2d_priv(const std::string& filename, dvk_device* dev, uint64_t cq, ImageLayoutBarrier imageLayout, VkImageUsageFlags imageUsageFlags)
	dvk_texture* dvk_texture::new2d_priv(const std::string& filename, dvk_device* dev, ImageLayoutBarrier imageLayout, uint32_t imageUsageFlags)
	{
		VkImageLayout il = GetImageLayout(imageLayout);
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
		auto tex = new dvk_texture(dev);
		if (tex)
		{
			tex->set_data(pxdata, (uint64_t)width * height * 4, VK_FORMAT_R8G8B8A8_UNORM, width, height, dev, imageUsageFlags, il);
		}

		stbi_image_free(pxdata);
		return tex;
	}
	// todo 需要重新写
	void dvk_texture::set_data(void* buffer, VkDeviceSize bufferSize, uint32_t format
							   , uint32_t w, uint32_t h, dvk_device* dev, uint32_t imageUsageFlags, uint32_t imageLayout)
		//void dvk_texture::set_data(void* buffer, VkDeviceSize bufferSize, uint32_t format, uint32_t w, uint32_t h, dvk_device* dev, VkQueue copyQueue, VkImageUsageFlags imageUsageFlags, uint32_t  imageLayout)//VkImageLayout
	{
		//assert(buffer);
		//try
		{
			_dev = dev;
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
			auto qctx = dev->get_graphics_queue(0);
			auto_destroy_cx dcx;
			auto cp = qctx->new_cmd_pool();

			auto copyQueue = qctx->get_vkptr();
			VkMemoryAllocateInfo memAllocInfo = {};
			memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			VkMemoryRequirements memReqs;
			// Use a separate command buffer for texture loading
			VkCommandBuffer copyCmd = vkc::createCommandBuffer1(dev->_dev, cp->command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

			//LOGW("createTexture:w:%d,h:%d,size:%lld,memReqs.size:%d\n", width, height, p->size, width * height * 4);

			// Get device properites for the requested texture format
			VkFormatProperties formatProperties;
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
			imageCreateInfo.extent = {width, height, 1};
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
					vkDestroyImage(dev->_dev, _image, 0);
					_image = 0;
				}
				(vkCreateImage(dev->_dev, &imageCreateInfo, 0, &_image));
			}
			//	LOGW("createTexture:w:%d,h:%d,size:%d,memReqs.size:%d\n", width, height, bufferSize, width*height*4);
			vkGetImageMemoryRequirements(dev->_dev, _image, &memReqs);

			memAllocInfo.allocationSize = memReqs.size;
			VkBool32 memTypeFound = 0;
			memAllocInfo.memoryTypeIndex = vkc::getMemoryType(dev, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memTypeFound);

			if (isnew)
			{
				if (deviceMemory)
				{
					vkFreeMemory(dev->_dev, deviceMemory, 0);
					deviceMemory = 0;
				}
				(vkAllocateMemory(dev->_dev, &memAllocInfo, nullptr, &deviceMemory));
				(vkBindImageMemory(dev->_dev, _image, deviceMemory, 0));
			}
			VkImageSubresourceRange subresourceRange = {};
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = mipLevels;
			subresourceRange.layerCount = 1;

			// Image barrier for optimal image (target)
			// Optimal image will be used as destination for the copy
			hz::vkc::setImageLayout(
				copyCmd,
				_image,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				subresourceRange
				, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
			);

			// Copy mip levels from staging buffer
			vkCmdCopyBufferToImage(
				copyCmd,
				staging.buffer,
				_image,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&bufferCopyRegion
			);

			// Change texture image layout to shader read after all mip levels have been copied
			hz::vkc::setImageLayout(
				copyCmd,
				_image,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				(VkImageLayout)imageLayout,
				subresourceRange
				, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

			vkc::flushCommandBuffer(_dev->_dev, copyCmd, cp->command_pool, copyQueue, true);
			qctx->free_cmd_pool(cp);
			if (psampler && psampler != _dev->_sampler)
			{
				sampler = (VkSampler)psampler;;
			}
			else
			{
				psampler = _dev->_sampler;
			}
			// Create image view
			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = (VkFormat)format;
			viewInfo.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.image = _image;
			if (isnew)
			{
				if (view)
				{
					vkDestroyImageView(dev->_dev, view, 0);
				}
				(vkCreateImageView(dev->_dev, &viewInfo, nullptr, &view));
			}
			// Update descriptor image info member that can be used for setting up descriptor sets
			descriptor->imageView = view;
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
		auto imageLayout = GetImageLayout(image_layout);
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
		image.tiling = VK_IMAGE_TILING_OPTIMAL;
		// We will sample directly from the color attachment
		image.usage = usage ? usage : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colorImageView.format = (VkFormat)format;
		colorImageView.flags = 0;
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

		auto qctx = _dev->get_graphics_queue(0);
		auto cp = qctx->new_cmd_pool();
		auto copyQueue = qctx->get_vkptr();
		VkCommandBuffer copyCmd = vkc::createCommandBuffer1(_dev->_dev, cp->command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);


		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.levelCount = mipLevels;
		subresourceRange.layerCount = 1;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		ImagePipelineBarrier(copyCmd, _image, ImageLayoutBarrier::UNDEFINED, image_layout, subresourceRange);
		vkc::flushCommandBuffer(_dev->_dev, copyCmd, cp->command_pool, copyQueue, true);
		qctx->free_cmd_pool(cp);
		descriptor->imageView = view;
		descriptor->sampler = _dev->_sampler;
		descriptor->imageLayout = imageLayout;
	}

	void dvk_texture::new_storage(int width, int height, uint32_t format, bool is_compute)
	{
		mkrt_image(width, height, format ? format : VK_FORMAT_R8G8B8A8_UNORM, false, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT
				   , is_compute ? ImageLayoutBarrier::ComputeGeneralRW : ImageLayoutBarrier::PixelGeneralRW);
	}

	Image* dvk_texture::save2Image(Image* outimage)
	{
		if (outimage)
		{
			outimage->resize(width, height);
			outimage->clearColor(0);
			get_buffer((char*)outimage->data());
			//File::save_binary_file("ztem.bin", (char*)outimage->data(), outimage->datasize());
			if (VK_FORMAT_B8G8R8A8_UNORM == _format)
			{
				outimage->bgra2rgba();
				//File::save_binary_file("ztem1.bin", (char*)outimage->data(), outimage->datasize());
			}
		}
		return outimage;
	}

	void dvk_texture::get_buffer(char* outbuf)
	{
		VkMemoryAllocateInfo memAllocInfo = {};// hz::initializers::memoryAllocateInfo();
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		//VkMemoryRequirements memReqs;

		dvk_staging_buffer staging;
		// Use a separate command buffer for texture loading
		auto qctx = _dev->get_graphics_queue(0);
		auto cp = qctx->new_cmd_pool();
		auto copyQueue = qctx->get_vkptr();
		VkCommandBuffer copyCmd = vkc::createCommandBuffer1(_dev->_dev, cp->command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

		VkImageAspectFlags    aspectMask = _format == VK_FORMAT_D32_SFLOAT_S8_UINT ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;;
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

		staging.initBuffer(_dev, width * height * 4);
		// Image barrier for optimal image (target)
		// Optimal image will be used as destination for the copy
		hz::vkc::setImageLayout(
			copyCmd,
			_image,
			aspectMask,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			subresourceRange
			, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

		// Copy mip levels from staging buffer
		vkCmdCopyImageToBuffer(
			copyCmd,
			_image,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			staging.buffer,
			1,
			&bufferCopyRegion
		);

		// Change texture image layout to shader read after all mip levels have been copied
		hz::vkc::setImageLayout(
			copyCmd,
			_image,
			aspectMask,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			descriptor->imageLayout,
			subresourceRange
			, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

		vkc::flushCommandBuffer(_dev->_dev, copyCmd, cp->command_pool, copyQueue, true);
		//vkQueueWaitIdle(copyQueue);
		staging.getBuffer(outbuf, -1);
		qctx->free_cmd_pool(cp);
		//device->waitIdle();
	}

	void dvk_texture::set_data(Image* img)
	{
		VkImageLayout il = GetImageLayout(_image_layout);
		if (!img || img->empty())
		{
			return;
		}
		int comp = 0;
		int width = img->width;
		int height = img->height;
		uint8_t* pxdata = (uint8_t*)img->data();
		auto imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		set_data(pxdata, (uint64_t)width * height * 4, VK_FORMAT_R8G8B8A8_UNORM, width, height, _dev, imageUsageFlags, il);
	}

	VkImageLayout GetImageLayout(ImageLayoutBarrier target)
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

	dvk_texture* dvk_texture::new2d(const std::string& filename, dvk_device* dev, ImageLayoutBarrier imageLayout)
	{
		auto p = dvk_texture::new2d_priv(filename, (dvk_device*)dev, imageLayout, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
		return p;
	}
	dvk_texture* dvk_texture::new_image2d(Image* img, dvk_device* dev, ImageLayoutBarrier imageLayout)
	{
		VkImageLayout il = GetImageLayout(imageLayout);
		if (!img || img->empty())
		{
			return nullptr;
		}
		int comp = 0;
		int width = img->width;
		int height = img->height;
		uint8_t* pxdata = (uint8_t*)img->data();
		auto imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		auto tex = new dvk_texture(dev);
		if (tex)
		{
			tex->set_data(pxdata, (uint64_t)width * height * 4, VK_FORMAT_R8G8B8A8_UNORM, width, height, dev, imageUsageFlags, il);
		}
		return tex;
	}
	dvk_texture* dvk_texture::new_storage2d(const std::string& filename, dvk_device* dev, bool is_compute)
	{
		return dvk_texture::new2d_priv(filename, (dvk_device*)dev, is_compute ? ImageLayoutBarrier::ComputeGeneralRW : ImageLayoutBarrier::PixelGeneralRW
									   , VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
	}

	dvk_texture* dvk_texture::new_storage2d(dvk_device* dev, int width, int height, uint32_t format, bool is_compute)
	{
		auto _dev = (dvk_device*)dev;
		auto p = new dvk_texture(_dev);
		if (p)
		{
			p->new_storage(width, height, format, is_compute);
		}
		return p;
	}
	dvk_texture* dvk_texture::new_render_target_color(dvk_device* dev, int width, int height, uint32_t format)
	{
		auto _dev = (dvk_device*)dev;
		auto p = new dvk_texture(_dev);
		if (p)
		{
			p->mkrt_image(width, height, format, false, 0, ImageLayoutBarrier::COLOR_ATTACHMENT);
		}
		return p;
	}

	dvk_texture* dvk_texture::new_render_target_depth(dvk_device* dev, int width, int height, uint32_t format)
	{
		auto _dev = (dvk_device*)dev;
		auto p = new dvk_texture(_dev);
		if (p)
		{
			p->mkrt_image(width, height, format, false, 0, ImageLayoutBarrier::DEPTH_STENCIL_ATTACHMENT);
		}
		return p;
	}

	VkDescriptorImageInfo* dvk_texture::get_descriptor_image_info()
	{
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
		assert(sub && sub->sv);
		uint32_t ret = 0;
		auto submitInfo = (std::vector<VkSubmitInfo>*)sub->sv;
		if (submitInfo->size())
		{
			if (fence)
				fence->reset();
			VkFence vkf = fence ? fence->get() : nullptr;
			auto rt = vkQueueSubmit(_queue, submitInfo->size(), submitInfo->data(), vkf);
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
		if (p->submit_fence)
		{
			p->submit_fence->wait_for();
		}
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
		return vkQueuePresentKHR(_queue, &presentInfo);
	}
#endif
	submit_infos::submit_infos()
	{
		sv = (submits*) new std::vector<VkSubmitInfo>();
	}
	submit_infos::~submit_infos()
	{
		auto psv = (std::vector<VkSubmitInfo>*)sv;
		if (psv)
		{
			delete psv; psv = 0;
		}
	}

	size_t submit_infos::insert_sub(submit_info* psi)
	{
		auto psv = (std::vector<VkSubmitInfo>*)sv;
		assert(psv);
		size_t ret = psv->size();
		auto r = psim[psi];
		if (!r)
		{
			auto it = psi;
			psim[psi] = ret;
			VkSubmitInfo dt = {};
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
			psv->push_back(dt);
		}
		return ret;
	}
	void submit_infos::erase(submit_info* psi)
	{
		psim.erase(psi);
	}
	void submit_infos::update_sub(submit_info* info)
	{
		size_t idx = psim[info];
		auto psv = (std::vector<VkSubmitInfo>*)sv;
		assert(psv);
		size_t ret = psv->size();
		if (idx < ret)
		{
			auto it = info;
			VkSubmitInfo& dt = psv->at(idx);
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
		}
	}
	void submit_infos::clear()
	{
		auto psv = (std::vector<VkSubmitInfo>*)sv;
		assert(psv);
		psv->clear(); psim.clear();
	}
	size_t submit_infos::size()
	{
		auto psv = (std::vector<VkSubmitInfo>*)sv;
		return psv->size();
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
		auto ct = g_CurrTime;
		UpdateFPS(ct, delta); uptime(ct, delta);
		//fpsup(delta * 1000);
		g_LastTime = nowT;
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

	queue_ctx::queue_ctx(const char* title) //:is_clear(true)
	{
		game_time* pgt = &gt;
		std::thread renderthr([=](submit_infos* psub, present_info* present, bool* isexits)
		{
			uint32_t image_idx = 0;
			set_thr_name_cur("queue_ctx");
			pgt->init();
			while (*isexits)
			{
				if (!isbegin) {
					sleep(8);
					continue;
				}
				pgt->update();
				if (frame_begin())
					frame_end();
				else
				{
					sleep(8);
					//printf("not form\n");
				}
			}
			printf("exit render thread\n");
		}, &sub, &present, &isrun);
		_rthr.swap(renderthr);
	}

	queue_ctx::~queue_ctx()
	{
		stop();
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
		present.submit_fence = new dvk_fence(_dev->_dev);
	}

	void queue_ctx::push(sdl_form* form1)
	{
		if (form1)
		{
			temqueue.push(form1);
		}
	}

	void queue_ctx::set_waitms(int ms)
	{
		_waitms = ms;
	}

	void queue_ctx::FixedUpdate(cpg_time* t)
	{

	}

	void queue_ctx::LateUpdate(cpg_time* t)
	{

	}

	void queue_ctx::game_logic(cpg_time* t)
	{
		if (update_cb)
		{
			update_cb(t);
		}

		LateUpdate(t);
	}

	void queue_ctx::game_update()
	{
		auto t = gt.get_gt();
		FixedUpdate(t);
		game_logic(t);
	}

	bool queue_ctx::frame_begin()
	{
		bool ret = false;
		auto form1 = temqueue.get_wait_one(1);
		if (form1)
		{
			_vforms.push_back(form1);
			auto fbo1 = form1->get_fbo();
			fbo1->set_queue(_qctx, &gt);
			if (!is_clear)
			{
				auto swap1 = fbo1->get_swapchain();
				present.push(swap1);
				sub.insert_sub(fbo1->get_cur_submit());

			}
			fbo1->inc_update();
		}
		if (_vforms.size())
			game_update();
		for (size_t i = 0; i < _vforms.size(); i++)
		{
			uint32_t image_idx = 0;
			auto& it = _vforms[i];
			auto fbo1 = it->get_fbo();
			auto swap1 = fbo1->get_swapchain();
			if (it->is_close())
			{
				it->set_stop_draw();
				_vforms.erase(_vforms.begin() + i);
				present.erase(swap1);
				printf("stop %p\n", it);
				i--;
				continue;
			}
			if (it->is_draw())
			{
				sub.insert_sub(fbo1->get_cur_submit());
				if (is_clear)
				{
					subform.push_back(it);
					present.push(swap1);
				}
				if (fbo1->is_gs())
				{
					printf("resize\n");
					auto t = gt.get_gt();
					//	if ((t->unscaledTime - fbo1->_gstime > fbo1->_fixed_gs_time) || fbo1->is_gs())
					{
						fbo1->_gstime = t->unscaledTime;
						auto fs = fbo1->get_size();
						int w = 0, h = 0;
						auto fws = it->get_size();
						w = fws.x;
						h = fws.y;
						//if (fsi.x != w || fsi.y != h)
						{
							//printf("fbo\t%p\tw: %d\th%d\n", it, (int)fs.x, (int)fs.y);
							//printf("%p\tw: %d\th%d\n", it, w, h);
							fbo1->resize(w, h);
							fbo1->close_gs();
						}
					}
					if (fbo1->dev_resize(is_outofdate))
					{
						//fbo1->inc_update();
					}
					fbo1->_upinc++;
				}
				if (fbo1->is_update()) {
					fbo1->build_cmd_cb();
					fbo1->dec_update();
				}
				fbo1->acquire(&image_idx);
				auto sp = fbo1->get_cur_submit();
				sub.update_sub(sp);
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
			if (sub.size())
				_qctx->submit(&sub, present.submit_fence, true);
			if (present.size())
			{
				VkResult hr = (VkResult)_qctx->present(&present);
				if (hr != 0)
				{
					if (hr == VK_ERROR_OUT_OF_DATE_KHR)is_outofdate = true;
					hr = hr;
				}
				else {
					is_outofdate = false;
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
		//else
		{
			sleep(_waitms);
		}
	}
	// !queue_ctx
}
// !hz
