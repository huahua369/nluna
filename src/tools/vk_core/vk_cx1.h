#pragma once
#include <thread>
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
#ifdef _WIN32
#define OUT_FILE_LINE_ printf("当行：%d\t<%s>\n", __LINE__, __FILE__)
#define OUT_LINE(str) printf("当行：%d\t<%s>\n", __LINE__, str)
#else
#define OUT_FILE_LINE_ 
#define OUT_LINE
#endif // _WIN32
#ifndef VkPhysicalDeviceProperties
struct VkPhysicalDeviceProperties;
#endif // !VkPhysicalDeviceProperties
#ifndef VkWriteDescriptorSet
struct VkWriteDescriptorSet;
#endif // !VkWriteDescriptorSet
//struct IMG_INFO;
#include "view_info.h"

namespace hz {
	class stbimage_cx;
	class sdl_form;
	class Image;
	class dvk_queue;


	enum class IBO_type {
		t_ushort = 0,
		t_uint32 = 1
	};

	struct IMG_INFO
	{
		uint32_t           width = 0;
		uint32_t           height = 0;
		uint32_t           depth = 1;
		uint32_t           arraySize = 1;
		uint32_t           mipMapCount = 1;
		uint32_t			format = {};
		uint32_t           bitCount = 0;
		uint8_t* data = 0;
		uint32_t size = 0;
		uint32_t vkformat = 0;
	};


	class Sync
	{
		int m_count = 0;
		std::mutex m_mutex;
		std::condition_variable condition;
	public:
		int Inc()
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_count++;
			return m_count;
		}

		int Dec()
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_count--;
			if (m_count == 0)
				condition.notify_all();
			return m_count;
		}

		int Get()
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			return m_count;
		}

		void Reset()
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_count = 0;
			condition.notify_all();
		}

		void Wait()
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			while (m_count != 0)
				condition.wait(lock);
		}

	};

	class UploadHeap
	{
		Sync allocating, flushing;
		struct COPY
		{
			VkImage m_image; VkBufferImageCopy m_bufferImageCopy;
		};
		std::vector<COPY> m_copies;

		std::vector<VkImageMemoryBarrier> m_toPreBarrier;
		std::vector<VkImageMemoryBarrier> m_toPostBarrier;

		std::mutex m_mutex;
	public:
		void OnCreate(dvk_device* pDevice, size_t uSize);
		void OnDestroy();

		uint8_t* Suballocate(size_t uSize, uint64_t uAlign);
		uint8_t* BeginSuballocate(size_t uSize, uint64_t uAlign);
		void EndSuballocate();
		uint8_t* BasePtr() { return m_pDataBegin; }
		VkBuffer GetResource() { return m_buffer; }
		VkCommandBuffer GetCommandList() { return m_pCommandBuffer; }

		void AddCopy(VkImage image, VkBufferImageCopy bufferImageCopy);
		void AddPreBarrier(VkImageMemoryBarrier imageMemoryBarrier);
		void AddPostBarrier(VkImageMemoryBarrier imageMemoryBarrier);

		void Flush();
		void FlushAndFinish(bool bDoBarriers = false);
	public:

		dvk_device* dev = 0;
	private:


		VkCommandPool           m_commandPool;
		VkCommandBuffer         m_pCommandBuffer;

		VkBuffer                m_buffer;
		VkDeviceMemory          m_deviceMemory;

		VkFence m_fence;

		uint8_t* m_pDataBegin = nullptr;    // starting position of upload heap
		uint8_t* m_pDataCur = nullptr;      // current position of upload heap
		uint8_t* m_pDataEnd = nullptr;      // ending position of upload heap 
	};


	// todo gpu info
	class gpu_info
	{
	public:
		struct devinfo_t {
			VkPhysicalDevice pd;
			VkPhysicalDeviceProperties* pdp;
		};
		VkInstance _instance = nullptr;
		//std::vector<VkPhysicalDeviceProperties> gpu_infos;
		//void* _gpu_infos = nullptr;
		std::map<std::string, devinfo_t> gpusname;		//所有GPU名称

	public:
		gpu_info();
		~gpu_info();
		void init();
		//int get_push_constant_size(size_t idx);
		//std::string get_name(size_t idx);
		//std::vector<VkPhysicalDevice> get_physicals();

		std::string get_idx(const char* name, int& idx);
	public:

	private:

		int enum_gpus();

	};

	template<typename T> inline T alignUp(T& val, T alignment)
	{
		auto r = (val + alignment - (T)1) & ~(alignment - (T)1);
		val = r;
		return r;
	}

	// align val to the next multiple of alignment
	template<typename T> inline T AlignUp(T val, T alignment)
	{
		return (val + alignment - (T)1) & ~(alignment - (T)1);
	}
	// align val to the previous multiple of alignment
	template<typename T> inline T AlignDown(T val, T alignment)
	{
		return val & ~(alignment - (T)1);
	}
	template<typename T> inline T DivideRoundingUp(T a, T b)
	{
		return (a + b - (T)1) / b;
	}

	class dynamic_buffer_cx
	{
	public:
		dvk_device* dev = 0;
		dvk_buffer* _ubo = 0;
		char* mdata = 0;
		int64_t last = 0;
		uint32_t _ubo_align = 64;
	public:
		// acsize预分配大小，是否vbo\ibo混用vibo = false, 是否混用transfer
		dynamic_buffer_cx(dvk_device* d, size_t acsize, bool vibo = false, bool transfer = false);
		~dynamic_buffer_cx();
	public:
		// 增加空间，原先分配的空间则会失效
		void append(size_t size);
		// 重新获取数据指针
		char* get_ptr(uint32_t offset);
		void flush(size_t pos, size_t size);
		// 清空分配
		void clear();
		// 删除内存显存占用
		void free_mem();
		// 分配n个对象大小的空间
		template<class T>
		uint32_t alloc_obj(T*& t, int n = 1) {
			uint32_t r = 0;
			t = (T*)alloc(sizeof(T) * n, &r);
			return r;
		}
		char* alloc(size_t size, uint32_t* offset);
		bool AllocConstantBuffer(uint32_t size, void** pData, VkDescriptorBufferInfo* pOut);
	private:

	};
	class upload_cx
	{
	public:
		struct COPY_T
		{
			VkImage _image; VkBufferImageCopy _bic;
		};
		dvk_device* _dev = nullptr;						// 设备
		dynamic_buffer_cx* db = 0;						// 缓存自动扩容
		size_t ncap = 0;								// 初始大小
		size_t last_size = 0, last_pos = 0, ups = 0;	// 最后的大小
		t_vector<VkImageMemoryBarrier> toPreBarrier;
		t_vector<COPY_T> _copies;
		t_vector<VkImageMemoryBarrier> toPostBarrier;
		VkQueue  _queue = {};
		VkCommandPool _commandPool = {};
		VkCommandBuffer _pCommandBuffer = {};
		VkFence _fence = {};
		bool isprint = true;
	public:
		upload_cx();
		~upload_cx();
		// size初始缓存大小
		void init(dvk_device* dev, size_t size, int idxqueue);
		void on_destroy();
		// 纹理数据复制到这里
		char* get_tbuf(size_t size, size_t uAlign);
		void AddPreBarrier(VkImageMemoryBarrier imb);
		void addCopy(VkImage image, VkBufferImageCopy bic);
		void AddPostBarrier(VkImageMemoryBarrier imb);

		void flush();
		int flushAndFinish(int wait_time = -1);
		// 显存不够用时，没纹理上传，释放缓存占用
		void free_buf();

		VkBuffer get_resource();
		VkCommandBuffer get_cmdbuf();
	public:
		void add_pre(VkImage image, VkFormat format, uint32_t aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout
			, uint32_t mipLevel = 1, uint32_t layerCount = 1);
		void add_post(VkImage image, VkFormat format, uint32_t aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout
			, uint32_t mipLevel = 1, uint32_t layerCount = 1);
	private:

	};


	// todo dvk_texture
	class dvk_texture
	{
	public:
		VkImage _image = 0;
		VkImageView _view = 0;
		VkSampler sampler = 0;
		VkDeviceMemory image_memory = 0;
		int64_t cap_device_mem_size = 0;
		int cap_inc = 0, caps = 8;			// 分配8次就重新释放显存
		VkDescriptorImageInfo* descriptor = {};
		VkImageCreateInfo* _info = 0;
		uint32_t width = 0, height = 0;
		uint32_t mipLevels = 1;
		uint32_t layerCount = 1;
		uint32_t _depth = 1;
		//VkDescriptorType dtype = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		//VkFormat _format = VK_FORMAT_R8G8B8A8_UNORM;
		uint32_t _format = 37;
		int64_t _alloc_size = 0;		// 数据字节大小
		dvk_device* _dev = nullptr;
		t_string _name;
		ImageLayoutBarrier _image_layout = ImageLayoutBarrier::SHADER_READ;
		void* user_data = 0;
		Image* uimg = 0;
		void* mapped = nullptr;
		IMG_INFO* _header = 0;
		void* ad = 0;
		void* ad1 = 0;
		pipeline_ptr_info* pipe = 0;
		yuv_info_t yuv = {};
		VkSamplerYcbcrConversion ycbcr_sampler_conversion = {};
	public:
		dvk_texture();
		dvk_texture(dvk_device* dev);

		~dvk_texture();

		bool check_format();

		void OnDestroy();
	public:

		static dvk_texture* new_yuv(yuv_info_t yuv, upload_cx* up);
		//static dvk_texture* new_2d(const std::string& filename, upload_cx* up, ImageLayoutBarrier imageLayout = ImageLayoutBarrier::SHADER_READ);
		static dvk_texture* new_image2d(Image* img, upload_cx* up, ImageLayoutBarrier imageLayout = ImageLayoutBarrier::SHADER_READ, bool storage = false);
		static dvk_texture* new_image2d(Image* img, dvk_device* dev, ImageLayoutBarrier imageLayout = ImageLayoutBarrier::SHADER_READ, bool storage = false);
		static dvk_texture* new_image2d(void* buffer, VkDeviceSize bufferSize, uint32_t format, uint32_t w, uint32_t h
			, upload_cx* up, uint32_t imageUsageFlags, ImageLayoutBarrier imageLayout = ImageLayoutBarrier::SHADER_READ);
		static dvk_texture* new_image2d(const char* fn, upload_cx* up, bool srgb);
		static dvk_texture* new_image2d(upload_cx* up, const void* data, int size, int width, int height, bool useSRGB, uint32_t format, uint32_t dxformat);
		static dvk_texture* new_storage2d(Image* img, upload_cx* up, bool is_compute = false);
		static dvk_texture* new_storage2d(const std::string& filename, upload_cx* up, bool is_compute = false);
		static dvk_texture* new_storage2d(dvk_device* dev, int width, int height, uint32_t format = 0, bool is_compute = false, bool sampled = true);
		static dvk_texture* new_render_target_color(dvk_device* dev, int width, int height, uint32_t format = 0, uint32_t sampleCount = 1);
		static dvk_texture* new_render_target_depth(dvk_device* dev, int width, int height, uint32_t format = 0, uint32_t sampleCount = 1);

		Image* save2Image(Image* outimage, dvk_queue* q, bool unpm);
		bool save2file(const char* fn, dvk_queue* q, bool unpm);
		VkDescriptorImageInfo* get_descriptor_image_info();

	public:
		int32_t Init(dvk_device* pDevice, VkImageCreateInfo* pCreateInfo, const char* name = nullptr);
		int32_t InitRenderTarget(dvk_device* pDevice, uint32_t width, uint32_t height, VkFormat format, VkSampleCountFlagBits msaa, VkImageUsageFlags usage, bool bUAV, const char* name = nullptr, VkImageCreateFlagBits flags = (VkImageCreateFlagBits)0);
		int32_t InitDepthStencil(dvk_device* pDevice, uint32_t width, uint32_t height, VkFormat format, VkSampleCountFlagBits msaa, const char* name = nullptr);
		bool InitFromFile(upload_cx* pUploadHeap, const char* szFilename, bool useSRGB = false, VkImageUsageFlags usageFlags = 0, float cutOff = 1.0f);
		bool InitFromData(upload_cx* uploadHeap, IMG_INFO& header, VkImageUsageFlags usageFlags = 0, bool useSRGB = false, const char* name = nullptr);

		VkImage Resource() const { return _image; }
		// 目标视图
		VkImageView CreateRTV(VkImageView* pRV, int mipLevel = -1, VkFormat format = VK_FORMAT_UNDEFINED);
		// shader资源视图
		VkImageView CreateSRV(VkImageView* pImageView, int mipLevel = -1);
		// 深度
		VkImageView CreateDSV(VkImageView* pView);
		VkImageView CreateCubeSRV(VkImageView* pImageView);
		// 创建image
		VkImage CreateTextureCommitted(upload_cx* pUploadHeap, const char* pName, bool useSRGB, VkImageUsageFlags usageFlags);
		void upload_data(upload_cx* up, IMG_INFO* info, uint32_t bufferOffset);

		void set_data(const void* data, int size, int width, int height, uint32_t format, upload_cx* up);

		void set_data(Image* img, upload_cx* up);

		void up_yuv(yuv_info_t yuv, upload_cx* up);
	private:
		void free_image();
		static dvk_texture* new2d_priv(const std::string& filename
			, upload_cx* up
			, ImageLayoutBarrier imageLayout /*= ImageLayoutBarrier::SHADER_READ*/
			//VkImageUsageFlags
			, uint32_t imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT*/
		);
#if 0
		void set_data(
			void* buffer,
			VkDeviceSize bufferSize,
			uint32_t format,
			uint32_t w,
			uint32_t h,
			upload_cx* up,
			uint32_t imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/,
			//VkImageLayout
			uint32_t imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/);
		void mkrt_image(int w, int h, uint32_t format, bool is_depth, uint32_t usage, ImageLayoutBarrier image_layout);
		void new_storage(int width, int height, uint32_t format, bool is_compute, bool sampled);

		void set_data(dvk_staging_buffer* staging, bool useSRGB, IMG_INFO* info, upload_cx* up, uint32_t imageUsageFlags, uint32_t imageLayout);

#endif
	public:
		void get_buffer(char* outbuf, dvk_queue* q);

		char* map();
		void unmap();
	};



	// todo dvk_cmd
	class dvk_cmd
	{
	public:
		VkCommandBuffer cmdb = 0;
		glm::vec2 _viewport;
		//private:
		dvk_device* _dev = 0;
		bool is_begin = false;
		float minDepth = 0.0;
		float maxDepth = 1.0;
		shader_info* _pipeline = 0;
		bool _is_compute = false;
		uint32_t bindPoint = 0;
	public:
		dvk_cmd() {
			//bindPoint = uint32_t(!_is_compute ? VK_PIPELINE_BIND_POINT_GRAPHICS : VK_PIPELINE_BIND_POINT_COMPUTE);
		}
		dvk_cmd(dvk_device* dev, VkCommandBuffer cb, bool is_compute);

		~dvk_cmd();
		VkCommandBuffer ptr() { return cmdb; }
		//operator VkCommandBuffer() { return cmdb; }
	public:
		void begin();
		void end();

		void barrier_mem_compute(bool src_write);
		void barrier_buffer(VkBuffer bufs, uint32_t size, int src_familyidx, int dst_familyidx, uint32_t srcStageMask, uint32_t dstStageMask, bool access_r2w);

		// srcStageMask和dstStageMask等于0的时候access_r2w =true时  srcStageMask=VK_ACCESS_SHADER_READ_BIT和srcStageMask=VK_ACCESS_SHADER_WRITE_BIT
		void barrier_image(dvk_texture* img, int src_familyidx, int dst_familyidx
			, ImageLayoutBarrier oldLayout, ImageLayoutBarrier newLayout
			, bool access_r2w, uint32_t srcStageMask = 0, uint32_t dstStageMask = 0);

		void blit_image(dvk_texture* dst, dvk_texture* src, bool is_linear = true);
	public:

		// set
		//void SetViewport(uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports);
		//void SetScissor(uint32_t firstScissor, uint32_t	scissorCount, const VkRect2D* pScissors);
		//void SetLineWidth(float lineWidth);
		void SetBlendConstants(const float blendConstants[4]);
		void SetDepthBiasBounds(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor, float minDepthBounds, float maxDepthBounds);
		// VkStencilFaceFlags
		void SetStencilCompareMask(uint32_t faceMask, uint32_t compareMask);
		void SetStencilWriteMask(uint32_t faceMask, uint32_t writeMask);
		void SetStencilReference(uint32_t faceMask, uint32_t reference);
		void set_viewport(const glm::vec4* s, uint32_t count, uint32_t first, float minDepth = 0.0, float maxDepth = 1.0);
		void set_viewport(glm::ivec4 s, uint32_t first, float minDepth = 0.0, float maxDepth = 1.0);
		void set_scissor(const glm::vec4* pScissors, uint32_t count, uint32_t first);
		void set_scissor(const glm::ivec4* pScissors, uint32_t first);
		void set_scissor(const glm::ivec4& scissors, uint32_t first);
		void set_state(uint32_t s, uint32_t cull_mod, uint32_t front_face, uint32_t primitive_topology);
		// bind
		void bind_pipeline(shader_info* pipeline, bool depthTestEnable, bool depthWriteEnable);
		//void bind_set(dvk_set* dsp, uint32_t first_set = 0);
		void bind_set(const VkDescriptorSet* sets, uint32_t count, uint32_t first_set, std::vector<uint32_t>* offsets = 0) const;
		void bind_set(const VkDescriptorSet* sets, uint32_t count, uint32_t first_set, uint32_t offsets) const;
		void bind_set(VkDescriptorSet sets, uint32_t first_set, uint32_t offsets) const;
		void bind_set(ubo_set_t* ubo) const;
		void bind_set(ubo_set_t ubo) const;
		void bind_set(VkWriteDescriptorSet* pds, uint32_t count, uint32_t setidx);
		// index_type 0=u16	1=u32
		void bind_ibo(VkBuffer buffer, VkDeviceSize offset, uint8_t index_type);
		void bind_ibo(VkBuffer buffer, VkDeviceSize offset, IBO_type index_type);
		void bind_ibo16(VkBuffer buffer, VkDeviceSize offset);
		void bind_ibo32(VkBuffer buffer, VkDeviceSize offset);
		void bind_vbos(uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets);
		// 绑定单个vbo
		void bind_vbo(VkBuffer pBuffer, VkDeviceSize offset, uint32_t firstBinding = 0);

		// draw
		void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
		void draw_indexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
		void draw(uint32_t vertexCount, uint32_t firstVertex);
		void draw_indexed(uint32_t indexCount, uint32_t firstIndex, int32_t vertexOffset);
		// compute
		void dispatch(uint32_t gcx, uint32_t gcy, uint32_t gcz);
		void dispatchIndirect(VkBuffer buffer, VkDeviceSize offset);
		// 执行一cmd数组
		void exe_cmds(const VkCommandBuffer* pCommandBuffers, uint32_t count);
	public:

	};

	// todo submit_info
	class submit_info
	{
	public:
		int idx = 0;
		std::vector<VkSemaphore> pWaitSemaphores;
		//std::vector<VkPipelineStageFlags> pWaitDstStageMask;
		std::vector<uint32_t> pWaitDstStageMask;
		std::vector<VkCommandBuffer> pCommandBuffers;
		std::vector<VkSemaphore> pSignalSemaphores;

	public:
		submit_info();

		~submit_info();
	public:
		void clear();
		void push_cmd(VkCommandBuffer cmd, VkSemaphore signal = 0, VkSemaphore wait = 0, uint32_t wait_dst_stage_mask = 0);
		VkSemaphore get_idx_sem(size_t idx);
	private:

	};

	typedef uint64_t submits;
	typedef uint64_t bind_sparses;

	// todo 交换链提交管理
	class present_info
	{
	public:
		std::vector<VkSwapchainKHR> pSwapchains;
		std::vector<uint32_t> pImageIndices;

		// 可选，pWaitSemaphores、pResults不为空时必需数量和pSwapchains一致
		std::vector<VkSemaphore> pWaitSemaphores;
		// pSwapchains和pImageIndices的数量必需一致
		// 可选
		//std::vector<VkResult> pResults;
		void* pResults = 0;
		// 可选
		dvk_fence* submit_fence = 0;
		struct idxwait {
			uint32_t idx = 0;
			VkSemaphore sem = 0;
		};
		std::map<dvk_swapchain*, idxwait> _swapchains;
	public:
		// 是否设置pResults
		present_info(bool is_rts = true);
		~present_info();
		void push(dvk_swapchain* sp);
		void update();
		// 设置显示的image
		void set_idx(dvk_swapchain* sp, uint32_t image_indices, VkSemaphore WaitSemaphores);

		void erase(dvk_swapchain* sp);
		size_t size();
	private:
		// 增加一个交换链用来显示
		void push_swapchain(VkSwapchainKHR swapchain, VkSemaphore WaitSemaphores);
		void reset();
	};
	class submit_infos
	{
	public:
		usp_ac uac;
		VkSubmitInfo* v = 0;
		size_t n = 0;
		size_t cap = 0;
		std::set<submit_info*> st;
	public:
		submit_infos();

		~submit_infos();

		// std::vector<VkSubmitInfo>
		void push(submit_info* psi);
		void clear();

		void update_sub();
		VkSubmitInfo* data();
		size_t size();
	private:

	};

	/*
		todo dvk_queue列队
		命令池从列队创建
	*/
	class dvk_queue
	{
	private:
		VkQueue  _queue = 0;
		uint32_t _fmily_indices = 0;
		dvk_queue_bit _t = dvk_queue_bit::GRAPHICS_BIT;
		dvk_device* _dev = 0;
	public:
		dvk_queue(dvk_device* dev, VkQueue q, uint32_t fmily_indices, dvk_queue_bit t);

		~dvk_queue();
	public:
		dvk_cmd_pool* new_cmd_pool();
		void free_cmd_pool(dvk_cmd_pool*);
		VkQueue get_vkptr() { return _queue; }
	public:
		// 提交列队
		uint32_t submit(submit_infos* sub, dvk_fence* fence, bool is_wait);
		// 提交稀疏内存绑定命令缓冲区bind_sparses=std::vector<VkBindSparseInfo>
		uint32_t bind_sparse(bind_sparses* bs, dvk_fence* fence, bool is_wait);
		// 批量提交显示
		uint32_t present(present_info* p);
	private:

	};
#define nfloat float
	//#define nfloat double
	class cpg_time
	{
	public:
		int deltaTimei = 0;
		// 只读
		nfloat time = 0.0;					//*The time at the beginning of this frame(Read Only).This is the time in seconds since the start of the game.
		nfloat deltaTime = 0.0;				//*The completion time in seconds since the last frame. (Read Only).
		nfloat fixedTime = 0.0;				//The time the latest FixedUpdate has started(Read Only).This is the time in seconds since the start of the game.
		nfloat fixedUnscaledDeltaTime = 0.0;//The timeScale - independent interval in seconds from the last fixed frame to the current one(Read Only).
		nfloat fixedUnscaledTime = 0.0;		//The TimeScale - independant time the latest FixedUpdate has started(Read Only).This is the time in seconds since the start of the game.
		nfloat frameCount = 0.0;			//*The total number of frames that have passed(Read Only).
		nfloat realtimeSinceStartup = 0.0;	//*The real time in seconds since the game started(Read Only).
		nfloat smoothDeltaTime = 0.0;		//*A smoothed out Time.deltaTime(Read Only).
		nfloat timeSinceLevelLoad = 0.0;	//*The time this frame has started(Read Only).This is the time in seconds since the last level has been loaded.
		nfloat unscaledDeltaTime = 0.0;		//*The timeScale - independent interval in seconds from the last frame to the current one(Read Only).
		nfloat unscaledTime = 0.0;			//*The timeScale - independant time for this frame(Read Only).This is the time in seconds since the start of the game.
		nfloat fps = 0.0f;
		// 可写
		nfloat captureFramerate = 0.0;		//Slows game playback time to allow screenshots to be saved between frames.
		nfloat timeScale = 1.0;				//The scale at which the time is passing.This can be used for slow motion effects.
		nfloat fixedDeltaTime = 0.02;		//The interval in seconds at which physicsand other fixed frame rate updates(like MonoBehaviour's FixedUpdate) are performed.

		nfloat maximumDeltaTime = 1.0 / 3.0;		//The maximum time a frame can take.Physics and other fixed frame rate updates(like MonoBehaviour's FixedUpdate) will be performed only for this duration of time per frame.
		nfloat maximumParticleDeltaTime = 1.0 / 3.0;//The maximum time a frame can spend on particle updates.If the frame takes longer than this, then updates are split into multiple smaller updates.
		bool inFixedTimeStep = true;		//Returns true if called inside a fixed time step callback(like MonoBehaviour's FixedUpdate), otherwise returns false.
	public:
		cpg_time()
		{
		}

		~cpg_time()
		{
		}

	private:

	};

	class time_u
	{
	public:
		std::chrono::time_point<std::chrono::high_resolution_clock> _begin;
		int fix_time = 1000;
	public:
		time_u()
		{
			reset();
		}

		~time_u()
		{
		}
		void reset() {
			_begin = std::chrono::high_resolution_clock::now();
		}

		int64_t elapsed()
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - _begin).count();
		}
		void set_fix(double t)
		{
			fix_time = t * 1000;
		}
		bool get_fix()
		{
			bool ret = false;
			if (elapsed() > fix_time)
			{
				ret = true; reset();
			}
			return ret;
		}
	private:

	};
	class game_time
	{
	private:
		int tj = 2;
		int tjs = 0;
		double g_LastTime = 0.0;
		double g_CurrTime = 0.0;

		int   _FrameCounter = 0;
		float _LastFrameTime = 0.0f;
		float _LastFPS = 0.0f;


		double avgDuration = 0.f;
		double alpha = 1.f / 100.f; // 采样数设置为100
		int frameCount = 0;
		float _LastFPS1 = 0.0f;
		// 线程id
		uint64_t _tid = 0;
		// 游戏时间
		cpg_time _gtime, _temgt;
		// 最近8帧的时间
		int maxdeltas = 8;
		std::vector<double> deltas;
		time_u ctu;
		bool   _pause = false;				// 暂停游戏
	public:
		game_time();

		~game_time();
		void init();
		void update();
		virtual void UpdateFPS(double time, double delta);
		void uptime(double time, double delta);
		int fpsup(double deltaTime);
		float get_fps();
		float get_fps1();
		bool nc_tid(uint64_t t);
		void set_pause();
		void set_play();
		cpg_time* get_gt();
	private:

	};

	class queue_ctx
	{
	private:
		dvk_device* _dev = 0;
		dvk_queue* _qctx = 0;

		submit_infos sub;
		present_info present;
		game_time gt;
		size_t _subsize = 0;
		std::thread _rthr;
		std::vector<sdl_form*> _vforms, subform;
		a_queue<sdl_form*> temqueue;
	public:
		std::function<void(cpg_time*)> update_cb1;
		std::function<void()> init_cb;
		game_time* pgt = 0;
		int _waitms = 8;
		int _minfps_ms = 8;
		int _minfps = 160;
		int _gfps = 0;
		bool isrun = true;
		bool isbegin = false;
		bool is_clear = false;
		bool is_outofdate = false;
		bool isinit = true;
		bool _end = true;
		bool _onethread = false;
	public:
		friend class gui_ctx;
	public:
		queue_ctx(bool onethread);
		~queue_ctx();
		void stop();
		void begin_thr();
		bool* get_rts();
		game_time* get_pgt();
	public:
		void init(dvk_device* dev, size_t qidx);
		void push(sdl_form* form1);
		void set_waitms(int ms, int minfps);
		void loop();
	private:
		void FixedUpdate(cpg_time* t);
		void LateUpdate(cpg_time* t);
		void game_logic(cpg_time* t);
		void game_update();

		int frame_it(sdl_form* it, fbo_cx* fbo1, glm::ivec2 fws, uint32_t* image_idx);
		bool frame_begin();
		void frame_end();

	};

	//VkImageLayout 
	uint32_t GetImageLayout(ImageLayoutBarrier target);
	//enum VkFormat;
	uint32_t GetVkFormat(int type, int id);
	uint32_t GetVkFormat(const std::string& str, int id);
	uint32_t SizeOfFormat(uint32_t format);


}
// !hz
