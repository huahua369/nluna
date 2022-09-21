
#include <thread>
#include <set>
#include <vlcpp/vlc.hpp>
//#include <vlc/plugins/vlc_fourcc.h>
#if 1

class media_player_t
{
public:
	int w = 0, h = 0;
	int length = 0;
	libvlc_media_player_t* mp = 0;
	void* hwnd = 0;

	//std::queue<std::vector<uint32_t>> video_queue;
	std::vector<char> pixel;
	int64_t idx = 0;					//当前显示帧
	std::mutex mtx;
	std::atomic_bool _ready = false;
	int planes[3] = {};
public:
	media_player_t();
	~media_player_t();
	void set_buffer();
	void set_hwnd(void* hwnd);

	void set_pos(float ps);
	void play();
	void stop();
	void pause(bool is);
	void set_volume(int v);
	int get_volume();
public:
	void release();
private:

};

class easyvlc_ctx
{
public:
	libvlc_instance_t* inst = 0;
	std::set<media_player_t*> mpv;
public:
	easyvlc_ctx();
	~easyvlc_ctx();
	void init();
	media_player_t* new_media_player(const char* path);
	void free_mp(media_player_t* mp);
private:

};

void* cb_lock(void* opaque, void** plane);
void cb_unlock(void* opaque, void* picture, void* const* plane);
void cb_display(void* opaque, void* picture);
unsigned int video_format_cb(void** opaque, char* chroma, unsigned* width, unsigned* height, unsigned* pitches, unsigned* lines);
void video_cleanup_cb(void* opaque);



media_player_t::media_player_t()
{

}

media_player_t::~media_player_t()
{
	stop();
	release();
}
void media_player_t::set_buffer()
{
	libvlc_video_set_callbacks(mp, &cb_lock, &cb_unlock, &cb_display, this);//设置回调函数
	libvlc_video_set_format_callbacks(mp, video_format_cb, video_cleanup_cb);
}
void media_player_t::set_hwnd(void* hwnd)
{
	//auto hwnd = GetDesktopWindow();
	if (hwnd)
		libvlc_media_player_set_hwnd(mp, hwnd);
	return;
}
void media_player_t::set_pos(float ps)
{
	libvlc_media_player_set_position(mp, ps);
}
void media_player_t::play()
{
	libvlc_media_player_play(mp);
	//wait until the tracks are created
	if (!_ready)
	{
		while (!_ready && length == 0)
		{
			_sleep(10);
			length = libvlc_media_player_get_length(mp);
		}
		_ready = true;
		auto width = libvlc_video_get_width(mp);
		auto height = libvlc_video_get_height(mp);
		w = width;
		h = height;
		//在video_format_cb设置了
		//libvlc_video_set_format(mp, "RGBA", w, h, w * 4);//设置取流参数 以RGBA格式取流
		//*libvlc_video_set_format(mp, "I420", w, h, w);//设置取流参数 以RGBA格式取流
		//pixel.resize(w * h);
		printf("Stream Duration: %ds\n", (int)(length / 1000));
		//printf("Resolution: %d x %d\n", width, height);
	}
}
void media_player_t::stop()
{
	// Stop playing
	libvlc_media_player_stop(mp);
}
void media_player_t::pause(bool is)
{
	libvlc_media_player_set_pause(mp, is ? 1 : 0);
}
void media_player_t::set_volume(int v)
{
	libvlc_audio_set_volume(mp, v);
}
int media_player_t::get_volume()
{
	return libvlc_audio_get_volume(mp);
}
void media_player_t::release()
{
	// Free the media_player
	libvlc_media_player_release(mp);
}

easyvlc_ctx::easyvlc_ctx()
{
}

easyvlc_ctx::~easyvlc_ctx()
{
	for (auto it : mpv)
		delete it;
	mpv.clear();
	if (inst)
		libvlc_release(inst);
	inst = 0;
}

void easyvlc_ctx::init()
{
	const char* cmdstr = "--direct3d11-hw-blending";
	inst = libvlc_new(0, 0);
}

media_player_t* easyvlc_ctx::new_media_player(const char* path)
{
	auto m = libvlc_media_new_path(inst, path);
	libvlc_media_add_option(m, ":avcodec-hw=any");
	/* Create a media player playing environement */
	auto mp = libvlc_media_player_new_from_media(m);
	media_player_t* p = 0;
	if (!mp)
	{
		return p;
	}
	p = new media_player_t();
	p->mp = mp;
	//libvlc_audio_set_callbacks(mp, on_audio_play, on_audio_pause, on_audio_resume, on_audio_flush, on_audio_drain, ectx);
	//libvlc_audio_set_volume_callback(mp, on_audio_set_volume);
	/* No need to keep the media now */
	libvlc_media_release(m);
	mpv.insert(p);
	return p;

}
void easyvlc_ctx::free_mp(media_player_t* mp)
{
	if (mp)
	{

		mpv.erase(mp);
		delete mp;
	}
}
void* cb_lock(void* opaque, void** plane)
{
	media_player_t* p = (media_player_t*)opaque;
	auto lk = &p->mtx;
	lk->lock();
	if (p->pixel.empty())
	{
		assert(0);
	}
	char* t = (char*)p->pixel.data();
	for (size_t i = 0; i < 3; i++)
	{
		plane[i] = t; t += p->planes[i];
	}
	return (void*)1;
}

void cb_unlock(void* opaque, void* picture, void* const* plane)
{
	media_player_t* p = (media_player_t*)opaque;
	unsigned char* pix = (unsigned char*)plane[0];
	unsigned char* pix1 = (unsigned char*)plane[1];
	unsigned char* pix2 = (unsigned char*)plane[2];
	unsigned int* pix4 = (unsigned int*)picture;
	auto lk = &p->mtx;

	if (pix)
	{
		auto fn = "h:\\tem\\" + std::to_string(p->idx) + ".jpg";
		auto fn1 = "h:\\tem\\" + std::to_string(p->idx) + ".png";
		//stbi_write_jpg(fn.c_str(), (int)p->w, (int)p->h, 4, pix, 90);
		//stbi_write_png(fn1.c_str(), (int)p->w, (int)p->h, 4, pix, 0);
		//std::vector<uint32_t> img;
		//img.resize(p->w * p->h);
		//memcpy(img.data(), pix, img.size() * sizeof(uint32_t));
		//if (p->video_queue.size() < MAX_QUEUE)
		//{
		//	p->video_queue.push(img);
		//}
		//else
		//{
		//	std::queue<std::vector<uint32_t>> empty;
		//	std::swap(empty, p->video_queue);
		//}
		//p->idx++;
	}
	else {
		printf("miss pix\n");
	}

	lk->unlock();
	//p->mtx.unlock();
}
void cb_display(void* opaque, void* picture)
{
	media_player_t* p = (media_player_t*)opaque;
	//printf("display\n");
	auto k = p->get_volume();
	int v = -1;
	if (v >= 0)
	{
		p->set_volume(v);
	}
}
// align val to the next multiple of alignment
inline size_t align_up(size_t val, size_t alignment)
{
	return (val + alignment - (size_t)1) & ~(alignment - (size_t)1);
}
unsigned int video_format_cb(void** opaque, char* chroma, unsigned* width, unsigned* height, unsigned* pitches, unsigned* lines)
{
	media_player_t* p = (media_player_t*)*opaque;
	p->w = *width;
	p->h = *height;
	/*
	"I420" (YUV420)		大小：height*wight*1.5
	"RGBA"				大小： height*wight*4
	*/
	//VLC_CODEC_I420
	//strcpy(chroma, "RGBA");
	strcpy(chroma, "I420");
	//*pitches = p->w * 4;
	//*lines = p->h;
	int64_t as = 0;
	for (size_t i = 0; i < 3; i++)
	{
		pitches[i] = (i > 0) ? (*width + 1) / 2 : *width;
		lines[i] = (i > 0) ? (*height + 1) / 2 : *height;
		p->planes[i] = pitches[i] * lines[i];
		as += p->planes[i];
	}
	auto oldas = as;
	as = align_up(as, 256);
	if (p->pixel.empty())
	{
		p->pixel.resize(as);
	}
	p->_ready = true;
	printf("video_format:%d,%d\n", *width, *height);
	return as;
}

void video_cleanup_cb(void* opaque)
{
	printf("video_cleanup_cb\n");
}
/**
 * Callback prototype for audio playback.
 *
 * The LibVLC media player decodes and post-processes the audio signal
 * asynchronously (in an internal thread). Whenever audio samples are ready
 * to be queued to the output, this callback is invoked.
 *
 * The number of samples provided per invocation may depend on the file format,
 * the audio coding algorithm, the decoder plug-in, the post-processing
 * filters and timing. Application must not assume a certain number of samples.
 *
 * The exact format of audio samples is determined by libvlc_audio_set_format()
 * or libvlc_audio_set_format_callbacks() as is the channels layout.
 *
 * Note that the number of samples is per channel. For instance, if the audio
 * track sampling rate is 48000 Hz, then 1200 samples represent 25 milliseconds
 * of audio signal - regardless of the number of audio channels.
 *
 * \param data data pointer as passed to libvlc_audio_set_callbacks() [IN]
 * \param samples pointer to a table of audio samples to play back [IN]
 * \param count number of audio samples to play back
 * \param pts expected play time stamp (see libvlc_delay())
 */
void on_audio_play(void* data, const void* samples, unsigned count, int64_t pts)
{
	easyvlc_ctx* p = (easyvlc_ctx*)data;

	printf("on_audio_play\n");
}
/**
 * Callback prototype for audio pause.
 *
 * LibVLC invokes this callback to pause audio playback.
 *
 * \note The pause callback is never called if the audio is already paused.
 * \param data data pointer as passed to libvlc_audio_set_callbacks() [IN]
 * \param pts time stamp of the pause request (should be elapsed already)
 */
void on_audio_pause(void* data, int64_t pts)
{

}

/**
 * Callback prototype for audio resumption.
 *
 * LibVLC invokes this callback to resume audio playback after it was
 * previously paused.
 *
 * \note The resume callback is never called if the audio is not paused.
 * \param data data pointer as passed to libvlc_audio_set_callbacks() [IN]
 * \param pts time stamp of the resumption request (should be elapsed already)
 */
void on_audio_resume(void* data, int64_t pts)
{

}

/**
 * Callback prototype for audio buffer flush.
 *
 * LibVLC invokes this callback if it needs to discard all pending buffers and
 * stop playback as soon as possible. This typically occurs when the media is
 * stopped.
 *
 * \param data data pointer as passed to libvlc_audio_set_callbacks() [IN]
 */
void on_audio_flush(void* data, int64_t pts)
{

}

/**
 * Callback prototype for audio buffer drain.
 *
 * LibVLC may invoke this callback when the decoded audio track is ending.
 * There will be no further decoded samples for the track, but playback should
 * nevertheless continue until all already pending buffers are rendered.
 *
 * \param data data pointer as passed to libvlc_audio_set_callbacks() [IN]
 */
void on_audio_drain(void* data)
{

}

/**
 * Callback prototype for audio volume change.
 * \param data data pointer as passed to libvlc_audio_set_callbacks() [IN]
 * \param volume software volume (1. = nominal, 0. = mute)
 * \param mute muted flag
 */
void on_audio_set_volume(void* data, float volume, bool mute)
{

}



#endif

void vlc_play(const char* fn, bool* isplay)
{
	std::thread a([=]() {

		easyvlc_ctx* ectx = new easyvlc_ctx();
		ectx->init();

		do {
			auto p = ectx->new_media_player(fn);
			p->set_buffer();//自定义播放设置buffer
			//p->set_hwnd(0);//设置或创建窗口播放
			p->play();
			_sleep(p->length);
			ectx->free_mp(p);
			//p->set_pos(0);
		} while (*isplay);
		delete ectx;
		});
	a.detach();
}