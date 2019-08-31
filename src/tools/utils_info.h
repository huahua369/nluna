#ifndef __utils_info_h__
#define __utils_info_h__
/*
 通用工具结构

*/
namespace hz
{
	/*
	todo draw结构体
	*/
	typedef struct {
		Image* user_image = nullptr;
		// a.xy=pos坐标，a.zw=width,height渲染宽高
		glm::vec4 a;
		// 图像区域
		glm::vec4 rect = glm::vec4{ -1 };
		// 是否九宫格缩放
		glm::vec4 sliced = glm::vec4{ -1 };
		unsigned int col = -1;
		int* out = nullptr;
		double adv = 0.0;
		void* unser_data = nullptr;
	}draw_image_info;
	class draw_font_info
	{
	public:
		std::vector<draw_image_info> vitem;
		std::vector<glm::ivec2> vpos;
		int diffbs = 0;
		double base_line = 0.0;
		double awidth = 0.0;
	public:
		draw_font_info()
		{
		}

		~draw_font_info()
		{
		}
		bool empty()
		{
			return vitem.empty();
		}
	private:

	};

}
// !hz
#endif // __utils_info_h__
