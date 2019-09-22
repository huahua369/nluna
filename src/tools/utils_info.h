#ifndef __utils_info_h__
#define __utils_info_h__
/*
 ͨ�ù��߽ṹ

*/
namespace hz
{
	namespace css
	{
		/*
		box-shadow��������
	@1��inset

	  ��inset ��Ϊ����Ӱ��û��insert ��Ϊ����Ӱ��Ĭ��Ϊ����Ӱ��

	@2��offset-x

	 ������Ӱ�Ĵ�С����ֵ��Ӱ���ұߣ���ֵ��Ӱ����ߣ�0��ʱ����Ӱ�к��Ӻ��棬�������ģ������blur-radiusֵ����ģ��Ч����

	@3��offset-y

	������Ӱ�Ĵ�С��ֵͬoffset-x��

	@4��blur-radius

	��Ӱ��ģ���̶ȣ�ֵԽ����ӰԽģ����

	@5��spread-radius

	 ��Ӱ��������С����ֵʱ����Ӱ���󣻸�ֵʱ����Ӱ��С��Ĭ��Ϊ0���ͺ���ͬ����
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
		border	��д���ԣ����ڰ�����ĸ��ߵ�����������һ��������
		border-style	��������Ԫ�����б߿����ʽ�����ߵ�����Ϊ�������ñ߿���ʽ���ϡ��ҡ�	�¡�	��
		border-width	��д���ԣ�����ΪԪ�ص����б߿����ÿ�ȣ����ߵ�����Ϊ���߱߿����ÿ�ȡ�
		border-color	��д���ԣ�����Ԫ�ص����б߿��пɼ����ֵ���ɫ����Ϊ 4 ���߷ֱ�������ɫ��
		border-bottom	��д���ԣ����ڰ��±߿�������������õ�һ�������С�
		border-bottom-color	����Ԫ�ص��±߿����ɫ��
		border-bottom-style	����Ԫ�ص��±߿����ʽ��
		border-bottom-width	����Ԫ�ص��±߿�Ŀ�ȡ�
		border-left	��д���ԣ����ڰ���߿�������������õ�һ�������С�
		border-left-color	����Ԫ�ص���߿����ɫ��
		border-left-style	����Ԫ�ص���߿����ʽ��
		border-left-width	����Ԫ�ص���߿�Ŀ�ȡ�
		border-right	��д���ԣ����ڰ��ұ߿�������������õ�һ�������С�
		border-right-color	����Ԫ�ص��ұ߿����ɫ��
		border-right-style	����Ԫ�ص��ұ߿����ʽ��
		border-right-width	����Ԫ�ص��ұ߿�Ŀ�ȡ�
		border-top	��д���ԣ����ڰ��ϱ߿�������������õ�һ�������С�
		border-top-color	����Ԫ�ص��ϱ߿����ɫ��
		border-top-style	����Ԫ�ص��ϱ߿����ʽ��
		border-top-width	����Ԫ�ص��ϱ߿�Ŀ�ȡ�
		*/
		enum border_style
		{
			bse_none,		//�����ޱ߿�
			bse_hidden,		//�� "none" ��ͬ������Ӧ���ڱ�ʱ���⣬���ڱ�hidden ���ڽ���߿��ͻ��
			bse_dotted,		//�����״�߿��ڴ����������г���Ϊʵ�ߡ�
			bse_dashed,		//�������ߡ��ڴ����������г���Ϊʵ�ߡ�
			bse_solid,		//����ʵ�ߡ�
			bse_double,		//����˫�ߡ�˫�ߵĿ�ȵ��� border - width ��ֵ��
			bse_groove,		//���� 3D ���۱߿���Ч��ȡ���� border - color ��ֵ��
			bse_ridge,		//���� 3D ¢״�߿���Ч��ȡ���� border - color ��ֵ��
			bse_inset,		//���� 3D inset �߿���Ч��ȡ���� border - color ��ֵ��
			bse_outset,		//���� 3D outset �߿���Ч��ȡ���� border - color ��ֵ��
			bse_inherit,	//�涨Ӧ�ôӸ�Ԫ�ؼ̳б߿����ʽ��
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
	todo draw�ṹ��
	*/
	typedef struct {
		Image* user_image = nullptr;
		// a.xy=pos���꣬a.zw=width,height��Ⱦ���
		glm::vec4 a;
		// ͼ������
		glm::vec4 rect = glm::vec4{ -1 };
		// �Ƿ�Ź�������
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
