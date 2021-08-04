#pragma once

typedef struct pipeline_state_obj_s {
	// 填充方式VK_POLYGON_MODE_FILL = 0, VK_POLYGON_MODE_LINE = 1, VK_POLYGON_MODE_POINT
	uint32_t polygon_mode = 0;
	// 剔除模式 VK_CULL_MODE_NONE = 0, VK_CULL_MODE_FRONT_BIT = 1, VK_CULL_MODE_BACK_BIT = 2, VK_CULL_MODE_FRONT_AND_BACK = 3,
	uint8_t cull_mode = 0;
	//表示顺时针方向为正面（VK_FRONT_FACE_CLOCKWISE=1）和逆时针方向为正面（VK_FRONT_FACE_COUNTER_CLOCKWISE=0）
	uint8_t front_face = 0;
	// 混合颜色RGBA vec4
	float blendConstants[4] = {};
	// 自定义混合
	struct blend_info
	{
		uint32_t blendEnable;
		uint32_t srcColorBlendFactor;// (VkBlendFactor)
		uint32_t dstColorBlendFactor;//(VkBlendFactor)
		uint32_t colorBlendOp;//(VkBlendOp)
		uint32_t srcAlphaBlendFactor;//(VkBlendFactor)
		uint32_t dstAlphaBlendFactor;//(VkBlendFactor)
		uint32_t alphaBlendOp;//(VkBlendOp)
	} blend = {};
	// _blend_type：默认透明混合=0，不混合=1，自定义混合=2(同时blend成员自己设置)
	int _blend_type = 0;
	// 深度测试
	bool depthTestEnable = true;
	// 深度可读写，如果可写则_pipe[1]为不可写时的管线
	//bool depthWriteEnable = true;
}pipeline_state_obj_t;
