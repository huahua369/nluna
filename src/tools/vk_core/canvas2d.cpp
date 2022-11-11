#include <ntype.h>
#include <math.h>
// 生成矢量图三角形
#include <base/camera.h>
#include <view/image.h>
#include <view/draw_info.h>
#include <view/layout_info.h>
//#include <vk_core/vk_cx.h>
//#include <vk_core/vk_cx1.h>
#include <font/font_cx.h>

using namespace std;
#include "canvas2d.h"
#include "..\gui\hz_gui.h"
//#include "view.h"
#ifndef M_PI
#define M_PI _PI
#endif // !M_PI
namespace hz {
#define KAPPA90 0.5522847493f	// Length proportional to radius of a cubic bezier handle for 90deg arcs.
#define COF(arr) (sizeof(arr) / sizeof(0[arr]))

	//static float signf(float a) { return a >= 0.0f ? 1.0f : -1.0f; }
	//static float clampf(float a, float mn, float mx) { return a < mn ? mn : (a > mx ? mx : a); }
	//static float cross(float dx0, float dy0, float dx1, float dy1) { return dx1 * dy0 - dx0 * dy1; }

	static int mini(int a, int b) { return a < b ? a : b; }
	static int maxi(int a, int b) { return a > b ? a : b; }
	static int clampi(int a, int mn, int mx) { return a < mn ? mn : (a > mx ? mx : a); }
	static float minf(float a, float b) { return a < b ? a : b; }
	static float maxf(float a, float b) { return a > b ? a : b; }
	static float absf(float a) { return a >= 0.0f ? a : -a; }
	static float signf(float a) { return a >= 0.0f ? 1.0f : -1.0f; }
	static float clampf(float a, float mn, float mx) { return a < mn ? mn : (a > mx ? mx : a); }
	static float cross(float dx0, float dy0, float dx1, float dy1) { return dx1 * dy0 - dx0 * dy1; }
	static float normalize(float* x, float* y)
	{
		float d = sqrtf((*x) * (*x) + (*y) * (*y));
		if (d > 1e-6f) {
			float id = 1.0f / d;
			*x *= id;
			*y *= id;
		}
		return d;
	}
	static int ptEquals(float x1, float y1, float x2, float y2, float tol)
	{
		float dx = x2 - x1;
		float dy = y2 - y1;
		return dx * dx + dy * dy < tol* tol;
	}

	static float distPtSeg(float x, float y, float px, float py, float qx, float qy)
	{
		float pqx, pqy, dx, dy, d, t;
		pqx = qx - px;
		pqy = qy - py;
		dx = x - px;
		dy = y - py;
		d = pqx * pqx + pqy * pqy;
		t = pqx * dx + pqy * dy;
		if (d > 0) t /= d;
		if (t < 0) t = 0;
		else if (t > 1) t = 1;
		dx = px + t * pqx - x;
		dy = py + t * pqy - y;
		return dx * dx + dy * dy;
	}


	void setVec(glm::vec3& d, const glm::vec2& s)
	{
		//d = { s, 0.f };
		d.x = s.x; d.y = s.y; d.z = 0.f;
	}
	void setVec(glm::vec3& d, const glm::vec3& s)
	{
		d = s;
		//d.x = s.x; d.y = s.y; d.z = s.z;
	}
	void setVec(glm::vec3& d, const glm::vec4& s)
	{
		glm::vec4 d1 = s;
		d.x = s.x; d.y = s.y; d.z = s.z;
	}
	void setVec(glm::dvec3& d, const glm::vec2& s)
	{
		//d = { s, 0.f };
		d.x = s.x; d.y = s.y; d.z = 0.f;
	}
	void setVec(glm::dvec3& d, const glm::vec3& s)
	{
		d = s;
		//d.x = s.x; d.y = s.y; d.z = s.z;
	}
	void setVec(glm::dvec3& d, const glm::vec4& s)
	{
		glm::vec4 d1 = s;
		d.x = s.x; d.y = s.y; d.z = s.z;
	}
	void PrimRectUV(const glm::vec2& a, const glm::vec2& c, const glm::vec2& uv_a, const glm::vec2& uv_c, unsigned int col, DrawIdx idx
		, vert_t* wvtx, DrawIdx* widx)
	{
		glm::vec2 b(c.x, a.y), d(a.x, c.y), uv_b(uv_c.x, uv_a.y), uv_d(uv_a.x, uv_c.y);
		widx[0] = idx; widx[1] = (DrawIdx)(idx + 1); widx[2] = (DrawIdx)(idx + 2);
		widx[3] = idx; widx[4] = (DrawIdx)(idx + 2); widx[5] = (DrawIdx)(idx + 3);
		setVec(wvtx[0].pos, a); wvtx[0].uv = uv_a; wvtx[0].col = col;
		setVec(wvtx[1].pos, b); wvtx[1].uv = uv_b; wvtx[1].col = col;
		setVec(wvtx[2].pos, c); wvtx[2].uv = uv_c; wvtx[2].col = col;
		setVec(wvtx[3].pos, d); wvtx[3].uv = uv_d; wvtx[3].col = col;
	}

	void PrimQuadUV(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d
		, const glm::vec2& uv_a, const glm::vec2& uv_b, const glm::vec2& uv_c, const glm::vec2& uv_d
		, unsigned int col, DrawIdx idx, vert_t* wvtx, DrawIdx* widx)
	{
		widx[0] = idx; widx[1] = (idx + 1); widx[2] = (idx + 2);
		widx[3] = idx; widx[4] = (idx + 2); widx[5] = (idx + 3);
		setVec(wvtx[0].pos, a); wvtx[0].uv = uv_a; wvtx[0].col = col;
		setVec(wvtx[1].pos, b); wvtx[1].uv = uv_b; wvtx[1].col = col;
		setVec(wvtx[2].pos, c); wvtx[2].uv = uv_c; wvtx[2].col = col;
		setVec(wvtx[3].pos, d); wvtx[3].uv = uv_d; wvtx[3].col = col;
	}
	// 生成渲染图像的顶点
	//void make_image(const glm::ivec4& a, const glm::ivec2& texsize, t_vector<vert_t>& out_vec, t_vector<unsigned int>& idx, const glm::ivec4& rect, unsigned int col)
	//{
	//	glm::ivec2 pos = { a.x,a.y }, size = { a.z,a.w };
	//	glm::vec4 v4 = { 0,0,1,1 };
	//	glm::vec4 uv = v4;
	//	glm::vec2 s = size;
	//	if (!(rect.x < 0))
	//	{
	//		v4 = rect;
	//		v4.z += v4.x; v4.w += v4.y;//加上原点坐标
	//		uv = {
	//			v4.x / texsize.x, v4.y / texsize.y, v4.z / texsize.x, v4.w / texsize.y,
	//		};
	//	}
	//	PrimRectUV(pos, { pos.x + s.x,pos.y + s.y }, { uv.x,uv.y }, { uv.z,uv.w }, col, out_vec.size(), out_vec, idx);
	//	return;
	//}
	static double multi(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c) {
		return (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y);
	}
	static int across(const glm::vec4& v1, const glm::vec4& v2) {
		if (std::max(v1.x, v1.z) >= std::min(v2.x, v2.z) &&
			std::max(v2.x, v2.z) >= std::min(v1.x, v1.z) &&
			std::max(v1.y, v1.w) >= std::min(v2.y, v2.w) &&
			std::max(v2.y, v2.w) >= std::min(v1.y, v1.w) &&
			multi(glm::vec2(v2.x, v2.y), glm::vec2(v1.z, v1.w), glm::vec2(v1.x, v1.y)) * multi(glm::vec2(v1.z, v1.w), glm::vec2(v2.z, v2.w), glm::vec2(v1.x, v1.y)) > 0 &&
			multi(glm::vec2(v1.x, v1.y), glm::vec2(v2.z, v2.w), glm::vec2(v2.x, v2.y)) * multi(glm::vec2(v2.z, v2.w), glm::vec2(v1.z, v1.w), glm::vec2(v2.x, v2.y)) > 0)
			return 1;
		return 0;
	}
	static bool intersection(const glm::vec4& u, const glm::vec4& v, glm::vec2* pt = nullptr)
	{
		glm::vec2 p;
		if (across(u, v))
		{
			p.x = (multi(glm::vec2(v.z, v.w), glm::vec2(u.z, u.w), glm::vec2(v.x, v.y)) * v.x - multi(glm::vec2(v.x, v.y), glm::vec2(u.z, u.w), glm::vec2(u.x, u.y)) * v.z)
				/ (multi(glm::vec2(v.z, v.w), glm::vec2(u.z, u.w), glm::vec2(v.x, v.y)) - multi(glm::vec2(v.x, v.y), glm::vec2(u.z, u.w), glm::vec2(u.x, u.y)));
			p.y = (multi(glm::vec2(v.z, v.w), glm::vec2(u.z, u.w), glm::vec2(v.x, v.y)) * v.y - multi(glm::vec2(v.x, v.y), glm::vec2(u.z, u.w), glm::vec2(u.x, u.y)) * v.w)
				/ (multi(glm::vec2(v.z, v.w), glm::vec2(u.z, u.w), glm::vec2(v.x, v.y)) - multi(glm::vec2(v.x, v.y), glm::vec2(u.z, u.w), glm::vec2(u.x, u.y)));
			if (pt)
			{
				*pt = p;
			}
			return true;
		}
		return false;
	}

	inline uint8_t is_rect_intersect(int x01, int x02, int y01, int y02,
		int x11, int x12, int y11, int y12)
	{
		int zx = abs(x01 + x02 - x11 - x12);
		int x = abs(x01 - x02) + abs(x11 - x12);
		int zy = abs(y01 + y02 - y11 - y12);
		int y = abs(y01 - y02) + abs(y11 - y12);
		if (zx <= x && zy <= y)
			return 1;
		else
			return 0;
	}
	inline bool is_rect_intersect(glm::vec4 r1, glm::vec4 r2)
	{
		//第一种情况：如果b.x > a.x + a.w，则a和b一定不相交，
			//第二种情况：如果a.y > b.y + b.h，则a和b一定不相交，
			//第三种情况：如果b.y > a.y + a.h，则a和b一定不相交，
			//第四种情况：如果a.x > b.x + b.w，则a和b一定不相交
		auto& a = r1; auto& b = r2;
		if (a.x > b.x + b.z || b.x > a.x + a.z || a.y > b.y + b.w || b.y > a.y + a.w) {
			return false;
		}
		else {
			return true;
		}
		return is_rect_intersect(r1.x, r1.y, r1.z, r1.w, r2.x, r2.y, r2.z, r2.w);
		//return !(((r1.z < r2.x) || (r1.w > r2.y)) || ((r2.z < r1.x) || (r2.w > r1.y)));
	}

	inline void pad5(glm::vec2& p, float p1 = 0.5f)
	{
		glm::ivec2 t = p;
		p = t;
		const auto pad = glm::vec2(p1, p1);
		p += pad;
	}
	void canvas_cx::pad5s(glm::vec2& p, float p1)
	{
		glm::ivec2 t = p;
		p = t;
		const auto pad = glm::vec2(p1, p1);
		p += pad;
	}
#if 1

	// 图集
	image_atlas_u::image_atlas_u()
	{
		img = new Image(ds, ds, 0);
		_packer = new stb_packer();
		_packer->init_target(ds, ds);
	}

	image_atlas_u::~image_atlas_u()
	{
		delop(img);
		delop(_packer);
	}

	void image_atlas_u::resize(int width, int height)
	{
		if (width > 0 && height > 0)
		{
			img->resize(width, height);
			_packer->init_target(width, height);
			img->clear_color(0);
		}
	}

	void image_atlas_u::clear()
	{
		_packer->clear();
		img->clear_color(0);
	}

	glm::ivec2 image_atlas_u::push_rect(glm::ivec2 rc)
	{
		glm::ivec2 prs;
		int k = _packer->push_rect(rc, &prs);
		return prs;
	}

	image_cs* image_atlas_u::push_rect(glm::ivec2 rc, image_cs* ot)
	{
		auto ps = push_rect(rc);
		do
		{
			if (ps.x < 0 || ps.y < 0)
			{
				ot = 0;
				break;
			}
			if (ot)
			{
				ot->img = img;
				ot->rect = { ps, rc };
				ot->size = rc;
			}
		} while (0);
		return ot;
	}

	image_m_cs* image_atlas_u::push_rect(glm::ivec2 rc, image_m_cs* ot)
	{
		auto ps = push_rect(rc);
		do
		{
			if (ps.x < 0 || ps.y < 0)
			{
				ot = 0;
				break;
			}
			if (ot)
			{
				ot->img = img;
				ot->rect = { ps, rc };
				ot->size = rc;
			}
		} while (0);
		return ot;
	}
	int image_atlas_u::push_rect(glm::ivec4* rc, int n)
	{
		return _packer->push_rect(rc, n);
	}
	void image_atlas_u::cp2atlas(glm::ivec2& pos, glm::ivec4 src4, Image* src)
	{
		assert(img);
		if (src)
		{
			img->copyImage(src, pos, src4);
		}
	}

	void image_atlas_u::cp2atlas(glm::ivec2& pos, glm::ivec4 src4, image_gray* src, uint32_t fill)
	{
		assert(img);
		if (src)
		{
			unsigned char* bdata = src->data();
			glm::ivec2 rbs = { src->width, src->height };
			if (bdata && rbs.x > 0 && rbs.y > 0)
			{
				img->copy2(src, pos, src4, fill);
			}
		}
	}





	// todo can_path
	can_path::can_path()
	{
	}
	can_path::~can_path()
	{
	}
	size_t can_path::size()
	{
		return _points.size();
	}

	void TransformPoint(float* dx, float* dy, const float* t, float sx, float sy)
	{
		*dx = sx * t[0] + sy * t[2] + t[4];
		*dy = sx * t[1] + sy * t[3] + t[5];
	}
	void can_path::append_commands(float* vals, int nvals)
	{
		int i;
		float xform[6] = {};
		glm::vec2 cnd;
		if ((commands)vals[0] != commands::P_CLOSE && (commands)vals[0] != commands::P_WINDING) {
			cnd.x = vals[nvals - 2];
			cnd.y = vals[nvals - 1];
		}

		// transform commands
		i = 0;
		while (i < nvals) {
			commands cmd = (commands)vals[i];
			switch (cmd) {
			case commands::P_MOVETO:
				TransformPoint(&vals[i + 1], &vals[i + 2], xform, vals[i + 1], vals[i + 2]);
				i += 3;
				break;
			case commands::P_LINETO:
				TransformPoint(&vals[i + 1], &vals[i + 2], xform, vals[i + 1], vals[i + 2]);
				i += 3;
				break;
			case commands::P_BEZIERTO:
				TransformPoint(&vals[i + 1], &vals[i + 2], xform, vals[i + 1], vals[i + 2]);
				TransformPoint(&vals[i + 3], &vals[i + 4], xform, vals[i + 3], vals[i + 4]);
				TransformPoint(&vals[i + 5], &vals[i + 6], xform, vals[i + 5], vals[i + 6]);
				i += 7;
				break;
			case commands::P_CLOSE:
				i++;
				break;
			case commands::P_WINDING:
				i += 2;
				break;
			default:
				i++;
			}
		}
	}
	size_t can_path::PathArcToFast(const glm::vec2& centre, float radius, int amin, int amax, float scale /*= 1.0f*/)
	{
		static int aa[13] = {};
		static std::once_flag flag;
		static glm::vec2 circle_vtx[12];
		static bool circle_vtx_builds = false;
		static auto mcv = [](int n)
		{
			vector_vec2 cv;
			for (int i = 0; i < n; i++)
			{
				const float a = ((float)i * 2.0 * _PI) / (float)n;
				cv.push_back({ cosf(a), sinf(a) });
			}
			return cv;
		};
		const int circle_vtx_count = ARRAYSIZE(circle_vtx);
		if (!circle_vtx_builds)
		{
			std::call_once(flag, [&]()
				{
					aa[0] = 0;
					aa[3] = 1;
					aa[6] = 2;
					aa[9] = 3;
					aa[12] = 4;
					auto cv = mcv(12);
					memcpy(circle_vtx, cv.data(), sizeof(circle_vtx));
					circle_vtx_builds = true;
				});
		}

		if (amin > amax) return 0;
		if (radius == 0.0f)
		{
			_points.push_back(centre);
		}
		else
		{
#if 1
			int ar = radius / 2;
			int m[12] = {};
			m[6] = 12 / 6;
			if (ar > 3)
			{
				if (ar % 6)
				{
					ar = ((ar / 6) + 1) * 6;
				}
				int stride = ar * 4;
				int n = stride * scale;
				int tmin = aa[amin] * ar;
				int tmax = aa[amax] * ar;
				float na = (float)2 * _PI;
				for (int i = tmin; i <= tmax; i++)
				{
					const float a = (float)i / n * na;
					_points.push_back({ centre.x + cosf(a) * radius, centre.y + sinf(a) * radius });
				}
			}
			else
#endif
			{
				_points.reserve(_points.size() + (amax - amin + 1));
				for (int a = amin; a <= amax; a++)
				{
					const glm::vec2& c = circle_vtx[a % circle_vtx_count];
					_points.push_back(glm::vec2(centre.x + c.x * radius, centre.y + c.y * radius));
				}
			}
		}
		return _points.size();
	}
	// Stateful path API, add points then finish with PathFill() or PathStroke()
	void can_path::clear()
	{
		_points.clear();
	}
	void can_path::line_to(const glm::vec2& pos)
	{
		_points.push_back(pos);
	}
	void can_path::path_merge()
	{
		if (_points.empty())
		{
			return;
		}
		auto fb = _points[0];
		if (_sv.size() < _points.size())
		{
			//_sv.reserve(_points.size());
			_temp.reserve(_points.size());
		}
		_temp.clear();
		_sv.clear();
		glm::vec2 curr = _points[0];

		for (auto it : _points)
		{
			if (_sv.insert(it).second)
			{
				_temp.push_back(it);
			}
		}
		_points.swap(_temp);
	}
	void can_path::PathLineToMergeDuplicate(const glm::vec2& pos)
	{
		if (_points.size() == 0 || memcmp(&_points[_points.size() - 1], &pos, 8) != 0) _points.push_back(pos);
	}

	void can_path::arc_to(const glm::vec2& centre, float radius, int num_segments, bool closure,
		vector_vec2* path)
	{
		double pos = 0; glm::ivec2 range = { 0, -1 };
		double offset = 0.0f;
		path_ellipse_circle(centre, { radius, radius }, num_segments, path, pos, range, offset, closure);
		return;
#if 0
		if (!path)
		{
			path = &_points;
		}
		if (radius == 0.0f)
			path->push_back(centre);
		path->reserve(path->size() + (num_segments));
		for (int i = 0; i < num_segments; i++)
		{
			const float a = amin + ((float)i / (float)num_segments) * (amax - amin);
			path->push_back(glm::vec2(centre.x + cosf(a) * radius, centre.y + sinf(a) * radius));
		}
#endif
	}
	size_t can_path::arc_to(const glm::vec2& centre, const glm::vec2& r, int num_segments, bool closure, vector_vec2* path)
	{
		double pos = 0; glm::ivec2 range = { 0, -1 };
		double offset = 0.0f;
		return path_ellipse_circle(centre, r, num_segments, path, pos, range, offset, closure);
#if 0
		if (!path)
		{
			path = &_points;
		}
		size_t ret = path->size();
		if (rx == 0.0f)
			path->push_back(centre);
		path->reserve(path->size() + (num_segments));
		for (int i = 0; i < num_segments; i++)
		{
			const float a = amin + ((float)i / (float)num_segments) * (amax - amin);
			path->push_back(glm::vec2(centre.x + cosf(a) * rx, centre.y + sinf(a) * ry));
		}
		return path->size() - ret;
#endif
	}
	void can_path::arc_to(const glm::vec2& centre, const glm::vec2& r, int num_start, int num_end, int num_segments,
		vector_vec2* path)
	{
		path_ellipse_circle(centre, r, num_segments, path, 0.0, { num_start, num_end });
		return;
#if 0
		//float amin, float amax,
		if (!path)
		{
			path = &_points;
		}
		if (rx == 0.0f)
			path->push_back(centre);
		path->reserve(path->size() + (num_segments + 1));
		size_t count = 0;
		for (int i = num_start; i < num_end && count <= num_segments; i++, count++)
		{
			int ti = i;
			if (i > 360)
			{
				ti -= 360;
			}
			const float a = amin + ((float)ti / (float)num_segments) * (amax - amin);
			path->push_back(glm::vec2(centre.x + cosf(a) * rx, centre.y + sinf(a) * ry));
		}
#endif
	}
	//void PathLarmeTo(const glm::vec2& centre, float radius, float amin, float amax, int num_segments, int n)
	//{
	//	if (radius == 0.0f)
	//		_points.push_back(centre);
	//	_points.reserve(_points.size() + (num_segments + 1));
	//	for (int i = 0; i <= num_segments; i++)
	//	{
	//		const float a = amin + ((float)i / (float)num_segments) * (amax - amin);
	//		_points.push_back(glm::vec2(centre.x + cosf(a) * radius,
	//			centre.y + sinf(a) * radius * powf(sinf(a / 2.0f), n)));
	//	}
	//}

	void can_path::PathBezierToCasteljau(vector_vec2* path
		, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float tess_tol, int level)
	{
		float dx = x4 - x1;
		float dy = y4 - y1;
		float d2 = ((x2 - x4) * dy - (y2 - y4) * dx);
		float d3 = ((x3 - x4) * dy - (y3 - y4) * dx);
		d2 = (d2 >= 0) ? d2 : -d2;
		d3 = (d3 >= 0) ? d3 : -d3;
		if ((d2 + d3) * (d2 + d3) < tess_tol * (dx * dx + dy * dy))
		{
			path->push_back(glm::vec2(x4, y4));
		}
		else if (level < 10)
		{
			float x12 = (x1 + x2) * 0.5f, y12 = (y1 + y2) * 0.5f;
			float x23 = (x2 + x3) * 0.5f, y23 = (y2 + y3) * 0.5f;
			float x34 = (x3 + x4) * 0.5f, y34 = (y3 + y4) * 0.5f;
			float x123 = (x12 + x23) * 0.5f, y123 = (y12 + y23) * 0.5f;
			float x234 = (x23 + x34) * 0.5f, y234 = (y23 + y34) * 0.5f;
			float x1234 = (x123 + x234) * 0.5f, y1234 = (y123 + y234) * 0.5f;

			PathBezierToCasteljau(path, x1, y1, x12, y12, x123, y123, x1234, y1234, tess_tol, level + 1);
			PathBezierToCasteljau(path, x1234, y1234, x234, y234, x34, y34, x4, y4, tess_tol, level + 1);
		}
	}
	void can_path::PathBezierCurveTo(const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4, int num_segments)
	{
		glm::vec2 p1 = _points.back();
		if (num_segments <= 0) // Auto-tessellated
		{
			PathBezierToCasteljau(&_points, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, CurveTessellationTol, 0);
		}
		else
		{
			float t_step = 1.0 / (float)num_segments;
			for (int i_step = 1; i_step <= num_segments; i_step++)
			{
				float t = t_step * i_step;
				float u = 1.0f - t;
				float w1 = u * u * u;
				float w2 = 3 * u * u * t;
				float w3 = 3 * u * t * t;
				float w4 = t * t * t;
				_points.push_back(glm::vec2(w1 * p1.x + w2 * p2.x + w3 * p3.x + w4 * p4.x
					, w1 * p1.y + w2 * p2.y + w3 * p3.y + w4 * p4.y));
			}
		}
	}
	double tj(double ti, glm::vec2 Pi, glm::vec2 Pj) {
		double t = sqrt(sqrt(pow((Pj.x - Pi.x), 2) + pow((Pj.y - Pi.y), 2))) + ti;
		return t;
	}
	void catMullRomSpline4(const glm::vec2* p, int numSpace, vector_vec2* outs)
	{
		// 输入的四个点
		glm::vec2 P0, P1, P2, P3;
		P0.x = p[0].x;
		P0.y = p[0].y;
		P1.x = p[1].x;
		P1.y = p[1].y;
		P2.x = p[2].x;
		P2.y = p[2].y;
		P3.x = p[3].x;
		P3.y = p[3].y;

		double t0 = 0;
		double t1 = tj(t0, P0, P1);
		double t2 = tj(t1, P1, P2);
		double t3 = tj(t2, P2, P3);

		// 可以理解为点与点之间的间隔
		double linespace = (t2 - t1) / numSpace;

		double t = t1;
		while (t <= t2) {
			double A1_x = (t1 - t) / (t1 - t0) * P0.x + (t - t0) / (t1 - t0) * P1.x;
			double A1_y = (t1 - t) / (t1 - t0) * P0.y + (t - t0) / (t1 - t0) * P1.y;
			double A2_x = (t2 - t) / (t2 - t1) * P1.x + (t - t1) / (t2 - t1) * P2.x;
			double A2_y = (t2 - t) / (t2 - t1) * P1.y + (t - t1) / (t2 - t1) * P2.y;
			double A3_x = (t3 - t) / (t3 - t2) * P2.x + (t - t2) / (t3 - t2) * P3.x;
			double A3_y = (t3 - t) / (t3 - t2) * P2.y + (t - t2) / (t3 - t2) * P3.y;
			double B1_x = (t2 - t) / (t2 - t0) * A1_x + (t - t0) / (t2 - t0) * A2_x;
			double B1_y = (t2 - t) / (t2 - t0) * A1_y + (t - t0) / (t2 - t0) * A2_y;
			double B2_x = (t3 - t) / (t3 - t1) * A2_x + (t - t1) / (t3 - t1) * A3_x;
			double B2_y = (t3 - t) / (t3 - t1) * A2_y + (t - t1) / (t3 - t1) * A3_y;
			double C_x = (t2 - t) / (t2 - t1) * B1_x + (t - t1) / (t2 - t1) * B2_x;
			double C_y = (t2 - t) / (t2 - t1) * B1_y + (t - t1) / (t2 - t1) * B2_y;
			C_x = floor(C_x);
			C_y = floor(C_y);
			outs->push_back(glm::vec2(C_x, C_y));
			t = t + linespace;
		}
		// C 是输入4个点中，第2个点和第3个点之间的插值点
		return;
	}
	void can_path::catMullRomSpline(const vector_vec2& inputPoints, vector_vec2* outs, int numSpace)
	{
		// numSpace每两个点中间插入多少个点
		int numPoints = inputPoints.size();
		if (numPoints < 4 || !outs)
		{
			assert(numPoints > 3);
			return;
		}
		if (numPoints > 3) {
			int numSplines = numPoints - 3;
			outs->reserve(numPoints * numSpace);
			const glm::vec2* part = inputPoints.data();
			for (int i = 0; i < numSplines; i++, part++) {
				// 从第0个点开始，每次取4个点，输出插值出的点
				catMullRomSpline4(part, numSpace, outs);
			}
		}
	}
	void can_path::PathRect(const glm::vec2& a, const glm::vec2& b, float rounding, int rounding_corners)
	{
		float r = rounding;
		auto f1 = fabsf(b.x - a.x), f2 = fabsf(b.y - a.y);
		r = std::min(r, f1 * (((rounding_corners & (1 | 2)) == (1 | 2)) || ((rounding_corners & (4 | 8)) == (4 | 8)) ? 0.5f : 1.0f) - 0.5f/*- 1.0f*/);
		r = std::min(r, f2 * (((rounding_corners & (1 | 8)) == (1 | 8)) || ((rounding_corners & (2 | 4)) == (2 | 4)) ? 0.5f : 1.0f) - 0.5f/*- 1.0f*/);

		if (r <= 0.0f || rounding_corners == 0)
		{
			line_to(a);
			line_to(glm::vec2(b.x, a.y));
			line_to(b);
			line_to(glm::vec2(a.x, b.y));
		}
		else
		{
			const float r0 = (rounding_corners & 1) ? r : 0.0f;
			const float r1 = (rounding_corners & 2) ? r : 0.0f;
			const float r2 = (rounding_corners & 4) ? r : 0.0f;
			const float r3 = (rounding_corners & 8) ? r : 0.0f;
			size_t arcs[4] = {};
			arcs[0] = PathArcToFast(glm::vec2(a.x + r0, a.y + r0), r0, 6, 9);
			arcs[1] = PathArcToFast(glm::vec2(b.x - r1, a.y + r1), r1, 9, 12);
			arcs[2] = PathArcToFast(glm::vec2(b.x - r2, b.y - r2), r2, 0, 3);
			arcs[3] = PathArcToFast(glm::vec2(a.x + r3, b.y - r3), r3, 3, 6);

		}
	}
	glm::ivec4 can_path::PathRect(const glm::vec2& a, const glm::vec2& b, const glm::vec4& rounding)
	{
		glm::ivec4 ret = {};
		int rounding_corners = rounding.x + rounding.y + rounding.z + rounding.w;
		if (rounding_corners == 0)
		{
			line_to(a);
			line_to(glm::vec2(b.x, a.y));
			line_to(b);
			line_to(glm::vec2(a.x, b.y));
		}
		else
		{
			auto& r = rounding;
			const float r0 = r.x;
			const float r1 = r.y;
			const float r2 = r.z;
			const float r3 = r.w;
			size_t arcs[4] = {};
			arcs[0] = PathArcToFast(glm::vec2(a.x + r0, a.y + r0), r0, 6, 9);
			arcs[1] = PathArcToFast(glm::vec2(b.x - r1, a.y + r1), r1, 9, 12);
			arcs[2] = PathArcToFast(glm::vec2(b.x - r2, b.y - r2), r2, 0, 3);
			arcs[3] = PathArcToFast(glm::vec2(a.x + r3, b.y - r3), r3, 3, 6);
			for (int i = 1; i < 4; i++)
				ret[i] = arcs[i - 1];
		}
		return ret;
	}
	void can_path::rect2(const glm::vec2& a, const glm::vec2& b, const glm::vec4& rounding)
	{
		{
			auto& r = rounding;
			const float r0 = r.x;
			const float r1 = r.y;
			const float r2 = r.z;
			const float r3 = r.w;
			size_t arcs[4] = {};
			if (r0 > 0)
				arcs[0] = PathArcToFast(glm::vec2(a.x + r0, a.y + r0), r0, 9, 12);
			if (r1 > 0)
				arcs[1] = PathArcToFast(glm::vec2(b.x - r1, a.y + r1), r1, 6, 9);
			if (r2 > 0)
				arcs[2] = PathArcToFast(glm::vec2(a.x + r3, b.y - r3), r2, 0, 3);
			if (r3 > 0)
				arcs[3] = PathArcToFast(glm::vec2(b.x - r2, b.y - r2), r3, 3, 6);
		}
	}

	/*
	椭圆路径生成
	centre中点vec2，radius半径vec2，num_segments数量int，path输出，
	pos从哪个位置开始
	ivec2 range范围，默认{0，-1}全部，
	offset半径缩小，closure是否闭合，false可以省空间

	*/
	size_t can_path::path_ellipse_circle(const glm::vec2& centre, glm::vec2 radius, int num_segments,
		vector_vec2* path, double pos, glm::ivec2 range /*= { 0,-1 }*/,
		double offset /*= 0.0f*/, bool closure /*= true*/, glm::vec4* rect_scale /*= nullptr*/)
	{
		if (!path)
		{
			path = &_points;
		}
		if (num_segments < 3)
		{
			num_segments = 360;
		}
		size_t old = path->size();
		path->reserve(path->size() + (num_segments));
		double rx = radius.x - offset, ry = radius.y - offset;
		double pi2 = _PI * 2.0f / num_segments;
		int64_t n = std::min((unsigned int)(range.y), (unsigned int)num_segments);
		int first = range.x;
#if 1
		if (closure)
		{
			n++;
		}
		for (int64_t i = first; i < n; i++)
		{
			double a = pi2 * (i + pos);
			path->push_back(glm::vec2(centre.x + cosf(a) * rx, centre.y + sinf(a) * ry));
		}
#else
		//1/(x-x0)^2 + b^2/(y-y0)^2 = r^2
		double x, b, y, x0 = centre.x, y0 = centre.y, r = radius;
		auto a = 1.0 / (x - x0) * (x - x0) + b * b / (y - y0) * (y - y0) = r * r;
		for (int64_t i = first; i <= n; i++)
		{
			double x, y;
			x = (1.0 / x - 1.0 / x0) * (x - x0) - radius.x;
			y = b * b / (y - y0) * (y - y0) - radius.y;
			path->push_back(glm::vec2(x, y));
		}
#endif
		return path->size() - old;
	}

	void can_path::path_line(const glm::vec2& a1, const glm::vec2& b1)
	{
		auto a = a1; auto b = b1;
		pad5(a);
		pad5(b);
		_points.push_back(a);
		_points.push_back(b);
	}
	void can_path::sol1(const vector_vec2& path, unsigned int col)  //绘制控制多边形的轮廓//n控制点的个数
	{
		if ((col & HZ_COL32_A_MASK) == 0)
			return;
		auto n = path.size();
		for (size_t i = 0; i < n - 1; i += 2)
			path_line(path[i], path[i + 1]);
	}
	double can_path::sol2(int nn, int k)  //计算多项式的系数C(nn,k)
	{
		int i;
		double sum = 1;
		for (i = 1; i <= nn; i++)
			sum *= i;
		for (i = 1; i <= k; i++)
			sum /= i;
		for (i = 1; i <= nn - k; i++)
			sum /= i;
		return sum;
	}
	void can_path::sol3(double t, glm::vec2* path, size_t count, size_t n)  //计算Bezier曲线上点的坐标
	{
		glm::dvec2 point;
		double Ber;
		for (size_t k = 0; k < count; k++)
		{
			Ber = sol2(n - 1, k) * pow(t, k) * pow(1 - t, n - 1 - k);
			point.x += path[k].x * Ber;
			point.y += path[k].y * Ber;
		}
		_points.push_back(point);
	}
	void can_path::sol4(glm::vec2* path, double m, size_t n, bool first)  //根据控制点，求曲线上的m个点
	{
		for (size_t i = 0; i <= m; i++)
		{
			if (i == 0 && !first)
			{
				continue;
			}
			sol3((double)i / (double)m, path, 4, n);
		}
	}
#endif



	canvas_cx::canvas_cx()
	{
	}
	canvas_cx::~canvas_cx()
	{
	}


	void canvas_cx::set_viewport(const glm::ivec4& v)
	{
		glm::uvec4 vp = viewport;
		//if (vp < v)
		{
			viewport = v;
			vp0 = v;
			if (cmd_data.empty())
			{
				clear();
			}
		}
	}

	glm::ivec4 canvas_cx::get_viewport()
	{
		return viewport;
	}
	glm::ivec4 canvas_cx::get_viewport0()
	{
		return vp0;
	}
	glm::ivec2 canvas_cx::data_size()
	{
		return { (_wptr.vsize + _wptr.vc) * sizeof(vert_t), (_wptr.isize + _wptr.ic) * sizeof(DrawIdx) };
		//return {_vtx_data.size() * sizeof(vert_t), _idx_data.size() * sizeof(DrawIdx)};
	}

	void canvas_cx::clear()
	{
		_vtx_data.clear();					// 顶点数据
		_idx_data.clear();					// 索引数据
		cmd_data.clear();					// 指令数据
		_wptr = {};
		for (; _image_stack.size();)_image_stack.pop();
		for (; _clip_rect.size();)_clip_rect.pop();
		for (; _ubo_stack.size();)_ubo_stack.pop();
		_clip_rect.push({ 0, 0.0, -1, -1 });
		_image_stack.push({ });
		_ubo_stack.push(0);
		update_image();

	}
	//add start
#if 1
	float  InvLength(const glm::vec2& lhs, float fail_value) { float d = lhs.x * lhs.x + lhs.y * lhs.y; if (d > 0.0f) return 1.0 / sqrtf(d); return fail_value; }
	can_path* canvas_cx::get_cpath()
	{
		return &_path;
	}
	// NB: this can be called with negative count for removing primitives (as long as the result does not underflow)
	void canvas_cx::PathFillConvex(unsigned int col, bool isclear, bool a, vector_ivec3* cols)
	{
		draw_convex_poly_filled({ 0, 0 }, _path._points.data(), _path._points.size(), col, a, cols); if (isclear) { _path.clear(); }
	}
	void canvas_cx::PathStroke(unsigned int col, bool closed, float thickness, bool a)
	{
		draw_polyline({ 0, 0 }, _path._points.data(), _path._points.size(), col, closed, thickness, a); _path.clear();
	}
	// NB: this can be called with negative count for removing primitives (as long as the result does not underflow)
	canvas_cx::vidptr_t canvas_cx::PrimReserve(int idx_count, int vtx_count)
	{
		//auto dc = cmd_data.rbegin();
		auto dc = &cmd_data[cmd_data.size() - 1];
		dc->indexCount += idx_count;
		dc->firstIndex;
		dc->vertexOffset;
		auto v = _wptr.vsize + _wptr.vc;
		auto i = _wptr.isize + _wptr.ic;
		//if (v + vtx_count >= _vtx_data.size())
		{
			auto ns = v + vtx_count;
			//ns += _vtx_data.size() * 0.5;
			_vtx_data.resize(ns);
		}
		//if (i + idx_count >= _idx_data.size())
		{
			auto ns = i + idx_count;
			//ns += _idx_data.size() * 0.5;
			_idx_data.resize(ns);
		}
		_wptr = vidptr_t{ &_vtx_data[v], &_idx_data[i], v, i, (size_t)vtx_count, (size_t)idx_count };
		return _wptr;
	}

	void push_vtx(canvas_cx::vidptr_t& od, const vert_t& v)
	{
		assert(od.vc);
		*od.vtx = v; od.vtx++; od.vc--;
	}
	void cpy_vtx(canvas_cx::vidptr_t& od, vert_t* v, size_t n)
	{
		assert(n <= od.vc);
		memcpy(od.vtx, v, sizeof(vert_t) * n); od.vtx += n; od.vc -= n;
	}
	void cpy_idx(canvas_cx::vidptr_t& od, DrawIdx* v, size_t n)
	{
		assert(n <= od.ic);
		memcpy(od.idx, v, sizeof(DrawIdx) * n); od.idx += n; od.ic -= n;
	}
	void push_idx(canvas_cx::vidptr_t& od, DrawIdx v)
	{
		assert(od.ic);
		*od.idx = v; od.idx++; od.ic--;
	}

	// Fully unrolled with  call to keep our debug builds decently fast.
	void canvas_cx::PrimRect(const glm::vec2& a, const glm::vec2& c, unsigned int col)
	{
		glm::vec2 b(c.x, a.y), d(a.x, c.y), uv(_uv_white_pixel);
		DrawIdx idx = _wptr.vsize + _wptr.vc;// _vtx_data.size();
		auto p = PrimReserve(6, 4);
		DrawIdx id[] = { idx, (idx + 1), (idx + 2), idx, (idx + 2), (idx + 3) };
		vert_t vd[4] = { {a, uv, col}, {b, uv, col}, {c, uv, col}, {d, uv, col} };
		cpy_idx(p, id, 6);
		cpy_vtx(p, vd, 4);
	}
#define gmax(x,y) (x>y?x:y)
#define gmin(x, y) (x < y ? x : y)
	glm::vec4 merge2(glm::vec4 r1, glm::vec4 r2) {
		glm::vec4 r;
		r.x = r1.x < r2.x ? r1.x : r2.x;
		r.z = r1.z > r2.z ? r1.z : r2.z;
		r.y = r1.y < r2.y ? r1.y : r2.y;
		r.w = r1.w > r2.w ? r1.w : r2.w;
		return r;
	}

	glm::vec4 getBothArea(glm::vec4 r1, glm::vec4 r2) {
		auto& a = r1;
		auto& b = r2;
		a.z += a.x;
		b.z += b.x;
		a.w += a.y;
		b.w += b.y;
		glm::vec4 merge = merge2(r1, r2);
		glm::vec4 both;
		both.x = r1.x == merge.x ? r2.x : r1.x;
		both.z = r1.z == merge.z ? r2.z : r1.z;
		both.y = r1.y == merge.y ? r2.y : r1.y;
		both.w = r1.w == merge.w ? r2.w : r1.w;
		both.z -= both.x;
		both.w -= both.y;
		return both;
	}
	glm::vec4 rect_cross1(glm::vec4 a, glm::vec4 b)
	{
		glm::vec4 r;
		double m1, n1, m2, n2;
		a.z += a.x;
		b.z += b.x;
		a.w += a.y;
		b.w += b.y;
		m1 = gmax(gmin(a.x, a.z), gmin(b.x, b.z));
		n1 = gmax(gmin(a.y, a.w), gmin(b.y, b.w));
		m2 = gmin(gmax(a.x, a.z), gmax(b.x, b.z));
		n2 = gmin(gmax(a.y, a.w), gmax(b.y, b.w));
		if (m2 > m1 && n2 > n1) {
			r = { m1, n1, m2 - m1, n2 - n1 };
		}
		else {
			r = r;
		}
		return r;
	}
	void canvas_cx::push_clipRect(glm::vec2 cr_min, glm::vec2 cr_max, bool intersect_with_current_clip_rect)
	{
		glm::vec4 cr(cr_min.x, cr_min.y, cr_max.x, cr_max.y);
		if (intersect_with_current_clip_rect && _clip_rect.size())
		{
			glm::uvec4 current = _clip_rect.top();
			//bool cross = math_cx::rect_cross(current, cr);
			//if (cross)
			cr = rect_cross1(current, cr);
		}
		if (!(cr.z > 0))
			cr = { -1,-1,1,1 };
		_clip_rect.push(cr);
		update_clipRect();
	}
	void canvas_cx::push_clipRect(glm::vec4 cr, bool intersect_with_current_clip_rect)
	{
		push_clipRect({ cr.x, cr.y }, { cr.z, cr.w }, intersect_with_current_clip_rect);
	}

	void canvas_cx::push_full_clip()
	{
		push_clipRect(viewport, false);
	}

	void canvas_cx::pop_clipRect()
	{
		if (_clip_rect.size() > 0)
		{
			assert(_clip_rect.size() > 0);
			_clip_rect.pop();
			update_clipRect();
		}
	}
	void canvas_cx::AddDrawCmd()
	{
		DrawCmd draw_cmd;
		draw_cmd.clipRect = _clip_rect.top();
		auto cti = _image_stack.top();
		draw_cmd.image = (cti);
		draw_cmd.ubo = _ubo_stack.top();
		//assert(draw_cmd.clipRect.x <= draw_cmd.clipRect.z && draw_cmd.clipRect.y <= draw_cmd.clipRect.w);
		cmd_data.push_back(draw_cmd);
	}

	void canvas_cx::push_image(Image* user_image)
	{
		if (!user_image)
		{
			return;
		}
		_image_stack.push({ user_image, 0 });
		_img_set.insert(user_image);
		if (user_image->width == 286)
		{
			//printf("");
		}
		update_image();
	}

	void canvas_cx::push_image(dvk_texture* user_image)
	{
		_image_stack.push({ user_image, 1 });
		_tex_set.insert(user_image);
		auto cp = update_image();
		if (cp)
		{
			cp->pipe = user_image->pipe;
		}
	}

	void canvas_cx::pop_image()
	{
		if (_image_stack.size() > 0)
		{
			_image_stack.pop();
			update_image();
		}
		imginc--;
	}
	void canvas_cx::push_ubo(motion_t* user_ubo) {
		assert(user_ubo);
		if (user_ubo)
		{
			user_ubo->viewport_ptr = this;
			_ubo_stack.push(user_ubo);
			_ubo_set.insert(user_ubo);
			update_ubo();
		}
	}
	void canvas_cx::pop_ubo() {

		if (_ubo_stack.size() > 0)
		{
			_ubo_stack.pop();
			update_ubo();
		}
	}
	void canvas_cx::add_obj(const void* pd, size_t n, size_t stride, size_t offset)
	{
		draw_info_t* p = (draw_info_t*)pd;
		if (!p || !n || !(p->_type < (geo_type_t)geo_type::nums))
			return;
		size_t ic = 0;
		char* pt = (char*)p;
		pt += offset;
		for (; n > 0; n--) {
			geo_type t = p->_type.e;
			switch (t)
			{
			case geo_type::line:
			{
				auto o = (lines_info_t*)p;
				if (o->d)
				{
					//draw_lines(o->d->data(), o->d->size(), o->col, o->thickness, o->anti_aliased);
				}
				else {
					draw_line({ o->a.x, o->a.y }, { o->a.z, o->a.w }, o->col, o->thickness, o->anti_aliased);
				}
				if (!stride) ic = sizeof(*o);
			}break;
			case geo_type::polyline:
			{
				auto o = (polyline_info_t*)p;
				draw_polyline(o->pos, o->points, o->points_count, o->col, o->closed, o->thickness, o->anti_aliased);
				if (!stride) ic = sizeof(*o);
			}break;
			case geo_type::polygon:
			{
				auto o = (polygon_info_t*)p;
				draw_polyline(o->pos, o->points, o->points_count, o->col, true, o->thickness, o->anti_aliased);
				if (!stride) ic = sizeof(*o);
			}break;
			case geo_type::poly_filled:
			{
				auto o = (convex_poly_filled_info_t*)p;
				draw_convex_poly_filled(o->pos, o->points, o->points_count, o->col, o->anti_aliased, o->cols);
				if (!stride) ic = sizeof(*o);
			}	break;
			case geo_type::path_filled:
			{
				auto o = (convex_path_filled_info_t*)p;
				draw_convex_path_filled(o->pos, o->points, o->points_count, o->col, o->anti_aliased, o->cols);
				if (!stride) ic = sizeof(*o);
			}	break;
			case geo_type::path:
			{
				auto o = (svg_path_t*)p;
				draw_svg_path(o);
				if (!stride) ic = sizeof(*o);
			}	break;
			case geo_type::rect:
			{
				auto o = (rect_info_t*)p;
				if (o->cols)
					draw_rect_filled_multi_color(o->a, o->cols, o->rounding, o->rounding_corners_flags);
				else if (o->cols4)
					draw_rect_filled_multi_color(o->a, o->cols4->x, o->cols4->y, o->cols4->z, o->cols4->w);
				else
					draw_rect(o->a, o->col, o->fill, o->rounding, o->rounding_corners_flags, o->thickness);

				if (!stride) ic = sizeof(*o);
			}	break;
			/*		case geo_type::rect_filled_gradient:
					{
						auto o = (lines_info_t*)p;
						draw_lines(o->d, o->n, o->col, o->thickness, o->anti_aliased);
					}	break;*/
			case geo_type::triangle:
			{
				auto o = (triangle_info_t*)p;
				draw_triangle(o->a, o->b, o->c, o->col, o->fill, o->thickness, o->anti_aliased);
				if (!stride) ic = sizeof(*o);
			}	break;
			case geo_type::quad:
			{
				auto o = (quad_info_t*)p;
				draw_quad(o->a, o->b, o->c, o->d, o->col, o->fill, o->thickness);
				if (!stride) ic = sizeof(*o);
			}	break;
			case geo_type::circle:
			{
				auto o = (circle_info_t*)p;
				draw_circle(o->centre, o->radius, o->col, o->fill, o->num_segments, o->thickness);
				if (!stride) ic = sizeof(*o);
			}	break;
			case geo_type::ellipse:
			{
				auto o = (ellipse_info_t*)p;
				draw_ellipse(o->centre, o->rx, o->ry, o->col, o->fill, o->num_segments, o->thickness);
				if (!stride) ic = sizeof(*o);
			}	break;
			case geo_type::arc:
			{
				auto o = (arc_info_t*)p;
				draw_arc(o->centre, o->rx, o->ry, o->col, o->fill, o->num_start, o->num_end, o->num_segments, o->thickness);
				if (!stride) ic = sizeof(*o);
			}	break;
			case geo_type::bezier_curve:
			{
				auto o = (bezier_curve_info_t*)p;
				draw_bezier_curve(o);
				if (!stride) ic = sizeof(*o);
			}	break;
			//case geo_type::grid:
			//{
			//	auto o = (lines_info_t*)p;
			//	draw_lines(o->d, o->n, o->col, o->thickness, o->anti_aliased);
			//	if (!stride) ic = sizeof(*o);
			//}	break;
			case geo_type::radial_gradient:
			{
				auto o = (radial_gradient_info_t*)p;
				draw_radial_gradient(o);
				if (!stride) ic = sizeof(*o);
			}	break;
			case geo_type::linear_gradient:
			{
				auto o = (linear_gradient_info_t*)p;
				draw_linear_gradient(o->rect, o->pstops, o->dire);
				if (!stride) ic = sizeof(*o);
			}	break;
			case geo_type::poly_gradient:
			{
				auto o = (poly_gradient_info_t*)p;
				draw_poly_gradient(o->center, o->radius, o->c, o->n, o->bc, o->nums);
				if (!stride) ic = sizeof(*o);
			}	break;
			case geo_type::image:
			{
				auto o = (image_info_t*)p;
				if (o->repeat > 0)
				{
					draw_image_repeat(o->user_image, o->a, o->repeat, o->col);
				}
				else {
					draw_image(o->user_image, o->a, o->rect, o->sliced, o->col);
				}
				if (!stride) ic = sizeof(*o);
			}	break;
			case geo_type::text:
			{
				auto o = (text_info_t*)p;
				draw_text(o);
				if (!stride) ic = sizeof(*o);
			}	break;
			case geo_type::label:
			{
				auto o = (label_info_t*)p;
				draw_text(o->itbox, o->pos, o->color);
				if (!stride) ic = sizeof(*o);
			}	break;
			default:
				break;
			}
			pt += ic; p = (draw_info_t*)pt;
		}
	}
	bool canvas_cx::is_last(DrawCmd* prev_cmd)
	{
		//int cr = memcmp(&prev_cmd->clipRect, &clip, sizeof(glm::vec4));
		return (_image_stack.size() && prev_cmd && prev_cmd->image.p.img == _image_stack.top().p.img && prev_cmd->clipRect == _clip_rect.top() && prev_cmd->ubo == _ubo_stack.top());
	}
	DrawCmd* canvas_cx::update_clipRect()
	{
		// If current command is used with different settings we need to add a new command
		glm::ivec4 curr_clip_rect = _clip_rect.top();
		DrawCmd* curr_cmd = cmd_data.size() ? &cmd_data.back() : NULL;
		if (!curr_cmd || curr_cmd->indexCount)
		{
			if (!is_last(curr_cmd))
			{
				AddDrawCmd();
			}
			return &cmd_data.back();
		}
		// Try to merge with previous command if it matches, else use current command
		DrawCmd* prev_cmd = cmd_data.size() > 1 ? curr_cmd - 1 : NULL;
		if (is_last(prev_cmd))
			cmd_data.pop_back();
		else if (curr_cmd)
			curr_cmd->clipRect = curr_clip_rect;
		return &cmd_data.back();
	}
	DrawCmd* canvas_cx::update_image()
	{
		auto curr_texture_id = _image_stack.top();
		DrawCmd* curr_cmd = cmd_data.size() ? &cmd_data.back() : NULL;
		if (!curr_cmd || curr_cmd->indexCount)
		{
			if (!is_last(curr_cmd))
			{
				AddDrawCmd();
			}
			return &cmd_data.back();
		}
		// Try to merge with previous command if it matches, else use current command
		DrawCmd* prev_cmd = cmd_data.size() > 1 ? curr_cmd - 1 : NULL;
		if (is_last(prev_cmd))
			cmd_data.pop_back();
		else
			curr_cmd->image = curr_texture_id;
		return &cmd_data.back();
	}

	DrawCmd* canvas_cx::update_ubo()
	{
		auto top_ubo = _ubo_stack.top();
		DrawCmd* curr_cmd = cmd_data.size() ? &cmd_data.back() : NULL;
		if (!curr_cmd || curr_cmd->indexCount)
		{
			if (!is_last(curr_cmd))
			{
				AddDrawCmd();
			}
			return &cmd_data.back();
		}
		// Try to merge with previous command if it matches, else use current command
		DrawCmd* prev_cmd = cmd_data.size() > 1 ? curr_cmd - 1 : NULL;
		if (is_last(prev_cmd))
			cmd_data.pop_back();
		else
			curr_cmd->ubo = top_ubo;
		return &cmd_data.back();
	}


	// TODO: Thickness anti-aliased lines cap are missing their AA fringe.
	void canvas_cx::draw_polyline(const glm::vec2& pos, const glm::vec2* points, const int points_count, unsigned int col, bool closed, float thickness, bool anti_aliased)
	{
		if (points_count < 2)
			return;

		const glm::vec2 uv = _uv_white_pixel;
		//anti_aliased &= GImGui->Style.AntiAliasedLines;
		//if (ImGui::GetIO().KeyCtrl) anti_aliased = false; // Debug

		int count = points_count;
		float distTol = 0.01f;
		auto p0 = points;
		auto p1 = points + points_count - 1;
		//bool cod = (ptEquals(p0->x, p0->y, p1->x, p1->y, distTol));
		//if (cod)closed = false;
		if (!closed)
			count = points_count - 1;

		const bool thick_line = thickness > 1.0f;
		if (anti_aliased)
		{
			// Anti-aliased stroke
			const float AA_SIZE = 1.0f;
			const unsigned int col_trans = col & 0x00ffffff;// IM_COL32(255, 255, 255, 0);

			const int idx_count = thick_line ? count * 18 : count * 12;
			const int vtx_count = thick_line ? points_count * 4 : points_count * 3;
			auto od = PrimReserve(idx_count, vtx_count);

			// Temporary buffer
			//vector_vec2 temp_normalsdata(points_count * (thick_line ? 5 : 3));

			temp_normalsd.resize(points_count * (thick_line ? 5 : 3));
			glm::vec2* temp_normals = temp_normalsd.data();
			glm::vec2* temp_points = temp_normals + points_count;

			for (int i1 = 0; i1 < count; i1++)
			{
				const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
				glm::vec2 diff = points[i2] - points[i1];
				diff *= InvLength(diff, 1.0f);
				temp_normals[i1].x = diff.y;
				temp_normals[i1].y = -diff.x;
			}
			if (!closed)
				temp_normals[points_count - 1] = temp_normals[points_count - 2];

			if (!thick_line)
			{
				if (!closed)
				{
					temp_points[0] = pos + points[0] + temp_normals[0] * AA_SIZE;
					temp_points[1] = pos + points[0] - temp_normals[0] * AA_SIZE;
					temp_points[(points_count - 1) * 2 + 0] = pos + points[points_count - 1] + temp_normals[points_count - 1] * AA_SIZE;
					temp_points[(points_count - 1) * 2 + 1] = pos + points[points_count - 1] - temp_normals[points_count - 1] * AA_SIZE;
				}

				// FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
				unsigned int idx1 = od.vsize;
				for (int i1 = 0; i1 < count; i1++)
				{
					const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
					unsigned int idx2 = (i1 + 1) == points_count ? od.vsize : idx1 + 3;

					// Average normals
					glm::vec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
					float dmr2 = dm.x * dm.x + dm.y * dm.y;
					if (dmr2 > 0.000001f)
					{
						float scale = 1.0 / dmr2;
						if (scale > 100.0f) scale = 100.0f;
						dm *= scale;
					}
					dm *= AA_SIZE;
					temp_points[i2 * 2 + 0] = pos + points[i2] + dm;
					temp_points[i2 * 2 + 1] = pos + points[i2] - dm;
					int iwp[12] = { 0 };
					// Add indexes
					std::array<DrawIdx, 12> widx = {
						(idx2 + 0), (idx1 + 0), (idx1 + 2),
						(idx1 + 2), (idx2 + 2), (idx2 + 0),
						(idx2 + 1), (idx1 + 1), (idx1 + 0),
						(idx1 + 0), (idx2 + 0), (idx2 + 1) };
					cpy_idx(od, widx.data(), 12);
					for (int nk = 0; nk < 12; nk++)
					{
						iwp[nk] = widx[nk];
					}
					//widx += 12;
					idx1 = idx2;
				}

				// Add vertexes
				for (int i = 0; i < points_count; i++)
				{
					//mcpy_idx(od, widx.data(), 12);
					push_vtx(od, { pos + points[i], uv, col });
					push_vtx(od, { temp_points[i * 2 + 0], uv, col_trans });
					push_vtx(od, { temp_points[i * 2 + 1], uv, col_trans });
				}
			}
			else
			{
				const float half_inner_thickness = (thickness - AA_SIZE) * 0.5f;
				if (!closed)
				{
					temp_points[0] = pos + points[0] + temp_normals[0] * (half_inner_thickness + AA_SIZE);
					temp_points[1] = pos + points[0] + temp_normals[0] * (half_inner_thickness);
					temp_points[2] = pos + points[0] - temp_normals[0] * (half_inner_thickness);
					temp_points[3] = pos + points[0] - temp_normals[0] * (half_inner_thickness + AA_SIZE);
					temp_points[(points_count - 1) * 4 + 0] = pos + points[points_count - 1] + temp_normals[points_count - 1] * (half_inner_thickness + AA_SIZE);
					temp_points[(points_count - 1) * 4 + 1] = pos + points[points_count - 1] + temp_normals[points_count - 1] * (half_inner_thickness);
					temp_points[(points_count - 1) * 4 + 2] = pos + points[points_count - 1] - temp_normals[points_count - 1] * (half_inner_thickness);
					temp_points[(points_count - 1) * 4 + 3] = pos + points[points_count - 1] - temp_normals[points_count - 1] * (half_inner_thickness + AA_SIZE);
				}

				// FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
				unsigned int idx1 = od.vsize;
				for (int i1 = 0; i1 < count; i1++)
				{
					const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
					unsigned int idx2 = (i1 + 1) == points_count ? od.vsize : idx1 + 4;

					// Average normals
					glm::vec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
					float dmr2 = dm.x * dm.x + dm.y * dm.y;
					if (dmr2 > 0.000001f)
					{
						float scale = 1.0 / dmr2;
						if (scale > 100.0f) scale = 100.0f;
						dm *= scale;
					}
					glm::vec2 dm_out = dm * (half_inner_thickness + AA_SIZE);
					glm::vec2 dm_in = dm * half_inner_thickness;
					temp_points[i2 * 4 + 0] = pos + points[i2] + dm_out;
					temp_points[i2 * 4 + 1] = pos + points[i2] + dm_in;
					temp_points[i2 * 4 + 2] = pos + points[i2] - dm_in;
					temp_points[i2 * 4 + 3] = pos + points[i2] - dm_out;

					// Add indexes
					DrawIdx widx[18] = {};
					widx[0] = (idx2 + 1); widx[1] = (idx1 + 1); widx[2] = (idx1 + 2);
					widx[3] = (idx1 + 2); widx[4] = (idx2 + 2); widx[5] = (idx2 + 1);
					widx[6] = (idx2 + 1); widx[7] = (idx1 + 1); widx[8] = (idx1 + 0);
					widx[9] = (idx1 + 0); widx[10] = (idx2 + 0); widx[11] = (idx2 + 1);
					widx[12] = (idx2 + 2); widx[13] = (idx1 + 2); widx[14] = (idx1 + 3);
					widx[15] = (idx1 + 3); widx[16] = (idx2 + 3); widx[17] = (idx2 + 2);
					cpy_idx(od, widx, 18);
					idx1 = idx2;
				}
				// Add vertexes
				for (int i = 0; i < points_count; i++)
				{
					push_vtx(od, { temp_points[i * 4 + 0], uv, col_trans });
					push_vtx(od, { temp_points[i * 4 + 1], uv, col });
					push_vtx(od, { temp_points[i * 4 + 2], uv, col });
					push_vtx(od, { temp_points[i * 4 + 3], uv, col_trans });
				}
			}
		}
		else
		{
			// Non Anti-aliased Stroke
			const int idx_count = count * 6;
			const int vtx_count = count * 4;      // FIXME-OPT: Not sharing edges
			auto od = PrimReserve(idx_count, vtx_count);
			DrawIdx curridx = od.vsize;
			for (int i1 = 0; i1 < count; i1++)
			{
				const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
				glm::vec2 p1 = pos + points[i1];
				glm::vec2 p2 = pos + points[i2];
				glm::vec2 diff = p2 - p1;
				diff *= InvLength(diff, 1.0f);

				const float dx = diff.x * (thickness * 0.5f);
				const float dy = diff.y * (thickness * 0.5f);
				push_vtx(od, { {p1.x + dy, p1.y - dx}, uv, col });
				push_vtx(od, { {p2.x + dy, p2.y - dx}, uv, col });
				push_vtx(od, { {p2.x - dy, p2.y + dx}, uv, col });
				push_vtx(od, { {p1.x - dy, p1.y + dx}, uv, col });
				DrawIdx widx[6] = { curridx, curridx + 1, curridx + 2, curridx, curridx + 2, curridx + 3 };
				cpy_idx(od, widx, 6);

			}
		}
	}
	// 填充多颜色cols
	void canvas_cx::draw_convex_poly_filled(const glm::vec2& pos, const glm::vec2* points, const int points_count, unsigned int col, bool anti_aliased, vector_ivec3* cols)
	{
		const glm::vec2 uv = _uv_white_pixel;

		glm::ivec3* ct = nullptr;
		int ctlen = 0;
		if (cols)
		{
			ct = cols->data();
			ctlen = cols->size();
		}
		if (anti_aliased)
		{
			// Anti-aliased Fill
			const float AA_SIZE = 1.0f;
			const unsigned int col_trans = col & 0x00ffffff;
			const int idx_count = (points_count - 2) * 3 + points_count * 6;
			const int vtx_count = (points_count * 2);
			auto od = PrimReserve(idx_count, vtx_count);

			// Add indexes for fill
			unsigned int vtx_inner_idx = od.vsize;
			unsigned int vtx_outer_idx = vtx_inner_idx + 1;

			for (int i = 2; i < points_count; i++)
			{
				DrawIdx widx[6] = { vtx_inner_idx, vtx_inner_idx + ((i - 1) << 1), vtx_inner_idx + (i << 1) };
				cpy_idx(od, widx, 3);
			}

			// Compute normals
			temp_normalsd.resize(points_count);
			glm::vec2* temp_normals = temp_normalsd.data();
			for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
			{
				glm::vec2 p0 = pos + points[i0];
				glm::vec2 p1 = pos + points[i1];
				glm::vec2 diff = p1 - p0;
				diff *= InvLength(diff, 1.0f);
				temp_normals[i0].x = diff.y;
				temp_normals[i0].y = -diff.x;
			}

			unsigned int inner_col = col, outer_col = col_trans;
			for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
			{
				// Average normals
				const glm::vec2& n0 = temp_normals[i0];
				const glm::vec2& n1 = temp_normals[i1];
				glm::vec2 dm = (n0 + n1) * 0.5f;
				float dmr2 = dm.x * dm.x + dm.y * dm.y;
				if (dmr2 > 0.000001f)
				{
					float scale = 1.0 / dmr2;
					if (scale > 100.0f) scale = 100.0f;
					dm *= scale;
				}
				dm *= AA_SIZE * 0.5f;

				if (ct)
				{
					if (ct->y == i1)
					{
						ct++; inner_col = col; outer_col = col_trans;
						if (0 > --ctlen)ct = 0;
					}
					if (ct->x == i1)
					{
						inner_col = ct->z; outer_col = ct->z & 0x00ffffff;
					}
				}
				// Add vertices
				push_vtx(od, { pos + points[i1] - dm, uv, inner_col });  // Inner
				push_vtx(od, { pos + points[i1] + dm, uv, outer_col });  // Outer
				// Add indexes for fringes
				DrawIdx widx[6] = { (vtx_inner_idx + (i1 << 1)), (vtx_inner_idx + (i0 << 1)), (vtx_outer_idx + (i0 << 1)),
					(vtx_outer_idx + (i0 << 1)), (vtx_outer_idx + (i1 << 1)), (vtx_inner_idx + (i1 << 1)) };
				cpy_idx(od, widx, 6);
			}
		}
		else
		{
			// Non Anti-aliased Fill
			const int idx_count = (points_count - 2) * 3;
			const int vtx_count = points_count;
			auto od = PrimReserve(idx_count, vtx_count);
			DrawIdx vtxidx = od.vsize;
			unsigned int inner_col = col;
			for (int i = 0; i < vtx_count; i++)
			{
				if (ct)
				{
					if (ct->y == i)
					{
						ct++; inner_col = col;
						if (0 > --ctlen)ct = 0;
					}
					if (ct->x == i)
					{
						inner_col = ct->z;
					}
				}
				push_vtx(od, { pos + points[i], uv, inner_col });
			}
			vector_ivec3 ts;

			auto is_intersection = [=](const glm::ivec3& idx)
			{
				glm::vec2 vidx[3] = { pos + points[idx.x], pos + points[idx.y], pos + points[idx.z] };
				glm::vec4 v4[3] = { {vidx[0], vidx[1]}, {vidx[0], vidx[2]}, {vidx[1], vidx[2]} };
				for (auto it : ts)
				{
					glm::vec2 vidx[] = { pos + points[it.x], pos + points[it.y], pos + points[it.z] };
					glm::vec4 it4[3] = { {vidx[0], vidx[1]}, {vidx[0], vidx[2]}, {vidx[1], vidx[2]} };
					for (int i = 0; i < 3; i++)
					{
						for (int j = 0; j < 3; j++)
						{
							if (intersection(v4[j], it4[i]))
							{
								return false;
							}
						}

					}
				}
				return true;
			};
			int cur = 0;
			for (int i = 2; i < points_count; i++)
			{
				if (is_intersection(glm::ivec3(cur, i - 1, i)))
				{
					ts.push_back(glm::ivec3(cur, i - 1, i));
					DrawIdx widx[6] = { (vtxidx), (vtxidx + i - 1), (vtxidx + i) };
					cpy_idx(od, widx, 3);
				}
				else
				{
					i = i;
				}
				cur += 3;
			}
		}
	}


	// 填充多边形
	void canvas_cx::draw_convex_path_filled(const glm::vec2& pos, const glm::vec2* points, const int points_count, unsigned int col, bool anti_aliased, vector_ivec3* cols /*= nullptr*/)
	{
		const glm::vec2 uv = _uv_white_pixel;

		glm::ivec3* ct = nullptr;
		int ctlen = 0;
		if (cols)
		{
			ct = cols->data();
			ctlen = cols->size();
		}
		if (anti_aliased)
		{
			// Anti-aliased Fill
			const float AA_SIZE = 1.0f;
			const unsigned int col_trans = col & 0x00ffffff;
			const int idx_count = (points_count - 2) * 3 + points_count * 6;
			const int vtx_count = (points_count * 2);
			auto od = PrimReserve(idx_count, vtx_count);

			// Add indexes for fill
			unsigned int vtx_inner_idx = od.vsize;
			unsigned int vtx_outer_idx = vtx_inner_idx + 1;
			for (int i = 2; i < points_count; i++)
			{
				DrawIdx widx[6] = { (vtx_inner_idx), (vtx_inner_idx + ((i - 1) << 1)), (vtx_inner_idx + (i << 1)) };
				cpy_idx(od, widx, 3);
			}

			// Compute normals
			temp_normalsd.resize(points_count);
			//vector_vec2 temp_normalsd(points_count);
			glm::vec2* temp_normals = temp_normalsd.data();
			for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
			{
				glm::vec2 p0 = pos + points[i0];
				glm::vec2 p1 = pos + points[i1];
				glm::vec2 diff = p1 - p0;
				diff *= InvLength(diff, 1.0f);
				temp_normals[i0].x = diff.y;
				temp_normals[i0].y = -diff.x;
			}

			unsigned int inner_col = col, outer_col = col_trans;
			for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
			{
				// Average normals
				const glm::vec2& n0 = temp_normals[i0];
				const glm::vec2& n1 = temp_normals[i1];
				glm::vec2 dm = (n0 + n1) * 0.5f;
				float dmr2 = dm.x * dm.x + dm.y * dm.y;
				if (dmr2 > 0.000001f)
				{
					float scale = 1.0 / dmr2;
					if (scale > 100.0f) scale = 100.0f;
					dm *= scale;
				}
				dm *= AA_SIZE * 0.5f;

				if (ct)
				{
					if (ct->y == i1)
					{
						ct++; inner_col = col; outer_col = col_trans;
						if (0 > --ctlen)ct = 0;
					}
					if (ct->x == i1)
					{
						inner_col = ct->z; outer_col = ct->z & 0x00ffffff;
					}
				}
				// Add vertices
				push_vtx(od, { (pos + points[i1] - dm), uv, inner_col });  // Inner
				push_vtx(od, { (pos + points[i1] + dm), uv, outer_col });  // Outer

				// Add indexes for fringes

				DrawIdx widx[6] = { (vtx_inner_idx + (i1 << 1)), (vtx_inner_idx + (i0 << 1)), (vtx_outer_idx + (i0 << 1)),
					(vtx_outer_idx + (i0 << 1)), (vtx_outer_idx + (i1 << 1)), (vtx_inner_idx + (i1 << 1)) };

				cpy_idx(od, widx, 6);
			}
		}
		else
		{
			// Non Anti-aliased Fill
			const int idx_count = (points_count - 2) * 3;
			const int vtx_count = points_count;
			auto od = PrimReserve(idx_count, vtx_count);
			DrawIdx vci = od.vsize;
			unsigned int inner_col = col;
			for (int i = 0; i < vtx_count; i++)
			{
				if (ct)
				{
					if (ct->y == i)
					{
						ct++; inner_col = col;
						if (0 > --ctlen)ct = 0;
					}
					if (ct->x == i)
					{
						inner_col = ct->z;
					}
				}
				push_vtx(od, { pos + points[i], uv, inner_col });
			}
			for (int i = 2; i < points_count; i++)
			{
				DrawIdx widx[6] = { (vci), (vci + i - 1), (vci + i) };
				cpy_idx(od, widx, 3);
			}
		}
	}
#if 1


	void canvas_cx::draw_line(const glm::vec2& a1, const glm::vec2& b1, unsigned int col, float thickness, bool anti_aliased)
	{
		if ((col & HZ_COL32_A_MASK) == 0)
			return;

		auto a = a1; auto b = b1;
		pad5(a);
		pad5(b);
		_path.line_to(a);
		_path.line_to(b);
		PathStroke(col, false, thickness, anti_aliased);
	}
	void canvas_cx::draw_lines(const glm::vec2& pos, const glm::vec2* d, size_t n, unsigned int col, float thickness, bool anti_aliased)
	{
		if ((col & HZ_COL32_A_MASK) == 0 || n < 2 || !d)
			return;
		auto t = d;
		for (size_t i = 0; i < n; i++, t++)
		{
			glm::vec2 a = *t + pos;
			pad5(a);
			_path.line_to(a);
		}
		PathStroke(col, false, thickness, anti_aliased);
	}
	//auto path = canvas->get_cpath();
	//path->line_to(glm::vec2(bx - third, by - third));
	//path->line_to(glm::vec2(bx, by));
	//path->line_to(glm::vec2(bx + third * 2.0f, by - third * 2.0f));
	//canvas->PathStroke(col, false, thickness);
	// a: upper-left, b: lower-right. we don't render 1 px sized rectangles properly.
	void canvas_cx::add_rect_a(const glm::vec2& a, const glm::vec2& b, unsigned int col, unsigned int fill, float rounding /*= 0.0f*/, int rounding_corners_flags /*= ~0*/, float thickness /*= 1.0f*/)
	{
		bool iscol = (col & HZ_COL32_A_MASK);
		float pof = iscol ? 1.0 : 0.0;
		if ((fill & HZ_COL32_A_MASK))
		{
			add_rect_filled(a + (thickness * pof), b - (thickness * pof), fill, rounding, rounding_corners_flags);
		}
		if (iscol)
		{
			auto a1 = a; auto b1 = b;
			pad5(a1);
			pad5(b1, -0.5);
			_path.PathRect(a1, b1, rounding, rounding_corners_flags);
			PathStroke(col, true, thickness);
		}
	}
	void canvas_cx::draw_rect(const glm::vec2& pos, const glm::vec2& size, unsigned int col, unsigned int fill, glm::vec4 r, float thickness)
	{
		if (nohas_clip({ pos, size }))
			return;
		glm::vec2 a = pos, b = a + size;
		bool iscol = (col & HZ_COL32_A_MASK);
		float pof = iscol ? 1.0 : 0.0;
		//glm::vec4 r = { rounding[0],rounding[1],rounding[2],rounding[3] };

		if ((fill & HZ_COL32_A_MASK))
		{
			vector_ivec3* cols = 0;
			add_rect_filled(a + (thickness * pof), b - (thickness * pof), fill, r, cols);
		}
		if (iscol)
		{
			pad5(a);
			pad5(b, -0.5);
			_path.PathRect(a, b, r);
			PathStroke(col, true, thickness);
		}
	}
	void canvas_cx::draw_rect(const glm::vec4& rc, unsigned int col, unsigned int fill, const glm::vec4& rounding, float thickness /*= 1.0f*/)
	{
		if (nohas_clip(rc))
			return;
		glm::vec2 a = { rc.x, rc.y }, b = { a.x + rc.z, a.y + rc.w };
		bool iscol = (col & HZ_COL32_A_MASK);
		float pof = iscol ? 1.0 : 0.0;
		if ((fill & HZ_COL32_A_MASK))
		{
			vector_ivec3* cols = 0;
			add_rect_filled(a + (thickness * pof), b - (thickness * pof), fill, rounding, cols);
		}
		if (iscol)
		{
			pad5(a);
			pad5(b, -0.5);
			_path.PathRect(a, b, rounding);
			PathStroke(col, true, thickness);
		}
	}
	void canvas_cx::draw_rect_filled_multi_color(const glm::vec4& a, vector_ivec3* cols, float rounding /*= 0.0f*/, int rounding_corners_flags /*= ~0*/)
	{
		add_rect_filled({ a.x, a.y }, { a.x + a.z, a.y + a.w }, 0, rounding, rounding_corners_flags, cols);
	}

	void canvas_cx::draw_rect(const glm::vec2& pos, const glm::vec2& size, unsigned int col, unsigned int fill, float rounding, float thickness)
	{
		if (nohas_clip({ pos, size }))
			return;
		rect_temp_c rt = {};
		rt.size = size;
		rt.rounding = rounding;
		rt.thickness = thickness;
		glm::vec2 a = pos, b = a + size;
		bool iscol = (col & HZ_COL32_A_MASK);
		float pof = iscol ? 0.5 : 0.0;
		if (_rcc != rt) {
			if ((fill & HZ_COL32_A_MASK))
			{
				add_rect_filled(a + (thickness * pof), b - (thickness * pof), fill, rounding, ~0);
			}
			if (iscol)
			{
				pad5(a);
				pad5(b, -0.5);
				_path.PathRect(a, b, rounding);
				PathStroke(col, true, thickness);
			}
			_rcc = rt;
		}
		else {
			if ((fill & HZ_COL32_A_MASK))
			{
				add_rect_filled(a + (thickness * pof), b - (thickness * pof), fill, rounding, ~0);
			}
			if (iscol)
			{
				pad5(a);
				pad5(b, -0.5);
				_path.PathRect(a, b, rounding);
				PathStroke(col, true, thickness);
			}
		}
	}

	void canvas_cx::add_rect_filled(const glm::vec2& a, const glm::vec2& b, unsigned int col, const glm::vec4& rounding, vector_ivec3* cols)
	{
		if (!cols && (col & HZ_COL32_A_MASK) == 0)
			return;
		if (rounding.x > 0.0f || (cols && !col))
		{
			_path.PathRect(a, b, rounding);
			if (cols && cols->size())
			{
				size_t count = _path.size(), cs = cols->size();

				// 如果 cols[0].x小于0
				if (cs <= 4 && (*cols)[0].x < 0)
				{
					auto n = count * 0.25;
					float k = 1.0 / cs;
					auto& tem = *cols;
					int c = 0;
					vector_ivec3 ncs;
					for (int i = 0; i < cs; i++)
					{
						tem[i].x = c;
						c += n;
						tem[i].y = c;
					}

				}
			}
			else {
				cols = nullptr;
			}
			PathFillConvex(col, true, true, cols);
		}
		else
		{
			PrimRect(a, b, col);
		}

	}

	void canvas_cx::add_rect_filled(const glm::vec2& a, const glm::vec2& b, unsigned int col, float rounding /*= 0.0f*/, int rounding_corners_flags /*= ~0*/, vector_ivec3* cols /*= nullptr*/, float offset /*= 0.5*/)
	{
		if (!cols && (col & HZ_COL32_A_MASK) == 0)
			return;
		if (rounding > 0.0f || (cols && !col))
		{
			_path.PathRect(a, b, rounding, rounding_corners_flags);
			if (cols && cols->size())
			{
				size_t count = _path.size(), cs = cols->size();

				// 如果 cols[0].x小于0
				if (cs <= 4 && (*cols)[0].x < 0)
				{
					auto n = count * 0.25;
					float k = 1.0 / cs;
					auto& tem = *cols;
					int c = 0;
					vector_ivec3 ncs;
#if 1
					for (int i = 0; i < cs; i++)
					{
						tem[i].x = c;
						c += n;
						tem[i].y = c;
					}
#else
					_points.push_back({ a.x + b.x * 0.5, a.y + b.y * 0.5 });
					c = n * offset;
					int tc = c;
					bool isrb = c > 0;
					set<unsigned int> ac;
					for (int i = 0; i < cs; i++)
					{
						tem[i].x = tc ? n - tc : c;
						c += n;
						if (c > count)
						{
							tc = c - count;
							c = count;
						}
						else
						{
							tc = 0;
						}
						tem[i].y = c;
						ac.insert(tem[i].z);
						if (tc)
						{
							c = 0;
							auto rt = tem[i];
							if (i + 1 == cs)
							{
								rt.x = 0;
								rt.y = n - tc;
							}
							else
							{
								cs++;
							}
							tem.insert(tem.begin() + i + 1, rt);
						}
					}
					tem.push_back({ _points.size() - 1, _points.size() - 1, 0xf });
					if (isrb)
					{
					}

#endif
				}
			}
			else {
				cols = nullptr;
			}
			PathFillConvex(col, true, true, cols);
		}
		else
		{
			PrimRect(a, b, col);
		}

	}

	void canvas_cx::draw_rect(const glm::ivec4& a, unsigned int col, unsigned int fill, float rounding /*= 0.0f*/, int rounding_corners_flags /*= ~0*/, float thickness /*= 1.0f*/)
	{
		if (a.z < 1)
		{
			col = fill = 0;
		}
		add_rect_a({ a.x, a.y }, { a.x + a.z, a.y + a.w }, col, fill, rounding, rounding_corners_flags, thickness);
	}
	void canvas_cx::draw_rect_filled_multi_color(glm::ivec4 a4, unsigned int col_upr_left, unsigned int col_upr_right,
		unsigned int col_bot_right, unsigned int col_bot_left, bool diagonal, const glm::vec4& r)
	{
		if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) & HZ_COL32_A_MASK) == 0)
			return;
		glm::vec2 a = { a4.x, a4.y }, c = { a4.x + a4.z, a4.y + a4.w };
		const glm::vec2 uv = _uv_white_pixel;
		float radTopLeft = r.x, radTopRight = r.y, radBottomRight = r.z, radBottomLeft = r.w;
		float w = a4.z, h = a4.w;
		if (radTopLeft < 0.1f && radTopRight < 0.1f && radBottomRight < 0.1f && radBottomLeft < 0.1f) {
			auto od = PrimReserve(6, 4);
			DrawIdx vci = od.vsize;

			DrawIdx widx[6] = { vci, vci + 1, vci + 2, vci, vci + 2, vci + 3 };
			if (diagonal) {
				widx[2] = vci + 3;
				widx[3] = vci + 1;
				widx[4] = vci + 2;
				widx[5] = vci + 3;
			}
			cpy_idx(od, widx, 6);
			push_vtx(od, { a, uv, col_upr_left });
			push_vtx(od, { glm::vec2(c.x, a.y), uv, col_upr_right });
			push_vtx(od, { c, uv, col_bot_right });
			push_vtx(od, { glm::vec2(a.x, c.y), uv, col_bot_left });
			return;
		}
		else {
			_path.PathRect(a, c, r);	// 生成圆角矩形顶点
			vector_ivec3 cols;
			int ks = _path.size() / 4;
			uint32_t tc[] = { col_upr_left, col_upr_right, col_bot_right, col_bot_left };
			// 配置颜色
			for (size_t i = 0; i < 4; i++) { cols.push_back({ i * ks,(i + 1) * ks, tc[i] }); }
			auto nps = _path._points;
			_path.clear();
			DrawIdx idx = _wptr.vsize + _wptr.vc;
			t_vector<DrawIdx> path2;
			int ps = nps.size() / 2;
			get_path2_index(ps, path2, idx, idx + ps, false);	// 计算索引
			size_t vtx_count = nps.size();
			auto od = PrimReserve(path2.size(), vtx_count);		// 预创建顶点、索引
			cpy_idx(od, path2.data(), path2.size());			// 添加索引到绘图缓冲区
			int ps0 = ps - 1;
			int psc = ps * 0.5;
			for (int i = 0; i < psc; i++) {
				std::swap(nps[i], nps[ps0 - i]);				// 因为顶点是环形，所以要翻转顶点
			}
			{
				// 顶点设置颜色、复制到绘图缓冲
				glm::ivec3* ct = nullptr;
				int ctlen = 0;
				{
					ct = cols.data();
					ctlen = cols.size();
				}
				int i = 0;
				uint32_t inner_col = 0, col = ct->z;
				for (auto it : nps)
				{
					if (ct)
					{
						if (ct->y == i)
						{
							ct++; inner_col = col;// outer_col = col_trans;
							if (0 > --ctlen)ct = 0;
						}
						if (ct && ct->x == i)
						{
							inner_col = ct->z;// outer_col = ct->z & 0x00ffffff;
						}
					}
					push_vtx(od, { it, uv, inner_col });			// 添加顶点到绘图缓冲区
					i++;
				}
			}

			return;
			float x = a4.x, y = a4.y;
			float halfw = abs(w) * 0.5f;
			float halfh = abs(h) * 0.5f;
			float rxBL = min(radBottomLeft, halfw) * signf(w), ryBL = min(radBottomLeft, halfh) * signf(h);
			float rxBR = min(radBottomRight, halfw) * signf(w), ryBR = min(radBottomRight, halfh) * signf(h);
			float rxTR = min(radTopRight, halfw) * signf(w), ryTR = min(radTopRight, halfh) * signf(h);
			float rxTL = min(radTopLeft, halfw) * signf(w), ryTL = min(radTopLeft, halfh) * signf(h);
			float vals[] = {
				((float)can_path::commands::P_MOVETO), x, y + ryTL,
				((float)can_path::commands::P_LINETO), x, y + h - ryBL,
				((float)can_path::commands::P_BEZIERTO), x, y + h - ryBL * (1 - KAPPA90), x + rxBL * (1 - KAPPA90), y + h, x + rxBL, y + h,
				((float)can_path::commands::P_LINETO), x + w - rxBR, y + h,
				((float)can_path::commands::P_BEZIERTO), x + w - rxBR * (1 - KAPPA90), y + h, x + w, y + h - ryBR * (1 - KAPPA90), x + w, y + h - ryBR,
				((float)can_path::commands::P_LINETO), x + w, y + ryTR,
				((float)can_path::commands::P_BEZIERTO), x + w, y + ryTR * (1 - KAPPA90), x + w - rxTR * (1 - KAPPA90), y, x + w - rxTR, y,
				((float)can_path::commands::P_LINETO), x + rxTL, y,
				((float)can_path::commands::P_BEZIERTO), x + rxTL * (1 - KAPPA90), y, x, y + ryTL * (1 - KAPPA90), x, y + ryTL,
				((float)can_path::commands::P_CLOSE)
			};
			can_path cp;
			cp.append_commands(vals, COF(vals));

		}



	}
	static float sdroundrect(glm::vec2 pt, glm::vec2 ext, float rad) {
		glm::vec2 ext2 = ext - glm::vec2(rad, rad);
		glm::vec2 d = abs(pt) - ext2;
		return std::min(std::max(d.x, d.y), 0.0f) + length(std::max(d, { 0, 0 })) - rad;
	}

	// Scissoring
	static float scissorMask(glm::vec2 p) {
		float scissorMat = 1.0, scissorExt = 0, scissorScale = 1.0;
		glm::vec2 sc = (abs(scissorMat * p) - scissorExt);
		sc = glm::vec2(0.5, 0.5) - sc * scissorScale;
		return glm::clamp(sc.x, 0.0f, 1.0f) * glm::clamp(sc.y, 0.0f, 1.0f);
	}
#ifdef EDGE_AA
	// Stroke - from [0..1] to clipped pyramid, where the slope is 1px.
	static float strokeMask() {
		return min(1.0, (1.0 - abs(ftcoord.x * 2.0 - 1.0)) * strokeMult) * min(1.0, ftcoord.y);
	}
#endif
	static glm::vec4 gradient(glm::vec2 fpos, glm::vec4 innerCol, glm::vec4 outerCol,
		glm::vec2 extent, float radius, float feather)
	{
		glm::vec4 result;
		float scissor = scissorMask(fpos);
#ifdef EDGE_AA
		float strokeAlpha = strokeMask();
		if (strokeAlpha < strokeThr) discard;
#else
		float strokeAlpha = 1.0;
#endif
		float paintMat = 1.0f;
		// Gradient
			// Calculate gradient color using box gradient
		glm::vec2 pt = paintMat * fpos;

		float d = glm::clamp((sdroundrect(pt, extent, radius) + feather * 0.5) / feather, 0.0, 1.0);
		glm::vec4 color = mix(innerCol, outerCol, d);
		// Combine alpha
		color *= strokeAlpha * scissor;
		result = color;
		return result;
	}
	//	0 = rect 或 1 = circle 或 2 = ellipse
	// 填充阴影效果
	// center和inner二选一，inner可选
	// glm::ivec3 center = { 270,280,0x800000ff };
	// glm::ivec3 inner = { 10,10,0x50800000 };
	void canvas_cx::draw_rect_filled_gradient(const glm::vec4& rect, const glm::ivec3& center,
		const glm::ivec3* inner_,/* x=%,y=%,z = col*/
		float rounding, unsigned int col_outer
	)
	{
		/*
		索引
		0                               1
			   6                7
						4
			   5                8
		3                               2

		+-------------------------------+
		|                               |
		|                               |
		+               +               +
		|                               |
		|                               |
		+-------------------------------+
		*/
		const glm::vec2 uv = _uv_white_pixel;
		glm::vec2 origin = {};
		glm::vec2 a = { rect.x, rect.y }, s = { rect.z, rect.w };
		glm::vec2 c = { s.x + a.x, s.y + a.y };
		glm::vec2 o1 = { (0.01f * (center.x + s.x)) + a.x, (0.01f * (center.y + s.y)) + a.y };
		glm::vec2 o = { center.x, center.y };
		if (!inner_)
		{
			int idx_count = 12;
			int vtx_count = 5;

			auto od = PrimReserve(idx_count, vtx_count);
			DrawIdx idx = od.vsize;
			//顶点
			push_vtx(od, { a, uv, col_outer });
			push_vtx(od, { glm::vec2(c.x, a.y), uv, col_outer });
			push_vtx(od, { c, uv, col_outer });
			push_vtx(od, { glm::vec2(a.x, c.y), uv, col_outer });
			push_vtx(od, { o, uv, (uint32_t)center.z });
			t_vector<DrawIdx> idxv;
			get_point_path_index(4, idxv, idx + 0, idx + 4);
			int idxs[][3] = { {0, 1, 4}, {0, 4, 3}, {4, 1, 2}, {4, 2, 3}, {-1, -1, -1} };
			//复制索引到绘图缓冲
			cpy_idx(od, idxv.data(), idxv.size());
		}
		/*
		索引
		0                               1
			   6                7
						4
			   5                8
		3                               2

		+-------------------------------+
		|                               |
		|                               |
		+               +               +
		|                               |
		|                               |
		+-------------------------------+
		*/
		if (inner_)
		{
			if (!col_outer)
			{
				col_outer = 0x00ffffff & inner_->z;
			}
			t_vector<vector_vec2>  path;
			t_vector<unsigned int> ptcs;
			{
				auto it = *inner_;
				float width = it.x, height = it.y;
				float inroun = rounding * 3;
				ptcs.push_back(col_outer);
				_path.PathRect(a, c, inroun);
				path.push_back(_path._points);

				_path._points.clear();

				_path.PathRect({ a.x + width, a.y + width }, { c.x - height, c.y - height }, rounding);
				path.push_back(_path._points);

				ptcs.push_back(it.z);

			}

			{
				DrawIdx idx = _wptr.vsize + _wptr.vc;// _vtx_data.size();
				t_vector<DrawIdx> idxv;
				get_path2_index(_path._points.size(), idxv, idx, idx + _path.size());
				int idx_count = idxv.size();
				int vtx_count = _path.size() * 2;
				size_t vtx_counts = 0;
				for (auto pt : path)
				{
					vtx_counts += pt.size();
				}
				auto od = PrimReserve(idxv.size(), vtx_counts);
				cpy_idx(od, idxv.data(), idxv.size());
				unsigned int* ptc = ptcs.data();
				for (auto pt : path)
				{
					for (auto it : pt)
					{
						push_vtx(od, { it, uv, *ptc });
					}
					ptc++;
				}
			}
			_path._points.clear();
		}
	}

	/*
	径向渐变
	ivec3坐标x,y颜色z
	center={中心点}
	shape=半径，x=y则是圆，否则是椭圆
	rect_scale矩形缩放
	clip剪裁
	*/
	void canvas_cx::draw_radial_gradient(const glm::ivec3& point, const glm::ivec2& center, vector_ivec3* shape,
		int num_segments, glm::vec4* rect_scale, glm::vec4* clip, unsigned int tcol)
	{
		if (!shape || shape->empty())
		{
			return;
		}
		bool is_clip = clip && clip->z * clip->w;
		if (is_clip)
		{
			push_clipRect(*clip);
		}
		const glm::vec2 uv = _uv_white_pixel;
		// 点+圆
		vector_vec2 path; t_vector<unsigned int> ptcs;
		ptcs.push_back(point.z);
		path.push_back(point);
		t_vector<size_t> soffset;
		double np[] = { 0.0, 1.0 / num_segments / 360 };
		glm::vec2 range = { 0, -1 };
		for (auto& r : *shape)
		{
			auto s = _path.path_ellipse_circle(center, r, num_segments, &path, 0.0, range, 0, true, rect_scale);
			soffset.push_back(s);
			ptcs.push_back(r.z);
		}
		DrawIdx idx = _wptr.vsize + _wptr.vc;// _vtx_data.size();
		t_vector<DrawIdx> idxv, path2, testidx;
		t_vector<t_vector<DrawIdx>> a;

		int ps = num_segments + 1;
		int ss = shape->size();
		int shapeidxs = ps * 2 * ss + ps;
		int idx_count = shapeidxs * 3;
		int vtx_count = path.size();
		a.resize(ss);
		auto pidx = get_point_path_index(ps, idxv, idx + 1, idx + 0);
		ss--;
		for (size_t i = 0; i < ss; i++)
		{
			pidx += get_path2_index(ps, path2, idx + ps * i + 1, idx + ps * (i + 1) + 1);
			a[i].swap(path2);
		}
		size_t idxn = idxv.size();
		for (auto& it : a)
		{
			idxn += it.size();
		}
		auto od = PrimReserve(idxn, vtx_count);
		cpy_idx(od, idxv.data(), idxv.size());
		for (auto& it : a)
		{
			cpy_idx(od, it.data(), it.size());
		}
		unsigned int* ptc = ptcs.data();
		for (int i = 0; i < vtx_count; i++)
		{
			auto it = path[i];
			push_vtx(od, { it, uv, *ptc });
			if (i == 0 || !(i % ps))
			{
				ptc++;
			}
		}
		if (tcol)
		{
			for (auto& r : *shape)
			{
				draw_ellipse(center, r.x, r.y, tcol, 0, num_segments);
			}
		}
		if (is_clip)
		{
			pop_clipRect();
		}
	}
	//void canvas_cx::draw_radial_gradient(css::radial_gradient* radial)
	//{
	//	if (radial)
	//		draw_radial_gradient(radial->point, radial->center, radial->shape, radial->num_segments, radial->rect_scale, radial->clip, radial->tcol);
	//}
	void canvas_cx::draw_radial_gradient(radial_gradient_info_t* radial)
	{
		if (radial)
			draw_radial_gradient(radial->point, radial->center, radial->shape, radial->num_segments, radial->rect_scale, radial->clip, radial->tcol);
	}
	/*
	 线性渐变
	 rect x,y width,height
	 颜色数组glm::ivec2{x.color, y.offset=0-100%};
	  1,0 x+ y
	  0,1 x  y+

			水平渐变 y12固定p2 = { a.x, c.y };//1,0
			--------
			--------
			垂直渐变 x12固定p2 = { c.x, a.y };//0,1
			|	|
			|	|
			|	|

	*/
	void canvas_cx::draw_linear_gradient(glm::ivec4 rect, vector_ivec2* pstops, glm::vec2 dire /*= { 1, 0 }*/)
	{
		const glm::vec2 uv = _uv_white_pixel;
		glm::vec2 s = { rect.z, rect.w };
		glm::vec2 a = { rect.x, rect.y }, c = a + s;
		auto& stops = *pstops;
		t_vector<unsigned int> cols;
		t_vector<vector_vec2> path;
		path.resize(2);
		// 补 0%位置
		if (stops[0].y > 0)
			stops.insert(stops.begin(), glm::ivec2(stops[0].x, 0));
		size_t count = stops.size();
		// 补 100%位置
		if (stops[count - 1].y < 100)
		{
			stops.push_back(glm::ivec2(stops[count - 1].x, 100));
			count++;
		}
		// 水平dire={1,0}，垂直dire={0,1}
		glm::vec2 p1 = a, p2, xt = { a.x, c.x }, yt = { c.y, a.y };
		glm::vec2 t1 = xt * dire, t2 = yt * dire;
		p2 = { t1.x + t1.y, t2.x + t2.y };
		for (int i = 0; i < count; i++)
		{
			float px = (float)stops[i].y / 100.0;
			glm::vec2 p = glm::vec2(px * s.x, px * s.y) * dire;
			path[0].push_back(p1 + p);
			path[1].push_back(p2 + p);
			cols.push_back(stops[i].x);
		}
		DrawIdx idx = _wptr.vsize + _wptr.vc;// _vtx_data.size();
		t_vector<DrawIdx> path2;
		int ps = path[0].size();
		get_path2_index(ps, path2, idx, idx + ps, false);
		size_t vtx_count = 0;
		for (auto& pt : path)
		{
			vtx_count += pt.size();
		}
		auto od = PrimReserve(path2.size(), vtx_count);
		cpy_idx(od, path2.data(), path2.size());

		for (auto& pt : path)
		{
			int i = 0;
			for (auto it : pt)
				push_vtx(od, { it, uv, cols[i++] });
		}
		return;
	}

	void canvas_cx::draw_linear_gradient(linear_gradient_info_t* p)
	{
		if (p)
			draw_linear_gradient(p->rect, p->pstops, p->dire);
	}

	void canvas_cx::draw_poly_gradient(glm::ivec3 center, double radius, unsigned int c, int n /*= 2*/, unsigned int bc /*= 0*/, int nums /*= 360*/)
	{
		vector_ivec3 shape;
		{
			double r = radius;
			if (n == 2)
			{
				shape.push_back({ r * 0.5, r * 0.5, 0 });
			}
			//shape.push_back({ r * 0.75 , r * 0.75, set_alpha_f(c, 0.25), nums });
			shape.push_back({ r, r, c });
			shape.push_back({ r + 1.0, r + 1.0, set_alpha_f(c, 0) });
		}
		draw_radial_gradient(center, center, &shape, nums, nullptr, nullptr, bc);
	}

	void canvas_cx::draw_poly_gradient(css::poly_gradient* p)
	{
		if (p)draw_poly_gradient(p->center, p->radius, p->c, p->n, p->bc, p->nums);
	}


	// -----------------------------------------------------------------------------------------
	//angle旋转角度(0-360)
	static glm::vec2 getRotate(const glm::vec2& pos, const glm::vec2& center, double angle)
	{
		double x = pos.x, y = pos.y;				//原始点坐标
		double rx = center.x, ry = center.y;        //旋转中心点坐标
		double nx, ny;								//旋转后的点坐标
		double as, ac;
		as = sin(angle * M_PI / 180.0);
		ac = cos(angle * M_PI / 180.0);
		nx = rx + ((x - rx) * ac - (y - ry) * as);
		ny = ry + ((x - rx) * as + (y - ry) * ac);
		return { nx, ny };
	}
	void canvas_cx::draw_quad(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, unsigned int col, unsigned int fill, float thickness)
	{
		_path.line_to(a);
		_path.line_to(b);
		_path.line_to(c);
		_path.line_to(d);
		if ((fill & HZ_COL32_A_MASK))
			PathFillConvex(fill, false);
		if ((col & HZ_COL32_A_MASK))
			PathStroke(col, true, thickness);
		_path._points.clear();
	}
	// todo triangle
	void canvas_cx::draw_triangle(glm::vec2 a, glm::vec2 b, glm::vec2 c, unsigned int col, unsigned int fill, float thickness, bool aa)
	{
		if ((fill & HZ_COL32_A_MASK))
		{
			pad5(a, 0);
			pad5(b, 0);
			pad5(c, 0);
			_path.line_to(a);
			_path.line_to(b);
			_path.line_to(c);
			PathFillConvex(fill, true, aa);
		}
		if ((col & HZ_COL32_A_MASK))
		{
			pad5(a);
			pad5(b);
			pad5(c);
			_path.line_to(a);
			_path.line_to(b);
			_path.line_to(c);
			PathStroke(col, true, thickness, aa);
		}
		_path._points.clear();
	}
	void canvas_cx::draw_triangle(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, unsigned int col, const glm::ivec3& fill, float thickness, bool aa)
	{
		_path.line_to(a);
		_path.line_to(b);
		_path.line_to(c);
		if ((fill.x & HZ_COL32_A_MASK))
		{
			vector_ivec3 vc = { {0,1,fill.x},{1,2,fill.y},{2,3,fill.z} };
			PathFillConvex(0, false, aa, &vc);
		}
		if ((col & HZ_COL32_A_MASK))
			PathStroke(col, true, thickness, aa);
		_path._points.clear();
	}
	void canvas_cx::draw_triangle(const glm::vec2& pos, triangle_cs* tp)
	{
		glm::vec2 tpos[3] = {};
		float df = (tp->spos / 100.0);
		switch (tp->dir)
		{
		case 0:
		{
			tpos[0] = { tp->size.x * df, 0 };
			tpos[1] = { tp->size.x, tp->size.y };
			tpos[2] = { 0, tp->size.y };
		}
		break;
		case 1:
		{
			tpos[0] = { tp->size.x, tp->size.y * df };
			tpos[1] = { 0, tp->size.y };
			tpos[2] = { 0, 0 };
		}
		break;
		case 2:
		{
			tpos[0] = { tp->size.x * df, tp->size.y };
			tpos[1] = { 0, 0 };
			tpos[2] = { tp->size.x, 0 };
		}
		break;
		case 3:
		{
			tpos[0] = { 0, tp->size.y * df };
			tpos[1] = { tp->size.x, 0 };
			tpos[2] = { tp->size.x, tp->size.y };
		}
		break;
		default:
			break;
		}
		style_cs s = {};
		if (tp->t)
		{
			s = *tp->t;
		}
		if (s.fill)
			draw_triangle(pos + tpos[0], pos + tpos[1], pos + tpos[2], s.color, s.fill, s.thickness, s.anti_aliased);
		else
			draw_triangle(pos + tpos[0], pos + tpos[1], pos + tpos[2], s.color, s.cols, s.thickness, s.anti_aliased);

	}
#if 0
	void canvas_cx::draw_triangle(const glm::vec2& pos, triangle_o* tp, unsigned int col, unsigned int fill, float thickness, bool aa)
	{
		while (tp)
		{
			auto dp = pos + tp->pos;
			glm::vec2 tpos[3] = {};
			float df = (tp->spos / 100.0);
			switch (tp->dir)
			{
			case 0:
			{
				tpos[0] = { tp->size.x * df, 0 };
				tpos[1] = { tp->size.x, tp->size.y };
				tpos[2] = { 0, tp->size.y };
			}
			break;
			case 1:
			{
				tpos[0] = { tp->size.x, tp->size.y * df };
				tpos[1] = { 0, tp->size.y };
				tpos[2] = { 0, 0 };
			}
			break;
			case 2:
			{
				tpos[0] = { tp->size.x * df, tp->size.y };
				tpos[1] = { 0, 0 };
				tpos[2] = { tp->size.x, 0 };
			}
			break;
			case 3:
			{
				tpos[0] = { 0, tp->size.y * df };
				tpos[1] = { tp->size.x, 0 };
				tpos[2] = { tp->size.x, tp->size.y };
			}
			break;
			default:
				break;
			}
			draw_triangle(dp + tpos[0], dp + tpos[1], dp + tpos[2], col, fill, thickness, aa);
			tp = (triangle_o*)tp->next;
		}
	}
	void canvas_cx::draw_triangle(const glm::vec2& pos, triangle_o* tp, unsigned int col, const glm::ivec3& fill, float thickness, bool aa)
	{

		while (tp)
		{
			auto dp = pos + tp->pos;
			glm::vec2 tpos[3] = {};
			float df = (tp->spos / 100.0);
			switch (tp->dir)
			{
			case 0:
			{
				tpos[0] = { tp->size.x * df, 0 };
				tpos[1] = { tp->size.x, tp->size.y };
				tpos[2] = { 0, tp->size.y };
			}
			break;
			case 1:
			{
				tpos[0] = { tp->size.x, tp->size.y * df };
				tpos[1] = { 0, tp->size.y };
				tpos[2] = { 0, 0 };
			}
			break;
			case 2:
			{
				tpos[0] = { tp->size.x * df, tp->size.y };
				tpos[1] = { 0, 0 };
				tpos[2] = { tp->size.x, 0 };
			}
			break;
			case 3:
			{
				tpos[0] = { 0, tp->size.y * df };
				tpos[1] = { tp->size.x, 0 };
				tpos[2] = { tp->size.x, tp->size.y };
			}
			break;
			default:
				break;
			}
			draw_triangle(dp + tpos[0], dp + tpos[1], dp + tpos[2], col, fill, thickness, aa);
			tp = (triangle_o*)tp->next;
		}
	}
#endif

	// Render a triangle to denote expanded/collapsed state
	void canvas_cx::addDirTriangle(const glm::vec2 s, glm::vec2 pos, int dir0, unsigned int fill, unsigned int color/* = 0*/)
	{
		float r = 1.0, l = 0.0;
		glm::vec2 a, b, c;
		auto dir = (GuiDir)dir0;
		switch (dir)
		{
		case GuiDir_Up:
		case GuiDir_Down:
			if (dir == GuiDir_Up) {
				r = 0;
				l = 1;
				c = glm::vec2(s.x * 0.5, s.y * r);
				b = glm::vec2(0.0, s.y * l);
				a = glm::vec2(s.x, s.y * l);
			}
			else
			{
				a = glm::vec2(s.x * 0.5, s.y * r);
				b = glm::vec2(0.0, s.y * l);
				c = glm::vec2(s.x, s.y * l);
			}
			break;
		case GuiDir_Left:
		case GuiDir_Right:
			if (dir == GuiDir_Left) {
				r = 0;
				l = 1;
				a = glm::vec2(s.x * r, s.y * 0.5);
				b = glm::vec2(s.x * l, 0.0);
				c = glm::vec2(s.x * l, s.y);
			}
			else
			{
				c = glm::vec2(s.x * r, s.y * 0.5);
				b = glm::vec2(s.x * l, 0.0);
				a = glm::vec2(s.x * l, s.y);
			}
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
					a = glm::vec2(0, 0);
					b = glm::vec2(s.x * r, s.y * l);
					c = glm::vec2(s.x, s.y);
				}
				else
				{
					a = glm::vec2(0, 0);
					b = glm::vec2(s.x * r, s.y * l);
					c = glm::vec2(s.x, s.y);

				}
			}
			else if (dir & GuiDir_Up || dir & GuiDir_Down)
			{
				if (dir & GuiDir_Up) {
					r = 0;
					l = 1;
				}
				else
				{

				}
				a = glm::vec2(s.x * r, s.y * r);
				b = glm::vec2(s.x * r, s.y * l);
				c = glm::vec2(s.x * l, s.y * r);
			}
		}
		draw_triangle(pos + a, pos + b, pos + c, color, fill, color ? 1 : 0, true);
		if (color)
		{
			draw_triangle(pos + a, pos + b, pos + c, color & 0x00ffffff | 0x80000000, 0, color ? 1 : 0, false);
		}
	}


	void canvas_cx::draw_circle(const glm::vec2& centre, float radius, unsigned int col, unsigned int fill, int num_segments, float thickness)
	{
		//const float a_max = _PI * 2.0f * ((float)num_segments - 1.0f) / (float)num_segments;

		if ((fill & HZ_COL32_A_MASK))
		{
			_path.arc_to(centre, radius - thickness * 0.5f, num_segments, false);
			PathFillConvex(fill);
		}
		if ((col & HZ_COL32_A_MASK))
		{
			_path.arc_to(centre, radius - 0.5f, num_segments, false);
			PathStroke(col, true, thickness);
		}
	}
	void canvas_cx::draw_ellipse(const glm::vec2& centre, float rx, float ry, unsigned int col, unsigned int fill, int num_segments, float thickness)
	{
		//const float a_max = _PI * 2.0f * ((float)num_segments - 1.0f) / (float)num_segments;

		if ((fill & HZ_COL32_A_MASK))
		{
			_path.arc_to(centre, { rx - thickness * 0.5f, ry - thickness * 0.5f }, num_segments, false);
			PathFillConvex(fill);
		}
		if ((col & HZ_COL32_A_MASK))
		{
			_path.arc_to(centre, { rx - 0.5f, ry - 0.5f }, num_segments, false);
			PathStroke(col, true, thickness);
		}
	}
	void canvas_cx::draw_arc(const glm::vec2& centre, float rx, float ry, unsigned int col, unsigned int fill, int num_start, int num_end, int num_segments, float thickness)
	{
		//const float a_max = _PI * 2.0f * ((float)num_segments - 1.0f) / (float)num_segments;

		if ((fill & HZ_COL32_A_MASK))
		{
			_path.arc_to(centre, { rx - thickness * 0.5f, ry - thickness * 0.5f }, num_start, num_end, num_segments);
			PathFillConvex(fill);
		}
		if ((col & HZ_COL32_A_MASK))
		{
			_path.arc_to(centre, { rx - 0.5f, ry - 0.5f }, num_start, num_end, num_segments);
			PathStroke(col, true, thickness);
		}
	}
	//void canvas::draw_larme(const glm::vec2& centre, float radius, int n, unsigned int col, unsigned int fill, int num_segments, float thickness = 1.0f)
	//{
	//	const float a_max = _PI * 2.0f * ((float)num_segments - 1.0f) / (float)num_segments;

	//	if ((fill & HZ_COL32_A_MASK))
	//	{
	//		PathLarmeTo(centre, radius - thickness * 0.5f, 0.0f, a_max, num_segments, n);
	//		PathFillConvex(fill);
	//	}
	//	if ((col & HZ_COL32_A_MASK))
	//	{
	//		PathLarmeTo(centre, radius - 0.5f, 0.0f, a_max, num_segments, n);
	//		PathStroke(col, true, thickness);
	//	}
	//}

	static void get_path_idxf(glm::vec4* idx_pos, vector_vec2* path)
	{
		if (idx_pos && path->size())
		{
			double f = std::min(std::max(idx_pos->w, 0.0f), 1.0f);
			auto ps = path->size();
			size_t idx = f * ps;
			if (idx >= ps)idx = ps - 1;
			auto pt = path->at(idx);
			idx_pos->x = pt.x;
			idx_pos->y = pt.y;
		}
	}
	void canvas_cx::draw_bezier_curve(const glm::vec2& pos0, const glm::vec2& cp0, const glm::vec2& cp1, const glm::vec2& pos1, unsigned int col, float thickness, int num_segments
		, glm::vec4* idx_pos, vector_vec2* outpath
	)
	{
		if ((col & HZ_COL32_A_MASK) == 0)
			return;
		_path.line_to(pos0);
		_path.PathBezierCurveTo(cp0, cp1, pos1, num_segments);
		_path.path_merge();
		get_path_idxf(idx_pos, &_path._points);
		if (outpath)
		{
			*outpath = _path._points;
		}
		PathStroke(col, false, thickness);
	}
	void canvas_cx::draw_bezier_curve(bezier_curve_info_t* p)
	{
		assert(!(!p || p->point_ctrl->size() < 2));
		if (!p || p->point_ctrl->size() < 2)return;
		size_t n = p->point_ctrl->size() * 0.5;
		auto pc = p->point_ctrl->data();
		for (size_t i = 0; i < n; i++, pc += 2)
		{
			glm::vec2 pos0 = { pc[0].x, pc[0].y }; glm::vec2 pos1 = { pc[0].z, pc[0].w }; glm::vec2 cp0 = { pc[1].x, pc[1].y };  glm::vec2 cp1 = { pc[1].z, pc[1].w };
			_path.line_to(pos0);
			_path.PathBezierCurveTo(cp0, cp1, pos1, p->num_segments);
			_path.path_merge();
			get_path_idxf(p->idx_pos, &_path._points);
		}
		if (p->outpath)
		{
			*p->outpath = _path._points;
		}
		PathStroke(p->col, false, p->thickness);
	}
	// 三阶贝塞尔曲线 1段线2个控制点,point节点，ctrl控制点，col颜色，thickness线宽，num_segments段数
	void canvas_cx::draw_bezier_curve_multi(const vector_vec2& point, const vector_vec2& ctrl, unsigned int col, float thickness, int num_segments)
	{
		int64_t n = std::min(point.size() - 1, ctrl.size() / 2);
		if ((col & HZ_COL32_A_MASK) == 0 || n < 1)
			return;
		_path.line_to(point[0]);
		auto ct = ctrl.data();
		auto pt = point.data();
		pt++;
		for (size_t i = 0; i < n; i++, pt++, ct += 2)
		{
			_path.PathBezierCurveTo(ct[0], ct[1], *pt, num_segments);
		}
		// mergePath();
		PathStroke(col, false, thickness);
	}

	void canvas_cx::add_bezier_multi(const vector_vec2& path, unsigned int col, float thickness, int num_segments, int cp, unsigned int control_col)
	{
		_path.sol1(path, control_col);

		PathStroke(col, false, 1.0f, true);
		glm::vec2* p = (glm::vec2*)path.data();
		if (cp < 1)
		{
			cp = path.size();
		}
		size_t c = path.size() / cp;

		for (size_t i = 0; i < c; i++, p += cp)
		{
			_path.sol4(p, num_segments, cp, i == 0);
		}
		// mergePath();
		PathStroke(col, false, thickness);
	}
	// todo 多阶贝塞尔曲线未完成
	void canvas_cx::add_bezier_multi(const vector_vec2& point, const vector_vec2& ctrl,
		unsigned int col, float thickness, int num_segments, unsigned int control_col, int ctrl_count)
	{
		int64_t n = std::min(point.size() - 1, ctrl.size() / ctrl_count);
		if ((col & HZ_COL32_A_MASK) == 0 || n < 1)
			return;
		auto ct = ctrl.data();
		auto pt = point.data();
		vector_vec2 temp;
		for (size_t i = 0; i < n; i++)
		{
			temp.push_back(*pt++);
			for (int k = 0; k < ctrl_count; k++)
			{
				temp.push_back(*ct++);
			}
			temp.push_back(*pt);
		}
		add_bezier_multi(temp, col, thickness, num_segments, ctrl_count + 2, 0);
		return;
		vector_vec2 path;
		path.push_back(*pt++);
		for (size_t i = 0; i < n; i++)
		{
			for (int k = 0; k < ctrl_count; k++)
			{
				path.push_back(*ct++);
			}
			path.push_back(*pt++);
			for (size_t s = 0; s <= num_segments; s++)
			{
				_path.sol3((double)i / (double)num_segments, path.data(), path.size(), ctrl_count + 2);
			}
			path.clear();
		}
		// mergePath();
		PathStroke(col, false, thickness);
	}

#if 1
	// todo		文本渲染
	glm::ivec2 canvas_cx::draw_text(css_text* csst, glm::ivec2 pos, const std::string& str, size_t count, size_t first_idx, draw_font_info* out)
	{
		auto ft = csst->ft;
		draw_font_info dfi;
		if (!out)
			out = &dfi;
		glm::ivec2 ret = ft->build_info(csst, str, count, first_idx, pos, out);
		double oly = csst->row_base_y + 1;
		double tns = 0;
		auto font = csst->get_font();
		auto fns = csst->get_fheight();
		double base_line = font->get_base_line(fns);
		double bs = csst->row_base_y - base_line;

		if (csst->text_decoration & css_text::underline)
		{
			//下划线
			tns = csst->get_row_lt(css_text::underline);
			oly += tns;
			//oly = -csst->underline_position + 0.5 * tns;
			auto col = csst->color;
			draw_line({ pos.x, pos.y + oly }, { out->awidth, pos.y + oly }, col, tns, false);
		}
		if (csst->text_decoration & css_text::overline)
		{
			//上划线
			tns = csst->get_row_lt(css_text::overline);
			auto col = csst->color;
			draw_line({ pos.x, pos.y }, { out->awidth, pos.y }, col, tns, false);
		}
		// 显示文本阴影
		for (auto& it : out->blurs)
			draw_image(&it);
		// 替换扩展颜色
		for (auto& it : csst->colors)
		{
			for (size_t i = 0; i < it.y; i++)
			{
				auto idx = i + it.x;
				if (idx < out->vitem.size())
					out->vitem[idx].col = it.z;
			}
		}
		// 显示文本
		for (auto& it : out->vitem)
		{
			draw_image(&it);
		}
		if (csst->text_decoration & css_text::line_through)
		{
			//贯穿线
			tns = csst->line_thickness;
			oly = bs + base_line * 0.68;
			auto col = csst->color;
			draw_line({ pos.x, pos.y + oly }, { out->awidth, pos.y + oly }, col, tns, false);
		}
		return ret;
	}
	glm::ivec2 canvas_cx::draw_text(text_info_t* p, size_t n)
	{
		glm::ivec2 ret;
		for (size_t i = 0; i < n; i++)
		{
			ret = draw_text(p->csst, p->pos, p->str, p->count, p->first_idx, p->out);
		}
		return ret;
	}
	void canvas_cx::draw_text(item_box_info* itbox, glm::ivec2 pos1, unsigned int color)
	{
		if (!itbox)return;
		int rh = itbox->box_size.y;
		unsigned int view_width = itbox->extent ? itbox->extent->_view_width : -1;
		int my = 0, cx = 0, cy = 0;
		int height = itbox->fontheight + itbox->row_span;
		int base_line = itbox->base_line;
		size_t count = 0;
		size_t i = 0;
		ispopimg = false;
		imginc = 0;
		for (auto& it : itbox->vtf)
		{
			auto nt = it.item.size();
			count = 0;
			bool nauto = true;
			// 计算自动换行
			if (cx > 0 && ((unsigned int)(cx + it.div.width) >= view_width))
			{
				cy += height;//box_size.y
				cx = my = 0; nauto = false;
			}
			//glm::ivec4 r = { pos1.x + cx,pos1.y + cy, it.rc.z, itbox->box_size.y };
			//draw_rect(r, 0xff0080ff, 0);
			for (auto& ct : it.item)
			{
				glm::ivec2 dpos = { pos1.x + cx + ct._dwpos.x + ct._offset.x
					, pos1.y + cy + base_line + ct._dwpos.y + ct.adv.y + ct._offset.y };
				glm::ivec2 size = { ct._rect.z, ct._rect.w };
				glm::ivec4 rc = { dpos, size };

				if (size.x > 0 && size.y > 0)
					draw_image(ct._image, rc, ct._rect, glm::ivec4{ -1 }, ct.color ? ct.color : color);
				else
					cx = cx;
				count += ct.chlen;
				cx += ct.adv.x;
				if (ct.ch[0] == '\n')
				{
					if (count > 1 || nauto)
						cy += height;
					cx = my = 0;
				}
			}
		}
		if (!ispopimg && imginc > 0)
			pop_image();
		ispopimg = true;
	}

#endif // 0

#if 0
	// todo 渲染到图像
	typedef struct draw_text_info_t
	{
		css_text* csst;
		glm::ivec2 pos;
		const std::string& str;
		size_t count = -1;
		size_t first_idx = 0;
		draw_font_info* out = nullptr;
		Image* _out_image = nullptr;
	}draw_text_info_t;
	glm::ivec2 draw_text(draw_text_info_t* p)
	{
		auto ft = Fonts::s();
		draw_font_info dfi;
		if (!p->out)
			p->out = &dfi;
		auto pos = p->pos;
		auto csst = p->csst;
		auto out = p->out;
		glm::ivec2 ret = ft->build_info(p->csst, p->str, p->count, p->first_idx, pos, p->out);
		double oly = csst->row_base_y + 1;
		double tns = 0;
		auto font = csst->get_font();
		auto fns = csst->get_fheight();
		double base_line = font->get_base_line(fns);
		double bs = csst->row_base_y - base_line;
		if (csst->text_decoration & css_text::underline)
		{
			//下划线
			tns = p->csst->get_row_lt(css_text::underline);
			oly += tns;
			//oly = -csst->underline_position + 0.5 * tns;
			auto col = p->csst->color;
			draw_line({ pos.x, pos.y + oly }, { out->awidth, pos.y + oly }, col, tns, false);
		}
		if (csst->text_decoration & css_text::overline)
		{
			//上划线
			tns = csst->get_row_lt(css_text::overline);
			auto col = csst->color;
			draw_line({ pos.x, pos.y }, { out->awidth, pos.y }, col, tns, false);
		}
		// 显示文本阴影
		for (auto& it : out->blurs)
			draw_image(&it);
		// 替换扩展颜色
		for (auto& it : csst->colors)
		{
			for (size_t i = 0; i < it.y; i++)
			{
				auto idx = i + it.x;
				if (idx < out->vitem.size())
					out->vitem[idx].col = it.z;
			}
		}
		// 显示文本
		for (auto& it : out->vitem)
		{
			draw_image(&it);
		}
		if (csst->text_decoration & css_text::line_through)
		{
			//贯穿线
			tns = csst->line_thickness;
			oly = bs + base_line * 0.68;
			auto col = csst->color;
			draw_line({ pos.x, pos.y + oly }, { out->awidth, pos.y + oly }, col, tns, false);
		}
		return ret;
	}
#endif // 1

	// todo 把文本输出到Image*
#if 0
	static glm::ivec2 writeText(Image* dst, const std::string& str, const glm::vec2& pos, unsigned int col,
		font_att* fa)
	{
		int x = 0, y = 0;
		std::wstring text = AtoW(str);
		writeText(dst, text, pos, col, fa);
		return { 0, 0 };
#if 0
		t_vector<FontCache::FCInfo> vfnstr;
		Font::BuildText(text, fa, &vfnstr);

		for (auto it : vfnstr)
		{
			x += it.rect.z;
			y = std::max(y, it.rect.w);
		}
		int xd = 0;
		if (dst)
		{
			//dst = Image::createNull(x + wrap_width * 2, y + wrap_width * 2);
			int i = 0;
			for (auto it : vfnstr)
			{
				auto ct = text[i++];
				dst->copyImage(it.image, { pos.x + fa->wrap_width + xd, pos.y + fa->wrap_width },
					{ it.rect }, col);
				xd += it.rect.z;
			}
		}
#endif
		return { x + fa->wrap_width * 2, y + fa->wrap_width * 2 };
	}

	static void writeText(Image* dst, const std::wstring& text, const glm::vec2& pos, unsigned int col,
		font_att* fa,
		const glm::ivec4& scope = { 0, 0, 0, 0 }
	)
	{
#if 0
		if (!dst || dst->width < 1 || dst->height < 1)
		{
			return;
		}
		std::map<wchar_t, FontCache::FCInfo> vfnstr;
		t_vector<glm::vec4> hotarea;
		Font::BuildText(text, fa, &vfnstr);

		int xd = 0, ss = 0;
		njson temns;
		if (ss && vfnstr.size())
		{
			auto vi = vfnstr.begin()->second;
			vi.image->saveImage("d:\\fts.png");
		}
		glm::vec2 ps = { pos.x /*+ wrap_width*/ + xd, pos.y + fa->wrap_width };
		glm::ivec2 minpos = { 0, 0 }, maxsize = { dst->width, dst->height };

		{
			glm::ivec4 a = { ps.x, ps.y, 0, fa->line_height };

			size_t i = 0, scx = scope.x, scy = scope.y;
			bool is_back = scope.x >= 0 && scx != scy;
			//for (auto it : vfnstr)
			auto tls = text.size() - scope.z;
			auto txtcol = col;

			int incs = 0;
			auto ist = vfnstr[text[0]];
			if (!ist.image)
				return;
			size_t last = scope.w;
			if (last > 0)
			{
				tls = std::min(tls, last);
			}
			for (size_t i = scope.z; i < tls; i++)
			{
				//cols
				auto tc = text[i];
				auto it = vfnstr[tc];
				auto oi = i;
				bool icb = is_back && i >= scx && i < scy;

				int fw = fa->get_char_width(tc);
				if (fw < 0)
				{
					fw = it.rect.z;
				}

				if (tc == L'\n')
				{
					auto ac = a;
					ac.x = ps.x + xd;
					ac.y = ps.y;
					ac.z = fw;

					xd = 0;
					ps.y += fa->line_height + fa->wrap_width;
					maxsize.y = pos.y + ps.y;
					continue;
				}
				if (maxsize.x > ps.x + xd /*&& maxsize.y > ps.y*/)
				{
					glm::ivec4 txtpos = { ps.x + xd, ps.y, it.rect.z, it.rect.w };
					dst->copyImage(it.image, txtpos, it.rect, col);
				}
				xd += fw;
			}
		}
#endif
	}
#endif
	void canvas_cx::AddImage_b(Image* user_image, const glm::vec2& a, const glm::vec2& b, const glm::vec2& uv_a, const glm::vec2& uv_b, unsigned int col /*= -1*/)
	{
		const bool is_push_image = _image_stack.empty() || user_image != _image_stack.top().p.img;
		if (is_push_image)
			push_image(user_image);
		auto od = PrimReserve(6, 4);
		PrimRectUV(a, b, uv_a, uv_b, col, od.vsize, od.vtx, od.idx);
		imginc++;
		if (is_push_image && ispopimg)
		{
			pop_image();
		}
		return;
	}
	void canvas_cx::AddImage_a(Image* user_image, const glm::ivec4& a, const glm::ivec2& texsize, const glm::ivec4& rect /*= glm::ivec4{ -1 }*/, unsigned int col/* = -1*/)
	{
		glm::ivec2 pos = { a.x, a.y }, size = { a.z, a.w };
		glm::vec4 v4 = { 0, 0, 1, 1 };
		glm::vec4 uv = v4;
		glm::vec2 s = size;
		if (!(rect.x < 0))
		{
			v4 = rect;
			v4.z += v4.x; v4.w += v4.y;//加上原点坐标
			uv = { v4.x / texsize.x, v4.y / texsize.y, v4.z / texsize.x, v4.w / texsize.y };
		}
		AddImage_b(user_image, pos, { pos.x + s.x, pos.y + s.y }, { uv.x, uv.y }, { uv.z, uv.w }, col);
	}

	/*


	九宫格渲染:
	+--+---------------+--+
	|0 |       1       |2 |
	+--+---------------+--+
	|  |               |  |
	|  |               |  |
	|3 |    center     |4 |
	|  |               |  |
	+--+---------------+--+
	|5 |       6       |7 |
	+--+---------------+--+

	九宫格:索引
	0  12                     14  2
	8  4                      6   10

	9  5                      7   11
	1  13                     15  3
	+--+-------------------------+--+
	|  |                         |  |
	+--+-------------------------+--+
	|  |                         |  |
	|  |                         |  |
	+--+-------------------------+--+
	|  |                         |  |
	+--+-------------------------+--+
	sliced.x=左宽，y上高，z右宽，w下高
	*/
	void canvas_cx::make_sliced(Image* user_image, const glm::ivec4& a, const glm::ivec4& sliced /*= glm::ivec4{ 10.0f,10.0f,10.0f,10.0f }*/
		, const glm::ivec4& rect /*= glm::ivec4{ -1 }*/, unsigned int col /*= -1*/, glm::ivec2 texsize)
	{
		static t_vector<DrawIdx> vt_index =// { 0,8,12,4,14,6,2,10,11,6,7,4,5,8,9,1,5,13,7,15,11,3 };//E_TRIANGLE_STRIP
		{ 0, 8, 12, 8, 12, 4, 12, 4, 14, 4, 14, 6, 14, 6, 2, 6, 2, 10,
			6, 7, 10, 7, 10, 11, 4, 5, 6, 5, 6, 7, 8, 9, 4, 9, 4, 5,
			9, 1, 5, 1, 5, 13, 5, 13, 7, 13, 7, 15, 7, 15, 11, 15, 11, 3 };//E_TRIANGLE_LIST
		if (user_image && (texsize.x < 1 || texsize.y < 1))
			texsize = { user_image->width, user_image->height };
		glm::ivec2 pos = { a.x, a.y }, size = { a.z, a.w };
		glm::vec4 uv = { 0, 0, 1, 1 };
		glm::vec4 v4 = { 0, 0, texsize.x, texsize.y };
		if (!(rect.x < 0))
		{
			v4 = rect;
			v4.z += v4.x; v4.w += v4.y;//加上原点坐标
			uv = { v4.x / texsize.x, v4.y / texsize.y, v4.z / texsize.x, v4.w / texsize.y, };
		}
		float left = sliced.x,
			top = sliced.y,
			right = sliced.z,
			bottom = sliced.w;
		float x = pos.x, y = pos.y, z = 0.0f, width = size.x, height = size.y;
		glm::vec4 suv = { (left + v4.x) / texsize.x, (top + v4.y) / texsize.y,
			(v4.z - right) / texsize.x, (v4.w - bottom) / texsize.y };

		t_vector<vert_t> vertex = {
#if 1
			//0
			{{x, y, z}, {uv.x, uv.y}, col},
			//1
			{{x, y + height, z}, {uv.x, uv.w}, col},
			//2
			{{x + width, y, z}, {uv.z, uv.y}, col},
			//3
			{{x + width, y + height, z}, {uv.z, uv.w}, col},
			//4
			{{x + left, y + top, z}, {suv.x, suv.y}, col},
			//5
			{{x + left, y + height - bottom, z}, {suv.x, suv.w}, col},
			//6
			{{x + width - right, y + top, z}, {suv.z, suv.y}, col},
			//7
			{{x + width - right, y + height - bottom, z}, {suv.z, suv.w}, col},
			//8
			{{x, y + top, z}, {uv.x, suv.y}, col},
			//9
			{{x, y + height - bottom, z}, {uv.x, suv.w}, col},
			//10
			{{x + width, y + top, z}, {uv.z, suv.y}, col},
			//11
			{{x + width, y + height - bottom, z}, {uv.z, suv.w}, col},
			//12
			{{x + left, y, z}, {suv.x, uv.y}, col},
			//13
			{{x + left, y + height, z}, {suv.x, uv.w}, col},
			//14
			{{x + width - right, y, z}, {suv.z, uv.y}, col},
			//15
			{{x + width - right, y + height, z}, {suv.z, uv.w}, col},
#else
			//0
			{{x, y}, {0.0f, 0.f}, col},
			//1
			{{x, y + height}, {0.f, 1.0f}, col},
			//2
			{{x + width, y}, {1.0f, 0.f}, col},
			//3
			{{x + width, y + height}, {1.0f, 1.0f}, col},
			//4
			{{x + left, y + top}, {0.0f + suv.x, 0.f + suv.y}, col},
			//5
			{{x + left, y + height - bottom}, {0.f + suv.x, 1.0f - suv.w}, col},
			//6
			{{x + width - right, y + top}, {1.0f - suv.z, 0.f + suv.z}, col},
			//7
			{{x + width - right, y + height - bottom}, {1.0f - suv.z, 1.0f - suv.w}, col},
			//8
			{{x, y + top}, {0.0f, 0.f + suv.y}, col},
			//9
			{{x, y + height - bottom}, {0.f, 1.0f - suv.w}, col},
			//10
			{{x + width, y + top}, {1.0f, 0.f + suv.y}, col},
			//11
			{{x + width, y + height - bottom}, {1.0f, 1.0f - suv.w}, col},
			//12
			{{x + left, y}, {0.0f + suv.x, 0.f}, col},
			//13
			{{x + left, y + height}, {0.f + suv.x, 1.0f}, col},
			//14
			{{x + width - right, y}, {1.0f - suv.z, 0.f}, col},
			//15
			{{x + width - right, y + height}, {1.0f - suv.z, 1.0f}, col},
#endif
		};

		const bool is_push_image = (_image_stack.empty() || user_image != _image_stack.top().p.img) && user_image;
		if (is_push_image)
			push_image(user_image);
		{
			auto od = PrimReserve(vt_index.size(), vertex.size());
			DrawIdx idx = od.vsize;
			for (auto& it : vt_index)
			{
				push_idx(od, it + idx);
			}
			cpy_vtx(od, vertex.data(), vertex.size());
		}
		if (is_push_image)
			pop_image();
		return;
	}


	// 普通图片显示 {支持显示部分区域、九宫格显示}
	void canvas_cx::draw_image(draw_image_info* info)
	{
		if (!info || !info->user_image)
		{
			return;
		}
		glm::vec2 texsize = { info->user_image->width, info->user_image->height };
		auto rect = info->rect;
		auto a = info->a;
		auto sliced = info->sliced;
		if (rect.z < 1)
		{
			rect.x = rect.y = 0;
			rect.z = texsize.x;
		}
		if (rect.w < 1)
		{
			rect.w = texsize.y;
		}

		if (nohas_clip(a))
			return;
		if (sliced.x < 1 || sliced.y < 1 || sliced.z < 1 || sliced.w < 1)
		{
			AddImage_a(info->user_image, a, texsize, rect, info->col);
		}
		else
		{
			make_sliced(info->user_image, a, sliced, rect, info->col);
		}
	}
	/*
	void canvas_cx::draw_image(image_cs* p, const glm::vec2& pos1, uint32_t color)
	{
		assert(p);
		if (!p || !p->img || (!color || !p->color))
		{
			return;
		}
		auto pos = pos1 + p->pos;
		glm::vec2 texsize = { p->img->width, p->img->height };
		auto& rect = p->rect;
		auto a = glm::vec4(p->offset, p->size);
		a.x += pos.x;
		a.y += pos.y;
		auto& sliced = p->sliced;
		if (rect.z < 1)
		{
			rect.x = rect.y = 0;
			rect.z = texsize.x;
		}
		if (rect.w < 1)
		{
			rect.w = texsize.y;
		}
		auto clip = _clip_rect.top();
		if (nohas_clip({ pos, p->size }))
			return;
		if (sliced.x < 1 || sliced.y < 1 || sliced.z < 1 || sliced.w < 1)
		{
			AddImage_a(p->img, a, texsize, rect, color ? color : p->color);
		}
		else
		{
			make_sliced(p->img, a, sliced, rect, color ? color : p->color);
		}

	}
	*/
	void canvas_cx::draw_image(Image* user_image, glm::vec4 a, glm::vec4 rect, const glm::vec4& sliced, unsigned int col)
	{
		glm::vec2 texsize = { user_image->width, user_image->height };
		if (a.z < 1)
		{
			a.z = texsize.x;
		}
		if (a.w < 1)
		{
			a.w = texsize.y;
		}
		if (rect.z < 1)
		{
			rect.x = rect.y = 0;
			rect.z = texsize.x;
		}
		if (rect.w < 1)
		{
			rect.w = texsize.y;
		}

		if (nohas_clip(a) || (!col))
			return;
		if (sliced.x < 1 || sliced.y < 1 || sliced.z < 1 || sliced.w < 1)
		{
			AddImage_a(user_image, a, texsize, rect, col);
		}
		else
		{
			make_sliced(user_image, a, sliced, rect, col);
		}
	}
	// gif专用
	void canvas_cx::draw_image_gif(Image* user_image, glm::vec2 pos, int idx /*= -1*/)
	{
		glm::vec4 rct = user_image->get_idx_rect(idx);
		draw_image(user_image, { pos, rct.z, rct.w }, rct);
	}
#if 0
	void canvas_cx::draw_shape(shape_base_t type, shape1_cs* s, const glm::vec2& pos1)
	{
		{
			glm::vec2 pos = pos1 + *((glm::vec2*)s);
			auto t = (shape_base)type;
			switch (t)
			{
			case shape_base::rect:
			{
				auto t1 = s->v.r.t;
				draw_rect(pos, s->v.r.size, t1->color, t1->fill, t1->rounding, t1->thickness);
			}
			break;
			case shape_base::ellipse:
			{
				auto& e = s->v.e;
				draw_ellipse(pos, e.r.x, e.r.y, e.t->color, e.t->fill, e.t->num_segments, e.t->thickness);
			}
			break;
			case shape_base::circle:
			{
				auto& c = s->v.c;
				draw_circle(pos, c.r, c.t->color, c.t->fill, c.t->num_segments, c.t->thickness);
			}
			break;
			case shape_base::triangle:
			{
				auto& t = s->v.t;
				draw_triangle(pos, &t);
			}
			break;
			case shape_base::path:
			{
				auto& p = s->v.p;
				auto t = p.t;
				auto& pv = p.path->_points;
				// todo 填充
				p.path->flattenPaths(false);
				for (auto& v : pv)
					draw_convex_poly_filled(pos, v.data(), v.size(), t->fill, t->anti_aliased);
				// 线框
				p.path->flattenPaths(true);
				for (auto& v : pv)
					draw_polyline(pos, v.data(), v.size(), t->color, t->closed, t->thickness, t->anti_aliased);
			}
			break;
			case shape_base::image:
			{
			}
			break;
			case shape_base::text:
			{
			}
			break;
			default:
				break;
			}
		}
	}
#endif
	void canvas_cx::draw_shape(line_cs* p, int n, const glm::vec2& pos)
	{
		for (size_t i = 0; i < n; i++, p++)
		{
			draw_line(p->pos + pos, p->pos2 + pos, p->t->color, p->t->thickness, p->t->anti_aliased);
		}
	}

	size_t get_point_path(int64_t points_count, t_vector<DrawIdx>& out, int offset /*= 1*/, int base_point /*= 0*/)
	{
		size_t ret = points_count * 3;
		out.resize(ret);
		DrawIdx* ptr = out.data();
		/*
		索引
		1                               2

						0

		4                               3

		*/

		for (int i = 0; i < points_count; i++)
		{
			ptr[0] = base_point;
			ptr[1] = (i + offset);
			ptr[2] = (i + offset + 1);
			ptr += 3;
		}
		out[ret - 1] = offset;
		return ret;
	}

	// todo 外边圆角矩形
	void canvas_cx::draw_rect2(rect4d_cs* r, const glm::vec2& pos)
	{
		auto ps1 = pos + r->pos;
		auto rs = r->size;
		auto& st = r->t;
		glm::vec2 pos0 = ps1, pos1 = ps1, pos2 = ps1 + rs;
		glm::vec4 rounding = r->t->rounding;
		glm::ivec4 pc = {};
		for (size_t i = 0; i < 4; i++)
		{
			auto& it = rounding[i];
			if (it < 0)
			{
				pc[i] = 5;
				it = absf(it);
			}
		}
		glm::vec2 bc[7] = { { 0,rs.y }, { rounding[0] * 0.5, rs.y  }, {rounding[0]  , rs.y - rounding[0] * 0.5 }, { rounding[0], rs.y - rounding[0]}
		,{ rounding[0] * 1.5 , rs.y - rounding[0]}
		,{ rounding[0] * 1.5 , rs.y + rounding[0] * 0.5 },{ 0  , rs.y + rounding[0] * 0.5 } };
		bool fb = ((st->fill & HZ_COL32_A_MASK));
		bool cb = ((st->color & HZ_COL32_A_MASK));
		for (size_t i = 0; i < 7; i++)
		{
			bc[i] += pos;
			if (cb)
				pad5(bc[i]);
		}

		auto path = get_cpath();
		_path.line_to(bc[0]);
		_path.PathBezierCurveTo(bc[1], bc[2], bc[3], st->num_segments);
		_path.line_to(bc[4]);
		_path.line_to(bc[5]);
		_path.line_to(bc[6]);
		_path.path_merge();
		//get_path_idxf(idx_pos, &_path._points);

		if ((st->fill & HZ_COL32_A_MASK))
			PathFillConvex(st->fill);
		if ((st->color & HZ_COL32_A_MASK))
			PathStroke(st->color, true, st->thickness);


		//draw_bezier_curve(bc[0], bc[1], bc[2], bc[3], 0xffff8080, 2, 64);
		glm::ivec4 c = { -1,0xff0000aa,0xff0000aa,-1 };
		for (size_t i = 0; i < 4; i++)
		{
			//bc[i].y += 100;
			//draw_circle(bc[i], 6, 0, c[i], 32);
		}
		//draw_bezier_curve(bc[0], bc[1], bc[2], bc[3], 0xffff8080, 2, 64);

	}

	void canvas_cx::draw_shape(rect_cs* r, int n, const glm::vec2& pos)
	{
		for (size_t i = 0; i < n && r; i++, r++)
		{
			auto t1 = r->t;
			//assert(t1);
			if (t1 && r->size.x > 0 && r->size.y > 0)
				draw_rect(pos + r->pos, r->size, t1->color, t1->fill, t1->rounding, t1->thickness);
		}
	}
	void canvas_cx::draw_shape(rect4d_cs* r, int n, const glm::vec2& pos)
	{
		for (size_t i = 0; i < n; i++, r++)
		{
			auto t1 = r->t;
#if 1
			if (t1->position_swap > 0)
			{
				draw_rect2(r, pos);
			}
			else
#endif
			{
				draw_rect(pos + r->pos, r->size, t1->color, t1->fill, t1->rounding, t1->thickness);
			}
		}
	}
	void canvas_cx::draw_shape(ellipse_cs* e, int n, const glm::vec2& pos)
	{
		for (size_t i = 0; i < n; i++, e++)
			draw_ellipse(pos + e->pos, e->r.x, e->r.y, e->t->color, e->t->fill, e->t->num_segments, e->t->thickness);
	}
	void canvas_cx::draw_shape(circle_cs* c, int n, const glm::vec2& pos)
	{
		for (size_t i = 0; i < n; i++, c++)
		{
			auto fc = c->t->fill;
			auto t1 = c->t;
			draw_circle(pos + c->pos, c->r, c->t->color, fc, c->t->num_segments, c->t->thickness);
		}
	}
	// 三角形基于矩形内
	void canvas_cx::draw_shape(triangle_cs* t, int n, const glm::vec2& pos)
	{
		for (size_t i = 0; i < n; i++, t++)
		{
			if (t->t)
				draw_triangle(pos + t->pos, t);
		}
	}
	// xx路径可以填充可线条
	void canvas_cx::draw_shape(path_cs* p1, int n, const glm::vec2& pos0/*, const glm::vec2& pos1 = {}*/)
	{
		for (size_t i = 0; i < n; i++, p1++)
		{
			auto pos = pos0 + p1->pos/*+ (pos1 * p1->mask_scroll)*/;
			auto& p = *p1;
			auto t = p.t;
			auto& pv = p.path1->_points;
			// todo 填充
			if (t->fill)
			{
				p.path1->flattenPaths(false);
				for (auto& v : pv)
					draw_convex_poly_filled(pos, v.data(), v.size(), t->fill, t->anti_aliased);
			}
			// 线框
			if (t->color)
			{
				p.path1->flattenPaths(true);
				for (auto& v : pv)
					draw_polyline(pos, v.data(), v.size(), t->color, t->closed, t->thickness, t->anti_aliased);
			}
		}
	}
	// check打勾

	void drawCheckMark(canvas_cx* canvas, glm::vec2 pos, uint32_t col, float sz1, bool mixed)
	{
		if (!(col & HZ_COL32_A_MASK))
			return;
		float sz = sz1;
		float thickness = std::max(sz / 5.0f, 1.0f);
		sz -= thickness * 0.5f;
		pos += glm::vec2(thickness * 0.25f, thickness * 0.25f);

		float third = sz / 3.0f;
		float bx = pos.x + third;
		float td = sz - third * 0.5f;
		float by = pos.y + td;
		auto path = canvas->get_cpath();
		if (mixed)
		{
			td = thickness * 0.5f;
			auto ps = glm::vec2(pos.x + td, by - third);
			path->line_to(ps);
			ps.x += sz1 - thickness;
			path->line_to(ps);
		}
		else {
			path->line_to(glm::vec2(bx - third, by - third));
			path->line_to(glm::vec2(bx, by));
			path->line_to(glm::vec2(bx + third * 2.0f, by - third * 2.0f));
		}
		//auto k = (bx + third * 2.0f) - (bx - third);
		canvas->PathStroke(col, false, thickness);
	}
	void canvas_cx::draw_shape(check_mark_cs* cmp, int n, const glm::vec2& pos)
	{
		auto p = cmp;
		if (!p)return;
		for (size_t i = 0; i < n; i++)
		{
			auto cc = p->check_col;
			draw_rect(p->pos + pos, { p->square_sz,p->square_sz }, p->col, p->fill, p->rounding, 0);
			const float pad = std::max(1.0f, floor(p->square_sz / 6.0f));
			if (p->new_alpha >= 0)
				cc = set_alpha_f(cc, p->new_alpha);
			drawCheckMark(this, p->pos + pos + glm::vec2(pad, pad), cc, p->square_sz - pad * 2.0f, p->mixed);
			p++;
		}
	}	// 图像基本属性有：区域、偏移、九宫格；64字节
	void canvas_cx::draw_shape(image_cs* p, int n, uint32_t color, const glm::vec2& pos)
	{
		for (size_t i = 0; i < n; i++)
		{
			draw_image(p++, pos, color);
		}
	}
	//void canvas_cx::draw_shape(image_cs1* p, int n, uint32_t color, const glm::vec2& pos)
	//{
	//	for (size_t i = 0; i < n; i++)
	//	{
	//		draw_image(p++, pos, color);
	//	}
	//}
	// 图像min：区域、偏移；48字节
	void canvas_cx::draw_shape(image_m_cs* p, int n, uint32_t color, const glm::vec2& pos)
	{
		for (size_t i = 0; i < n; i++)
		{
			draw_image(p++, pos, color);
		}

	}
	// text渲染本质由图像区域组成
	void canvas_cx::draw_shape(text_cs* text, int n, const glm::vec2& pos1)
	{
		auto pt = text->t;
		for (size_t i = 0; i < n; i++)
		{
			draw_shape(text++, pos1, pt);
		}
	}
	void canvas_cx::draw_shape(text_cs* text, const glm::vec2& pos1, image_m_cs* t)
	{
		if (!t && text)t = text->t;
		if ((!t || !text) || !text->visible)return;
		for (; text; text = text->next)
		{
			auto pos = pos1 + text->pos + text->pos1;
			auto p = ((text->t) ? text->t : t) + text->first;
			for (int i = 0; i < text->count; i++)
			{
				draw_image(p++, pos, text->color);
			}
		}
	}





	bool canvas_cx::nohas_clip(glm::vec4 a)
	{
		auto clip = _clip_rect.top();
		if (clip.z > viewport.z || clip.z < 0)clip.z = viewport.z;
		if (clip.w > viewport.w || clip.w < 0)clip.w = viewport.w;
		if (clip.z < 0 || clip.w < 0)
		{
			return false;
		}
		return (!is_rect_intersect(clip, a));
	}
	glm::vec4 canvas_cx::get_topclip()
	{
		return _clip_rect.top();
	}
	void canvas_cx::draw_image(image_m_cs* p, const glm::vec2& pos1, uint32_t color)
	{
		//assert(p && p->img);
		if (!p || !p->img)
		{
			return;
		}
		auto pos = pos1 + p->pos;
		glm::vec2 texsize = {};
		if (p->img) { texsize = { p->img->width, p->img->height }; }
		auto& rect = p->rect;
		auto a = glm::vec4(p->offset, p->size);
		a.x += pos.x;
		a.y += pos.y;
		if (rect.z < 1)
		{
			rect.x = rect.y = 0;
			rect.z = texsize.x;
		}
		if (rect.w < 1)
		{
			rect.w = texsize.y;
		}
		if (nohas_clip(a))
			return;
		AddImage_a(p->img, a, texsize, rect, color ? color : p->color);
	}

	void canvas_cx::draw_image(image_cs* p, const glm::vec2& pos1, uint32_t color)
	{
		//assert(p && p->img);
		if (!p || (!p->img && !p->tex))
		{
			return;
		}
		auto pos = pos1 + p->pos;

		glm::vec2 texsize = {};
		if (p->img) { texsize = { p->img->width, p->img->height }; }
		if (p->tex) { texsize = { p->tex->width, p->tex->height }; }
		auto& rect = p->rect;
		auto a = glm::vec4(pos, p->size);
		if (rect.z < 1)
		{
			rect.x = rect.y = 0;
			rect.z = texsize.x;
		}
		if (rect.w < 1)
		{
			rect.w = texsize.y;
		}

		if (nohas_clip(a))
			return;

		{
			if (!color)color = p->color;
			glm::ivec2 pos = { a.x, a.y }, size = { a.z, a.w };
			glm::vec4 v4 = { 0, 0, 1, 1 };
			glm::vec4 uv = v4;
			glm::vec2 s = size;
			if (!(rect.x < 0))
			{
				v4 = rect;
				v4.z += v4.x; v4.w += v4.y;//加上原点坐标
				uv = { v4.x / texsize.x, v4.y / texsize.y, v4.z / texsize.x, v4.w / texsize.y };
			}
			auto timg = _image_stack.empty() ? nullptr : _image_stack.top().p.img;
			dvk_texture* pt = (dvk_texture*)timg;
			glm::vec2 a1 = pos, b1 = { pos.x + s.x, pos.y + s.y }, uv_a = { uv.x, uv.y }, uv_b{ uv.z, uv.w };
			bool is_push_image = false;// ;

			if (p->img && timg != p->img)
			{
				push_image(p->img);
				is_push_image = true;
			}
			else if (p->tex && pt != p->tex)
			{
				push_image(p->tex);
				is_push_image = true;
			}
			if (p->sliced[0] < 1 || p->sliced[1] < 1 || p->sliced[2] < 1 || p->sliced[3] < 1)
			{
				auto od = PrimReserve(6, 4);
				PrimRectUV(a1, b1, uv_a, uv_b, color, od.vsize, od.vtx, od.idx);
			}
			else
			{
				make_sliced(nullptr, a, { p->sliced[0] , p->sliced[1], p->sliced[2] , p->sliced[3] }, rect, color, texsize);
			}
			imginc++;

			if (is_push_image && ispopimg)
			{
				pop_image();
			}
		}
	}

	// 可用于拉伸显示
	void canvas_cx::draw_image_quad(Image* user_image, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d
		, const glm::vec2& uv_a, const glm::vec2& uv_b, const glm::vec2& uv_c, const glm::vec2& uv_d, unsigned int col /*= -1*/)
	{
		if ((col & HZ_COL32_A_MASK) == 0)
			return;

		const bool is_push_image = _image_stack.empty() || user_image != _image_stack.top().p.img;
		if (is_push_image)
			push_image(user_image);
		auto od = PrimReserve(6, 4);
		PrimQuadUV(a, b, c, d, uv_a, uv_b, uv_c, uv_d, col, od.vsize, od.vtx, od.idx);

		if (is_push_image)
			pop_image();
		return;
	}

	// 渲染
	void canvas_cx::draw_image_uv(Image* user_image, glm::vec4 rect, glm::vec4 uv_top /*= { 0,0,0,1 }*/, glm::vec4 uv_bottom /*= { 1,1,1,0 }*/, bool isswap /*= true*/)
	{
		if (!user_image)return;
		glm::vec2 rc = { user_image->width, user_image->height };
		if (rect.z > 0)
		{
			rc.x = rect.z;
		}
		if (rect.w > 0)
		{
			rc.y = rect.w;
		}
		if (isswap)
		{
			std::swap(rc.x, rc.y);
		}
		glm::vec2 pos = { rect.x, rect.y };
		glm::vec2 a[4] = { {pos.x, pos.y}, {pos.x + rc.x, pos.y}, {pos.x + rc.x, pos.y + rc.y}, {pos.x, pos.y + rc.y}, }
		, uv[4];// = { {0,0},{0,1},{1,1},{1,0} };
		uv[0] = glm::vec2(uv_top);
		uv[1] = glm::vec2(uv_top.z, uv_top.w);
		uv[2] = glm::vec2(uv_bottom);
		uv[3] = glm::vec2(uv_bottom.z, uv_bottom.w);
		draw_image_quad(user_image, a[0], a[1], a[2], a[3], uv[0], uv[1], uv[2], uv[3]);
	}
	void canvas_cx::draw_image3(Image* user_image, glm::vec4 rect, glm::vec4 uv_top, glm::vec2 uv_bottom)
	{

		if (!user_image)return;
		glm::vec2 rc = { user_image->width, user_image->height };
		if (rect.z > 0)
		{
			rc.x = rect.z;
		}
		if (rect.w > 0)
		{
			rc.y = rect.w;
		}
		glm::vec2 pos = { rect.x, rect.y };
		glm::vec2 a[4], uv[4];


		const bool is_push_image = _image_stack.empty() || user_image != _image_stack.top().p.img;
		if (is_push_image)
			push_image(user_image);
		int ret = 0;
		auto od = PrimReserve(3, 3);
		auto idx = od.isize;
		auto vdx = od.vsize;
		push_idx(od, idx);
		push_idx(od, idx + 1);
		push_idx(od, idx + 2);

		uv[1] = glm::vec2(uv_top.z, uv_top.w);
		uv[2] = glm::vec2(uv_bottom.x, uv_bottom.y);
		a[1] = { pos.x + rc.x, pos.y };
		a[2] = { pos.x, pos.y + rc.y };
		uint32_t col = -1;
		push_vtx(od, { a[0], uv[0], col });
		push_vtx(od, { a[1], uv[1], col });
		push_vtx(od, { a[2], uv[2], col });

		if (is_push_image)
			pop_image();
	}
	void canvas_cx::draw_image_repeat(Image* user_image, glm::vec4 rect, int tn, unsigned int col)
	{
		assert(user_image && (user_image->width * user_image->height) > 0 && rect.z > 0 && rect.w > 0);
		double mx = std::max(rect.z, rect.w);
		if (tn == 1)
		{
			rect.z *= 2; rect.w *= 2;
		}
		double stride = rect.z / user_image->width;
		double stridey = rect.w / user_image->height;
		const bool is_push_image = _image_stack.empty() || user_image != _image_stack.top().p.img;
		if (is_push_image)
			push_image(user_image);
		DrawIdx idx = _wptr.vsize + _wptr.vc;// _vtx_data.size();
		glm::vec4 uv = { 0, 0, stride, stridey };
		rect.z += rect.x; rect.w += rect.y;
		if (tn == 2)
		{
			auto od = PrimReserve(6, 4);
			glm::vec2 uv_a{ uv.x, uv.y }, uv_c{ uv.z, uv.w };
			glm::vec2 uv_b(uv_c.x, uv_a.y), uv_d(uv_a.x, uv_c.y);
			glm::vec2 a(rect.x, rect.y), c(rect.z, rect.w);
			glm::vec2 b(c.x, a.y), d(a.x, c.y);
			DrawIdx id[] = { idx, (idx + 1), (idx + 2), idx, (idx + 2), (idx + 3) };
			vert_t vd[4] = { {a, uv_a, col}, {b, uv_b, col}, {c, uv_c, col}, {d, uv_d, col} };
			cpy_idx(od, id, 6);
			cpy_vtx(od, vd, 4);
		}
		else if (tn == 1)
		{
			auto od = PrimReserve(3, 3);
			auto idx = od.isize;
			auto vdx = od.vsize;
			push_idx(od, idx);
			push_idx(od, idx + 1);
			push_idx(od, idx + 2);

			glm::vec2 uv_a{ uv.x, uv.y }, uv_c{ uv.x, uv.w };
			glm::vec2 uv_b(uv.z, uv.y);
			glm::vec2 a(rect.x, rect.y), b(rect.z, rect.y), c(rect.x, rect.w);
			vert_t vd[3] = { {a, uv_a, col}, {b, uv_b, col}, {c, uv_c, col} };
			cpy_vtx(od, vd, 3);
		}
		if (is_push_image)
			pop_image();
	}
	/*
		struct grid_info_t {
			glm::vec3 pos;			// pos坐标（有motion_t*时推荐设置0）
			glm::vec2 size;			// size边框大小
			glm::ivec2 subdivs;		// subdivs{细分每格宽高}
			glm::ivec3 color;		// {边框颜色，网格颜色，背景颜色}默认黑色
			vector_ivec2* x_width = nullptr, * y_height = nullptr;//单独设置宽高,glm::ivec2{x位置,y宽/高度}
		};
	*/
	void canvas_cx::draw_grid(grid_info_t* git)
	{
		assert(git);
		glm::ivec3 color = git->color;
		glm::vec2 pos = { git->pos.x, git->pos.y };
		//if (!color.x) { color.x = 0xff999999; }
		//if (!color.y) { color.y = 0xffd4d4d4; }
		//if (!color.z) { color.z = -1; }
		glm::ivec4 border_r = { git->pos.x, git->pos.y, git->size.x, git->size.y };
		auto box = border_r;
		box.x -= 10;
		box.y -= 10;
		box.z += 20;
		box.w += 20;
		draw_rect(box, 0, color.z);
		int xx = git->size.x / git->subdivs.x;
		int yy = git->size.y / git->subdivs.y;
		//xx++; yy++;
		if (xx < 1 || yy < 1)return;
		glm::ivec2 maxs = { xx * git->subdivs.x, yy * git->subdivs.y };

		//xx--; yy--;
		t_vector<glm::vec4> gs;
		//push_clipRect({ pos.x, pos.y, git->size.x, git->size.y }, true);
		for (size_t x = 1; x < xx; x++)
		{
			int xp = git->subdivs.x * x;
			gs.push_back({ xp, 0, xp, maxs.y });
			glm::vec2 a = { xp, 0 }, b = { xp, maxs.y };
			draw_line(a + pos, b + pos, color.y, 1.0, false);
		}
		for (size_t y = 1; y < yy; y++)
		{
			int yp = git->subdivs.y * y;
			gs.push_back({ 0, yp, maxs.x, yp });
			glm::vec2 a = { 0, yp }, b = { maxs.x, yp };
			draw_line(a + pos, b + pos, color.y, 1.0, false);
		}
		//pop_clipRect();
		for (auto& it : gs) {
			it.x += pos.x; it.z += pos.x;
			it.y += pos.y; it.w += pos.y;
		}
		draw_rect(border_r, color.x, 0);
		//draw_lines(gs.data(), gs.size(), color.y, 1.0, false);
	}
#endif // 0
	// 获取两条相同顶点数路径组成的三角形索引
	size_t canvas_cx::get_path2_index(int64_t points_count, t_vector<DrawIdx>& out, int offset, int offset1, bool is_close /*= true*/)
	{
		out.resize((points_count * 2) * 3);
		DrawIdx* ptr = out.data();
		/*
		 索引左上角开始
		 两组分别共有8个点{0-7，8-15}
		 输出：16个三角形
		 {
		  {0,1,9}, {0,9,8},
		  {1,2,10}, {1,10,9},
		  {2,3,11}, {2,11,10},
		  ...
		 }
		*/
#if 0

		for (int i = 0; i < points_count; i++)
		{
			{
				ptr[0] = (i + offset);
				ptr[1] = (i + offset + 1);
				ptr[2] = (i + offset + points_count + 1);
				ptr += 3;
			}
			{
				ptr[0] = (i + offset);
				ptr[1] = (i + offset + points_count + 1);
				ptr[2] = (i + offset + points_count);
				ptr += 3;
			}

		}
#else
		for (int i = 0; i < points_count; i++)
		{
			{
				ptr[0] = (i + offset);
				ptr[1] = (i + offset + 1);
				ptr[2] = (i + offset1 + 1);
				ptr += 3;
			}
			{
				ptr[0] = (i + offset);
				ptr[1] = (i + offset1 + 1);
				ptr[2] = (i + offset1);
				ptr += 3;
			}
		}

#endif
		if (is_close)
		{
			// 处理尾部4个顶点
			int dec[] = { -1, -1, 0, -1 };
			int idxe = out.size() - 5;
			for (int i = 0; i < 4; i++)
			{
				out[idxe + i] += points_count * dec[i];
			}
		}
		else {
			out.resize(out.size() - 6);
		}
		return out.size();
		/*
		 共一组8个点
		 输出：6个三角形{ 0 1 2，0 2 3，0 3 4，0 4 5，0 5 6， 0 6 7 }
		*/
		//for (int i = 2; i < points_count; i++)
		//{
		//	ptr[0] = (0);
		//	ptr[1] = (0 + i - 1);
		//	ptr[2] = (0 + i);
		//	ptr += 3;
		//}
	}

	/*
	获取点/路径组成的索引
	{点在0索引，所以只需输入路径顶点数量}
	偏移offset=1
	基点base_point=0

	*/
	size_t canvas_cx::get_point_path_index(int64_t points_count, t_vector<DrawIdx>& out, int offset /*= 1*/, int base_point /*= 0*/)
	{
		size_t ret = points_count * 3;
		out.resize(ret);
		DrawIdx* ptr = out.data();
		/*
		索引
		1                               2

						0

		4                               3

		*/

		for (int i = 0; i < points_count; i++)
		{
			ptr[0] = base_point;
			ptr[1] = (i + offset);
			ptr[2] = (i + offset + 1);
			ptr += 3;
		}
		out[ret - 1] = offset;
		return ret;
	}

	typedef enum {
		SVG_PATH_OP_MOVE_TO,
		SVG_PATH_OP_LINE_TO,
		SVG_PATH_OP_CURVE_TO,
		SVG_PATH_OP_QUAD_CURVE_TO,
		SVG_PATH_OP_ARC_TO,
		SVG_PATH_OP_CLOSE_PATH
	} svg_path_op;
	void canvas_cx::draw_svg_path(svg_path_t* p)
	{
		/*
			<path> 标签用来定义路径
			命令	名称	参数
			M	moveto  移动到	(x y)+
			Z	closepath  关闭路径	(none)
			L	lineto  画线到	(x y)+
			H	horizontal lineto  水平线到	x+
			V	vertical lineto  垂直线到	y+
			C	curveto  三次贝塞尔曲线到	(x1 y1 x2 y2 x y)+
			S	smooth curveto  光滑三次贝塞尔曲线到	(x2 y2 x y)+
			Q	quadratic Bézier curveto  二次贝塞尔曲线到	(x1 y1 x y)+
			T	smooth quadratic Bézier curveto  光滑二次贝塞尔曲线到	(x y)+
			A	elliptical arc  椭圆弧	(rx ry x-axis-rotation large-arc-flag sweep-flag x y)+
			R	Catmull-Rom curveto*  Catmull-Rom曲线	x1 y1 (x y)+

		*/
		auto path = p->path;
		glm::vec2 pos = p->pos;
		glm::vec2 scale = p->scale;
		unsigned int col = p->col;
		unsigned int fillcol = p->fillcol;
		double thickness = p->thickness;
		bool a = true;
		can_path cp;
		if (col == 0)
		{
			pos = {}; scale = { 1, 1 };
		}
		auto& d = *path;
		cp.clear();
		glm::vec2 reflected_cubic_pt, reflected_quad_pt, current_pt;
		svg_path_op op;
		bool isop = false;
		std::function<void(int& i, char ch)> func[128];

		auto set_current_pt = [&current_pt, &op](const glm::vec2& pt, svg_path_op sop)
		{
			op = sop;
			current_pt = pt;
		};
		auto set_reflected_cubic_pt = [&reflected_cubic_pt](const glm::vec2& pt)
		{
			reflected_cubic_pt = pt;
		};
		auto set_reflected_quad_pt = [&reflected_quad_pt](const glm::vec2& pt)
		{
			reflected_quad_pt = pt;
		};
		auto next_cur_pos = [&](int& i, char ch)->glm::vec2
		{
			return (islower(ch)) ? current_pt + d[i++] : d[i++];
		};
		func['m'] = func['M'] = [&](int& i, char ch)
		{
			set_current_pt(next_cur_pos(i, ch), SVG_PATH_OP_MOVE_TO);
			cp.line_to(pos + scale * current_pt /*+ glm::vec2(0.5f, 0.5f)*/);

		};
		func['z'] = func['Z'] = [&](int& i, char ch)
		{
			cp.path_merge();//使用int去重
			if (get_alpha(fillcol))
			{
				draw_convex_poly_filled({ 0, 0 }, cp._points.data(), cp._points.size(), fillcol, a);
			}
			if (get_alpha(col))
			{
				draw_polyline({ 0, 0 }, cp._points.data(), cp._points.size(), col, true, thickness, a);
			}
			cp.clear();
			op = SVG_PATH_OP_CLOSE_PATH;
		};
		func['l'] = func['L'] = [&](int& i, char ch)
		{
			set_current_pt(next_cur_pos(i, ch), SVG_PATH_OP_LINE_TO);
			cp.line_to(pos + scale * current_pt);
		};
		// 水平线\垂直线 在解析时填充另一个属性
		func['h'] = func['H'] = func['v'] = func['V'] = func['L'];

		// bezier_curve
		auto curve_to = [&](glm::vec2 cp0, glm::vec2 cp1, glm::vec2 pos1)
		{
			glm::vec2 pos0 = current_pt;
			auto dif = abs(scale * pos0 - scale * pos1);
			int difi = dif.x + dif.y;
			cp.PathBezierCurveTo(pos + scale * cp0, pos + scale * cp1, pos + scale * pos1, difi);
			set_reflected_cubic_pt(pos1 + pos1 - cp1);
			set_current_pt(pos1, SVG_PATH_OP_CURVE_TO);
		};
		func['c'] = func['C'] = [&](int& i, char ch)
		{
			glm::vec2 cp0 = next_cur_pos(i, ch);
			glm::vec2 cp1 = next_cur_pos(i, ch);
			glm::vec2 pos1 = next_cur_pos(i, ch);
			curve_to(cp0, cp1, pos1);
		};
		func['s'] = func['S'] = [&](int& i, char ch)
		{

			glm::vec2 cp1 = next_cur_pos(i, ch);
			glm::vec2 pos1 = next_cur_pos(i, ch);
			if (isop)
				curve_to(reflected_cubic_pt, cp1, pos1);
			else
				curve_to(current_pt, cp1, pos1);
		};
		auto quadratic_curve_to = [&](glm::vec2 cp1, glm::vec2 pos1)
		{
			curve_to(current_pt, cp1, pos1);
			set_reflected_quad_pt(pos1 + pos1 - cp1);
			set_current_pt(pos1, SVG_PATH_OP_QUAD_CURVE_TO);
		};
		func['q'] = func['Q'] = [&](int& i, char ch)
		{
			glm::vec2 cp1 = next_cur_pos(i, ch);
			glm::vec2 pos1 = next_cur_pos(i, ch);
			quadratic_curve_to(cp1, pos1);
		};
		func['t'] = func['T'] = [&](int& i, char ch)
		{
			glm::vec2 pos1 = next_cur_pos(i, ch);
			if (isop)
				quadratic_curve_to(reflected_quad_pt, pos1);
			else
				quadratic_curve_to(current_pt, pos1);
		};
		func['a'] = func['A'] = [&](int& i, char ch)
		{
			glm::vec2 r = next_cur_pos(i, ch);
			glm::vec2 x_axis_rotation = next_cur_pos(i, ch);
			glm::vec2 flag = next_cur_pos(i, ch);//large_arc_flag、sweep_flag
			glm::vec2 pos = next_cur_pos(i, ch);

			cp.arc_to(pos, r, 360, false);
			set_current_pt(pos, SVG_PATH_OP_ARC_TO);
		};

		int i = 0;
		for (auto it : *(p->cmdstr))
		{
			func[it](i, it);
		}
		return;
	}

#endif
	/*!add end*/


#if 1

#define P_KAPPA90 0.5522847493f	// Length proportional to radius of a cubic bezier handle for 90deg arcs.
#define COUNT_OF(arr) (sizeof(arr) / sizeof(0[arr]))
#define v_move STBTT_vmove
#define v_line	STBTT_vline
#define v_curve	STBTT_vcurve
#define v_cubic	STBTT_vcubic
#define v_winding 8
#define v_close	9
	enum pointFlags
	{
		PT_CORNER = 0x01,
		PT_LEFT = 0x02,
		PT_BEVEL = 0x04,
		PR_INNERBEVEL = 0x08,
	};
	enum winding_e {
		CCW = 1,			// Winding for solid shapes
		CW = 2,				// Winding for holes
	};

	enum solidity_e {
		SOLID = 1,			// CCW
		HOLE = 2,			// CW
	};


	void TransformIdentity(float* t)
	{
		t[0] = 1.0f; t[1] = 0.0f;
		t[2] = 0.0f; t[3] = 1.0f;
		t[4] = 0.0f; t[5] = 0.0f;
	}

	void TransformTranslate(float* t, float tx, float ty)
	{
		t[0] = 1.0f; t[1] = 0.0f;
		t[2] = 0.0f; t[3] = 1.0f;
		t[4] = tx; t[5] = ty;
	}

	void TransformScale(float* t, float sx, float sy)
	{
		t[0] = sx; t[1] = 0.0f;
		t[2] = 0.0f; t[3] = sy;
		t[4] = 0.0f; t[5] = 0.0f;
	}

	void TransformRotate(float* t, float a)
	{
		float cs = cosf(a), sn = sinf(a);
		t[0] = cs; t[1] = sn;
		t[2] = -sn; t[3] = cs;
		t[4] = 0.0f; t[5] = 0.0f;
	}

	void TransformSkewX(float* t, float a)
	{
		t[0] = 1.0f; t[1] = 0.0f;
		t[2] = tanf(a); t[3] = 1.0f;
		t[4] = 0.0f; t[5] = 0.0f;
	}

	void TransformSkewY(float* t, float a)
	{
		t[0] = 1.0f; t[1] = tanf(a);
		t[2] = 0.0f; t[3] = 1.0f;
		t[4] = 0.0f; t[5] = 0.0f;
	}

	void TransformMultiply(float* t, const float* s)
	{
		float t0 = t[0] * s[0] + t[1] * s[2];
		float t2 = t[2] * s[0] + t[3] * s[2];
		float t4 = t[4] * s[0] + t[5] * s[2] + s[4];
		t[1] = t[0] * s[1] + t[1] * s[3];
		t[3] = t[2] * s[1] + t[3] * s[3];
		t[5] = t[4] * s[1] + t[5] * s[3] + s[5];
		t[0] = t0;
		t[2] = t2;
		t[4] = t4;
	}

	void TransformPremultiply(float* t, const float* s)
	{
		float s2[6];
		memcpy(s2, s, sizeof(float) * 6);
		TransformMultiply(s2, t);
		memcpy(t, s2, sizeof(float) * 6);
	}

	int TransformInverse(float* inv, const float* t)
	{
		double invdet, det = (double)t[0] * t[3] - (double)t[2] * t[1];
		if (det > -1e-6 && det < 1e-6) {
			TransformIdentity(inv);
			return 0;
		}
		invdet = 1.0 / det;
		inv[0] = (float)(t[3] * invdet);
		inv[2] = (float)(-t[2] * invdet);
		inv[4] = (float)(((double)t[2] * t[5] - (double)t[3] * t[4]) * invdet);
		inv[1] = (float)(-t[1] * invdet);
		inv[3] = (float)(t[0] * invdet);
		inv[5] = (float)(((double)t[1] * t[4] - (double)t[0] * t[5]) * invdet);
		return 1;
	}

	void vTransformPoint(float* dx, float* dy, const float* t, float sx, float sy)
	{
		*dx = sx * t[0] + sy * t[2] + t[4];
		*dy = sx * t[1] + sy * t[3] + t[5];
	}
	path_stb::path_stb()
	{
		/*TransformIdentity(xform);*/ setDevicePixelRatio(1.0);
		setDevicePixelRatio(0.2);
	}

	path_stb::~path_stb()
	{

	}
	void path_stb::clear()
	{
		v.clear();
		vf.clear();
		// 渲染用
		// img = 0;
		_points.clear();
		windings = {};
		bounds = {};
		scale = { 1,1 };

		ccommands = 0;
		ncommands = 0;
		tessTol = 1.25f;
		distTol = 0;
		fringeWidth = 0;
		devicePxRatio = 0;
		commandx = 0, commandy = 0;
		stroke = true;
		thickness = 1.0;
	}
	bool path_stb::empty()
	{
		return vf.empty();
	}
	void path_stb::setDevicePixelRatio(float ratio)
	{
		this->tessTol = 0.25f / ratio;
		this->distTol = 0.01f / ratio;
		this->fringeWidth = 1.0f / ratio;
		this->devicePxRatio = ratio;
	}
	void path_stb::tesselateBezier(
		float x1, float y1, float x2, float y2,
		float x3, float y3, float x4, float y4,
		int level, int type, float tessTol, t_vector<glm::vec2>* p)
	{
		float x12, y12, x23, y23, x34, y34, x123, y123, x234, y234, x1234, y1234;
		float dx, dy, d2, d3;

		if (level > 10)
			return;

		x12 = (x1 + x2) * 0.5f;
		y12 = (y1 + y2) * 0.5f;
		x23 = (x2 + x3) * 0.5f;
		y23 = (y2 + y3) * 0.5f;
		x34 = (x3 + x4) * 0.5f;
		y34 = (y3 + y4) * 0.5f;
		x123 = (x12 + x23) * 0.5f;
		y123 = (y12 + y23) * 0.5f;

		dx = x4 - x1;
		dy = y4 - y1;
		d2 = absf(((x2 - x4) * dy - (y2 - y4) * dx));
		d3 = absf(((x3 - x4) * dy - (y3 - y4) * dx));

		if ((d2 + d3) * (d2 + d3) < tessTol * (dx * dx + dy * dy)) {
			//addPoint(x4, y4, type);
			p->push_back({ x4,y4 });
			return;
		}

		/*	if (absf(x1+x3-x2-x2) + absf(y1+y3-y2-y2) + absf(x2+x4-x3-x3) + absf(y2+y4-y3-y3) < ctx->tessTol) {
				addPoint(x4, y4, type);
				return;
			}*/

		x234 = (x23 + x34) * 0.5f;
		y234 = (y23 + y34) * 0.5f;
		x1234 = (x123 + x234) * 0.5f;
		y1234 = (y123 + y234) * 0.5f;

		tesselateBezier(x1, y1, x12, y12, x123, y123, x1234, y1234, level + 1, 0, tessTol, p);
		tesselateBezier(x1234, y1234, x234, y234, x34, y34, x4, y4, level + 1, type, tessTol, p);
	}

	static float triarea2(float ax, float ay, float bx, float by, float cx, float cy)
	{
		float abx = bx - ax;
		float aby = by - ay;
		float acx = cx - ax;
		float acy = cy - ay;
		return acx * aby - abx * acy;
	}

	static float polyArea(glm::vec2* pts, int npts)
	{
		int i;
		float area = 0;
		for (i = 2; i < npts; i++) {
			glm::vec2* a = &pts[0];
			glm::vec2* b = &pts[i - 1];
			glm::vec2* c = &pts[i];
			area += triarea2(a->x, a->y, b->x, b->y, c->x, c->y);
		}
		return area * 0.5f;
	}

	static void polyReverse(glm::vec2* pts, int npts)
	{
		glm::vec2 tmp;
		int i = 0, j = npts - 1;
		while (i < j) {
			tmp = pts[i];
			pts[i] = pts[j];
			pts[j] = tmp;
			i++;
			j--;
		}
	}

	template<class T, class Ts>
	void flatten(T ccd, size_t n, Ts& points, float tessTol, float distTol)
	{
		auto* path = points.data();

		for (size_t i = 0; i < n; i++) {
			auto& it = ccd[i];
			switch (it.type)
			{
			case v_move:
				points.push_back({});
				path = &points[points.size() - 1];
				path->push_back({ it.x, it.y });
				break;
			case v_line:
				path->push_back({ it.x, it.y });
				break;
			case v_cubic:
			{
				auto last = path->rbegin();
				if (last != path->rend() && path) {
					path_stb::tesselateBezier(last->x, last->y, it.cx, it.cy, it.cx1, it.cy1, it.x, it.y, 0, tessTol, PT_CORNER, path);
				}
				else {
					assert(0);
				}
			}
			break;
			case v_close:
				//closePath(ctx);
			{
				auto p0 = path->data();
				auto p1 = path->data() + path->size() - 1;
				bool cod = (ptEquals(p0->x, p0->y, p1->x, p1->y, distTol));
				if (cod)
					path->pop_back();
			}
			break;
			case v_winding:
				//pathWinding(ctx, (int)ctx->commands[i + 1]);
				break;
			default:
				break;
			}
		}

	}
	void path_stb::flattenPaths(bool is_stroke)
	{
		_points.clear();
		// Flatten
		if (is_stroke)
			flatten(vf.data(), vf.size(), _points, tessTol, distTol);
		else
			flatten(v.data(), v.size(), _points, tessTol, distTol);
		if (is_stroke)return;
#if 1
		bounds[0] = bounds[1] = 1e6f;
		bounds[2] = bounds[3] = -1e6f;

		// Calculate the direction and length of line segments.
		auto npaths = _points.size();
		float area = 0;
		bool path_closed = 0;
		auto winding = 1;// windings[j];不支持挖空2
		for (int j = 0; j < npaths; j++) {
			auto path = &_points[j];
			//pts = &points[path->first];
			auto pts = path->data();
			// If the first and last points are the same, remove the last, mark as closed path.
			auto p0 = &pts[path->size() - 1];
			auto p1 = &pts[0];
			if (ptEquals(p0->x, p0->y, p1->x, p1->y, distTol)) {
				path->pop_back();
				p0 = &pts[path->size() - 1];
				path_closed = 1;
			}

			// Enforce winding.
			auto count = path->size();
			if (count > 2) {
				//area = polyArea(pts, count);
				if (winding == CCW && area < 0.0f)
					polyReverse(pts, count);
				if (winding == CW && area > 0.0f)
					polyReverse(pts, count);
			}

			for (int i = 0; i < count; i++) {
				// Calculate segment direction and length
				auto p0dx = p1->x - p0->x;
				auto p0dy = p1->y - p0->y;
				auto p0len = normalize(&p0dx, &p0dy);
				// Update bounds
				bounds[0] = minf(bounds[0], p0->x);
				bounds[1] = minf(bounds[1], p0->y);
				bounds[2] = maxf(bounds[2], p0->x);
				bounds[3] = maxf(bounds[3], p0->y);
				// Advance
				p0 = p1++;
			}
		}
#endif
	}

	void path_stb::appendCommands(float* vals, int nvals, int st)
	{
		int i;
		if (ncommands + nvals > ccommands) {
			int c = ncommands + nvals + ccommands / 2;
			//float* pc = (float*)realloc(commands, sizeof(float) * c);
			//if (pc == NULL)
			//{
			//	assert(pc);
			//	return;
			//}
			//vf.resize(c);
			//commands = vf.data();
			//ccommands = c;
		}

		if ((int)vals[0] != v_close && (int)vals[0] != v_winding) {
			commandx = vals[nvals - 2];
			commandy = vals[nvals - 1];
		}

		// transform commands
		i = 0;
		while (i < nvals) {
			int cmd = (int)vals[i];
			stbtt_vertex vx = {};
			vertex_f vxf = {};
			vxf.type = cmd;
			vx.type = cmd;
			switch (cmd) {
			case v_move:
				//vTransformPoint(&vals[i + 1], &vals[i + 2], xform, vals[i + 1], vals[i + 2]);
				vx.x = vals[i + 1]; vx.y = vals[i + 2];
				vxf.x = vals[i + 1]; vxf.y = vals[i + 2];
				if (st & 2)
					vf.push_back(vxf);
				if (st & 1)
					v.push_back(vx);
				i += 3;
				break;
			case v_line:
				//vTransformPoint(&vals[i + 1], &vals[i + 2], xform, vals[i + 1], vals[i + 2]);
				vx.x = vals[i + 1]; vx.y = vals[i + 2];
				vxf.x = vals[i + 1]; vxf.y = vals[i + 2];
				if (st & 2)
					vf.push_back(vxf);
				if (st & 1)
					v.push_back(vx);
				i += 3;
				break;
			case v_cubic:
				//vTransformPoint(&vals[i + 1], &vals[i + 2], xform, vals[i + 1], vals[i + 2]);
				//vTransformPoint(&vals[i + 3], &vals[i + 4], xform, vals[i + 3], vals[i + 4]);
				//vTransformPoint(&vals[i + 5], &vals[i + 6], xform, vals[i + 5], vals[i + 6]);
			{
				vx.cx = vals[i + 1]; vx.cy = vals[i + 2];
				vx.cx1 = vals[i + 3]; vx.cy1 = vals[i + 4];
				vx.x = vals[i + 5]; vx.y = vals[i + 6];
				vertex_f vxf = {};
				vxf.type = cmd;
				vxf.cx = vals[i + 1]; vxf.cy = vals[i + 2];
				vxf.cx1 = vals[i + 3]; vxf.cy1 = vals[i + 4];
				vxf.x = vals[i + 5]; vxf.y = vals[i + 6];
				if (st & 2)
					vf.push_back(vxf);
				if (st & 1)
					v.push_back(vx);
				i += 7;
			}
			break;
			case v_close:
				i++;
				if (st & 2)
					vf.push_back(vxf);
				if (st & 1)
					v.push_back(vx);
				break;
			case v_winding:
				i += 2;
				break;
			default:
				i++;
			}
		}
		commands.resize(commands.size() + nvals);
		memcpy(&commands[ncommands], vals, nvals * sizeof(float));

		ncommands += nvals;

	}


	// Draw
	void path_stb::BeginPath()
	{
		v.clear();
		vf.clear();
	}
	void path_stb::set_stroke(bool is, float t)
	{
		stroke = is; thickness = std::max(.01f, t);
	}
	void path_stb::MoveTo(float x, float y)
	{
		float vals[] = { v_move, x, y };
		appendCommands(vals, COUNT_OF(vals));
	}

	void path_stb::move_to(glm::vec2 pos)
	{
		float vals[] = { v_move,pos.x, pos.y };
		appendCommands(vals, COUNT_OF(vals));
	}

	void path_stb::LineTo(float x, float y)
	{
		float vals[] = { v_line, x, y };
		appendCommands(vals, COUNT_OF(vals));
	}

	void path_stb::line_to(glm::vec2 pos)
	{
		float vals[] = { v_line,pos.x, pos.y };
		appendCommands(vals, COUNT_OF(vals));
	}

	void path_stb::BezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y)
	{
		float vals[] = { v_cubic, c1x, c1y, c2x, c2y, x, y };
		appendCommands(vals, COUNT_OF(vals));
	}

	void path_stb::bezier_to(glm::vec2 c1, glm::vec2 c2, glm::vec2 pos)
	{
		float vals[] = { v_cubic, c1.x, c1.y, c2.x, c2.y,pos.x,pos.y };
		appendCommands(vals, COUNT_OF(vals));
	}

	void path_stb::QuadTo(float cx, float cy, float x, float y)
	{
		float x0 = commandx;
		float y0 = commandy;
		float vals[] = { v_cubic,
			x0 + 2.0f / 3.0f * (cx - x0), y0 + 2.0f / 3.0f * (cy - y0),
			x + 2.0f / 3.0f * (cx - x), y + 2.0f / 3.0f * (cy - y),
			x, y };
		appendCommands(vals, COUNT_OF(vals));
	}

	void path_stb::ArcTo(float x1, float y1, float x2, float y2, float radius)
	{
		float x0 = commandx;
		float y0 = commandy;
		float dx0, dy0, dx1, dy1, a, d, cx, cy, a0, a1;
		int dir;

		if (ncommands == 0) {
			return;
		}

		// Handle degenerate cases.
		if (ptEquals(x0, y0, x1, y1, distTol) ||
			ptEquals(x1, y1, x2, y2, distTol) ||
			distPtSeg(x1, y1, x0, y0, x2, y2) < distTol * distTol ||
			radius < distTol) {
			LineTo(x1, y1);
			return;
		}

		// Calculate tangential circle to lines (x0,y0)-(x1,y1) and (x1,y1)-(x2,y2).
		dx0 = x0 - x1;
		dy0 = y0 - y1;
		dx1 = x2 - x1;
		dy1 = y2 - y1;
		normalize(&dx0, &dy0);
		normalize(&dx1, &dy1);
		a = acosf(dx0 * dx1 + dy0 * dy1);
		d = radius / tanf(a / 2.0f);

		//	printf("a=%f° d=%f\n", a/_PI*180.0f, d);

		if (d > 10000.0f) {
			LineTo(x1, y1);
			return;
		}

		if (cross(dx0, dy0, dx1, dy1) > 0.0f) {
			cx = x1 + dx0 * d + dy0 * radius;
			cy = y1 + dy0 * d + -dx0 * radius;
			a0 = atan2f(dx0, -dy0);
			a1 = atan2f(-dx1, dy1);
			dir = P_CW;
			//		printf("CW c=(%f, %f) a0=%f° a1=%f°\n", cx, cy, a0/_PI*180.0f, a1/_PI*180.0f);
		}
		else {
			cx = x1 + dx0 * d + -dy0 * radius;
			cy = y1 + dy0 * d + dx0 * radius;
			a0 = atan2f(-dx0, dy0);
			a1 = atan2f(dx1, -dy1);
			dir = P_CCW;
			//		printf("CCW c=(%f, %f) a0=%f° a1=%f°\n", cx, cy, a0/_PI*180.0f, a1/_PI*180.0f);
		}

		Arc(cx, cy, radius, a0, a1, dir);
	}

	void path_stb::ClosePath()
	{
		float vals[] = { v_close };
		appendCommands(vals, COUNT_OF(vals));
	}

	void path_stb::PathWinding(int dir)
	{
		float vals[] = { v_winding, (float)dir };
		appendCommands(vals, COUNT_OF(vals));
	}

	void path_stb::Arc(float cx, float cy, float r, float a0, float a1, int dir)
	{
		float a = 0, da = 0, hda = 0, kappa = 0;
		float dx = 0, dy = 0, x = 0, y = 0, tanx = 0, tany = 0;
		float px = 0, py = 0, ptanx = 0, ptany = 0;
		float vals[3 + 5 * 7 + 100];
		int i, ndivs, nvals;
		int move = ncommands > 0 ? v_line : v_move;

		// Clamp angles
		da = a1 - a0;
		if (dir == P_CW) {
			if (absf(da) >= _PI * 2) {
				da = _PI * 2;
			}
			else {
				while (da < 0.0f) da += _PI * 2;
			}
		}
		else {
			if (absf(da) >= _PI * 2) {
				da = -_PI * 2;
			}
			else {
				while (da > 0.0f) da -= _PI * 2;
			}
		}

		// Split arc into max 90 degree segments.
		ndivs = maxi(1, mini((int)(absf(da) / (_PI * 0.5f) + 0.5f), 5));
		hda = (da / (float)ndivs) / 2.0f;
		kappa = absf(4.0f / 3.0f * (1.0f - cosf(hda)) / sinf(hda));

		if (dir == P_CCW)
			kappa = -kappa;

		nvals = 0;
		for (i = 0; i <= ndivs; i++) {
			a = a0 + da * (i / (float)ndivs);
			dx = cosf(a);
			dy = sinf(a);
			x = cx + dx * r;
			y = cy + dy * r;
			tanx = -dy * r * kappa;
			tany = dx * r * kappa;

			if (i == 0) {
				vals[nvals++] = (float)move;
				vals[nvals++] = x;
				vals[nvals++] = y;
			}
			else {
				vals[nvals++] = v_cubic;
				vals[nvals++] = px + ptanx;
				vals[nvals++] = py + ptany;
				vals[nvals++] = x - tanx;
				vals[nvals++] = y - tany;
				vals[nvals++] = x;
				vals[nvals++] = y;
			}
			px = x;
			py = y;
			ptanx = tanx;
			ptany = tany;
		}

		appendCommands(vals, nvals);
	}

	void path_stb::Rect(float x, float y, float w, float h)
	{
		float pof = stroke ? 0.5 : 0.0;
		float filltp = (thickness * pof);

		if (stroke)
		{
			float pof = stroke ? 0.5 : 0.0;
			float filltp = (thickness * pof);
			int n = stroke ? 2 : 1;
			glm::vec2 a[2] = { { x,y },{ x,y } }
				//, b[2] = { { w, h}, { w, h} };
			, b[2] = { {x + w,y + h}, {x + w,y + h} };
			a[0] += filltp, b[0] -= filltp;
			a[1] += 0.5f, b[1] -= 0.5f;
			// 先填充后线
			for (int i = 0; i < n; i++)
			{
				auto sa = a[i];
				auto sb = b[i];
				float vals[] = {
					v_move, sa.x,sa.y,
					v_line, sb.x ,sa.y,
					v_line, sb.x,sb.y ,
					v_line, sa.x,sb.y,
					v_close
				};
				appendCommands(vals, COUNT_OF(vals), n == 1 ? -1 : i + 1);
			}
		}
		else {
			float vals[] = {
				v_move, x,y,
				v_line, x,y + h,
				v_line, x + w,y + h,
				v_line, x + w,y,
				v_close
			};
			appendCommands(vals, COUNT_OF(vals), -1);
		}
	}

	void path_stb::Rect(float x, float y, float w, float h, float r)
	{
		RectVarying(x, y, w, h, r, r, r, r);
	}

	void path_stb::RectVarying(float x, float y, float w, float h, float radTopLeft, float radTopRight, float radBottomRight, float radBottomLeft)
	{
		if (radTopLeft < 0.1f && radTopRight < 0.1f && radBottomRight < 0.1f && radBottomLeft < 0.1f) {
			Rect(x, y, w, h);
			return;
		}
		else {
			float pof = stroke ? 0.5 : 0.0;
			float filltp = (thickness * pof);
			int n = stroke ? 2 : 1;
			glm::vec2 a[2] = { { x,y },{ x,y } }
				//, b[2] = { { w, h}, { w, h} };
			, b[2] = { {x + w,y + h}, {x + w,y + h} };
			a[0] += filltp, b[0] -= filltp;
			a[1] += 0.5f, b[1] -= 0.5f;
			// 先填充后线
			for (int i = 0; i < n; i++)
			{
				x = a[i].x;
				y = a[i].y;
				auto s = b[i];

				float halfw = absf(s.x) * 0.5f;
				float halfh = absf(s.y) * 0.5f;
				float rxBL = minf(radBottomLeft, halfw) * signf(s.x), ryBL = minf(radBottomLeft, halfh) * signf(s.y);
				float rxBR = minf(radBottomRight, halfw) * signf(s.x), ryBR = minf(radBottomRight, halfh) * signf(s.y);
				float rxTR = minf(radTopRight, halfw) * signf(s.x), ryTR = minf(radTopRight, halfh) * signf(s.y);
				float rxTL = minf(radTopLeft, halfw) * signf(s.x), ryTL = minf(radTopLeft, halfh) * signf(s.y);
				float vals[44] = {
					v_move, x, y + ryTL,
					v_line, x, s.y - ryBL,
					v_cubic, x, s.y - ryBL * (1 - P_KAPPA90), x + rxBL * (1 - P_KAPPA90), s.y, x + rxBL, s.y,
					v_line, s.x - rxBR, s.y,
					v_cubic, s.x - rxBR * (1 - P_KAPPA90), s.y, s.x, s.y - ryBR * (1 - P_KAPPA90), s.x, s.y - ryBR,
					v_line, s.x, y + ryTR,
					v_cubic, s.x, y + ryTR * (1 - P_KAPPA90), s.x - rxTR * (1 - P_KAPPA90), y, s.x - rxTR, y,
					v_line, x + rxTL, y,
					v_cubic, x + rxTL * (1 - P_KAPPA90), y, x, y + ryTL * (1 - P_KAPPA90), x, y + ryTL,
					v_close
				};
				appendCommands(vals, COUNT_OF(vals), n == 1 ? -1 : i + 1);
			}
		}
	}

	void path_stb::Ellipse(float cx, float cy, float rx, float ry, int st)
	{
		float vals[] = {
			v_move, cx - rx, cy,
			v_cubic, cx - rx, cy + ry * P_KAPPA90, cx - rx * P_KAPPA90, cy + ry, cx, cy + ry,
			v_cubic, cx + rx * P_KAPPA90, cy + ry, cx + rx, cy + ry * P_KAPPA90, cx + rx, cy,
			v_cubic, cx + rx, cy - ry * P_KAPPA90, cx + rx * P_KAPPA90, cy - ry, cx, cy - ry,
			v_cubic, cx - rx * P_KAPPA90, cy - ry, cx - rx, cy - ry * P_KAPPA90, cx - rx, cy,
			v_close
		};
		appendCommands(vals, COUNT_OF(vals), st);
	}

	void path_stb::Circle(float cx, float cy, float r)
	{
		if (stroke)
		{
			float pof = stroke ? 0.5 : 0.0;
			float filltp = (thickness * pof);
			Ellipse(cx, cy, r - filltp, r, 1);
			Ellipse(cx, cy, r - 0.5f, r, 2);
		}
		else {

			Ellipse(cx, cy, r, r, -1);
		}
	}


#endif // 1

}
// !hz
