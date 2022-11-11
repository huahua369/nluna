#include <ntype.h>
#include <base/camera.h>
#include <view/image.h>
#include <view/draw_info.h>
#include <base/smq.h>
#include <font/font_cx.h>
#include <algorithm>

#include "base/net/crc32.hpp"
using namespace std;
#include "canvas2d.h"
#include "bw_sdl.h"
//#include <vulkan/vulkan.h>
#include <vk_core/vk_cx.h>
#include <vk_core/vk_cx1.h>

#include "view.h"
#if 1
#include <SDL2/SDL_keyboard.h>
#endif
#include <base/Action.h>
#include <base/win_core_dragdrop.h>
#include <base/text/buffer.h>
#include <base/ecc_sv.h>
#include <gui/hz_gui.h>
#include "geometry/AABB2D.h"
//#include "geometry/KDTree.h"
//#include "geometry/QuadTree.h"
//#include "geometry/QuadTree/QuadTree.h"
//#include "geometry/QuadTree/QuadTree.inl"

#include <shared.h>

using namespace entt;
using namespace hz;

#include <shared.h>
namespace hz {
	std::string format(const char* format, ...);
	std::string log_format(const char* format, ...);
}
MC_EXPORT void dslog(const char* d, int n = 0);


// 内存泄漏检测
#ifdef _DEBUG
#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif

namespace hz {

	ubo_ac::ubo_ac() {}
	ubo_ac::~ubo_ac()
	{
		for (auto it : allubo)
			delop(it);
		delop(_dset);
	}
	void ubo_ac::init(pipeline_ptr_info* pipe)
	{
		_pipe = (pipe);
		_dset = pipe->new_dvkset();
		ubon = pipe->_srinfo.get_uboname();
		texn = pipe->_srinfo.get_tex_name();
		assert(ubon.size());
		auto dev = pipe->_dev;
		if (ubon.size())
		{
			dev_maxubo_size = dev->_limits->maxUniformBufferRange;	// 设备支持的ubo大小
			ubo_stride = pipe->_srinfo.get_ubosize(ubon[0]);		// shader使用的ubo大小
			if (dev_maxubo_size == -1)
			{
				dev_maxubo_size = 65536;
			}
			printf("dev_maxubo_size\t%d\n", dev_maxubo_size);
		}
		make_sets();
	}

	void ubo_ac::mk_ubo(ubo_set_t& us)
	{
		assert(ubo_stride <= dev_maxubo_size);
		assert(ucap.upos <= _ubo->_size);
		if (us.size > ubo_stride)
		{
			return;	// 大于设备支持最大ubo返回空
		}
		if (!ucap.dsb.dset)
		{
			// 创建新set
			ucap.dsb = _dset->write_buffer(ubon[0], _ubo, ubo_stride, 0, us.dset);
			//ucap.last_pos += dev_maxubo_size;
			ucap.upos = 0;
		}
		//四个参数 set，绑定号，动态偏移，ubo数据更新偏移memcpy用
		us.dset = ucap.dsb.dset;
		us.first_set = ucap.dsb.first_set;
		us.offsets = ucap.upos;
		//us = { , , ucap.upos, ucap.last_pos - dev_maxubo_size, us.size };
		ucap.upos += us.size;
	}

	void ubo_ac::make_sets()
	{
		auto dev = _pipe->_dev;
		if (!_ubo || _ubo_cap == _ubo_count)
		{
			_ubo = dvk_buffer::new_ubo(dev, dev_maxubo_size * _ubo_count);
			allubo.push_back(_ubo);
			_ubo_cap = 0;
		}
		_ubo_cap++;
	}
	ubo_set_t ubo_ac::write_image(dvk_texture* p)
	{
		ubo_set_t& ds = image_setm[p];
		if (!ds.dset)
		{
			ds = _dset->write_image(texn[0], p, ds.dset);
			if (ds.dset)
			{
				image_setm[p] = ds;
				if (p->user_data)
				{
					image_setm[p->user_data] = ds;
				}
			}
		}
		return ds;
	}


	std::array<ubo_set_t, 2> ubo_ac::get_set2(motion_t* ubo, void* img)
	{
		std::array<ubo_set_t, 2> ret = {};
		auto it = image_setm.find(img);
		if (it != image_setm.end()) { ret[1] = it->second; }
		auto ut = ubo_setm.find(ubo);
		if (ut != ubo_setm.end()) { ret[0] = ut->second; }
		if (ret[1].dset == ret[0].dset)
		{
			ret[1].dset = 0;
		}
		return ret;
	}

	void ubo_ac::push_ubo(motion_t* m)
	{
		if (m)
		{
			auto& it = ubo_setm[m];
			if (!it.dset)
			{
				//	it = new_ubo();
			}
		}
	}

	bool ubo_ac::needed(size_t n, size_t stride)
	{
		size_t ubosize = (ubo_setm.size() + n) * stride;
		bool r = false;
		if (ubosize > _ubo->_size)
		{
			_ubo->resize(ubosize + dev_maxubo_size);
			r = true;
			for (auto& [k, v] : ubo_setm) {
				v.size = 0;
			}
		}
		return r;
	}

	void ubo_ac::update(std::map<canvas_cx*, matubo_t>& vpcubo)
	{
		for (auto& [k, v] : ubo_setm)
		{
			auto it = (motion_t*)k;
			// 平移
			auto tm = glm::translate(glm::mat4(1.0f), it->pos);
			// 缩放
			auto sm = glm::mat4(1.0);
			assert((it->scale.x * it->scale.y * it->scale.z) > 0);
			if ((it->scale.x * it->scale.y * it->scale.z) > 0)
			{
				sm = glm::scale(glm::mat4(1.0f), it->scale);
			}
			// 旋转
			auto rm = glm::translate(glm::mat4(1.0f), it->rotate_pos) * glm::mat4_cast(it->rotate)
				* glm::translate(glm::mat4(1.0f), -it->rotate_pos);

			auto cp = (canvas_cx*)it->viewport_ptr;
			auto vpd = vpcubo[cp];
			auto mat = vpd.m * tm * sm * rm;	// 合并平移缩放旋转矩阵
			auto& up = v;
			if (!up.size)
			{
				up.size = sizeof(glm::mat4);
				mk_ubo(up);
			}
			if (up.dset)
			{
				_ubo->set_data(&mat, sizeof(glm::mat4), (size_t)up.offsets, false);
			}
		}
		_ubo->flush();
	}


	// page_view begin

	page_view::page_view() {}
	page_view::~page_view()
	{
		for (auto it : tex_free)
			delop(it);
		delop(p_upload);
	}

	// 管线、cap_count预先分配顶点(单位m)

	int page_view::init(pipeline_ptr_info* pipe, Fonts* ftctx)
	{
		if (!pipe && _isinit)
		{
			return -1;
		}
		_ftctx = ftctx;
		_pipe = pipe;
		auto dev = pipe->_dev;
		auto white_img = new Image(512, 512, -1); //白色纹理用来画矢量图
		p_upload = new upload_cx();
		p_upload->init(dev, 1024 * 1024 * 16, 1);
		tex_white = dvk_texture::new_image2d(white_img, p_upload);
		p_upload->flushAndFinish();
		delop(white_img);
		if (!tex_white)
		{
			return -1;
		}
		// todo 预计分配空间
		_vbo = dvk_buffer::new_vbo(dev, false, false, 1024, 0);
		//auto mpf = "[vbo]:\t" + std::to_string((uint64_t)_vbo) + "memoryPropertyFlags:" + std::to_string(_vbo->memoryPropertyFlags);
		//mpf.push_back('\n');
		//dslog(mpf.c_str(), mpf.size());
		_ibo = dvk_buffer::new_ibo(dev, 1, false, 1024, 0);
		dc.push(tex_white);
		dc.push(_vbo);
		dc.push(_ibo);
		_uac.init(pipe);
		white_set = _uac.write_image(tex_white);
		return 0;
	}



	motion_t* page_view::new_mot(int n, bool iscap)
	{
#ifdef _pmr_pool_nt
		motion_t* ret = (motion_t*)_alloc.allocate(sizeof(motion_t) * n, 16);
#else
		motion_t* ret = new motion_t[n];
#endif
		* ret = motion_t{};
		ubo_set_t binding = {};
		if (iscap)
		{
			auto t = ret;
			for (size_t i = 0; i < n; i++)
			{
				push_ubo(t++);
			}
		}
		//ubo_data.push_back(ret);
		return ret;
	}

	void page_view::clear_mot()
	{
#ifdef _pmr_pool_nt
		_alloc.release();
#endif
	}

	canvas_cx* page_view::new_canvas(bool is_draw)
	{
		auto p = new canvas_cx();
		dc.push(p);
		if (is_draw && p)
		{
			push_canvas(p);
		}
		return p;
	}

	void page_view::push_canvas(canvas_cx* p)
	{
		if (p)
		{
			auto_lock ak(&_mtx);
			_adc.push(p);
		}
	}

	void page_view::pop_canvas(canvas_cx* p)
	{
		if (p)
		{
			auto_lock ak(&_mtx);
			_fdc.push(p);
		}
	}

	ubo_set_t page_view::get_idxubo(canvas_cx* p)
	{
		ubo_set_t ret;
		auto it = _vpcubo.find(p);
		if (it != _vpcubo.end())
		{
			ret = it->second.us;
		}
		return ret;
	}

	void page_view::pop_image_tex(Image* img)
	{
		auto t = image_map.find(img);
		if (t != image_map.end())
		{
			if (meimg.find(t->second) != meimg.end())
				image_gc.push(t->second);
			image_map.erase(img);
		}
	}
	dvk_texture* page_view::new_yuvtex(yuv_info_t* py, upload_cx* up)
	{
		auto p = dvk_texture::new_yuv(*py, up ? up : p_upload);
		tex_free.insert(p);
		return p;
	}
	upload_cx* page_view::get_upptr()
	{
		return p_upload;
	}
	void page_view::push_image(Image* img)
	{
		dvk_texture* tex = image_map[img];
		if (!tex)
		{
			tex = get_image_tex(img);
		}

		{
			auto ihr = img->get_crc();
			//auto icrc = img->dcrc;
			//{
			//	static uint32_t ncrc = 0;
			//	if (ncrc != icrc)
			//	{
			//		std::string logstr = hz::format("\nget_crc: %s old crc:%d crc:%d\n", ihr ? "true" : "false", ncrc, icrc);
			//		ncrc = icrc;
			//		//ihr = true;
			//		//dslog(logstr.c_str(), logstr.size());
			//	}
			//}
			if (ihr)
			{
				tex->set_data(img, p_upload);
				//p_upload->flush();
				auto hr = p_upload->flushAndFinish();
				if (hr < 0)
				{
					std::string logstr = hz::format("\np_upload->flushAndFinish error:%d\n", hr);
					dslog(logstr.c_str(), logstr.size());
				}
			}
		}
		if (img && tex)
		{
			image_map[img] = tex;
			tex->user_data = img;
			_uac.write_image(tex);
		}
	}

	void page_view::push_ubo(motion_t* m)
	{
		if (m)
		{
			_uac.push_ubo(m);
		}
	}

	dvk_texture* page_view::get_image_tex(Image* img)
	{
		dvk_texture* p = nullptr;
		if (image_gc.size())
		{
			p = image_gc.front();
			image_gc.pop();
		}
		else {
			p = dvk_texture::new_image2d(img, p_upload);
			meimg.insert(p);
			dc.push(p);
		}
		return p;
	}

	// 写入vbo\ibo
	void page_view::update_data()
	{
		auto tid = get_tid();
		//log_format("update_data: %lld\n", tid);
		//return;
		int64_t vbosize = 0, ibosize = 0;
		{
			auto_lock ak(&_mtx);
			for (; _adc.size();)
			{
				auto p = _adc.top();
				_adc.pop();
				auto& u = _vpcubo[p];
				if (!u.us.dset)
				{
					// todo push_canvas
					//u = { new_ubo() ,glm::mat4(1.0) };
				}
				canvas_list.insert(p);
			}
			for (; _fdc.size();)
			{
				auto p = _fdc.top();
				_fdc.pop();
				_vpcubo.erase(p);
				canvas_list.erase(p);// remove(canvas_list.begin(), canvas_list.end(), p), canvas_list.end());
			}
		}
		for (auto dt : canvas_list)
		{
			auto ds = dt->data_size();
			dt->updt = false;
			int64_t nk = ds.x;
			nk *= ds.y;
			if (nk > 0)
			{
				dt->_vbo_offset = vbosize;
				dt->_ibo_offset = ibosize;
				vbosize += ds.x; ibosize += ds.y;
				dt->updt = true;
			}
		}

		if (vbosize > _vbo->_size)
		{
			_vbo->resize(vbosize + 8000);
			std::string logstr = "\nvbo resize:" + std::to_string(vbosize);
			logstr.push_back('\n');
			dslog(logstr.c_str(), logstr.size());
		}
		if (ibosize > _ibo->_size)
		{
			_ibo->resize(ibosize + 10000);
		}
		int64_t posv = 0, posi = 0;
		int vinc = 0, iinc = 0;
		njson kn;
		for (auto dt : canvas_list)
		{
			auto vs = dt->vsize();
			auto is = dt->isize();
#if 1
			posv = _vbo->set_data(dt->vdata(), vs, posv, false, true);
			posi = _ibo->set_data(dt->idata(), is, posi, false, true);

#else
			auto vc = Crc::crc32_dword(dt->vdata(), vs);
			auto ic = Crc::crc32_dword(dt->idata(), is);
			kn.push_back(vc);
			int ucp = vinc;
			if (dt->_vcrc != vc)
			{
				dt->_vcrc = vc;
				vinc++;
			}
			posv = _vbo->set_data(dt->vdata(), vs, posv, false, ucp != vinc);
			ucp = iinc;
			if (dt->_icrc != ic)
			{
				dt->_icrc = ic;
				iinc++;
			}
			posi = _ibo->set_data(dt->idata(), is, posi, false, ucp != iinc);
#endif
		}
		{
			static int64_t a = 0;
			if (a != vinc + iinc)
			{
				a = vinc + iinc;
				std::string logstr = hz::format("vbo size: %d, idx size: %d\n", (int)posv, (int)posi) + kn.dump(1);
				//dslog(logstr.c_str(), logstr.size());
			}
		}
		_vbo->flush();
		_ibo->flush();
		//_vbo->unmap();
		//_ibo->unmap();

		t_set<Image*> img_set;
		t_set<motion_t*> ubo_set;
		if (_uac.needed(canvas_list.size(), sizeof(glm::mat4)))
		{
			for (auto& [k, v] : _vpcubo) {
				v.us.size = 0;
			}
		}
		std::set<Image*> temv;
		// 更新视口
		auto clsize = canvas_list.size();
		auto ct = canvas_list.begin();
		for (size_t i = 0; i < clsize; i++)
		{
			auto it = *ct; ct++;
			auto& u = _vpcubo[it];

			if (!u.us.dset)
			{
				// todo 只有一个矩阵
				u.m = glm::mat4(1.0); it->vp0 = it->viewport;
			}
			if (!u.us.size)
			{
				u.us.size = sizeof(glm::mat4);
				_uac.mk_ubo(u.us);
			}
			auto& mt = it->vp0;
			if ((mt.z > 0 && mt.w > 0))
			{
				u.m = ortho(mt.z, mt.w); mt.z = mt.w = 0;
			}
			_uac._ubo->set_data(&u.m, sizeof(glm::mat4), u.us.offsets, false);
			// 配置纹理/图像/ubo
			for (auto tex : it->_tex_set) {
				if (tex)_uac.write_image(tex);
			}
			for (auto img : it->_img_set) { if (img) { push_image(img); temv.insert(img); } }
			for (auto ut : it->_ubo_set) { if (ut)push_ubo(ut); }

		}

		bool save_font_img = false;
		if (save_font_img)
		{
			for (auto it : temv)
			{
				it->saveImage("temp/font_img_pack_" + ecc_c::ptr2hex(it, true) + ".png");
			}
		}
		// 更新ubo
		_uac.update(_vpcubo);
		auto hr = p_upload->flushAndFinish();

		if (hr < 0)
		{
			std::string logstr = "\n 500line\tp_upload->flushAndFinish error:" + std::to_string(hr);
			logstr.push_back('\n');
			dslog(logstr.c_str(), logstr.size());
		}
		else
		{
			static size_t ik = -1;
			if (ik != temv.size())
			{
				ik = temv.size();
				std::string logstr = "\n img count:" + std::to_string(ik);
				logstr.push_back('\n');
				dslog(logstr.c_str(), logstr.size());
			}
		}
	}
	// float znear = 0.0f, zfar = 10000.0f;
	glm::mat4 page_view::ortho(float width, float height)
	{
		return _isdx ? glm::ortho(0.0f, width, height, 0.0f, znear, zfar) : glm::ortho(0.0f, width, 0.0f, height, znear, zfar);
	}

	// 格式化裁剪矩形
	glm::ivec4 page_view::mk_scissor(glm::vec4 src, canvas_cx* c)
	{
		glm::ivec4 st = c->viewport;
		//if (src.x < 0)
		//{
		//	src.x = 0;
		//	src.z += src.x;
		//}
		//if (src.y < 0)
		//{
		//	src.y = 0;
		//}
		if (src.z > st.z || src.z < 1)
		{
			src.z = st.z;
		}
		if (src.w > st.w || src.w < 1)
		{
			src.w = st.w;
		}
		src.x += st.x;
		src.y += st.y;
		return src;
	}

	void page_view::draw_call(dvk_cmd* cmd, canvas_cx** p, size_t count)
	{
		auto tid = get_tid();
		//log_format("draw_call: %p\t%lld\n", cmd, tid);
		auto dynamic_state = 0;
		auto pipe = _pipe;
		auto npipe = _pipe;
		for (size_t i = 0; i < count; i++)
		{
			auto dt = p[i];
			auto defset = get_idxubo(dt);
			auto vis = dt->data_size();
			cmd->bind_pipeline(_pipe, dt->depth_test, dt->depth_write);					// 绑定pipeline
			cmd->bind_vbo(_vbo->buffer, dt->_vbo_offset);			// 绑定顶点数据
			cmd->bind_ibo32(_ibo->buffer, dt->_ibo_offset);			// 绑定32位索引
			cmd->set_viewport(&dt->viewport, 1, 0);
			glm::ivec4 scissor_tem = dt->viewport;
			cmd->set_scissor(&scissor_tem, 0);
			uint64_t firstidx = 0;
			for (auto& ct : dt->cmd_data)
			{
				if (ct.indexCount == 0)continue;
				auto pipe1 = (pipeline_ptr_info*)ct.pipe;
				if (pipe1)
				{
					pipe = pipe1;
				}
				else {
					pipe = _pipe;
				}
				if (npipe != pipe || ct.dynamic_state != dynamic_state)
				{
					npipe = pipe;
					dynamic_state = ct.dynamic_state;
					if (dynamic_state & 1)
					{
						bool depth_test = (ct.dynamic_state & (uint32_t)dynamic_state_e::DepthTest)
							, depth_write = (ct.dynamic_state & (uint32_t)dynamic_state_e::DepthWrite);
						cmd->bind_pipeline(npipe, depth_test, depth_write);							// 绑定pipeline
						cmd->set_state(dynamic_state, 0, 0, ct.topology);
					}
					else {
						cmd->bind_pipeline(npipe, dt->depth_test, dt->depth_write);					// 绑定默认pipeline
						//uint32_t tds = 1;
						//if (dt->depth_test)
						//	tds |= (uint32_t)dynamic_state_e::DepthTest;
						//if (dt->depth_test)
						//	tds |= (uint32_t)dynamic_state_e::DepthWrite;
						//cmd->set_state(tds, 0, 0, 0);
					}
				}
				auto st = mk_scissor(ct.clipRect, dt);
				if (st != scissor_tem)
				{
					scissor_tem = st;
					cmd->set_scissor(&scissor_tem, 0);
				}
				auto set2 = _uac.get_set2(ct.ubo, ct.image.p.p);
				if (set2[0].dset)
					cmd->bind_set(set2[0]);
				else
					cmd->bind_set(defset);		// 使用默认ubo
				if (set2[1].dset)
					cmd->bind_set(set2[1]);
				else
					cmd->bind_set(white_set);	// 使用默认纹理
				// 索引绘图vkAcquireNextImageKHR返回VK_NOT_READY怎么解决
				//int nn = (ct.indexCount > 800) ? 1000 : 1;
				//for (size_t b = 0; b < nn; b++)
				{
					cmd->draw_indexed(ct.indexCount, ct.firstIndex + firstidx, ct.vertexOffset);
				}
				firstidx += ct.indexCount;
			}
		}
	}

	// !page_view



	//判断矩形相交

	bool math_cx::rect_cross(const glm::vec4& r1, const glm::vec4& r2)
	{
		return !((r1.y + r1.w < r2.y) || (r2.y + r2.w < r1.y) || (r1.x + r1.z < r2.x) || (r2.x + r2.z < r1.x));
	}

	int math_cx::distance(const glm::vec2& s, const glm::vec2& p)
	{
		int dis = (s.x - p.x) * (s.x - p.x) + (s.y - p.y) * (s.y - p.y);
		return dis;
	}

	bool math_cx::inCircle(const glm::vec3& c, const glm::vec2& p)
	{
		//计算点p和 当前圆圆心c 的距离
		int dis = distance(p, glm::vec2(c.x, c.y));
		auto r = c.z - 1;
		//和半径比较
		return (dis <= r * r);
	}

	bool math_cx::inRect(const glm::vec4& r, const glm::vec2& p, glm::vec2* o)
	{
		bool ret = !((p.x < r.x) || (p.y < r.y) || (p.x > r.x + r.z - 1) || (p.y > r.y + r.w - 1));
		if (o && ret) { o->x -= r.x; o->y -= r.y; }
		return ret;
	}
	// 判断坐标是否在一组热区
	size_t math_cx::inData(const t_vector<glm::vec4>* d, const t_string& type, const glm::vec2& p, t_set<int>* outidx)
	{
		bool is = false;
		size_t ic = 0;
		if (d)
		{
			if (outidx)outidx->clear();
			auto n = d->size();
			for (size_t i = 0; i < n; i++)
			{
				auto ch = type[i];
				auto it = d->at(i);
				if (ch == '3')
				{
					is = inCircle(*(glm::vec3*)&it, p);
				}
				else if (ch == '4')
				{
					is = inRect(it, p);
				}
				if (is)
				{
					if (outidx)
						outidx->insert(i);
					else
						break;
					ic++;
				}
			}
		}
		return ic;
	}
	size_t math_cx::in_rect(const t_vector<glm::vec4>* d, const glm::vec2& p, t_set<int>* outidx)
	{
		bool is = false;
		size_t ic = 0;
		if (d)
		{
			if (outidx)outidx->clear();
			auto n = d->size();
			for (size_t i = 0; i < n; i++)
			{
				auto it = d->at(i);
				is = inRect(it, p);
				if (is)
				{
					if (outidx)
						outidx->insert(i);
					else
						break;
					ic++;
				}
			}
		}
		return ic;
	}

#if 1

	event_data_cx::event_data_cx()
	{
		_maskcb.resize((int)event_type::et_max_num);
	}

	event_data_cx::~event_data_cx()
	{
		if (_em)
		{
			_em->pop(this);
		}
#ifdef _WIN32
		if (_oledrop)
		{
			delete _oledrop; _oledrop = 0;
		}
#endif
	}

	void event_data_cx::run_thr(event_data_cx* p)
	{
		while (p) { p->at_run(false); p = p->next; }
	}


	void event_data_cx::mket(event_type t)
	{
		switch (t)
		{
		case event_type::on_scroll:
			is_wheel = true;
			break;
		case event_type::on_drag:
		case event_type::on_dragstart:
		case event_type::on_dragend:
			is_drag = true;
			break;
		case event_type::on_dragenter:
		case event_type::on_dragleave:
		case event_type::on_dragover:
		case event_type::on_drop:
			is_drop = true;
			break;
		case event_type::on_ole_drag:
			is_oledrag = true;
			break;
		case event_type::on_ole_drop:
			mk_oledrop();
			break;
		default:
			break;
		}
		_maskcb[(int)t] = true;
	}
	void event_data_cx::set_cd(event_type t, uint64_t* result, uint64_t value)
	{
		auto& rt = _on_data[(uint32_t)t];
		rt[result] = value;
		_maskcb[(int)t] = true;
	}
	void event_data_cx::mk_oledrop()
	{
#ifdef _WIN32
		if (!_oledrop)
		{
			_oledrop = new drop_info_cx();
			_oledrop->on_drop_cb = [=](int type, int idx)
			{
				auto& it = _on_cbs[(int)event_type::on_ole_drop];
				for (auto& cb : it)
				{
					if (cb)	cb(0);
				}
			};
		}
#endif
	}
	std::string event_data_cx::get_dropstr()
	{
		std::string ret;
		if (_oledrop)
		{
			ret = _oledrop->_str;
		}
		return ret;
	}
	void event_data_cx::set_dragable(bool is)
	{
		is_auto_drag = is;
	}
	bool event_data_cx::get_dragable()
	{
		return is_auto_drag;
	}
	void event_data_cx::set_v4(size_t idx, const glm::vec4& c, bool is)
	{
		//auto_lock ak(&_mtx);
		int n = idx;
		if (n >= 0)
		{
			if (n >= _hot.size())
			{
				_hot.resize(n + 1); _ht.resize(n + 1);
			}
			_hot[n] = c;
			_ht[n] = is ? '4' : '3';
			lidx = -1;
		}
	}
	glm::vec4 event_data_cx::get_v4(size_t idx)
	{
		if (idx == lidx)
		{
			return lv4;
		}
		//auto_lock ak(&_mtx);
		if (_hot.empty() || idx >= _hot.size())
			return glm::vec4();
		lv4 = _hot[idx]; lidx = idx;
		return lv4;
	}
	void event_data_cx::clear_hot()
	{
		_hot.clear();
		_ht.clear();
	}
	size_t event_data_cx::add_rect(const glm::vec4& c)
	{
		size_t n = _hot.size();
		_hot.push_back(c);
		_ht.push_back('4');
		return n;
	}
	size_t event_data_cx::add_circle(const glm::vec4& c)
	{
		size_t n = _hot.size();
		_hot.push_back(c);
		_ht.push_back('3');
		return n;
	}
	void event_data_cx::at_run(bool is_one)
	{
		if (!ay_tid)
			ay_tid = get_tid();
		for (; exe_cbs.size();)
		{
			vcb_t c = {};
			{
				auto_lock ak(&_mtx);
				c = exe_cbs.front();
				exe_cbs.pop();
			}
			if (c)
			{
				c();
			}
			if (is_one)
				break;
		}
		for (; event_runcb.size();)
		{
			event_km_t e = {};
			{
				auto_lock ak(&_mtx);
				e = event_runcb.front();
				event_runcb.pop();
			}
			call_acb(&e, true);
			if (is_one)
				break;
		}
	}
	size_t event_data_cx::size()
	{
		return _hot.size();
	}
	int event_data_cx::get_hot_count()
	{
		auto_lock ak(&_mtx);
		return _hotidx.size();
	}
	int event_data_cx::get_begin()
	{
		auto_lock ak(&_mtx);
		if (_hotidx.empty())
		{
			return -1;
		}
		return *(_hotidx.begin());
	}
	int event_data_cx::get_rbegin()
	{
		auto_lock ak(&_mtx);
		if (_hotidx.empty())
		{
			return -1;
		}
		return *(_hotidx.rbegin());
	}
	void event_data_cx::post_vcb(vcb_t cb)
	{
		auto_lock ak(&_mtx);
		exe_cbs.push(cb);
	}
	void event_data_cx::send_vcb(vcb_t cb)
	{
		auto t = get_tid();
		bool isret = true;
		if (t != ay_tid)
		{
			auto_lock ak(&_mtx);
			exe_cbs.push([&]() { cb(); isret = false; });
		}
		else
		{
			cb();
			isret = false;
		}
		while (isret)
		{
			sleep(1);
		}
	}
	//void event_data_cx::add_exetype(event_type_t e)
	//{
	//	auto_lock ak(&_mtx);
	//	_thread_exetype.insert(e);
	//}
	//void event_data_cx::add_exetype(const t_vector<event_type_t>& e)
	//{
	//	auto_lock ak(&_mtx);
	//	for (auto it : e)
	//		_thread_exetype.insert(it);
	//}
	void event_data_cx::push_event(event_km_t* e)
	{
		auto_lock ak(&_mtx);
		event_runcb.push(*e);
	}
	void event_data_cx::send_event(event_km_t* e)
	{
		auto t = get_tid();
		bool isret = true;
		if (t != ay_tid)
		{
			auto_lock ak(&_mtx);
			exe_cbs.push([&]() { call_acb(e); isret = false; });
		}
		else
		{
			call_acb(e);
			isret = false;
		}
		while (isret)
		{
			sleep(1);
		}
	}
	bool event_data_cx::is_atexe(event_type_t e)
	{
		event_type_t ne = event_type::on_ole_drag;
		return is_athread && e < ne;
	}
	void event_data_cx::call_cb(event_km_t* e, bool is_data)
	{
		if (!(e->etype < _maskcb.size() && _maskcb[e->etype]))
		{
			return;
		}
		bool isa = is_atexe(e->etype);
		if (isa)
		{
			if (e->form)
			{
				push_event(e);	// 设置事件在自定义线程执行			
			}
			else {
				send_event(e);
			}
		}
		else {
			call_acb(e, is_data);
		}
	}
	void event_data_cx::call_acb(event_km_t* e, bool is_data)
	{
		// 执行回调函数
		const auto& it = _on_cbs.find(e->etype);
		if (it != _on_cbs.end())
		{
			for (auto& cb : it->second)
				if (cb)cb(e);
		}
		// 更新数据
		if (is_data && e->etype >= (uint32_t)event_type::on_click)
		{
			auto dt = _on_data.find(e->etype);
			if (dt != _on_data.end())
			{
				for (auto& [k, v] : dt->second)
				{
					*k = v;
				}
				if (dt->second.size())
				{
					_is_upcd = true;
				}
			}
			if (e->_bubble && _parent)
				_parent->call_cb(e, is_data);
		}
	}

	bool event_data_cx::is_upcd()
	{
		bool ret = _is_upcd;
		_is_upcd = false;
		return ret;
	}


	bool event_data_cx::is_click()
	{
		return is_type2on(event_type::on_click);
	}

	bool event_data_cx::ishot()
	{
		return curr_drag || _is_hot;
	}
	void event_data_cx::set_drag_data(void* data, size_t size)
	{
		_drag_data = data;
		_drag_size = size;
	}
	bool event_data_cx::is_dblclick()
	{
		return is_type2on(event_type::on_click);
	}


	bool event_data_cx::is_tripleclick()
	{
		return is_type2on(event_type::on_click);
	}
	bool event_data_cx::is_type2on(event_type et)
	{
		auto it = _on_cbs.find((uint32_t)et);
		return (it != _on_cbs.end());
	}

	void event_data_cx::on_scroll(int x, int y, event_km_t* e)
	{
		if (is_wheel && is_type2on(event_type::on_scroll))
		{
			if (y < 0)
			{
				wheel_info._nWheel += wheel_info._step;
				if (wheel_info._nWheel > wheel_info._maxW)
				{
					wheel_info._nWheel = wheel_info._maxW;
				}
			}
			else  if (y > 0)
			{
				wheel_info._nWheel -= wheel_info._step;
				if (wheel_info._nWheel < wheel_info._minW)
				{
					wheel_info._nWheel = wheel_info._minW;
				}
			}
			auto te = *e;
			te.etype = (uint32_t)event_type::on_scroll;
			call_cb(&te);
		}
	}
	bool event_data_cx::is_contains(glm::ivec2 pos)
	{
		if (cm_pos == pos) {
			//printf("重复%p\t%d\n", this, _is_hot);
			//return false;
		}
		if (!enabled)return false;
		cm_pos = pos;
		pos -= node_pos;	//格式化为组件坐标
		auto_lock ak(&_mtx);
		auto n = math_cx::inData(&_hot, _ht, pos, &_hotidx);
		_is_hot = n > 0;
		return n > 0;
	}
	// --------------------------------------------------------------------------------------------
	event_master::event_master()
	{
#ifdef _WIN32
		_dbt = GetDoubleClickTime();//获取系统双击间隔时间
#endif // _WIN32

		_hover_act = new timer_cx(_hover_ms, [](event_master* t)
			{
				if (t && t->_hotnode)
				{
					auto tem = t->te;
					tem.etype = (int)event_type::on_hover;
					t->_hotnode->call_cb(&tem);
					t->_hover_act->stop();
				}
			}, this);
	}

	event_master::~event_master()
	{
		delop(_hover_act);
	}
	void event_master::set_bwctx(bw_sdl* p)
	{
		assert(_bw_ctx != p);
		_bw_ctx = p;
		if (p)
		{
			//_bw_ctx->push_timer(_hover_act);
		}
	}
	bw_sdl* event_master::get_bwctx()
	{
		return _bw_ctx;
	}
	void event_master::push(event_data_cx* p)
	{
		if (p)
		{
			if (p->_em)
				p->_em->pop(p);
			auto_lock ak(&_mtx);
			_tem_ar.push({ p,true });
		}
	}
	void event_master::pop(event_data_cx* p)
	{
		if (p)
		{
			auto_lock ak(&_mtx);
			_tem_ar.push({ p, false });
			p->_em = 0;
		}
	}
	void event_master::up_sort()
	{
		sort_inc++;
	}
	void event_master::run_newdel()
	{
		if (_tem_ar.size())
		{
			sort_inc++;
			auto_lock ak(&_mtx);
			for (; _tem_ar.size();) {
				auto it = _tem_ar.front();
				auto np = it.p;
				if (it.ispush) {
					if (np->_oledrop)
						nodes_drop.insert(np);
					nodes.push_back(np);
					np->_em = this;
				}
				else
				{
					nodes.erase(remove(nodes.begin(), nodes.end(), np), nodes.end());
					nodes_drop.erase(np);
				}
				_tem_ar.pop();
			}

		}

		if (sort_inc != 0 && nodes.size())
		{
			sort_inc = 0;
			std::sort(nodes.begin(), nodes.end(), [](event_data_cx* p1, event_data_cx* p2) { return (p1->order < p2->order); });
		}
	}
	// 设置视图区域

	void event_master::set_viewport(glm::ivec4 viewport)
	{
		_viewport = viewport;
	}

#define VMK_UP 			HZ_BIT(0) //鼠标左键弹起
#define VMK_DOWN	 	HZ_BIT(1) //按下
#define VMK_MOVE 		HZ_BIT(2) //移动
#define VMK_ENTER 		HZ_BIT(3) //进入
#define VMK_LEAVE 		HZ_BIT(4) //离开
#define VMK_FOCUS 		HZ_BIT(5) //焦点
#define VMK_DISABLE 	HZ_BIT(6) //禁用
#define VMK_LBUTTON 	HZ_BIT(7) //鼠标左键
#define VMK_RBUTTON 	HZ_BIT(8) //鼠标右键
#define VMK_CLICK 		HZ_BIT(9) //单击
#define VMK_DBLCLK 		HZ_BIT(10) //双击
#define VMK_RDOWN 		HZ_BIT(11) //右键按下
#define VMK_RUP 		HZ_BIT(12) //右键弹起

	drop_info_cx* event_master::call_mouse_move_oledrop(int x, int y)
	{
		x -= _viewport.x;
		y -= _viewport.y;
		te.pos = { x, y };
		t_vector<event_data_cx*> n_hotnode;
		for (auto it : nodes_drop)
		{
			if (it)
			{
				if (it->is_contains({ x, y }))
				{
					n_hotnode.push_back(it);
				}
			}
		}
		drop_info_cx* ret = 0;
		if (n_hotnode.size())
		{
			_hover_act->runcb();
			ret = n_hotnode[0]->_oledrop;
			_olehotnode = n_hotnode[0];
			// 热区内移动
			auto tem = te;
			tem.etype = (int)event_type::on_ole_dragover;
			_olehotnode->call_cb(&tem);
			if (!tem.user_ret)
			{
				ret = 0;
			}
			//printf("hot move %d\n", (int)n_hotnode.size());
		}
		return ret;
	}
	bool event_master::is_contains_hit(glm::vec2 pos)
	{
		event_km_t ekm[1] = {};
		ekm->etype = (uint32_t)event_type::mouse_move;
		bool ret = call_mouse_move(pos.x, pos.y, 0);
		return math_cx::inRect(_viewport, pos, &pos) && ret;
	}
	bool event_master::is_contains(glm::vec2 pos)
	{
		bool isin = math_cx::inRect(_viewport, pos, &pos);
		if (!isin)
		{
			_hover_act->stop();
			if (_lasthotnode)
			{
				if (_lasthotnode->_nMouseState & VMK_DOWN)
				{
					return true;
				}
				else {
					onMouseLeave(_lasthotnode, 0);
					_lasthotnode = 0;
				}
			}
		}
		return isin;
	}
	// 鼠标进入才会执行
	bool event_master::call_mouse_move(int x, int y, event_km_t* e)
	{
		run_newdel();
		glm::ivec2 pos = { x - _viewport.x, y - _viewport.y };
		if (e)
		{
			e->pos = pos;
			e->pos1 = pos;
			te = *e;
		}
		if (pos != last_mouse)
		{
			last_mouse = pos;
		}
		t_vector<event_data_cx*> n_hotnode;
		int mcc = 0;
		do {
			for (auto rit = nodes.rbegin(); rit != nodes.rend(); rit++)
			{
				auto it = *rit;
				bool ismc = false;
				if (it && it->visible && (!it->pvisible || *(it->pvisible)))
				{
					if (it->is_contains({ pos.x, pos.y }))
					{
						n_hotnode.push_back(it);
						if (e)
							it->call_cb(e);
						ismc = true;
						mcc += it->mchild_count;
					}
					if (it->mchild_count > 0)
					{
						hover_vn.insert(it);
					}
					else
					{
						hover_vn.erase(it);
					}
					if (mcc > 0)
					{
						break;
					}
				}
				else {
					_hotnode = _hotnode;
				}
			}
			if (n_hotnode.size())
				_hotnode = n_hotnode[0];
			else
				_hotnode = 0;
		} while (0);
		if (e)
			onMouseMove(pos.x, pos.y);
		return _hotnode || mcc > 0;
	}


	void event_master::call_mouse_down(int idx, int x, int y, event_km_t* e)
	{
		x -= _viewport.x;
		y -= _viewport.y;
		e->pos1 = e->pos = { x, y };
		for (auto rit = nodes.rbegin(); rit != nodes.rend(); rit++)
		{
			auto it = *rit;
			if (it)
			{
				it->call_cb(e);
			}
		}
		onMouseDown(idx, x, y);
	}

	void event_master::call_mouse_up(int idx, int x, int y, event_km_t* e)
	{
		//printf("up:%p", e->p);
		x -= _viewport.x;
		y -= _viewport.y;
		e->pos1 = e->pos = { x, y };
		for (auto rit = nodes.rbegin(); rit != nodes.rend(); rit++)
		{
			auto it = *rit;
			if (it)
			{
				it->call_cb(e);
			}
		}
		onMouseUp(idx, x, y);
	}

	void event_master::call_mouse_wheel(int x, int y, event_km_t* e)
	{
		for (auto rit = nodes.rbegin(); rit != nodes.rend(); rit++)
		{
			auto it = *rit;
			if (it)
			{
				it->call_cb(e);
			}
		}
		if (_hotnode)
		{
			_hotnode->on_scroll(x, y, e);
		}
	}

	void event_master::call_key(event_km_t* e)
	{
		if (_keynode)
		{
			auto tem = *e;
			_keynode->call_cb(&tem);
		}
	}

	void event_master::call_input(event_km_t* e)
	{
		if (_keynode)
		{
			auto tem = *e;
			_keynode->call_cb(&tem);
		}
	}


	glm::ivec4 drag_info_t::re_rect()
	{
		glm::ivec4& ret = _rect;
		ret.x = std::min(_pos_down.x, _pos_end.x);
		ret.y = std::min(_pos_down.y, _pos_end.y);
		ret.z = std::max(_pos_down.x, _pos_end.x);
		ret.w = std::max(_pos_down.y, _pos_end.y);
		return ret;
	}
	// x,y鼠标坐标，pos对象坐标
	void drag_info_t::set_drag_begin(int x, int y, const glm::ivec2& pos)
	{
		_pos_down = _firstpos = { x - pos.x, y - pos.y };
	}
	void drag_info_t::set_drag_end(int x, int y, const glm::ivec2& pos)
	{
		_pos_end = { x - pos.x, y - pos.y };
		re_rect();
	}


	void event_master::on_oledrag(event_data_cx* node)
	{
		auto tem = te;
		if (node->is_oledrag && node->curr_oledrag == 1 && node->_is_hot)
		{
			node->curr_oledrag++;
			tem.etype = (int)event_type::on_ole_drag;
			node->call_cb(&tem);//执行拖动处理
			//node->_nMouseState = VMK_UP;
		}
	}
	void event_master::on_drag(int x, int y)
	{
		if (!_dragnode)return;
		//printf("on_drag: %d\n", _dragnode->child_count);
		auto tem = te;
		if (_dragnode->curr_drag > 0)
		{
			glm::vec2 pos = { x - drag_info._firstpos.x, y - drag_info._firstpos.y };
			drag_info.set_drag_end(x, y, _dragnode->node_pos);
			tem.pos1 = pos;
			if (_dragnode->curr_drag == 1)
			{
				tem.etype = (int)event_type::on_dragstart;
				drag_info.first_pos = pos;
				_dragnode->call_cb(&tem);//执行拖动开始
			}
			_dragnode->curr_drag++;
			tem.etype = (int)event_type::on_drag;
			if (!_dragnode->child_count && _dragnode->is_auto_drag)
			{
				_dragnode->node_pos = pos;
				//printf("on_drag: %d,%d\t%d,%d\n", x, y, (int)pos.x, (int)pos.y);
			}
			tem.pos2 = drag_info.first_pos;
			_dragnode->call_cb(&tem);//执行拖动处理
		}
	}
	void event_master::onMouseMove(int x, int y)
	{
		auto tem = te;
		on_drag(x, y);
		if (_hotnode)
		{
			_hotnode->_nMouseState |= VMK_MOVE;
			if (_hotnode->_nMouseState & VMK_DOWN && te.mouse_idx == 0)
			{
				on_oledrag(_hotnode);
			}
			if (_lasthotnode != _hotnode)
			{
				if (_lasthotnode && !_lasthotnode->ishot())
				{
					onMouseLeave(_lasthotnode, event_type::on_move);
				}
				_lasthotnode = _hotnode;
				onMouseEnter(_hotnode);
				// 有悬停事件则开始定时器
				if (_hotnode->is_type2on(event_type::on_hover))
					_hover_act->start();
			}
			else
			{
				// 热区内移动
				tem.etype = (int)event_type::on_move;
				_hotnode->call_cb(&tem);
				if (_dragnode && _hotnode != _dragnode)
				{
					tem.etype = (int)event_type::on_dragover;
					_hotnode->call_cb(&tem);
				}
			}
		}
		else
		{

			_hover_act->stop();
			if (_lasthotnode)
			{
				auto hp = _lasthotnode;
				if ((hp->_nMouseState & VMK_DOWN))
				{
					// 鼠标按下时也会触发热区内移动
					tem.etype = (int)event_type::on_move;
					hp->call_cb(&tem);
				}
				else {
					_lasthotnode = 0;
				}
				onMouseLeave(hp, event_type::mouse_move);
			}
		}
	}

	void event_master::onMouseEnter(event_data_cx* node)
	{
		node->_nMouseState &= ~VMK_LEAVE;

		node->_nMouseState |= VMK_ENTER;
		auto tem = te;
		tem.etype = (int)event_type::on_enter;
		node->call_cb(&tem, !(node->_nMouseState & VMK_DOWN));
		if (_dragnode && node != _dragnode)
		{
			tem.etype = (int)event_type::on_dragenter;
			node->call_cb(&tem);
		}
	}
	void event_master::onMouseLeave(event_data_cx* node, event_type_t t)
	{
		if (node->_nMouseState & VMK_DOWN)
		{
			//printf("%p,down\t%d\n", node, (int)t);
		}
		if (!(node->_nMouseState & VMK_ENTER) || (node->_nMouseState & VMK_DOWN))
		{
			return;
		}
		//printf("%p,onMouseLeave\n", node);
		node->_nMouseState &= ~VMK_ENTER;
		node->_nMouseState &= ~VMK_MOVE;
		node->_nMouseState |= VMK_LEAVE;
		//执行事件
		auto tem = te;
		tem.etype = (int)event_type::on_leave;
		node->call_cb(&tem, !(node->_nMouseState & VMK_DOWN));

		if (_dragnode && node != _dragnode)
		{
			tem.etype = (int)event_type::on_dragleave;
			node->call_cb(&tem);
		}
		node->_nMouseState &= ~VMK_DOWN;
		node->_nMouseState &= ~VMK_RDOWN;
		node->_nMouseState |= VMK_UP;
	}

	void event_master::onMouseDown(int idx, int x, int y)
	{
		if (!_hotnode)return;
		if (_hotnode->_is_hot)
		{
			_hotnode->_nMouseState |= VMK_DOWN;
			_hotnode->_nMouseState &= ~VMK_UP;
			//执行事件
			te.mouse_idx = idx;
			auto tem = te;
			tem.etype = (int)event_type::on_down;
			_hotnode->call_cb(&tem);
			if (idx == 0)
			{
				if (_hotnode->is_drag || _hotnode->is_auto_drag)
				{
					_hotnode->curr_drag = 1;
					_dragnode = _hotnode;
					drag_info.set_drag_begin(x, y, _hotnode->node_pos);
				}
				if (_hotnode->is_oledrag)
				{
					_hotnode->curr_oledrag = 1;
				}
				if (te.form && _hotnode->pridata)
				{
					te.form->set_now_ui(_hotnode->pridata);
				}
				// 切换焦点
				if (_keynode)
					_keynode->is_input = false;
				_keynode = _hotnode;
				_keynode->is_input = true;
			}
		}
	}
	void event_master::ondragend(event_data_cx* node, event_km_t& te, int x, int y)
	{
		if (node && node->curr_drag)
		{
			node->curr_drag = 0;
			drag_info.set_drag_end(x, y, node->node_pos);
			te.etype = (int)event_type::on_dragend;
			node->call_cb(&te);//执行拖动结束
		}
		if (_hotnode && _hotnode->curr_drop && _hotnode->_is_hot && te.mouse_idx == 0)
		{
			_hotnode->curr_drop = false;
			te.etype = (int)event_type::on_drop;
			_hotnode->call_cb(&te);//接收拖动结束
		}
	}
	void event_master::mulit_click(event_data_cx* node)
	{
		if (!node->_is_hot)return;
		node->_nMouseState |= VMK_CLICK;
		{
			auto tem = te;
			tem.etype = (int)event_type::on_click; // 默认单击
			int idx = te.mouse_idx;
			//printf("up\n%d,%p\n\n", idx, _lasthotnode);
			do
			{
				if (idx == 0 && (node->is_dblclick() || node->is_tripleclick()))
				{
					int es = _t.elapsed();
					if (es < _dbt)	//判断时间是否小于多击间隔
					{
						_cks++;
						node->_nMouseState &= ~VMK_DOWN;
						if (_cks == 2)
						{
							tem.etype = (int)event_type::on_dblclick;		//双击
						}
						if (_cks == 3)
						{
							tem.etype = (int)event_type::on_tripleclick;	//三击

						}
						break;
					}
				}
				_cks = 1;
			} while (0);
			if (_cks > 0)
			{
				_t.reset();
				node->call_cb(&tem);	// 执行点击事件
			}
		}
		node->_nMouseState &= ~VMK_CLICK;
	}
	void event_master::on_node_up(event_data_cx* node, int idx)
	{
		if (!node)return;
		node->_nMouseState |= VMK_UP;

		auto tem = te;
		tem.etype = (int)event_type::on_up;	// 不在区域内也可以触发
		node->call_cb(&tem);
		if (node != _hotnode)
			node->_nMouseState &= ~VMK_DOWN;

	}
	void event_master::onMouseUp(int idx, int x, int y)
	{
		te.mouse_idx = idx;
		auto tem = te;
		on_node_up(_dragnode, idx);
		ondragend(_dragnode, tem, x, y);
		if (_lasthotnode != _dragnode)
			on_node_up(_lasthotnode, idx);
		_dragnode = 0;
		if (_hotnode)
		{
			_hotnode->_nMouseState |= VMK_UP;
			if (_hotnode->_nMouseState & VMK_DOWN)
			{
				mulit_click(_hotnode);
			}
			_hotnode->_nMouseState &= ~VMK_DOWN;

		}
		_lasthotnode = 0;
	}


	timer_pex::timer_pex()
	{
		init_task();
	}

	timer_pex::~timer_pex()
	{
	}
	void timer_pex::init_task()
	{
		insert1(this, &timer_pex::call_cb);
	}
	bool timer_pex::call_cb()
	{
		return false;
	}


#endif // 1
	// 数组列表事件
#if 1

	table_view_e::table_view_e()
	{
	}

	table_view_e::~table_view_e()
	{
	}

	void table_view_e::init_row(int n, int height)
	{
	}

	void table_view_e::set_row(int idx, int height)
	{
	}


	//block_view::block_view()
	//{
	//}

	//block_view::~block_view()
	//{
	//}

	//void block_view::push_page(page_node* p)
	//{
	//	if (p)
	//		_pages.push_back(p);
	//}

#endif // 1
	gui_ctx::gui_ctx(queue_ctx* qc) :qctx(qc)
	{
	}

	gui_ctx::~gui_ctx()
	{
	}

	void gui_ctx::init(bw_sdl* bw, vk_render_cx* vkr, dvk_device* dev)
	{
		sdl_ctx = bw;
		_vkr = vkr;
		_dev = dev;
		qctx->init(dev, 0);
		bw->set_rts(qctx->get_rts());
	}


	void gui_ctx::push_form(sdl_form* form)
	{
		if (form)
		{
			qctx->push(form);
		}
	}

	void gui_ctx::begin_thr()
	{
		qctx->begin_thr();
	}

	void gui_ctx::wait_end()
	{
		qctx->stop();
		while (!qctx->_end)
			sleep(1);
	}
	bool gui_ctx::loop()
	{
		if (!qctx->_onethread)
			qctx->loop();
		return qctx->_onethread;
	}

	void gui_ctx::set_update_cb(std::function<void(cpg_time*)> update_cb, std::function<void()>init_cb)
	{
		qctx->update_cb1 = update_cb;
		qctx->init_cb = init_cb;
	}
	void gui_ctx::set_waitms(int ms, int minfps)
	{
		if (qctx)
		{
			qctx->set_waitms(ms, minfps);
		}
	}
	// todo 文本域
#if 1

	struct text_area_t
	{
		text_info_t ct;			// 文本渲染信息
		text_extent ext;		// 文本宽高拾取结构信息

		// 每个行实体row_info_t
		t_vector<entity_t> row_entity;
		glm::ivec2	text_pos;	// 文本渲染坐标
		int	row_height = 0;		// 行高
		int	row_y = 0;			// 字高
		int	row_hy2 = 0;		// (row_height - row_y) * 0.5
		int row_base = 0;		// 起始行号
		int row_n = 0;			// 行数量
		str2_t select_str = {};	// 选中的字符串
	};

	text_area_cx::text_area_cx()
	{
		_reg = dc.ac<entt::registry>();
		//_listen._hotidx = &_hotidx;
		_cursor_blink_entity = new_cursor_blink();
		// 设置事件在渲染线程执行
		_listen.set_athread();
		_listen.set_cb(event_type::on_down, this, &text_area_cx::on_down);
		_listen.set_cb(event_type::on_move, this, &text_area_cx::on_move);
		_listen.set_cb(event_type::on_up, this, &text_area_cx::on_up);
		_listen.set_cb(event_type::on_leave, this, &text_area_cx::on_leave);
		_listen.set_cb(event_type::on_enter, this, &text_area_cx::on_enter);
		_listen.set_cb(event_type::on_dragstart, this, &text_area_cx::on_dragstart);
		_listen.set_cb(event_type::on_ole_drop, this, &text_area_cx::on_ole_drop);
		_listen.set_cb(event_type::on_ole_drag, this, &text_area_cx::on_ole_drag);
		_listen.set_cb(event_type::on_ole_dragover, this, &text_area_cx::on_ole_dragover);
		_listen.set_cb(event_type::on_input, this, &text_area_cx::on_input);
		_listen.set_cb(event_type::on_keypress, this, &text_area_cx::on_key);
	}

	text_area_cx::~text_area_cx()
	{
	}

	entity_t text_area_cx::new_cursor_blink()
	{
		entity_t ret = _reg->create();		//创建实体
		_reg->emplace<cursor_blink_t>(ret);	//绑定空数据
		return ret;
	}
	entity_t text_area_cx::add_paragraph(size_t n)
	{
		entity_t ret = {};
		assert(_reg);
		if (n > 0)
		{
			auto& v = _area_entity;
			size_t idx = v.size();
			v.resize(v.size() + n);
			auto d1 = v.data();
			_reg->create(d1, d1 + n);
			for (size_t i = 0; i < n; i++)
			{
				_reg->emplace<text_area_t>(d1[i]);
			}

			ret = v[idx];
		}

		return ret;
	}
	entity_t text_area_cx::add_row(t_vector<entity_t>& row_entity, size_t n)
	{
		entity_t ret = {};
		assert(_reg);
		if (n > 0)
		{
			auto& v = row_entity;
			size_t idx = v.size();
			v.resize(v.size() + n);
			auto d1 = v.data();
			_reg->create(d1, d1 + n);
			for (size_t i = 0; i < n; i++)
			{
				row_info_t ed = {};
				ed._selection = _reg->create();
				_reg->emplace<row_info_t>(d1[i], ed);
			}
			ret = v[idx];
		}
		return ret;
	}

	cursor_blink_t& text_area_cx::get_cursor_blink(entity_t e)
	{
		return *ecs::mk_data<cursor_blink_t>(_reg, e);
	}

	rect_info_t& text_area_cx::get_rect(entity_t e)
	{
		return *ecs::mk_data<rect_info_t>(_reg, e);
	}
	void text_area_cx::mk_row(t_vector<entity_t>& row_entity, size_t idx)
	{
		if (idx >= row_entity.size())
		{
			add_row(row_entity, 1 + idx - row_entity.size());
		}
	}
	row_info_t* text_area_cx::get_row(t_vector<entity_t>& row_entity, size_t idx)
	{
		assert(idx < row_entity.size());
		return ecs::mk_data<row_info_t>(_reg, row_entity[idx]);
	}

	void text_area_cx::set_text(entity_t id, css_text* csst, glm::ivec2 pos, const char* str, size_t count, size_t first, bool is_up)
	{
		auto p = ecs::mk_data<text_area_t>(_reg, id);
		auto& it = *p;
		auto ts1 = utf8_char_pos(str, first);
		if (count == -1)
			count = get_utf8_count(ts1, strlen(ts1));
		it.ct.count = count;
		it.ct.csst = csst;
		it.ct.str = str;
		it.ct.str_b = ts1;
		it.ct.first_idx = first;
		if (is_up)
		{
			print_time exs("get_extent_str");
			it.ext.clear();
			csst->ft->get_extent_str(csst, str, count, first, &it.ext);// 获取n个字符宽高rect_info_t
			it.row_height = csst->row_height;
			it.row_y = csst->row_y;
			it.row_hy2 = (csst->row_height - csst->row_y) * 0.5;
			it.row_n = it.ext._line.size();
			size_t i = 0, n = _area_entity.size();
			for (i = 0; i < n; i++)
			{
				if (id == _area_entity[i])
					break;
			}
			if (i < n)
			{
				glm::ivec4 v4 = { pos.x, pos.y, it.ext.x1(), it.ext.y1() };
				_listen.set_v4(i, v4, true);
			}
			it.text_pos = pos;
			pos.y += it.row_hy2;
			it.ct.pos = pos;
			it.row_base;
			{
				mk_row(it.row_entity, it.row_n);
				for (size_t i = 0; i < it.row_n; i++)
				{
					auto rtp = get_row(it.row_entity, i);
					rtp->_size.x = it.ext._y_width[i];
					rtp->_size.y = it.row_height;
					rtp->_pos = { it.text_pos.x, it.text_pos.y + it.row_height * i };
				}
			}
		}
		ecs::mk_edata<display_et>(_reg, id);
	}

	glm::ivec4 text_area_cx::on_first_pos(glm::ivec2 pos, int* outlast, text_area_t** op, bool is_clear_select, bool isupcursor, bool* israng)
	{
		glm::ivec4 ret;
		if (is_clear_select)
		{
			is_select = false;
			auto view_selection = _reg->view<selection_et, rect_info_t>();
			for (const entt::entity e : view_selection) {
				if (_reg->has<selection_et>(e))
					_reg->remove<selection_et>(e);
			}
		}
		auto lt = _listen.get_begin();
		if (lt < 0)
		{
			return ret;
		}
		int hidx = lt;
		{
			auto p = ecs::mk_data<text_area_t>(_reg, _area_entity[hidx]);
			if (op)
			{
				*op = p;
			}
			auto& it = *p;
			glm::ivec2 cpos;
			ret = it.ext.get_pos_idx({ pos.x - it.text_pos.x, pos.y - it.text_pos.y }, israng, &cpos);
			if (!(ret.x < 0 || ret.y < 0))
			{
				ret.y += it.row_base;
				auto& dpc = get_cursor_blink(_cursor_blink_entity);
				auto pc = dpc;
				pc.pos.x = ret.z + it.text_pos.x; pc.pos.y = ret.w + it.text_pos.y;
				pc.height = it.row_height;
				if (israng && !(*israng))
				{
					isupcursor = false;
				}
				if (isupcursor)
				{
					p->ext.set_cpos(cpos);
					p->ext.set_cpos(cpos, 1);
					dpc = pc;

				}
				//printf("pos\t%d,%d\n", e->pos.x, e->pos.y);
			}
		}
		if (outlast)*outlast = hidx;
		return ret;
	}


	void text_area_cx::on_down(event_km_t* e) {
		assert(_listen.get_hot_count());
		print_time exs("get_pos_idx");
		pta_second = 0; is_cs = true;
		if (is_select && in_selection(e->pos))
		{
			is_dragstart = true;
			return;
		}
		pos_first = on_first_pos(e->pos, &lasthot, &pta_first, true, true);
	}

	void text_area_cx::on_up(event_km_t* e)
	{
		bool isds = is_dragstart;
		is_dragstart = false;
		if (is_cs)
		{
			_selection_hot.clear();
			auto view_selection = _reg->view<selection_et, rect_info_t>();
			// 选择框
			for (const entt::entity e : view_selection) {
				auto vsr = &view_selection.get<rect_info_t>(e);
				_selection_hot.push_back(vsr->a);
			}
			is_select = !_selection_hot.empty();
		}
		if (!pta_second)
		{
			on_first_pos(e->pos, 0, 0, is_cs, is_cs);
		}
		//printf("on up %d\n", (int)_selection_hot.size());
	}

	void text_area_cx::on_enter(event_km_t* e)
	{
		_sc = (uint8_t)mouse_cursor::SC_IBEAM;	// 输入光标
		if (is_select)
		{
			bool ins = in_selection(e->pos);
			_sc = (uint8_t)(ins ? mouse_cursor::SC_ARROW : mouse_cursor::SC_IBEAM);
		}
	}
	void text_area_cx::on_dragstart(event_km_t* e)
	{

	}
	void text_area_cx::on_ole_drop(event_km_t* e)
	{
		auto kstr = _listen.get_dropstr();
		kstr = u8_gbk(kstr);
		//printf("接收\t%s\n", kstr.c_str());

	}
	void text_area_cx::on_ole_drag(event_km_t* e)
	{
		if (is_select && is_dragstart)
		{
			//printf("开始拖动\n");
#ifdef _WIN32
			std::wstring olestr = u8_w(selection_str);
			do_dragdrop_begin(olestr); // 会在消息线程执行

			is_drag_me = true;
#endif
		}
	}
	// OLE拖动时鼠标经过
	void text_area_cx::on_ole_dragover(event_km_t* e)
	{
		assert(_listen.get_hot_count());
		auto t = this;
		_listen.send_vcb([&]()
			{
				// 本对象拖动的要不在选中区域时
				if (is_dragstart)
				{
					if (in_selection(e->pos)) {
						t = 0;
						is_cs = false;
						return;
					}
					// todo 判断文本域
				}
				if (!is_range(e->pos))
				{
					t = 0;
				}
				//printf("on_ole_dragover %d\t%d\n", (int)pf.x, (int)pf.y);
			});
		e->user_ret = t;	// 设置可以接收ole
	}
	void text_area_cx::on_leave(event_km_t* e)
	{
		_sc = (uint8_t)mouse_cursor::SC_ARROW;	// 箭头光标
	}
	void text_area_cx::on_move(event_km_t* e)
	{
		// 拖动时不更新
		if (is_dragstart)return;
		do
		{
			if (pos_first.x < 0 || pos_first.y < 0)
			{
				//printf("no hot\n");
				return;
			}
			if (!e->MouseDown[0]) {
				if (is_select)
				{
					bool ins = in_selection(e->pos);
					_sc = (uint8_t)(ins ? mouse_cursor::SC_ARROW : mouse_cursor::SC_IBEAM);
				}
				return;
			}
		} while (0);
		auto lt = _listen.get_rbegin();
		if (lt >= 0)
		{
			lasthot = lt;
		}
		int hidx = lasthot;
		{
			auto reg = _reg;
			auto p = ecs::mk_data<text_area_t>(_reg, _area_entity[hidx]);
			auto& it = *p;
			pos_second = it.ext.get_pos_idx({ e->pos.x - it.text_pos.x, e->pos.y - it.text_pos.y });
			if (!(pos_second.x < 0 || pos_second.y < 0))
			{
				pta_second = p;
				//pos_second.y += it.row_base;
				auto& pc = get_cursor_blink(_cursor_blink_entity);
				pc.pos.x = pos_second.z + it.text_pos.x; pc.pos.y = pos_second.w + it.text_pos.y;
				pc.height = it.row_height;
				if (pc.pos.x < it.text_pos.x)
				{
					//printf("on_move\n");
				}
				if (!pta_first || !pta_second)return;
				text_area_t* pta[2] = { pta_first, pta_second };
				glm::ivec2 rs[2] = { {pos_first.x, pos_first.y}, {pos_second.x, pos_second.y} };
				glm::ivec2 rpx[2] = { {pos_first.z, pos_first.w}, {pos_second.z, pos_second.w} };
				if (pta[0] > pta[1] || (pta[0] == pta[1] && rs[0] > rs[1]))
				{
					std::swap(pta[0], pta[1]);
					std::swap(rs[0], rs[1]);
					std::swap(rpx[0], rpx[1]);
					rpos[0] = rs[0];
					rpos[1] = rs[1];
				}
				//size_t rn = _row_entity.size();

				//for (size_t k = 0; k < rn; k++)
				//{
				//	auto rp = get_row(k);
				//	if (k < rs[0].y || k > rs[1].y)
				//	{
				//		if (_reg->has<selection_et>(rp->_selection))
				//			_reg->remove<selection_et>(rp->_selection);
				//	}
				//	else {
				//		ecs::mk_edata<selection_et>(_reg, rp->_selection);
				//	}
				//	auto& kt = get_rect(rp->_selection);
				//	kt.a.x = rp->_pos.x;
				//	kt.a.y = rp->_pos.y;
				//	kt.a.w = rp->_size.y;
				//	kt.a.z = 0;
				//	kt.fill = selecol;
				//}
				// 清空选中效果
				auto view_selection = _reg->view<selection_et, rect_info_t>();
				for (const entt::entity e : view_selection) {
					if (_reg->has<selection_et>(e))
						_reg->remove<selection_et>(e);
				}
				auto bp = get_row(pta[0]->row_entity, rs[0].y);
				auto ep = get_row(pta[1]->row_entity, rs[1].y);
				auto& b = get_rect(bp->_selection);
				auto& e = get_rect(ep->_selection);
				b.a.x = bp->_pos.x;
				b.a.y = bp->_pos.y;
				b.a.w = bp->_size.y;
				b.a.z = bp->_size.x;
				b.fill = selecol;
				e.a.x = ep->_pos.x;
				e.a.y = ep->_pos.y;
				e.a.w = ep->_size.y;
				e.fill = selecol;
				e.a.z = ep->_size.x;
				selection1 = rs[0].y;
				selection2 = rs[1].y;

				ecs::mk_edata<selection_et>(_reg, bp->_selection);
				ecs::mk_edata<selection_et>(_reg, ep->_selection);
				std::string& str = selection_str;
				str.clear();
				glm::uvec2 ef2 = rs[1];
				auto er1 = bp->_size.x - rpx[1].x;
				b.a.x += rpx[0].x;
				b.a.z -= rpx[0].x;
				e.a.z -= er1;

				size_t bns = pta[0]->row_entity.size();
				size_t ens = 0;
				if (rs[0].y != rs[1].y || pta[0] != pta[1])
				{
					e.a.z = rpx[1].x;
					ens = rs[1].y;
				}
				if (pta[0] != pta[1])
				{
					ef2 = { -1, -1 };
				}
				auto sn = pta[0]->ext.get_substr(rs[0], ef2, str, &pta[0]->select_str.str);
				pta[0]->select_str.str1 = pta[0]->select_str.str + sn;
				auto mkedcb = [=](size_t b, int64_t n, text_area_t* pt)
				{
					if (n < 0)return;
					if (n > pt->row_entity.size())
					{
						n = pt->row_entity.size();
					}
					for (size_t i = b; i < n; i++)
					{
						auto mp = get_row(pt->row_entity, i);
						auto& mt = get_rect(mp->_selection);
						mt.a.z = mp->_size.x;
						mt.a.x = mp->_pos.x;
						mt.a.y = mp->_pos.y;
						mt.a.w = mp->_size.y;
						mt.fill = selecol;
						ecs::mk_edata<selection_et>(_reg, mp->_selection);
					}
				};
				// 设置中间选中行
				for (text_area_t* pt = pta[0] + 1; pt < pta[1]; pt++)
				{
					size_t ptns = pt->row_entity.size();
					str.push_back('\n');
					sn = pt->ext.get_substr({ 0, 0 }, { -1, -1 }, str, &pt->select_str.str);
					pt->select_str.str1 = pt->select_str.str + sn;
					mkedcb(0, ptns, pt);

				}
				if (ef2.y == -1)
				{
					str.push_back('\n');
					sn = pta[1]->ext.get_substr({ 0, 0 }, rs[1], str, &pta[1]->select_str.str);
					pta[1]->select_str.str1 = pta[1]->select_str.str + sn;
				}
				//if (str.size() > 3)
				//{
				//	static std::string astr;
				//	if (astr != str)
				//	{
				//		astr = str;
				//		auto str1 = u8_gbk(str);
				//		int tid = get_tid();
				//		//printf("%d\t%s\n\n", tid, str1.c_str());
				//	}
				//}
				if (pta[0] == pta[1])
				{
					if (rs[1].y - rs[0].y > 0)
						mkedcb((int64_t)rs[0].y + 1, rs[1].y, pta[0]);
				}
				else
				{
					mkedcb((int64_t)rs[0].y + 1, pta[0]->row_entity.size(), pta[0]);
					mkedcb(0, (int64_t)rs[1].y - 1, pta[1]);
				}

				//for (int m = rs[0].y + 1; m < rs[1].y; m++)
				//{
				//	auto mp = get_row(m);
				//	auto& mt = get_rect(mp->_selection);
				//	mt.a.z = mp->_size.x;
				//	ecs::mk_edata<selection_et>(_reg, mp->_selection);
				//}

			}
		}
	}

	void text_area_cx::draw_update(cpg_time* t, canvas_cx* canvas)
	{
		auto& reg = *_reg;
		_listen.at_run(true);
		auto view_background = reg.view<background_et, rect_info_t>();
		auto view_selection = reg.view<selection_et, rect_info_t>();
		auto view_text = reg.view<display_et, text_area_t>();

		// 渲染每行背景色
		for (auto it = view_background.rbegin(); it != view_background.rend(); it++) {
			auto vsr = &view_background.get<rect_info_t>(*it);
			canvas->add_obj(vsr, 1);
		}
		// 渲染选择效果
		for (auto it = view_selection.rbegin(); it != view_selection.rend(); it++) {
			auto vsr = &view_selection.get<rect_info_t>(*it);
			canvas->add_obj(vsr, 1);
		}

		//渲染文本
		for (auto it = view_text.rbegin(); it != view_text.rend(); it++) {
			auto tp = &view_text.get<text_area_t>(*it);
			canvas->add_obj(tp, 1, sizeof(text_area_t), 0);
		}
		// 更新光标
		if (is_cursor)
		{
			auto& pc = get_cursor_blink(_cursor_blink_entity);
			if (t->time - pc.timecap > pc.st)
			{
				pc.timecap = t->time;
				pc.iscap = !pc.iscap;
			}
			static glm::ivec2 cpos;
			if (cpos != pc.pos)
			{
				cpos = pc.pos;
				//printf("cpos\t%d,%d\n", pc.pos.x, pc.pos.y);
			}
			//画光标
			if (pc._ud)
			{
				canvas->add_obj(pc._ud, 1);
			}
			else {
				pc._line.a = { pc.pos.x, pc.pos.y, pc.pos.x, pc.pos.y + pc.height };
				pc._line.thickness = pc.thickness;
				pc._line.col = pc.iscap ? pc.color : 0;
				canvas->add_obj(&pc._line, 1);
			}
		}
		//if (_ctx)
		//	_ctx->set_system_cursor((mouse_cursor)_sc);
	}

	bool text_area_cx::in_selection(glm::ivec2 pos)
	{
		auto n = math_cx::in_rect(&_selection_hot, pos, &_select_hotidx);
		return n > 0;
	}
	bool text_area_cx::is_range(glm::ivec2 pos)
	{
		text_area_t* pta = 0;
		int lhot = -1;
		bool israng = false;
		auto pf = on_first_pos(pos, &lhot, &pta, false, true, &israng);
		return israng;
	}

	void text_area_cx::on_input(event_km_t* e)
	{
		if (!pta_first)return;

	}

	void text_area_cx::on_key(event_km_t* e)
	{
		if (!pta_first || !e->pressed)return;
		auto& ext = pta_first->ext;
		printf("%s\n", e->kn);
		bool isupcursor = false;
		switch (e->keycode)
		{
		case SDLK_PRINTSCREEN:
		{}
		break;
		case SDLK_SCROLLLOCK:
		{}
		break;
		case SDLK_PAUSE:
		{}
		break;
		case SDLK_INSERT:
		{

		}
		break;
		case SDLK_HOME:
		{
			ext.set_cpos_home_end(true); isupcursor = true;
		}
		break;
		case SDLK_PAGEUP:
		{

		}
		break;
		case SDLK_DELETE:
		{

		}
		break;
		case SDLK_END:
		{
			ext.set_cpos_home_end(false); isupcursor = true;
		}
		break;
		case SDLK_PAGEDOWN:
		{

		}
		break;
		case SDLK_RIGHT:
		{
			ext.set_cpos_inc_x(1); isupcursor = true;
		}
		break;
		case SDLK_LEFT:
		{
			ext.set_cpos_inc_x(-1); isupcursor = true;
		}
		break;
		case SDLK_DOWN:
		{
			ext.set_cpos_inc_y(1); isupcursor = true;
		}
		break;
		case SDLK_UP:
		{
			ext.set_cpos_inc_y(-1); isupcursor = true;
		}
		break;
		default:
			break;
		}
		if (isupcursor)
		{
			auto ret = ext.get_px_idx(1);
			if (!(ret.x < 0 || ret.y < 0))
			{
				ret.y += pta_first->row_base;
				auto& dpc = get_cursor_blink(_cursor_blink_entity);
				auto pc = dpc;
				pc.pos.x = ret.z + pta_first->text_pos.x; pc.pos.y = ret.w + pta_first->text_pos.y;
				pc.height = pta_first->row_height;
				dpc = pc;
			}
		}
	}

#endif // 1

	// todo 列表
	list_event_t::list_event_t()
	{
	}

	list_event_t::~list_event_t()
	{
	}

	void list_event_t::set_pos(glm::ivec2 pos)
	{
		edc.set_nodepos(pos);
	}

	void list_event_t::set_type(int t)
	{
	}

	void list_event_t::set_size(int w, int h)
	{
	}

	void list_event_t::set_weight(int w)
	{
	}
#if 1


#endif // 1
#ifndef no_trv
	class edit_es_t
	{
	public:

		struct input_info_t
		{
			glm::ivec2 pos;
			glm::ivec2 pos1;
			std::string str;
		};
	private:
		event_data_cx* listen = 0;
		item_box_info* _ibd = 0;
		text_view_t* tvt = 0;

		// 焦点文本域
		//t_set<int> _hotidx;
		t_set<int> _select_hotidx;
		// 选中热区判断
		t_vector<glm::vec4> _selection_hot;
		glm::ivec4 pos_first = { -1, -1, -1, -1 }, pos_second;
		//text_area_t* pta_first = 0, * pta_second = 0;
		bool is_select = false;
		bool is_dragstart = false;
		bool is_drag_me = false;
		bool is_cs = false;
		bool is_upinput = true;
		bool is_upeting = true;
		bool is_cursor = true;
		bool is_uptcp = false;
		// 最后拾取的idx
		int lasthot = -1;
		std::vector<input_info_t> _iit;


		cursor_blink_t cblink;
		uint32_t _text_color = -1;
		uint32_t _cursor_color = -1;

		// 默认选中背景色
		uint32_t selecol = 0x80ff9033;
		size_t selection1 = 0, selection2 = 0;
		// 选中的文本
		std::string selection_str;
		t_vector<glm::ivec4> _rectvs;
		int sct_count = 0;
		glm::ivec4	_rscp1, _rscp2;
		glm::ivec2	_tcp1, _tcp2;
		// editing
		std::string _editingstr;
		text_info_t _editinginfo;
		glm::ivec2	_etc;
		int _etcount = 0;
		std::vector<int> _et_x;
		// 渲染偏移坐标
		glm::ivec2 _t_pos;
		// 内容偏移坐标
		glm::ivec2 _scroll_pos;
		int scroll_count = 1;
		bool is_scroll = true;
		bool is_undo = false;
		bool is_redo = false;
		// 单行
		bool single_line = false;
		size_t tidx = 0;		// hot索引
		// 鼠标光标
		uint8_t _sc = 0;
		uint8_t _scc = 0;
		// 窗口管理器
		bw_sdl* _ctx = 0;
		// 编辑器存储
		buffer_t* _storage_buf = 0;
		// 输入缓存
		std::string _inbuf;
		std::function<void(const char* str, int len)> _on_input;
		std::function<void(edit_es_t*, bool is_input, bool is_editing)> _on_upcb;
		glm::ivec2 _cpos = { -1, -1 };
		sdl_form* _cform = 0;
	public:
		int* ick = 0;
		bool is_ibeam = true;
		bool _read_only = false;
		// 是否响应事件
		bool no_event = false;
	public:
		edit_es_t(event_data_cx* p, item_box_info* ibp, buffer_t* buf, text_view_t* pt) :_storage_buf(buf), tvt(pt)
		{
			assert(_storage_buf);
			_ibd = ibp;
			listen = p;
			//listen->_hotidx = &_hotidx;
			listen->set_athread();
			listen->set_cb(event_type::on_down, this, &edit_es_t::on_down);
			listen->set_cb(event_type::on_move, this, &edit_es_t::on_move);
			listen->set_cb(event_type::on_drag, this, &edit_es_t::on_drag);
			listen->set_cb(event_type::on_up, this, &edit_es_t::on_up);
			listen->set_cb(event_type::on_click, this, &edit_es_t::on_click);
			listen->set_cb(event_type::on_scroll, this, &edit_es_t::on_scroll);
			listen->set_cb(event_type::on_leave, this, &edit_es_t::on_leave);
			listen->set_cb(event_type::on_enter, this, &edit_es_t::on_enter);
			listen->set_cb(event_type::on_dragstart, this, &edit_es_t::on_dragstart);
			listen->set_cb(event_type::on_ole_drop, this, &edit_es_t::on_ole_drop);
			listen->set_cb(event_type::on_ole_drag, this, &edit_es_t::on_ole_drag);
			listen->set_cb(event_type::on_ole_dragover, this, &edit_es_t::on_ole_dragover);
			listen->set_cb(event_type::on_input, this, &edit_es_t::on_input);
			listen->set_cb(event_type::on_editing, this, &edit_es_t::on_editing);
			listen->set_cb(event_type::on_keypress, this, &edit_es_t::on_key);
			cblink.pos = { -100, -100 };
		}

		~edit_es_t()
		{
		}
		void set_ctx(bw_sdl* ctx)
		{
			_ctx = ctx;
		}
		void set_hot(glm::ivec4 v4, std::function<void(edit_es_t*, bool is_input, bool is_editing)> upcb)
		{
			listen->set_v4(0, v4, true);
			_on_upcb = upcb;
		}
		void set_hotv4(glm::ivec4 v4)
		{
			listen->set_v4(0, v4, true);
		}
		void set_color(uint32_t text_color, uint32_t select_color, uint32_t cursor_color);
	public:
		glm::ivec2 get_dc_pos(bool iscontent = true) {
			glm::ivec2 ret;
			if (listen)
				ret += listen->get_nodepos();
			auto v4 = listen->get_v4(tidx);
			ret.x += v4.x;
			ret.y += v4.y;
			if (iscontent)ret -= _scroll_pos;
			return ret;
		}
	public:
		void on_down(event_km_t* e) {
			if (listen->get_hot_count() == 0 || no_event)
				return;
			assert(listen->get_hot_count());
			if (_etcount > 0)
				return;
			if (ick)
				*ick += 1;
			//print_time exs("on_down get_pos_idx");
			//pta_second = 0;
			is_cs = true;
			if (is_select && in_selection(e->pos))
			{
				is_dragstart = true;
				return;
			}
			pos_first = on_first_pos(e->pos, &lasthot, true, true, 0);
			//set_cursor_idx();
			if (is_cursor)
			{
				auto& pc = get_cursor_blink(); pc.timecap = pc.st + 1;
				if (!_cform)
					set_ime_pos(pc.pos);
				if (_cform)
				{
					//if (!_cform->is_text_input_active())
					_cform->start_text_input();
					//set_ime_pos(pc.pos);
					//printf("cf ime pos: %d\t%d\n", pc.pos.x, pc.pos.y);
				}
			}
		}

		void on_click(event_km_t* e)
		{
			if (no_event)return;
		}
		void on_up(event_km_t* e)
		{
			if (_etcount > 0 || no_event)
				return;
			bool isds = is_dragstart;
			is_dragstart = false;
			if (is_cs)
			{
				if (isds)
				{
					_selection_hot.clear();
					_rectvs.clear();
				}
				//
				//auto view_selection = _reg->view<selection_et, rect_info_t>();
				//// 选择框
				//for (const entt::entity e : view_selection) {
				//	auto vsr = &view_selection.get<rect_info_t>(e);
				//	_selection_hot.push_back(vsr->a);
				//}
				is_select = !_selection_hot.empty();
			}
			//if (!pta_second)
			if (!is_select)
			{
				on_first_pos(e->pos, 0, is_cs, is_cs, 0);
			}
			//printf("on up %d\n", (int)_selection_hot.size());
		}
		void on_scroll(event_km_t* e)
		{
			if (!_ibd || !_ibd->extent || no_event)return;
			auto oldy = _scroll_pos.y;
			auto h = _ibd->fontheight + _ibd->row_span;
			_scroll_pos.y += e->wheel * scroll_count * h;
			auto slc = _ibd->extent->get_showline_count();
			slc--;
			if (_scroll_pos.y > slc * h)
			{
				_scroll_pos.y = slc * h;
			}
			if (_scroll_pos.y < 0)
			{
				_scroll_pos.y = 0;
			}
			if (oldy != _scroll_pos.y)
			{
				// todo 更新坐标
			}
		}
		void on_enter(event_km_t* e)
		{
			if (!is_ibeam || no_event)return;
			_sc = (uint8_t)mouse_cursor::SC_IBEAM;	// 输入光标
			if (is_select)
			{
				bool ins = in_selection(e->pos);
				_sc = (uint8_t)(ins ? mouse_cursor::SC_ARROW : mouse_cursor::SC_IBEAM);
			}
		}
		void on_dragstart(event_km_t* e)
		{

		}
		void on_ole_drop(event_km_t* e)
		{
			auto kstr = listen->get_dropstr();
			kstr = u8_gbk(kstr);
			//printf("接收\t%s\n", kstr.c_str());

		}
		void on_ole_drag(event_km_t* e)
		{
			if (is_select && is_dragstart)
			{
#ifdef _WIN32
				//printf("开始拖动\n");
				std::wstring olestr = u8_w(selection_str);
				do_dragdrop_begin(olestr); // 会在消息线程执行
				is_drag_me = true;
#endif
			}
		}
		// OLE拖动时鼠标经过
		void on_ole_dragover(event_km_t* e)
		{
			assert(listen->get_hot_count());
			auto t = this;
			listen->send_vcb([&]()
				{
					// 本对象拖动的要不在选中区域时
					if (is_dragstart)
					{
						if (in_selection(e->pos)) {
							t = 0;
							is_cs = false;
							return;
						}
						// todo 判断文本域
					}
					if (!is_range(e->pos))
					{
						t = 0;
					}
					//printf("on_ole_dragover %d\t%d\n", (int)pf.x, (int)pf.y);
				});
			e->user_ret = t;	// 设置可以接收ole
		}
		void on_leave(event_km_t* e)
		{
			//_sc = (uint8_t)mouse_cursor::SC_ARROW;	// 箭头光标
		}
		void on_move(event_km_t* e)
		{
			//printf("move:%d,%d\n", e->pos.x, e->pos.y);
#if 1
			// 拖动时不更新
			if (is_dragstart || no_event)return;
			do
			{
				if (pos_first.x < 0 || pos_first.y < 0 || !is_cs)
				{
					//printf("no hot\n");
					return;
				}
				if (!e->MouseDown[0]) {
					if (is_select && is_ibeam)
					{
						bool ins = in_selection(e->pos);
						_sc = (uint8_t)(ins ? mouse_cursor::SC_ARROW : mouse_cursor::SC_IBEAM);
					}
					return;
				}
			} while (0);

#endif
		}
		void on_drag(event_km_t* e)
		{
			//printf("on_drag:%d,%d\n", e->pos.x, e->pos.y);
#if 1
			// 拖动时不更新
			if (is_dragstart || no_event)return;
			auto lt = listen->get_rbegin();
			if (lt >= 0)
			{
				lasthot = lt;
			}
			int hidx = lasthot;
			{
				auto ext = _ibd->extent;
				auto text_pos = get_dc_pos(false);
				auto ps = _scroll_pos + e->pos - text_pos;
				pos_second = ext->get_pos_idx(ps);

				if (!(pos_second.x < 0 || pos_second.y < 0))
				{
					// 更新光标
					auto& pc = get_cursor_blink();
					//pc.pos.x = pos_second.z /*+ text_pos.x*/; pc.pos.y = pos_second.w /*+ text_pos.y*/;
					//pc.height = _ibd->fontheight + _ibd->row_span;//_ibd->box_size.y;//
					if (_ibd->mheight > pc.height)
					{
						//pc.height = _ibd->mheight;
					}
					if (pc.pos.x < text_pos.x)
					{
						//printf("on_move\n");
					}
					glm::ivec2 rs[2] = { {pos_first.x, pos_first.y}, {pos_second.x, pos_second.y} };
					glm::ivec2 rpx[2] = { {pos_first.z, pos_first.w}, {pos_second.z, pos_second.w} };
					selection_str;
					// 选区
					glm::ivec2 cp0 = { pos_first.x, pos_first.y };
					glm::ivec2 cp = { pos_second.x, pos_second.y };
					//if (cp0 != cp)
					{
						_tcp2 = cp;
						ext->set_cpos(cp, 1);
						is_uptcp = true;
					}
				}
			}
#endif
		}

		glm::ivec4 on_first_pos(glm::ivec2 pos, int* outlast, bool is_clear_select, bool isupcursor, bool* israng)
		{
			glm::ivec4 ret;
#if 1
			if (is_clear_select)
			{
				is_select = false;
				//auto view_selection = _reg->view<selection_et, rect_info_t>();
				//for (const entt::entity e : view_selection) {
				//	if (_reg->has<selection_et>(e))
				//		_reg->remove<selection_et>(e);
				//}
			}
			auto lt = listen->get_begin();
			if (lt < 0)
			{
				return ret;
			}

			int hidx = lt;
			{
				auto ext = _ibd->extent;
				auto tps = get_dc_pos(false);
				auto ps = _scroll_pos + pos - tps;
				glm::ivec2 cpos;
				ret = ext->get_pos_idx(ps, israng, &cpos);
				glm::ivec2 xcpos = { ret.z, ret.w };
				//if (!(ret.x < 0 || ret.y < 0))
				{
					//ret.y += it.row_base;
					auto& dpc = get_cursor_blink();
					auto pc = dpc;
					pc.pos = xcpos;

					if (israng && !(*israng))
					{
						isupcursor = false;
					}
					if (isupcursor)
					{
						if (outlast)
						{
							_tcp1 = cpos;
							ext->set_cpos(cpos);
							_tcp2 = cpos;
							ext->set_cpos(cpos, 1);
							is_uptcp = true;
							//printf("pos\t%d,%d\n", cpos.x, cpos.y);
						}
						//dpc = pc;
					}
				}
			}
			if (outlast)*outlast = hidx;
#endif
			return ret;
		}

		bool in_selection(glm::ivec2 pos)
		{
			auto text_pos = get_dc_pos(false);
			auto ps = _scroll_pos + pos - text_pos;
			auto n = math_cx::in_rect(&_selection_hot, ps, &_select_hotidx);
			return n > 0;
		}
		bool is_range(glm::ivec2 pos)
		{
			int lhot = -1;
			bool israng = false;
			auto pf = on_first_pos(pos, &lhot, false, true, &israng);
			return israng;
		}
		void on_editing(event_km_t* e)
		{
			if (no_event)return;
			_etc = e->editing;
			_editingstr = e->text;
			_etcount = get_utf8_count(_editingstr.c_str(), _editingstr.size());
			is_upeting = true;
		}
		// todo ime input
		void on_input(event_km_t* e)
		{
			//std::string s = u8_gbk(e->text, e->str_size);
			//printf("on input:\t%s\n", s.c_str());
			if (_read_only || no_event)return;
			_inbuf.append(e->text, e->str_size);
		}

		void on_key(event_km_t* e)
		{
			//printf("on_key\t%s\t%d:%d\n", e->kn, e->keycode, e->pressed);
			// 编辑中的时候不响应按键
			if (!_ibd || _editingstr.size() || no_event)return;
			auto ext = _ibd->extent;
			if (!e->pressed)
			{
				do {
					if (!e->KeyCtrl)break;
					switch (e->keycode) {
					case SDLK_a:
					{
						_tcp1 = { 0, 0 };
						_tcp2 = { -1, -1 };
						ext->set_cpos(_tcp1, 0);
						ext->set_cpos(_tcp2, 1);
						is_uptcp = true;
					}
					break;
					case SDLK_x:
					case SDLK_c:
					{
						auto cp1 = ext->get_cpos2(0);
						auto cp2 = ext->get_cpos2(1);
						auto str = _storage_buf->get_range(cp1, cp2);
						if (str.size() && e->form)
						{
							e->form->set_clipboard(str.c_str());
							if (e->keycode == SDLK_x && !_read_only)
							{
								_iit.push_back({ cp1, cp2 });
								is_upinput = true;
							}
						}
					}
					break;
					break;
					case SDLK_v:
						_inbuf.append(e->form->get_clipboard());
						break;
					case SDLK_y:
						is_redo = true;	//_storage_buf->redo();
						break;
					case SDLK_z:
						is_undo = true;	//_storage_buf->undo();
						break;
					}
				} while (0);
				return;
			}

			bool isupcursor = false;
			switch (e->keycode)
			{
			case SDLK_TAB:
			{}
			break;
			case SDLK_BACKSPACE:
			{
				auto cp1 = ext->get_cpos2(0);
				auto cp2 = ext->get_cpos2(1);
				if (cp1 == cp2) {
					cp1 = ext->te2cpos2(ext->get_cpos_inc_x(-1, 0));
				}
				if (cp1 > cp2)std::swap(cp1, cp2);
				if (cp1.x > 0 && cp1 != cp2)
				{
					if (!_read_only)
						_iit.push_back({ cp1, cp2 });
					is_upinput = true;
				}
			}
			break;
			case SDLK_PRINTSCREEN:
			{}
			break;
			case SDLK_SCROLLLOCK:
			{}
			break;
			case SDLK_PAUSE:
			{}
			break;
			case SDLK_INSERT:
			{

			}
			break;
			case SDLK_HOME:
			{
				ext->set_cpos_home_end(true); isupcursor = true;
			}
			break;
			case SDLK_PAGEUP:
			{

			}
			break;
			case SDLK_DELETE:
			{
				auto cp1 = ext->get_cpos2(0);
				auto cp2 = ext->get_cpos2(1);
				if (cp1 > cp2) {
					std::swap(cp1, cp2);
				}
				if (cp1 == cp2) {
					cp2 = ext->te2cpos2(ext->get_cpos_inc_x(1, 1));
				}
				if (cp1 != cp2)
				{
					if (!_read_only)
						_iit.push_back({ cp1, cp2 });
					is_upinput = true;
				}
			}
			break;
			case SDLK_END:
			{
				ext->set_cpos_home_end(false); isupcursor = true;
			}
			break;
			case SDLK_PAGEDOWN:
			{

			}
			break;
			case SDLK_RIGHT:
			{
				ext->set_cpos_inc_x(1); isupcursor = true;
			}
			break;
			case SDLK_LEFT:
			{
				ext->set_cpos_inc_x(-1); isupcursor = true;
			}
			break;
			case SDLK_DOWN:
			{
				ext->set_cpos_inc_y(1); isupcursor = true;
			}
			break;
			case SDLK_UP:
			{
				ext->set_cpos_inc_y(-1); isupcursor = true;
			}
			break;
			case SDLK_RETURN:
			{
				if (!single_line)
				{
					e->text[0] = '\n';
					e->str_size = 1;
					on_input(e);
				}
			}
			break;
			default:
				break;
			}
			up_cursor(isupcursor, false);
		}
		double dcscroll(double cox, double isx, double scroll_increment_x, int& scrollx)
		{
			double ret = .0;
			//const = 2;// inner_size.x* 0.25f;
			if (cox < scrollx)
			{
				ret = floor(std::max(0.0, cox - scroll_increment_x));
				scrollx = ret;
			}
			else if (cox - isx >= scrollx && isx > 0)
			{
				ret = floor(cox - isx + scroll_increment_x);
				scrollx = ret;
			}
			return ret;
		}
		void up_cursor(bool is, bool isime)
		{
			if (tvt)
				single_line = tvt->single_line;
			if (is && _ibd && _ibd->extent)
			{
				auto ext = _ibd->extent;
				auto ret = ext->get_px_idx(1);
				auto evs = ext->_ev_size;
				auto h = tvt->row_size();// _ibd->fontheight + _ibd->row_span;
				int ey = evs.y / h;
				ey *= h;

				glm::ivec2 pos;
				if (is_scroll) {
					dcscroll(ret.z, evs.x, 2, _scroll_pos.x);
					dcscroll(ret.w, ey, h, _scroll_pos.y);
				}
				else
				{
					_scroll_pos = { .0, .0 };
				}

				if (!(ret.x < 0 || ret.y < 0))
				{
					pos.x += ret.z;
					pos.y += ret.w;
				}
				auto& dpc = get_cursor_blink();
				auto pc = dpc;
				pc.pos = pos;
				pc.height = h;
				if (single_line)pc.height = _ibd->_size.y;
				if (_ibd->mheight > pc.height)
				{
					pc.height = _ibd->mheight;
				}
				dpc = pc;
				is_uptcp = true;
				//auto pc = get_cursor_blink();
				//if (isime)
				set_ime_pos(pc.pos);

			}
		}
		void set_ime_pos(glm::ivec2 pcp)
		{
			//printf("ime pos: %d\t%d\n", pcp.x, pcp.y);
			pcp += get_dc_pos();
			//if (_cpos != pcp)
			{
				_cpos = pcp;
				if (_ctx)
				{
					glm::ivec4 tir = { pcp, glm::ivec2{2, 2} };

					auto pfm = _ctx->get_activate_form();
					if (pfm || _cform)
					{
						tir.y += _ibd->fontheight + _ibd->row_span - 16;
						bool iscf = !_cform;
						if (pfm && pfm != _cform)
							_cform = pfm;
						if (listen->is_input)
						{
							_cform->set_text_input_rect(tir);
							//printf("set_text%p_input_rect\t%d\t%d\n", this, pcp.x, pcp.y);
						}
						else {
							printf("no input %d\n", listen->is_input);
						}
					}
				}
			}
		}
		cursor_blink_t& get_cursor_blink()
		{
			return cblink;
		}
		void up_selection()
		{
			if (!_ibd || !_ibd->extent || !is_uptcp)return;
			auto ext = _ibd->extent;
			auto ps1 = ext->get_px_idx(0);
			auto ps2 = ext->get_px_idx(1);
			if (ps1 != ps2)
			{
				if (_rscp1 != ps1 || _rscp2 != ps2)
				{
					_rscp1 = ps1;
					_rscp2 = ps2;
					_rectvs.clear();
					selection_str.clear();
					_selection_hot.clear();
					float ith = _ibd->fontheight + _ibd->row_span;
					if (_ibd->mheight > ith)
					{
						ith = _ibd->mheight;
					}
					//ith = 0;
					ext->get_pos2_rects(ps1, ps2, _rectvs, ith);// _ibd->box_size.y);
					ext->get_substr(ps1, ps2, selection_str, nullptr);
					sct_count = _rectvs.size();

					for (auto it : _rectvs) {
						_selection_hot.push_back(it);
					}
					//printf("选中：%s\n", "a");
				}
			}
			else {
				_rscp1 = _rscp2 = {};
				_rectvs.clear();
			}
		}
		// 事件/更新
		void event_cb(cpg_time* t)
		{
			{
				//print_time pt("event_cb");
				listen->at_run(false);
			}

			if (_sc && !listen->ishot()) {
				_sc = (uint8_t)mouse_cursor::SC_ARROW;	// 箭头光标
			}
			if (_ctx && _scc != _sc)
			{
				//printf("mouse_cursor\t%p %d\n", this, (int)_sc);
				_scc = _sc;
				_ctx->set_cursor((mouse_cursor)_sc);
			}
			auto ext = _ibd->extent;
			if (is_uptcp)
			{
				//ext->set_cpos(_tcp1, 0);
				//ext->set_cpos(_tcp2, 1);
				//printf("cpos\t%d\t%d == %d\t%d\n", _tcp1.x, _tcp1.y, _tcp2.x, _tcp2.y);
			}
			if (_on_upcb)
			{
				glm::ivec2 r;
				bool isup = false;
				bool isupep = false;
				// 处理editing
				if (!listen->is_input && _editingstr.size())
				{
					_editingstr.clear(); _etcount = 0;
				}
				if (_ibd->next && is_upeting)
				{
					auto& s = _ibd->next->s;
					if (_editingstr.size())
					{
						isup = true;
						s.str = _editingstr.c_str();
						s.count = get_utf8_count(s.str, _editingstr.size());
						s.first = 0;
						if (!is_upinput && s.count > 0)
						{
							isupep = true;
							_on_upcb(this, false, true);
						}
					}
				}
				// 处理输入
				if (is_upinput && _iit.size() || _inbuf.size() || is_undo || is_redo)
				{

					int cuinc = 0;
					glm::ivec2 zero = {};
					if (is_undo) {
						r = _storage_buf->undo();
						//if (r > zero)
						{
							cuinc++;
						}
						is_undo = false;
					}
					if (is_redo) {
						r = _storage_buf->redo();
						//if (r > zero)
						{
							cuinc++;
						}
						is_redo = false;
					}

					if (_inbuf.size())
					{
						auto ext = _ibd->extent;
						auto cp1 = ext->get_cpos2(0);
						auto cp2 = ext->get_cpos2(1);

						_iit.push_back({ cp1, cp2, _inbuf });
						_inbuf.clear();
					}
					//printf("输入数量：\t%d\n", (int)_iit.size());
					auto ic = cuinc;
					for (auto& it : _iit)
					{
						if (it.pos > it.pos1)
						{
							std::swap(it.pos, it.pos1);
						}
						if (it.str.empty())
						{
							// 执行删除
							_storage_buf->remove(it.pos, it.pos1);
							r = it.pos; cuinc++;
							//printf("\t光标d:%d\t%d\t%d\n", (int)it.str.size(), r.x, r.y);
						}
						else {
							// 插入文本
							if (single_line)
							{
								std::remove(it.str.begin(), it.str.end(), '\n');
							}
							if (tvt && tvt->on_input)
							{
								tvt->on_input(&it.str);
							}
							if (it.str.empty())
							{
								r = it.pos;
							}
							else {
								r = _storage_buf->insert(it.pos, it.str.c_str(), it.str.size(), &it.pos1);
							}
							cuinc++;
							//printf("\t光标:%d\t%d\t%d\n", (int)it.str.size(), r.x, r.y);
						}

					}
					if (ic != cuinc)
					{
						_storage_buf->clear_redo();
					}
					// 更新文本布局、渲染
					_on_upcb(this, true, isupep);
					// 重置光标坐标
					if (cuinc)
					{

						_ibd->extent->set_cpos2(0, r);
						_ibd->extent->set_cpos2(1, r);
					}
					_iit.clear();
					isup = true;
				}
				// 处理editing光标的坐标
				if (is_upeting && _ibd->next)
				{
					is_upeting = false;
					_et_x.clear();
					auto& s = _ibd->next->items;
					if (s.size())
					{
						int xx = 0;
						for (auto& it : s)
						{
							_et_x.push_back(xx);
							xx += it.adv.x;
						}
						_et_x.push_back(xx);
					}
				}
				if (!is_uptcp)
					is_uptcp = isup;
				up_cursor(is_uptcp || isup, true);
				is_upinput = false;
			}
			// 处理选中
			up_selection();
			is_uptcp = false;
			// 更新光标
			if (is_cursor)
			{
				auto& pc = get_cursor_blink();
				pc.timecap += t->deltaTime;
				if (pc.timecap > pc.st) {
					//printf("%lld\n", pc.tu.elapsed());
					pc.timecap = 0;
					pc.iscap = !pc.iscap;
				}
				//pc.tu.set_fix(pc.st);
				//if (pc.tu.get_fix())
				//{
				//	pc.tu.reset();
				//}
			}
			// 渲染wa款式a
			//draw(canvas);
		}
		int get_pade(item_box_info* p, int ssy)
		{
			auto k = 0.0;
			auto d = p->base_line / p->ad.x * p->ad.y;
			auto rs = p->_raw_size.y + p->base_line;
			glm::ivec2 v4 = listen->get_v4(tidx);
			if (single_line)
			{
				k += (ssy - v4.y * 2 - p->ad.z);// +_ibd->row_span;
			}
			return k;
		}
	private:

		void draw_selection(canvas_cx* canvas, glm::ivec2 pos)
		{
			if (_rectvs.empty())
				return;
			glm::ivec4 r = { pos, 0, 0 };
			auto ext = _ibd->extent;
			_ibd->base_line;
			//r.y -= ext->_row_span * 0.5;
			for (auto it : _rectvs)
			{
				canvas->draw_rect(r + it, 0, selecol);
			}
		}
	public:
		void draw(canvas_cx* canvas)
		{
			bool save_font_img = false;
			if (save_font_img)
			{
				//it.item[0]._image->saveImage("temp/font_img_pack.png");
			}

			glm::ivec2 pos1 = get_dc_pos();	// 获取本对象坐标

			auto pos = pos1;
			if (_ibd->s.count)
			{
				pos.y += get_pade(_ibd, _ibd->_size.y);
			}
			draw_selection(canvas, pos);

			if (_ibd->s.count)
			{
				canvas->draw_text(_ibd, pos, _text_color);			// 渲染文本
			}

			auto& pc = get_cursor_blink();
			glm::ivec2 etpos = pc.pos + pos1;
			glm::ivec2 etpos1 = pc.pos + pos1;
			auto cc = _cursor_color;// pc.color;
			auto plt = pc.thickness;
			if (_ibd->next && _editingstr.size())
			{
				auto ep = _ibd->next;
				etpos1.y += get_pade(ep, _ibd->_size.y);
				glm::ivec4 r = { etpos1, ep->box_size };
				r.w = ep->fontheight + ep->row_span;
				auto rcc = 0xFFFFFF - set_alpha_f(_text_color, 0);
				rcc = set_alpha_f(rcc, 1.0);
				//r.w = pc.height + 4;
				canvas->push_full_clip();
				canvas->draw_rect(r, 0, rcc);						// editing背景色
				canvas->draw_text(ep, etpos1, _text_color);	// 渲染editing文本s
				auto etcount = get_utf8_count(_editingstr.c_str(), _editingstr.size());
				cc = _cursor_color;
				plt = 2;
				if (_et_x.size())
				{
					if (_etc.x >= _et_x.size())
						_etc.x = _et_x.size() - 1;
					etpos.x += _et_x[_etc.x];	// 修正为editing光标的坐标
				}
				canvas->pop_clipRect();
			}

			if (is_cursor && listen && listen->is_input)
			{
				//画光标
				if (pc._ud)
				{
					canvas->add_obj(pc._ud, 1);
				}
				else {
					pc._line.a = { etpos.x, etpos.y, etpos.x, etpos.y + pc.height };
					pc._line.thickness = plt;
					pc._line.col = pc.iscap ? cc : 0;
					canvas->add_obj(&pc._line, 1);
				}
			}
		}
	};
	edit_es_t* text_event_t::make_event_cb(event_data_cx* listen, item_box_info* p, buffer_t* buf, text_view_t* tvt)
	{
		assert(listen);
		auto ep = new edit_es_t(listen, p, buf, tvt);
		p->_edit = ep;
		return ep;
	}
	void free_edit(edit_es_t* p)
	{
		if (p)delete p;
	}
	void text_event_t::set_rect(edit_es_t* p, glm::ivec4 v4, std::function<void(edit_es_t*, bool is_input, bool is_editing)> upcb)
	{
		assert(p);
		p->set_hot(v4, upcb);
	}

	void text_event_t::set_ctx(edit_es_t* p, bw_sdl* ctx)
	{
		p->set_ctx(ctx);
	}

	void text_event_t::set_ick_ibeam(edit_es_t* p, int* pick, bool is_ibeam)
	{
		p->ick = pick;
		p->is_ibeam = is_ibeam;
	}

	void text_event_t::set_read_only(edit_es_t* p, bool is)
	{
		p->_read_only = is;
	}
	void text_event_t::set_no_event(edit_es_t* p, bool is)
	{
		p->no_event = is;
	}

	void trt::update(cpg_time* t, item_box_info* p)
	{
		auto& info = *p;
		if (p->_edit)
		{
			p->_edit->event_cb(t);
		}
	}
	void trt::draw_ptext(cpg_time* t, canvas_cx* cp, item_box_info* p)
	{
		auto& info = *p;
		if (p->_edit)
		{
			p->_edit->draw(cp);
		}
	}

	void trt::set_color(item_box_info* p, uint32_t tc, uint32_t sc, uint32_t cursor_color)
	{
		if (p->_edit)
		{
			p->_edit->set_color(tc, sc, cursor_color);
		}
	}

#endif
	// todo ecs
#if 1

	namespace ecs_at {
		entt::entity new_em(entt::registry* reg, const glm::vec4& rc, const glm::ivec2& pos)
		{
			const entt::entity e = reg->create();
			//reg->emplace<hot_et>(e, hot_et{ rc, pos ,0,0 });
			reg->emplace<button_et>(e, button_et{ -1, 0 });
			return e;
		}
		entt::entity* new_em(entt::registry* reg, entt::entity* o, int n, bool bind_hb)
		{
			reg->create(o, o + n);
			if (!bind_hb)n = 0;
			for (size_t i = 0; i < n; i++)
			{
				//o[i] = reg->create();
				//reg->emplace<hot_et>(o[i]);
				reg->emplace<button_et>(o[i]);
			}
			return o;
		}

		void free_em(entt::registry* reg, entt::entity* e, int n)
		{
			if (reg && e && n > 0)
			{
				reg->destroy(e, e + n);
			}
		}

		hot_et* get_hs_ptr(entt::registry& reg, entt::entity e)
		{
			hot_et* p = 0;
			if (reg.has<hot_et>(e))
			{
				p = &reg.get<hot_et>(e);
			}
			else {
				p = &reg.emplace<hot_et>(e);
			}
			return p;
		}
		button_et* get_btn_ptr(entt::registry* reg, entt::entity e)
		{
			button_et* p = 0;
			if (reg->has<button_et>(e))
			{
				p = &reg->get<button_et>(e);
			}
			else {
				p = &reg->emplace<button_et>(e);
			}
			return p;
		}
		void set_hot_spot_rc(entt::registry& reg, entt::entity e, const glm::ivec4& rc)
		{
			//auto& d = reg.get_or_emplace<hot_et>(e);
			auto& d = reg.get<hot_et>(e);
			d.rc = rc;
		}
		void set_hot_spot_pos(entt::registry& reg, entt::entity e, const glm::ivec2& pos)
		{
			auto& d = reg.get<hot_et>(e);
			d.pos = pos;
		}
		void sort_reg(entt::registry* reg)
		{
			reg->sort<hot_et>([](const auto& lhs, const auto& rhs)
				{
					return lhs.order > rhs.order;
				}, entt::insertion_sort{});
		}
		hot_et* get_hotptr(entt::registry* reg, entt::entity e)
		{
			hot_et* ty = 0;
			return ecs_at::get_ptr0(reg, e, ty);
		}
		hover_et* get_hover_p(entt::registry* reg, entt::entity e)
		{
			hover_et* p = 0;
			return ecs_at::get_ptr0(reg, e, p);
		}
	}
	namespace em_system {
		void bind_listen(event_data_cx* ld, entt::registry* reg)
		{
			assert(ld && reg);
			if (ld && reg)
			{
				ld->set_cb(event_type::on_move, [=](event_km_t* e) {
					ld->mchild_count = update_move(reg, e->pos - ld->get_nodepos(), e->form);
					/*if (ld->mchild_count == 0) {
						ld->_em->get_bwctx()->set_cursor(mouse_cursor::SC_ARROW);
					}*/
					});
				ld->set_cb(event_type::on_down, [=](event_km_t* e) {
					//if (ld->mchild_count < 1)
					if (!e->form->_main_form) {
						sleep(0);
					}
					ld->mchild_count = update_move(reg, e->pos - ld->get_nodepos(), e->form);
					ld->child_count = update_on_down(reg, e->mouse_idx);
					});
				ld->set_cb(event_type::on_up, [=](event_km_t* e) {
					update_move(reg, e->pos - ld->get_nodepos(), e->form);
					update_on_up(reg, e->mouse_idx);
					//ld->mchild_count = 0;
					//有时会自己执行释放消息ld->child_count = 0;
					//printf("update_on_up\n");
					});
				ld->set_cb(event_type::on_click, [=](event_km_t* e) { update_on_click(reg, e->mouse_idx, 1); });
				ld->set_cb(event_type::on_dblclick, [=](event_km_t* e) { update_on_click(reg, e->mouse_idx, 2); });
				ld->set_cb(event_type::on_tripleclick, [=](event_km_t* e) { update_on_click(reg, e->mouse_idx, 3); });
				ld->set_cb(event_type::on_scroll, [=](event_km_t* e) { update_on_scroll(reg, e->wheel, e->wheelH); });

				ld->set_cb(event_type::on_dragstart, [=](event_km_t* e) { update_drag(reg, e->pos - ld->get_nodepos(), e->pos1, 0); });
				ld->set_cb(event_type::on_drag, [=](event_km_t* e) { update_drag(reg, e->pos - ld->get_nodepos(), e->pos1, 1); });
				ld->set_cb(event_type::on_dragend, [=](event_km_t* e) { update_drag(reg, e->pos - ld->get_nodepos(), e->pos1, 2); });

				ld->set_cb(event_type::on_hover, [=](event_km_t* e) { update_on_hover(reg, e->pos - ld->get_nodepos()); });
				// 鼠标进入
				// 鼠标离开
				ld->set_cb(event_type::on_enter, [=](event_km_t* e) { update_on_enter2leave(reg, e->pos - ld->get_nodepos(), true); });
				ld->set_cb(event_type::on_leave, [=](event_km_t* e) { update_on_enter2leave(reg, e->pos - ld->get_nodepos(), false); });
			}
		}
		int update_move_hover(entt::registry* reg, const glm::ivec2& pos);
		// todo 更新鼠标移动
		int update_move(entt::registry* reg, const glm::ivec2& pos, bool ts)
		{
			int ret = update_move_hover(reg, pos);
			if (ret > 0)
			{
				return ret;
			}
			auto view = reg->view<div_event_et>();
			for (auto e : view) {
				auto& d = view.get<div_event_et>(e);
				if (d.mouse_motion)
				{
					ret += d.mouse_motion(pos, d.p, ts);
				}
			}
			//printf("update_move: %d\n", ret);
			return ret;
		}
		int update_move_hover(entt::registry* reg, const glm::ivec2& pos)
		{
			int ret = 0;
			int idx = -1;
			auto view = reg->view<hover_et, button_et>();
			for (auto e : view) {
				auto& d = view.get<hover_et>(e);
				auto p = d.p;
				if (p)
				{
					if (reg->has<down_et>(e))
					{
						//ret++;
						//continue;
					}
					auto ap = (glm::ivec2)p->get_pos1();
					bool ic = p->contains(pos - ap, &idx);
					d.inc = 1;
					if (ic)
					{

						//ret += p->mouse_motion(ap, 1);
						//if (!p->is_div && !p->mouse_motion_cb)
						//	ret++;
					}
					else {
						p->hit = false;
						//auto ph = p->get_hover();
						//printf("%p\n", ph);
						//if (ph)
						{
							p->isleave = true;
							reg->remove_if_exists<hover_et>(e);		// 删除鼠标离开的对象属性
						}
					}

				}
			}
			return ret;
		}

		int update_on_down(entt::registry* reg, int idx)
		{
			int ret = 0;
			reg->view<active_et>().each([&](entt::entity e) { reg->remove<active_et>(e); });	// 清掉之前的激活对象
			auto view = reg->view<hover_et, button_et>();	// 倒序遍历
			//printf("update_on_down\n");
			//ret = view.size_hint();
			//if (ret == 0)
			//	return ret;
			////printf("\tupdate_on_click\t%d\n", ret);
			//if (ret > 1)
			//	ret = ret;
			t_vector<entt::entity> ae;
			for (auto e : view) {

				//printf("update_on_down\t%d\n", ret);
				auto& hs = view.get<hover_et>(e);
				auto& btn = view.get<button_et>(e);
				//printf("%p\t%p\t%p\n", &btn, &hs, &hd);
				//printf("pos %d\t%d\n", hs.pos.x, hs.pos.y);
				btn.idx = idx;		// 按下的鼠标索引
				btn.button = 1;
				btn.pidx = hs.idx;
				btn.pos = hs.pos1;
				btn.inc = 0;
				ae.push_back(e);
				ecs_at::set_entity<down_et>(reg, e, true);
				//printf("btn.pidx %d\n", hs.idx);
				ecs_at::set_entity<active_et>(reg, e, true);
				ecs_at::set_entity<click_et>(reg, e, false);
				ret++;
			}
			if (ret > 0)
			{
				ret = ret;
			}
			return ret;
		}
		int update_on_up(entt::registry* reg, int idx)
		{
			int ret = 0;
			auto view = reg->view<button_et, down_et>();

			for (auto e : view) {
				auto& btn = view.get<button_et>(e);
				btn.button = 0;
				if (reg->has<down_et>(e))
				{
					ret++;
					//printf("remove down\n");
					reg->remove<down_et>(e);
					// 只有按下在范围内的才触发click
					if (reg->has<hover_et>(e))
					{
						click_et* pck = 0;
						ecs_at::get_ptr(reg, e, pck);
						if (pck)
						{
							pck->idx = idx;	// 0左，1右，2中
							pck->n = 0;
						}
					}
				}
				else
				{
					ecs_at::set_entity<click_et>(reg, e, false);	// 除移click状态如有
				}
			}
			return ret;
		}
		struct ckh {
			hot_et* p;
			entity e;
		};
		int update_on_click(entt::registry* reg, int idx, int count)
		{
			auto view = reg->view<click_et, hover_et, active_et /*, hot_et,visible_et*/>();
			int ret = view.size_hint();
			//printf("click %d\t%d\tn:%d\n", (int)view.size_hint(), idx, count);

#if 1
			for (auto e : view) {
#else
			t_vector<ckh> child;
			for (auto e : view) {
				auto& k = view.get<hot_et>(e);
				child.push_back({ &k ,e });
			}
			if (child.size())
			{
				std::sort(child.begin(), child.end(), [](const ckh& p1, const ckh& p2) {
					return (p1.p->order < p2.p->order) || (p1.p->order == p2.p->order && (int64_t)p1.e < (int64_t)p2.e);
					});
				auto e = child[0].e;
#endif
				click_et* pck = 0;
				ecs_at::get_ptr(reg, e, pck);
				if (pck)
				{
					bool is = pck->idx == idx;
					pck->n = count;				// 1单击/2双击/3三击
				}
				ecs_at::set_entity<click_ut>(reg, e, true);
				if (reg->has<click_cb_et>(e))
				{
					auto& kc = reg->get<click_cb_et>(e);
					if (kc.cb)
					{
						kc.cb(kc.ptr, idx, count);
					}
				}
				if (reg->has<click_cb1_et>(e))
				{
					auto& kc = reg->get<click_cb1_et>(e);
					if (kc.cb)
					{
						kc.cb(kc.ptr, idx, count);
					}
				}
			}
			return ret;
		}
		int update_on_hover(entt::registry * reg, const glm::ivec2 & pos)
		{

			auto view = reg->view<hover_et, button_et>();
			for (auto e : view) {
				auto& d = view.get<hover_et>(e);
				auto p = d.p;
				if (p)
				{
				}
			}
			return 0;
		}
		int update_on_enter2leave(entt::registry * reg, const glm::ivec2 & pos, bool is)
		{
			if (is)
			{
				int ret = 0;
				int idx = -1;
				auto view = reg->view<hover_et, button_et>();
				for (auto e : view) {
					auto& d = view.get<hover_et>(e);
					auto p = d.p;
					if (p)
					{
						p->add_mte(mouse_type_e::e_enter);
						p->isleave = false;
					}
				}
			}
			else
			{
				int ret = 0;
				int idx = -1;
				auto view = reg->view<hover_et, button_et>();
				for (auto e : view) {
					auto& d = view.get<hover_et>(e);
					auto p = d.p;
					if (p)
					{
						p->dec_mte(mouse_type_e::e_enter);
						p->hit = false;
						p->isleave = true;
						reg->remove_if_exists<hover_et>(e);		// 删除鼠标离开的对象属性						
					}
				}
			}
			return 0;
		}

		void update_on_scroll(entt::registry * reg, int wheel, int wheelH)
		{
			auto view = reg->view<hover_et, scroll_cb1_et>();
			for (auto e : view) {
				auto& d = view.get<scroll_cb1_et>(e);
				auto cb = d.cb;
				if (cb)
				{
					cb(d.ptr, wheel, wheelH);
				}
			}
		}

		void update_on_key(entt::registry * reg, event_km_t * e)
		{
		}
		void update_on_input(entt::registry * reg, const char* str, int len)
		{
		}
		// 目标拖动的事件，state：0开始，1拖动时，2结束
		int update_drag(entt::registry * reg, const glm::ivec2 & pos, const glm::ivec2 & pos1, int state)
		{
			auto view = reg->view<drag_cb_et, down_et>();
			int ret = view.size_hint();
			for (auto e : view) {
				auto& kc = view.get<drag_cb_et>(e);
				if (kc.cb)
				{
					kc.cb(kc.ptr, pos, pos1, state);
				}
			}
			auto view1 = reg->view<drag_cb1_et, down_et>();
			ret += view1.size_hint();
			//printf("update_drag %d\n", ret);
			for (auto e : view1) {
				auto& kc = view1.get<drag_cb1_et>(e);

				auto npos = pos;// -hs.pos;
				//if (state == 1)
				{
					drag_ut* pkc = 0;
					ecs_at::get_ptr(reg, e, pkc);
					assert(pkc);
					pkc->pos = npos; pkc->pos1 = pos1; pkc->state = state;
				}
				if (kc.cb)
				{
					kc.cb(kc.ptr, npos, pos1, state);
				}
				if (state == 2)
				{
					reg->remove_if_exists<drag_ut>(e);
				}
			}
			return ret;
		}
		void draw_ui(canvas_cx * cp, shape_n * p, const glm::vec2 & pos)
		{
			assert(cp);
			if (!cp)return;
			while (p)
			{
				ubo_x bu(p->mt, cp);
				auto pos1 = pos + p->pos;
				auto t = (shape_base)p->t;
				switch (t)
				{
				case shape_base::line:
				{
					cp->draw_shape(p->v.l, p->n, pos1);
				}
				break;
				case shape_base::rect:
				{
					cp->draw_shape(p->v.r, p->n, pos1);
				}
				break;
				case shape_base::ellipse:
				{
					cp->draw_shape(p->v.e, p->n, pos1);
				}
				break;
				case shape_base::circle:
				{
					cp->draw_shape(p->v.c, p->n, pos1);
				}
				break;
				case shape_base::triangle:
				{
					cp->draw_shape(p->v.t, p->n, pos1);
				}
				break;
				case shape_base::path:
				{
					cp->draw_shape(p->v.p, p->n, pos1);
				}
				break;
				case shape_base::image:
				{
					cp->draw_shape(p->v.img, p->n, -1, pos1);
				}
				break;
				case shape_base::image_m:
				{
					cp->draw_shape(p->v.imgm, p->n, -1, pos1);
				}
				break;
				case shape_base::text:
				{
					cp->draw_shape(p->v.text, p->n, pos1);
				}
				default:
					break;
				};

				p = p->pNext;
			}
		}
	}
	namespace em_render {
		void draw_item(entt::registry* reg, entt::entity e, cpg_time* t, canvas_cx* cp, const glm::ivec2& pos, font_style_c* ft_style)
		{
			if (!reg->has<hot_et>(e)) {
				return;
			}
			auto& hs = reg->get<hot_et>(e);	// 获取矩形、坐标
			auto dpos = pos + hs.pos;
			glm::ivec4 rc = hs.rc;
			//if (reg->has<size_c>(e))
			//{
			//	auto sc = reg->get<size_c>(e).s;
			//	//rc.z = sc.x; rc.w = sc.y;
			//}
			rc.x += dpos.x;
			rc.y += dpos.y;
			int is_hover = reg->has<hover_et>(e);
			int is_down = reg->has<down_et>(e);
			int inc = is_hover + is_down;
			if (reg->has<bg_c>(e))
			{
				auto& bg = reg->get<bg_c>(e);
				auto ds = &bg;
				for (int i = 0; i < inc && (ds->next); i++)
				{
					ds = (bg_c*)ds->next;
				}
				cp->draw_rect(rc, 0, ds->fill, ds->radius);
			}
			if (reg->has<content_c>(e))
			{
				auto& content = reg->get<content_c>(e);

				glm::vec4 clip = content.clip;
				clip.x += dpos.x;
				clip.y += dpos.y;
				// 裁剪
				clicp_x ccp(clip.z > 0 && clip.w > 0 ? cp : nullptr, clip);
				void* p = content.next;
				while (p)
				{
					auto kt = *(gte*)p;
					auto p1 = (geo_e*)p;
					switch (kt)
					{
						/*case dc_e::e_null:
							break;
						case dc_e::e_font_style:
							break;
						case dc_e::e_shadow:
							break;*/
					case gte::e_rect:
						break;
					case gte::e_circle:
						break;
					case gte::e_triangle:
						break;
					case gte::e_path:
						break;
					case gte::e_text:
					{
						auto t = (text_c*)p;
						item_box_info* pt = t->rbox;
						auto fs = ft_style;
						if (reg->has<font_style_c*>(e))
							fs = reg->get<font_style_c*>(e);
						pt->set_style(ft_style);
						auto lt = cp->get_layout_text();
						if (reg->has<layout_text*>(e))
							lt = reg->get<layout_text*>(e);
						pt->update(lt, t->str, 0);
						cp->draw_text(pt, dpos, t->color ? t->color : content.color);
						p = t->next;
					}
					break;
					case gte::e_image:
					{
						auto t = (image_c*)p;
						auto img = &t->img;
						auto dp = img->offset + dpos;
						auto a = glm::vec4{ 0, 0, img->rect.z, img->rect.w };
						a.x += dp.x;
						a.y += dp.y;
						cp->draw_image(img->img, a, img->rect);
						p = t->next;
					}
					break;
					default:
						auto t = (content_c*)p;
						p = t->next;
						break;
					}
				}
			}
			if (reg->has<border_c>(e))
			{
				auto& border = reg->get<border_c>(e);
				auto ds = &border;
				for (int i = 0; i < inc && (ds->next); i++)
				{
					ds = (border_c*)ds->next;
				}
				cp->draw_rect(rc, ds->color, 0, ds->radius);
			}
		}
	}

#endif // 1
#ifndef NO_FLEX


	flex_system::flex_system(entt::registry* r) :reg(r)
	{
		assert(reg);
		acb = flex_cx::get_item_func();
	}

	flex_system::~flex_system()
	{
	}

	entity flex_system::new_entity(entity* result, int n)
	{
		assert(reg);
		entity e = {};
		if (result && n > 0)
		{
			reg->create(result, result + n);
			e = *result;
		}
		else { e = reg->create(); }
		return e;
	}
	void flex_system::free_entity(entity* result, int n)
	{
		assert(reg);
		if (result && n > 0)
		{
			reg->destroy(result, result + n);
		}
		return;
	}

	flex::item_ptr_t* flex_system::new_item(entity e)
	{
		flex::item_ptr_t* p = 0;
		if (ecs_at::get_ptr(reg, e, p))
		{
			if (p->item)
				p->item = flex_item_new(1);
		}
		return p;
	}
	void flex_system::free_item(entity e)
	{
		flex::item_ptr_t* p = 0;
		if (ecs_at::get_ptr(reg, e, p))
		{
			flex_item_free(p->item);
		}
		return;
	}

	void flex_system::item_add(entity e, entity c)
	{
		flex::item_ptr_t* p = 0;
		flex::item_ptr_t* p1 = 0;
		if (ecs_at::get_ptr(reg, e, p) && ecs_at::get_ptr(reg, c, p1))
		{
			flex_item_add(p->item, p1->item);
		}
		return;
	}

	void flex_system::item_detach(entity e)
	{
		flex::item_ptr_t* p = 0;
		if (ecs_at::get_ptr(reg, e, p))
		{
			flex_item_detach(flex_item_parent(p->item), p->item);
		}
	}


	void flex_system::item_make(entity e)
	{
		flex::item_ptr_t* p = 0;
		flex::flex_size_t* p0 = 0;
		flex::flex_rect_t* p1 = 0;
		flex::flex_padding_t* p2 = 0;
		flex::flex_margin_t* p3 = 0;
		flex::flex_align_t* p4 = 0;
		flex::flex_wrap_t* p5 = 0;
		flex::flex_base_t* p6 = 0;

		if (!ecs_at::get_ptr0(reg, e, p))
			return;
		if (ecs_at::get_ptr0(reg, e, p0))
		{
			acb->item_set_width(p->item, p0->width);
			acb->item_set_height(p->item, p0->height);
		}
		if (ecs_at::get_ptr0(reg, e, p1))
		{
			acb->item_set_left(p->item, p1->left);
			acb->item_set_right(p->item, p1->right);
			acb->item_set_top(p->item, p1->top);
			acb->item_set_bottom(p->item, p1->bottom);
		}
		if (ecs_at::get_ptr0(reg, e, p2))
		{
			acb->item_set_padding_left(p->item, p2->left);
			acb->item_set_padding_right(p->item, p2->right);
			acb->item_set_padding_top(p->item, p2->top);
			acb->item_set_padding_bottom(p->item, p2->bottom);
		}
		if (ecs_at::get_ptr0(reg, e, p3))
		{
			acb->item_set_margin_left(p->item, p3->left);
			acb->item_set_margin_right(p->item, p3->right);
			acb->item_set_margin_top(p->item, p3->top);
			acb->item_set_margin_bottom(p->item, p3->bottom);
		}
		if (ecs_at::get_ptr0(reg, e, p4))
		{
			acb->item_set_justify_content(p->item, (flex_align)p4->justify_content);
			acb->item_set_align_content(p->item, (flex_align)p4->align_content);
			acb->item_set_align_items(p->item, (flex_align)p4->align_items);
			acb->item_set_align_self(p->item, (flex_align)p4->align_self);
		}
		if (ecs_at::get_ptr0(reg, e, p5))
		{
			acb->item_set_position(p->item, (flex_position)p5->position);
			acb->item_set_direction(p->item, (flex_direction)p5->direction);
			acb->item_set_wrap(p->item, (flex_wrap)p5->wrap);
		}
		if (ecs_at::get_ptr0(reg, e, p6))
		{
			acb->item_set_grow(p->item, p6->grow);
			acb->item_set_shrink(p->item, p6->shrink);
			acb->item_set_order(p->item, p6->order);
			acb->item_set_basis(p->item, p6->basis);
		}
	}
	void flex_system::do_layout(entity e)
	{
		flex::item_ptr_t* p = 0;
		if (ecs_at::get_ptr(reg, e, p))
		{
			flex_layout(p->item);
		}
	}
	glm::vec4 flex_system::get_merge(entity e)
	{
		glm::vec4 ret;
		flex::item_ptr_t* p = 0;
		if (ecs_at::get_ptr(reg, e, p))
		{
			auto pos1 = flex_cx::get_item_merge_pos(p->item);
			auto size = flex_cx::get_item_size(p->item);
			ret = { pos1, size };
		}
		return ret;
	}

	void flex_system::set_size(entity e, const glm::vec2& s)
	{
		flex::flex_size_t* ps = 0;
		if (ecs_at::get_ptr(reg, e, ps))
		{
			ps->width = s.x;
			ps->height = s.y;
		}
	}

	void flex_system::set_rect_pos(entity e, const glm::vec4& s)
	{
		flex::flex_rect_t* p = 0;
		if (ecs_at::get_ptr(reg, e, p))
		{
			memcpy(p, &s, sizeof(float) * 4);
		}
	}

	void flex_system::set_padding(entity e, const glm::vec4& s)
	{
		flex::flex_padding_t* p = 0;
		if (ecs_at::get_ptr(reg, e, p))
		{
			memcpy(p, &s, sizeof(float) * 4);
		}
	}

	void flex_system::set_margin(entity e, const glm::vec4& s)
	{
		flex::flex_margin_t* p = 0;
		if (ecs_at::get_ptr(reg, e, p))
		{
			memcpy(p, &s, sizeof(float) * 4);
		}
	}

	void flex_system::set_align(entity e, flex::e_align justify_content, flex::e_align align_content, flex::e_align align_items, flex::e_align align_self)
	{
		flex::flex_align_t* p = 0;
		if (ecs_at::get_ptr(reg, e, p))
		{
			p->justify_content = justify_content;
			p->align_content = align_content;
			p->align_items = align_items;
			p->align_self = align_self;
		}
	}

	void flex_system::set_align_justify_content(entity e, flex::e_align justify_content)
	{
		flex::flex_align_t* p = 0;
		if (ecs_at::get_ptr(reg, e, p))
		{
			p->justify_content = justify_content;
		}
	}

	void flex_system::set_align_align_content(entity e, flex::e_align align_content)
	{
		flex::flex_align_t* p = 0;
		if (ecs_at::get_ptr(reg, e, p))
		{
			p->align_content = align_content;
		}
	}

	void flex_system::set_align_align_items(entity e, flex::e_align align_items)
	{
		flex::flex_align_t* p = 0;
		if (ecs_at::get_ptr(reg, e, p))
		{
			p->align_items = align_items;
		}
	}

	void flex_system::set_align_align_self(entity e, flex::e_align align_self)
	{
		flex::flex_align_t* p = 0;
		if (ecs_at::get_ptr(reg, e, p))
		{
			p->align_self = align_self;
		}
	}

	void flex_system::set_align(entity e, const glm::ivec4& s)
	{
		set_align(e, (flex::e_align)s.x, (flex::e_align)s.y, (flex::e_align)s.z, (flex::e_align)s.w);
	}

	void flex_system::set_wraps(entity e, flex::e_position position, flex::e_direction direction, flex::e_wrap wrap)
	{
		flex::flex_wrap_t* p = 0;
		if (ecs_at::get_ptr(reg, e, p))
		{
			p->wrap = wrap;
			p->position = position;
			p->direction = direction;
		}
	}

	void flex_system::set_position(entity e, flex::e_position position)
	{
		flex::flex_wrap_t* p = 0;
		if (ecs_at::get_ptr(reg, e, p))
		{
			p->position = position;
		}
	}

	void flex_system::set_direction(entity e, flex::e_direction direction)
	{
		flex::flex_wrap_t* p = 0;
		if (ecs_at::get_ptr(reg, e, p))
		{
			p->direction = direction;
		}
	}

	void flex_system::set_wrap(entity e, flex::e_wrap wrap)
	{
		flex::flex_wrap_t* p = 0;
		if (ecs_at::get_ptr(reg, e, p))
		{
			p->wrap = wrap;
		}
	}

	void flex_system::set_wraps(entity e, const glm::ivec3& s)
	{
		set_wraps(e, (flex::e_position)s.x, (flex::e_direction)s.y, (flex::e_wrap)s.z);
	}

	void flex_system::set_base(entity e, float grow, float shrink, float order, float basis)
	{
		set_base(e, { grow, shrink, order, basis });
	}

	void flex_system::set_base(entity e, const glm::vec4& s)
	{
		flex::flex_base_t* p = 0;
		if (ecs_at::get_ptr(reg, e, p))
		{
			memcpy(p, &s, sizeof(float) * 4);
		}
	}

#endif // !NO_FLEX

#if 1
	fbo_m::fbo_m(world_cx* wdc, int queueidx)
	{
		auto dev = wdc->get_dev();
		glm::ivec4 viewport = { 0, 0, 1024, 1024 };
		_page = new page_view();
		psub = new submit_infos();
		_fence = new dvk_fence(dev->device);
		//if (is_event)_em = new event_master();
		canvas = _page->new_canvas(true);
		int first = 0, count = 1;
		canvas->set_viewport(viewport);		// 设置视区位置大小
		//if (_em)_em->set_viewport(viewport);
		_page->init(wdc->_base3d, wdc->get_ftctx()); // 设置shader			vulkan
		dq = dev->get_graphics_queue(queueidx);	// 获取第1个列队，设备没有两个列队就返回第0个

		assert(dq);
		auto vkctx = wdc->get_vkctx();
		// 创建渲染到纹理的fbo
		//fbo_cx* new_fbo_image(int width, int height, int count, unsigned int clear_color, dvk_device* dev, void* pass = nullptr);
		fbo1 = vkctx->new_fbo_image(viewport.z, viewport.w, 1, 0, dev, wdc->_render_pass);
		fbo1->set_queue(dq, 0);						// 设置列队创建cmd_pool
		psub->push(fbo1->get_submit(0));		// 获取提交信息
		// 录制绘图命令回调函数
		fbo1->push_drawcb([](dvk_cmd* cmd, page_view* page, canvas_cx** c)
			{
				page->draw_call(cmd, c, 1);
			}, _page, &canvas);
	}

	fbo_m::~fbo_m()
	{
		delete _page;
		delete psub;
		delete _fence;
		_page = 0;
		psub = 0;
		_fence = 0;
		//if (_em)delete _em;		_em = 0;
	}
	void fbo_m::set_view(glm::ivec2 size, unsigned int color)
	{
		fbo1->resize(size.x, size.y);
		fbo1->dev_resize(false);
		fbo1->set_clear_color(color);
		glm::ivec4 viewport = { 0, 0, size.x, size.y };
		canvas->set_viewport(viewport);		// 设置视区位置大小
		//if (_em)_em->set_viewport(viewport);
	}
	canvas_cx* fbo_m::g_begin()
	{
		canvas->clear();
		return canvas;
	}
	void fbo_m::g_end()
	{
		fbo1->build_cmd_cb(); //生成命令
		//auto sp = fbo1->get_cur_submit();
		//psub->update_sub(sp);
// 更新数据到显存(顶点索引纹理ubo等)
		_page->update_data();
	}
	void fbo_m::add_data(const void* p, size_t n, size_t stride, size_t offset)
	{
		canvas->add_obj(p, n, stride, offset);
	}

	void fbo_m::submit()
	{
		dq->submit(psub, _fence, true);	// 提交列队，执行完再返回，也可以异步获取
	}

	void fbo_m::to_image(Image* img, int idx)
	{
		fbo1->to_image(img, idx);
	}
	void fbo_m::save_image(const std::string& fn)
	{
		fbo1->save_image(fn, 0);
	}
	void fbo_m::set_unm(bool is)
	{
		if (fbo1)
			fbo1->unm = is;
	}
#endif


	void edit_es_t::set_color(uint32_t text_color, uint32_t sele_color, uint32_t cursor_color)
	{
		_text_color = text_color;
		_cursor_color = cursor_color;
		if (sele_color)
			selecol = sele_color;
	}

}
// !hz
