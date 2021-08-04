#pragma once
#include <thread>

#define OUT_FILE_LINE_ printf("当行：%d\t<%s>\n", __LINE__, __FILE__)
namespace vkm {
	enum class IBO_type {
		t_ushort = 0,
		t_uint32 = 1
	};
}
struct VkPhysicalDeviceProperties;
namespace hz {
	class stbimage_cx;
	class sdl_form;
	class Image;


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
		void init(VkInstance ip);
		//int get_push_constant_size(size_t idx);
		//std::string get_name(size_t idx);
		//std::vector<VkPhysicalDevice> get_physicals();

		std::string get_idx(const char* name, int& idx);
	public:

	private:

		int enum_gpus();

	};
	// todo dvk_texture
	class dvk_texture
	{
	public:
		VkImage _image = 0;
		VkImageView view = 0;
		VkSampler sampler = 0;
		VkDeviceMemory deviceMemory = 0;
		int64_t cap_device_mem_size = 0;
		int cap_inc = 0, caps = 8;			// 分配8次就重新释放显存
		VkDescriptorImageInfo* descriptor = {};

		uint32_t width = 0, height = 0;
		uint32_t mipLevels = 1;
		uint32_t layerCount = 1;
		//VkDescriptorType dtype = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		//VkFormat _format = VK_FORMAT_R8G8B8A8_UNORM;
		uint32_t _format = 37;
		int64_t _alloc_size = 0;
		dvk_device* _dev = nullptr;
		ImageLayoutBarrier _image_layout = ImageLayoutBarrier::SHADER_READ;
		void* user_data = 0;
	public:
		dvk_texture();
		dvk_texture(dvk_device* dev);

		~dvk_texture();

	public:

		static dvk_texture* new2d(const std::string& filename, dvk_device* dev, ImageLayoutBarrier imageLayout = ImageLayoutBarrier::SHADER_READ);
		static dvk_texture* new_image2d(Image* img, dvk_device* dev, ImageLayoutBarrier imageLayout = ImageLayoutBarrier::SHADER_READ);
		static dvk_texture* new_storage2d(const std::string& filename, dvk_device* dev, bool is_compute = false);
		static dvk_texture* new_storage2d(dvk_device* dev, int width, int height, uint32_t format = 0, bool is_compute = false);
		static dvk_texture* new_render_target_color(dvk_device* dev, int width, int height, uint32_t format = 0);
		static dvk_texture* new_render_target_depth(dvk_device* dev, int width, int height, uint32_t format = 0);

		Image* save2Image(Image* outimage);
		VkDescriptorImageInfo* get_descriptor_image_info();
	private:
		void free_image();
		static dvk_texture* new2d_priv(const std::string& filename
									   , dvk_device* dev
									   , ImageLayoutBarrier imageLayout /*= ImageLayoutBarrier::SHADER_READ*/
									   //VkImageUsageFlags
									   , uint32_t imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT*/
		);
		void set_data(
			void* buffer,
			VkDeviceSize bufferSize,
			uint32_t format,
			uint32_t w,
			uint32_t h,
			dvk_device* dev,
			uint32_t imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/,
			//VkImageLayout
			uint32_t imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/);
		void mkrt_image(int w, int h, uint32_t format, bool is_depth, uint32_t usage, ImageLayoutBarrier image_layout);
		void new_storage(int width, int height, uint32_t format, bool is_compute);


		void get_buffer(char* outbuf);
	public:
		void set_data(Image* img);

	};



	// todo dvk_cmd
	class dvk_cmd
	{
	public:
		VkCommandBuffer cmdb = 0;
		glm::vec2 _viewport;
	private:
		dvk_device* _dev = 0;
		bool is_begin = false;
		float minDepth = 0.0;
		float maxDepth = 1.0;
		shader_info* _pipeline = 0;
		bool _is_compute = false;
		uint32_t bindPoint = 0;
	public:
		dvk_cmd(dvk_device* dev, VkCommandBuffer cb, bool is_compute);

		~dvk_cmd();
		VkCommandBuffer ptr() { return cmdb; }
		//operator VkCommandBuffer() { return cmdb; }
	public:
		void begin();
		void end();
		void dispatch(int groupX, int groupY, int groupZ);
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
		void set_state(uint32_t cull_mod, uint32_t front_face, uint32_t primitive_topology, int depth_test_enable, int depth_write_enable);
		// bind
		void bind_pipeline(shader_info* pipeline, bool depthWriteEnable);
		//void bind_set(dvk_set* dsp, uint32_t first_set = 0);
		void bind_set(const VkDescriptorSet* sets, uint32_t count, uint32_t first_set, std::vector<uint32_t>* offsets = 0) const;
		void bind_set(const VkDescriptorSet* sets, uint32_t count, uint32_t first_set, uint32_t offsets) const;
		void bind_set(VkDescriptorSet sets, uint32_t first_set, uint32_t offsets) const;
		void bind_set(ubo_set_t* ubo) const;
		void bind_set(ubo_set_t ubo) const;
		// index_type 0=u16	1=u32
		void bind_ibo(VkBuffer buffer, VkDeviceSize offset, uint8_t index_type);
		void bind_ibo(VkBuffer buffer, VkDeviceSize offset, vkm::IBO_type index_type);
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

		void ExecuteCommands(const VkCommandBuffer* pCommandBuffers, uint32_t count);
	public:

	};

	// todo submit_info
	class submit_info
	{
	public:
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
		present_info(bool is_rts = false);
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
		submits* sv = 0;
		std::map<submit_info*, size_t> psim;
	public:
		submit_infos();

		~submit_infos();

		// std::vector<VkSubmitInfo>
		size_t insert_sub(submit_info* psi);
		void erase(submit_info* psi);
		void update_sub(submit_info* info);
		void clear();
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
	class cpg_time
	{
	public:
		// 只读
		double time = 0.0;					//*The time at the beginning of this frame(Read Only).This is the time in seconds since the start of the game.
		double deltaTime = 0.0;				//*The completion time in seconds since the last frame. (Read Only).
		double fixedTime = 0.0;				//The time the latest FixedUpdate has started(Read Only).This is the time in seconds since the start of the game.
		double fixedUnscaledDeltaTime = 0.0;//The timeScale - independent interval in seconds from the last fixed frame to the current one(Read Only).
		double fixedUnscaledTime = 0.0;		//The TimeScale - independant time the latest FixedUpdate has started(Read Only).This is the time in seconds since the start of the game.
		double frameCount = 0.0;			//*The total number of frames that have passed(Read Only).
		double realtimeSinceStartup = 0.0;	//*The real time in seconds since the game started(Read Only).
		double smoothDeltaTime = 0.0;		//*A smoothed out Time.deltaTime(Read Only).
		double timeSinceLevelLoad = 0.0;	//*The time this frame has started(Read Only).This is the time in seconds since the last level has been loaded.
		double unscaledDeltaTime = 0.0;		//*The timeScale - independent interval in seconds from the last frame to the current one(Read Only).
		double unscaledTime = 0.0;			//*The timeScale - independant time for this frame(Read Only).This is the time in seconds since the start of the game.
		float fps = 0.0f;
		// 可写
		double captureFramerate = 0.0;		//Slows game playback time to allow screenshots to be saved between frames.
		double timeScale = 1.0;				//The scale at which the time is passing.This can be used for slow motion effects.
		double fixedDeltaTime = 0.02;		//The interval in seconds at which physicsand other fixed frame rate updates(like MonoBehaviour's FixedUpdate) are performed.
		bool inFixedTimeStep = true;		//Returns true if called inside a fixed time step callback(like MonoBehaviour's FixedUpdate), otherwise returns false.
		double maximumDeltaTime = 1.0 / 3.0;		//The maximum time a frame can take.Physics and other fixed frame rate updates(like MonoBehaviour's FixedUpdate) will be performed only for this duration of time per frame.
		double maximumParticleDeltaTime = 1.0 / 3.0;//The maximum time a frame can spend on particle updates.If the frame takes longer than this, then updates are split into multiple smaller updates.

	public:
		cpg_time()
		{
		}

		~cpg_time()
		{
		}

	private:

	};

	class game_time
	{
	private:

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
		bool isrun = true;
		bool isbegin = false;
		bool is_clear = false;
		std::function<void(cpg_time*)> update_cb;
		bool is_outofdate = false;
		int _waitms = 8;
	public:
		friend class gui_ctx;
	public:
		queue_ctx(const char* title);
		~queue_ctx();
		void stop();
		void begin_thr();
		bool* get_rts();
		game_time* get_pgt();
	public:
		void init(dvk_device* dev, size_t qidx);
		void push(sdl_form* form1);
		void set_waitms(int ms);
	private:
		void FixedUpdate(cpg_time* t);
		void LateUpdate(cpg_time* t);
		void game_logic(cpg_time* t);
		void game_update();
		bool frame_begin();
		void frame_end();

	};

}
// !hz
