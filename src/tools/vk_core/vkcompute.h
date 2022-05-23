/*
	主要实现：dvk_compute
	参考实用样例void comp_test()
*/
#define _IN_VKC_CX

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
#ifdef _IN_VKC_CX
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
	VkCommandBufferBeginInfo cmdBufInfo = { };
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
	blit.srcOffsets[0] = { 0, 0, 0 };
	blit.srcOffsets[1] = { (int)src->width, (int)src->height, 1 };
	blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blit.srcSubresource.mipLevel = src->mipLevels;
	blit.srcSubresource.baseArrayLayer = 0;
	blit.srcSubresource.layerCount = 1;
	blit.dstOffsets[0] = { 0, 0, 0 };
	blit.dstOffsets[1] = { (int)dst->width,(int)dst->height, 1 };
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
	VkDeviceSize pOffsets[] = { offset };
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
void comp_test()
{
	// 混合
	auto blend_comp = sh->new_compute(dev, { "blend.comp" });
	auto texfn = opentexfn();
	if (texfn.empty())
	{
		return;
	}
	auto srcimg = Image::create(texfn);
	srcimg->clear_zero_a();
	srcimg->saveImage("temp/a" + std::to_string(1) + ".png");
	Image* dstimg = Image::create_null(srcimg->width, srcimg->height);
	sigma = 10;
	{
		//print_time pt("cpu_pass");
		//GaussianFilter(srcimg, dstimg, sigma);
	}
	//dstimg->saveImage("temp/cpu_pass" + std::to_string(0) + ".png");
	sigma = 6;
	{
		print_time pt("cpu_pass");
		GaussianFilter(srcimg, dstimg, sigma);
	}
	dstimg->saveImage("temp/cpu_pass" + std::to_string(1) + ".png");
	auto tex0 = dvk_texture::new_storage2d(texfn, dev, true);
	//auto tex0 = dvk_texture::new_storage2d("rio-2-blu-icon-5.png", dev, 0, true);
	if (!tex0)
	{
		return;
	}
	dvk_texture* compute_targets[6] = {};
	// 创建3个输出image
	uint32_t format = 0;// 91;// VK_FORMAT_R16G16B16A16_UNORM;
	for (size_t i = 0; i < 5; i++)
	{
		compute_targets[i] = dvk_texture::new_storage2d(dev, tex0->width, tex0->height, format, true);
	}
	auto st = dvk_texture::new_storage2d(dev, tex0->width, tex0->height, true);
	auto gc1_ubo1 = dvk_buffer::new_ubo(dev, false, 32 * 1024 * 1024 - 2);
	auto gc1_ubo = dvk_buffer::new_ubo(dev, false, 32 * 1024 * 1024);
	// 创建compute着色器实现高斯模糊
	ut_compute* gauss_comp = sh->new_compute(dev, { "gauss_blur.comp" });
	ut_compute* gauss_comp1 = sh->new_compute(dev, { "gauss_blur1.comp" });
	auto set1 = gauss_comp->new_set();			// pass 1 用
	auto set2 = gauss_comp->new_set();			// pass 2 用
	auto set_g1 = gauss_comp1->new_set();		// pass 1 用
	auto set_g2 = gauss_comp1->new_set();		// pass 2 用
	auto set_blend = blend_comp->new_set();
	set1->write_image("inputImage", tex0);
	set1->write_image("outputImage", compute_targets[0]);
	set2->write_image("inputImage", compute_targets[0]);
	set2->write_image("outputImage", compute_targets[1]);
	set_g1->write_image("inputImage", tex0);
	set_g1->write_image("outputImage", compute_targets[3]);
	set_g2->write_image("inputImage", compute_targets[3]);
	set_g2->write_image("outputImage", compute_targets[4]);
	set_blend->write_image("srcSampler", tex0);
	set_blend->write_image("tempSampler", compute_targets[1]);
	set_blend->write_image("outputImage", compute_targets[2]);

	struct gauss_info {
		glm::vec2  dir;			// 水平过滤 { 1.0 / 256.0, 0 }，垂直过滤 { 0, 1.0 / 256.0 }，
		float glowFactor = 1.0;	// 颜色强度，水平过滤时glowFactor=1.0，垂直过滤时设置4.5之类
		float wt_normalize = (1.0 / (1.0 + 2.0 * (0.93 + 0.8 + 0.7 + 0.6 + 0.5 + 0.4 + 0.3 + 0.2 + 0.1)));
		float gauss[9] = { 0.93, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1 };
	}upc_gauss;
	const int NW = 5;
	struct gauss_info1 {
		glm::vec2  dir;			// H=x, V=y
		float blurScale = 1.5;
		float blurStrength = 1;
		float weight[NW] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };
	}upc_gauss1;
	struct blend_info {
		glm::vec4  glow_color = { 1.0,0.0,0.0,3.6 };
		float outerGlow = 1.0;
		float innerGlow = 0.0;
	}upc_blend;
	double wtn = 0.0;
	printf("\n");
	glm::ivec3 gs = { tex0->width / 16, tex0->height / 16, 1 };
	cmd1->begin();
	{
		print_time pts("cmd");
		gauss_comp->bind(cmd1, set1);
		//upc.dir = { 1.0 / tex0->_width, 0 };
		upc_gauss.dir = { 1.0 , 0 };
		gauss_comp->set_push_constant(cmd1, &upc_gauss, sizeof(gauss_info), 0);
		cmd1->dispatch(gs.x, gs.y, gs.z);
		// 同步
		cmd1->barrier_image(compute_targets[0], 1, 1, hz::ImageLayoutBarrier::ComputeGeneralRW, hz::ImageLayoutBarrier::ComputeGeneralRW, false);
		gauss_comp->bind(cmd1, set2);
		//upc.dir = { 0, 1.0 / tex0->_height };
		upc_gauss.dir = { 0, 1.0 };
		upc_gauss.glowFactor = 1.2;
		gauss_comp->set_push_constant(cmd1, &upc_gauss, sizeof(gauss_info), 0);
		cmd1->dispatch(gs.x, gs.y, gs.z);
		// cmd1->barrier_mem_compute(true);//无关系不用同步
		upc_gauss1.dir = { 1.0,0 };
		gauss_comp1->bind(cmd1, set_g1);
		gauss_comp1->set_push_constant(cmd1, &upc_gauss1, sizeof(gauss_info1), 0);
		cmd1->dispatch(gs.x, gs.y, gs.z);
		cmd1->barrier_image(compute_targets[3], 1, 1, hz::ImageLayoutBarrier::ComputeGeneralRW, hz::ImageLayoutBarrier::ComputeGeneralRW, false);
		//cmd1->barrier_mem_compute(true);
		upc_gauss1.dir = { 0, 1.0 };
		gauss_comp1->bind(cmd1, set_g2);
		gauss_comp1->set_push_constant(cmd1, &upc_gauss1, sizeof(gauss_info1), 0);
		cmd1->dispatch(gs.x, gs.y, gs.z);

		//cmd1->barrier_image(compute_targets[4], 1, 1, ImageLayoutBarrier::ComputeGeneralRW, ImageLayoutBarrier::ComputeGeneralRW, false);
		//cmd1->blit_image(st, compute_targets[4]);
		//// 混合成光辉
		//blend_comp->bind(cmd1, set_blend);
		//blend_comp->set_push_constant(cmd1, &upc_blend, sizeof(blend_info), 0);
		//cmd1->dispatch(gs.x, gs.y, gs.z);
	}
	cmd1->end();
	std::vector< Image*> ct;
	{
		print_time pts("submit");
		qctx->submit(subptr, &subfence, true);
	}
	{
		print_time pts("copy2mem");
		ct.push_back(compute_targets[4]->save2Image(Image::create()));
	}
	ct.push_back(tex0->save2Image(Image::create()));
	ct.push_back(compute_targets[3]->save2Image(Image::create()));
	ct.push_back(st->save2Image(Image::create()));

	for (size_t i = 0; i < ct.size(); i++)
	{
		ct[i]->saveImage("temp/compute_pass" + std::to_string(i) + ".png");
	}
	printf("done\n");
}
#endif // _IN_VKC_CX