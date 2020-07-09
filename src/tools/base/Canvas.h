#ifndef __Canvas__H__
#define __Canvas__H__
#include <unordered_set>
#if 0
/*
纹理0,0位置颜色设置成0xffffffff

*/
draw_polyline(const glm::vec2* points, const int points_count, unsigned int col, bool closed, float thickness, bool anti_aliased)
draw_convex_poly_filled(const glm::vec2* points, const int points_count, unsigned int col, bool anti_aliased)
draw_line(const glm::vec2& a, const glm::vec2& b, unsigned int col, float thickness = 1.0f)
draw_rect(const glm::ivec4& a, unsigned int col, unsigned int fill, float rounding = 0.0f, int rounding_corners_flags = ~0, float thickness = 1.0f)
draw_rect_filled_multi_color(const glm::vec2& a, const glm::vec2& c, unsigned int col_upr_left, unsigned int col_upr_right, unsigned int col_bot_right, unsigned int col_bot_left)
draw_rect_filled_gradient(const glm::vec4& rect, const glm::ivec3& center, const glm::ivec3* inner_, float rounding = 0.0f, unsigned int col_outer = 0)
draw_quad(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, unsigned int col, unsigned int fill, float thickness = 1.0f)
draw_triangle(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, unsigned int col, unsigned int fill, float thickness = 1.0f, bool aa = true)
draw_circle(const glm::vec2& centre, float radius, unsigned int col, unsigned int fill, int num_segments, float thickness = 1.0f)
draw_ellipse(const glm::vec2& centre, float rx, float ry, unsigned int col, unsigned int fill, int num_segments, float thickness = 1.0f)
draw_arc(const glm::vec2& centre, float rx, float ry, unsigned int col, unsigned int fill, int num_start, int num_end, int num_segments, float thickness = 1.0f)
draw_larme(const glm::vec2& centre, float radius, int n, unsigned int col, unsigned int fill, int num_segments, float thickness = 1.0f)
draw_bezier_curve(const glm::vec2& pos0, const glm::vec2& cp0, const glm::vec2& cp1, const glm::vec2& pos1, unsigned int col, float thickness, int num_segments)
draw_bezier_curve_multi(const std::vector<glm::vec2>& point, const std::vector<glm::vec2>& ctrl, unsigned int col, float thickness, int num_segments = 0)
draw_text(const std::wstring& text, const glm::vec2& pos, unsigned int col, float wrap_width, const glm::vec4* cpu_fine_clip_rectconst, const std::wstring& font, float font_size, float font_dpi = 96)
draw_text(const std::wstring& text, const glm::vec2& pos, unsigned int col, const glm::vec4* cpu_fine_clip_rectconst, font_att* fa, const glm::ivec3& scope = { 0, 0, 0 }, std::function<void(std::vector<glm::vec4>& hotarea, glm::vec2 outsize)> ha_func = nullptr, std::vector<glm::ivec3>* cols = nullptr)
draw_text(const std::string& text, const glm::vec2& pos, unsigned int col, const glm::vec4* cpu_fine_clip_rectconst, font_att* fa, const glm::ivec3& scope = { 0, 0, 0 })


GUI_API void  PushClipRect(glm::vec2 clip_rect_min, glm::vec2 clip_rect_max, bool intersect_with_current_clip_rect = false);  // Render-level scissoring. This is passed down to your render function but not used for CPU-side coarse clipping. Prefer using higher-level ImGui::PushClipRect() to affect logic (hit-testing and widget culling)
GUI_API void  PushClipRectFullScreen();
GUI_API void  PopClipRect();
GUI_API void  PushImage(const ImImage& image);
GUI_API void  PopImage();

// Primitives
GUI_API void  draw _line(const glm::vec2& a, const glm::vec2& b, UINT col, float thickness = 1.0f);
GUI_API void  draw _rect(const glm::vec2& a, const glm::vec2& b, UINT col, float rounding = 0.0f, int rounding_corners_flags = ~0, float thickness = 1.0f);   // a: upper-left, b: lower-right, rounding_corners_flags: 4-bits corresponding to which corner to round
GUI_API void  draw _rect_filled(const glm::vec2& a, const glm::vec2& b, UINT col, float rounding = 0.0f, int rounding_corners_flags = ~0);                     // a: upper-left, b: lower-right
GUI_API void  draw _rect_filled_multi_color(const glm::vec2& a, const glm::vec2& b, UINT col_upr_left, UINT col_upr_right, UINT col_bot_right, UINT col_bot_left);
GUI_API void  draw _quad(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, UINT col, float thickness = 1.0f);
GUI_API void  draw _quad_filled(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, UINT col);
GUI_API void  draw _triangle(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, UINT col, float thickness = 1.0f);
GUI_API void  draw _triangle_filled(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, UINT col);
GUI_API void  draw _circle(const glm::vec2& centre, float radius, UINT col, int num_segments = 12, float thickness = 1.0f);
GUI_API void  draw _circle_filled(const glm::vec2& centre, float radius, UINT col, int num_segments = 12);
GUI_API void  draw _text(const glm::vec2& pos, UINT col, const char* text_begin, const char* text_end = NULL);
GUI_API void  draw _text(const ImFont* font, float font_size, const glm::vec2& pos, UINT col, const char* text_begin, const char* text_end = NULL, float wrap_width = 0.0f, const glm::vec4* cpu_fine_clip_rect = NULL);
GUI_API void  draw _image(ImImage user_image, const glm::vec2& a, const glm::vec2& b, const glm::vec2& uv_a = glm::vec2(0, 0), const glm::vec2& uv_b = glm::vec2(1, 1), UINT col = 0xFFFFFFFF);
GUI_API void  draw _image_quad(ImImage user_image, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, const glm::vec2& uv_a = glm::vec2(0, 0), const glm::vec2& uv_b = glm::vec2(1, 0), const glm::vec2& uv_c = glm::vec2(1, 1), const glm::vec2& uv_d = glm::vec2(0, 1), UINT col = 0xFFFFFFFF);
GUI_API void  draw _polyline(const glm::vec2* points, const int num_points, UINT col, bool closed, float thickness, bool anti_aliased);
GUI_API void  draw _convex_poly_filled(const glm::vec2* points, const int num_points, UINT col, bool anti_aliased);
GUI_API void  draw _bezier_curve(const glm::vec2& pos0, const glm::vec2& cp0, const glm::vec2& cp1, const glm::vec2& pos1, UINT col, float thickness, int num_segments = 0);

// Stateful path API, add points then finish with PathFill() or PathStroke()
inline    void  PathClear() { _points.resize(0); }
inline    void  PathLineTo(const glm::vec2& pos) { _points.push_back(pos); }
inline    void  PathLineToMergeDuplicate(const glm::vec2& pos) { if (_points.Size == 0 || memcmp(&_points[_points.Size - 1], &pos, 8) != 0) _points.push_back(pos); }
inline    void  PathFillConvex(UINT col) { draw_convex_poly_filled(_points.Data, _points.Size, col, true); PathClear(); }
inline    void  PathStroke(UINT col, bool closed, float thickness = 1.0f) { draw_polyline(_points.Data, _points.Size, col, closed, thickness, true); PathClear(); }
GUI_API void  PathArcTo(const glm::vec2& centre, float radius, float a_min, float a_max, int num_segments = 10);
GUI_API void  PathArcToFast(const glm::vec2& centre, float radius, int a_min_of_12, int a_max_of_12);                                // Use precomputed angles for a 12 steps circle
GUI_API void  PathBezierCurveTo(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, int num_segments = 0);
GUI_API void  PathRect(const glm::vec2& rect_min, const glm::vec2& rect_max, float rounding = 0.0f, int rounding_corners_flags = ~0);   // rounding_corners_flags: 4-bits corresponding to which corner to round

GUI_API void  ChannelsSplit(int channels_count);
GUI_API void  ChannelsMerge();
GUI_API void  ChannelsSetCurrent(int channel_index);

// Advanced
GUI_API void  AddCallback(ImDrawCallback callback, void* callback_data);  // Your rendering function must check for 'UserCallback' in DrawCmd and call the function instead of rendering triangles.
GUI_API void  AddDrawCmd();                                               // This is useful if you need to forcefully create a new draw call (to allow for dependent rendering / blending). Otherwise primitives are merged into the same draw-call as much as possible

																			// Internal helpers
																			// NB: all primitives needs to be reserved via PrimReserve() beforehand!
GUI_API void  Clear();
GUI_API void  ClearFreeMemory();
GUI_API void  PrimReserve(int idx_count, int vtx_count);
GUI_API void  PrimRect(const glm::vec2& a, const glm::vec2& b, UINT col);      // Axis aligned rectangle (composed of two triangles)
GUI_API void  PrimRectUV(const glm::vec2& a, const glm::vec2& b, const glm::vec2& uv_a, const glm::vec2& uv_b, UINT col);
GUI_API void  PrimQuadUV(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, const glm::vec2& uv_a, const glm::vec2& uv_b, const glm::vec2& uv_c, const glm::vec2& uv_d, UINT col);
inline    void  PrimWriteVtx(const glm::vec2& pos, const glm::vec2& uv, UINT col) { _VtxWritePtr->pos = pos; _VtxWritePtr->uv = uv; _VtxWritePtr->col = col; _VtxWritePtr++; _VtxCurrentIdx++; }
inline    void  PrimWriteIdx(ImDrawIdx idx) { *_IdxWritePtr = idx; _IdxWritePtr++; }
inline    void  PrimVtx(const glm::vec2& pos, const glm::vec2& uv, UINT col) { PrimWriteIdx((ImDrawIdx)_VtxCurrentIdx); PrimWriteVtx(pos, uv, col); }


void testQuaternion()
{
	glm::vec3 origin(0, 0, 1);
	glm::vec3 dest(0, 1, 0);
	glm::quat q = glm::rotation(origin, dest);
	glm::vec3 v0(1, 0, 0); glm::vec3 v1 = q * v0;
	q = glm::rotate(q, glm::half_pi<float>(), glm::vec3(1, 0, 0));
	//lerp\slerp
	glm::quat invq = glm::inverse(q);
	glm::quat conq = glm::conjugate(q);
	glm::mat4 rotMat = glm::toMat4(q);
}
#endif
namespace std {
	template<>
	struct hash<glm::ivec2> {
		std::size_t operator()(const glm::ivec2& j) const
		{
#ifdef _WIN32
			return _Hash_array_representation(&j.x, 2);
#else
			return std::_Hash_impl::hash(&j.x, sizeof(glm::ivec2));
#endif // !_WIN32
		}
	};
};

namespace hz
{
#ifndef HZ_COL32_A_MASK
#define ARRAYSIZE(_ARR) ((int)(sizeof(_ARR)/sizeof(*_ARR)))
#define _PI                   3.14159265358979323846f
#define HZ_COL32_A_MASK     0xFF000000
#endif

#ifndef M_PI
#define M_PI  3.1415926535897932384626433832795
#endif
#define ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR)/sizeof(*_ARR)))         // Size of a static C-style array. Don't use on pointers!
#define OFFSETOF(_TYPE,_MEMBER)  ((size_t)&(((_TYPE*)0)->_MEMBER))           // Offset of _MEMBER within _TYPE. Standardized as offsetof() in modern C++.
#define UNUSED(_VAR)             ((void)_VAR)                                // Used to silence "unused variable warnings". Often useful as asserts may be stripped out from final builds.

	/*
	环境光	ambient
	平行光	directional
	点光	源	point
	聚光灯	spot
	区域光	area
	体积光	volume
	*/
#define maxLightCount  32
	/*

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
		};*/



	class LightParams
	{
	public:
		enum
		{
			light_ambient = 1,
			light_directional,
			light_point,
			light_spot,
			light_area,
			light_volume,
		};
	public:
		LightParams()
		{
			_Direction.w = 1.0;
		}

		~LightParams()
		{
		}
		void setEnabled(bool is)
		{
			_Direction.w = is ? 1 : 0;
		}
		void setDiffuse(glm::vec4 Diffuse)
		{
			_Diffuse = Diffuse;
		}
		void setPointLight(glm::vec4 Diffuse, glm::vec4 Specular, glm::vec4 Position, glm::vec4 RangeAttenuation)
		{
			_Diffuse = Diffuse;
			_Specular = Specular;
			_Position = Position;
			_RangeAttenuation = RangeAttenuation;
			_Position.w = light_point;
		}
		void setDirectLight(glm::vec4 Diffuse, glm::vec4 Specular, glm::vec4 Direction)
		{
			_Diffuse = Diffuse;
			_Specular = Specular;
			_Direction.x = Direction.x;
			_Direction.y = Direction.y;
			_Direction.z = Direction.z;
			_Position.w = light_directional;
		}


		void setSpotLight(glm::vec4 Diffuse, glm::vec4 Specular, glm::vec4 Position, glm::vec4 Direction, glm::vec4 RangeAttenuation, glm::vec4 FalloffThetaPhi)
		{
			_Diffuse = Diffuse;
			_Specular = Specular;
			_Position = Position;
			_Direction.x = Direction.x;
			_Direction.y = Direction.y;
			_Direction.z = Direction.z;
			_RangeAttenuation = RangeAttenuation;
			_FalloffThetaPhi = FalloffThetaPhi;
			_Position.w = light_spot;
		}
		void setPos(glm::vec3 Position)
		{
			_Position.x = Position.x;
			_Position.y = Position.y;
			_Position.z = Position.z;
		}
		glm::vec3 getPos()
		{
			return glm::vec3{ _Position.x,_Position.y,_Position.z };
		}
	public:
		//PointLight 点光源		
		glm::vec4 _Diffuse;		//漫反射的颜色r,g,b,a
		glm::vec4 _Specular;	//镜面高光的颜色r,g,b,a
		glm::vec4 _Position;	//光源位置x,y,z,  w光源类型
		//glm::vec4 _RangeAttenuation;//范围,恒定衰减,光强,二次衰减

								//DirectLight 方向光
								//glm::vec4 Diffuse;		//漫反射的颜色r,g,b,a
								//glm::vec4 Specular;	//镜面高光的颜色r,g,b,a
		glm::vec4 _Direction;	//方向x,y,z,高光,w没用到

							//SpotLight	聚光灯
							//glm::vec4 Diffuse;		//漫反射的颜色r,g,b,a
							//glm::vec4 Specular;	//镜面高光的颜色r,g,b,a
							//glm::vec4 Position;	//光源位置x,y,z,w没用到	
							//glm::vec4 Direction;	//方向x,y,z,w没用到	
		glm::vec4 _RangeAttenuation;//范围,恒定衰减,光强,二次衰减
		glm::vec4 _FalloffThetaPhi;	//从内锥到外锥之间的强度衰减,y没用到,内锥的弧度,外锥的弧度
	};
	class Light :public Res
	{
	public:

		std::array<LightParams, maxLightCount> light;
		glm::vec4   g_ViewPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);;		//观察点(相机)
		glm::vec4	 AmbientColA = glm::vec4(0.0, 0.0, 0.0, 0.0);	//环境光的颜色rgb,环境光的强弱程度a,介于0-1之间 
	public:
		static Light* create()
		{
			return new Light();
		}
		Light()
		{
			AmbientColA = glm::vec4(0.0, 0.0, 0.0, 0.0);
		}

		~Light()
		{
		}
		LightParams* getLight(int idx)
		{
			if (idx >= 0 && idx < light.size())
				return &light[idx];
			return nullptr;
		}
		void copy(void* dst, size_t maxlen)
		{
			size_t clen = light.size() * sizeof(LightParams);
			if (clen > maxlen)
			{
				clen = maxlen;
			}
			memcpy(dst, light.data(), clen);
			maxlen -= clen;
			if (maxlen >= sizeof(glm::vec4) * 2)
			{
				char* cdst = (char*)dst;
				memcpy(cdst + clen, &g_ViewPos, sizeof(glm::vec4) * 2);
			}
		}
		void setAmbient(const glm::vec4& col)
		{
			AmbientColA = col;
		}
		void setPos(glm::vec3 Position, size_t idx)
		{
			auto lt = getLight(idx);
			if (lt)
			{
				lt->setPos(Position);
			}
		}
		glm::vec3 getPos(size_t idx)
		{
			glm::vec3 pos;
			auto lt = getLight(idx);
			if (lt)
			{
				pos = lt->getPos();
			}
			return pos;
		}
		void setDiffuse(glm::vec4 Diffuse, size_t idx)
		{
			auto lt = getLight(idx);
			if (lt)
			{
				lt->setDiffuse(Diffuse);
			}
		}
		void setViewPos(const glm::vec3& pos)
		{
			g_ViewPos.x = pos.x;
			g_ViewPos.y = pos.y;
			g_ViewPos.z = pos.z;
		}
	private:

	};

	//-----------------------------------------------------------------------------


	class UniformBuffer :public Res
	{
	public:
		std::vector<char> buf;
		std::atomic_int64_t _upt = 0;		//判断是否需要更新到显存
		LockS _lock;
	public:

		int64_t get_upt()
		{
			return _upt;
		}
		void set_update()
		{
			_upt++;
		}
		char* data()
		{
			return buf.data();
		}
		size_t size()
		{
			return buf.size();
		}
	public:
		UniformBuffer(size_t s = sizeof(float))
		{
			if (s > 0)
				buf.resize(s);
		}
		UniformBuffer(const UniformBuffer& u)
		{
			buf = u.buf;
		}

		~UniformBuffer()
		{
		}
		static UniformBuffer* create(size_t s)
		{
			return new UniformBuffer(s);
		}
		void setData(void* d, size_t len)
		{
			len = std::min(len, buf.size());
			if (len > 0 && buf.size())
			{
				memcpy(buf.data(), d, len);
				set_update();
			}
		}
		void setCamera(hz::Camera* camera)
		{
			if (buf.size() == sizeof(glm::mat4) * 3)
			{
				memcpy(&buf[0], &camera->matrices._world, sizeof(glm::mat4));
				memcpy(&buf[1 * sizeof(glm::mat4)], &camera->matrices._view, sizeof(glm::mat4));

				memcpy(&buf[2 * sizeof(glm::mat4)], &camera->matrices._perspective, sizeof(glm::mat4));
				glm::mat4 tem;
				tem = camera->matrices._perspective * camera->matrices._view * camera->matrices._world;
				memcpy(&buf[2 * sizeof(glm::mat4)], &tem, sizeof(glm::mat4));
				set_update();
			}
			if (buf.size() == sizeof(glm::mat4) * 2)
			{
				memcpy(&buf[0], &camera->matrices._world, sizeof(glm::mat4));
				glm::mat4 tem;
				tem = camera->matrices._perspective * camera->matrices._view * camera->matrices._world;
				memcpy(&buf[1 * sizeof(glm::mat4)], &tem, sizeof(glm::mat4));
				set_update();
			}
			else if (buf.size() == sizeof(glm::mat4))
			{
				glm::mat4 tem;
				tem = camera->matrices._perspective * camera->matrices._view * camera->matrices._world;
				memcpy(&buf[0 * sizeof(glm::mat4)], &tem, sizeof(glm::mat4));
				set_update();
			}
		}
		void setLight(Light* light)
		{
			//std::vector<float> a;
			//a.resize(1024);
			if (light)
			{
				//light->copy(&a[0], buf.size());
				light->copy(&buf[0], buf.size());
				set_update();
				//memcpy(&buf[0], light, sizeof(Light));
			}
		}
		void setMat4(glm::mat4* m)
		{
			if (m)
			{
				memcpy(&buf[0 * sizeof(glm::mat4)], m, sizeof(glm::mat4));
				set_update();
			}
		}
	private:

	};


	//-----------------------------------------------------------------------------
	typedef struct DrawIndexedIndirectCommand {
		uint32_t    indexCount;
		uint32_t    instanceCount;
		uint32_t    firstIndex;
		int32_t     vertexOffset;
		uint32_t    firstInstance;
	} DrawIndexedIndirectCommand;
	// Per-instance data block
	typedef	struct InstanceData {
		glm::vec3 pos;
		glm::vec3 rot;
		float scale;
		uint32_t texIndex;
	}InstanceData;


	typedef struct DrawIndexedCmd
	{
		uint32_t    elemCount = 0;
		uint32_t    idx_offset = 0;
		int32_t     vtx_offset = 0;
		void* image = 0;
		UniformBuffer* vs_ubo = 0;
		UniformBuffer* ps_ubo = 0;
		uint32_t	instanced = 0;
	}DrawIndexedCmd;

	typedef unsigned int DrawIdx;
	typedef struct DrawVert {
		glm::vec3 pos;	// Position data
		glm::vec2 uv;			// texture u,v
		unsigned int col;		// Color
	}DrawVert;
	typedef struct {
		glm::vec3 pos;			// Position data
		glm::vec2 uv;			// texture u,v
		unsigned int col;		// Color
		unsigned short imgidx;	// image index
	}DrawVert2;

	class gutil
	{
	public:
		gutil()
		{
		}

		~gutil()
		{
		}
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
	private:

	};
	//-----------------------------------------------------------------------------------------------
	class PassObj :public Res
	{
	public:
		std::vector<unsigned int> stride;
		std::vector<unsigned int> offset;
		void* _instanceData = nullptr;
		void* _instanceCMD = nullptr;
		uint32_t indirectDrawCount = 0;
		uint32_t _icmd_stride = 0;
		std::vector<void*> _vbosd;
		std::vector<void*> _vbos;
		void* _vbo = nullptr;
		void* _ibo = nullptr;
		uint64_t _vbo_offset = 0;
		uint64_t _ibo_offset = 0;
		unsigned int _ibos = sizeof(DrawIdx);
		void* _ubo = nullptr;
		void* _shader = 0;
	public:
		PassObj()
		{
		}

		~PassObj()
		{
		}

		static PassObj* create()
		{
			return new PassObj();
		}
	private:

	};
	//-----------------------------------------------------------------------------------------------
	/*
	todo	绘图类Canvas
	*/
	class Canvas :public Res
	{
	public:

		class VertIndex
		{
		public:
			//顶点位置
			int index = 0;
			//索引
			size_t vtxnum = 0;
		public:
			VertIndex() {}
			~VertIndex() {}

		private:

		};
	public:
	public:
		enum GuiCorner
		{
			GuiCorner_TopLeft = 1 << 0, // 1
			GuiCorner_TopRight = 1 << 1, // 2
			GuiCorner_BottomRight = 1 << 2, // 4
			GuiCorner_BottomLeft = 1 << 3, // 8
			GuiCorner_All = 0x0F
		};


		typedef struct DrawCmd
		{
			typedef void (*DrawCallback)(const Canvas* parent_list, const DrawCmd* cmd);

			unsigned int    ElemCount = 0;              // Number of indices (multiple of 3) to be rendered as triangles. Vertices are stored in the callee ImDrawList's vtx_buffer[] array, indices in idx_buffer[].
			unsigned int	idx_offset = 0, ins_offset = 0;
			int vtx_offset = 0;
			glm::vec4       ClipRect = { 0,0,-1,-1 };               // Clipping rectangle (x1, y1, x2, y2)
			Image* image = 0;              // User-provided texture ID. Set by user in ImfontAtlas::SetTexID() for fonts or passed to Image*() functions. Ignore if never using images or multiple fonts atlas.
			glm::mat4* mat = 0;
			uint32_t instanced = 0;
			// 默认0就是E_TRIANGLE_LIST, E_POINT_LIST, E_LINE_LIST, E_TRIANGLE_LIST。
			unsigned int _mode = 0;
			DrawCallback  UserCallback = 0;           // If != NULL, call the function instead of rendering the vertices. clip_rect and texture_id will be set normally.
			void* UserCallbackData = 0;       // The draw callback code can access this.

		}DrawCmd;


		// Draw channels are used by the Columns API to "split" the render list into different channels while building, so items of each column can be batched together.
		// You can also use them to simulate drawing layers and submit primitives in a different order than how they will be rendered.
		struct DrawChannel
		{
			std::vector<DrawCmd>     CmdBuffer;
			std::vector<DrawIdx>     IdxBuffer;
		};

		// This is what you have to render
		std::vector<DrawCmd>		CmdBuffer;          // Commands. Typically 1 command = 1 GPU draw call.
		std::vector<DrawIdx>		IdxBuffer;          // Index buffer. Each command consume DrawCmd::ElemCount of those
		std::vector<DrawVert>		VtxBuffer;          // Vertex buffer.
		std::vector<float>			fVtxBuffer;
		std::vector<InstanceData>	instanceData;
		std::vector<DrawIndexedIndirectCommand> indirect_buff;
		int							_stride = 0;
		size_t						vtxCount = 0;
		// 		void*						_pipe = 0;
// 		void*						_pipeinfo = 0;
		// [Internal, used while building lists]
		const char* _OwnerName;         // Pointer to owner window's name for debugging
		unsigned int				_VtxCurrentIdx;     // [Internal] == VtxBuffer.size()
		DrawVert* _VtxWritePtr;       // [Internal] point within VtxBuffer.data() after each add command (to avoid using the std::vector<> operators too much)
		DrawIdx* _IdxWritePtr;       // [Internal] point within IdxBuffer.data() after each add command (to avoid using the std::vector<> operators too much)
		// PrimReserve
		int _idx_count = 0, _vtx_count = 0;

		std::vector<glm::vec4>      _ClipRectStack;     // [Internal]
		std::vector<Image*>			_ImageStack;    // [Internal]纹理
		std::vector<glm::mat4*>		_MatStack;
		std::vector<glm::vec2>      _points;              // [Internal] current path building
		int                     _ChannelsCurrent;   // [Internal] current channel number (0)
		int                     _ChannelsCount;     // [Internal] number of active channels (1+)
		std::vector<DrawChannel> _Channels;          // [Internal] draw channels for columns API (not resized down so _ChannelsCount may be smaller than _Channels.size())
		glm::vec2 FontTexUvWhitePixel = { 0.0,0.0 };
		float CurveTessellationTol = 1.25f;			// Tessellation tolerance. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.

		//需要创建或更新的纹理
		std::set<void*> _make_image;
		std::set<UniformBuffer*> _make_ubo;
		LockS _lock_mi;
		void* _shader = 0;

		bool _isVisible = true;
		//状态
		bool depthTestEnable = true, depthWriteEnable = false;
		//Camera* camera = 0;
		UniformBuffer* vs_ubo = 0;
		UniformBuffer* ps_ubo = 0;
		Image* _image = 0;
		void* _imgtex = 0;
		bool _blend = true;
		//3d渲染命令
		std::vector<DrawIndexedCmd> _cmd3d;
		glm::vec4       _ClipRect = { 0,0,-1,-1 };

		std::atomic_int64_t _upt = 0;

		// temp

		std::unordered_set<glm::ivec2> _sv;
		std::vector<glm::vec2> _temp;
	public:
		Canvas()
		{
			Clear();

			glm::vec2 tex_uv_scale(1.0f / 512, 1.0f / 512), r = { 0.0,0.0 };
			FontTexUvWhitePixel = glm::vec2((r.x + 0.5f) * tex_uv_scale.x, (r.y + 0.5f) * tex_uv_scale.y);

		}

		virtual ~Canvas()
		{
			ClearFreeMemory();

		}
		static Canvas* create()
		{
			return new Canvas();
		}
		void setShader(void* shader)
		{
			_shader = shader;
		}
		void setImage(Image* img)
		{
			_image = img;
		}
		void* getShader() { return _shader; }

		void setUbo(UniformBuffer* u, UniformBuffer* psu)
		{
			vs_ubo = u;
			ps_ubo = psu;
		}

		int64_t get_upt()
		{
			return _upt;
		}
		void set_update()
		{
			_upt++;
		}
	public:
		void make_idx_offset(int& a_idx, int& local_idx)
		{
			//local_idx = 0;
			a_idx = 0;
		}
	public:

		inline void setVec(glm::vec3& d, const glm::vec2& s)
		{
			//d = { s, 0.f };
			d.x = s.x; d.y = s.y; d.z = 0.f;
		}
		inline void setVec(glm::vec3& d, const glm::vec3& s)
		{
			d = s;
			//d.x = s.x; d.y = s.y; d.z = s.z;
		}
		inline void setVec(glm::vec3& d, const glm::vec4& s)
		{
			glm::vec4 d1 = s;
			d.x = s.x; d.y = s.y; d.z = s.z;
		}
	public:
		//设置实例数据
		std::vector<InstanceData>* getInstanceData()
		{
			return &instanceData;
		}
		std::vector<DrawIndexedIndirectCommand>* getIndexedIndirectCMD()
		{
			return &indirect_buff;
		}
		void setCMDinstanced(uint32_t instanced, size_t idx = 0)
		{
			CmdBuffer[idx].instanced = instanced;
		}
		void clearCmd3d()
		{
			_cmd3d.clear();
		}
		void pushCmd3d(DrawIndexedCmd& cmd)
		{
			insertImage((Image*)cmd.image);
			_cmd3d.push_back(cmd);
		}
	public:
		//void setVisible(bool is)
		//{
		//	LOCK(lock_mi);
		//	_isVisible = is;
		//}
		//bool getVisible()
		//{
		//	LOCK(lock_mi);
		//	return _isVisible;
		//}
		//void setUpdate(bool is = true)
		//{
		//	LOCK(lock_mi);
		//	_isupdate = is;
		//}
		//bool isUpdate() {
		//	LOCK(lock_mi);
		//	bool u = _isupdate;
		//	_isupdate = false;
		//	return u;
		//}
		//void updateCMD_Clip(glm::vec4 clip)
		//{
		//	LOCK(lock_mi);
		//	//CmdBuffer[idx].ClipRect = clip;
		//	_ClipRect = clip;
		//}
		size_t getVtxSize()
		{
			return VtxBuffer.empty() ? fVtxBuffer.size() * sizeof(float) : VtxBuffer.size() * sizeof(DrawVert);
		}
		size_t getIdxSize()
		{
			return IdxBuffer.size() * sizeof(DrawIdx);
		}
		size_t getVtxCount()
		{
			size_t ret = 0;
			if (fVtxBuffer.size())
			{
				ret = fVtxBuffer.size() * sizeof(float) / _stride;
			}
			else if (VtxBuffer.size())
			{
				ret = VtxBuffer.size();
			}
			return ret;
		}
		size_t getIdxCount()
		{
			return IdxBuffer.size();
		}


		float  InvLength(const glm::vec2& lhs, float fail_value) { float d = lhs.x * lhs.x + lhs.y * lhs.y; if (d > 0.0f) return 1.0f / sqrtf(d); return fail_value; }
	public:

		//-----------------------------------------------------------------------------
		// ImDrawList
		//-----------------------------------------------------------------------------

		const glm::vec4 GNullClipRect = { .0f, .0f, -1,-1 }; // Large values that are easy to encode in a few bits+shift

		void Clear()
		{
			CmdBuffer.clear();
			IdxBuffer.clear();
			VtxBuffer.clear();
			_VtxCurrentIdx = 0;
			_VtxWritePtr = NULL;
			_IdxWritePtr = NULL;
			_ClipRectStack.clear();
			_ImageStack.clear();
			_points.clear();
			_ChannelsCurrent = 0;
			_ChannelsCount = 1;
			PushImage(0);
		}

		void ClearFreeMemory()
		{
			CmdBuffer.clear();
			IdxBuffer.clear();
			VtxBuffer.clear();
			_VtxCurrentIdx = 0;
			_VtxWritePtr = NULL;
			_IdxWritePtr = NULL;
			_ClipRectStack.clear();
			_ImageStack.clear();
			_points.clear();
			_ChannelsCurrent = 0;
			_ChannelsCount = 1;
			for (int i = 0; i < _Channels.size(); i++)
			{
				if (i == 0) memset(&_Channels[0], 0, sizeof(_Channels[0]));  // channel 0 is a copy of CmdBuffer/IdxBuffer, don't destruct again
				_Channels[i].CmdBuffer.clear();
				_Channels[i].IdxBuffer.clear();
			}
			_Channels.clear();
		}

		// Use macros because C++ is a terrible language, we want guaranteed inline, no code in header, and no overhead in Debug mode
#define GetCurrentClipRect()    (_ClipRectStack.size() ? _ClipRectStack.data()[_ClipRectStack.size()-1]  : GNullClipRect)
#define GetCurrentImage()   (_ImageStack.size() ? _ImageStack.data()[_ImageStack.size()-1] : NULL)
		glm::mat4* GetCurrentMat()
		{
			return (_MatStack.size() ? _MatStack.data()[_MatStack.size() - 1] : NULL);
		}
		void AddDrawCmd()
		{
			DrawCmd draw_cmd;
			draw_cmd.ClipRect = GetCurrentClipRect();
			draw_cmd.image = GetCurrentImage();

			//assert(draw_cmd.ClipRect.x <= draw_cmd.ClipRect.z && draw_cmd.ClipRect.y <= draw_cmd.ClipRect.w);
			CmdBuffer.push_back(draw_cmd);
		}

		//void AddCallback(ImDrawCallback callback, void* callback_data)
		//{
		//	DrawCmd* current_cmd = CmdBuffer.size() ? &CmdBuffer.back() : NULL;
		//	if (!current_cmd || current_cmd->ElemCount != 0 || current_cmd->UserCallback != NULL)
		//	{
		//		AddDrawCmd();
		//		current_cmd = &CmdBuffer.back();
		//	}
		//	current_cmd->UserCallback = callback;
		//	current_cmd->UserCallbackData = callback_data;

		//	AddDrawCmd(); // Force a new command after us (see comment below)
		//}

		// Our scheme may appears a bit unusual, basically we want the most-common calls draw_line draw_rect etc. to not have to perform any check so we always have a command ready in the stack.
		// The cost of figuring out if a new command has to be added or if we can merge is paid in those Update** functions only.
		void UpdateClipRect()
		{
			// If current command is used with different settings we need to add a new command
			glm::vec4 curr_clip_rect = GetCurrentClipRect();
			DrawCmd* curr_cmd = CmdBuffer.size() > 0 ? &CmdBuffer[CmdBuffer.size() - 1] : NULL;
			if (!curr_cmd || (curr_cmd->ElemCount != 0 && memcmp(&curr_cmd->ClipRect, &curr_clip_rect, sizeof(glm::vec4)) != 0))
			{
				AddDrawCmd();
				return;
			}

			// Try to merge with previous command if it matches, else use current command
			DrawCmd* prev_cmd = CmdBuffer.size() > 1 ? curr_cmd - 1 : NULL;
			if (curr_cmd->ElemCount == 0 && prev_cmd && memcmp(&prev_cmd->ClipRect, &curr_clip_rect, sizeof(glm::vec4)) == 0 && prev_cmd->image == GetCurrentImage() && prev_cmd->mat == GetCurrentMat())
				CmdBuffer.pop_back();
			else
				curr_cmd->ClipRect = curr_clip_rect;
		}
		void UpdateImage()
		{
			// If current command is used with different settings we need to add a new command
			Image* curr_texture_id = GetCurrentImage();
			DrawCmd* curr_cmd = CmdBuffer.size() ? &CmdBuffer.back() : NULL;
			if (!curr_cmd || (curr_cmd->ElemCount != 0 && curr_cmd->image != curr_texture_id))
			{
				AddDrawCmd();
				return;
			}

			// Try to merge with previous command if it matches, else use current command
			DrawCmd* prev_cmd = CmdBuffer.size() > 1 ? curr_cmd - 1 : NULL;
			if (prev_cmd && prev_cmd->image == curr_texture_id && memcmp(&prev_cmd->ClipRect, &GetCurrentClipRect(), sizeof(glm::vec4)) == 0 && prev_cmd->mat == GetCurrentMat())
				CmdBuffer.pop_back();
			else
				curr_cmd->image = curr_texture_id;
		}
		void UpdateMat()
		{
			// If current command is used with different settings we need to add a new command
			glm::mat4* curr = GetCurrentMat();
			DrawCmd* curr_cmd = CmdBuffer.size() ? &CmdBuffer.back() : NULL;
			if (!curr_cmd || (curr_cmd->ElemCount != 0 && curr_cmd->mat != curr))
			{
				AddDrawCmd();
				return;
			}

			// Try to merge with previous command if it matches, else use current command
			DrawCmd* prev_cmd = CmdBuffer.size() > 1 ? curr_cmd - 1 : NULL;
			if (prev_cmd && prev_cmd->mat == curr && memcmp(&prev_cmd->ClipRect, &GetCurrentClipRect(), sizeof(glm::vec4)) == 0 && prev_cmd->image == GetCurrentImage())
				CmdBuffer.pop_back();
			else
				curr_cmd->mat = curr;
		}



#undef GetCurrentClipRect
#undef GetCurrentImage
		void PushMat(glm::mat4* mat)
		{
			_MatStack.push_back(mat);
			UpdateMat();
		}
		void PopMat()
		{
			assert(_ClipRectStack.size() > 0);
			_MatStack.pop_back();
			UpdateMat();
		}
		// Render-level scissoring. This is passed down to your render function but not used for CPU-side coarse clipping. Prefer using higher-level ImGui::PushClipRect() to affect logic (hit-testing and widget culling)
		void PushClipRect(glm::vec2 cr_min, glm::vec2 cr_max, bool intersect_with_current_clip_rect = false)
		{
			glm::vec4 cr(cr_min.x, cr_min.y, cr_max.x, cr_max.y);
			if (intersect_with_current_clip_rect && _ClipRectStack.size())
			{
				glm::vec4 current = _ClipRectStack.back();
				if (cr.x < current.x) cr.x = current.x;
				if (cr.y < current.y) cr.y = current.y;
				if (cr.z > current.z) cr.z = current.z;
				if (cr.w > current.w) cr.w = current.w;
			}
			//cr.z = std::max(cr.x, cr.z);
			//cr.w = std::max(cr.y, cr.w);
			if (cr.z < 0)
			{
				cr.z = cr.z;
			}
			_ClipRectStack.push_back(cr);
			UpdateClipRect();
		}
		void PushClipRect(glm::vec4 cr, bool intersect_with_current_clip_rect = false)
		{
			PushClipRect({ cr.x,cr.y }, { cr.x + cr.z,cr.y + cr.w }, intersect_with_current_clip_rect);
		}
		void PushClipRectFullScreen()
		{
			PushClipRect(glm::vec2(GNullClipRect.x, GNullClipRect.y), glm::vec2(GNullClipRect.z, GNullClipRect.w));
			//PushClipRect(GetVisibleRect());   // FIXME-OPT: This would be more correct but we're not supposed to access ImGuiContext from here?
		}

		void PopClipRect()
		{
			if (_ClipRectStack.size() > 0)
			{
				assert(_ClipRectStack.size() > 0);
				_ClipRectStack.pop_back();
				UpdateClipRect();
			}
		}
		void PushImage(Image* image)
		{
			_ImageStack.push_back(image);
			insertImage(image);
			UpdateImage();
		}

		void PopImage()
		{
			if (_ImageStack.size() > 0)
			{
				_ImageStack.pop_back();
				UpdateImage();
			}
		}
		void insertImage(Image* image)
		{
			LOCK_W(_lock_mi);
			if (image)
				_make_image.insert(image);
		}
		//void getMakeImage(std::set<void*>& out)
		//{
		//	LOCK(lock_mi);
		//	out.swap(_make_image);
		//}
		//bool makeImageEmpty()
		//{
		//	return _make_image.empty();
		//}
	public:
		void ChannelsSplit(int channels_count)
		{
			//IM_ASSERT(_ChannelsCurrent == 0 && _ChannelsCount == 1);
			int old_channels_count = _Channels.size();
			if (old_channels_count < channels_count)
				_Channels.resize(channels_count);
			_ChannelsCount = channels_count;

			// 			 _Channels[] (24 bytes each) hold storage that we'll swap with this->_CmdBuffer/_IdxBuffer
			// 			 The content of _Channels[0] at this point doesn't matter. We clear it to make state tidy in a debugger but we don't strictly need to.
			// 			 When we switch to the next channel, we'll copy _CmdBuffer/_IdxBuffer into _Channels[0] and then _Channels[1] into _CmdBuffer/_IdxBuffer
						//memset(&_Channels[0], 0, sizeof(DrawChannel));
			for (int i = 1; i < channels_count; i++)
			{
				if (i >= old_channels_count)
				{
					//IM_PLACEMENT_NEW(&_Channels[i]) DrawChannel();
				}
				else
				{
					_Channels[i].CmdBuffer.resize(0);
					_Channels[i].IdxBuffer.resize(0);
				}
				if (_Channels[i].CmdBuffer.size() == 0)
				{
					DrawCmd draw_cmd;
					draw_cmd.ClipRect = _ClipRectStack.back();
					draw_cmd.image = _ImageStack.back();
					_Channels[i].CmdBuffer.push_back(draw_cmd);
				}
			}
		}

		void ChannelsMerge()
		{
			// Note that we never use or rely on channels.size() because it is merely a buffer that we never shrink back to 0 to keep all sub-buffers ready for use.
			if (_ChannelsCount <= 1)
				return;

			ChannelsSetCurrent(0);
			if (CmdBuffer.size() && CmdBuffer.back().ElemCount == 0)
				CmdBuffer.pop_back();

			int new_cmd_buffer_count = 0, new_idx_buffer_count = 0;
			for (int i = 1; i < _ChannelsCount; i++)
			{
				DrawChannel& ch = _Channels[i];
				if (ch.CmdBuffer.size() && ch.CmdBuffer.back().ElemCount == 0)
					ch.CmdBuffer.pop_back();
				new_cmd_buffer_count += ch.CmdBuffer.size();
				new_idx_buffer_count += ch.IdxBuffer.size();
			}
			CmdBuffer.resize(CmdBuffer.size() + new_cmd_buffer_count);
			IdxBuffer.resize(IdxBuffer.size() + new_idx_buffer_count);

			DrawCmd* cmd_write = CmdBuffer.data() + CmdBuffer.size() - new_cmd_buffer_count;
			_IdxWritePtr = IdxBuffer.data() + IdxBuffer.size() - new_idx_buffer_count;
			for (int i = 1; i < _ChannelsCount; i++)
			{
				DrawChannel& ch = _Channels[i];
				if (int sz = ch.CmdBuffer.size()) { memcpy(cmd_write, ch.CmdBuffer.data(), sz * sizeof(DrawCmd)); cmd_write += sz; }
				if (int sz = ch.IdxBuffer.size()) { memcpy(_IdxWritePtr, ch.IdxBuffer.data(), sz * sizeof(DrawIdx)); _IdxWritePtr += sz; }
			}
			AddDrawCmd();
			_ChannelsCount = 1;
		}

		void ChannelsSetCurrent(int idx)
		{
			//IM_ASSERT(idx < _ChannelsCount);
			if (_ChannelsCurrent == idx) return;
			memcpy(&_Channels.data()[_ChannelsCurrent].CmdBuffer, &CmdBuffer, sizeof(CmdBuffer)); // copy 12 bytes, four times
			memcpy(&_Channels.data()[_ChannelsCurrent].IdxBuffer, &IdxBuffer, sizeof(IdxBuffer));
			_ChannelsCurrent = idx;
			memcpy(&CmdBuffer, &_Channels.data()[_ChannelsCurrent].CmdBuffer, sizeof(CmdBuffer));
			memcpy(&IdxBuffer, &_Channels.data()[_ChannelsCurrent].IdxBuffer, sizeof(IdxBuffer));
			_IdxWritePtr = IdxBuffer.data() + IdxBuffer.size();
		}

		// NB: this can be called with negative count for removing primitives (as long as the result does not underflow)
		void PrimReserve(int idx_count, int vtx_count, int* VtxPtr = 0)
		{
			DrawCmd& draw_cmd = CmdBuffer.data()[CmdBuffer.size() - 1];
			draw_cmd.ElemCount += idx_count;
			_idx_count = idx_count;
			_vtx_count = vtx_count;
			int vtx_buffer_size = VtxBuffer.size();
			VtxBuffer.resize(vtx_buffer_size + vtx_count);
			_VtxWritePtr = VtxBuffer.data() + vtx_buffer_size;
			if (VtxPtr)
			{
				*VtxPtr = vtx_buffer_size;
			}
			int idx_buffer_size = IdxBuffer.size();
			IdxBuffer.resize(idx_buffer_size + idx_count);
			_IdxWritePtr = IdxBuffer.data() + idx_buffer_size;
		}

		// Fully unrolled with inline call to keep our debug builds decently fast.
		void PrimRect(const glm::vec2& a, const glm::vec2& c, unsigned int col)
		{
			glm::vec2 b(c.x, a.y), d(a.x, c.y), uv(FontTexUvWhitePixel);
			DrawIdx idx = (DrawIdx)_VtxCurrentIdx;
			_IdxWritePtr[0] = idx; _IdxWritePtr[1] = (DrawIdx)(idx + 1); _IdxWritePtr[2] = (DrawIdx)(idx + 2);
			_IdxWritePtr[3] = idx; _IdxWritePtr[4] = (DrawIdx)(idx + 2); _IdxWritePtr[5] = (DrawIdx)(idx + 3);
			setVec(_VtxWritePtr[0].pos, a); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
			setVec(_VtxWritePtr[1].pos, b); _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
			setVec(_VtxWritePtr[2].pos, c); _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
			setVec(_VtxWritePtr[3].pos, d); _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
			_VtxWritePtr += 4;
			_VtxCurrentIdx += 4;
			_IdxWritePtr += 6;
		}
		void PrimRectUV(const glm::vec2& a, const glm::vec2& c, const glm::vec2& uv_a, const glm::vec2& uv_c, unsigned int col)
		{
			glm::vec2 b(c.x, a.y), d(a.x, c.y), uv_b(uv_c.x, uv_a.y), uv_d(uv_a.x, uv_c.y);
			DrawIdx idx = (DrawIdx)_VtxCurrentIdx;
			_IdxWritePtr[0] = idx; _IdxWritePtr[1] = (DrawIdx)(idx + 1); _IdxWritePtr[2] = (DrawIdx)(idx + 2);
			_IdxWritePtr[3] = idx; _IdxWritePtr[4] = (DrawIdx)(idx + 2); _IdxWritePtr[5] = (DrawIdx)(idx + 3);
			setVec(_VtxWritePtr[0].pos, a); _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
			setVec(_VtxWritePtr[1].pos, b); _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
			setVec(_VtxWritePtr[2].pos, c); _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
			setVec(_VtxWritePtr[3].pos, d); _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
			_VtxWritePtr += 4;
			_VtxCurrentIdx += 4;
			_IdxWritePtr += 6;
		}

		void PrimQuadUV(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, const glm::vec2& uv_a, const glm::vec2& uv_b, const glm::vec2& uv_c, const glm::vec2& uv_d, unsigned int col)
		{
			DrawIdx idx = (DrawIdx)_VtxCurrentIdx;
			_IdxWritePtr[0] = idx; _IdxWritePtr[1] = (DrawIdx)(idx + 1); _IdxWritePtr[2] = (DrawIdx)(idx + 2);
			_IdxWritePtr[3] = idx; _IdxWritePtr[4] = (DrawIdx)(idx + 2); _IdxWritePtr[5] = (DrawIdx)(idx + 3);
			setVec(_VtxWritePtr[0].pos, a); _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
			setVec(_VtxWritePtr[1].pos, b); _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
			setVec(_VtxWritePtr[2].pos, c); _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
			setVec(_VtxWritePtr[3].pos, d); _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
			_VtxWritePtr += 4;
			_VtxCurrentIdx += 4;
			_IdxWritePtr += 6;
		}
		inline void PrimWriteVtx(const glm::vec2& pos, const glm::vec2& uv, unsigned int col)
		{
			setVec(_VtxWritePtr->pos, pos); _VtxWritePtr->uv = uv; _VtxWritePtr->col = col; _VtxWritePtr++; _VtxCurrentIdx++;
		}
		inline void PrimWriteIdx(DrawIdx idx) { *_IdxWritePtr = idx; _IdxWritePtr++; }
		inline void PrimWriteIdxs(DrawIdx* idx, size_t c)
		{
			if (idx)
			{
				memcpy(_IdxWritePtr, idx, sizeof(DrawIdx) * c);
			}
			_IdxWritePtr += c;
		}
		inline void PrimVtx(const glm::vec2& pos, const glm::vec2& uv, unsigned int col) { PrimWriteIdx((DrawIdx)_VtxCurrentIdx); PrimWriteVtx(pos, uv, col); }
#if 1
		//修改属性
		void setVtx(int index, DrawVert* dv)
		{
			DrawVert* vtx = VtxBuffer.data();
			if (!vtx)
				return;
			if (index < VtxBuffer.size())
				*(vtx + index) = *dv;
		}
		void setRect(int index, const glm::vec2& a, const glm::vec2& c, unsigned int col)
		{
			glm::vec2 b(c.x, a.y), d(a.x, c.y), uv(FontTexUvWhitePixel);
			DrawVert* vtx = VtxBuffer.data();
			if (!vtx)
				return;
			vtx += index;
			setVec(vtx[0].pos, a); vtx[0].uv = uv; vtx[0].col = col;
			setVec(vtx[1].pos, b); vtx[1].uv = uv; vtx[1].col = col;
			setVec(vtx[2].pos, c); vtx[2].uv = uv; vtx[2].col = col;
			setVec(vtx[3].pos, d); vtx[3].uv = uv; vtx[3].col = col;
		}

		void setRectUV(int index, const glm::vec2& a, const glm::vec2& c, const glm::vec2& uv_a, const glm::vec2& uv_c, unsigned int col)
		{
			glm::vec2 b(c.x, a.y), d(a.x, c.y), uv_b(uv_c.x, uv_a.y), uv_d(uv_a.x, uv_c.y);
			DrawVert* vtx = VtxBuffer.data();
			if (!vtx)
				return;
			vtx += index;
			setVec(vtx[0].pos, a); vtx[0].uv = uv_a; vtx[0].col = col;
			setVec(vtx[1].pos, b); vtx[1].uv = uv_b; vtx[1].col = col;
			setVec(vtx[2].pos, c); vtx[2].uv = uv_c; vtx[2].col = col;
			setVec(vtx[3].pos, d); vtx[3].uv = uv_d; vtx[3].col = col;
		}
		void setQuadUV(int index, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, const glm::vec2& uv_a, const glm::vec2& uv_b, const glm::vec2& uv_c, const glm::vec2& uv_d, unsigned int col)
		{
			DrawVert* vtx = VtxBuffer.data();
			if (!vtx)
				return;
			vtx += index;
			setVec(vtx[0].pos, a); vtx[0].uv = uv_a; vtx[0].col = col;
			setVec(vtx[1].pos, b); vtx[1].uv = uv_b; vtx[1].col = col;
			setVec(vtx[2].pos, c); vtx[2].uv = uv_c; vtx[2].col = col;
			setVec(vtx[3].pos, d); vtx[3].uv = uv_d; vtx[3].col = col;
		}
#endif

		//路径
#if 1
		static void get_sphere(double radius)
		{
			auto func = [radius](double u, double v)->glm::vec3 {
				double x = radius * std::sin(_PI * v) * radius * std::cos(2.0 * _PI * u);
				double y = radius * std::sin(_PI * v) * radius * std::sin(2.0 * _PI * u);
				double z = radius * std::cos(_PI * u);
				return { x,y,z };
			};
		}
		//对称得到整个圆
		static void CirPot(int cx, int cy, int x, int y, std::vector<glm::vec2>* path)
		{
			int rx, ry;
			//  if (cx != cy)
			{
				rx = (cx + x - 1); ry = (cy + y - 1);
				path->push_back({ rx, ry });

				rx = (cx + y - 1); ry = (cy - x);
				path->push_back({ rx, ry });

				rx = (cx - x); ry = (cy - y);
				path->push_back({ rx, ry });

				rx = (cx - y); ry = (cy + x - 1);
				path->push_back({ rx, ry });
			}
			if (x != y)
			{
				rx = (cx + y - 1); ry = (cy + x - 1);
				path->push_back({ rx, ry });
				rx = (cx + x - 1); ry = (cy - y);
				path->push_back({ rx, ry });
				rx = (cx - y); ry = (cy - x);
				path->push_back({ rx, ry });
				rx = (cx - x); ry = (cy + y - 1);
				path->push_back({ rx, ry });
			}
		}
		// 先得到1 / 8圆, 在对称画出整个圆

		//正负法画圆

		static void Drewcircle1(int cx, int cy, int r, std::vector<glm::vec2>* path)
		{
			int x = r, y = 0, F = 0, n = r;
			while (n > 0)
			{
				CirPot(cx, cy, x, y, path);
				if (F <= 0)
				{
					F += 2 * y + 1;
					y += 1;
				}
				else
				{
					F += 1 - 2 * x;
					x -= 1;
				}
				n--;
			}
			if (x == y)
				CirPot(cx, cy, x, y, path);
		}

		//DDA法画圆(数值微分法)

		static void Drewcircle2(int cx, int cy, int r, std::vector<glm::vec2>* path)
		{
			int x = 0, y = r, n = r;
			double cr = (double)r;
			double z = 1.0 / cr;
			double a = x, b = y;
			double tmp;
			while (n > 0)
			{

				CirPot(cx, cy, x, y, path);
				tmp = a;
				a -= b * z;
				b += tmp * z;
				x = (int)(a);
				y = (int)(b);
				n--;
			}
			if (x == y)
				CirPot(cx, cy, x, y, path);
		}

		//Bresenham法画圆
		static void Drewcircle3(int cx, int cy, int r, std::vector<glm::vec2>* path, int fx = -1)
		{
			int x = 0, y = r, F = 3 - 2 * r;
			int inc = 0;
			while (x < y)
			{
				if (fx == -1)
					CirPot(cx, cy, x, y, path);
				else
				{
					if (inc < r / 2)
						CirPot(cx, cy, x, y, path);
				}
				if (F < 0)
					F += 4 * x + 6;
				else
				{
					F += 4 * (x - y) + 10;
					y--;
				}
				x++;
				++inc;
			}
			{
				if (x == y)
					CirPot(cx, cy, x, y, path);
			}
		}
		/*
		椭圆路径生成
		centre中点vec2，radius半径vec2，num_segments数量int，path输出，
		pos从哪个位置开始
		ivec2 range范围，默认{0，-1}全部，
		offset半径缩小，closure是否闭合，false可以省空间

		*/
		size_t path_ellipse_circle(const glm::vec2& centre, glm::vec2 radius, int num_segments,
			std::vector<glm::vec2>* path = nullptr, double pos = 0, glm::ivec2 range = { 0,-1 },
			double offset = 0.0f, bool closure = true, glm::vec4* rect_scale = nullptr)
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
			int64_t n = std::min((unsigned int)range.y, (unsigned int)num_segments);
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
#endif
		static void get_path_idxf(glm::vec4* idx_pos, std::vector<glm::vec2>* path)
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


		//add start
#if 1
		// TODO: Thickness anti-aliased lines cap are missing their AA fringe.
		void draw_polyline(const glm::vec2* points, const int points_count, unsigned int col, bool closed, float thickness, bool anti_aliased)
		{
			if (points_count < 2)
				return;

			const glm::vec2 uv = FontTexUvWhitePixel;
			//anti_aliased &= GImGui->Style.AntiAliasedLines;
			//if (ImGui::GetIO().KeyCtrl) anti_aliased = false; // Debug

			int count = points_count;
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
				PrimReserve(idx_count, vtx_count);

				// Temporary buffer
				glm::vec2* temp_normals = (glm::vec2*)alloca(points_count * (thick_line ? 5 : 3) * sizeof(glm::vec2));
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
						temp_points[0] = points[0] + temp_normals[0] * AA_SIZE;
						temp_points[1] = points[0] - temp_normals[0] * AA_SIZE;
						temp_points[(points_count - 1) * 2 + 0] = points[points_count - 1] + temp_normals[points_count - 1] * AA_SIZE;
						temp_points[(points_count - 1) * 2 + 1] = points[points_count - 1] - temp_normals[points_count - 1] * AA_SIZE;
					}

					// FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
					unsigned int idx1 = _VtxCurrentIdx;
					for (int i1 = 0; i1 < count; i1++)
					{
						const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
						unsigned int idx2 = (i1 + 1) == points_count ? _VtxCurrentIdx : idx1 + 3;

						// Average normals
						glm::vec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
						float dmr2 = dm.x * dm.x + dm.y * dm.y;
						if (dmr2 > 0.000001f)
						{
							float scale = 1.0f / dmr2;
							if (scale > 100.0f) scale = 100.0f;
							dm *= scale;
						}
						dm *= AA_SIZE;
						temp_points[i2 * 2 + 0] = points[i2] + dm;
						temp_points[i2 * 2 + 1] = points[i2] - dm;
						int iwp[12] = { 0 };
						// Add indexes
						_IdxWritePtr[0] = (DrawIdx)(idx2 + 0); _IdxWritePtr[1] = (DrawIdx)(idx1 + 0); _IdxWritePtr[2] = (DrawIdx)(idx1 + 2);
						_IdxWritePtr[3] = (DrawIdx)(idx1 + 2); _IdxWritePtr[4] = (DrawIdx)(idx2 + 2); _IdxWritePtr[5] = (DrawIdx)(idx2 + 0);
						_IdxWritePtr[6] = (DrawIdx)(idx2 + 1); _IdxWritePtr[7] = (DrawIdx)(idx1 + 1); _IdxWritePtr[8] = (DrawIdx)(idx1 + 0);
						_IdxWritePtr[9] = (DrawIdx)(idx1 + 0); _IdxWritePtr[10] = (DrawIdx)(idx2 + 0); _IdxWritePtr[11] = (DrawIdx)(idx2 + 1);
						for (int nk = 0; nk < 12; nk++)
						{
							iwp[nk] = _IdxWritePtr[nk];
						}
						_IdxWritePtr += 12;

						idx1 = idx2;
					}
					glm::vec2  poss[3];
					// Add vertexes
					for (int i = 0; i < points_count; i++)
					{
						setVec(_VtxWritePtr[0].pos, points[i]);          _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
						setVec(_VtxWritePtr[1].pos, temp_points[i * 2 + 0]); _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;
						setVec(_VtxWritePtr[2].pos, temp_points[i * 2 + 1]); _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col_trans;
						for (int pi = 0; pi < 3; pi++)
							poss[pi] = _VtxWritePtr[pi].pos * 10.0f;
						_VtxWritePtr += 3;
					}
				}
				else
				{
					const float half_inner_thickness = (thickness - AA_SIZE) * 0.5f;
					if (!closed)
					{
						temp_points[0] = points[0] + temp_normals[0] * (half_inner_thickness + AA_SIZE);
						temp_points[1] = points[0] + temp_normals[0] * (half_inner_thickness);
						temp_points[2] = points[0] - temp_normals[0] * (half_inner_thickness);
						temp_points[3] = points[0] - temp_normals[0] * (half_inner_thickness + AA_SIZE);
						temp_points[(points_count - 1) * 4 + 0] = points[points_count - 1] + temp_normals[points_count - 1] * (half_inner_thickness + AA_SIZE);
						temp_points[(points_count - 1) * 4 + 1] = points[points_count - 1] + temp_normals[points_count - 1] * (half_inner_thickness);
						temp_points[(points_count - 1) * 4 + 2] = points[points_count - 1] - temp_normals[points_count - 1] * (half_inner_thickness);
						temp_points[(points_count - 1) * 4 + 3] = points[points_count - 1] - temp_normals[points_count - 1] * (half_inner_thickness + AA_SIZE);
					}

					// FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
					unsigned int idx1 = _VtxCurrentIdx;
					for (int i1 = 0; i1 < count; i1++)
					{
						const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
						unsigned int idx2 = (i1 + 1) == points_count ? _VtxCurrentIdx : idx1 + 4;

						// Average normals
						glm::vec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
						float dmr2 = dm.x * dm.x + dm.y * dm.y;
						if (dmr2 > 0.000001f)
						{
							float scale = 1.0f / dmr2;
							if (scale > 100.0f) scale = 100.0f;
							dm *= scale;
						}
						glm::vec2 dm_out = dm * (half_inner_thickness + AA_SIZE);
						glm::vec2 dm_in = dm * half_inner_thickness;
						temp_points[i2 * 4 + 0] = points[i2] + dm_out;
						temp_points[i2 * 4 + 1] = points[i2] + dm_in;
						temp_points[i2 * 4 + 2] = points[i2] - dm_in;
						temp_points[i2 * 4 + 3] = points[i2] - dm_out;

						// Add indexes
						_IdxWritePtr[0] = (DrawIdx)(idx2 + 1); _IdxWritePtr[1] = (DrawIdx)(idx1 + 1); _IdxWritePtr[2] = (DrawIdx)(idx1 + 2);
						_IdxWritePtr[3] = (DrawIdx)(idx1 + 2); _IdxWritePtr[4] = (DrawIdx)(idx2 + 2); _IdxWritePtr[5] = (DrawIdx)(idx2 + 1);
						_IdxWritePtr[6] = (DrawIdx)(idx2 + 1); _IdxWritePtr[7] = (DrawIdx)(idx1 + 1); _IdxWritePtr[8] = (DrawIdx)(idx1 + 0);
						_IdxWritePtr[9] = (DrawIdx)(idx1 + 0); _IdxWritePtr[10] = (DrawIdx)(idx2 + 0); _IdxWritePtr[11] = (DrawIdx)(idx2 + 1);
						_IdxWritePtr[12] = (DrawIdx)(idx2 + 2); _IdxWritePtr[13] = (DrawIdx)(idx1 + 2); _IdxWritePtr[14] = (DrawIdx)(idx1 + 3);
						_IdxWritePtr[15] = (DrawIdx)(idx1 + 3); _IdxWritePtr[16] = (DrawIdx)(idx2 + 3); _IdxWritePtr[17] = (DrawIdx)(idx2 + 2);

						_IdxWritePtr += 18;

						idx1 = idx2;
					}
					// Add vertexes
					for (int i = 0; i < points_count; i++)
					{
						setVec(_VtxWritePtr[0].pos, temp_points[i * 4 + 0]); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col_trans;
						setVec(_VtxWritePtr[1].pos, temp_points[i * 4 + 1]); _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
						setVec(_VtxWritePtr[2].pos, temp_points[i * 4 + 2]); _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
						setVec(_VtxWritePtr[3].pos, temp_points[i * 4 + 3]); _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col_trans;
						_VtxWritePtr += 4;
					}
				}
				_VtxCurrentIdx += (DrawIdx)vtx_count;
			}
			else
			{
				// Non Anti-aliased Stroke
				const int idx_count = count * 6;
				const int vtx_count = count * 4;      // FIXME-OPT: Not sharing edges
				PrimReserve(idx_count, vtx_count);

				for (int i1 = 0; i1 < count; i1++)
				{
					const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
					const glm::vec2& p1 = points[i1];
					const glm::vec2& p2 = points[i2];
					glm::vec2 diff = p2 - p1;
					diff *= InvLength(diff, 1.0f);

					const float dx = diff.x * (thickness * 0.5f);
					const float dy = diff.y * (thickness * 0.5f);
					_VtxWritePtr[0].pos.x = p1.x + dy; _VtxWritePtr[0].pos.y = p1.y - dx; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
					_VtxWritePtr[1].pos.x = p2.x + dy; _VtxWritePtr[1].pos.y = p2.y - dx; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
					_VtxWritePtr[2].pos.x = p2.x - dy; _VtxWritePtr[2].pos.y = p2.y + dx; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
					_VtxWritePtr[3].pos.x = p1.x - dy; _VtxWritePtr[3].pos.y = p1.y + dx; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
					glm::vec2  poss[4]; int iwp[12] = { 0 };
					for (int pi = 0; pi < 4; pi++)
						poss[pi] = _VtxWritePtr[pi].pos * 10.0f;
					_VtxWritePtr += 4;

					_IdxWritePtr[0] = (DrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (DrawIdx)(_VtxCurrentIdx + 1); _IdxWritePtr[2] = (DrawIdx)(_VtxCurrentIdx + 2);
					_IdxWritePtr[3] = (DrawIdx)(_VtxCurrentIdx); _IdxWritePtr[4] = (DrawIdx)(_VtxCurrentIdx + 2); _IdxWritePtr[5] = (DrawIdx)(_VtxCurrentIdx + 3);
					for (int nk = 0; nk < 6; nk++)
					{
						iwp[nk] = _IdxWritePtr[nk];
					}
					_IdxWritePtr += 6;
					_VtxCurrentIdx += 4;
				}
			}
		}
		// 填充多颜色cols
		void draw_convex_poly_filled(const glm::vec2* points, const int points_count, unsigned int col, bool anti_aliased
			, std::vector<glm::ivec3>* cols = nullptr)
		{
			const glm::vec2 uv = FontTexUvWhitePixel;

			glm::ivec3* ct = nullptr;
			if (cols)
			{
				ct = cols->data();
			}
			if (anti_aliased)
			{
				// Anti-aliased Fill
				const float AA_SIZE = 1.0f;
				const unsigned int col_trans = col & 0x00ffffff;
				const int idx_count = (points_count - 2) * 3 + points_count * 6;
				const int vtx_count = (points_count * 2);
				PrimReserve(idx_count, vtx_count);

				// Add indexes for fill
				unsigned int vtx_inner_idx = _VtxCurrentIdx;
				unsigned int vtx_outer_idx = _VtxCurrentIdx + 1;
				for (int i = 2; i < points_count; i++)
				{
					_IdxWritePtr[0] = (DrawIdx)(vtx_inner_idx); _IdxWritePtr[1] = (DrawIdx)(vtx_inner_idx + ((i - 1) << 1)); _IdxWritePtr[2] = (DrawIdx)(vtx_inner_idx + (i << 1));
					_IdxWritePtr += 3;
				}

				// Compute normals
				glm::vec2* temp_normals = (glm::vec2*)alloca(points_count * sizeof(glm::vec2));
				for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
				{
					const glm::vec2& p0 = points[i0];
					const glm::vec2& p1 = points[i1];
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
						float scale = 1.0f / dmr2;
						if (scale > 100.0f) scale = 100.0f;
						dm *= scale;
					}
					dm *= AA_SIZE * 0.5f;

					if (ct)
					{
						if (ct->y == i1)
						{
							ct++; inner_col = col; outer_col = col_trans;
						}
						if (ct->x == i1)
						{
							inner_col = ct->z; outer_col = ct->z & 0x00ffffff;
						}
					}
					/*if (((int)(points[i1].x) == 876 && (int)(points[i1].y) == 142)
						|| ((int)(points[i1].x) == 583 && (int)(points[i1].y) == 393)
						|| ((int)(points[i1].x) == 423 && (int)(points[i1].y) == 339)
						)
					{
						inner_col = 0xff00ff00;
					}
					else
					{
						inner_col = col;
					}*/
					// Add vertices
					setVec(_VtxWritePtr[0].pos, (points[i1] - dm)); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = inner_col;  // Inner
					setVec(_VtxWritePtr[1].pos, (points[i1] + dm)); _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = outer_col;  // Outer
					_VtxWritePtr += 2;

					// Add indexes for fringes
					_IdxWritePtr[0] = (DrawIdx)(vtx_inner_idx + (i1 << 1)); _IdxWritePtr[1] = (DrawIdx)(vtx_inner_idx + (i0 << 1)); _IdxWritePtr[2] = (DrawIdx)(vtx_outer_idx + (i0 << 1));
					_IdxWritePtr[3] = (DrawIdx)(vtx_outer_idx + (i0 << 1)); _IdxWritePtr[4] = (DrawIdx)(vtx_outer_idx + (i1 << 1)); _IdxWritePtr[5] = (DrawIdx)(vtx_inner_idx + (i1 << 1));
					_IdxWritePtr += 6;
				}
				_VtxCurrentIdx += (DrawIdx)vtx_count;
			}
			else
			{
				// Non Anti-aliased Fill
				const int idx_count = (points_count - 2) * 3;
				const int vtx_count = points_count;
				PrimReserve(idx_count, vtx_count);

				unsigned int inner_col = col;
				for (int i = 0; i < vtx_count; i++)
				{
					if (ct)
					{
						if (ct->y == i)
						{
							ct++; inner_col = col;
						}
						if (ct->x == i)
						{
							inner_col = ct->z;
						}
					}
					setVec(_VtxWritePtr[0].pos, points[i]); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = inner_col;
					_VtxWritePtr++;
				}
				std::vector<glm::ivec3> ts;

				auto is_intersection = [=](const glm::ivec3& idx) {
					glm::vec2 vidx[3] = { points[idx.x], points[idx.y], points[idx.z] };
					glm::vec4 v4[3] = { {vidx[0],vidx[1]},{vidx[0],vidx[2]},{vidx[1],vidx[2]} };
					for (auto it : ts)
					{
						glm::vec2 vidx[] = { points[it.x], points[it.y], points[it.z] };
						glm::vec4 it4[3] = { {vidx[0],vidx[1]},{vidx[0],vidx[2]},{vidx[1],vidx[2]} };
						for (int i = 0; i < 3; i++)
						{
							for (int j = 0; j < 3; j++)
							{
								if (gutil::intersection(v4[j], it4[i]))
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

						_IdxWritePtr[0] = (DrawIdx)(_VtxCurrentIdx);
						_IdxWritePtr[1] = (DrawIdx)(_VtxCurrentIdx + i - 1);
						_IdxWritePtr[2] = (DrawIdx)(_VtxCurrentIdx + i);
						_IdxWritePtr += 3;
					}
					else
					{
						i = i;
					}
					cur += 3;
				}
				_VtxCurrentIdx += (DrawIdx)vtx_count;
			}
		}


		// 填充多边形
		void draw_convex_path_filled(const glm::vec2* points, const int points_count, unsigned int col, bool anti_aliased
			, std::vector<glm::ivec3>* cols = nullptr)
		{
			const glm::vec2 uv = FontTexUvWhitePixel;

			glm::ivec3* ct = nullptr;
			if (cols)
			{
				ct = cols->data();
			}
			if (anti_aliased)
			{
				// Anti-aliased Fill
				const float AA_SIZE = 1.0f;
				const unsigned int col_trans = col & 0x00ffffff;
				const int idx_count = (points_count - 2) * 3 + points_count * 6;
				const int vtx_count = (points_count * 2);
				PrimReserve(idx_count, vtx_count);

				// Add indexes for fill
				unsigned int vtx_inner_idx = _VtxCurrentIdx;
				unsigned int vtx_outer_idx = _VtxCurrentIdx + 1;
				for (int i = 2; i < points_count; i++)
				{
					_IdxWritePtr[0] = (DrawIdx)(vtx_inner_idx); _IdxWritePtr[1] = (DrawIdx)(vtx_inner_idx + ((i - 1) << 1)); _IdxWritePtr[2] = (DrawIdx)(vtx_inner_idx + (i << 1));
					_IdxWritePtr += 3;
				}

				// Compute normals
				glm::vec2* temp_normals = (glm::vec2*)alloca(points_count * sizeof(glm::vec2));
				for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
				{
					const glm::vec2& p0 = points[i0];
					const glm::vec2& p1 = points[i1];
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
						float scale = 1.0f / dmr2;
						if (scale > 100.0f) scale = 100.0f;
						dm *= scale;
					}
					dm *= AA_SIZE * 0.5f;

					if (ct)
					{
						if (ct->y == i1)
						{
							ct++; inner_col = col; outer_col = col_trans;
						}
						if (ct->x == i1)
						{
							inner_col = ct->z; outer_col = ct->z & 0x00ffffff;
						}
					}
					// Add vertices
					setVec(_VtxWritePtr[0].pos, (points[i1] - dm)); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = inner_col;  // Inner
					setVec(_VtxWritePtr[1].pos, (points[i1] + dm)); _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = outer_col;  // Outer
					_VtxWritePtr += 2;

					// Add indexes for fringes
					_IdxWritePtr[0] = (DrawIdx)(vtx_inner_idx + (i1 << 1)); _IdxWritePtr[1] = (DrawIdx)(vtx_inner_idx + (i0 << 1)); _IdxWritePtr[2] = (DrawIdx)(vtx_outer_idx + (i0 << 1));
					_IdxWritePtr[3] = (DrawIdx)(vtx_outer_idx + (i0 << 1)); _IdxWritePtr[4] = (DrawIdx)(vtx_outer_idx + (i1 << 1)); _IdxWritePtr[5] = (DrawIdx)(vtx_inner_idx + (i1 << 1));
					_IdxWritePtr += 6;
				}
				_VtxCurrentIdx += (DrawIdx)vtx_count;
			}
			else
			{
				// Non Anti-aliased Fill
				const int idx_count = (points_count - 2) * 3;
				const int vtx_count = points_count;
				PrimReserve(idx_count, vtx_count);

				unsigned int inner_col = col;
				for (int i = 0; i < vtx_count; i++)
				{
					if (ct)
					{
						if (ct->y == i)
						{
							ct++; inner_col = col;
						}
						if (ct->x == i)
						{
							inner_col = ct->z;
						}
					}
					setVec(_VtxWritePtr[0].pos, points[i]); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = inner_col;
					_VtxWritePtr++;
				}
				for (int i = 2; i < points_count; i++)
				{
					_IdxWritePtr[0] = (DrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (DrawIdx)(_VtxCurrentIdx + i - 1); _IdxWritePtr[2] = (DrawIdx)(_VtxCurrentIdx + i);
					_IdxWritePtr += 3;
				}
				_VtxCurrentIdx += (DrawIdx)vtx_count;
			}
		}
#if 1
		size_t PathArcToFast(const glm::vec2& centre, float radius, int amin, int amax, float scale = 1.0f)
		{
			static int aa[13] = {};
			static std::once_flag flag;
			static glm::vec2 circle_vtx[12];
			static bool circle_vtx_builds = false;
			static auto mcv = [](int n) {
				std::vector<glm::vec2> cv;
				for (int i = 0; i < n; i++)
				{
					const float a = ((float)i / (float)n) * 2 * _PI;
					cv.push_back({ cosf(a), sinf(a) });
				}
				return cv;
			};
			const int circle_vtx_count = ARRAYSIZE(circle_vtx);
			if (!circle_vtx_builds)
			{
				std::call_once(flag, [&]() {
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
		inline void PathClear()
		{
			_points.clear();
		}
		inline void PathLineTo(const glm::vec2& pos)
		{
			_points.push_back(pos);
		}
		inline void path_merge()
		{
			if (_points.empty())
			{
				return;
			}
			auto fb = _points[0];
			if (_sv.size() < _points.size())
			{
				_sv.reserve(_points.size());
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
		inline void PathLineToMergeDuplicate(const glm::vec2& pos)
		{
			if (_points.size() == 0 || memcmp(&_points[_points.size() - 1], &pos, 8) != 0) _points.push_back(pos);
		}
		inline void PathFillConvex(unsigned int col, bool isclear = true, bool a = true, std::vector<glm::ivec3>* cols = nullptr)
		{
			draw_convex_poly_filled(_points.data(), _points.size(), col, a, cols); if (isclear) { PathClear(); }
		}
		inline void PathStroke(unsigned int col, bool closed, float thickness = 1.0f, bool a = true)
		{
			draw_polyline(_points.data(), _points.size(), col, closed, thickness, a); PathClear();
		}
		void PathArcTo(const glm::vec2& centre, float radius, int num_segments, bool closure,
			std::vector<glm::vec2>* path = nullptr)
		{
			double pos = 0; glm::ivec2 range = { 0,-1 };
			double offset = 0.0f;
			path_ellipse_circle(centre, { radius,radius }, num_segments, path, pos, range, offset, closure);
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
		size_t PathArcTo(const glm::vec2& centre, const glm::vec2& r, int num_segments, bool closure, std::vector<glm::vec2>* path = nullptr)
		{
			double pos = 0; glm::ivec2 range = { 0,-1 };
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
		void PathArcTo(const glm::vec2& centre, const glm::vec2& r, int num_start, int num_end, int num_segments,
			std::vector<glm::vec2>* path = nullptr)
		{
			path_ellipse_circle(centre, r, num_segments, path, 0.0, { num_start,num_end });
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
		void PathLarmeTo(const glm::vec2& centre, float radius, float amin, float amax, int num_segments, int n)
		{
			if (radius == 0.0f)
				_points.push_back(centre);
			_points.reserve(_points.size() + (num_segments + 1));
			for (int i = 0; i <= num_segments; i++)
			{
				const float a = amin + ((float)i / (float)num_segments) * (amax - amin);
				_points.push_back(glm::vec2(centre.x + cosf(a) * radius,
					centre.y + sinf(a) * radius * powf(sinf(a / 2.0f), n)));
			}
		}

		static void PathBezierToCasteljau(std::vector<glm::vec2>* path
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
		void PathBezierCurveTo(const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4, int num_segments)
		{
			glm::vec2 p1 = _points.back();
			if (num_segments <= 0) // Auto-tessellated
			{
				PathBezierToCasteljau(&_points, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, CurveTessellationTol, 0);
			}
			else
			{
				float t_step = 1.0f / (float)num_segments;
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

		void PathRect(const glm::vec2& a, const glm::vec2& b, float rounding = 0.0f, int rounding_corners = ~0)
		{
			float r = rounding;
			r = std::min(r, fabsf(b.x - a.x) * (((rounding_corners & (1 | 2)) == (1 | 2)) || ((rounding_corners & (4 | 8)) == (4 | 8)) ? 0.5f : 1.0f) - 1.0f);
			r = std::min(r, fabsf(b.y - a.y) * (((rounding_corners & (1 | 8)) == (1 | 8)) || ((rounding_corners & (2 | 4)) == (2 | 4)) ? 0.5f : 1.0f) - 1.0f);

			if (r <= 0.0f || rounding_corners == 0)
			{
				PathLineTo(a);
				PathLineTo(glm::vec2(b.x, a.y));
				PathLineTo(b);
				PathLineTo(glm::vec2(a.x, b.y));
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
#endif
		void draw_line(const glm::vec2& a, const glm::vec2& b, unsigned int col, float thickness = 1.0f, bool anti_aliased = true)
		{
			if ((col & HZ_COL32_A_MASK) == 0)
				return;
			PathLineTo(a + glm::vec2(0.5f, 0.5f));
			PathLineTo(b + glm::vec2(0.5f, 0.5f));
			PathStroke(col, false, thickness, anti_aliased);
		}

		// a: upper-left, b: lower-right. we don't render 1 px sized rectangles properly.
		void add_rect_a(const glm::vec2& a, const glm::vec2& b, unsigned int col, unsigned int fill, float rounding = 0.0f, int rounding_corners_flags = ~0, float thickness = 1.0f)
		{
			bool iscol = (col & HZ_COL32_A_MASK);
			float pof = iscol ? 0.5 : 0.0;
			if ((fill & HZ_COL32_A_MASK))
			{
				add_rect_filled(a + (thickness * pof), b - (thickness * pof), fill, rounding, rounding_corners_flags);
			}
			if (iscol)
			{
				PathRect(a + glm::vec2(0.5f, 0.5f), b - glm::vec2(0.5f, 0.5f), rounding, rounding_corners_flags);
				PathStroke(col, true, thickness);
			}
		}
		void draw_rect_filled_multi_color(const glm::vec4& a, std::vector<glm::ivec3>* cols = nullptr, float rounding = 0.0f, int rounding_corners_flags = ~0)
		{
			add_rect_filled({ a.x,a.y }, { a.x + a.z,a.y + a.w }, 0, rounding, rounding_corners_flags, cols);
		}
		void add_rect_filled(const glm::vec2& a, const glm::vec2& b, unsigned int col, float rounding = 0.0f, int rounding_corners_flags = ~0, std::vector<glm::ivec3>* cols = nullptr, float offset = 0.5)
		{
			if (!cols && (col & HZ_COL32_A_MASK) == 0)
				return;
			if (rounding > 0.0f)
			{
				PathRect(a, b, rounding, rounding_corners_flags);
				if (cols && cols->size())
				{
					size_t count = _points.size(), cs = cols->size();

					// 如果 cols[0].x小于0 
					if (cs <= 4 && (*cols)[0].x < 0)
					{
						auto n = count / 4;
						float k = 1.0 / cs;
						auto& tem = *cols;
						int c = 0;
						std::vector<glm::ivec3> ncs;
#if 1
						for (int i = 0; i < cs; i++)
						{
							tem[i].x = c;
							c += n;
							tem[i].y = c;
						}
#else
						_points.push_back({ a.x + b.x * 0.5,a.y + b.y * 0.5 });
						c = n * offset;
						int tc = c;
						bool isrb = c > 0;
						std::set<unsigned int> ac;
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
						tem.push_back({ _points.size() - 1,_points.size() - 1,0xf });
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
				PrimReserve(6, 4);
				PrimRect(a, b, col);
			}
		}

		void draw_rect(const glm::ivec4& a, unsigned int col, unsigned int fill, float rounding = 0.0f, int rounding_corners_flags = ~0, float thickness = 1.0f)
		{
			add_rect_a({ a.x,a.y }, { a.x + a.z,a.y + a.w }, col, fill, rounding, rounding_corners_flags, thickness);
		}
		void draw_rect_filled_multi_color(glm::ivec4 a4, unsigned int col_upr_left, unsigned int col_upr_right,
			unsigned int col_bot_right, unsigned int col_bot_left)
		{
			if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) & HZ_COL32_A_MASK) == 0)
				return;
			glm::vec2 a = { a4.x,a4.y }, c = { a4.x + a4.z,a4.y + a4.w };
			const glm::vec2 uv = FontTexUvWhitePixel;
			PrimReserve(6, 4);
			PrimWriteIdx((DrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((DrawIdx)(_VtxCurrentIdx + 1)); PrimWriteIdx((DrawIdx)(_VtxCurrentIdx + 2));
			PrimWriteIdx((DrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((DrawIdx)(_VtxCurrentIdx + 2)); PrimWriteIdx((DrawIdx)(_VtxCurrentIdx + 3));
			PrimWriteVtx(a, uv, col_upr_left);
			PrimWriteVtx(glm::vec2(c.x, a.y), uv, col_upr_right);
			PrimWriteVtx(c, uv, col_bot_right);
			PrimWriteVtx(glm::vec2(a.x, c.y), uv, col_bot_left);
		}
		void draw_rect_filled_multi_color(const glm::vec2& a, const glm::vec2& c, unsigned int col_upr_left, unsigned int col_upr_right, unsigned int col_bot_right, unsigned int col_bot_left)
		{
			if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) & HZ_COL32_A_MASK) == 0)
				return;

			const glm::vec2 uv = FontTexUvWhitePixel;
			PrimReserve(6, 4);
			PrimWriteIdx((DrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((DrawIdx)(_VtxCurrentIdx + 1)); PrimWriteIdx((DrawIdx)(_VtxCurrentIdx + 2));
			PrimWriteIdx((DrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((DrawIdx)(_VtxCurrentIdx + 2)); PrimWriteIdx((DrawIdx)(_VtxCurrentIdx + 3));
			PrimWriteVtx(a, uv, col_upr_left);
			PrimWriteVtx(glm::vec2(c.x, a.y), uv, col_upr_right);
			PrimWriteVtx(c, uv, col_bot_right);
			PrimWriteVtx(glm::vec2(a.x, c.y), uv, col_bot_left);
		}
		static float sdroundrect(glm::vec2 pt, glm::vec2 ext, float rad) {
			glm::vec2 ext2 = ext - glm::vec2(rad, rad);
			glm::vec2 d = abs(pt) - ext2;
			return std::min(std::max(d.x, d.y), 0.0f) + length(std::max(d, { 0,0 })) - rad;
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
		// center和inner二选一
		// glm::ivec3 center = { 270,280,0x800000ff };
		// glm::ivec3 inner = { 10,10,0x50800000 };
		void draw_rect_filled_gradient(const glm::vec4& rect, const glm::ivec3& center,
			const glm::ivec3* inner_ = 0,/* x=%,y=%,z = col*/
			float rounding = 0.0f, unsigned int col_outer = 0
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
			const glm::vec2 uv = FontTexUvWhitePixel;
			glm::vec2 origin = {};
			glm::vec2 a = { rect.x,rect.y }, s = { rect.z,rect.w };
			glm::vec2 c = { s.x + a.x, s.y + a.y };
			glm::vec2 o1 = { (0.01f * (center.x + s.x)) + a.x, (0.01f * (center.y + s.y)) + a.y };
			glm::vec2 o = { center.x ,center.y };
			if (!inner_)
			{
				int idx_count = 12;
				int vtx_count = 5;

				PrimReserve(idx_count, vtx_count);
				DrawIdx idx = _VtxCurrentIdx;
				//顶点
				PrimWriteVtx(a, uv, col_outer);
				PrimWriteVtx(glm::vec2(c.x, a.y), uv, col_outer);
				PrimWriteVtx(c, uv, col_outer);
				PrimWriteVtx(glm::vec2(a.x, c.y), uv, col_outer);
				PrimWriteVtx(o, uv, center.z);
				std::vector<DrawIdx> idxv;
				get_point_path_index(4, idxv, idx + 0, idx + 4);
				int idxs[][3] = { { 0,1,4 },{ 0,4,3 },{ 4,1,2 },{ 4,2,3 } ,{ -1,-1,-1 } };
				//复制索引到绘图缓冲
				PrimWriteIdxs(idxv.data(), idxv.size());
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
				std::vector<std::vector<glm::vec2>>  path;
				std::vector<unsigned int> ptcs;
				{
					auto it = *inner_;
					float width = it.x, height = it.y;
					float inroun = rounding * 3;
					ptcs.push_back(col_outer);
					PathRect(a, c, inroun);
					path.push_back(_points);
					//ConvexPolyFilled(_points.data(), _points.size(), center.z);
					_points.clear();

					PathRect({ a.x + width, a.y + width }, { c.x - height, c.y - height }, rounding);
					path.push_back(_points);
					//PathFillConvex(center.z);
					//draw_convex_poly_filled(path.data(), path.size(), center.z, true);
					ptcs.push_back(it.z);
					//ConvexPolyFilled(_points.data(), _points.size(), it.z);
				}

				{
					DrawIdx idx = _VtxCurrentIdx;
					std::vector<DrawIdx> idxv;
					get_path2_index(_points.size(), idxv, idx, idx + _points.size());
					int idx_count = idxv.size();
					int vtx_count = _points.size() * 2;
					PrimReserve(idx_count, vtx_count);
					PrimWriteIdxs(idxv.data(), idxv.size());
					unsigned int* ptc = ptcs.data();
					for (auto pt : path)
					{
						for (auto it : pt)
						{
							PrimWriteVtx(it, uv, *ptc);
						}
						ptc++;
					}
				}
				_points.clear();
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
		void draw_radial_gradient(const glm::ivec3& point, const glm::ivec2& center, std::vector<glm::ivec4>* shape,
			int num_segments, glm::vec4* rect_scale = nullptr, glm::vec4* clip = nullptr, unsigned int tcol = 0)
		{
			if (!shape || shape->empty())
			{
				return;
			}
			bool is_clip = clip && clip->z * clip->w;
			if (is_clip)
			{
				PushClipRect(*clip);
			}
			const glm::vec2 uv = FontTexUvWhitePixel;
			// 点+圆
			std::vector<glm::vec2> path; std::vector<unsigned int> ptcs;
			ptcs.push_back(point.z);
			path.push_back(point);
			std::vector<size_t> soffset;
			double np[] = { 0.0, 1.0 / num_segments / 360 };
			glm::vec2 range = { 0,-1 };
			for (auto& r : *shape)
			{
				auto s = path_ellipse_circle(center, r, num_segments, &path, 0.0, range, 0, true, rect_scale);
				soffset.push_back(s);
				ptcs.push_back(r.z);
			}
			DrawIdx idx = _VtxCurrentIdx;
			std::vector<DrawIdx> idxv, path2, testidx;
			std::vector<std::vector<DrawIdx>> a;

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
			PrimReserve(pidx, vtx_count);
			PrimWriteIdxs(idxv.data(), idxv.size());
			for (auto& it : a)
			{
				PrimWriteIdxs(it.data(), it.size());
			}
			unsigned int* ptc = ptcs.data();
			for (int i = 0; i < vtx_count; i++)
			{
				auto it = path[i];
				PrimWriteVtx(it, uv, *ptc);
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
				PopClipRect();
			}
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
		void draw_linear_gradient(glm::ivec4 rect, std::vector<glm::ivec2>* pstops, glm::vec2 dire = { 1, 0 })
		{
			const glm::vec2 uv = FontTexUvWhitePixel;
			glm::vec2 s = { rect.z, rect.w };
			glm::vec2 a = { rect.x, rect.y }, c = a + s;
			auto& stops = *pstops;
			std::vector<unsigned int> cols;
			std::vector<std::vector<glm::vec2>> path;
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
			DrawIdx idx = _VtxCurrentIdx;
			std::vector<DrawIdx> path2;
			int ps = path[0].size();
			get_path2_index(ps, path2, idx, idx + ps, false);
			PrimReserve(path2.size(), ps * 2);
			PrimWriteIdxs(path2.data(), path2.size());
			for (auto& pt : path)
			{
				int i = 0;
				for (auto it : pt) PrimWriteVtx(it, uv, cols[i++]);
			}
			return;
		}
		// 获取两条相同顶点数路径组成的三角形索引
		static size_t get_path2_index(int points_count, std::vector<DrawIdx>& out, int offset, int offset1, bool is_close = true)
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
					ptr[0] = (DrawIdx)(i + offset);
					ptr[1] = (DrawIdx)(i + offset + 1);
					ptr[2] = (DrawIdx)(i + offset + points_count + 1);
					ptr += 3;
				}
				{
					ptr[0] = (DrawIdx)(i + offset);
					ptr[1] = (DrawIdx)(i + offset + points_count + 1);
					ptr[2] = (DrawIdx)(i + offset + points_count);
					ptr += 3;
				}
			}
#else
			for (int i = 0; i < points_count; i++)
			{
				{
					ptr[0] = (DrawIdx)(i + offset);
					ptr[1] = (DrawIdx)(i + offset + 1);
					ptr[2] = (DrawIdx)(i + offset1 + 1);
					ptr += 3;
				}
				{
					ptr[0] = (DrawIdx)(i + offset);
					ptr[1] = (DrawIdx)(i + offset1 + 1);
					ptr[2] = (DrawIdx)(i + offset1);
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
			//	ptr[0] = (DrawIdx)(0);
			//	ptr[1] = (DrawIdx)(0 + i - 1);
			//	ptr[2] = (DrawIdx)(0 + i);
			//	ptr += 3;
			//}
		}
		/*
		获取点/路径组成的索引
		{点在0索引，所以只需输入路径顶点数量}
		偏移offset=1
		基点base_point=0

		*/
		static size_t get_point_path_index(int points_count, std::vector<DrawIdx>& out, int offset = 1, int base_point = 0)
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
				ptr[1] = (DrawIdx)(i + offset);
				ptr[2] = (DrawIdx)(i + offset + 1);
				ptr += 3;
			}
			out[ret - 1] = offset;
			return ret;
		}

		void draw_poly_gradient(glm::ivec3 center, double radius, unsigned int c, int n = 2, unsigned int bc = 0, int nums = 360)
		{
			std::vector<glm::ivec4> shape;
			{
				double r = radius;
				if (n == 2)
				{
					shape.push_back({ r * 0.5 , r * 0.5,0, nums });
				}
				//shape.push_back({ r * 0.75 , r * 0.75, set_alpha_f(c, 0.25), nums });
				shape.push_back({ r , r, c, nums });
				shape.push_back({ r + 1.0 , r + 1.0,set_alpha_f(c, 0), nums });
			}
			draw_radial_gradient(center, center, &shape, nums, nullptr, nullptr, bc);
		}

		void ConvexPolyFilled(const glm::vec2* points, const int points_count, unsigned int col)
		{
			const glm::vec2 uv = FontTexUvWhitePixel;
			{
				// Non Anti-aliased Fill
				const int idx_count = (points_count - 2) * 3;
				const int vtx_count = points_count;
				PrimReserve(idx_count, vtx_count);
				for (int i = 0; i < vtx_count; i++)
				{
					setVec(_VtxWritePtr[0].pos, points[i]);
					_VtxWritePtr[0].uv = uv;
					_VtxWritePtr[0].col = col;
					_VtxWritePtr++;
				}
				for (int i = 2; i < points_count; i++)
				{
					_IdxWritePtr[0] = (DrawIdx)(_VtxCurrentIdx);
					_IdxWritePtr[1] = (DrawIdx)(_VtxCurrentIdx + i - 1);
					_IdxWritePtr[2] = (DrawIdx)(_VtxCurrentIdx + i);
					_IdxWritePtr += 3;
				}
				_VtxCurrentIdx += (DrawIdx)vtx_count;
			}
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
		void draw_quad(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, unsigned int col, unsigned int fill, float thickness = 1.0f)
		{
			PathLineTo(a);
			PathLineTo(b);
			PathLineTo(c);
			PathLineTo(d);
			if ((fill & HZ_COL32_A_MASK))
				PathFillConvex(fill, false);
			if ((col & HZ_COL32_A_MASK))
				PathStroke(col, true, thickness);
			_points.clear();
		}

		void draw_triangle(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, unsigned int col, unsigned int fill, float thickness = 1.0f, bool aa = true)
		{
			PathLineTo(a);
			PathLineTo(b);
			PathLineTo(c);
			if ((fill & HZ_COL32_A_MASK))
				PathFillConvex(fill, false, aa);
			if ((col & HZ_COL32_A_MASK))
				PathStroke(col, true, thickness, aa);
			_points.clear();
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

		void draw_circle(const glm::vec2& centre, float radius, unsigned int col, unsigned int fill, int num_segments, float thickness = 1.0f)
		{
			//const float a_max = _PI * 2.0f * ((float)num_segments - 1.0f) / (float)num_segments;

			if ((fill & HZ_COL32_A_MASK))
			{
				PathArcTo(centre, radius - thickness * 0.5f, num_segments, false);
				PathFillConvex(fill);
			}
			if ((col & HZ_COL32_A_MASK))
			{
				PathArcTo(centre, radius - 0.5f, num_segments, false);
				PathStroke(col, true, thickness);
			}
		}
		void draw_ellipse(const glm::vec2& centre, float rx, float ry, unsigned int col, unsigned int fill, int num_segments, float thickness = 1.0f)
		{
			//const float a_max = _PI * 2.0f * ((float)num_segments - 1.0f) / (float)num_segments;

			if ((fill & HZ_COL32_A_MASK))
			{
				PathArcTo(centre, { rx - thickness * 0.5f, ry - thickness * 0.5f }, num_segments, false);
				PathFillConvex(fill);
			}
			if ((col & HZ_COL32_A_MASK))
			{
				PathArcTo(centre, { rx - 0.5f, ry - 0.5f }, num_segments, false);
				PathStroke(col, true, thickness);
			}
		}
		void draw_arc(const glm::vec2& centre, float rx, float ry, unsigned int col, unsigned int fill, int num_start, int num_end, int num_segments, float thickness = 1.0f)
		{
			//const float a_max = _PI * 2.0f * ((float)num_segments - 1.0f) / (float)num_segments;

			if ((fill & HZ_COL32_A_MASK))
			{
				PathArcTo(centre, { rx - thickness * 0.5f, ry - thickness * 0.5f }, num_start, num_end, num_segments);
				PathFillConvex(fill);
			}
			if ((col & HZ_COL32_A_MASK))
			{
				PathArcTo(centre, { rx - 0.5f, ry - 0.5f }, 0.0f, num_end, num_segments);
				PathStroke(col, true, thickness);
			}
		}
		void draw_larme(const glm::vec2& centre, float radius, int n, unsigned int col, unsigned int fill, int num_segments, float thickness = 1.0f)
		{
			const float a_max = _PI * 2.0f * ((float)num_segments - 1.0f) / (float)num_segments;

			if ((fill & HZ_COL32_A_MASK))
			{
				PathLarmeTo(centre, radius - thickness * 0.5f, 0.0f, a_max, num_segments, n);
				PathFillConvex(fill);
			}
			if ((col & HZ_COL32_A_MASK))
			{
				PathLarmeTo(centre, radius - 0.5f, 0.0f, a_max, num_segments, n);
				PathStroke(col, true, thickness);
			}
		}

		void draw_bezier_curve(const glm::vec2& pos0, const glm::vec2& cp0, const glm::vec2& cp1, const glm::vec2& pos1, unsigned int col, float thickness, int num_segments
			, glm::vec4* idx_pos = nullptr, std::vector<glm::vec2>* outpath = nullptr
		)
		{
			if ((col & HZ_COL32_A_MASK) == 0)
				return;
			PathLineTo(pos0);
			PathBezierCurveTo(cp0, cp1, pos1, num_segments);
			path_merge();
			get_path_idxf(idx_pos, &_points);
			if (outpath)
			{
				*outpath = _points;
			}
			PathStroke(col, false, thickness);
		}
		// 三阶贝塞尔曲线 1段线2个控制点,point节点，ctrl控制点，col颜色，thickness线宽，num_segments段数
		void draw_bezier_curve_multi(const std::vector<glm::vec2>& point, const std::vector<glm::vec2>& ctrl, unsigned int col, float thickness, int num_segments = 0)
		{
			int64_t n = std::min(point.size() - 1, ctrl.size() / 2);
			if ((col & HZ_COL32_A_MASK) == 0 || n < 1)
				return;
			PathLineTo(point[0]);
			auto ct = ctrl.data();
			auto pt = point.data();
			pt++;
			for (size_t i = 0; i < n; i++, pt++, ct += 2)
			{
				PathBezierCurveTo(ct[0], ct[1], *pt, num_segments);
			}
			// mergePath();
			PathStroke(col, false, thickness);
		}
		void sol1(const std::vector<glm::vec2>& path, unsigned int col)  //绘制控制多边形的轮廓//n控制点的个数
		{
			if ((col & HZ_COL32_A_MASK) == 0)
				return;
			auto n = path.size();
			for (size_t i = 0; i < n - 1; i += 2)
				draw_line(path[i], path[i + 1], col);
		}
		double sol2(int nn, int k)  //计算多项式的系数C(nn,k)
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
		void sol3(double t, glm::vec2* path, size_t count, size_t n)  //计算Bezier曲线上点的坐标
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
		void sol4(glm::vec2* path, double m, size_t n, bool first)  //根据控制点，求曲线上的m个点
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
		void add_bezier_multi(const std::vector<glm::vec2>& path, unsigned int col, float thickness, int num_segments, int cp = 0, unsigned int control_col = 0)
		{
			sol1(path, control_col);
			glm::vec2* p = (glm::vec2*) path.data();
			if (cp < 1)
			{
				cp = path.size();
			}
			size_t c = path.size() / cp;

			for (size_t i = 0; i < c; i++, p += cp)
			{
				sol4(p, num_segments, cp, i == 0);
			}
			// mergePath();
			PathStroke(col, false, thickness);
		}
		// todo 多阶贝塞尔曲线未完成
		void add_bezier_multi(const std::vector<glm::vec2>& point, const std::vector<glm::vec2>& ctrl,
			unsigned int col, float thickness, int num_segments, unsigned int control_col = 0, int ctrl_count = 2)
		{
			int64_t n = std::min(point.size() - 1, ctrl.size() / ctrl_count);
			if ((col & HZ_COL32_A_MASK) == 0 || n < 1)
				return;
			auto ct = ctrl.data();
			auto pt = point.data();
			std::vector<glm::vec2> temp;
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
			std::vector<glm::vec2> path;
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
					sol3((double)i / (double)num_segments, path.data(), path.size(), ctrl_count + 2);
				}
				path.clear();
			}
			// mergePath();
			PathStroke(col, false, thickness);
		}
		//void drawtext(const ImFont* font, float font_size, const glm::vec2& pos, unsigned int col, const char* text_begin, const char* text_end, float wrap_width, const glm::vec4* cpu_fine_clip_rect)
		//{
		//	if ((col & HZ_COL32_A_MASK) == 0)
		//		return;

		//	if (text_end == NULL)
		//		text_end = text_begin + strlen(text_begin);
		//	if (text_begin == text_end)
		//		return;

		//	// Note: This is one of the few instance of breaking the encapsulation of ImDrawList, as we pull this from ImGui state, but it is just SO useful.
		//	// Might just move Font/FontSize to ImDrawList?
		//	if (font == NULL)
		//		font = GImGui->Font;
		//	if (font_size == 0.0f)
		//		font_size = GImGui->FontSize;

		//	IM_ASSERT(font->ContainerAtlas->TexID == _ImageStack.back());  // Use high-level ImGui::PushFont() or low-level PushImage() to change font.

		//	glm::vec4 clip_rect = _ClipRectStack.back();
		//	if (cpu_fine_clip_rect)
		//	{
		//		clip_rect.x = std::max(clip_rect.x, cpu_fine_clip_rect->x);
		//		clip_rect.y = std::max(clip_rect.y, cpu_fine_clip_rect->y);
		//		clip_rect.z = std::min(clip_rect.z, cpu_fine_clip_rect->z);
		//		clip_rect.w = std::min(clip_rect.w, cpu_fine_clip_rect->w);
		//	}
		//	font->RenderText(this, font_size, pos, col, clip_rect, text_begin, text_end, wrap_width, cpu_fine_clip_rect != NULL);
		//}

		//void drawtext(const glm::vec2& pos, unsigned int col, const char* text_begin, const char* text_end)
		//{
		//	draw_text(GImGui->Font, GImGui->FontSize, pos, col, text_begin, text_end);
		//}

		//void draw_text(const std::wstring& text, const glm::vec2& pos, unsigned int col, float wrap_width,
		//	const glm::vec4* cpu_fine_clip_rectconst, const std::wstring& font, float font_size, float font_dpi = 96
		///*glm::vec2 *outsize = 0*/)
		//{
		//	font_att fa;
		//	fa.setFName(font);
		//	fa.wrap_width = wrap_width;
		//	fa.set_size(font_size, font_dpi);
		//	draw_text(text, pos, col, cpu_fine_clip_rectconst, &fa/*, outsize*/);
		//}

		// todo		文本渲染
		glm::ivec2 draw_text(Fonts::css_text* csst, glm::ivec2 pos, const std::string& str
			, size_t count = -1, size_t first_idx = 0, draw_font_info* out = nullptr)
		{
			auto ft = Fonts::s();
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
			if (csst->text_decoration & Fonts::css_text::underline)
			{
				//下划线
				tns = csst->get_row_lt(Fonts::css_text::underline);
				oly += tns;
				//oly = -csst->underline_position + 0.5 * tns;
				auto col = csst->color;
				draw_line({ pos.x, pos.y + oly }, { out->awidth, pos.y + oly }, col, tns, false);
			}
			if (csst->text_decoration & Fonts::css_text::overline)
			{
				//上划线
				tns = csst->get_row_lt(Fonts::css_text::overline);
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
			if (csst->text_decoration & Fonts::css_text::line_through)
			{
				//贯穿线
				tns = csst->line_thickness;
				oly = bs + base_line * 0.68;
				auto col = csst->color;
				draw_line({ pos.x, pos.y + oly }, { out->awidth, pos.y + oly }, col, tns, false);
			}
			return ret;
		}
#if 1
		// todo 渲染到图像
		typedef struct draw_text_info_t
		{
			Fonts::css_text* csst;
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
			if (csst->text_decoration & Fonts::css_text::underline)
			{
				//下划线
				tns = p->csst->get_row_lt(Fonts::css_text::underline);
				oly += tns;
				//oly = -csst->underline_position + 0.5 * tns;
				auto col = p->csst->color;
				draw_line({ pos.x, pos.y + oly }, { out->awidth, pos.y + oly }, col, tns, false);
			}
			if (csst->text_decoration & Fonts::css_text::overline)
			{
				//上划线
				tns = csst->get_row_lt(Fonts::css_text::overline);
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
			if (csst->text_decoration & Fonts::css_text::line_through)
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

#if 0
		void draw_text(const std::wstring& text, const glm::vec2& pos, unsigned int col,
			const glm::vec4* cpu_fine_clip_rectconst, font_att* fa,/*float wrap_width,const std::wstring &font, float font_size, int line_height, float font_dpi = 96,*/
			/*glm::vec2 *outsize = 0,*/ const glm::ivec4& scope = { 0, 0, 0,0 },
			std::function<void(std::vector<glm::vec4>& hotarea, glm::vec2 outsize)> ha_func = nullptr,
			std::vector<glm::ivec3>* cols = nullptr
		)
		{
#if 0
			std::map<wchar_t, FontCache::FCInfo> vfnstr;
			std::vector<glm::vec4> hotarea;

			Font::BuildText(text, fa/*font, font_size, font_dpi, line_height*/, &vfnstr);
			if (vfnstr.empty())
			{
				return;
				//throw text.c_str();
			}
			// 			int x = 0, y = 0;
			// 			for (auto it : vfnstr)
			// 			{
			// 				x += it.rect.z;
			// 				y = std::max(y, it.rect.w);
			// 			}
						//if (outsize)
						//{
						//	outsize->x = x;
						//	outsize->y = y;
						//}
			int xd = 0, ss = 0;
			njson temns;
			if (ss && vfnstr.size())
			{
				auto vi = vfnstr.begin()->second;
				vi.image->saveImage("d:\\fts.png");
			}
			glm::vec2 ps = { pos.x /*+ wrap_width*/ + xd,pos.y + fa->wrap_width };
			glm::ivec2 minpos, maxsize = { MAXINT,MAXINT };
			if (cpu_fine_clip_rectconst)
			{
				minpos = { cpu_fine_clip_rectconst->x ,cpu_fine_clip_rectconst->y };
				maxsize = { cpu_fine_clip_rectconst->z , cpu_fine_clip_rectconst->w };
			}
			//if (ps.x > minpos.x && ps.y > minpos.y)
			{
				glm::ivec4 a = { ps.x,ps.y, 0, fa->line_height };
				auto bcol = fa->back_color;

				size_t i = 0, scx = scope.x, scy = scope.y;
				bool is_back = scope.x >= 0 && scx != scy;
				//for (auto it : vfnstr)
				auto tls = text.size() - scope.z;
				auto txtcol = col;
				glm::ivec3* ct = nullptr;
				if (cols)
				{
					ct = cols->data();
				}
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
					fw += fa->wrap_width;
					if (tc == L' ' && fa->getFname() == L"微软雅黑")
					{
						i = i;
					}
					if (tc == L'\n')
					{
						auto ac = a;
						ac.x = ps.x + xd;
						ac.y = ps.y;
						ac.z = fw;
						if (icb)
						{
							// 渲染选中背景
							draw_rect(ac, 0, bcol);
							hotarea.push_back(ac);
						}

						xd = 0;
						ps.y += fa->line_height + fa->wrap_width;
						maxsize.y = pos.y + ps.y;
						continue;
					}
					if (maxsize.x > ps.x + xd /*&& maxsize.y > ps.y*/)
					{
						auto ac = a;
						glm::ivec4 txtpos = { ps.x + xd,ps.y , it.rect.z,it.rect.w };
						glm::ivec2 isize = { it.image->width, it.image->height };
						glm::ivec4 rc = it.rect;
						ac.x = txtpos.x;
						ac.y = txtpos.y;
						ac.z = fw;
						if (icb)
						{
							draw_rect(ac, 0, bcol);
							hotarea.push_back(ac);
						}
						if (incs > 0)
						{
							if (incs == ct->y)
							{
								ct++; incs = 0; txtcol = col;
							}
							else
							{
								incs++;
							}
						}
						if (ct && i == ct->x)
						{
							txtcol = ct->z; incs = 1;
						}
						draw_image(it.image, txtpos, rc, { 0,0,0,0 }, txtcol);
						if (ss == 1)
						{
							//printf("%c,%d,%d", text[i - 1], it.rect.z, it.rect.w);
							wchar_t tt[] = { tc,0 };
							njson an;
							an["a"] = jsont::wstring_to_ansi(tt);
							an["width"] = it.rect.z;
							an["height"] = it.rect.w;
							//temns.push_back(an);
						}
					}
					xd += fw;
				}
				if (ha_func)
				{
					ps.x -= pos.x;
					ps.y -= pos.y;
					ha_func(hotarea, ps);
				}
			}
			if (ss)
			{
				//auto st = temns.dump(2);
				//File::save_binary_file("C:\\Users\\huazai\\Desktop\\zimg\\" + jsont::wstring_to_ansi(text) + ".json", st.data(), st.size());
			}
#endif
		}
		void draw_text(const std::string& text, const glm::vec2& pos, unsigned int col, const glm::vec4* cpu_fine_clip_rectconst, font_att* fa, const glm::ivec4& scope = { 0, 0, 0, 0 })
		{
			draw_text(AtoW(text), pos, col, cpu_fine_clip_rectconst, fa,/* outsize,*/ scope);
		}
#endif
	public:
		static std::wstring AtoW(const std::string& str)
		{
			if (hz::hstring::IsTextUTF8(str.c_str()))
			{
				return u8_u16(str);
			}
			else
				return gbk_u16(str);
		}
		// todo 把文本输出到Image*
#if 0
		static glm::ivec2 writeText(Image* dst, const std::string& str, const glm::vec2& pos, unsigned int col,
			font_att* fa)
		{
			int x = 0, y = 0;
			std::wstring text = AtoW(str);
			writeText(dst, text, pos, col, fa);
			return { 0,0 };
#if 0
			std::vector<FontCache::FCInfo> vfnstr;
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
			std::vector<glm::vec4> hotarea;
			Font::BuildText(text, fa, &vfnstr);

			int xd = 0, ss = 0;
			njson temns;
			if (ss && vfnstr.size())
			{
				auto vi = vfnstr.begin()->second;
				vi.image->saveImage("d:\\fts.png");
			}
			glm::vec2 ps = { pos.x /*+ wrap_width*/ + xd,pos.y + fa->wrap_width };
			glm::ivec2 minpos = { 0,0 }, maxsize = { dst->width , dst->height };

			{
				glm::ivec4 a = { ps.x,ps.y, 0, fa->line_height };

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
						glm::ivec4 txtpos = { ps.x + xd,ps.y , it.rect.z,it.rect.w };
						dst->copyImage(it.image, txtpos, it.rect, col);
					}
					xd += fw;
				}
			}
#endif
		}
#endif
	private:
		int AddImage_b(Image* user_image, const glm::vec2& a, const glm::vec2& b, const glm::vec2& uv_a, const glm::vec2& uv_b, unsigned int col = -1)
		{
			//if ((col & HZ_COL32_A_MASK) == 0)
			//	return;

			// FIXME-OPT: This is wasting draw calls.
			const bool push_image = _ImageStack.empty() || user_image != _ImageStack.back();
			if (push_image)
				PushImage(user_image);
			int ret = 0;
			PrimReserve(6, 4, &ret);
			PrimRectUV(a, b, uv_a, uv_b, col);

			if (push_image)
				PopImage();
			return ret;
		}
		int AddImage_a(Image* user_image, const glm::ivec4& a, const glm::ivec2& texsize, const glm::ivec4& rect = glm::ivec4{ -1 }, unsigned int col = -1)
		{
			glm::ivec2 pos = { a.x,a.y }, size = { a.z,a.w };
			glm::vec4 v4 = { 0,0,1,1 };
			glm::vec4 uv = v4;
			glm::vec2 s = size;
			if (!(rect.x < 0))
			{
				v4 = rect;
				v4.z += v4.x; v4.w += v4.y;//加上原点坐标
				uv = {
					v4.x / texsize.x, v4.y / texsize.y, v4.z / texsize.x, v4.w / texsize.y,
				};
				// 				s.x = rect.z;
				// 				s.y = rect.w;
			}
			return AddImage_b(user_image, pos, { pos.x + s.x,pos.y + s.y }, { uv.x,uv.y }, { uv.z,uv.w }, col);
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

		*/
		int AddImageSliced(Image* user_image, const glm::ivec4& a, const glm::ivec2& texsize, const glm::ivec4& sliced = glm::ivec4{ 10.0f,10.0f,10.0f,10.0f }, const glm::ivec4& rect = glm::ivec4{ -1 }, unsigned int col = -1)
		{
			static std::vector<short> vt_index =// { 0,8,12,4,14,6,2,10,11,6,7,4,5,8,9,1,5,13,7,15,11,3 };//E_TRIANGLE_STRIP
			{ 0,8,12, 8,12,4, 12,4,14, 4,14,6, 14,6,2, 6,2,10,
			  6,7,10, 7,10,11, 4,5,6, 5,6,7, 8,9,4, 9,4,5,
			  9,1,5, 1,5,13, 5,13,7, 13,7,15, 7,15,11, 15,11,3 };//E_TRIANGLE_LIST

			glm::ivec2 pos = { a.x,a.y }, size = { a.z,a.w };
			glm::vec4 uv = { 0,0,1,1 };
			glm::vec4 v4 = { 0,0, texsize.x, texsize.y };
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

			std::vector<DrawVert> vertex = {
#if 1
				//0				
				{ { x, y ,z},{ uv.x, uv.y }, col },
				//1
				{ { x, y + height ,z },{ uv.x, uv.w }, col },
				//2
				{ { x + width, y  ,z },{uv.z, uv.y } , col },
				//3
				{ { x + width, y + height ,z },{uv.z, uv.w } , col },
				//4
				{ { x + left, y + top ,z },{suv.x, suv.y } , col },
				//5
				{ { x + left, y + height - bottom  ,z },{suv.x, suv.w }, col },
				//6 
				{ { x + width - right, y + top ,z },{ suv.z, suv.y} , col },
				//7
				{ { x + width - right, y + height - bottom  ,z },{ suv.z, suv.w } , col },
				//8
				{ { x, y + top  ,z },{ uv.x, suv.y } , col },
				//9
				{ { x, y + height - bottom  ,z },{ uv.x, suv.w } , col },
				//10
				{ { x + width, y + top ,z },{uv.z, suv.y }, col },
				//11
				{ { x + width, y + height - bottom  ,z },{uv.z, suv.w } , col },
				//12
				{ { x + left, y  ,z },{suv.x, uv.y } , col },
				//13
				{ { x + left, y + height ,z },{suv.x, uv.w } , col },
				//14
				{ { x + width - right, y  ,z },{ suv.z, uv.y } , col },
				//15
				{ { x + width - right, y + height  ,z },{ suv.z, uv.w } , col },
#else
				//0				
				{{ x, y}, { 0.0f, 0.f}, col},
				//1
				{ {x, y + height}, { 0.f, 1.0f }, col},
				//2
				{ {x + width, y},  { 1.0f, 0.f} , col},
				//3
				{{ x + width, y + height},  { 1.0f, 1.0f} , col},
				//4
				{{ x + left, y + top},  {0.0f + suv.x, 0.f + suv.y} , col},
				//5
				{ {x + left, y + height - bottom}, { 0.f + suv.x, 1.0f - suv.w }, col},
				//6 
				{ {x + width - right, y + top},  {1.0f - suv.z, 0.f + suv.z} , col},
				//7
				{ {x + width - right, y + height - bottom},  {1.0f - suv.z, 1.0f - suv.w} , col},
				//8
				{{ x, y + top},  {0.0f, 0.f + suv.y} , col},
				//9
				{ {x, y + height - bottom},  {0.f, 1.0f - suv.w} , col},
				//10
				{ {x + width, y + top}, { 1.0f, 0.f + suv.y }, col},
				//11
				{ {x + width, y + height - bottom}, { 1.0f, 1.0f - suv.w} , col},
				//12
				{ {x + left, y},  {0.0f + suv.x, 0.f} , col},
				//13
				{ {x + left, y + height},  {0.f + suv.x, 1.0f} , col},
				//14
				{ {x + width - right, y},  {1.0f - suv.z, 0.f} , col},
				//15
				{ {x + width - right, y + height}, { 1.0f - suv.z, 1.0f} , col},
#endif
			};
			int ret = 0;

			const bool push_image = _ImageStack.empty() || user_image != _ImageStack.back();
			if (push_image)
				PushImage(user_image);
			{
				PrimReserve(vt_index.size(), vertex.size(), &ret);
				DrawIdx idx = (DrawIdx)_VtxCurrentIdx;
				int i = 0;
				for (auto& it : vt_index)
				{
					_IdxWritePtr[i++] = it + idx;
				}
				i = 0;
				for (auto& it : vertex)
				{
					_VtxWritePtr[i++] = it;
				}
				_VtxWritePtr += vertex.size();
				_VtxCurrentIdx += vertex.size();
				_IdxWritePtr += vt_index.size();
			}
			if (push_image)
				PopImage();
			return ret;
		}
	public:

		// 普通图片显示 {支持显示部分区域、九宫格显示}
		void draw_image(draw_image_info* info)
		{
			if (!info || !info->user_image)
			{
				return;
			}
			glm::vec2 texsize = { info->user_image->width , info->user_image->height };
			auto rect = info->rect;
			auto a = info->a;
			auto sliced = info->sliced;
			if (rect.z < 1)
			{
				rect.x = rect.y = 0;
				rect.z = a.z;
			}
			if (rect.w < 1)
			{
				rect.w = a.w;
			}
			int ret = -1;
			if (sliced.x < 1 || sliced.y < 1 || sliced.z < 1 || sliced.w < 1)
			{
				ret = AddImage_a(info->user_image, a, texsize, rect, info->col);
			}
			else
			{
				ret = AddImageSliced(info->user_image, a, texsize, sliced, rect, info->col);
			}
			if (info->out)
				*(info->out) = ret;
		}
		void draw_image(Image* user_image, const glm::vec4& a,
			glm::vec4 rect = glm::vec4{ -1 }, const glm::vec4& sliced = glm::vec4{ -1 }, unsigned int col = -1, int* out = nullptr)
		{
			glm::vec2 texsize = { user_image->width , user_image->height };

			if (rect.z < 1)
			{
				rect.x = rect.y = 0;
				rect.z = a.z;
			}
			if (rect.w < 1)
			{
				rect.w = a.w;
			}
			int ret = -1;
			if (sliced.x < 1 || sliced.y < 1 || sliced.z < 1 || sliced.w < 1)
			{
				ret = AddImage_a(user_image, a, texsize, rect, col);
			}
			else
			{
				ret = AddImageSliced(user_image, a, texsize, sliced, rect, col);
			}
			if (out)
				*out = ret;
		}
		// gif专用
		void draw_image_gif(Image* user_image, glm::vec2 pos, int idx = -1)
		{
			glm::vec4 rct = user_image->get_idx_rect(idx);
			draw_image(user_image, { pos, rct.z, rct.w }, rct);
		}
		// 可用于拉伸显示
		void draw_image_quad(Image* user_image, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, const glm::vec2& uv_a, const glm::vec2& uv_b, const glm::vec2& uv_c, const glm::vec2& uv_d, unsigned int col = -1, int* out = nullptr)
		{
			if ((col & HZ_COL32_A_MASK) == 0)
				return;

			const bool push_image = _ImageStack.empty() || user_image != _ImageStack.back();
			if (push_image)
				PushImage(user_image);
			int ret = 0;
			PrimReserve(6, 4, &ret);
			PrimQuadUV(a, b, c, d, uv_a, uv_b, uv_c, uv_d, col);

			if (push_image)
				PopImage();
			if (out)
				*out = ret;
			return;
		}

		// 渲染
		void draw_image_uv(Image* user_image, glm::vec2 pos, glm::vec4 uv_top = { 0,0,0,1 }, glm::vec4 uv_bottom = { 1,1,1,0 }, bool isswap = true)
		{
			if (!user_image)return;
			glm::vec2 rc = { user_image->width, user_image->height };
			if (isswap)
			{
				std::swap(rc.x, rc.y);
			}
			glm::vec2 a[4] = { {pos.x, pos.y},{pos.x + rc.x,pos.y},{pos.x + rc.x,pos.y + rc.y},{pos.x,pos.y + rc.y}, }
			, uv[4];// = { {0,0},{0,1},{1,1},{1,0} };
			uv[0] = glm::vec2(uv_top);
			uv[1] = glm::vec2(uv_top.z, uv_top.w);
			uv[2] = glm::vec2(uv_bottom);
			uv[3] = glm::vec2(uv_bottom.z, uv_bottom.w);
			draw_image_quad(user_image, a[0], a[1], a[2], a[3], uv[0], uv[1], uv[2], uv[3]);
		}
#endif
		/*!add end*/
#ifndef _NO_SETIMAGE
		int setImage(int id, void* user_image, const glm::vec2& a, const glm::vec2& b, const glm::vec2& uv_a, const glm::vec2& uv_b, unsigned int col = -1)
		{
			int ret = 0;
			setRectUV(id, a, b, uv_a, uv_b, col);
			return ret;
		}
		int setImage(int id, void* user_image, const glm::vec2& pos, const glm::vec2& size, const glm::vec4& rect = glm::vec4{ -1 }, unsigned int col = -1)
		{
			glm::vec4 v4 = { 0,0,1,1 };
			glm::vec4 uv = v4;
			glm::vec2 s = size;
			if (!(rect.x < 0))
			{
				v4 = rect;
				v4.z += v4.x; v4.w += v4.y;//加上原点坐标
				uv = { v4.z / size.x, v4.w / size.y,
					v4.x / size.x, v4.y / size.y,
				};
				s.x = rect.z;
				s.y = rect.w;
			}
			return setImage(id, user_image, pos, { pos.x + s.x,pos.y + s.y }, { uv.x,uv.y }, { uv.z,uv.w }, col);
		}
		int setImageQuad(int id, void* user_image, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, const glm::vec2& uv_a, const glm::vec2& uv_b, const glm::vec2& uv_c, const glm::vec2& uv_d, unsigned int col = -1)
		{
			if ((col & HZ_COL32_A_MASK) == 0)
				return -1;

			int ret = 0;
			setQuadUV(id, a, b, c, d, uv_a, uv_b, uv_c, uv_d, col);

			return ret;
		}
		int setImageSliced(int id, void* user_image, const glm::vec2& pos, const glm::vec2& size, const glm::vec2& texsize, const glm::vec4& sliced = glm::vec4{ 10.0f,10.0f,10.0f,10.0f }, const glm::vec4& rect = glm::vec4{ -1 })
		{
			static std::vector<short> vt_index =// { 0,8,12,4,14,6,2,10,11,6,7,4,5,8,9,1,5,13,7,15,11,3 };//E_TRIANGLE_STRIP
			{ 0,8,12, 8,12,4, 12,4,14, 4,14,6, 14,6,2, 6,2,10,
				6,7,10, 7,10,11, 4,5,6, 5,6,7, 8,9,4, 9,4,5,
				9,1,5, 1,5,13, 5,13,7, 13,7,15, 7,15,11, 15,11,3 };//E_TRIANGLE_LIST

			glm::vec4 uv = { 0,0,1,1 };
			glm::vec4 v4 = { 0,0, texsize.x, texsize.y };
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

			unsigned int col = (unsigned int)-1;
			std::vector<DrawVert> vertex = {
				//0				
				{ { x, y  ,z },{ uv.x, uv.y }, col },
				//1
				{ { x, y + height ,z },{ uv.x, uv.w }, col },
				//2
				{ { x + width, y  ,z },{ uv.z, uv.y } , col },
				//3
				{ { x + width, y + height  ,z },{ uv.z, uv.w } , col },
				//4
				{ { x + left, y + top  ,z },{ suv.x, suv.y } , col },
				//5
				{ { x + left, y + height - bottom  ,z },{ suv.x, suv.w }, col },
				//6 
				{ { x + width - right, y + top  ,z },{ suv.z, suv.z } , col },
				//7
				{ { x + width - right, y + height - bottom  ,z },{ suv.z, suv.w } , col },
				//8
				{ { x, y + top ,z },{ uv.x, suv.y } , col },
				//9
				{ { x, y + height - bottom  ,z },{ uv.x, suv.w } , col },
				//10
				{ { x + width, y + top ,z },{ uv.z, suv.y }, col },
				//11
				{ { x + width, y + height - bottom  ,z },{ uv.z, suv.w } , col },
				//12
				{ { x + left, y  ,z },{ suv.x, uv.y } , col },
				//13
				{ { x + left, y + height  ,z },{ suv.x, uv.w } , col },
				//14
				{ { x + width - right, y  ,z },{ suv.z, uv.y } , col },
				//15
				{ { x + width - right, y + height  ,z },{ suv.z, uv.w } , col },
			};
			int ret = 0;
			{
				int i = 0;
				for (auto& it : vertex)
				{
					setVtx(id + i, &it);
					i++;
				}
			}
			return ret;
		}
#endif
#if 1
		int addModel(std::vector<DrawVert>& vertex, std::vector<DrawIdx>& vt_index, Image* user_image)
		{
			int ret = 0;

			const bool push_image = user_image && (_ImageStack.empty() || user_image != _ImageStack.back());
			if (push_image)
				PushImage(user_image);
			{
				PrimReserve(vt_index.size(), vertex.size(), &ret);
				DrawIdx idx = (DrawIdx)_VtxCurrentIdx;
				int i = 0;
				for (auto& it : vt_index)
				{
					_IdxWritePtr[i++] = it + idx;
				}
				i = 0;
				for (auto& it : vertex)
				{
					_VtxWritePtr[i++] = it;
				}
				_VtxWritePtr += vertex.size();
				_VtxCurrentIdx += vertex.size();
				_IdxWritePtr += vt_index.size();
			}
			if (push_image)
				PopImage();
			return ret;
		}

		int addModel(std::vector<float>& vertex, int stride, std::vector<DrawIdx>& vt_index, Image* user_image)
		{
			int ret = 0;

			const bool push_image = user_image && (_ImageStack.empty() || user_image != _ImageStack.back());
			if (push_image)
				PushImage(user_image);
			{
				PrimReserve(vt_index.size(), vertex.size() * 0, &ret);
				DrawIdx idx = (DrawIdx)_VtxCurrentIdx;
				int i = 0;
				for (auto& it : vt_index)
				{
					_IdxWritePtr[i++] = it + idx;
				}
				_stride = stride;
				fVtxBuffer = vertex;
				_VtxCurrentIdx += vertex.size();
				_IdxWritePtr += vt_index.size();
			}
			if (push_image)
				PopImage();
			return ret;
		}
#endif

#ifdef GeoList_R
		void toCanvas(geo::GeoList* geolist)
		{
			if (geolist)
			{
				//LOCK(lck);
				auto p = geolist->getdata();
				for (auto it : p)
				{
					callG(&it, this);
				}
			}
		}
		void callG(geo::s_geo* d, Canvas* g)
		{
#define MFUNC(t) {geo::T_##t,&Canvas::make_##t}
			static std::map<unsigned int, void(Canvas::*)(geo::s_geo* d, Canvas* g)> a_func = {
				MFUNC(polyline),
				MFUNC(line),
				MFUNC(rect),
				MFUNC(quad),
				MFUNC(triangle),
				MFUNC(circle),
				MFUNC(ellipse),
				MFUNC(larme),
				MFUNC(curve),
				MFUNC(image),
				//MFUNC(ImageQuad),
				MFUNC(text),
			};
#undef MFUNC
			auto it = a_func.find(*(unsigned int*)d);
			if (it != a_func.end())
			{
				(this->*(it->second))(d, g);
			}
		}
		void make_polyline(geo::s_geo* d, Canvas* g)
		{
			geo::polyline* p = (geo::polyline*)d;
			g->draw_polyline(p->points->data(), p->points->size(), p->col, p->closed, p->thickness, p->anti_aliased);
		}

		void make_line(geo::s_geo* d, Canvas* g)
		{
			geo::line* p = (geo::line*)d;
			g->draw_line(p->a, p->b, p->col, p->thickness);
		}

		void make_rect(geo::s_geo* d, Canvas* g)
		{
			geo::rect* p = (geo::rect*)d;
			g->draw_rect(p->a, p->col, p->fill, p->rounding, p->rounding_corners_flags, p->thickness);
		}

		void make_quad(geo::s_geo* d, Canvas* g)
		{
			geo::quad* p = (geo::quad*)d;
			g->draw_quad(p->a, p->b, p->c, p->d, p->col, p->fill, p->thickness);
		}

		void make_triangle(geo::s_geo* d, Canvas* g)
		{
			geo::triangle* p = (geo::triangle*)d;
			g->draw_triangle(p->a, p->b, p->c, p->col, p->fill, p->thickness);
		}

		void make_circle(geo::s_geo* d, Canvas* g)
		{
			geo::circle* p = (geo::circle*)d;
			g->draw_circle(p->centre, p->radius, p->col, p->fill, p->num_segments, p->thickness);
		}

		void make_ellipse(geo::s_geo* d, Canvas* g)
		{
			geo::ellipse* p = (geo::ellipse*)d;
			g->draw_ellipse(p->centre, p->rx, p->ry, p->col, p->fill, p->num_segments, p->thickness);
		}

		void make_larme(geo::s_geo* d, Canvas* g)
		{
			geo::larme* p = (geo::larme*)d;
			g->draw_larme(p->centre, p->radius, p->n, p->col, p->fill, p->num_segments, p->thickness);
		}

		void make_curve(geo::s_geo* d, Canvas* g)
		{
			geo::curve* p = (geo::curve*)d;
			g->draw_bezier_curve(p->pos0, p->cp0, p->cp1, p->pos1, p->col, p->thickness, p->num_segments);
		}

		void make_image(geo::s_geo* d, Canvas* g)
		{
			geo::image* p = (geo::image*)d;
			glm::ivec2 texsize = { p->a.z, p->a.w };
			if (p->img)
			{
				texsize = { p->img->width, p->img->height };
			}
			float sums = (p->rect.x + p->rect.y + p->rect.z + p->rect.w);
			if (!(sums > 0))
			{
				p->rect.x = -1;
			}
			g->draw_image(p->img, p->a, p->rect, p->sliced, p->col);
		}

		void make_imageQuad(geo::s_geo* d, Canvas* g)
		{
			geo::image* p = (geo::image*)d;
			g->draw_image_quad(p->img, p->a, p->b, p->c, p->d, p->uv_a, p->uv_b, p->uv_c, p->uv_d, p->col);
		}
		void make_text(geo::s_geo* d, Canvas* g)
		{
			geo::text* p = (geo::text*)d;
			g->draw_text(*p->text, p->pos, p->col, p->wrap_width, 0, *p->font, p->font_size, p->font_dpi);
		}

#endif

		typedef enum {
			SVG_PATH_OP_MOVE_TO,
			SVG_PATH_OP_LINE_TO,
			SVG_PATH_OP_CURVE_TO,
			SVG_PATH_OP_QUAD_CURVE_TO,
			SVG_PATH_OP_ARC_TO,
			SVG_PATH_OP_CLOSE_PATH
		} svg_path_op;
		void svg_draw_path(const std::string& cmd, std::vector<glm::vec2>* path, glm::vec2 pos, glm::vec2 scale, unsigned int col, unsigned int fillcol = 0, double thickness = 1.0)
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
			if (col == 0)
			{
				pos = {}; scale = { 1,1 };
			}
			auto& d = *path;
			PathClear();
			glm::vec2 reflected_cubic_pt, reflected_quad_pt, current_pt;
			svg_path_op op;
			bool isop = false;
			std::function<void(int& i, char ch)> func[128];
			std::vector<glm::vec2> testpath;
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
			func['m'] = func['M'] = [&](int& i, char ch) {
				set_current_pt(next_cur_pos(i, ch), SVG_PATH_OP_MOVE_TO);
				PathLineTo(pos + scale * current_pt /*+ glm::vec2(0.5f, 0.5f)*/);

			};
			func['z'] = func['Z'] = [&](int& i, char ch) {
				path_merge();//使用int去重
				if (get_alpha(fillcol))
				{
					PathFillConvex(fillcol, false);
				}
				if (get_alpha(col))
				{
					PathStroke(col, true, thickness);
				}
				op = SVG_PATH_OP_CLOSE_PATH;
				if (i + 1 < d.size())
				{
					//i++;
				}
			};
			func['l'] = func['L'] = [&](int& i, char ch) {
				set_current_pt(next_cur_pos(i, ch), SVG_PATH_OP_LINE_TO);
				PathLineTo(pos + scale * current_pt);
			};
			// 水平线\垂直线 在解析时填充另一个属性
			func['h'] = func['H'] = func['v'] = func['V'] = func['L'];

			// bezier_curve
			auto curve_to = [&](glm::vec2 cp0, glm::vec2 cp1, glm::vec2 pos1) {
				glm::vec2 pos0 = current_pt;
				auto dif = abs(scale * pos0 - scale * pos1);
				int difi = dif.x + dif.y;
				PathBezierCurveTo(pos + scale * cp0, pos + scale * cp1, pos + scale * pos1, difi);
				set_reflected_cubic_pt(pos1 + pos1 - cp1);
				set_current_pt(pos1, SVG_PATH_OP_CURVE_TO);
			};
			func['c'] = func['C'] = [&](int& i, char ch) {
				glm::vec2 cp0 = next_cur_pos(i, ch);
				glm::vec2 cp1 = next_cur_pos(i, ch);
				glm::vec2 pos1 = next_cur_pos(i, ch);
				curve_to(cp0, cp1, pos1);
			};
			func['s'] = func['S'] = [&](int& i, char ch) {

				glm::vec2 cp1 = next_cur_pos(i, ch);
				glm::vec2 pos1 = next_cur_pos(i, ch);
				if (isop)
					curve_to(reflected_cubic_pt, cp1, pos1);
				else
					curve_to(current_pt, cp1, pos1);
			};
			auto quadratic_curve_to = [&](glm::vec2 cp1, glm::vec2 pos1) {
				curve_to(current_pt, cp1, pos1);
				set_reflected_quad_pt(pos1 + pos1 - cp1);
				set_current_pt(pos1, SVG_PATH_OP_QUAD_CURVE_TO);
			};
			func['q'] = func['Q'] = [&](int& i, char ch) {
				glm::vec2 cp1 = next_cur_pos(i, ch);
				glm::vec2 pos1 = next_cur_pos(i, ch);
				quadratic_curve_to(cp1, pos1);
			};
			func['t'] = func['T'] = [&](int& i, char ch) {
				glm::vec2 pos1 = next_cur_pos(i, ch);
				if (isop)
					quadratic_curve_to(reflected_quad_pt, pos1);
				else
					quadratic_curve_to(current_pt, pos1);
			};
			func['a'] = func['A'] = [&](int& i, char ch) {
				glm::vec2 r = next_cur_pos(i, ch);
				glm::vec2 x_axis_rotation = next_cur_pos(i, ch);
				glm::vec2 flag = next_cur_pos(i, ch);//large_arc_flag、sweep_flag
				glm::vec2 pos = next_cur_pos(i, ch);

				PathArcTo(pos, r, 360, false);
				set_current_pt(pos, SVG_PATH_OP_ARC_TO);
			};

			int i = 0;
			for (auto it : cmd)
			{
				func[it](i, it);
			}
			return;
		}
		/*
		箭头线
		原点origin
		终点end_point
		控制点ctrl_point
		进度progress最大1，最小0
		*/
		void gui_draw_arrow_point(glm::ivec3 origin, glm::ivec3 end_point, glm::vec4* ctrl_point = nullptr,
			unsigned int col = -1, double thickness = 1.0, double blur = 3.0, double progress = 1.0, int num_segments = -1
			, glm::vec4* idx_pos = nullptr, std::vector<glm::vec2>* outpath = nullptr)
		{
			auto doe = origin - end_point;
			auto doe_a = abs(doe);
			glm::vec2 pos0 = origin;
			glm::vec2 cp0;
			glm::vec2 cp1;
			glm::vec2 pos1 = end_point;
			if (ctrl_point)
			{
				cp0 = { ctrl_point->x,ctrl_point->y };
				cp1 = { ctrl_point->z,ctrl_point->w };
			}
			else
			{
				//（(x1 + x2) / 2, (y1 + y2) / 2）
				double xx = doe_a.x - doe_a.y;
				double yy = xx;
				if (xx > 0)
					xx *= 2;
				else
					yy *= 2;
				cp0 = cp1 = glm::vec2((origin.x + end_point.x) * 0.5 + abs(xx), (origin.y + end_point.y) * 0.5 + abs(xx) + yy);

			}
			//if (num_segments < 1)
			//	num_segments = doe_a.x + doe_a.y;
			//if (num_segments < 360)
			//{
			//	num_segments = 360;
			//}
			draw_poly_gradient({ pos0.x, pos0.y , col }, thickness + blur + 2, set_alpha_f(col, 0.1), 1);
			draw_poly_gradient({ pos1.x, pos1.y , col }, thickness + blur + 2, set_alpha_f(col, 0.1), 1);

			draw_poly_gradient({ cp0.x, cp0.y , col }, blur, set_alpha_f(col, 0.1), 1);
			draw_poly_gradient({ cp1.x, cp1.y , col }, blur, set_alpha_f(col, 0.1), 1);
			draw_line({ cp0.x, cp0.y }, { pos0.x, pos0.y }, 0x50999999);
			draw_line({ cp1.x, cp1.y }, { pos1.x, pos1.y }, 0x50999999);
			idx_pos->w = progress;
			draw_bezier_curve(pos0, cp0, cp1, pos1, set_alpha_f(col, 0.1), thickness + blur, num_segments, idx_pos, outpath);
			draw_bezier_curve(pos0, cp0, cp1, pos1, col, thickness, num_segments);

		}
	private:
	};
	// !canvas

	class DrawData :public Res
	{
	public:
		std::vector<hz::Canvas*>	CmdLists;
		int				CmdListsCount = 0;
		int             TotalVtxCount = 0;          // For convenience, sum of all cmd_lists vtx_buffer.Size
		int             TotalIdxCount = 0;          // For convenience, sum of all cmd_lists idx_buffer.Size
		glm::vec2       DisplayPos;             // Upper-left position of the viewport to render (== upper-left of the orthogonal projection matrix to use)
		glm::vec2       DisplaySize;            // Size of the viewport to render (== io.DisplaySize for the main viewport) (DisplayPos + DisplaySize == lower-right of the orthogonal projection matrix to use)
		glm::vec2       FramebufferScale;       // Amount of pixels for each unit of DisplaySize. Based on io.DisplayFramebufferScale. Generally (1,1) on normal display, (2,2) on OSX with Retina display.


		void* shader = 0;
		size_t			stride = 0;
		LockS _lockres;

		std::atomic_int64_t _upt = 0;
		int64_t c_upt = 0;

	public:
		void push(hz::Canvas* canvas)
		{
			stride = canvas->_stride;
			CmdLists.push_back(canvas);
			TotalVtxCount += canvas->getVtxCount();
			TotalIdxCount += canvas->getIdxCount();
			CmdListsCount++;
			if (!shader)
			{
				shader = canvas->getShader();
			}
		}
		void clear()
		{
			CmdListsCount = TotalVtxCount = TotalIdxCount = 0;
			CmdLists.clear();
		}
		void update()
		{
			int clc = 0, tvc = 0, tic = 0;
			bool isup = true;
			for (auto it : CmdLists)
			{
				auto& k = it->_lock_mi;
				LOCK_R(k);
				tvc += it->getVtxCount();
				tic += it->getIdxCount();
				clc++;
			}
			if (TotalVtxCount != tvc || TotalIdxCount != tic || CmdListsCount != clc || isup)
			{
				TotalVtxCount = tvc;
				TotalIdxCount = tic;
				CmdListsCount = clc;
				set_update();
			}
		}
		//bool isUpdate() { return get_upt()!=_uptn; }
		size_t size()
		{
			return	CmdLists.size();
		}
		DrawData() {}
		~DrawData() {}
		int64_t get_upt()
		{
			return _upt;
		}
		void set_update()
		{
			_upt++;
		}
	public:
		// Helper to scale the ClipRect field of each ImDrawCmd. Use if your final output buffer is at a different scale than ImGui expects, or if there is a difference between your window resolution and framebuffer resolution.
		void ScaleClipRects(const glm::vec2& scale)
		{
			for (int i = 0; i < CmdListsCount; i++)
			{
				Canvas* cmd_list = CmdLists[i];
				for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++)
				{
					Canvas::DrawCmd* cmd = &cmd_list->CmdBuffer[cmd_i];
					cmd->ClipRect = glm::vec4(cmd->ClipRect.x * scale.x, cmd->ClipRect.y * scale.y, cmd->ClipRect.z * scale.x, cmd->ClipRect.w * scale.y);
				}
			}
		}
	};


	//-----------------------------------------------------------------------------

	//-----------------------------------------------------------------------------
	/*3d*/
		//AABB
	typedef struct Dimension
	{
		glm::vec3 min = glm::vec3(FLT_MAX);
		glm::vec3 max = glm::vec3(-FLT_MAX);
		glm::vec3 size;
	} Dimension;
	class Object3d :public Res
	{
	public:
		// 		std::vector<float> *vertexBuffer = 0;
		// 		std::vector<uint32_t> *indexBuffer = 0;
		std::vector<DrawIdx>		IdxBuffer;
		std::vector<DrawVert>		VtxBuffer;
		std::vector<float>			fVtxBuffer;
		int							_stride = 0;
		typedef struct BaseSize
		{
			uint64_t parts = 0;
			uint64_t fvtx = 0;
			uint64_t idx = 0;
			BaseSize(
				uint64_t p = 0,
				uint64_t f = 0,
				uint64_t i = 0) :
				parts(p),
				fvtx(f),
				idx(i)
			{}
		}BaseSize;
		//uint32_t indexCount = 0;
		//uint32_t vertexCount = 0;
		/** @brief Stores vertex and index base and counts for each part of a model */
		struct ModelPart {
			uint32_t vertexBase;
			uint32_t vertexCount;
			uint32_t indexBase;
			uint32_t indexCount;
			//uint32_t instanced = 0;
		};
		std::vector<ModelPart> parts;
		Dimension dim;

		void* shader = 0;
		//std::vector<float> ubo;
		glm::mat4* mat = 0;
		bool isupdate = true;
		Canvas* g = 0;
		Image* _img = 0;
	public:
		void savebin(const std::string& fn)
		{
			BaseSize bs = BaseSize{
				sizeof(uint32_t) + sizeof(ModelPart) * parts.size(), sizeof(uint32_t) + sizeof(float) * fVtxBuffer.size(),
				sizeof(uint32_t) + sizeof(DrawIdx) * IdxBuffer.size() };
			size_t size = 0;
			size += sizeof(_stride);
			size += sizeof(dim);
			size += bs.parts;
			size += bs.fvtx;
			size += bs.idx;
#ifdef _WIN32 
			hz::MapView mv;
			if (mv.openfile(fn.c_str(), false) || mv.createfile(fn.c_str()))			//打开文件					
			{
				mv.creatmap(size);
				char* buf = (char*)mv.mapview();	//获取映射内容
				std::vector<char> vbuf;
				char* vb = buf;
				if (buf)
				{
					memcpy(buf, &_stride, sizeof(_stride));
					buf += sizeof(_stride);
					memcpy(buf, &dim, sizeof(dim));
					buf += sizeof(dim);
					uint32_t s = parts.size();
					memcpy(buf, &s, sizeof(s));
					buf += sizeof(s);
					memcpy(buf, &parts[0], sizeof(ModelPart) * s);
					buf += bs.parts - sizeof(s);

					s = fVtxBuffer.size();
					memcpy(buf, &s, sizeof(s));
					buf += sizeof(s);
					memcpy(buf, &fVtxBuffer[0], sizeof(float) * s);
					buf += bs.fvtx - sizeof(s);

					s = IdxBuffer.size();
					memcpy(buf, &s, sizeof(s));
					buf += sizeof(s);
					memcpy(buf, &IdxBuffer[0], sizeof(DrawIdx) * s);

					vbuf.resize(size);
					memcpy(vbuf.data(), vb, size);
					mv.FlushView();
				}
			}
#endif
		}
		void loadbin(const std::string& fn)
		{

#ifdef _WIN32 
			hz::MapView mv;
			if (mv.openfile(fn.c_str(), true))			//打开文件					
			{
				char* buf = (char*)mv.mapview();	//获取映射内容
				if (buf)
				{
					memcpy(&_stride, buf, sizeof(_stride));
					buf += sizeof(_stride);
					memcpy(&dim, buf, sizeof(dim));
					buf += sizeof(dim);

					uint32_t s = 0;
					memcpy(&s, buf, sizeof(s));
					buf += sizeof(s);
					parts.resize(s);
					memcpy(&parts[0], buf, sizeof(ModelPart) * s);
					buf += sizeof(ModelPart) * s;

					s = 0;
					memcpy(&s, buf, sizeof(s));
					buf += sizeof(s);
					fVtxBuffer.resize(s);
					memcpy(&fVtxBuffer[0], buf, sizeof(float) * s);
					buf += sizeof(float) * s;

					s = 0;
					memcpy(&s, buf, sizeof(s));
					buf += sizeof(s);
					IdxBuffer.resize(s);
					memcpy(&IdxBuffer[0], buf, sizeof(DrawIdx) * s);

					if (g)
					{
						g->addModel(fVtxBuffer, _stride, IdxBuffer, _img);
						g->_blend = false;
					}
					//mv.FlushView();
				}
			}
#endif

		}
		int getPartsCount()
		{
			return parts.size();
		}
#if 0
		typedef struct DrawIndexedCmd
		{
			uint32_t    elemCount = 0;
			uint32_t    idx_offset = 0;
			int32_t     vtx_offset = 0;
			void* image = 0;
			UniformBuffer* vs_ubo = 0;
			UniformBuffer* ps_ubo = 0;
			uint32_t	instanced = 0;
		}DrawIndexedCmd;
#endif
		void setDraw(int idx, Image* img, UniformBuffer* vs_ubo, UniformBuffer* ps_ubo)
		{
			DrawIndexedCmd icmd = {};
			int n = parts.size();
			if (g && n > 0 && idx >= 0 && idx < n)
			{
				icmd.elemCount = parts[idx].indexCount;
				icmd.idx_offset = parts[idx].indexBase;
				icmd.vtx_offset = parts[idx].vertexBase;
				icmd.image = img;
				icmd.vs_ubo = vs_ubo;
				icmd.ps_ubo = ps_ubo;
				g->pushCmd3d(icmd);
			}
		}
		void clearDraw()
		{
			g->clearCmd3d();
		}
	public:
		Object3d()
		{
			g = Canvas::create();
			g->depthWriteEnable = true;
		}

		~Object3d()
		{
			deletemem();
		}

		void deletemem()
		{
			// 			if (vertexBuffer)
			// 			{
			// 				delete vertexBuffer;
			// 			}
			// 			if (indexBuffer)
			// 			{
			// 				delete indexBuffer;
			// 			}
						//indexBuffer = 0;
						//vertexBuffer = 0;
			Canvas::destroy(g); g = 0;
			Image::destroy(_img); _img = 0;
		}
	};

	class DrawData3d :public Res
	{
	public:
		std::vector<Object3d*>	_lists;
	public:
		void push(Object3d* obj)
		{
			_lists.push_back(obj);
		}
		void pop(Object3d* obj)
		{
			for (size_t i = 0; i < _lists.size(); i++)
			{
				if (_lists[i] == obj)
				{
					_lists.erase(_lists.begin() + i);
					break;
				}
			}
		}
		size_t count()
		{
			return	_lists.size();
		}
	public:
		DrawData3d() {}
		~DrawData3d() {}
	};


#define OBJ3d Canvas
	//-----------------------------------------------------------------------------
	namespace cav {

#ifdef NO_CB
		//      class Text :public Odd { public: std::wstring &text; glm::vec2& pos; unsigned int col; float wrap_width; glm::vec4* cpu_fine_clip_rectconst; std::wstring &font; float font_size; float font_dpi = 96; glm::vec2 *outsize = 0)};
		// 		class ImageI :public Odd { public: Image* user_image;  glm::vec4& a; glm::vec2 texsize = glm::vec2{ -1 };	glm::vec4 rect = glm::vec4{ -1 };  glm::vec4 &sliced = glm::vec4{ -1 }; unsigned int col = -1; };
		// 		class BezierCurve :public Odd { public:  glm::vec2& pos0;  glm::vec2& cp0;  glm::vec2& cp1;  glm::vec2& pos1; unsigned int col; float thickness; int num_segments; };
		// 		class Arc :public Odd { public:  glm::vec2& centre; float rx; float ry; unsigned int col; unsigned int fill; int num_start; int num_end; int num_segments; float thickness = 1.0f; };
		// 		class Eellipse :public Odd { public:  glm::vec2& centre; float rx; float ry; unsigned int col; unsigned int fill; int num_segments; float thickness = 1.0f; };
		// 		class Circle :public Odd { public:  glm::vec2& centre; float radius; unsigned int col; unsigned int fill; int num_segments; float thickness = 1.0f; };
		// 		class DirTriangle :public Odd { public:  glm::vec2 s; glm::vec2 pos; GuiDir dir; unsigned int fill; unsigned int color = 0; };
		// 		class Triangle :public Odd { public:  glm::vec2& a;  glm::vec2& b;  glm::vec2& c; unsigned int col; unsigned int fill; float thickness = 1.0f; bool aa = true; };
		// 		class Quad :public Odd { public:  glm::vec2& a;  glm::vec2& b;  glm::vec2& c;  glm::vec2& d; unsigned int col; unsigned int fill; float thickness = 1.0f; };
		// 		class rect_filledGradient :public Odd { public:  glm::vec4& rect;  glm::ivec3 &center;	 glm::ivec3 *inner_;/* x=%;y=%;z = col*/	float rounding = 0.0f; unsigned int col_outer = 0; };
		// 		class rect_filledMultiColor :public Odd { public:  glm::vec2& a;  glm::vec2& c; unsigned int col_upr_left; unsigned int col_upr_right; unsigned int col_bot_right; unsigned int col_bot_left; };
		// 		class Rect :public Odd { public:  glm::ivec4& a; unsigned int col; unsigned int fill; float rounding = 0.0f; int rounding_corners_flags = ~0; float thickness = 1.0f; };
		// 		class Line :public Odd { public:  glm::vec2& a;  glm::vec2& b; unsigned int col; float thickness = 1.0f; };
		// 		class Polyline :public Odd { public:  glm::vec2* points;  int points_count; unsigned int col; bool closed; float thickness; bool anti_aliased; };
		// 		class ConvexPolyFilled :public Odd { public:  glm::vec2* points;  int points_count; unsigned int col; bool anti_aliased; };
		class Odd
		{
		public:
			void* _user_data = 0;
		public:
			Odd() {}
			virtual ~Odd() {}
		public:
			// 获取宽高
			virtual glm::vec2 get_extent()
			{
				return glm::vec2(0, 0);
			}
			virtual void draw(Canvas* g) {}
		};
		class Text :public Odd
		{
		private:
			std::string u8text;
			std::wstring text;
		public:
			glm::vec2 pos; unsigned int col = -1;
			glm::vec4 cpu_fine_clip_rectconst = glm::vec4{ -1,-1,-1,-1 };
			// 单字、单词颜色cols<x=index,y=length,z=color>
			std::vector<glm::ivec3> cols;
			font_att* fat = nullptr;
			bool is_me = false;
			// 选中范围x-y, z为开始偏移,w结束位置
			glm::vec4 scope;
			std::function<void(std::vector<glm::vec4>& hotarea, glm::vec2 outsize)> ha_func;
		public:
			Text()
			{
				//fat = font_att::create();
			}
			~Text()
			{
				if (fat && is_me)
				{
					delete fat;
					fat = nullptr;
				}
			}
			void set_text(const std::string& s)
			{
				u8text = s;
				text = jsont::utf8_to_wstring(s);
			}
			void set_text(const std::wstring& s)
			{
				text = s;
				u8text = jsont::wstring_to_utf8(s);
			}
			void set_font(font_att* fa)
			{
				fat = fa;
				col = fa->color;
				//font = fa->wfontname;
				//font_size = fa->fontsize;
				//font_dpi = fa->dpi;
				//wrap_width = fa->wrap_width;
				//line_height = Font::get_font_height(fa) + 4;
			}
			// 获取长度
			glm::vec2 get_extent()
			{
				return { 0,0 };// Font::get_text_size(u8text, fat);
			}
			virtual void draw(Canvas* g)
			{
				g->draw_text(text, pos, col, //wrap_width,
					(cpu_fine_clip_rectconst.z < 0 || cpu_fine_clip_rectconst.w < 0) ? nullptr : &cpu_fine_clip_rectconst,
					fat, scope, ha_func, &cols);
			}
		};
		class ImageI :public Odd
		{
		public:
			Image* user_image;  glm::vec4 a; glm::vec2 texsize = glm::vec2{ -1 };	glm::vec4 rect = glm::vec4{ -1 };  glm::vec4 sliced = glm::vec4{ -1 }; unsigned int col = -1;
		public:
			virtual void draw(Canvas* g)
			{
				g->draw_image(user_image, a, rect, sliced, col);
			}
		};
		class BezierCurve :public Odd
		{
		public:
			glm::vec2 pos0;  glm::vec2 cp0;  glm::vec2 cp1;  glm::vec2 pos1; unsigned int col = -1; float thickness = 1.0; int num_segments = 100;
		public:
			virtual void draw(Canvas* g)
			{
				g->draw_bezier_curve(pos0, cp0, cp1, pos1, col, thickness, num_segments);
			}
		};
		class Arc :public Odd
		{
		public:
			glm::vec2 centre; float rx; float ry; unsigned int col; unsigned int fill; int num_start; int num_end; int num_segments; float thickness = 1.0f;
		public:
			virtual void draw(Canvas* g)
			{
				g->draw_arc(centre, rx, ry, col, fill, num_start, num_end, num_segments, thickness);
			}
		};
		class Eellipse :public Odd
		{
		public:
			glm::vec2 centre; float rx; float ry; unsigned int col; unsigned int fill; int num_segments; float thickness = 1.0f;
		public:
			virtual void draw(Canvas* g)
			{
				g->draw_ellipse(centre, rx, ry, col, fill, num_segments, thickness);
			}
		};
		class Circle :public Odd
		{
		public:
			glm::vec2 centre; float radius; unsigned int col; unsigned int fill; int num_segments; float thickness = 1.0f;
		public:
			virtual void draw(Canvas* g)
			{
				g->draw_circle(centre, radius, col, fill, num_segments, thickness);
			}
		};
		class DirTriangle :public Odd
		{
		public:
			glm::vec2 s; glm::vec2 pos; GuiDir dir; unsigned int fill; unsigned int color = 0;
		public:
			virtual void draw(Canvas* g)
			{
				g->addDirTriangle(s, pos, dir, fill, color);
			}
		};
		class Triangle :public Odd
		{
		public:
			glm::vec2 a;  glm::vec2 b;  glm::vec2 c; unsigned int col; unsigned int fill; float thickness = 1.0f; bool aa = true;
		public:
			virtual void draw(Canvas* g)
			{
				g->draw_triangle(a, b, c, col, fill, thickness, aa);
			}
		};
		class Quad :public Odd
		{
		public:
			glm::vec2 a;  glm::vec2 b;  glm::vec2 c;  glm::vec2 d; unsigned int col; unsigned int fill; float thickness = 1.0f;
		public:
			virtual void draw(Canvas* g)
			{
				g->draw_quad(a, b, c, d, col, fill, thickness);
			}
		};
		class RectFilledGradient :public Odd
		{
		public:
			glm::vec4 rect;  glm::ivec3 center;	 glm::ivec3 inner_;/* x=%;y=%;z = col*/	float rounding = 0.0f; unsigned int col_outer = 0;
		public:
			virtual void draw(Canvas* g)
			{
				g->draw_rect_filled_gradient(rect, center, &inner_, rounding, col_outer);
			}
		};
		class RectFilledMultiColor :public Odd
		{
		public:
			glm::vec2 a;  glm::vec2 c; unsigned int col_upr_left; unsigned int col_upr_right; unsigned int col_bot_right; unsigned int col_bot_left;
		public:
			virtual void draw(Canvas* g)
			{
				g->draw_rect_filled_multi_color(a, c, col_upr_left, col_upr_right, col_bot_right, col_bot_left);
			}
		};
		class Rect :public Odd
		{
		public:
			glm::ivec4 a; unsigned int col; unsigned int fill; float rounding = 0.0f; int rounding_corners_flags = ~0; float thickness = 1.0f;
		public:
			virtual void draw(Canvas* g)
			{
				g->draw_rect(a, col, fill, rounding, rounding_corners_flags, thickness);
			}
		};
		class Line :public Odd
		{
		public:
			//glm::vec2 a;  glm::vec2 b;
			unsigned int col; float thickness = 1.0f;
			std::vector<glm::vec4> data;
		public:
			void push(glm::vec2 a, glm::vec2 b)
			{
				data.push_back({ a,b });
			}
			void clear()
			{
				data.clear();
			}
		public:
			virtual void draw(Canvas* g)
			{
				for (auto& it : data)
				{
					g->draw_line({ it.x,it.y }, { it.z,it.w }, col, thickness);
				}
			}
		};
		class Polyline :public Odd
		{
		public:
			glm::vec2* points;  int points_count; unsigned int col; bool closed; float thickness; bool anti_aliased = true;
		public:
			virtual void draw(Canvas* g)
			{
				g->draw_polyline(points, points_count, col, closed, thickness, anti_aliased);
			}
		};
		class ConvexPolyFilled :public Odd
		{
		public:
			glm::vec2* points;  int points_count; unsigned int col; bool anti_aliased = true;
		public:
			virtual void draw(Canvas* g)
			{
				g->draw_convex_poly_filled(points, points_count, col, anti_aliased);
			}
		};
#endif

		class Div
		{
		private:
			std::map<std::string, std::function<void()>> _json_func_draw;
		public:
			Div()
			{
			}

			~Div()
			{
			}

		private:

		};

	}//!cav
	//  ---------------------------------------------------------------------------
#if 0
	class soft_dev :public Singleton<soft_dev>
	{
	public:
		soft_dev()
		{
		}

		~soft_dev()
		{
		}
	public:
		void push(Canvas* p)
		{
			if (p)
			{
				_data.push(p);
			}
		}
		void save_fbo(const char* fn)
		{
			_fbo.saveImage(fn);
		}
		void resize(int w, int h)
		{
			if (!_dev)
			{
				_dev = SoftRender::create();
			}
			_dev->init(w, h, &_fbo);
			_dev->set_state(RENDER_STATE_COLOR);
		}
		void draw_data()
		{
			// Render command lists			
			int vtx_offset = 0;
			int idx_offset = 0;
			for (int n = 0; n < _data.CmdListsCount; n++)
			{
				hz::Canvas* g = _data.CmdLists[n];
				if (g->CmdBuffer.empty())
				{
					continue;
				}
				//setDepthState(g->depthTestEnable, g->depthWriteEnable);
				_dev->setBuffers(0, g->VtxBuffer.data(), g->VtxBuffer.size());
				_dev->setBuffers(1, g->IdxBuffer.data(), g->IdxBuffer.size());
				_dev->setVertex(0, sizeof(DrawVert));
				_dev->setIndex(1, sizeof(DrawIdx));
				if (g->_image)
				{
					_dev->setTexture(g->_image);
				}
				//ctx->RSSetState(g_pRasterizerState);
				for (int i = 0; i < g->CmdBuffer.size(); i++)
				{
					const hz::Canvas::DrawCmd* pcmd = &g->CmdBuffer[i];
					{
						//const D3D11_RECT r = { (LONG)pcmd->ClipRect.x, (LONG)pcmd->ClipRect.y, (LONG)pcmd->ClipRect.z, (LONG)pcmd->ClipRect.w };
						_dev->setTexture((Image*)pcmd->image);
						//ctx->RSSetScissorRects(1, &r);
						//if (pass->_instanceCMD)
						//{
							//	ctx->DrawIndexedInstancedIndirect(pass->_instanceCMD->buffer_ptr(), pass->_icmd_stride);
						//}
						//else
						{
							if (pcmd->instanced)
							{
								//ctx->DrawIndexedInstanced(pcmd->ElemCount, pcmd->instanced, idx_offset, vtx_offset, 0);
							}
							else
							{
								_dev->DrawIndexed(pcmd->ElemCount, idx_offset, vtx_offset);
							}

						}
					}
					idx_offset += pcmd->ElemCount;
				}
				vtx_offset += g->getVtxCount();
			}
		}
	private:
		LockS _k;
		SoftRender* _dev = nullptr;
		Image _fbo;
		DrawData _data;
	};
#endif
	// ------------------------------------------------------------------------------
}//!HZ
#endif // !__Canvas__H__
