
#include <stdint.h>
#ifndef __hlContext__H_
#define __hlContext__H_

#if 1

/*
SINT整数
UINT无符号整数
R8_UINT=1字节、R8G8_UINT=2字节
R16_UINT=2字节、R16G16_UINT=4字节、
R32_UINT=4字节、
*/
enum FORMAT
{
	HZ_FORMAT_NULL = 0,
	HZ_FORMAT_R8_UNORM,
	HZ_FORMAT_R8_SNORM,
	HZ_FORMAT_R8_USCALED,
	HZ_FORMAT_R8_SSCALED,
	HZ_FORMAT_R8_UINT,
	HZ_FORMAT_R8_SINT,
	HZ_FORMAT_R8_SRGB,
	HZ_FORMAT_R8G8_UNORM,
	HZ_FORMAT_R8G8_SNORM,
	HZ_FORMAT_R8G8_USCALED,
	HZ_FORMAT_R8G8_SSCALED,
	HZ_FORMAT_R8G8_UINT,
	HZ_FORMAT_R8G8_SINT,
	HZ_FORMAT_R8G8_SRGB,
	HZ_FORMAT_R8G8B8_UNORM,
	HZ_FORMAT_R8G8B8_SNORM,
	HZ_FORMAT_R8G8B8_USCALED,
	HZ_FORMAT_R8G8B8_SSCALED,
	HZ_FORMAT_R8G8B8_UINT,
	HZ_FORMAT_R8G8B8_SINT,
	HZ_FORMAT_R8G8B8_SRGB,
	HZ_FORMAT_B8G8R8_UNORM,
	HZ_FORMAT_B8G8R8_SNORM,
	HZ_FORMAT_B8G8R8_USCALED,
	HZ_FORMAT_B8G8R8_SSCALED,
	HZ_FORMAT_B8G8R8_UINT,
	HZ_FORMAT_B8G8R8_SINT,
	HZ_FORMAT_B8G8R8_SRGB,
	HZ_FORMAT_R8G8B8A8_UNORM,
	HZ_FORMAT_R8G8B8A8_SNORM,
	HZ_FORMAT_R8G8B8A8_USCALED,
	HZ_FORMAT_R8G8B8A8_SSCALED,
	HZ_FORMAT_R8G8B8A8_UINT,
	HZ_FORMAT_R8G8B8A8_SINT,
	HZ_FORMAT_R8G8B8A8_SRGB,
	HZ_FORMAT_B8G8R8A8_UNORM,
	HZ_FORMAT_B8G8R8A8_SNORM,
	HZ_FORMAT_B8G8R8A8_USCALED,
	HZ_FORMAT_B8G8R8A8_SSCALED,
	HZ_FORMAT_B8G8R8A8_UINT,
	HZ_FORMAT_B8G8R8A8_SINT,
	HZ_FORMAT_B8G8R8A8_SRGB,
	HZ_FORMAT_A8B8G8R8_UNORM_PACK32,
	HZ_FORMAT_A8B8G8R8_SNORM_PACK32,
	HZ_FORMAT_A8B8G8R8_USCALED_PACK32,
	HZ_FORMAT_A8B8G8R8_SSCALED_PACK32,
	HZ_FORMAT_A8B8G8R8_UINT_PACK32,
	HZ_FORMAT_A8B8G8R8_SINT_PACK32,
	HZ_FORMAT_A8B8G8R8_SRGB_PACK32,
	HZ_FORMAT_A2R10G10B10_UNORM_PACK32,
	HZ_FORMAT_A2R10G10B10_SNORM_PACK32,
	HZ_FORMAT_A2R10G10B10_USCALED_PACK32,
	HZ_FORMAT_A2R10G10B10_SSCALED_PACK32,
	HZ_FORMAT_A2R10G10B10_UINT_PACK32,
	HZ_FORMAT_A2R10G10B10_SINT_PACK32,
	HZ_FORMAT_A2B10G10R10_UNORM_PACK32,
	HZ_FORMAT_A2B10G10R10_SNORM_PACK32,
	HZ_FORMAT_A2B10G10R10_USCALED_PACK32,
	HZ_FORMAT_A2B10G10R10_SSCALED_PACK32,
	HZ_FORMAT_A2B10G10R10_UINT_PACK32,
	HZ_FORMAT_A2B10G10R10_SINT_PACK32,
	HZ_FORMAT_R16_UNORM,
	HZ_FORMAT_R16_SNORM,
	HZ_FORMAT_R16_USCALED,
	HZ_FORMAT_R16_SSCALED,
	HZ_FORMAT_R16_UINT,
	HZ_FORMAT_R16_SINT,
	HZ_FORMAT_R16_SFLOAT,
	HZ_FORMAT_R16G16_UNORM,
	HZ_FORMAT_R16G16_SNORM,
	HZ_FORMAT_R16G16_USCALED,
	HZ_FORMAT_R16G16_SSCALED,
	HZ_FORMAT_R16G16_UINT,
	HZ_FORMAT_R16G16_SINT,
	HZ_FORMAT_R16G16_SFLOAT,
	HZ_FORMAT_R16G16B16_UNORM,
	HZ_FORMAT_R16G16B16_SNORM,
	HZ_FORMAT_R16G16B16_USCALED,
	HZ_FORMAT_R16G16B16_SSCALED,
	HZ_FORMAT_R16G16B16_UINT,
	HZ_FORMAT_R16G16B16_SINT,
	HZ_FORMAT_R16G16B16_SFLOAT,
	HZ_FORMAT_R16G16B16A16_UNORM,
	HZ_FORMAT_R16G16B16A16_SNORM,
	HZ_FORMAT_R16G16B16A16_USCALED,
	HZ_FORMAT_R16G16B16A16_SSCALED,
	HZ_FORMAT_R16G16B16A16_UINT,
	HZ_FORMAT_R16G16B16A16_SINT,
	HZ_FORMAT_R16G16B16A16_SFLOAT,
	HZ_FORMAT_R32_UINT,
	HZ_FORMAT_R32_SINT,
	HZ_FORMAT_R32_SFLOAT,
	HZ_FORMAT_R32G32_UINT,
	HZ_FORMAT_R32G32_SINT,
	HZ_FORMAT_R32G32_SFLOAT,
	HZ_FORMAT_R32G32B32_UINT,
	HZ_FORMAT_R32G32B32_SINT,
	HZ_FORMAT_R32G32B32_SFLOAT,
	HZ_FORMAT_R32G32B32A32_UINT,
	HZ_FORMAT_R32G32B32A32_SINT,
	HZ_FORMAT_R32G32B32A32_SFLOAT,
	HZ_FORMAT_R64_UINT,
	HZ_FORMAT_R64_SINT,
	HZ_FORMAT_R64_SFLOAT,
	HZ_FORMAT_R64G64_UINT,
	HZ_FORMAT_R64G64_SINT,
	HZ_FORMAT_R64G64_SFLOAT,
	HZ_FORMAT_R64G64B64_UINT,
	HZ_FORMAT_R64G64B64_SINT,
	HZ_FORMAT_R64G64B64_SFLOAT,
	HZ_FORMAT_R64G64B64A64_UINT,
	HZ_FORMAT_R64G64B64A64_SINT,
	HZ_FORMAT_R64G64B64A64_SFLOAT,
};
typedef enum ShaderStageFlagBits {
	E_SHADER_STAGE_VERTEX_BIT = 0x00000001,
	E_SHADER_STAGE_TESSELLATION_CONTROL_BIT = 0x00000002,
	E_SHADER_STAGE_TESSELLATION_EVALUATION_BIT = 0x00000004,
	E_SHADER_STAGE_GEOMETRY_BIT = 0x00000008,
	E_SHADER_STAGE_FRAGMENT_BIT = 0x00000010,
	E_SHADER_STAGE_COMPUTE_BIT = 0x00000020,
	E_SHADER_STAGE_ALL_GRAPHICS = 0x0000001F,
	E_SHADER_STAGE_ALL = 0x7FFFFFFF,
	E_SHADER_STAGE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} ShaderStageFlagBits;
typedef enum DynamicStateBits {
	E_DYNAMIC_STATE_VIEWPORT = 0x00000001,
	E_DYNAMIC_STATE_SCISSOR = 0x00000002,
	E_DYNAMIC_STATE_LINE_WIDTH = 0x00000004,
	E_DYNAMIC_STATE_DEPTH_BIAS = 0x00000008,
	E_DYNAMIC_STATE_BLEND_CONSTANTS = 0x00000010,
	E_DYNAMIC_STATE_DEPTH_BOUNDS = 0x00000020,
	E_DYNAMIC_STATE_STENCIL_COMPARE_MASK = 0x00000040,
	E_DYNAMIC_STATE_STENCIL_WRITE_MASK = 0x00000080,
	E_DYNAMIC_STATE_STENCIL_REFERENCE = 0x00000100
}DynamicStateBits;

//顶点原始拓扑topology
typedef enum ShaderTopologyFlag {
	E_POINT_LIST = 1,
	E_LINE_LIST,
	E_TRIANGLE_LIST,
	E_TRIANGLE_STRIP,
	E_TRIANGLE_FAN,
	E_LINE_STRIP,
	E_LINE_LIST_WITH_ADJACENCY,
	E_LINE_STRIP_WITH_ADJACENCY,
	E_TRIANGLE_LIST_WITH_ADJACENCY,
	E_TRIANGLE_STRIP_WITH_ADJACENCY,
	E_PATCH_LIST,
}ShaderTopologyFlag;
typedef enum IndexType {
	E_INDEX_TYPE_UINT16 = 0,
	E_INDEX_TYPE_UINT32 = 1
}IndexType;
typedef enum VertexType {
	D_POSITION = 0,
	D_TEXCOORD,
	D_COLOR,
	D_TANGENT,
	D_NORMAL,
	D_TEXCOORD0,
	D_TEXCOORD1,
	D_TEXCOORD2,
	D_TEXCOORD3,
	D_TEXCOORD4,
	D_TEXCOORD5,
	D_TEXCOORD6,
	D_TEXCOORD7,
	D_COLOR0,
	D_COLOR1,
	D_COLOR2,
	D_COLOR3,
	D_COLOR4,
	D_COLOR5,
	D_COLOR6,
	D_COLOR7,

}VertexType;
//---------------------------------------------------------------------------------------------
enum ShapeInfo
{
	//球
	S_Sphere,
	//立方体
	S_Cube,
	//圆柱体
	S_Cylinder,
	//圆锥体
	S_Cone,
	//面
	S_Plane,
	//圆环
	S_Torus,
	//金字塔
	S_Pyramid,
	//管
	S_Pipe,
};
//---------------------------------------------------------------------------------------------


class v2 {
public:
	union {
		struct { int width; int height; };
		struct { int x; int y; };
	};
};
typedef struct d32_8
{
	union {
		struct {
			uint8_t shaderstage;
			uint8_t type;
			uint8_t count;
			uint8_t binding;
		};
		uint32_t u32 = 0;
	};
}d32_8;
//--------------------------------------------------------------------------------------------------------------------------------

//typedef struct BindDescriptorSetInfo
//{
//	int type = E_BindDescriptorSet;

//	//需要绑定的纹理、UBO、数量，需要跟着色器设置一样
//	void *image = nullptr;
//	int   image_size = 0;
//	void *uniform = nullptr;
//	int   uniform_size = 0;

//	void *pipeline = nullptr;

//	//返回绑定的描述符
//	void *descriptorSet = nullptr;
//}BindDescriptorSetInfo;
//--------------------------------------------------------------------------------------------------------------------------------
//多边形结构
//球
typedef struct Info_Sphere
{
	int type = S_Sphere;
	float radius = 1.0;					//半径0.01-100
	int axial_subdivision = 20;			//轴向细分数3-50
	int height_subdivision = 20;		//高度细分数3-50
	float axis[3] = { 0.0,1.0,0.0 };	//轴，旋转
}Info_Sphere;
//立方体
typedef struct Info_Cube
{
	int type = S_Cube;
	float width = 1.0;					//宽度0.01-100
	float height = 1.0;					//宽度0.01-100
	float depth = 1.0;					//宽度0.01-100
	int width_subdivision = 1;			//轴向细分数1-50
	int height_subdivision = 1;			//轴向细分数1-50
	int depth_subdivision = 1;			//高度细分数1-50
	float axis[3] = { 0.0,1.0,0.0 };	//轴，旋转
}Info_Cube;
//圆柱体
typedef struct Info_Cylinder
{
	int type = S_Cylinder;
	float radius = 1.0;					//半径0.01-100
	float height = 2.0;					//高度0.01-100
	int axial_subdivision = 20;			//轴向细分数3-50
	int height_subdivision = 1;			//高度细分数1-50
	int endface_subdivision = 1;		//端面细分数0-50
	bool round_face = false;			//圆形端面
	float axis[3] = { 0.0,1.0,0.0 };	//轴，旋转

}Info_Cylinder;
//圆锥体
typedef struct Info_Cone
{
	int type = S_Cone;
	float radius = 1.0;					//半径0.01-100
	float height = 2.0;					//高度0.01-100
	int axial_subdivision = 20;			//轴向细分数3-50
	int height_subdivision = 20;		//高度细分数3-50
	int endface_subdivision = 1;		//端面细分数0-50
	bool round_face = false;			//圆形端面，最好设置端面细分大点
	float axis[3] = { 0.0,1.0,0.0 };	//轴，旋转

}Info_Cone;
//面
typedef struct Info_Plane
{
	int type = S_Plane;
	float width = 1.0;					//宽度0.01-100
	float height = 1.0;					//宽度0.01-100
	int width_subdivision = 10;			//轴向细分数1-50
	int height_subdivision = 10;		//轴向细分数1-50
	float axis[3] = { 0.0,1.0,0.0 };	//轴，旋转
}Info_Plane;
//圆环
typedef struct Info_Torus
{
	int type = S_Torus;
	float radius = 1.0;					//半径0.01-100
	float sectionradius = 0.500;		//截面半径0.01-100
	float distortion = 0.000;			//扭曲0-360度，高度细分旋转量
	int axial_subdivision = 20;			//轴向细分数3-50
	int height_subdivision = 20;		//高度细分数3-50
	float axis[3] = { 0.0,1.0,0.0 };	//轴，旋转
}Info_Torus;
//金字塔
typedef struct Info_Pyramid
{
	int type = S_Pyramid;
	float side_length = 1.0;			//侧面长度0.01-100
	int side_num = 5;					//边数3\4\5
	int height_subdivision = 20;		//高度细分数1-50
	int endface_subdivision = 0;		//端面细分数0-50
	float axis[3] = { 0.0,1.0,0.0 };	//轴，旋转
}Info_Pyramid;
//管
typedef struct Info_Pipe
{
	int type = S_Pipe;
	float radius = 1.0;					//半径0.01-100
	float height = 2.0;					//高度0.01-100
	float thickness = 0.500;			//厚度0.01-100
	int axial_subdivision = 20;			//轴向细分数3-50
	int height_subdivision = 1;			//高度细分数1-50
	int endface_subdivision = 0;		//端面细分数0-50
	bool round_face = false;			//圆形端面，最好设置端面细分大点
	float axis[3] = { 0.0,1.0,0.0 };	//轴，旋转

}Info_Pipe;

enum GuiMouseCursor_
{
	GuiMouseCursor_None = -1,
	GuiMouseCursor_Arrow = 0,
	GuiMouseCursor_TextInput,         // When hovering over InputText, etc.
	GuiMouseCursor_ResizeAll,         // Unused
	GuiMouseCursor_ResizeNS,          // When hovering over an horizontal border
	GuiMouseCursor_ResizeEW,          // When hovering over a vertical border or a column
	GuiMouseCursor_ResizeNESW,        // When hovering over the bottom-left corner of a window
	GuiMouseCursor_ResizeNWSE,        // When hovering over the bottom-right corner of a window
	GuiMouseCursor_Count_
};

enum RenderTargetType
{
	RenderTargetTypeVulkan,
	RenderTargetTypeOpenGL,
	RenderTargetTypeDX,
	RenderTargetTypeGDI,

	RenderTargetTypeTotal
};

enum class dvkObjType :uint32_t
{
	E_Element = 1,
	E_Buffer,
	E_VBO,
	E_IBO,
	E_UBO,
	E_FBO,
	E_Texture,
	E_Pipeline,
	E_BindDescriptorSet,
	E_RCIMAX
};
typedef struct ElementInfo
{
public:
	int type = (int)dvkObjType::E_Element;

	void* vbo = nullptr;
	void* ibo = nullptr;

	//UBO数组指针、数量
	void* ubo = nullptr;
	int* ubo_binding = 0;
	int   ubo_count = 0;
	//纹理数组指针、数量
	void* texture = nullptr;
	int* tex_binding = 0;
	int   texture_count = 0;

	float lineWidth = 1.0f;				//线宽
	int	  scissor[4] = { 0,0,-1,-1 };	//裁剪区域，默认-1大小为全窗口
	//在哪个FBO渲染
	void* fbo = 0;
	bool  isdrawQueue = true;			//是否加入到渲染列队
	//shader管线
	void* pipeline = nullptr;

	void* element = nullptr;
}ElementInfo;

typedef struct BufferInfo
{
public:
	int type = (int)dvkObjType::E_Buffer;
	//大小（字节）
	size_t size = 0;
	//计数
	int count = 0;
	//数据
	void* data = nullptr;
	int ibotype = E_INDEX_TYPE_UINT16;

	//内部对象指针
	void* bo = nullptr;

}BufferInfo;

typedef struct TextureInfo
{
	int type = (int)dvkObjType::E_Texture;
	int width = 0;
	int height = 0;
	int imageType = 1;// {VK_IMAGE_TYPE_1D = 0, VK_IMAGE_TYPE_2D = 1, VK_IMAGE_TYPE_3D = 2, }
	int components = 4;
	int format = 37;	//VK_FORMAT_R8G8B8A8_UNORM=37,VK_FORMAT_R32G32B32A32_SFLOAT=109
	void* data = nullptr;	//纹理数据
	uint64_t size = 0;
	void* sampler = 0;
	void* texture = nullptr;
}TextureInfo;

typedef struct FrameBufferInfo
{
public:
	int type = (int)dvkObjType::E_FBO;

	int width = 0;
	int height = 0;
	//数量
	int count = 1;
	bool ispush = true;
	//内部对象指针
	void* fbo = nullptr;

}FrameBufferInfo;

/*
   顶点属性
   坐标				VK_FORMAT_R32G32B32A32_SFLOAT
   纹理坐标UV		VK_FORMAT_R32G32_SFLOAT
   color			VK_FORMAT_R8G8B8A8_UNORM
   Normal			VK_FORMAT_R32G32B32A32_SFLOAT
*/
typedef struct PipelineInfo
{
	int type = (int)dvkObjType::E_Pipeline;
	int polygonMode = 0;				//默认填充  FILL = 0,LINE = 1,POINT = 2,
	int cullMode = 2;					//默认剔除背面 NONE = 0,    FRONT_BIT = 1,    BACK_BIT = 2,    FRONT_AND_BACK=3
	int frontFace = 0;					//默认正面逆时针COUNTER_CLOCKWISE=0 顺时针CLOCKWISE=1
	float lineWidth = 1.0f;				//线宽
	//顶点原始拓扑
	int topology = 0;
	//纹理\ubo绑定位置
	uint32_t image_first_binding = 0;
	uint32_t uniform_first_binding = 1;

	//纹理、ubo绑定到的shader。
	/*uint32_t  imageCount = 0;
	uint32_t *imageStage = 0; //[HZ_UNIFORM_NUM_MAX] = { E_SHADER_STAGE_FRAGMENT_BIT };
	uint32_t  uniformCount = 0;
	uint32_t *uniformStage = 0;// [HZ_UNIFORM_NUM_MAX] = { E_SHADER_STAGE_VERTEX_BIT };*/
	uint32_t* descriptorType = 0;
	uint32_t  descriptorCount = 0;
	unsigned int* stride = 0;//顶点结构大小
	unsigned int  stride_count = 0;//数量
	//顶点属性
	uint32_t vfoCount = 0;	//数量
	int* vertexformat = 0;	//顶点类型数组
	int* vertexOffset = 0;	//如果偏移对应类型则可以自动计算不设置
	int* vertexstr = 0;		//DX扩展

	int depthTestEnable = 1;	//深度测试是否开启
	int depthWriteEnable = 1;	//深度是否写入

	unsigned int            blendEnable = 0;
	unsigned int            srcColorBlendFactor = 1;
	unsigned int            dstColorBlendFactor = 0;
	unsigned int            colorBlendOp = 0;
	unsigned int            srcAlphaBlendFactor = 1;
	unsigned int            dstAlphaBlendFactor = 0;
	unsigned int            alphaBlendOp = 0;
	float					blendConstants[4] = { 0.0f };
	int						blend = 0;					//-1使用自定义混合参数，0关闭混合，1启用默认颜色混合

	//设置需要动态改变的状态bit
	unsigned int			dynamicState = 0;
	//默认0为字符类型，2为二进制类型,
	int pipe_type = 0;
	unsigned char* vert_data = nullptr;
	size_t vert_size = 0;
	unsigned char* frag_data = nullptr;
	size_t frag_size = 0;
	//返回管线的指针
	void* pipeline = nullptr;
	char* name = 0;
}PipelineInfo;

#endif

#ifndef GLFWwindow
typedef struct GLFWwindow GLFWwindow;
#endif
namespace hz
{
	//---------------------------------------------------------------------------------------------

#if 0
	class Render
	{
	public:
		Render()
		{
		}

		virtual	~Render()
		{
		}

		virtual void onSize(int width, int height)
		{

		}
		virtual void dev_size() {}
		virtual void render()
		{

		}
		virtual int initRender(uint32_t width, uint32_t height) = 0;
		// 		{
		// 			width_ = width;
		// 			height_ = height;
		// 			return 0;
		// 		}
		//创建资源
		virtual void execute(void* data, size_t size)
		{
			int type = 0;
			jsont::sscanf((char*)data, "%d", &type);
			if (type)
			{
				//LOGE("func=%s,line=%d,file=%s\n", __FUNCTION__, __LINE__, __FILE__);
				//LOGI("%d", (int)func_.size());
				auto it = func_.find(type);
				if (it != func_.end())
					it->second(data);
			}
		}
		virtual int getFPS()
		{
			return 0;
		}
		virtual void resetDrawCall() {}
		virtual void cleanup() {}
		virtual void destroyRes(Res* res) {}
		virtual void pushDrawData(Canvas* p) {}
		virtual void push3D(OBJ3d* obj) {}
		virtual void pop3D(OBJ3d* obj) {}

		virtual void setBackColor(unsigned int col)
		{
		}
		virtual bool is_drawcall()
		{
			return false;
		}
	public:
		uint32_t width_ = 0, height_ = 0;

		GLFWwindow* glfw_ = nullptr;
		void* platformWindow_ = nullptr;

		std::map<int, std::function<void(void* p)>> func_;
	};
#endif
	//---------------------------------------------------------------------------------------------


}//!hz
#endif // !__hlContext__H_
