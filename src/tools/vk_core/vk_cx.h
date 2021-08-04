#pragma once
#include <thread>
#include <functional>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <ntype.h>
#include <base/promise_cx.h>
#include "render_info.h"

#if !defined(VK_DEFINE_NON_DISPATCHABLE_HANDLE)
#if defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) && !defined(__ILP32__) ) || defined(_M_X64) || defined(__ia64) || defined (_M_IA64) || defined(__aarch64__) || defined(__powerpc64__)
#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef struct object##_T *object;
#else
#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef uint64_t object;
#endif
#define VK_DEFINE_HANDLE(object) typedef struct object##_T* object;
typedef uint32_t VkFlags;
typedef uint32_t VkBool32;
typedef uint64_t VkDeviceSize;
typedef uint32_t VkSampleMask;
VK_DEFINE_HANDLE(VkInstance)
VK_DEFINE_HANDLE(VkPhysicalDevice)
VK_DEFINE_HANDLE(VkDevice)
VK_DEFINE_HANDLE(VkQueue)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkSemaphore)
VK_DEFINE_HANDLE(VkCommandBuffer)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkFence)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkDeviceMemory)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkBuffer)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkImage)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkEvent)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkQueryPool)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkBufferView)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkImageView)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkShaderModule)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkPipelineCache)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkPipelineLayout)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkRenderPass)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkPipeline)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkDescriptorSetLayout)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkSampler)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkDescriptorPool)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkDescriptorSet)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkFramebuffer)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkCommandPool)
#ifndef VK_KHR_surface
#define VK_KHR_surface 1
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkSurfaceKHR)
#define VK_KHR_swapchain 1
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkSwapchainKHR)
#endif // !VK_KHR_surface



#endif

typedef struct VkDescriptorImageInfo VkDescriptorImageInfo;
typedef struct VkPhysicalDeviceLimits VkPhysicalDeviceLimits;

#ifndef hz_FLOBYTE
#define hz_FLOBYTE(c)           ((unsigned char)(((size_t)(c)) & 0xff))
#endif// !LOBYTE
#ifndef hz_R
#define hz_R(rgb)      (hz_FLOBYTE(rgb))
#define hz_G(rgb)      (hz_FLOBYTE(((unsigned short)(rgb)) >> 8))
#define hz_B(rgb)      (hz_FLOBYTE((rgb)>>16))
#define hz_A(rgb)      (hz_FLOBYTE((rgb)>>24))
#endif

namespace hz
{
	enum class ImageLayoutBarrier
	{
		UNDEFINED,
		TRANSFER_DST,
		COLOR_ATTACHMENT,
		DEPTH_STENCIL_ATTACHMENT,
		TRANSFER_SRC,
		PRESENT_SRC,
		SHADER_READ,
		DEPTH_STENCIL_READ,
		ComputeGeneralRW,
		PixelGeneralRW,
	};
	enum class dvk_queue_bit :uint32_t {
		GRAPHICS_BIT = 0x00000001,
		COMPUTE_BIT = 0x00000002,
		TRANSFER_BIT = 0x00000004,
		SPARSE_BINDING_BIT = 0x00000008,
		PROTECTED_BIT = 0x00000010,
	};
	class set_pool_info;
	class pipeline_ptr_info;
	class dvk_buffer;
	class dvk_queue;
	class dev_info;
	class game_time;
	struct dynamic_state_ext_cb;
	class Image;
	// todo dvk_device
	class dvk_device
	{
	public:
		VkDevice _dev = 0;
		VkPhysicalDevice physicalDevice = 0;
		VkSampler _sampler = 0;
		dev_info* _info = 0;
		std::set<std::string> supportedExtensions;
		struct
		{
			uint32_t graphics;
			uint32_t compute;
			uint32_t transfer;
			uint32_t sparse_binding;
		} queueFamilyIndices = {};
		struct
		{
			//支持图形操作，例如绘制点，线和三角形。
			VkQueue graphics = 0;
			//支持计算操作，例如调度计算着色器。
			VkQueue compute = 0;
			//支持传输操作，例如复制缓冲区和映像内容。
			VkQueue transfer = 0;
			//支持用于更新稀疏资源的内存绑定操作。
			VkQueue sparse_binding = 0;
		}_queue1;
		// 是否启用debug
		bool enableDebugMarkers = false;
		VkPhysicalDeviceLimits* _limits = {};
		// 动态修改状态函数
		dynamic_state_ext_cb* ds_cb = 0;
	private:
		std::vector<VkQueue> graphics_queues;
		std::vector<VkQueue> compute_queues;
		std::vector<dvk_queue*> graphics_queue_cxs;
		std::vector<dvk_queue*> compute_queue_cxs;
	public:
		dvk_device();

		~dvk_device();
		void init(VkInstance inst, bool iscompute);
	public:
		// new
		void free_shader(VkShaderModule s);
		bool new_pipeline(pipeline_ptr_info* info);
		/*
			queueFamilyIndices
		*/
		VkCommandPool new_cmd_pool(uint32_t qfidx);
		//支持图形操作，例如绘制点，线和三角形。
		VkCommandPool new_graphics_cmd_pool();
		//支持计算操作，例如调度计算着色器。
		VkCommandPool new_compute_cmd_pool();
		//支持传输操作，例如复制缓冲区和映像内容。
		VkCommandPool new_transfer_cmd_pool();
		//支持用于更新稀疏资源的内存绑定操作。
		//VkCommandPool new_sparse_binding_cmd_pool();

		//VkResult create_buffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, size_t size, VkBuffer* buffer, VkDeviceMemory* memory, void* data = nullptr);

		bool new_set_pool(set_pool_info* sp);
		VkDescriptorPool new_desc_pool(std::map<uint32_t, uint32_t>& mps, uint32_t maxSets);
		bool alloc_set(VkDescriptorPool pool, VkDescriptorSetLayout* pSetLayouts, uint32_t descriptorSetCount, std::vector<VkDescriptorSet>& dset);
		// VkCommandBufferLevel
		int new_cmd(VkCommandPool pool, uint32_t level, VkCommandBuffer* outptr, uint32_t count);
		void free_cmd(VkCommandPool pool, VkCommandBuffer* pcbs, uint32_t count);
		// 创建RenderPass
		VkRenderPass new_render_pass(uint32_t color_format1, uint32_t depth_format1);
		std::string get_name();
	public:
		uint32_t  get_familyIndex(int idx);
		//uint32_t getMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound = nullptr);
		bool extensionSupported(const std::string& extension);

		dvk_queue* get_graphics_queue(unsigned int idx = 0);
		dvk_queue* get_compute_queue(unsigned int idx = 0);
		void free_gqp(unsigned int idx);
		void free_cqp(unsigned int idx);
		uint32_t get_gqueue_size();
		uint32_t get_cqueue_size();

		void wait_idle();
	public:
		VkShaderModule CreateShaderModule(const char* filename, size_t len = 0);
		void mkDeviceQueue();
	};

	// todo dvk_buffer
	class dvk_buffer
	{
	public:
		VkBuffer buffer = 0;
		VkDevice device = 0;
		VkDeviceMemory memory = 0;
		dvk_device* _dev = 0;
		//VkDescriptorBufferInfo descriptor;
		std::vector<char> descriptors;
		//VkDescriptorType	dtype;// =
		/*
			VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
			VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
			texel storage dynamic
		*/
		uint32_t dtype = 6;
		VkDeviceSize _size = 0;
		VkDeviceSize _capacity = 0;
		uint32_t	 _count = 0, stride = 0;
		uint32_t	 type_ = 0;

		VkDeviceSize alignment = 0;
		void* mapped = nullptr;

		/** @brief Usage flags to be filled by external source at buffer creation (to query at some later point) */
		//VkBufferUsageFlags usageFlags = 0;
		uint32_t usageFlags = 0;
		/** @brief Memory propertys flags to be filled by external source at buffer creation (to query at some later point) */
		//VkMemoryPropertyFlags memoryPropertyFlags = 0;
		uint32_t memoryPropertyFlags = 0;

		uint32_t e_size = 0;
		uint32_t fpos = -1;
		uint32_t fsize = 0;
		//std::vector<char>				data;

	public:
		dvk_buffer(dvk_device* dev, uint32_t usage, uint32_t mempro, uint32_t size, void* data);
		virtual ~dvk_buffer();

		// 可以创建所有buffer
		static dvk_buffer* create(dvk_device* dev, uint32_t usage, uint32_t mempro, uint32_t size, void* data = nullptr);
		// TEXEL_BUFFER
		static dvk_buffer* new_texel(dvk_device* dev, bool storage, uint32_t size, void* data);
		static dvk_buffer* new_staging(dvk_device* dev, uint32_t size, void* data);
		// device_local=true则CPU不能访问	compute_rw=compute shader是否可读写
		static dvk_buffer* new_vbo(dvk_device* dev, bool device_local, bool compute_rw, uint32_t size, void* data = nullptr);
		static dvk_buffer* new_indirect(dvk_device* dev, bool device_local, uint32_t size, void* data = nullptr);
		// 索引数量count，type 0=16，1=32
		static dvk_buffer* new_ibo(dvk_device* dev, int type, bool device_local, uint32_t count, void* data = nullptr);
		static dvk_buffer* new_ubo(dvk_device* dev, bool storage, uint32_t size);
		// todo*不能用
		static void copy_buffer(dvk_buffer* dst, dvk_buffer* src, uint64_t dst_offset = 0, uint64_t src_offset = 0, int64_t size = -1);
	public:
		//operator VkBuffer () { return buffer; }

		void setDesType(uint32_t dt);

		void* get_dbi();
		uint32_t get_dbi_count();
		// 重置大小
		void resize(size_t size);
	public:
		bool make_data(void* data, size_t size, size_t offset = 0, bool isun = true);
		/**
		* Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
		*
		* @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete buffer range.
		* @param offset (Optional) Byte offset from beginning
		*
		* @return VkResult of the buffer mapping call
		*/
		void* map(VkDeviceSize dsize, VkDeviceSize offset = 0);
		void* get_map(VkDeviceSize offset = 0);
		void unmap();
		uint32_t flush(VkDeviceSize dsize, VkDeviceSize offset = 0);
		uint32_t flush();

		/**
		* Attach the allocated memory block to the buffer
		*
		* @param offset (Optional) Byte offset (from the beginning) for the memory region to bind
		*
		* @return VkResult of the bindBufferMemory call
		*/
		//uint32_t bind(VkDeviceSize offset = 0);
		/**
		* Setup the default descriptor for this buffer
		*
		* @param size (Optional) Size of the memory range of the descriptor
		* @param offset (Optional) Byte offset from beginning
		*
		*/
		//void* new_descriptor(VkDeviceSize s, VkDeviceSize offset = 0);

		/**
		* Copies the specified data to the mapped buffer
		*
		* @param data Pointer to the data to copy
		* @param size Size of the data to copy in machine units
		*
		*/
		size_t set_data(void* data, size_t size, size_t offset, bool is_flush);

		/**
		* Flush a memory range of the buffer to make it visible to the device
		*
		* @note Only required for non-coherent memory
		*
		* @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the complete buffer range.
		* @param offset (Optional) Byte offset from beginning
		*
		* @return VkResult of the flush call
		*/

		/**
		* Invalidate a memory range of the buffer to make it visible to the host
		*
		* @note Only required for non-coherent memory
		*
		* @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate the complete buffer range.
		* @param offset (Optional) Byte offset from beginning
		*
		* @return VkResult of the invalidate call
		*/
		uint32_t invalidate(VkDeviceSize size_, VkDeviceSize offset = 0);

		/**
		* Release all Vulkan resources held by this buffer
		*/
		void destroybuf();

	};
	// todo staging_buffer
	class dvk_staging_buffer
	{
	public:
		VkBuffer buffer = 0;
		VkDeviceMemory mem = 0;
		VkDevice _dev = 0;

		VkDeviceSize bufferSize = 0, memSize = 0;

		void* mapped = nullptr;
		bool isd = true;
	public:
		dvk_staging_buffer();

		virtual	~dvk_staging_buffer();
		void freeBuffer();
		void initBuffer(dvk_device* dev, VkDeviceSize size);
		void copyToBuffer(void* data, size_t bsize);
		void copyGetBuffer(std::vector<char>& outbuf);
		size_t getBufSize();
		void getBuffer(char* outbuf, size_t len);
	};

	// todo dvk_texture
	class dvk_texture;

	struct pipe_info_s
	{
		//std::vector<VkPipelineShaderStageCreateInfo>* v;
		void* v = nullptr;
		int n = 0;
		dvk_device* dev = nullptr;
	};

	union desc_type {
		uint64_t u;
		struct {
			uint32_t descriptorType;
			uint8_t descriptorCount;
			uint16_t stageFlags;
		};
	};

	enum class stage_bit {
		vertex = 1,
		fragment = 2,
		compute = 4,
	};


	class vertex_info
	{
	public:
		std::string name;
		int vecsize = 0;
		int stride_size = 0;
		int location = 0;
		int binding = 0;
		// 类型
		uint32_t format = 0;
	public:
		vertex_info()
		{
		}

		~vertex_info()
		{
		}

	private:

	};

	struct inbind_desc {
		int			location;
		uint16_t	binding;
		uint16_t	stride;
		uint8_t		inputRate;		// VERTEX = 0,INSTANCE = 1
	};
	struct BindInfo
	{
		int16_t set;
		int16_t binding;
		desc_type dtype;
	};
	// todo spv_res_info
	class spv_res_info
	{
	public:
		struct buffer_info
		{
			uint32_t set = 0;
			uint32_t binding = 0;
			uint32_t descriptorType = 6;// VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uint32_t stageFlags = 0;
			uint32_t bufferSize = 0;
			//VkDescriptorType	descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			//VkShaderStageFlags	stageFlags = 0;
		};

		struct image_info
		{
			uint32_t set = 0;
			uint32_t binding = 0;
			uint32_t descriptorType = 1;// VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			uint32_t stageFlags = 0;
			//VkDescriptorType	descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			//VkShaderStageFlags	stageFlags = 0;
		};
	public:
		// set,binding,type
		std::map<int, std::map<int, desc_type>> bindings;
		std::map<int, vertex_info> input_info;
		// 如果没有INSTANCE可以
		std::vector<inbind_desc> input_binding;
		// 使用RGBA8的颜色
		bool color_unorm = true;


		// 通过名称查set\binding
		std::map<std::string, BindInfo> paramsMap;

		std::unordered_map<std::string, buffer_info>	bufferParams;
		std::unordered_map<std::string, image_info>		imageParams;
	public:
		spv_res_info()
		{
		}

		~spv_res_info()
		{
		}
	public:
		desc_type get_dt(int set, int binding);
		size_t get_ubosize(const std::string n);
		std::string get_uboname0();
		std::vector<std::string> get_uboname();
		std::vector<std::string> get_tex_name();
	public:
		// 设置多个VBO绑定
		void add_input_binding(int location, bool instance, uint16_t stride)
		{
			inbind_desc d = {};
			d.location = location;
			d.binding = input_binding.size();
			d.stride = stride;
			d.inputRate = instance ? 1 : 0;
			input_binding.push_back(d);
		}
		// 合并set绑定
		size_t union_binding(spv_res_info* sp)
		{
			size_t c = 0;
			for (auto& [s, v] : sp->bindings)
			{
				auto& dst = bindings[s];
				for (auto& [b, t] : v)
				{
					t.stageFlags |= dst[b].descriptorType;
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
		void make_input()
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
	private:

	};

	struct pipe_create_info {
		const char* data;
		size_t size;
		size_t stage;
		const char* pName = "main";
		std::string fn;
		uint64_t _module = 0;
		spv_res_info* spv_info = nullptr;
	};


	//layout(set = 1, binding = 0) uniform sampler2D、layout(push_constant) uniform
	// VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_FRAGMENT_BIT|VK_SHADER_STAGE_COMPUTE_BIT
	// todo set_pool_info
	class set_pool_info
	{
	public:
		dvk_device* _dev = nullptr;
		// set池对象
		VkPipelineLayout pipelineLayout = 0;
		//VkDescriptorSetLayout
		// 属于layout(set = 1
		std::vector<VkDescriptorSetLayout> set_ptr;
		// 分配池
		std::vector<VkDescriptorPool> pools;
		size_t pools_idx = 0;
		std::map<uint32_t, uint32_t> _ps;

		int max_sets = 32;
		uint16_t push_constant_size = 128;
		uint16_t pct_vert = 64;
		uint16_t pct_frag = 64;
		std::vector<uint32_t> sets;// = VK_SHADER_STAGE_ALL_GRAPHICS;
		// set,bind,type
		std::map<int, std::map<int, desc_type>> _binding;


	public:
		std::vector<std::string> names;
	public:
		set_pool_info();

		~set_pool_info();
		void set_push_constant_size(int vert_size, int frag_size);
		// VERTEX_BIT=1,FRAGMENT_BIT=2,COMPUTE_BIT=4
		static uint32_t get_stage(uint8_t stage_flags);
		// 增加池子VERTEX_BIT=1|FRAGMENT_BIT=2|COMPUTE_BIT=4
		void add_set(uint8_t stage_flags);
		// 增加uniform buffer
		void add_ubo(int idx, int binding, uint8_t count, uint8_t stage_flags, bool dynamic);
		// 增加纹理
		void add_image(int idx, int binding, uint8_t count, uint8_t stage_flags);
		// 绑定添加VkDescriptorType
		void add_binding(int idx, int binding, uint32_t descriptor_type, uint8_t count, uint8_t stage_flags);
		void set_binding(std::map<int, std::map<int, desc_type>>& binding);
	public:
		// vk操作
		void make_set_pool(dvk_device* dev);
		std::vector<VkDescriptorSet> new_sets();
		size_t new_sets(std::vector<VkDescriptorSet>& out);
		size_t new_sets(std::vector<VkDescriptorSet>& out, int idx, int n);
		static void test();
	private:

	};
	class bset_info
	{
	public:
		int set = 0;
		int binding = 0;
		uint32_t descriptorType = 0;
		dvk_texture* texture = 0;
		dvk_buffer* buffer = 0;
		uint64_t size = 0;
		uint64_t offset = 0;
	public:
		bset_info(int set1, int binding1, uint32_t descriptorType1, dvk_texture* texture1) :set(set1), binding(binding1), descriptorType(descriptorType1), texture(texture1)
		{}
		bset_info(int set1, int binding1, uint32_t descriptorType1, dvk_buffer* buffer1, uint64_t size1, uint64_t offset1) :set(set1), binding(binding1), descriptorType(descriptorType1)
			, buffer(buffer1), size(size1), offset(offset1)
		{}
		bset_info()
		{
		}

		~bset_info()
		{
		}

	private:

	};
	class shader_info;

	struct ubo_set_t {
		VkDescriptorSet dset = 0;
		uint32_t first_set = 0;
		uint32_t offsets = 0;
		uint32_t offsetsa = 0;		// 64k范围偏移
		uint32_t size = 0;			// 大小
	};
	// todo dvk_set
	class dvk_set
	{
	public:
		shader_info* _pipe = 0;
		//std::unordered_map<std::string, BindInfo>	paramsMap;
		dvk_device* _dev = nullptr;
		spv_res_info* setLayoutsInfo = nullptr;

		//DVKDescriptorSetLayoutsInfo

		std::vector<uint32_t> dynamicOffsets;
		std::vector<bset_info> _cy_info;
		// cmdbind时用
		std::map<int, std::vector<VkDescriptorSet>>	_dset;
		std::map<VkDescriptorSet, uint32_t> _first_set;
	public:

		dvk_set(dvk_device* dev, spv_res_info* p, shader_info* ps);

		//dvk_set();
		~dvk_set();
		ubo_set_t write_image(const std::string& name, dvk_texture* texture, VkDescriptorSet dstSet = 0);
		VkDescriptorSet write_image(int set, int binding, uint32_t descriptorType, dvk_texture* texture, VkDescriptorSet dstSet = 0);

		ubo_set_t write_buffer(const std::string& name, dvk_buffer* buffer, uint64_t size, uint64_t offset, VkDescriptorSet dstSet = 0);
		VkDescriptorSet write_buffer(int set, int binding, uint32_t descriptorType, dvk_buffer* buffer, uint64_t size, uint64_t offset, VkDescriptorSet dstSet = 0);
		// 创建n个ubo set
		std::vector<VkDescriptorSet> get_ubos(const std::string& name, dvk_buffer* buffer, int count, uint64_t size, uint64_t first_offset);
		//inline uint32_t size() { return descriptorSets.size(); }
		// 从dvk_set复制同样的配置
		void set_info(dvk_set* ds);
		// 设置只有1个纹理、ubo的绑定
		std::vector<VkDescriptorSet> set_one(dvk_texture* texture, dvk_buffer* buffer, uint64_t size, uint64_t offset);
		uint32_t get_setid(VkDescriptorSet st);
		uint32_t get_set_maxs();

	};


	struct vinbind_info {
		uint16_t binding;
		uint16_t stride;
		uint16_t inputRate;//VERTEX=0, INSTANCE=1
	};
	struct new_vertex_in_attr_info {
		uint8_t location;
		uint8_t binding;
		uint8_t format;
		uint8_t offset;
	};
	// todo 管线信息shader_info
	class shader_info
	{
	public:
		// out
		VkPipeline _pipe[2] = {};

		dvk_device* _dev = nullptr;

		pipe_info_s* shader_data = 0;
		std::vector<int> shader_idx;

		set_pool_info _spinfo = {};
		// input、set/绑定信息
		spv_res_info _srinfo = {};


	public:
		shader_info()
		{
		}

		virtual ~shader_info();
	public:
		int isDynamic(int first, int n);
		VkPipelineLayout get_pipelineLayout();
		VkPipeline get_vkpipe(bool depthWriteEnable = true);
		//创建全部set
		std::vector<VkDescriptorSet> new_sets();
		size_t new_sets(std::vector<VkDescriptorSet>& out);
		// 创建分散set
		size_t new_sets(std::vector<VkDescriptorSet>& out, int idx, int n);
	public:
		dvk_set* new_dvkset();
		//dvk_set* new_dvkset(int n);
	private:

	};
	// todo  pipeline_ptr_info
	class pipeline_ptr_info :public shader_info
	{
	public:
		// in
		//VkPipelineLayout PipelineLayout;在_spinfo
		//uint64_t pipeline_layout = 0;
		VkRenderPass render_pass = 0;
		VkPipelineCache pipelineCache = 0;
		// 动态改变的状态VkDynamicState，默认有VK_DYNAMIC_STATE_VIEWPORT=0，VK_DYNAMIC_STATE_SCISSOR=1
		std::set<uint32_t> dynamic_states;
		/* VBO、实例结构绑定，一个vbo就一个vinbind
		std::vector<vinbind_info> vertex_bindings;
		// stride可以空，stride_size则自动绑定一个vbo
		short stride_size = 0;
		// 必选，顶点信息
		std::vector<new_vertex_in_attr_info> vertex_attrs;*/
		uint8_t viewportCount = 1;
		uint8_t scissorCount = 1;
		/*
			参考VkPrimitiveTopology
			POINT_LIST = 0,
			LINE_LIST = 1,
			LINE_STRIP = 2,
			TRIANGLE_LIST = 3,
			TRIANGLE_STRIP = 4,
			TRIANGLE_FAN = 5
		*/
		uint32_t topology = 3;
		// 混合颜色常量RGBA vec4
		float blendConstants[4] = {};
		pipeline_state_obj_t pso = {};

		// vbo绑定数量{x=属性数量，y=顶点0、实例1}
		std::vector<glm::ivec2> vinput_info;
		std::string name;
		int align = sizeof(size_t);
	public:
		pipeline_ptr_info();
		~pipeline_ptr_info();
	public:
		void init(VkRenderPass render_pass);
		//n = 属性数量，t = 顶点0、实例1
		void add_input_vbo(int n, int t);
	};
	// todo dvk_cmd
	class dvk_cmd;
	class gpu_info;
	class submit_info;
	class fbo_info_cx;

	// todo vk_cmd_pool
	class dvk_cmd_pool
	{
	public:
		VkCommandPool command_pool = 0;
		dvk_device* _dev = nullptr;
		std::vector<VkCommandBuffer> cmdv;
		std::vector<uint32_t> _count;
	public:
		dvk_cmd_pool();
		dvk_cmd_pool(VkCommandPool cp, dvk_device* dev);

		~dvk_cmd_pool();
		void init(VkCommandPool cp, dvk_device* dev);
		/*
		* secondary=true无法直接提交，但可以从主命令缓冲区调用
		* 这里new的cmd可以自动释放
		*/
		dvk_cmd* new_cmd(bool secondary, bool isc);
		void new_cmd(std::vector<VkCommandBuffer>& out, bool secondary);
	public:
	};
	// todo dvk_fence
	class dvk_fence
	{
	private:
		VkFence fence = 0;
		VkDevice dev = 0;
	public:
		dvk_fence() {}
		dvk_fence(VkDevice d);

		~dvk_fence();
		void init(VkDevice d);
	public:
		void reset();
		void wait_for();
		int get_status();
		//operator VkFence() { return fence; };
		VkFence get() { return fence; };

	};

	// ut_compute
	class ut_compute
	{
	private:
		void* _compute = nullptr;
		dvk_cmd_pool* cmd_pool = nullptr;
		bool is_begin = false;
	public:
		ut_compute(void* p);

		~ut_compute();
		void set_cp(dvk_cmd_pool* p);
		dvk_cmd* new_cmd();
	public:
		void bind(dvk_cmd* cmd, dvk_set* dsp);
		dvk_set* new_set();
		void set_push_constant(dvk_cmd* commandBuffer, void* data, uint32_t size, uint32_t offset);
		//void setUniform(const std::string& name, void* dataPtr, uint32_t size);
		//void setTexture(const std::string& name, dvk_texture* texture);
		//void setStorageTexture(const std::string& name, dvk_texture* texture);
		//void setStorageBuffer(const std::string& name, void* buffer);
	};

	// todo dvk_swapchain交换链
	class dvk_swapchain
	{
	public:
		VkSurfaceKHR surface = 0;
		dvk_device* _dev = 0;

		uint32_t colorFormat = 0;
		//VkFormat colorFormat;
		uint32_t colorSpace = 0;
		//VkColorSpaceKHR colorSpace;
		/** @brief Handle to the current swap chain, required for recreation */
		VkSwapchainKHR swapChain = 0;
		uint32_t imageCount = 0;
		std::vector<VkImage> images;
		std::vector<VkSemaphore> semaphore;
		// Index of the deteced graphics and presenting device queue
		/** @brief Queue family index of the detected graphics and presenting device queue */
		uint32_t queueNodeIndex = UINT32_MAX;
		uint32_t _width = 0, _height = 0;
		uint32_t c_width = 0, c_height = 0;
		int _semaphoreIndex = 0;
		uint64_t _NumAcquireCalls = 0;
		uint32_t _curr_imageIndex = 0;
		class scb_t
		{
		public:
			VkImage image = 0;
			VkImageView view = 0;
			//VkFormat colorFormat;
			uint32_t colorFormat = 0;
		public:
			scb_t()
			{
			}

			~scb_t()
			{
			}

		private:

		};
		std::vector<scb_t> buffers;
	public:
		dvk_swapchain(dvk_device* dev, VkSurfaceKHR surfacekhr);
		virtual ~dvk_swapchain();
		// Creates an os specific surface
		/**
		* Create the surface object, an abstraction for the native platform window
		*
		* @pre Windows
		* @param platformHandle HINSTANCE of the window to create the surface for
		* @param platformWindow HWND of the window to create the surface for
		*
		* @pre Android
		* @param window A native platform window
		*
		* @pre Linux (XCB)
		* @param connection xcb connection to the X Server
		* @param window The xcb window to create the surface for
		* @note Targets other than XCB ar not yet supported
		*/

		void initSurface(dvk_device* dev, VkSurfaceKHR surfacekhr);

		/**
		* Create the swapchain and get it's images with given width and height
		*
		* @param width Pointer to the width of the swapchain (may be adjusted to fit the requirements of the swapchain)
		* @param height Pointer to the height of the swapchain (may be adjusted to fit the requirements of the swapchain)
		* @param vsync (Optional) Can be used to force vsync'd rendering (by using VK_PRESENT_MODE_FIFO_KHR as presentation mode)
		*/
		bool create(uint32_t* width, uint32_t* height, bool vsync = false);
		std::vector<scb_t>* getSwapChainVs();
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
		uint32_t acquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t* imageIndex);

		/**
		* Destroy and free Vulkan resources used for the swapchain
		*/
		void cleanup();

#if defined(_DIRECT2DISPLAY)
		/**
		* Create direct to display surface
		*/
		void createDirect2DisplaySurface(uint32_t width, uint32_t height);
#endif
	};

	// todo fbo_cx
	class fbo_cx
	{
	public:
		using drawcb_func = std::function<void(dvk_cmd* cmd)>;
	private:
		fbo_info_cx* ctx = 0;
		dvk_device* _dev = nullptr;
		int _width = 0; int _height = 0; unsigned int _clear_color = 0;
		dvk_swapchain* _swapcain = 0;
		dvk_queue* _queue = 0;
		dvk_cmd_pool* cmd_pool = 0;
		submit_info* _submitinfo = 0;
		VkSemaphore _presentComplete = 0;
		std::atomic_int _isreset;
		int r_width = 0; int r_height = 0;
		void* uds[6] = {};
		game_time* _gt = 0;
		//相机
		Camera* camera = 0;
		dvk_buffer* _ortho_ubo = 0; uint32_t _offset = 0;
		std::vector<drawcb_func> draw_call_cbs;
		std::atomic_int _incup = 0;
		// 是否判断更新窗口大小
		bool _is_getsize = false;
		auto_destroy_cx dc;
	public:
		double _gstime = 0.0;
		double _fixed_gs_time = 0.1;
		std::atomic_int _upinc;
	public:
		fbo_cx(int width, int height, unsigned int clear_color);

		~fbo_cx();
		void init(dvk_device* dev, int count, void* pass, dvk_swapchain* swapchain);
		void set_queue(dvk_queue* qp, game_time* gt);
		void set_clear_color(unsigned int clear_color);
		void build_cmd(int idx, std::function<void(dvk_cmd* cmd)> cb);
		void build_cmd_cb();
		void submit(int idx = -1);
		void save_image(const std::string& fn);
		void to_image(Image* img, int idx);
		// 获取所属设备
		dvk_device* get_dev();
		VkRenderPass get_render_pass();
		dvk_swapchain* get_swapchain();

		glm::vec2 get_size();
		// 获取格式化偶数大小
		glm::vec2 get_size2();
		glm::mat4* get_ortho_graphic();
		// 支持普通全局函数、静态函数、以及lambda表达式void(VkCommandBuffer commandBuffer, size_t index)
		template<typename Function, typename... Args>
		void push_drawcb(const Function& func, Args... args)
		{
			drawcb_func f = [&func, args...](dvk_cmd* cmd){ return func(cmd, args...); };
			draw_call_cbs.push_back(f);
		}
		//void set_drawcb(std::function<void(dvk_cmd* cmd)> cb);
		void set_ud(int idx, void* ud);
		void* get_ud(int idx);
		// 设置正交投影ubo，窗口改变时自动更新ubo
		void set_ortho_ubo(dvk_buffer* ubo, uint32_t offset);
	public:
		void acquire(uint32_t* imageIndex);
		submit_info* get_cur_submit();
		// 重置大小
		bool resize(int width, int height);
		bool resize_view(int width, int height);
		void begin();
		void end();
		bool dev_resize(bool isout);
		void update_ubo(glm::mat4& m);
		bool is_update();
		void inc_update();
		void dec_update();
		void set_gs();
		void close_gs();
		bool is_gs();
	private:
		void draw_call_cb(dvk_cmd* cmd);
	};
	class vk_base_info;
	// todo vk_render_cx
	class vk_render_cx
	{
	private:
		// base info
		vk_base_info* ctx = nullptr;
		VkInstance _instance = nullptr;
		gpu_info* _gpu = 0;
		auto_destroy_cx dc;
	public:
		vk_render_cx();

		~vk_render_cx();

	public:

	public:
		// , void* pass可以设置nullptr
		fbo_cx* new_fbo_swapchain(int width, int height, int* count, VkSurfaceKHR surface, unsigned int clear_color, dvk_device* dev, void* pass = nullptr);
		fbo_cx* new_fbo_image(int width, int height, int count, unsigned int clear_color, dvk_device* dev, void* pass = nullptr);
		dvk_device* get_dev(unsigned int idx);
		dvk_device* get_dev(const char* name);
		void load_pipe(dvk_device* dev_, pipe_create_info* info, uint16_t count, pipe_info_s* out);
		bool new_pipeline(dvk_device* dev_, pipeline_ptr_info* info);
		void free_pipe_info(pipe_info_s* p);
	public:
		int get_push_constant_size(size_t idx);
		VkInstance get_instance();
	private:



	};
	// todo shader_hp
	class shader_hp
	{
	public:
		struct pipe_od
		{
			std::string fn;
			std::vector<char> data;
		};
	private:
		pipe_info_s ps[1] = {};
		dvk_device* _dev = nullptr;
		vk_render_cx* _ctx = nullptr;
		std::vector<pipe_od> sod;
		std::vector<pipe_create_info> pci;
		auto_destroy_cx dc;

		// json 数据
		std::vector<njson> _jnd;
		size_t pipeidx = 0;

		std::map<std::string, pipeline_ptr_info*> mpipe;
	public:
		shader_hp();

		~shader_hp();
		// shader_path存放shader目录
		void init(vk_render_cx* ctx, dvk_device* dev, const t_vector<t_string>& shader_path_fn);
		void clear();
		// todo layout(set=0, binding=0) uniform、uniform sampler2D、uniform sampler2DArray
		// idx空的话，则选择fns全部
		pipeline_ptr_info* new_pipe(pipeline_ptr_info* sp, const std::vector<std::string>& fns, const std::vector<int>& idx = {});
		ut_compute* new_compute(dvk_device* dev_, const std::vector<std::string>& fns, const std::vector<int>& idx = {});
		int new_pipe(const njson& info, VkRenderPass render_pass);
		std::map<std::string, pipeline_ptr_info*>& mk_pipe(VkRenderPass render_pass);
	private:
		shader_info* init_pipe(shader_info* sp, const std::vector<std::string>& fns, const std::vector<int>& idx = {});
		void load_shader_zip(const std::string& zfn, std::vector<pipe_od>& sod, std::vector<pipe_create_info>& out);
		void load_file(const std::string& path, std::vector<pipe_od>& sod, std::vector<pipe_create_info>& out);
	};


}
//!hz
