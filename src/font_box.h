#ifndef __font_box_h__
#define __font_box_h__
#include <list>
#include <set>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>  
#include <thread>  
#include <queue>  
#include <functional>
/*
	装箱算法

	// 用法
	FontBox<Image> _box;
	//设置默认块大小
	_box.set_defmax({ 1024,1024 });

	//把一个图像矩形装箱返回位置
	Image* img = 0;//单个字符图像
	Image* oi = nullptr;//返回字体箱子的图像
	glm::ivec4 rs = _box.push(img, &oi);

	//清空所有
	_box.clear();
	*/
#ifndef __Image__h__

#if 1
#ifdef _HAS_SHARED_MUTEX
#define HSM
#endif
typedef std::shared_mutex LockS;
#define LOCK_R(ls) std::shared_lock<LockS> __locker_##ls(ls)
#define LOCK_W(ls) std::unique_lock<LockS> __locker_##ls(ls)

typedef std::recursive_mutex Lock;
#define LOCK(l) std::lock_guard<Lock> __locker_##ls(l)
#endif
#ifndef GLM_VERSION
namespace glm {
	class ivec2
	{
	public:

		int x = 0, y = 0;
	public:
		ivec2()
		{
		}
		ivec2(int xx, int yy) :x(xx), y(yy)
		{
		}

		~ivec2()
		{
		}

	private:

	};
	class ivec4
	{
	public:
		int x = 0, y = 0, z = 0, w = 0;
	public:
		ivec4()
		{
		}
		ivec4(int xx, int yy, int zz, int ww)
		{
		}
		ivec4(int xx, int yy, ivec2 zw)
		{
		}

		~ivec4()
		{
		}

	private:

	};

	ivec2 operator-(ivec2 const& v1, ivec2 const& v2)
	{
		return ivec2(v1.x - v2.x, v1.y - v2.y);
	}
	bool operator<(glm::ivec2 const& v1, glm::ivec2 const& v2)
	{
		return (v1.y < v2.y || (v1.y == v2.y && v1.x < v2.x));
	}
};
#endif
class Res
{
public:
	int _type = 0;
	int64_t _inc_ = 0;
	std::string _name;
public:
	Res()
	{
	}

	virtual ~Res()
	{
	}

	static void destroy(Res* p)
	{
		if (p)
		{
			p->release();
		}
	}

	void release()
	{
		if (--_inc_ < 0)
		{
			delete this;
		}
	}

	int64_t addRef()
	{
		return ++_inc_;
	}
	void set_name(const std::string& n)
	{
		_name = n;
	}
private:

};

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
};
#endif
template<class _Ty = Image, class pr_copy_value = _Ty::copy_image, class pr_create = _Ty::create_size>
class FontBox :public Res
{
public:
	using value_type =_Ty;
	pr_copy_value value_copy;
	pr_create value_create;
private:
	class box_node :public Res
	{
	public:
		glm::ivec2 _pos, _size;
		value_type* _value = nullptr;
		bool isfull = false;
		static const int minbn = 6;
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
						set_rect(nexts);
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

	glm::ivec2 _defmax = { 1024,1024 };
	LockS _lock;
public:
	FontBox()
	{
	}
	~FontBox()
	{
		LOCK_W(_lock);
		for (auto it : _box)
		{
			value_type::destroy(it);
		}
	}
public:
	// todo把一个图像矩形装箱返回位置
	glm::ivec4 push(value_type* img, value_type** oi)
	{
		glm::ivec4 rs = { 0,0,img->width,img->height };// img->_prc;
		glm::ivec4 ret = { -1,-1,rs.z,rs.w };
		glm::ivec2 s = { rs.z,rs.w };

		if (rs.z > _defmax.x || rs.w > _defmax.y)//|| (size.y > (_maxsize.y - _height)))
		{
			return ret;
		}
		bool no_put = find_put(s, rs, img, ret, oi);
		if (no_put)
		{
			push_box();
			find_put(s, rs, img, ret, oi);
		}
		return ret;
	}
private:
	bool find_put(const glm::ivec2& s, const glm::ivec4& rs, value_type* img, glm::ivec4& ret, value_type** oi)
	{
		LOCK_W(_lock);
		std::vector<box_node*> addtem, full;
		glm::ivec2 pos;
		bool no_put = true;
		//查找合适的位置
		for (auto it : _data_free)
		{
			auto nbn = it->find_split_put(s, &pos);
			if (pos.x >= 0 && (pos.y >= 0))
			{
				ret.x = pos.x;
				ret.y = pos.y;
				if (img && img->width > 0 && img->height > 0)
				{
					value_copy(it->_value, img, rs, ret);
					if (oi)
					{
						*oi = it->_value;
					}
				}
				if (nbn)
				{
					addtem.push_back(nbn);
				}
				if (it->is_full())
				{
					full.push_back(it);
				}
				no_put = false;
				break;
			}
			if (nbn)
			{
				printf("error\n");
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
		return no_put;
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
};

#endif // !__font_box_h__