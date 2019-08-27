#ifndef __image_packer_h__
#define __image_packer_h__

#include <list>
#include <set>
#include <algorithm>
#include <random>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>  
#include <thread>  
#include <queue>  
#include <functional>

#ifndef LOCK_R
#include <shared_mutex>
typedef std::shared_mutex LockS;
#define LOCK_R(ls) std::shared_lock<LockS> __locker_(ls)
#define LOCK_W(ls) std::unique_lock<LockS> __locker_(ls)
#endif // !LOCK_R
#ifndef GLM_OPERATOR
namespace glm
{
#define GLM_OPERATOR
	//vec
	bool operator<(glm::ivec4 const& v1, glm::ivec4 const& v2)
	{
		std::array<int, 4> va[2] = { {v1.x,v1.y,v1.z,v1.w}, {v2.x,v2.y,v2.z,v2.w} };
		bool ret = va[0] < va[1];
		return ret;
	}
	bool operator<(glm::vec4 const& v1, glm::vec4 const& v2)
	{
		std::array<double, 4> va[2] = { {v1.x,v1.y,v1.z,v1.w}, {v2.x,v2.y,v2.z,v2.w} };
		bool ret = va[0] < va[1];
		return ret;
	}
	bool operator<(glm::vec3 const& v1, glm::vec3 const& v2)
	{
		bool yr = (v1.z < v2.z) || (v1.y < v2.y || (v1.y == v2.y && v1.x < v2.x));
		std::array<double, 3> va[2] = { {v1.x,v1.y,v1.z}, {v2.x,v2.y,v2.z} };
		bool ret = va[0] < va[1];
		return ret;
	}
	bool operator<(glm::vec2 const& v1, glm::vec2 const& v2)
	{
		bool yr = (v1.y < v2.y || (v1.y == v2.y && v1.x < v2.x));
		std::array<double, 2> va[2] = { {v1.x,v1.y},{v2.x,v2.y} };
		bool ret = va[0] < va[1];
		return ret;
	}
	bool operator>(glm::vec3 const& v1, glm::vec3 const& v2)
	{
		bool yr = (v1.z > v2.z) || (v1.y > v2.y || (v1.y == v2.y && v1.x > v2.x));
		std::array<double, 3> va[2] = { {v1.x,v1.y,v1.z},{v2.x,v2.y,v2.z} };
		bool ret = va[1] < va[0];
		return ret;
	}
	bool operator>(glm::vec2 const& v1, glm::vec2 const& v2)
	{
		bool yr = (v1.y > v2.y || (v1.y == v2.y && v1.x > v2.x));
		std::array<double, 3> va[2] = { {v1.x,v1.y},{v2.x,v2.y} };
		bool ret = va[1] < va[0];
		return ret;
	}
	/*
		bool operator>=(glm::vec3 const & v1, glm::vec3 const & v2)
		{
			bool yr = (v1.z >= v2.z) || (v1.y >= v2.y || (v1.y == v2.y&& v1.x >= v2.x));
			std::array<double, 3> va[2] = { {v1.x,v1.y,v1.z},{v2.x,v2.y,v2.z} };
			bool ret = va[0] < va[1];
			return ret;
		}
		bool operator>=(glm::vec2 const & v1, glm::vec2 const & v2)
		{
			bool yr = (v1.y >= v2.y || (v1.y == v2.y&& v1.x >= v2.x));
			std::array<double, 3> va[2] = { {v1.x,v1.y,v1.z},{v2.x,v2.y,v2.z} };
			bool ret = va[0] < va[1];
			return ret;
		}*/
		//uvec
	bool operator<(glm::uvec3 const& v1, glm::uvec3 const& v2)
	{
		return (v1.z < v2.z) || (v1.y < v2.y || (v1.y == v2.y && v1.x < v2.x));
	}
	bool operator<(glm::uvec2 const& v1, glm::uvec2 const& v2)
	{
		return (v1.y < v2.y || (v1.y == v2.y && v1.x < v2.x));
	}
	bool operator>(glm::uvec3 const& v1, glm::uvec3 const& v2)
	{
		return (v1.z > v2.z) || (v1.y > v2.y || (v1.y == v2.y && v1.x > v2.x));
	}
	bool operator>(glm::uvec2 const& v1, glm::uvec2 const& v2)
	{
		return (v1.y > v2.y || (v1.y == v2.y && v1.x > v2.x));
	}
	bool operator>=(glm::uvec3 const& v1, glm::uvec3 const& v2)
	{
		return (v1.z >= v2.z) || (v1.y >= v2.y || (v1.y == v2.y && v1.x >= v2.x));
	}
	bool operator>=(glm::uvec2 const& v1, glm::uvec2 const& v2)
	{
		return (v1.y >= v2.y || (v1.y == v2.y && v1.x >= v2.x));
	}
	//ivec
	bool operator<(glm::ivec3 const& v1, glm::ivec3 const& v2)
	{
		return (v1.z < v2.z) || (v1.y < v2.y || (v1.y == v2.y && v1.x < v2.x));
	}
	bool operator<(glm::ivec2 const& v1, glm::ivec2 const& v2)
	{
		return (v1.y < v2.y || (v1.y == v2.y && v1.x < v2.x));
	}
	bool operator>(glm::ivec3 const& v1, glm::ivec3 const& v2)
	{
		return (v1.z > v2.z) || (v1.y > v2.y || (v1.y == v2.y && v1.x > v2.x));
	}
	bool operator>(glm::ivec2 const& v1, glm::ivec2 const& v2)
	{
		return (v1.y > v2.y || (v1.y == v2.y && v1.x > v2.x));
	}
	bool operator>=(glm::ivec3 const& v1, glm::ivec3 const& v2)
	{
		return (v1.z >= v2.z) || (v1.y >= v2.y || (v1.y == v2.y && v1.x >= v2.x));
	}
	bool operator>=(glm::ivec2 const& v1, glm::ivec2 const& v2)
	{
		return (v1.y >= v2.y || (v1.y == v2.y && v1.x >= v2.x));
	}
};
#endif // !GLM_OPERATOR
#ifndef __Image__h__
#ifdef HZ_IMAGE
#include "Image.h"
#else

namespace hz
{
	class Image :public Res
	{
	public:
		struct copy_image {
			void operator()(Image* dst, Image* src, const glm::ivec4& src_rect, const glm::ivec4& dest_rect)
			{
				dst->copy_value(src, src_rect, dest_rect);
			}
		};
		struct create_size {
			Image* operator()(int w, int h) { return create_null(w, h); }
		};
	public:
		int width, height;
	public:
		static Image* create_null(int w, int h)
		{
			Image* p = nullptr;
			return p;
		}
		void copy_value(Image* src, const glm::ivec4& src_rect, const glm::ivec4& dest_rect)
		{

		}
		void clear()
		{

		}
		void clear_color(unsigned int)
		{}
		void resize(unsigned int, unsigned int)
		{
		}
		void copy_to_image(unsigned char* bit, int pitch, glm::ivec4 rct, unsigned int color, int pixel_mode, float brightness = 0)
		{
		}
		void saveImage(const std::string)
		{
		}
		void DrawRect(const glm::ivec4& r, unsigned int col, unsigned int fill, float rounding = 0.0f,
			int rounding_corners_flags = ~0, float thickness = 1.0f)
		{
		}
	};
};
//!hz
#endif // HZ_IMAGE
#endif

namespace hz
{
#ifndef STB_RECT_PACK_VERSION

	struct stbrp_rect
	{
		// reserved for your use:
		int            id;

		// input:
		uint16_t    w, h;

		// output:
		uint16_t    x, y;
		int            was_packed;  // non-zero if valid packing

	}; // 16 bytes, nominally
#endif
#ifdef STB_RECT_PACK_VERSION
	class stb_packer
	{
	private:
		stbrp_context _ctx;
		std::vector<stbrp_node> _rpns;
	public:
		stb_packer()
		{
		}

		~stb_packer()
		{
		}
		void init_target(int width, int height)
		{
			_rpns.resize(width);
			stbrp_init_target(&_ctx, width, height, _rpns.data(), _rpns.size());
			setup_allow_out_of_mem(0);
		}
	public:
		int pack_rects(stbrp_rect* rects, int num_rects)
		{
			return stbrp_pack_rects(&_ctx, rects, num_rects);
		}
		void setup_allow_out_of_mem(int allow_out_of_mem)
		{
			stbrp_setup_allow_out_of_mem(&_ctx, allow_out_of_mem);
		}
		/*
		可以选择库应该使用哪个打包启发式方法。不同

		启发式方法将为不同的数据集生成更好/更差的结果。

		如果再次调用init，将重置为默认值。*/
		void etup_heuristic(int heuristic = 1)
		{
			stbrp_setup_heuristic(&_ctx, heuristic);
		}
	private:

	};

#endif // STB_RECT_PACK_VERSION

#if 1
	/*
	箱子类需要有两个结构

	依赖项 glm

	//例子图像类
	class Image
	{
	public:
		struct copy_image {
			void operator()(Image* dst, Image* src, const glm::ivec4& src_rect, const glm::ivec4& dest_rect)
			{ dst->copy_value(src, src_rect, dest_rect); }
		};
		struct create_size {
			Image* operator()(int w, int h) { return create_null(w, h); }
		};
	};

	// 用法
	ImagePacker<Image> _box;
	//设置默认块大小
	_box.set_defmax({ 1024,1024 });

	//把一个图像矩形装箱返回位置
	Image* img = 0;//单个字符图像
	Image* oi = nullptr;//返回字体箱子的图像
	glm::ivec4 rs = _box.push(img, &oi);

	//清空所有
	_box.clear();
	*/
	template<class _Ty = Image, class pr_copy_value = _Ty::copy_image, class pr_create = _Ty::create_size>
	class ImagePacker :public Res
	{
	public:
		using value_type =_Ty;
		pr_copy_value value_copy;
		pr_create value_create;
		static const int minbn = 2;
	private:
		class box_node :public Res
		{
		public:
			glm::ivec2 _pos, _size;
			value_type* _value = nullptr;
			bool isfull = false;
		public:
			box_node()
			{
			}
			box_node(glm::ivec4 rc, value_type* img) :_size({ rc.z,rc.w }), _pos({ rc.x,rc.y }), _value(img)
			{
			}
			~box_node()
			{
			}
			static box_node* create(glm::ivec4 rc, value_type* img)
			{
				box_node* p = new box_node(rc, img);
				return p;
			}
		public:
			bool is_full()
			{
				return isfull;
			}
			// 查询是否有空闲块、分割空闲块
			box_node* find_split_put(glm::ivec2 s, glm::ivec2* ot)
			{
				box_node* ret = nullptr;
				glm::ivec4 n;
				std::set<glm::ivec4> newrc;
				if (ot)
				{
					ot->x = -1;
				}
				if (_size.y >= s.y && _size.x >= s.x)
				{
					// 保存偶数对齐
					if (s.y % 2) s.y++;
					auto trc = _size - s;
					if (ot)
					{
						ot->x = _pos.x;
						ot->y = _pos.y;
					}
					if (trc.x < minbn && trc.y < minbn)
					{
						isfull = true;
					}
					else
					{
						n.x = _pos.x + s.x;
						n.y = _pos.y;
						n.z = trc.x;
						// 判断是否需要向下分割
						if (_pos.x == 0)
						{
							glm::ivec4 nexts = { 0, _pos.y + s.y, _size.x, trc.y };
							// 重新设置本块大小
							set_rect(nexts);
							n.w = s.y;

							if (n.z >= minbn)
							{
								//创建右边空闲块
								ret = create(n, _value);
							}
						}
						else if (_size.y - s.y > minbn)
						{
							// 分割小块
							glm::ivec4 nexts = n;
							nexts.z = s.x;
							nexts.w = _size.y - s.y;
							set_rect(nexts);
							n.z -= s.x;
							n.w = s.y;
							if (n.z >= minbn)
							{
								//创建右边空闲块
								ret = create(n, _value);
							}
						}
						else
						{
							n.w = _size.y;
							set_rect(n);
						}
					}
				}
				return ret;
			}
			void set_rect(const glm::ivec4& rt)
			{
				_size.x = rt.z;
				_size.y = rt.w;
				_pos.x = rt.x;
				_pos.y = rt.y;
			}
		private:

		};
		//空闲表
		std::list<box_node*> _data_free;
		std::set<value_type*> _box;
		glm::ivec2 _defmax = { 1024, 1024 };
		LockS _lock;
	public:
		ImagePacker()
		{
		}
		~ImagePacker()
		{
			LOCK_W(_lock);
			for (auto it : _box)
			{
				value_type::destroy(it);
			}
		}
		void maps(std::function<void(value_type*)> func)
		{
			LOCK_R(_lock);
			for (auto it : _box)
			{
				func(it);
			}
		}
	public:
		/*
		todo:把一个图像矩形装箱返回位置
		dst 是源图像区域，
		ivec2 返回坐标
		*/
		value_type* push(value_type* img, glm::ivec4* dst, glm::ivec2* pos)
		{
			glm::ivec4 td;
			if (!dst)
			{
				dst = &td;
			}
			if (dst->z < 1)
			{
				dst->z = img->width;
			}
			if (dst->w < 1)
			{
				dst->w = img->height;
			}
			int iw = (img->width - dst->x), ih = (img->height - dst->y);
			int width = std::min(iw, dst->z)
				, height = std::min(ih, dst->w);
			glm::ivec4 rs = { dst->x, dst->y, width, height };

			value_type* ret = nullptr;
			glm::ivec2 s = { rs.z,rs.w };
			if (rs.z > _defmax.x || rs.w > _defmax.y)
			{
				return ret;
			}
			ret = find_put(s, pos, img, &rs);
			if (!ret)
			{
				push_box();
				ret = find_put(s, pos, img, &rs);
			}
			return ret;
		}
		// todo把一个矩形块装箱返回位置
		value_type* push_rect(glm::ivec2 rc, glm::ivec2* pos)
		{
			value_type* ret = nullptr;
			glm::ivec2 s = { rc.x, rc.y };
			if (rc.x > _defmax.x || rc.y > _defmax.y)
			{
				return ret;
			}
			ret = find_put(s, pos);
			if (!ret)
			{
				push_box();
				ret = find_put(s, pos);
			}
			return ret;
		}

	private:
		value_type* find_put(const glm::ivec2& s, glm::ivec2* out_pos, value_type* img = nullptr, glm::ivec4* rs = nullptr)
		{
			LOCK_W(_lock);
			std::vector<box_node*> addtem, full;
			glm::ivec2 pos;
			glm::ivec4 dst_rect = { 0,0,s.x,s.y };
			value_type* ret = nullptr;
			//查找合适的位置
			std::vector<box_node*> tem;
			auto fs = s;

			if (fs.y % 2) fs.y++;
			for (auto it : _data_free)
			{
				auto c = it->_size - s;
				if (c.x >= 0 && c.y >= 0 && c.y <= minbn || it->_pos.x == 0)
				{
					tem.push_back(it);
				}
			}
			std::sort(tem.begin(), tem.end(), [](box_node* bn1, box_node* bn2) { return bn1->_size < bn2->_size; });
			for (auto it : tem)
			{
				auto nbn = it->find_split_put(s, &pos);
				if (pos.x >= 0 && (pos.y >= 0))
				{
					dst_rect.x = pos.x;
					dst_rect.y = pos.y;
					ret = it->_value;
					if (img && img->width > 0 && img->height > 0)
					{
						value_copy(it->_value, img, *rs, dst_rect);
					}
					if (nbn)
					{
						addtem.push_back(nbn);
					}
					if (it->is_full())
					{
						full.push_back(it);
					}
					break;
				}
				if (nbn)
				{
					printf("unknown error\n");
					throw "find_put";
				}
			}
			if (addtem.size() || full.size())
			{
				for (auto it : full)
				{
					_data_free.remove(it);
					box_node::destroy(it);
				}
				for (auto it : addtem)
				{
					push_box_free(it, false);
				}
				push_box_free(0, true);
			}
			if (out_pos)
			{
				*out_pos = pos;
			}
			return ret;
		}
	public:
		void clear()
		{
			LOCK_W(_lock);
			for (auto it : _data_free)
			{
				box_node::destroy(it);
			}
			_data_free.clear();
			glm::ivec4 rc = { 0,0,_defmax };
			for (auto it : _box)
			{
				it->clear();
				auto p = box_node::create(rc, it);
				_data_free.push_back(p);
			}
		}
	private:
		void push_box_free(box_node* p, bool issort)
		{
			if (p)
			{
				_data_free.push_back(p);
			}
			if (issort)
			{
				_data_free.sort([](box_node* f1, box_node* f2) { return f1->_size < f2->_size; });
			}
		}
	public:
		void set_defmax(const glm::ivec2& dm)
		{
			LOCK_W(_lock);
			if (dm.x > 0 && dm.y > 0)
				_defmax = dm;
		}
		glm::ivec2 get_defmax()
		{
			LOCK_R(_lock);
			return _defmax;
		}

		// 增加空白箱子
		void push_box()
		{
			LOCK_W(_lock);
			auto p = value_create(_defmax.x, _defmax.y);
			if (p)
			{
				_box.insert(p);
				glm::ivec4 rc = { 0,0,_defmax };
				auto pf = box_node::create(rc, p);
				push_box_free(pf, true);
			}
		}
	private:

	public:
		static void test_packer()
		{
			hz::ImagePacker<hz::Image> packer;							//缓存
			packer.set_defmax({ 512, 512 });
			std::set<hz::Image*> out;
			hz::Image* img = hz::Image::create_null(16, 16);

			static std::default_random_engine random(::time(0));
			std::uniform_int_distribution<int> dis1(4, 16);
			std::uniform_int_distribution<int> dis2(50, 255);

			// 箱子
#ifdef STB_RECT_PACK_VERSION
			stb_packer pack;
			pack.init_target(512, 512);
#endif
			std::vector<stbrp_rect> rcs;
			rcs.resize(1000);
			auto rss = rcs.size();
			int i;
			for (i = 0; i < rss; i++)
			{
				int tw = dis1(random);
				int th = dis1(random);
				auto& rc = rcs[i];
				rc.w = tw; rc.h = th;
				glm::vec3 v3;
				v3.x = dis2(random);
				v3.y = dis2(random);
				v3.z = dis2(random);
				unsigned int col = hz::to_uint(v3);
				rc.id = col;

				hz::Image* ret = 0;
				img->resize(tw, th);
				img->clear_color(col);
				packer.push(img, &ret);
				out.insert(ret);
			}
#ifdef STB_RECT_PACK_VERSION
			for (size_t i = 0; i < rss; i++)
			{
				pack.pack_rects(&rcs[i], 1);
			}
#endif
			Image tespack;
			tespack.resize(512, 512);
			for (size_t i = 0; i < rss; i++)
			{
				auto& it = rcs[i];
				glm::ivec4 trc = { it.x,it.y,it.w,it.h };
				tespack.draw_rect(trc, 0, it.id);
			}
			out.insert(&tespack);
			i = 0;
			for (auto it : out)
			{
				std::string fn = "test_packer" + std::to_string(i++) + ".png";
				it->saveImage(fn);
			}
		}
	};
	// !ImagePacker
#endif
}
// !hz
#endif // __image_packer_h__
