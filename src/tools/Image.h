/*
 图像操作
 支持打开图片格式
 JPEG、PNG、 BMP、 PSD、 TGA、 GIF、 HDR、 PIC、 PNM(.ppmand.pgm)
 支持保存bmp、png、jpg、tga、hdr

Image(std::string &imgfn)
static Image* create()
static Image* create(std::string imgfn)
static Image* create_null(int w, int h)
static void destroy(Image *p)

void loadImage(std::string imgfn)
void loadMemImage(const char *data, size_t len)
int saveImage(std::string fn, int comp = 4)

示例：
加载图片：
hz::Image *img = hz::Image::create("f.png");//"border.png");//
if (!img || img->datasize() == 0)
{
//加载成功
}
或者
hz::Image img1,img2("a.png");
img1.loadImage("f.png");
if (img1.datasize() == 0)
{
//加载成功
}
*/

#ifndef __Image__h__
#define __Image__h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <set>
#include <string>
#include <vector>
#include <math.h>
#ifndef _WIN32
//#include "s/wingdi.h"
#endif
#include "stb/lib_src.h"
#ifdef _GLI_HEAD_
#include "gli/gli.hpp"
#endif
//#include <stb/hlstb.h>
#ifdef _WIN32
#include <io.h>
#elif defined(__ANDROID__)
#include <unistd.h>  
#endif

//#include "svpng.inc"
#ifdef LIBPNG
#include "libpng.h"
#endif
#ifndef LOBYTE
#define LOBYTE(w)           ((unsigned char)(((size_t)(w)) & 0xff))
#define GetRValue(rgb)      (LOBYTE(rgb))
#define GetGValue(rgb)      (LOBYTE(((unsigned short)(rgb)) >> 8))
#define GetBValue(rgb)      (LOBYTE((rgb)>>16))
#endif // !LOBYTE

#ifndef HZ_COL32_A_MASK
#define ARRAYSIZE(_ARR) ((int)(sizeof(_ARR)/sizeof(*_ARR)))
#define _PI                   3.14159265358979323846f
#define HZ_COL32_A_MASK     0xFF000000
#endif
#ifndef HZ_BIT
//位左移
#define HZ_BIT(x) (1<<x)
#endif

#ifndef _WIN32
typedef unsigned short WORD;
typedef unsigned int UINT;
typedef unsigned long DWORD;
typedef struct tagRGBQUAD {
	unsigned char    rgbBlue;
	unsigned char    rgbGreen;
	unsigned char    rgbRed;
	unsigned char    rgbReserved;
} RGBQUAD;
typedef struct tagBITMAPINFOHEADER {
	DWORD      biSize;
	long       biWidth;
	long       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	long       biXPelsPerMeter;
	long       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
} BITMAPINFOHEADER, * LPBITMAPINFOHEADER, * PBITMAPINFOHEADER;
typedef struct tagBITMAPINFO {
	BITMAPINFOHEADER    bmiHeader;
	RGBQUAD             bmiColors[1];
} BITMAPINFO, * LPBITMAPINFO, * PBITMAPINFO;

typedef struct tagRECT
{
	long    left;
	long    top;
	long    right;
	long    bottom;
} RECT;
#endif
namespace hz {

	enum GuiDir
	{
		GuiDir_None = 0,
		GuiDir_Left = HZ_BIT(0),
		GuiDir_Right = HZ_BIT(1),
		GuiDir_Up = HZ_BIT(2),
		GuiDir_Down = HZ_BIT(3),
		GuiDir_bevel = HZ_BIT(4),
		GuiDir_bevel0 = GuiDir_bevel | GuiDir_Left,
		GuiDir_bevel1 = GuiDir_bevel | GuiDir_Up,
		GuiDir_bevel2 = GuiDir_bevel | GuiDir_Right,
		GuiDir_bevel3 = GuiDir_bevel | GuiDir_Down,
		GuiDir_Count_
	};

	typedef struct Offset3D {
		int32_t    x;
		int32_t    y;
		int32_t    z;
	} Offset3D;
	typedef struct Extent3D {
		uint32_t    width;
		uint32_t    height;
		uint32_t    depth;
	} Extent3D;
	typedef struct mageSubresourceLayers {
		uint32_t			  aspectMask;
		uint32_t              mipLevel;
		uint32_t              baseArrayLayer;
		uint32_t              layerCount;
	} ImageSubresourceLayers;
	typedef struct BufferImageCopy {
		size_t					  bufferOffset;
		uint32_t                  bufferRowLength;
		uint32_t                  bufferImageHeight;
		ImageSubresourceLayers    imageSubresource;
		Offset3D                  imageOffset;
		Extent3D                  imageExtent;
	} BufferImageCopy;

	// Histogram
	class yHist
	{
	public:
		yHist()
		{
		}

		~yHist()
		{
		}
		void init(int c = 256)
		{
			hists.resize(c);
			memset(hists.data(), 0, sizeof(int) * c);
		}
		void Hist(unsigned char c)
		{
			hists[c]++;
		}

		//比较两个直方图
		bool diff(yHist* p, int dmin = 1500, int dmax = 2000)
		{
			mset.clear();
			for (size_t i = 0; i < 256; i++)
			{
				mset.insert(hists[i] - p->hists[i]);
			}
			hmin = abs(*mset.begin());
			hmax = abs(*mset.rbegin());
			bool ret = (hmin > dmin) || (hmax > dmax);
			if (ret)
			{
				ret = true;
			}
			return ret;
		}

	public:
		//创建直方图
		std::vector<int> hists;
		std::set<int> mset;
		int hmin = 0, hmax = 0;
	};

	//图像类
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
			Image* operator()(int w, int h)
			{
				return create_null(w, h);
			}
		};
		class item
		{
		public:
			uint32_t width, height;
			uint32_t _size = 0;
		public:
			item()
			{
			}

			~item()
			{
			}
			uint32_t size()
			{
				return _size;
			}
		private:

		};

	public:
		std::string _filename;
		//请求的通道数
		int requested_components = 4;
		size_t width = 0, height = 0;// , data_size = 0;
		uint32_t mipLevels = 1;
		uint32_t layerCount = 1;
		std::vector<item> _levels_info;
		// gif
		// 延迟
		std::vector<int> _delays;
		int _layers = 1;
		int _delays_cu = 0, _cur_idx = -1;

		std::string savefilename;
#ifndef M_RES
		// 是否更新
		std::atomic_int64_t _upt = 0;
#endif // !__RES__H__
		
		LockS _locks, _lock_upt, _lk_gif;
		BITMAPINFO bmpInfo;
		RGBQUAD    bmiColors[2];	// 为BITMAPINFO的m_bmiColors补充两个元素空间
		uint64_t user_data = 0;
		//图像文件的通道
		size_t components = 0;
	private:
		std::vector<unsigned int> _data, _arrayimg;
		std::vector<unsigned char> temp_data;
		std::vector<std::vector<unsigned int>*> _array;
		std::vector<BufferImageCopy> bufferCopyRegions;


		Timer _giftime;
		yHist _yh;
		// 像素区域
		glm::ivec4 _prc;
	public:
		Image()
		{
		}
		Image(const std::string& imgfn)
		{
			loadImage(imgfn);
		}
		Image(const char* data, size_t len)
		{
			loadMemImage(data, len);
		}
		static Image* create()
		{
			return getRC((Image*)nullptr);
		}
		static Image* create(const std::string& imgfn)
		{
			Image* p = getRC((Image*)nullptr);
			if (!p->loadImage(imgfn) || !(p->width > 0 && p->height > 0))
			{
				destroy(p);
				p = nullptr;
			}
			return p;
			//return new Image(imgfn);
		}
		static Image* createMem(const char* data, size_t len)
		{
			Image* p = getRC((Image*)nullptr);
			if (!p->loadMemImage(data, len) || !(p->width > 0 && p->height > 0))
			{
				destroy(p);
				p = nullptr;
			}
			return p;// new Image(data, len);
		}
		static Image* create_null(int w, int h)
		{
			Image* p = getRC((Image*)nullptr);
			p->resize(w, h);
			p->clearColor(0);
			p->components = 4;
			return p;
		}
#ifndef M_RES
		static void destroy(Image* p)
		{
			if (p)
			{
				p->clear_img();
				getRC(p, false);
			}
		}
		template <typename T>
		static T* getRC(T* v, bool isPop = true)
		{
			return RecycleBinNew<T>::makeRC(v, isPop ? RcType::RT_POP : RcType::RT_PUSH);
		}
#else
		static Image* getRC(Image* p)
		{
			return new Image();
		}
#endif // !M_RES
		Image& operator=(const Image& img)
		{
			copy((Image*)& img);
			return *this;
		}
		Image(const Image& img)
		{
			copy((Image*)& img);
		}
		void clear_img()
		{
			LOCK_W(_locks);
			_filename = "";
			//请求的通道数
			requested_components = 4;
			width = 0; height = 0;// , data_size = 0;
			mipLevels = 0;
			layerCount = 0;
			// gif
			// 延迟
			_delays.clear();
			_layers = 1;
			_delays_cu = 0;
			_cur_idx = -1;

			savefilename = "";

			user_data = 0;

			//图像文件的通道
			components = 0;
			_data.clear();
			_arrayimg.clear();
			temp_data.clear();
			_array.clear();
			bufferCopyRegions.clear();
		}
		bool empty()
		{
			return _data.empty() || !(width * height);
		}

		unsigned int get_pixel(int x, int y)
		{
			if (y * width + x > _data.size())
			{
				throw 1;
			}
			return _data[y * width + x];
		}
		void set_pixel(int x, int y, unsigned int c, int m = 1)
		{
			if (y * width + x > _data.size())
			{
				return;
				throw 1;
			}
			unsigned int& px = _data[y * width + x];
			if (m)
			{
				//px = alpha_blend(px, c);
				px_blend(&px, c);
			}
			else
			{
				px = c;
			}
		}
	public:
		// gif更新
		int get_gif_idx()
		{
			LOCK_W(_lk_gif);
			if (_delays.size())
			{
				int idx = std::min(std::max(_cur_idx, 0), _layers - 1);
				int t = _giftime.elapsed();
				if (t > _delays[idx])
				{
					_cur_idx++;
					if (_cur_idx > _layers - 1)
					{
						_cur_idx = 0;
					}
					_giftime.reset();
				}
				else
				{
					t = t;
				}
			}
			else
			{
				_cur_idx = 0;
			}
			return _cur_idx;
		}
		void reset_gif_idx()
		{
			LOCK_W(_lk_gif);
			_cur_idx = 0;
			_giftime.reset();
		}
		glm::vec4 get_idx_rect(int idx = -1)
		{
			glm::vec4 rct;
			if (idx < 0)
			{
				idx = get_gif_idx();
			}
			LOCK_R(_locks);
			idx = std::min(idx, _layers - 1);
			rct.x = 0;
			rct.w = height / _layers;
			rct.y = idx * rct.w;
			rct.z = width;
			return rct;
		}

		void set_rect(glm::ivec4 rc)
		{
			LOCK_W(_locks);
			_prc = rc;
		}
		glm::ivec4 get_rect()
		{
			LOCK_R(_locks);
			return _prc;
		}
	public:
		std::vector<int> getHog(int xi = 4, int s = 16)
		{
			int c = s / xi;
			std::vector<int> ret;
			for (int y = 0; y < c; y++)
			{
				for (int x = 0; x < c; x++)
				{
					ret.push_back(getHA({ x * xi,y * xi,xi,xi }));
				}
			}
			return ret;
		}
		int getHA(glm::ivec4 rc)
		{
			int ret = 0;
			rc.z += rc.x; rc.w += rc.y;
			for (int y = rc.y; y < rc.w && y < height; y++)
			{
				for (int x = rc.x; x < rc.z && x < width; x++)
				{
					size_t idx = x + y * width;
					if (idx < _data.size())
					{
						if (_data[idx])
						{
							ret++;
						}
					}
				}
			}
			return ret;
		}
		yHist* getYHist(int c = 256)
		{
			_yh.init(c);
			for (auto it : _data)
			{
				int itR = GetRValue(it);
				int itG = GetGValue(it);
				int itB = GetBValue(it);
				//int v = (itR * 299 + itG * 587 + itB * 114);// < 576000 ? 0 : 255;
				int v = ((itR + itG + itB) / 3 < 127) ? 0 : 255;
				_yh.Hist(v);
			}
			return &_yh;
		}
		void Thresholding(unsigned int black = 0xff000000, unsigned int white = 0xffffffff)
		{
			for (auto& it : _data)
			{
				int itR = GetRValue(it);
				int itG = GetGValue(it);
				int itB = GetBValue(it);
				unsigned int v = ((itR + itG + itB) / 3 < 127) ? black : white;
				it = v;
			}
		}
		std::vector<Image*> split()
		{
			Image* imgs = getMinBox();
			std::vector<Image*> ret;
			std::vector<int > vs;
			int rx = 0;
			for (;;)
			{
				auto xx = imgs->getXm(rx);
				if (xx > 0)
				{
					//vs.push_back(xx);
					int w = xx - rx + 1;
					ret.push_back(imgs->getBox({ rx,0,w,imgs->height })->reMinBox());
					rx = imgs->getXf(xx + 1);
				}
				else
				{
					ret.push_back(imgs->getBox({ rx,0,imgs->width - rx + 1,imgs->height })->reMinBox());
					break;
				}
			}
			return ret;
		}

		Image* getBox(const glm::ivec4& rc)
		{
			Image* img = create_null(rc.z, rc.w);
			img->copyImage(this, { 0,0 }, { rc.x,rc.y,img->width,img->height });
			return img;
		}

		Image* getMinBox()
		{
			glm::ivec4 ret = { getXf(),getXs(),getYf(),getYs() };
			Image* img = create_null(ret.y - ret.x + 1, ret.w - ret.z + 1);
			img->copyImage(this, { 0,0 }, { ret.x,ret.z,img->width,img->height });
			return img;
		}
		Image* reMinBox()
		{
			Image* mib = getMinBox();
			copy(mib);
			destroy(mib);
			return this;
		}
	public:
		int getXm(int b)
		{
			for (int x = b; x < width; x++)
			{
				unsigned int c = 0;
				for (int y = 0; y < height; y++)
				{
					size_t idx = x + y * width;
					if (idx < _data.size())
					{
						if (_data[idx])
						{
							c = 1;
							break;;
						}
					}
				}
				if (c == 0)
				{
					return x;
				}
			}
			return -1;
		}
		int getXf(int b = 0)
		{
			for (int x = b; x < width; x++)
			{
				for (int y = 0; y < height; y++)
				{
					size_t idx = x + y * width;
					if (idx < _data.size())
					{
						unsigned int c = _data[idx];
						if (c)
						{
							return x;
						}
					}

				}
			}
			return 0;
		}
		int getXs(int eb = 0)
		{
			for (int x = width - 1; x >= eb; x--)
			{
				for (int y = 0; y < height; y++)
				{
					size_t idx = x + y * width;
					if (idx < _data.size())
					{
						if (_data[idx])
						{
							return x;
						}
					}
				}
			}
			return 0;
		}
		int getYf(int b = 0)
		{
			for (int y = b; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					size_t idx = x + y * width;
					if (idx < _data.size())
					{
						if (_data[idx])
						{
							return y;
						}
					}
				}
			}
			return 0;
		}
		int getYs(int eb = 0)
		{
			for (int y = height - 1; y >= eb; y--)
			{
				for (int x = 0; x < width; x++)
				{
					size_t idx = x + y * width;
					if (idx < _data.size())
					{
						if (_data[idx])
						{
							return y;
						}
					}
				}
			}
			return 0;
		}
	public:
#ifndef M_RES
		int64_t get_upt()
		{
			return _upt;
		}
		void set_update()
		{
			_upt++;
		}
#endif // !M_RES


		//LOCK_W(_lock_upt);
		//_uptime = Timer::get_micro();//微秒
		void copy(Image* src)
		{
			if (!(src->width * src->height > 0))
			{
				return;
			}
			LOCK_W(_locks);
			resize(src->width, src->height);
			components = src->components;
			size_t len = src->datasize();
			memcpy(_data.data(), src->data(), len);
			set_update();
		}
		void copyImage(Image* src, const glm::vec2& dst, const glm::vec4& rc, unsigned int col = -1)
		{
			if (src)
			{
				LOCK_W(_locks);
				DrawBmp(src, rc.x, rc.y, rc.z, rc.w, dst.x, dst.y, col);

				set_update();
			}
		}
		std::wstring AtoW(const std::string str)
		{
			if (hz::hstring::IsTextUTF8(str.c_str()))
				return hz::jsonT::utf8_to_wstring(str);
			else
				return hz::jsonT::ansi_to_wstring(str);
		}
	public:
		void draw_line(const glm::ivec4& p, unsigned int color)
		{
			int StartX = p.x, StartY = p.y, EndX = p.z, EndY = p.w;
			//////////////////////////////////////////////////////
			// Draws a line using the Bresenham line algorithm
			// Thanks to Jordan DeLozier <JDL>
			//////////////////////////////////////////////////////
			int jlen = 2;

			int x1 = StartX;
			int y1 = StartY;
			int x = x1;                       // Start x off at the first pixel
			int y = y1;                       // Start y off at the first pixel
			int x2 = EndX;
			int y2 = EndY;
			int xinc1, xinc2, yinc1, yinc2;      // Increasing values
			int den, num, numadd, numpixels;
			int deltax = abs(x2 - x1);        // The difference between the x's
			int deltay = abs(y2 - y1);        // The difference between the y's
											  // Get Increasing Values
			if (x2 >= x1) {                // The x-values are increasing
				xinc1 = 1; xinc2 = 1;
			}
			else {                         // The x-values are decreasing
				xinc1 = -1; xinc2 = -1;
			}

			if (y2 >= y1) {                // The y-values are increasing
				yinc1 = 1; yinc2 = 1;
			}
			else {                         // The y-values are decreasing
				yinc1 = -1; yinc2 = -1;
			}

			// Actually draw the line
			if (deltax >= deltay)         // There is at least one x-value for every y-value
			{
				xinc1 = 0;                  // Don't change the x when numerator >= denominator
				yinc2 = 0;                  // Don't change the y for every iteration
				den = deltax;
				num = deltax / jlen;
				numadd = deltay;
				numpixels = deltax;         // There are more x-values than y-values
			}
			else                          // There is at least one y-value for every x-value
			{
				xinc2 = 0;                  // Don't change the x for every iteration
				yinc1 = 0;                  // Don't change the y when numerator >= denominator
				den = deltay;
				num = deltay / jlen;
				numadd = deltax;
				numpixels = deltay;         // There are more y-values than x-values
			}

			for (int curpixel = 0; curpixel <= numpixels; curpixel++)
			{
				// Draw the current pixel
				setPixeColor(x, y, color);

				num += numadd;              // Increase the numerator by the top of the fraction
				if (num >= den)             // Check if numerator >= denominator
				{
					num -= den;               // Calculate the new numerator value
					x += xinc1;               // Change the x as appropriate
					y += yinc1;               // Change the y as appropriate
				}
				x += xinc2;                 // Change the x as appropriate
				y += yinc2;                 // Change the y as appropriate
			}

		}
		void draw_rect(const glm::ivec4& r, unsigned int col, unsigned int fill, float rounding = 0.0f,
			int rounding_corners_flags = ~0, float thickness = 1.0f)
		{
			glm::vec2 a = { r.x, r.y }; glm::vec2 b = { r.x + r.z,r.y + r.w };
			// 			Path p;
			// 			p.PathRect(a + glm::vec2(0.5f, 0.5f), b - glm::vec2(0.5f, 0.5f), rounding, rounding_corners_flags);
			// 			p.PathStroke(col, true, thickness);
			if (fill)
			{
				for (int y = a.y; y < b.y; y++)
				{
					for (int x = a.x; x < b.x; x++)
					{
						size_t idx = x + y * width;
						if (idx < _data.size())
						{
							_data[idx] = fill;
						}
					}
				}
			}
			if (col)
			{
				draw_line({ a.x, a.y, b.x, a.y }, col);
				draw_line({ b.x, a.y, b.x, b.y }, col);
				draw_line({ b.x, b.y, a.x, b.y }, col);
				draw_line({ a.x, a.y, a.x, b.y }, col);
			}
		}
		void AddTriangle(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, unsigned int col, unsigned int fill, float thickness = 1.0f, bool aa = true)
		{
			// 			if ((fill & HZ_COL32_A_MASK))
			// 				PathFillConvex(fill, false, aa);
			if ((col & HZ_COL32_A_MASK))
			{
				draw_line({ a,b }, col);
				draw_line({ b,c }, col);
				draw_line({ c,a }, col);
			}
		}
		// Render a triangle to denote expanded/collapsed state
		void addDirTriangle(const glm::vec2 s, glm::vec2 pos, GuiDir dir, unsigned int fill, unsigned int color = 0)
		{
			float r = 1.0, l = 0.0;
			glm::vec2 a, b, c;
			switch (dir)
			{
			case GuiDir_Up:
			case GuiDir_Down:
				if (dir == GuiDir_Up) {
					r = 0;
					l = 1;
				}
				a = glm::vec2(s.x * 0.5, s.y * r);
				b = glm::vec2(0.0, s.y * l);
				c = glm::vec2(s.x, s.y * l);
				break;
			case GuiDir_Left:
			case GuiDir_Right:
				if (dir == GuiDir_Left) {
					r = 0;
					l = 1;
				}
				a = glm::vec2(s.x * r, s.y * 0.5);
				b = glm::vec2(s.x * l, 0.0);
				c = glm::vec2(s.x * l, s.y);
				break;
			case GuiDir_None:
			case GuiDir_Count_:
				break;
			}
			if (dir & GuiDir_bevel)
			{
				if (dir & GuiDir_Left || dir & GuiDir_Right)
				{
					if (dir & GuiDir_Left) {
						r = 0;
						l = 1;
					}
					a = glm::vec2(0, 0);
					b = glm::vec2(s.x * r, s.y * l);
					c = glm::vec2(s.x, s.y);
				}
				else if (dir & GuiDir_Up || dir & GuiDir_Down)
				{
					if (dir & GuiDir_Up) {
						r = 0;
						l = 1;
					}
					a = glm::vec2(s.x * r, s.y * r);
					b = glm::vec2(s.x * r, s.y * l);
					c = glm::vec2(s.x * l, s.y * r);
				}
			}
			AddTriangle(pos + a, pos + b, pos + c, color, fill, color ? 1 : 0, true);
		}
	public:
		enum
		{
			E_BMP = 1,
			E_JPG = 2,
			E_PNG = 3,
			E_GIF = 4,

		};
		static   int istupian(char* entry, int len = 0)
			/*返加值为
			bmp;
			jpg;
			png;
			gif;
			*/
		{

			unsigned short  BMP = 0x4D42,
				JPG = 0xD8FF,
				PNG[4] = { 0x5089, 0x474E, 0x0A0D, 0x0A1A },
				GIF[3] = { 0x4947, 0x3846, 0x6139 };

			FILE* fis;
			short int i = 0;
			unsigned short pis[5];
			int flag = 0;//每次调用都进行初始化
			if (len == 0)
			{
				if ((fis = fopen(entry, "rb")) == NULL)
				{
					printf("can not read %s\n", entry);
					return 0;
				}
				fread(pis, 8, 1, fis);
				fclose(fis);
			}
			else if (len > 8)
			{
				memcpy(pis, entry, 8);
			}
			if (pis[0] == BMP)
			{
				flag = E_BMP;
				//printf("it is a bmp\n");
			}
			else if (pis[0] == JPG)
			{
				flag = E_JPG;
				//printf("it is a jpg\n");
			}
			else if (PNG[0] == pis[0] && PNG[1] == pis[1] && PNG[2] == pis[2] && PNG[3] == pis[3])
			{
				flag = E_PNG;
				//printf("it is a png\n");
			}
			else if (GIF[0] == pis[0] && GIF[1] == pis[1] && GIF[2] == pis[2])
			{
				flag = E_GIF;
				//printf("it is a gif\n");
			}

			return flag;
		}

		void imagetype()
		{
			/*
						int img_type = istupian((char*)pixelData, d_size);
						switch (img_type)
						{
						case E_BMP:
						{
						}break;
						case E_JPG:
						{
						}break;
						case E_PNG:
						{
						}break;
						case E_GIF:
						};*/
		}
		bool load3Dtexture(const std::string& filename)
		{
#ifdef _GLI_HEAD_
			std::vector<char> td;
			File::read_binary_file(filename, td);
			if (td.empty())
			{
				return false;
			}
			gli::texture2d_array tex2DArray(gli::load(td.data(), td.size()));
			if (!tex2DArray.empty())
			{
				resize(static_cast<uint32_t>(tex2DArray.extent().x), static_cast<uint32_t>(tex2DArray.extent().y));
				layerCount = static_cast<uint32_t>(tex2DArray.layers());
				mipLevels = static_cast<uint32_t>(tex2DArray.levels());
				size_t imgsize = tex2DArray.size();
				_arrayimg.resize(imgsize);
				memcpy(&_arrayimg[0], tex2DArray.data(), static_cast<size_t>(tex2DArray.size()));
				// Setup buffer copy regions for each layer including all of it's miplevels

				size_t offset = 0;
				bufferCopyRegions.clear();
				clearArray();
				char* tem = (char*)& _arrayimg[0];
				std::string fn = File::getPath(filename.c_str(), File::pathdrive | File::pathdir | File::pathfname);
				for (uint32_t layer = 0; layer < layerCount; layer++)
				{
					for (uint32_t level = 0; level < mipLevels; level++)
					{
						std::vector<unsigned int>* img = new std::vector<unsigned int>;
						_array.push_back(img);
						BufferImageCopy bufferCopyRegion = {};
						bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
						bufferCopyRegion.imageSubresource.mipLevel = level;
						bufferCopyRegion.imageSubresource.baseArrayLayer = layer;
						bufferCopyRegion.imageSubresource.layerCount = 1;
						bufferCopyRegion.imageExtent.width = static_cast<uint32_t>(tex2DArray[layer][level].extent().x);
						bufferCopyRegion.imageExtent.height = static_cast<uint32_t>(tex2DArray[layer][level].extent().y);
						bufferCopyRegion.imageExtent.depth = 1;
						bufferCopyRegion.bufferOffset = offset;

						bufferCopyRegions.push_back(bufferCopyRegion);
						img->resize(bufferCopyRegion.imageExtent.width * bufferCopyRegion.imageExtent.height);
						memcpy(img->data(), tem + offset, bufferCopyRegion.imageExtent.width * bufferCopyRegion.imageExtent.height * 4);
						savefilename = fn + "/array_" + std::to_string(layer) + "_" + std::to_string(level) + ".png";

						//stbi_write_png_to_func(png_write_func, this, bufferCopyRegion.imageExtent.width, bufferCopyRegion.imageExtent.height, 4, img->data(), 0);
						// Increase offset into staging buffer for next level / face
						offset += tex2DArray[layer][level].size();

					}
				}
				return true;
			}
#endif
			return false;
		}
		void saveImageArray(const std::string& filename, int w, int h)
		{
			for (size_t i = 0; i < _array.size(); i++)
			{
				std::string pngn = filename + "/array" + std::to_string(i) + ".png";
				stbi_write_png_to_func(png_write_func, this, (int)w, (int)h, 4, _array[i]->data(), 0);
			}
		}
	public:
		void setPixeColor(size_t x, size_t y, unsigned int c)
		{
			size_t idx = x + y * width;
			if (idx < _data.size())
			{
				_data[idx] = c;
			}
		}
		unsigned int getPixeColor(size_t x, size_t y)
		{
			size_t idx = x + y * width;
			return (idx < _data.size()) ? _data[idx] : 0;
		}
		void clear_color(unsigned int c = 0)
		{
			LOCK_W(_locks);
			if (c)
			{
				for (size_t i = 0; i < _data.size(); i++)
				{
					_data[i] = c;
				}
			}
			else
			{
				memset(_data.data(), c, _data.size() * sizeof(unsigned int));
			}
			//set_update();
		}
		void clearColor(unsigned int c = 0)
		{
			clear_color(c);
			return;
			LOCK_W(_locks);
			//if (!c)
			{
				memset(_data.data(), c, _data.size() * sizeof(unsigned int));

				//set_update();
			}
			//else
			//{
			//	for (size_t h = 0; h < height; h++)
			//	{
			//		for (size_t w = 0; w < width; w++)
			//		{
			//			_data[w*h * sizeof(unsigned int)] = c;
			//		}
			//	}
			//}
		}
		size_t resize(size_t w, size_t h, size_t z = 1)
		{
			if (w != width || h != height || _layers != z)
			{
				if (z < 1)
				{
					z = 1;
				}
				if (z > 1)
				{
					//width *= z;
					h *= z;
				}
				if (h > 10240)
				{
					//h = 10240;
				}
				//if (w > 0 && h > 0)
				_data.resize(w * h);

				//else
				//	_data.clear();
				width = w;
				height = h;
			}
			return _data.size();
		}
		void clearArray()
		{
			for (auto it : _array)
			{
				delete it;
			}
			_array.clear();
		}
		void ClipImage(size_t w, size_t h, size_t ox, size_t oy)
		{
			if (w == width && h == height)
			{
				return;
			}
			//if (w > 102400 || h > 102400)return;
			do {
				LOCK_W(_locks);
				//std::vector<unsigned int> tem;
				//tem.swap(_data);
				size_t ow = width, oh = height;
				size_t tw = width + ox, th = height + oy;
				/*if (w < tw)
				{
					w = tw;
				}*/
				if (h > th)
				{
					h = th;
				}
				resize(width, h);
				/*
				resize(w, h);
				unsigned int* dst = _data.data();
				unsigned int* src = tem.data();
				int y;
				if (h == 16)
				{
					h = h;
				}
				int sc = 0;
				//for (y = 0; y < h && y < oh; y++)
				//{
				//	sc = ow * (y + oy) + ox;
				//	auto sp = &src[sc];//(ox + ow) * (y + oy)
				//	auto dp = &dst[w * y];
				//	//memcpy(dp, sp, w * requested_components);
				//}
				unsigned int* ps = 0;
				unsigned int* pd = 0;
				for (y = 0; y < h; y++)
				{
					int dx = 0;
					{
						ps = &src[(ow * (y + oy) + ox)];
						pd = &dst[(width * y + dx)];
						memcpy(pd, ps, sizeof(unsigned int) * width);
					}
				}

				if (w)
				{
					w = w;
				}*/
			} while (0);
			set_update();
		}

		void copy_value(Image* src, const glm::ivec4& src_rect, const glm::ivec4& dest_rect)
		{
			if (src_rect.x > src_rect.z || src_rect.x > src_rect.w)return;
			LOCK_W(_locks);
#if 1
			draw_image2(src, src_rect, { dest_rect.x, dest_rect.y });
#else
			int src_width = src_rect.z, src_height = src_rect.y + src_rect.w;
			if (src_rect.z == 0)
				src_width = src->width;
			if (src_rect.w == 0)
				src_height = src->height;

			unsigned int* pdest = _data.data(), * psrc = (unsigned int*)src->data();
			int dy = dest_rect.y;
			int y = src_rect.y;

			if (y < 0)y = 0;
			if (dy < 0)dy = 0;

			unsigned int* ps = 0;
			unsigned int* pd = 0;
			for (; y < src_height; y++, dy++)
			{
				int dx = dest_rect.x;
#if 1
				for (int x = src_rect.x; x < src_width; x++, dx++)
				{
					if (dx < 0 || x < 0)
					{
						continue;
					}
					ps = &psrc[(src->width * y + x)];
					pd = &pdest[(width * dy + dx)];
					//*pd = *ps;
					px_blend(pd, *ps);
				}
#else
				int x = src_rect.x;
				{
					ps = &psrc[(src->width * y + x)];
					pd = &pdest[(width * dy + dx)];
					memcpy(pd, ps, sizeof(unsigned int) * src_width);
				}
#endif
			}
#endif
			set_update();
			return;
		}

		void lcd2rgba(byte* a, int W, int H)
		{
			byte* b = (byte*)_data.data();
			byte* end = b + (W * H * 4);
			int x = 0;
			int z = W & 3;
			while (1) {
				if (b >= end) break;
				b[0] = a[2]; // reverse RGB
				b[1] = a[1];
				b[2] = a[0];
				b[3] = 0;
				a += 3;
				b += 4;
				x++;
				if (x == W) {
					x = 0;
					a += z;
				}
			}
		}
		void copy_lcd_to_image(unsigned char* bit, int pitch, glm::ivec4 rct, bool rgb = false)
		{
			unsigned int* expanded_data = 0;

			int w = rct.z / 3, h = rct.w, posx = rct.x, posy = rct.y;
			int mxw = posx + w;
			int mxh = posx + h;
			int rows = h, j;
			glm::ivec2 outsize = { width, height };
			unsigned int* bdata = _data.data();
			int r = rgb ? 0 : 2, b = rgb ? 2 : 0;
			//if (pixel_mode == 5)				/*lcd灰度图*/
			{
				for (j = 0; j < h && (j + posy) < outsize.y; j++)
				{
					auto pj = pitch * j;
					UCHAR* pixel = bit + pj;
					auto jp = j + posy;
					int64_t psy = (jp * outsize.x);
					if (psy < 0 || jp >= height)
					{
						continue;
					}
					expanded_data = bdata + psy;
					unsigned int* dc = (unsigned int*)expanded_data;
					for (int i = 0; (i < w) && ((i + posx) < outsize.x); i++)
					{
						unsigned int uc = 0;
						unsigned char* pc = (unsigned char*)& uc;
						pc[r] = *pixel++; //r	
						pc[1] = *pixel++; //g	
						pc[b] = *pixel++; //b
						int a = pc[0] + pc[1] + pc[1];
						a = (a < 0) ? 0 : ((a > 255) ? 255 : ((int)a));
						if (a > 0)
						{
							pc[3] = 255; //a
							px_blend(&dc[i + posx], uc);
							//auto pd = &dc[i + posx];
							//*pd = alpha_blend(*pd, uc);
							//dc[i + posx] = alpha_blend(dc[i + posx], uc);
							//dc[i + posx] = layer_overlay(dc[i + posx], uc);
							//dc[i + posx] = layer_multiply(dc[i + posx], uc);
						}
					}
				}
			}
		}
		void copy_to_image(unsigned char* bit, int pitch, glm::ivec4 rct, unsigned int color, int pixel_mode, float brightness = 0, bool isblend = false)
		{
			unsigned int* expanded_data = 0;

			int w = rct.z, h = rct.w, posx = rct.x, posy = rct.y;
			int mxw = posx + w;
			int mxh = posx + h;
			int rows = h, j;
			if (_data.empty())
				resize(w, h);
			glm::ivec2 outsize = { width, height };
			color &= 0x00ffffff;
			unsigned int* bdata = _data.data();
			if (pixel_mode == 2)				/*256灰度图*/
			{
				for (j = 0; j < h && (j + posy) < outsize.y; j++)
				{
					auto pj = pitch * j;
					UCHAR* pixel = bit + pj;
					auto jp = j + posy;
					int64_t psy = (jp * outsize.x);
					if (psy < 0 || jp >= height)
					{
						continue;
					}
					expanded_data = bdata + psy;
					unsigned int* dc = (unsigned int*)expanded_data;
					for (int i = 0; (i < w) && ((i + posx) < outsize.x); i++)
					{
						unsigned char c = pixel[i];
						if (c)
						{
							unsigned int uc = 0, ut = std::min(255.0f, brightness * c + c);
#ifndef UF_COLOR
							unsigned char* pc = (unsigned char*)& uc;
							pc[3] = ut; //a	
							uc |= color;
#else
							//uc = multiply_alpha(color, ut);
							//uc = set_alpha(color, ut > 0 ? 255 : 0);
							uc = set_alpha(color, ut);
#endif // UF_COLOR
							//auto pd = &dc[i + posx];
							//*pd = alpha_blend(*pd, uc);
							if (isblend)
							{
								px_blend(&dc[i + posx], uc);
							}
							else {
								dc[i + posx] = uc;
							}

						}
					}
				}
			}
			else if (pixel_mode == 1)			/*单色位图1位*/
			{
				for (j = 0; j < h && (j + posy) < outsize.y; j++)
				{
					auto jp = j + posy;
					int64_t psy = (jp * outsize.x);
					if (psy < 0 || jp >= height)
					{
						continue;
					}
					UCHAR* pixel = bit + pitch * j;
					expanded_data = bdata + psy;
					unsigned int* dc = (unsigned int*)expanded_data;
					for (int i = 0; (i < w) && ((i + posx) < outsize.x); i++)
					{
						unsigned char c = (pixel[i / 8] & (0x80 >> (i & 7))) ? 255 : 0;
						if (c)
						{
							unsigned int uc = 0;
							unsigned char* pc = (unsigned char*)& uc;
							pc += 3;
							//*pc++ = 255 - c;	//r
							//*pc++ = 255 - c;	//g
							//*pc++ = 255 - c;	//b
							*pc = c;			//a
							uc |= color;
							//auto pd = &dc[i + posx];
							//*pd = alpha_blend(*pd, uc);
							if (isblend)
							{
								px_blend(&dc[i + posx], uc);
							}
							else {
								dc[i + posx] = uc;
							}
						}
					}
				}
			}
			else if (pixel_mode == 4)//FT_PIXEL_MODE_BGRA
			{
				memcpy(expanded_data, bit, pitch * rows * 4);
			}
			else
			{
				return;
			}
		}
	public:
		//0xaabbggrr
		//0xrrggbbaa
#define RGBA2ABGR( _COLOR ) (_COLOR << 24 |_COLOR >> 24 | _COLOR << 8 & 0x00ff0000 | _COLOR >> 8 & 0x0000ff00)
#define RGBA2BGRA( c ) ((c &0xff000000) | (c&0x0000ff00) | (c >> 16 & 0x000000ff) | (c<<16&0x00ff0000))
#define cbgra2rgba(c) (c<<16&0xff000000 | c&0xff0000 | c>>16&0xff00 | c&0xff)
		void bgra2rgba()
		{
			//ff800000
			//aa00ff00
			//ffaabbcc
			//ffccbbaa
			unsigned int c = 0x80ff0000, b = 0;
			b = RGBA2BGRA(c);
			for (auto& it : _data)
			{
				//if (it)
				{
					it = RGBA2BGRA(it);
					//	b = it;
				}
			}
		}
	public:
		bool loadImage(std::string imgfn)
		{
			bool ret = GetImageData(imgfn.c_str());
			return ret;
		}
		bool loadMemImage(const char* data, size_t len)
		{
			bool ret = GetImageData(data, len);
			return ret;
		}
#if 1
		/*
			.hdr.bmp可选comp通道数
			quality为jpeg品质，1-100

		*/
		int saveImage(std::string fn, int comp = 4, int quality = 100)
		{
			if (_data.empty())
				return -1;
			LOCK_R(_locks);
			std::string ext = File::getPath(fn.c_str(), File::pathext);
			if (fn.find(":") == std::string::npos)
			{
				fn = File::getAP(fn);
			}

			if (ext == ".bmp")
			{
				if (comp == 4)
					comp = 3;
				getCompdata(comp);
				void* d = temp_data.empty() ? (void*)& _data[0] : (void*)& temp_data[0];
				return stbi_write_bmp(fn.c_str(), (int)width, (int)height, comp, d);
			}
			if (ext == ".tga")
			{
				return stbi_write_tga(fn.c_str(), (int)width, (int)height, requested_components, &_data[0]);
			}
			if (ext == ".jpg")
			{
				return stbi_write_jpg(fn.c_str(), (int)width, (int)height, requested_components, &_data[0], quality);
			}
			if (ext == ".hdr")
			{
				temp_data.resize(width * height * comp * sizeof(float));
				float* t = (float*)& temp_data[0];
				for (size_t y = 0; y < height; ++y)
				{
					for (size_t x = 0; x < width; ++x)
					{
						unsigned int c = _data[y * width + x];
						unsigned int R = GetRValue(c), G = GetGValue(c), B = GetBValue(c), A = c >> 24;
						*t++ = R / 255.0f;
						*t++ = G / 255.0f;
						*t++ = B / 255.0f;
						if (comp == 4)
							* t++ = A / 255.0f;
					}
				}
				return stbi_write_hdr(fn.c_str(), (int)width, (int)height, comp, (float*)& temp_data[0]);
			}
			savefilename = fn;
#ifdef LIBPNG
			if (ext == ".png")
			{
				hz::ImageLibpng ipng;
			}
			//return to_rgba();
#endif
			return stbi_write_png_to_func(png_write_func, this, (int)width, (int)height, comp, &_data[0], 0);
			//return stbi_write_png(fn.c_str(), (int)width, (int)height, requested_components, &_data[0], 0);
		}
#endif
		std::string png_data()
		{
			if (_data.empty())
				return "";
			std::string ret;
			int err = stbi_write_png_to_func(png_write_buf_func, &ret, (int)width, (int)height, 4, &_data[0], 0);
			return ret;
		}
		void getCompdata(int comp)
		{

			temp_data.clear();
			if (comp > 3 || comp < 1 || comp == 2)
				return;
			temp_data.resize(width * height * comp);
			unsigned char* t = &temp_data[0];
			for (size_t y = 0; y < height; ++y)
			{
				for (size_t x = 0; x < width; ++x)
				{
					unsigned int c = _data[y * width + x];
					if (comp == 1)
					{
						unsigned int R = GetRValue(c), G = GetGValue(c), B = GetBValue(c);
						c = (R * 38 + G * 75 + B * 15) >> 7;
					}
					memcpy(t, &c, comp);
					t += comp;
				}
			}
		}
		static void png_write_func(void* context, void* data, int size)
		{
			if (context)
			{
				Image* p = (Image*)context;
				File::save_binary_file(p->savefilename, (char*)data, size);
			}
		}

		static void png_write_buf_func(void* context, void* data, int size)
		{
			if (context)
			{
				std::string* p = (std::string*)context;
				p->assign((char*)data, size);
			}
		}

#if 0
		bool to_rgb(void)
		{
			FILE* fp = fopen(savefilename.c_str(), "wb");
			if (fp)
			{
				std::vector<unsigned char> rgb;
				rgb.resize(width * height * 3);
				unsigned char* p = rgb.data();
				for (auto& it : _data)
				{
					*p++ = GetRValue(it);
					*p++ = GetGValue(it);
					*p++ = GetBValue(it);
				}
				svpng(fp, (int)width, (int)height, ((unsigned char*)& rgb[0]), 0);
				fclose(fp);
			}
			return fp ? true : false;
		}

		bool to_rgba(void) {
			FILE* fp = fopen(savefilename.c_str(), "wb");
			if (fp)
			{
				svpng(fp, (int)width, (int)height, ((unsigned char*)& _data[0]), 1);
				fclose(fp);
			}
			return fp ? true : false;
		}

		void test_rgb(void) {
			unsigned char rgb[256 * 256 * 3], * p = rgb;
			unsigned x, y;
			FILE* fp = fopen("rgb.png", "wb");
			for (y = 0; y < 256; y++)
				for (x = 0; x < 256; x++) {
					*p++ = (unsigned char)x;    /* R */
					*p++ = (unsigned char)y;    /* G */
					*p++ = 128;                 /* B */
				}
			svpng(fp, 256, 256, rgb, 0);
			fclose(fp);
		}

		void test_rgba(void) {
			unsigned char rgba[256 * 256 * 4], * p = rgba;
			unsigned x, y;
			FILE* fp = fopen("rgba.png", "wb");
			for (y = 0; y < 256; y++)
				for (x = 0; x < 256; x++) {
					*p++ = (unsigned char)x;                /* R */
					*p++ = (unsigned char)y;                /* G */
					*p++ = 128;                             /* B */
					*p++ = (unsigned char)((x + y) / 2);    /* A */
				}
			svpng(fp, 256, 256, rgba, 1);
			fclose(fp);
		}

#endif // 0
	public:
		// 读锁
		void lock_r()
		{
			_locks.lock_shared();
		}
		void unlock_r()
		{
			_locks.unlock_shared();
		}
		// 写锁
		void lock_w()
		{
			_locks.lock();
		}
		void unlock_w()
		{
			_locks.unlock();
		}
	public:
		~Image()
		{
			clearArray();
		}
		void clear()
		{
			LOCK_W(_locks);
			resize(0, 0);
		}
		size_t getWidth()
		{
			return width;
		}
		size_t getHeight()
		{
			return height;
		}
		size_t datasize()
		{
			return _data.size() * (size_t)sizeof(unsigned int);
		}

		size_t size()
		{
			return _data.size() * (size_t)sizeof(unsigned int);
		}

		size_t datacount()
		{
			return _data.size();
		}
		unsigned int* data()
		{
			return &_data[0];
		}
		unsigned int* getRow(size_t idx)
		{
			return &_data.data()[idx * width];
		}
		unsigned int* data_array()
		{
			return _arrayimg.data();
		}
		/// Create a view of the image identified by Level in the mipmap chain of the texture.
		item operator[](size_t Level) const
		{
			return _levels_info[Level];
		}
	public:
		void Horizontal()//水平翻转
		{
			unsigned int* ps = (unsigned int*)_data.data();
			int y = 0;
			for (y = 0; y < height; y++, ps += width)
			{
				unsigned int* pd = ps + width - 1;
				for (size_t x = 0; x < width / 2; x++, --pd)
				{
					unsigned int t = ps[x];
					ps[x] = *pd;
					*pd = t;
				}
			}
			set_update();
		}
		void Vertical()//垂直翻转
		{
			unsigned int* ps = 0;
			unsigned int* pd = 0;
			unsigned int* pixelData = _data.data();
			int y = 0;
			for (size_t x = 0; x < width; x++)
			{
				ps = (unsigned int*)(pixelData + x);
				pd = (unsigned int*)(pixelData + (x + (height - 1) * width));
				for (y = 0; y < height / 2; y++, ps += width, pd -= width)
				{
					unsigned int t = *ps;
					*ps = *pd;
					*pd = t;
				}
			}

			set_update();
		}
		void draw_image2(Image* src, glm::ivec4 st, glm::ivec2 dstpos, unsigned int col = -1)
		{
			const unsigned int* pBmp = src->data();
			RECT srcRect = { st.x, st.y, st.x + st.z - 1, st.y + st.w - 1 };
			RECT dstRect = { dstpos.x, dstpos.y, 0, 0 };

			// 边界校正
			{
				if (srcRect.right >= src->width)
					srcRect.right = src->width - 1;
				if (srcRect.bottom >= src->height)
					srcRect.bottom = src->height - 1;

				// 超出左边界
				if (dstRect.left < 0)
				{
					srcRect.left += -dstRect.left;
					dstRect.left = 0;
				}
				// 超出上边界
				if (dstRect.top < 0)
				{
					srcRect.top += -dstRect.top;
					dstRect.top = 0;
				}

				int visibleW = srcRect.right - srcRect.left + 1;
				int visibleH = srcRect.bottom - srcRect.top + 1;

				// 超出右边界
				if (dstRect.left + visibleW > width)
				{
					visibleW = width - dstRect.left;
					srcRect.right = srcRect.left + visibleW - 1;
				}
				// 超出下边界
				if (dstRect.top + visibleH > height)
				{
					visibleH = height - dstRect.top;
					srcRect.bottom = srcRect.top + visibleH - 1;
				}


				dstRect.right = dstRect.left + (srcRect.right - srcRect.left);
				dstRect.bottom = dstRect.top + (srcRect.bottom - srcRect.top);

				if (visibleW <= 0 || visibleH <= 0)	return;
			}

			const UINT* pSrc = pBmp + srcRect.top * src->width + srcRect.left;
			UINT* pDst = (UINT*)data() + dstRect.top * width + dstRect.left;

			ab32(pDst, width, pSrc, src->width, srcRect.right - srcRect.left + 1, srcRect.bottom - srcRect.top + 1, col);
		}
	private:
		// ************************************************************ //
		// GetImageData                                                 //
		//                                                              //
		// 从内存或文件加载图像数据解析									//
		// ************************************************************ //
		bool GetImageData(const char* filename, size_t len = 0)
		{
			LOCK_W(_locks);
			std::vector<char> file_data;
			std::string fn;
			if (len > 0)
			{
				file_data.resize(len);
				memcpy(&file_data[0], filename, len);
			}
			else
			{
				//hz::Timer t;
				fn = File::read_binary_file(filename, file_data);
				//int64_t s = t.elapsed();
				//LOGW("load %s file time: %lld ms%d\n", filename, s, (int)file_data.size());
				if (file_data.size() == 0 || fn.empty()) {
					return false;
				}
				_filename = filename;
			}
			int img_type = istupian((char*)& file_data[0], file_data.size());
			if (img_type == E_PNG)
			{
#ifdef LIBPNG
				hz::ImageLibpng ipng;
				ipng.load(&file_data);
				if (ipng.getHeight() && ipng.getWidth())
				{
					components = 4;
					resize(ipng.getWidth(), ipng.getHeight());
					memcpy(&_data[0], ipng.data(), ipng.datasize());
					return true;
		}
#endif
	}
			int tmp_width = 0, tmp_height = 0, tmp_z = 0, tmp_components = 0;
			int* delays = 0;
			unsigned char* image_data = 0;
			bool ret = false;
			if (img_type == E_GIF)
			{
				image_data = stbi_load_gif_from_memory(reinterpret_cast<unsigned char*>(&file_data[0]), static_cast<int>(file_data.size()),
					&delays, &tmp_width, &tmp_height, &tmp_z, &tmp_components, requested_components);
				_layers = tmp_z;
				_delays.resize(_layers);
				memcpy(_delays.data(), delays, sizeof(int) * _layers);
				stbi_image_free(delays);
				ret = tmp_z > 0;
			}
			else
			{
				image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(&file_data[0]), static_cast<int>(file_data.size()), &tmp_width, &tmp_height, &tmp_components, requested_components);
				if ((image_data == nullptr) ||
					(tmp_width <= 0) ||
					(tmp_height <= 0) ||
					(tmp_components <= 0)) {
					//std::cout << "Could not read image data!" << std::endl;
					return false;
				}
			}

			size_t size = (tmp_width) * (tmp_height) * (requested_components <= 0 ? tmp_components : requested_components);
			if (tmp_z > 0)
			{
				size *= tmp_z;
			}
			//width = tmp_width;
			//height = tmp_height;
			components = tmp_components;
			size_t bs = resize(tmp_width, tmp_height, tmp_z) * requested_components;
			memcpy(&_data[0], image_data, size);

			stbi_image_free(image_data);
			mipLevels = layerCount = 1;
			_levels_info.resize(mipLevels);
			_levels_info[0].width = width;
			_levels_info[0].height = height;
			_levels_info[0]._size = size;
			set_update();
			return true;
}

		void ab32(unsigned int* pDstBmp, int dst_width, const unsigned int* pSrcBmp, int src_width, int blend_width, int blend_height, unsigned int col)
		{
			// C实现
			{
				const int nextLineOffset_src = (src_width - blend_width);	// 混合完一行像素后，通过加上该值，便可直接定位到下行起始像素
				const int nextLineOffset_dst = (dst_width - blend_width);
				unsigned int* pSrc = (unsigned int*)pSrcBmp;
				unsigned int* pDst = pDstBmp;
				int below_A, below_R, below_G, below_B;
				int above_A, above_R, above_G, above_B;

				for (int h = 0, w = 0; h < blend_height; h++)
				{
					for (w = 0; w < blend_width; w++)
					{
						auto sc = *pSrc++;
						auto a = get_alpha(sc);
						if (a > 0)
						{
							if (col != -1)
							{
								sc = set_alpha(col, a);
							}
							//*pDst = sc;
							//*pDst = alpha_blend(*pDst, sc);
							px_blend(pDst, sc);
						}
						pDst++;
					}
					pSrc += nextLineOffset_src;
					pDst += nextLineOffset_dst;
				}
				return;
			}
		}
	public:
#ifndef CRDS
#define CRDS
		// Helpers macros to generate 32-bits encoded colors
#ifdef USE_BGRA_PACKED_COLOR
#define COL32_R_SHIFT    16
#define COL32_G_SHIFT    8
#define COL32_B_SHIFT    0
#define COL32_A_SHIFT    24
#define COL32_A_MASK     0xFF000000
#else
#define COL32_R_SHIFT    0
#define COL32_G_SHIFT    8
#define COL32_B_SHIFT    16
#define COL32_A_SHIFT    24
#define COL32_A_MASK     0xFF000000
#endif
#define F32_TO_INT8_UNBOUND(_VAL)    ((int)((_VAL) * 255.0f + ((_VAL)>=0 ? 0.5f : -0.5f)))   // Unsaturated, for display purpose 
#define F32_TO_INT8_SAT(_VAL)        ((int)(Saturate(_VAL) * 255.0f + 0.5f))               // Saturated, always output 0..255

#endif
		static inline float  Saturate(float f) { return (f < 0.0f) ? 0.0f : (f > 1.0f) ? 1.0f : f; }
		template <typename T>
		static inline T Clamp(T v, T mn, T mx) { return (v < mn) ? mn : (v > mx) ? mx : v; }

		glm::vec4 ColorConvertU32ToFloat4(unsigned int in)
		{
			float s = 1.0f / 255.0f;
			return glm::vec4(
				((in >> COL32_R_SHIFT) & 0xFF) * s,
				((in >> COL32_G_SHIFT) & 0xFF) * s,
				((in >> COL32_B_SHIFT) & 0xFF) * s,
				((in >> COL32_A_SHIFT) & 0xFF) * s);
		}

		unsigned int ColorConvertFloat4ToU32(const glm::vec4& in)
		{
			unsigned int out;
			out = ((unsigned int)F32_TO_INT8_SAT(in.x)) << COL32_R_SHIFT;
			out |= ((unsigned int)F32_TO_INT8_SAT(in.y)) << COL32_G_SHIFT;
			out |= ((unsigned int)F32_TO_INT8_SAT(in.z)) << COL32_B_SHIFT;
			out |= ((unsigned int)F32_TO_INT8_SAT(in.w)) << COL32_A_SHIFT;
			return out;
		}

		void AlphaBlend32(UINT* pDstBmp, int dst_width, const UINT* pSrcBmp, int src_width, int blend_width, int blend_height)
		{
#ifndef _ASM_AB
			// C实现
			{
				const int nextLineOffset_src = (src_width - blend_width) * 4;	// 混合完一行像素后，通过加上该值，便可直接定位到下行起始像素
				const int nextLineOffset_dst = (dst_width - blend_width) * 4;
				unsigned char* pSrc = (unsigned char*)pSrcBmp;
				unsigned char* pDst = (unsigned char*)pDstBmp;
				int below_A, below_R, below_G, below_B;
				int above_A, above_R, above_G, above_B;

				for (int h = 0, w = 0; h < blend_height; h++)
				{
					for (w = 0; w < blend_width; w++)
					{
						above_B = *pSrc++;
						above_G = *pSrc++;
						above_R = *pSrc++;
						above_A = *pSrc++;

						if (above_A == 0)
						{
							pDst += 4;
							continue;
						}

						below_B = *pDst;
						below_G = *(pDst + 1);
						below_R = *(pDst + 2);
						below_A = *(pDst + 3);

						*pDst++ = below_B - (below_B - above_B) * above_A / 255;
						*pDst++ = below_G - (below_G - above_G) * above_A / 255;
						*pDst++ = below_R - (below_R - above_R) * above_A / 255;

						if (below_A == 255)
							* pDst++ = 255;
						else
							*pDst++ = below_A - (below_A - above_A) * above_A / 255;
					}

					pSrc += nextLineOffset_src;
					pDst += nextLineOffset_dst;
				}
				return;
			}
#else
			const int nextLineOffset_src = (src_width - blend_width) * 4;	// 混合完一行像素后，通过加上该值，便可直接定位到下行起始像素
			const int nextLineOffset_dst = (dst_width - blend_width) * 4;

			__asm
			{
				mov			edi, pDstBmp; 目的像素
				mov			esi, pSrcBmp; 源像素
				xor ebx, ebx; 已混合的高度
				mov			ecx, blend_width; 要混合的宽度

				BLEND_BEGIN :
				cmp			dword ptr[esi], 0x00FFFFFF; 如果alpha为0, 则跳过混合部分
					jna			BLEND_END

					movd		mm0, [edi]; 把目的像素值移入mm0寄存器的低32位
					movd		mm1, [esi]; 把源像素值移入mm1寄存器的低32位

					; Core Begin：result = b - (b - a) * a_alpha / 255 （a为源像素分量, b为目的像素分量）
					pxor		mm2, mm2; ① 把MM2清0
					punpcklbw	mm0, mm2;    将mm0与mm2按字节交叉组合，存入mm0，mm0 = 0x00AA00BB00GG00RR
					punpcklbw	mm1, mm2;    将mm1与mm2按字节交叉组合，存入mm1，mm1 = 0x00AA00BB00GG00RR
					movq		mm3, mm1; ② mm3 = 0x00AA00BB00GG00RR
					punpckhwd	mm3, mm3;    将高32位按16位交错排列，mm3 = 0x00AA00AA00BB00BB
					punpckhdq	mm3, mm3;    将高32位按32位交错排列，mm3 = 0x00AA00AA00AA00AA
					movq		mm4, mm0; ③ mm4 = 目的像素 = 0x00AA00BB00GG00RR
					movq		mm5, mm1;    mm5 = 源像素 = 0x00AA00BB00GG00RR
					psubusw		mm4, mm1; ④ dst - src，按字饱和减，小于0为0
					psubusw		mm5, mm0;    src - dst，按字饱和减，小于0为0
					pmullw		mm4, mm3;    (dst - src)* alpha，若dst - src为0，则mm4为0
					pmullw		mm5, mm3;    (src - dst)* alpha，若src - dst为0，则mm5为0
					psrlw		mm4, 8; 按字右移8位，即除以256
					psrlw		mm5, 8; 按字右移8位，即除以256
					paddusw		mm0, mm5; 饱和加到原图象:D = Alpha * (O - S) + S，(src - dst) < 0 部分
					psubusw		mm0, mm4; 饱和加到原图象D = S - Alpha * (S - O)，(dst - src) > 0 部分
					packuswb	mm0, mm0; 按16位有符号数压缩为8位无符号数
					; Core End

					movd[edi], mm0; 混合结果写进目的像素

					BLEND_END :
				add			edi, 4
					add			esi, 4
					loop		BLEND_BEGIN; 循环

					add			esi, nextLineOffset_src; 加上偏移量，使定位到下行起始处
					add			edi, nextLineOffset_dst

					inc			ebx
					mov			ecx, blend_width

					cmp			ebx, blend_height; 若ebx小于blend_height, 则转移到上面继续混合
					jb			BLEND_BEGIN

					EMMS; 因为从mm0到mm7, 这些寄存器是“借用”浮点寄存器的低64位, 所以每次在用完MMX指令后一定要用EMMS指令将寄存器清空
			}
#endif
		}

		void AlphaBlend32(UINT* pDstBmp, int dst_width, const UINT* pSrcBmp, int src_width, int blend_width, int blend_height, unsigned int col)
		{
			// C实现

			const int nextLineOffset_src = (src_width - blend_width) * 4;	// 混合完一行像素后，通过加上该值，便可直接定位到下行起始像素
			const int nextLineOffset_dst = (dst_width - blend_width) * 4;
			unsigned char* pSrc = (unsigned char*)pSrcBmp;
			unsigned char* pDst = (unsigned char*)pDstBmp;
			int below_A, below_R, below_G, below_B;
			int above_A, above_R, above_G, above_B;
			glm::vec4 cf;
			size_t count = 0;
			for (int h = 0, w = 0; h < blend_height; h++)
			{
				for (w = 0; w < blend_width; w++)
				{
					count++;
					above_B = *pSrc++;
					above_G = *pSrc++;
					above_R = *pSrc++;
					above_A = *pSrc++;
					if (col != -1 && above_A > 0)
					{
						cf = ColorConvertU32ToFloat4(col);
						above_B = cf.x * above_B;
						above_G = cf.y * above_G;
						above_R = cf.z * above_B;
						above_A = cf.w * above_A;
					}
					if (above_A == 0)
					{
						pDst += 4;
						continue;
					}

					below_B = *pDst;
					below_G = *(pDst + 1);
					below_R = *(pDst + 2);
					below_A = *(pDst + 3);
					unsigned int uc[] = { below_B - (below_B - above_B) * above_A / 255 ,
						below_G - (below_G - above_G) * above_A / 255,
						below_R - (below_R - above_R) * above_A / 255,
					};
					*pDst++ = below_B - (below_B - above_B) * above_A / 255;
					*pDst++ = below_G - (below_G - above_G) * above_A / 255;
					*pDst++ = below_R - (below_R - above_R) * above_A / 255;

					if (below_A == 255)
						* pDst++ = 255;
					else
						*pDst++ = below_A - (below_A - above_A) * above_A / 255;
				}

				pSrc += nextLineOffset_src;
				pDst += nextLineOffset_dst;
			}
			return;
		}


		/* Thanks to Frank Warmerdam for this superior implementation
			of gdAlphaBlend(), which merges alpha in the
			destination color much better. */

			/**
			 * Function: gdAlphaBlend
			 *
			 * Blend two colors
			 *
			 * Parameters:
			 *   dst - The color to blend onto.
			 *   src - The color to blend.
			 *
			 * See also:
			 *   - <gdImageAlphaBlending>
			 *   - <gdLayerOverlay>
			 *   - <gdLayerMultiply>
			 */
		unsigned int alpha_blend(int dst, int src)
		{
			const int alpha_max = 127;
			auto st = ((u_col*)& src)->c;
			auto dt = ((u_col*)& dst)->c;
			int src_alpha = st.a;
			int dst_alpha, alpha, red, green, blue;
			int src_weight, dst_weight, tot_weight;

			/* -------------------------------------------------------------------- */
			/*      Simple cases we want to handle fast.                            */
			/* -------------------------------------------------------------------- */
			if (src_alpha == 0)
				return dst;
			dst_alpha = dt.a;
			if (dst_alpha == 0)
				return src;

			if (src_alpha == alpha_max)
				return dst;
			if (dst_alpha == alpha_max)
				return src;

			/* -------------------------------------------------------------------- */
			/*      What will the source and destination alphas be?  Note that      */
			/*      the destination weighting is substantially reduced as the       */
			/*      overlay becomes quite opaque.                                   */
			/* -------------------------------------------------------------------- */
			src_weight = alpha_max - src_alpha;
			dst_weight = (alpha_max - dst_alpha) * src_alpha / alpha_max;
			tot_weight = src_weight + dst_weight;

			/* -------------------------------------------------------------------- */
			/*      What red, green and blue result values will we use?             */
			/* -------------------------------------------------------------------- */
			alpha = src_alpha * dst_alpha / alpha_max;

			red = (st.r * src_weight
				+ dt.r * dst_weight) / tot_weight;
			green = (st.g * src_weight
				+ dt.g * dst_weight) / tot_weight;
			blue = (st.b * src_weight
				+ dt.b * dst_weight) / tot_weight;

			/* -------------------------------------------------------------------- */
			/*      Return merged result.                                           */
			/* -------------------------------------------------------------------- */
			return ((alpha << 24) + (red << 16) + (green << 8) + blue);
		}

		//static int gdAlphaOverlayColor(int src, int dst, int max);

		/**
		 * Function: gdLayerOverlay
		 *
		 * Overlay two colors
		 *
		 * Parameters:
		 *   dst - The color to overlay onto.
		 *   src - The color to overlay.
		 *
		 * See also:
		 *   - <gdImageAlphaBlending>
		 *   - <gdAlphaBlend>
		 *   - <gdLayerMultiply>
		 */
		static unsigned int layer_overlay(int dst, int src)
		{
			const int alpha_max = 127;
			auto st = ((u_col*)& src)->c;
			auto dt = ((u_col*)& dst)->c;
			int a1, a2;
			a1 = alpha_max - dt.a;
			a2 = alpha_max - st.a;
			return (((alpha_max - a1 * a2 / alpha_max) << 24) +
				(AlphaOverlayColor(st.r, dt.r, 255) << 16) +
				(AlphaOverlayColor(st.g, dt.g, 255) << 8) +
				(AlphaOverlayColor(st.b, dt.b, 255))
				);
		}

		/* Apply 'overlay' effect - background pixels are colourised by the foreground colour */
		static int AlphaOverlayColor(int src, int dst, int max)
		{
			dst = dst << 1;
			if (dst > max) {
				/* in the "light" zone */
				return dst + (src << 1) - (dst * src / max) - max;
			}
			else {
				/* in the "dark" zone */
				return dst * src / max;
			}
		}

		/**
		 * Function: gdLayerMultiply
		 *
		 * Overlay two colors with multiply effect
		 *
		 * Parameters:
		 *   dst - The color to overlay onto.
		 *   src - The color to overlay.
		 *
		 * See also:
		 *   - <gdImageAlphaBlending>
		 *   - <gdAlphaBlend>
		 *   - <gdLayerOverlay>
		 */
		static unsigned int layer_multiply(int dst, int src)
		{
			const int alpha_max = 127;
			auto st = ((u_col*)& src)->c;
			auto dt = ((u_col*)& dst)->c;
			int a1, a2, r1, r2, g1, g2, b1, b2;
			a1 = alpha_max - st.a;
			a2 = alpha_max - dt.a;

			r1 = 255 - (a1 * (255 - st.r)) / alpha_max;
			r2 = 255 - (a2 * (255 - dt.r)) / alpha_max;
			g1 = 255 - (a1 * (255 - st.g)) / alpha_max;
			g2 = 255 - (a2 * (255 - dt.g)) / alpha_max;
			b1 = 255 - (a1 * (255 - st.b)) / alpha_max;
			b2 = 255 - (a2 * (255 - dt.b)) / alpha_max;

			a1 = alpha_max - a1;
			a2 = alpha_max - a2;
			return (((a1 * a2 / alpha_max) << 24) +
				((r1 * r2 / 255) << 16) +
				((g1 * g2 / 255) << 8) +
				((b1 * b2 / 255))
				);
		}
		void px_blend(UINT* pDstBmp, UINT src, unsigned int col = -1)
		{
			// C实现
			unsigned char* pSrc = (unsigned char*)& src;
			unsigned char* pDst = (unsigned char*)pDstBmp;
			int below_A, below_R, below_G, below_B;
			int above_A, above_R, above_G, above_B;
			glm::vec4 cf;

			above_B = *pSrc++;
			above_G = *pSrc++;
			above_R = *pSrc++;
			above_A = *pSrc++;
			if (col != -1 && above_A > 0)
			{
				cf = ColorConvertU32ToFloat4(col);
				above_B = cf.x * above_B;
				above_G = cf.y * above_G;
				above_R = cf.z * above_B;
				above_A = cf.w * above_A;
			}
			if (above_A == 0)
			{
				pDst += 4;
				return;
			}
			below_B = *pDst;
			below_G = *(pDst + 1);
			below_R = *(pDst + 2);
			below_A = *(pDst + 3);
			if (below_A == 0)
			{
				//below_A = 255;
				//*pDstBmp = src;
			}
			unsigned int uc[] = { below_B - (below_B - above_B) * above_A / 255 ,
				below_G - (below_G - above_G) * above_A / 255,
				below_R - (below_R - above_R) * above_A / 255,
			};
			*pDst++ = below_B - (below_B - above_B) * above_A / 255;
			*pDst++ = below_G - (below_G - above_G) * above_A / 255;
			*pDst++ = below_R - (below_R - above_R) * above_A / 255;
			auto lsa = pDst;
			if (below_A == 255)
				* pDst++ = 255;
			else
				*pDst++ = below_A - (below_A - above_A) * above_A / 255;

			return;
		}
		void DrawBmp(Image* src, int x, int y, int w, int h, int dx, int dy, unsigned int col = -1)
		{
#if 1
			const unsigned int* pBmp = src->data();
			RECT srcRect = { x, y, x + w - 1, y + h - 1 };
			RECT dstRect = { dx, dy, 0, 0 };

			// 边界校正
			{
				if (srcRect.right >= src->width)
					srcRect.right = src->width - 1;
				if (srcRect.bottom >= src->height)
					srcRect.bottom = src->height - 1;

				// 超出左边界
				if (dstRect.left < 0)
				{
					srcRect.left += -dstRect.left;
					dstRect.left = 0;
				}
				// 超出上边界
				if (dstRect.top < 0)
				{
					srcRect.top += -dstRect.top;
					dstRect.top = 0;
				}

				int visibleW = srcRect.right - srcRect.left + 1;
				int visibleH = srcRect.bottom - srcRect.top + 1;

				// 超出右边界
				if (dstRect.left + visibleW > width)
				{
					visibleW = width - dstRect.left;
					srcRect.right = srcRect.left + visibleW - 1;
				}
				// 超出下边界
				if (dstRect.top + visibleH > height)
				{
					visibleH = height - dstRect.top;
					srcRect.bottom = srcRect.top + visibleH - 1;
				}


				dstRect.right = dstRect.left + (srcRect.right - srcRect.left);
				dstRect.bottom = dstRect.top + (srcRect.bottom - srcRect.top);

				if (visibleW <= 0 || visibleH <= 0)	return;
			}

			const UINT* pSrc = pBmp + srcRect.top * src->width + srcRect.left;
			UINT* pDst = (UINT*)data() + dstRect.top * width + dstRect.left;
			if (col != -1)
			{
				AlphaBlend32(pDst, width, pSrc, src->width, srcRect.right - srcRect.left + 1, srcRect.bottom - srcRect.top + 1, col);
			}
			else
			{
				AlphaBlend32(pDst, width, pSrc, src->width, srcRect.right - srcRect.left + 1, srcRect.bottom - srcRect.top + 1);
			}
#endif
		}

		/*
		  ct 0为RGB, 1为BGR
		*/
#ifdef _WIN32
		void copyToDc(HDC hdc, const glm::ivec2& pos = { 0, 0 }, int ct = 0)
		{
			if (_data.empty() || !(width > 0 && height > 0))
			{
				return;
			}
			memset(&bmpInfo, 0, sizeof(bmpInfo.bmiHeader));
			bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
			bmpInfo.bmiHeader.biWidth = width;
			bmpInfo.bmiHeader.biHeight = -height;
			bmpInfo.bmiHeader.biPlanes = 1;
			bmpInfo.bmiHeader.biBitCount = 32;
			bmpInfo.bmiHeader.biCompression = BI_BITFIELDS;

			if (ct == 0)
			{
				//RGB
				*(UINT*)(bmpInfo.bmiColors + 2) = 0xFF0000;	// red分量
				*(UINT*)(bmpInfo.bmiColors + 1) = 0x00FF00;	// green分量
				*(UINT*)(bmpInfo.bmiColors + 0) = 0x0000FF;	// blue分量
			}
			else
			{
				//BGR
				*(UINT*)(bmpInfo.bmiColors + 0) = 0xFF0000;	// red分量
				*(UINT*)(bmpInfo.bmiColors + 1) = 0x00FF00;	// green分量
				*(UINT*)(bmpInfo.bmiColors + 2) = 0x0000FF;	// blue分量
			}
			::SetDIBitsToDevice(hdc,
				pos.x, pos.y, width, height,
				0, 0, 0, height,
				_data.data(), &bmpInfo, DIB_RGB_COLORS);

		}
#endif

		Image* getScaleImage(int desW, int desH, int mode = 0)
		{
			unsigned char* srcBuf = (unsigned char*)data();
			int srcW = width;
			int srcH = height;
			int bitCount = 32;
			int nearest = 1;
			//先不支持小于16位的位图
			if (bitCount < 16)
			{
				return 0;//exit(-1);
			}
			int lineSize = bitCount * srcW / 8;
			//偏移量，windows系统要求每个扫描行按四字节对齐
			//   int alignunsigned chars = ((bmiHeader.biWidth * bitCount + 31) & ~31) / 8L
			//   - bmiHeader.biWidth * bitCount / 8L;
			//原图像缓存
			int srcBufSize = lineSize * srcH;
			int i, j;


			//目标图像缓存
			int desBufSize = ((desW * bitCount + 31) / 32) * 4 * desH;
			int desLineSize = ((desW * bitCount + 31) / 32) * 4;
			Image* ret = create_null(desW, desH);
			unsigned char* desBuf = (unsigned char*)ret->data();
			//FillMemory(desBuf, desBufSize, 0xff);
			memset(desBuf, 0, desBufSize);
			double rateH = (double)srcH / desH;
			double rateW = (double)srcW / desW;
			//最临近插值算法
			if (mode == nearest)
			{
				for (i = 0; i < desH; i++)
				{
					//选取最邻近的点
					int tSrcH = (int)(rateH * i + 0.5);
					for (j = 0; j < desW; j++)
					{
						int tSrcW = (int)(rateW * j + 0.5);
						memcpy(&desBuf[i * desLineSize] + j * bitCount / 8,
							&srcBuf[tSrcH * lineSize] + tSrcW * bitCount / 8,
							bitCount / 8);
					}
				}
			}
			//双线型内插值算法
			else
			{
				for (i = 0; i < desH; i++)
				{
					int tH = (int)(rateH * i);
					int tH1 = std::min(tH + 1, srcH - 1);
					float u = (float)(rateH * i - tH);
					for (j = 0; j < desW; j++)
					{
						int tW = (int)(rateW * j);
						int tW1 = std::min(tW + 1, srcW - 1);
						float v = (float)(rateW * j - tW);

						//f(i+u,j+v) = (1-u)(1-v)f(i,j) + (1-u)vf(i,j+1) + u(1-v)f(i+1,j) + uvf(i+1,j+1) 
						for (int k = 0; k < 4; k++)
						{
							desBuf[i * desLineSize + j * bitCount / 8 + k] =
								(1 - u) * (1 - v) * srcBuf[tH * lineSize + tW * bitCount / 8 + k] +
								(1 - u) * v * srcBuf[tH1 * lineSize + tW * bitCount / 8 + k] +
								u * (1 - v) * srcBuf[tH * lineSize + tW1 * bitCount / 8 + k] +
								u * v * srcBuf[tH1 * lineSize + tW1 * bitCount / 8 + k];
						}
					}
				}
			}
			return ret;
		}

	private:


	};
	//  [4/6/2018 huazai]
#if 0
	/*
	图集装箱
	*/
	class AtlasBox :public Res
	{
	private:
		//x空位偏移,y每行高度
		std::vector<glm::ivec2>  _line;
		//当前高度
		int						 _height = 0;
		//误差范围
		int						 _range = 2;
		Image					 _image;
		glm::ivec2				 _maxsize = { 2048,2048 };

	public:
		//把一个矩形装箱返回位置
		glm::ivec4 push(Image* img, const glm::ivec4& rs)
		{
			glm::ivec4 ret = { -1,-1,rs.z,rs.w };

			glm::ivec2 posrs = findPos({ rs.z,rs.w });
			if (!(posrs.y < 0))
			{
				ret.x = posrs.x;
				ret.y = posrs.y;
				if (img && img->width > 0 && img->height > 0)
				{
					_image.copy_value(img, rs, ret);
				}
			}
			return ret;
		}
		//查找合适的位置
		glm::ivec2 findPos(const glm::ivec2& size)
		{
			glm::ivec2 ret = { 0,-1 };
			if (size.x > _maxsize.x || size.y > _maxsize.y || (size.y > (_maxsize.y - _height)))
			{
				return ret;
			}
			std::set<int> tems;
			int i = 0, ah = 0;
			for (auto it : _line)
			{
				int y = it.y - size.y;
				if (y < _range && y >= 0 && (it.x + size.x <= _maxsize.x))
				{
					tems.insert(i);
				}
				i++;
			}
			if (tems.size())
			{
				glm::ivec2 t2 = { 0,0 };
				int hi = *tems.begin();
				t2 = _line[hi];
				if (!(t2.y > 0))
				{
					return ret;
				}
				ret = t2;
				ret.y = 0;
				for (size_t h = 0; h < hi; h++)
				{
					ret.y += _line[h].y;
				}
				_line[hi].x += size.x;
			}
			else
			{
				ret.y = _height;
				_line.push_back({ size.x, size.y });
				_height += size.y;
			}
			return ret;
		}
		//判断是否箱满
		bool isFull(int r, int h)
		{
			size_t inc = 0;
			for (auto it : _line)
			{
				if ((_maxsize.x - it.x > r))
				{
					inc++;
				}
			}
			if (_line.empty())
			{
				inc = 1;
			}
			return (!(inc > 0) || ((_maxsize.y - _height) < h));
		}
		//清空箱子
		void clear()
		{
			_line.clear();
			_image.clearColor(0);
			_height = 0;
		}
		//获取渲染图像
		Image* getImage()
		{
			return &_image;
		}
		void setUpdate()
		{
			_image.set_update();
		}
	public:
		static AtlasBox* create(const glm::ivec2& size = { 2048,2048 }, int r = 2)
		{
			return new AtlasBox(size, r);
		}

	public:
		AtlasBox(const glm::ivec2& size = { 2048,2048 }, int r = 6) :_maxsize(size), _range(r)
		{
			_image.resize(_maxsize.x, _maxsize.y);
			_image.clearColor(0);
		}

		~AtlasBox()
		{
		}

	};
	/*
	图集
	*/
	typedef struct AtlasInfo
	{
		//所属的图像
		Image* image = 0;
		glm::ivec4	 rect;
		//使用计数
		size_t		 use = 0;
		AtlasInfo(Image* img = 0, const glm::ivec4 & rs = { 0,0,0,0 }, size_t u = 0) :image(img), rect(rs), use(u)
		{}
	}AtlasInfo;
	class AtlasCache :public Res
	{
	private:
		std::map<std::string, AtlasInfo> _fc;
		//图像数据，记录当前新坐标
		std::vector<AtlasBox*> _box;
		size_t incempty = 0;
		std::set<std::wstring> _fn;

		Image temimg;
		Lock lock;
	public:

	public:
		static AtlasCache* create()
		{
			return new AtlasCache();
		}
		AtlasInfo* getAtlasImage(const std::string& k)
		{
			AtlasInfo* ret = getAtlasInfo(k);

			return ret;
		}
		void push(const std::string& k, Image* img)
		{
			LOCK(lock);
			auto pos = _box[incempty]->findPos({ img->width,img->height });
			if (_box[incempty]->isFull(img->width, img->height) || pos.y < 0)
			{
				//箱满增加一个箱子
				_box.push_back(AtlasBox::create());
				incempty++;
			}

			std::set<size_t> ups;
			glm::ivec4 rs = _box[incempty]->push(img, { 0,0,img->width,img->height });
			ups.insert(incempty);
			if (!(rs.x < 0 || rs.y < 0))
			{
				pushAtlasInfo(k, _box[incempty]->getImage(), rs);
			}
			for (auto it : ups)
			{
				_box[it]->setUpdate();
			}
		}

	private:
		AtlasInfo* getAtlasInfo(const std::string& k)
		{
			auto it = _fc.find(k);
			return it != _fc.end() ? &it->second : 0;
		}
		void pushAtlasInfo(const std::string& k, Image* img, const glm::ivec4& rs)
		{
			_fc[k] = AtlasInfo{ img,rs,1 };
		}

	public:
		AtlasCache()
		{
			_box.push_back(AtlasBox::create());
		}

		~AtlasCache()
		{
			for (auto it : _box)
			{
				AtlasBox::destroy(it);
			}
		}
		void clear()
		{
			_fc.clear();
			for (auto it : _box)
			{
				it->clear();
			}
		}
	private:

	};
#endif
}//!hz
#endif //__Image__h__