#ifndef __utils_info_h__
#define __utils_info_h__
/*
 通用工具结构

*/
namespace hz
{
	namespace css
	{
		/*
		box-shadow参数解释
	@1：inset

	  有inset 则为内阴影，没有insert 则为外阴影，默认为外阴影。

	@2：offset-x

	 横向阴影的大小，正值阴影在右边，负值阴影在左边，0的时候阴影中盒子后面，看不见的，如果有blur-radius值会有模糊效果。

	@3：offset-y

	纵向阴影的大小，值同offset-x。

	@4：blur-radius

	阴影的模糊程度，值越大，阴影越模糊。

	@5：spread-radius

	 阴影的扩大缩小，正值时，阴影扩大；负值时，阴影缩小，默认为0，和盒子同样大。
		*/
		typedef struct
		{
			int inset = 0;
			glm::vec2 offset;
			int blur_radius;
			int spread_radius;
			unsigned int color = 0xffcccccc;
		}box_shadow;
		/*
		border	简写属性，用于把针对四个边的属性设置在一个声明。
		border-style	用于设置元素所有边框的样式，或者单独地为各边设置边框样式。上、右、	下、	左
		border-width	简写属性，用于为元素的所有边框设置宽度，或者单独地为各边边框设置宽度。
		border-color	简写属性，设置元素的所有边框中可见部分的颜色，或为 4 个边分别设置颜色。
		border-bottom	简写属性，用于把下边框的所有属性设置到一个声明中。
		border-bottom-color	设置元素的下边框的颜色。
		border-bottom-style	设置元素的下边框的样式。
		border-bottom-width	设置元素的下边框的宽度。
		border-left	简写属性，用于把左边框的所有属性设置到一个声明中。
		border-left-color	设置元素的左边框的颜色。
		border-left-style	设置元素的左边框的样式。
		border-left-width	设置元素的左边框的宽度。
		border-right	简写属性，用于把右边框的所有属性设置到一个声明中。
		border-right-color	设置元素的右边框的颜色。
		border-right-style	设置元素的右边框的样式。
		border-right-width	设置元素的右边框的宽度。
		border-top	简写属性，用于把上边框的所有属性设置到一个声明中。
		border-top-color	设置元素的上边框的颜色。
		border-top-style	设置元素的上边框的样式。
		border-top-width	设置元素的上边框的宽度。
		*/
		enum border_style
		{
			bse_none,		//定义无边框。
			bse_hidden,		//与 "none" 相同。不过应用于表时除外，对于表，hidden 用于解决边框冲突。
			bse_dotted,		//定义点状边框。在大多数浏览器中呈现为实线。
			bse_dashed,		//定义虚线。在大多数浏览器中呈现为实线。
			bse_solid,		//定义实线。
			bse_double,		//定义双线。双线的宽度等于 border - width 的值。
			bse_groove,		//定义 3D 凹槽边框。其效果取决于 border - color 的值。
			bse_ridge,		//定义 3D 垄状边框。其效果取决于 border - color 的值。
			bse_inset,		//定义 3D inset 边框。其效果取决于 border - color 的值。
			bse_outset,		//定义 3D outset 边框。其效果取决于 border - color 的值。
			bse_inherit,	//规定应该从父元素继承边框框样式。
		};
		typedef struct
		{
			int style[4] = { 0 };
			int width = 0;
			int color = 0;
			int bottom = 0;
			int bottom_color = 0;
			int bottom_style = 0;
			int bottom_width = 0;
			int left = 0;
			int left_color = 0;
			int left_style = 0;
			int left_width = 0;
			int right = 0;
			int right_color = 0;
			int right_style = 0;
			int right_width = 0;
			int top = 0;
			int top_color = 0;
			int top_style = 0;
			int top_width = 0;
		}border;
		//----------------------------------------------------------------------------------------------------------------------------------------------------------

	}
	//!css
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
		void* unser_data1 = nullptr;
	}draw_image_info;
	class draw_font_info
	{
	public:
		std::vector<draw_image_info> vitem, blurs;
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
