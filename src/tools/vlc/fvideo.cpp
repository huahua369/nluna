/*
 * Copyright (c) 2003 Fabrice Bellard
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

 /**
  * @file
  * simple media player based on the FFmpeg libraries
  */
  //#define CONFIG_AVFILTER 1

#include "config.h"
#include <inttypes.h>
#include <math.h>
#include <limits.h>
#include <signal.h>
#include <stdint.h>
#include <data/json_helper.h>
#include <view/mapView.h>
#include <vk_core/view_info.h>
#ifdef __has_include
#if (__has_include(<base/print_time.h>))
#include <base/print_time.h>
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif

#include "config_components.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avassert.h"
#include "libavutil/avstring.h"
#include "libavutil/channel_layout.h"
#include "libavutil/eval.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/fifo.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/time.h"
#include "libavutil/bprint.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavcodec/avfft.h"
#include "libswresample/swresample.h"

#include "libavcodec/bsf.h"
#include "libavutil/cpu.h"

	//#include "libavutil/avstring.h"
	//#include "libavutil/pixdesc.h"
	//#include "libavfilter/buffersink.h"

#if CONFIG_AVFILTER
# include "libavfilter/avfilter.h"
# include "libavfilter/buffersink.h"
# include "libavfilter/buffersrc.h"
#endif


#include "cmdutils.h"
//#include "ffmpeg.h"
	//#include "opt_common.h"
#ifdef __cplusplus
}
#endif
#include <SDL.h>
#include <SDL_thread.h>
#include "ffp.h"

const char program_name[] = "ffplay";
const int program_birth_year = 2003;

#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MIN_FRAMES 25
#define EXTERNAL_CLOCK_MIN_FRAMES 2
#define EXTERNAL_CLOCK_MAX_FRAMES 10

/* Minimum SDL audio buffer size, in samples. */
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
/* Calculate actual buffer size keeping in mind not cause too frequent audio callbacks */
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30

/* Step size for volume control in dB */
#define SDL_VOLUME_STEP (0.75)

/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 0.04
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 0.1
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0

/* maximum audio speed change to get correct sync */
#define SAMPLE_CORRECTION_PERCENT_MAX 10

/* external clock speed adjustment constants for realtime sources based on buffer fullness */
#define EXTERNAL_CLOCK_SPEED_MIN  0.900
#define EXTERNAL_CLOCK_SPEED_MAX  1.010
#define EXTERNAL_CLOCK_SPEED_STEP 0.001

/* we use about AUDIO_DIFF_AVG_NB A-V differences to make the average */
#define AUDIO_DIFF_AVG_NB   20

/* polls for possible required screen refresh at least this often, should be less than 1/fps */
#define REFRESH_RATE 0.01

/* NOTE: the size must be big enough to compensate the hardware audio buffersize size */
/* TODO: We assume that a decoded and resampled frame fits into this buffer */
#define SAMPLE_ARRAY_SIZE (8 * 65536)

#define CURSOR_HIDE_DELAY 1000000

#define USE_ONEPASS_SUBTITLE_RENDER 1




static unsigned sws_flags = SWS_BICUBIC;

typedef struct MyAVPacketList {
	AVPacket* pkt;
	int serial;
} MyAVPacketList;

typedef struct PacketQueue {
	AVFifo* pkt_list;
	int nb_packets;
	int size;
	int64_t duration;
	int abort_request;
	int serial;
	SDL_mutex* mutex;
	SDL_cond* cond;

} PacketQueue;

#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE 16
#define SAMPLE_QUEUE_SIZE 9
#define FRAME_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))

typedef struct AudioParams {
	int freq;
	AVChannelLayout ch_layout;
	enum AVSampleFormat fmt;
	int frame_size;
	int bytes_per_sec;
} AudioParams;

typedef struct Clock {
	double pts;           /* clock base */
	double pts_drift;     /* clock base minus time at which we updated the clock */
	double last_updated;
	double speed;
	int serial;           /* clock is based on a packet with this serial */
	int paused;
	int* queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
} Clock;

/* Common struct for handling all types of decoded data and allocated render buffers. */
typedef struct Frame {
	AVFrame* frame;
	AVSubtitle sub;
	int serial;
	double pts;           /* presentation timestamp for the frame */
	double duration;      /* estimated duration of the frame */
	int64_t pos;          /* byte position of the frame in the input file */
	int width;
	int height;
	int format;
	AVRational sar;
	int uploaded;
	int flip_v;
} Frame;

typedef struct FrameQueue {
	Frame queue[FRAME_QUEUE_SIZE];
	int rindex;
	int windex;
	int size;
	int max_size;
	int keep_last;
	int rindex_shown;
	SDL_mutex* mutex;
	SDL_cond* cond;
	PacketQueue* pktq;
} FrameQueue;

enum {
	AV_SYNC_AUDIO_MASTER, /* default choice */
	AV_SYNC_VIDEO_MASTER,
	AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
};

enum ShowMode {
	SHOW_MODE_NONE = -1, SHOW_MODE_VIDEO = 0, SHOW_MODE_WAVES, SHOW_MODE_RDFT, SHOW_MODE_NB
};
typedef struct Decoder {
	AVPacket* pkt;
	PacketQueue* queue;
	AVCodecContext* avctx;
	int pkt_serial;
	int finished;
	int packet_pending;
	SDL_cond* empty_queue_cond;
	int64_t start_pts;
	AVRational start_pts_tb;
	int64_t next_pts;
	AVRational next_pts_tb;
	SDL_Thread* decoder_tid;
} Decoder;

typedef struct VideoState {
	SDL_Thread* read_tid;
	const AVInputFormat* iformat;
	int abort_request;
	int force_refresh;
	int paused;
	int last_paused;
	int queue_attachments_req;
	int seek_req;
	int seek_flags;
	int64_t seek_pos;
	int64_t seek_rel;
	int read_pause_return;
	AVFormatContext* ic;
	int realtime;

	Clock audclk;
	Clock vidclk;
	Clock extclk;

	FrameQueue pictq;
	FrameQueue subpq;
	FrameQueue sampq;

	Decoder auddec;
	Decoder viddec;
	Decoder subdec;

	int audio_stream;

	int av_sync_type;

	double audio_clock;
	int audio_clock_serial;
	double audio_diff_cum; /* used for AV difference average computation */
	double audio_diff_avg_coef;
	double audio_diff_threshold;
	int audio_diff_avg_count;
	AVStream* audio_st;
	PacketQueue audioq;
	int audio_hw_buf_size;
	uint8_t* audio_buf;
	uint8_t* audio_buf1;
	unsigned int audio_buf_size; /* in bytes */
	unsigned int audio_buf1_size;
	int audio_buf_index; /* in bytes */
	int audio_write_buf_size;
	int audio_volume;
	int muted;
	struct AudioParams audio_src;
#if CONFIG_AVFILTER
	struct AudioParams audio_filter_src;
#endif
	struct AudioParams audio_tgt;
	struct SwrContext* swr_ctx;
	int frame_drops_early;
	int frame_drops_late;

	enum ShowMode show_mode;
	int16_t sample_array[SAMPLE_ARRAY_SIZE];
	int sample_array_index;
	int last_i_start;
	RDFTContext* rdft;
	int rdft_bits;
	FFTSample* rdft_data;
	int xpos;
	double last_vis_time;
	SDL_Texture* vis_texture;
	SDL_Texture* sub_texture;
	SDL_Texture* vid_texture;

	int subtitle_stream;
	AVStream* subtitle_st;
	PacketQueue subtitleq;

	double frame_timer;
	double frame_last_returned_time;
	double frame_last_filter_delay;
	int video_stream;
	AVStream* video_st;
	PacketQueue videoq;
	double max_frame_duration;      // maximum duration of a frame - above this, we consider the jump a timestamp discontinuity
	struct SwsContext* img_convert_ctx;
	struct SwsContext* sub_convert_ctx;
	int eof;

	char* filename;
	int width, height, xleft, ytop;
	int step;

#if CONFIG_AVFILTER
	int vfilter_idx;
	AVFilterContext* in_video_filter;   // the first filter in the video chain
	AVFilterContext* out_video_filter;  // the last filter in the video chain
	AVFilterContext* in_audio_filter;   // the first filter in the audio chain
	AVFilterContext* out_audio_filter;  // the last filter in the audio chain
	AVFilterGraph* agraph;              // audio filter graph
#endif

	int last_video_stream, last_audio_stream, last_subtitle_stream;

	SDL_cond* continue_read_thread;
	void(*dcb)(yuv_info_t*);
	// 用户指针
	void* userptr = 0;

} VideoState;

struct hwdev_t
{
	AVHWDeviceType t = {};
	AVBufferRef* hdc = 0;//hw_device_ctx
	std::string name;
public:
	AVBufferRef* a() {
		return av_buffer_ref(hdc);
	}
	void unref() {
		auto n = av_buffer_get_ref_count(hdc);
		auto p = hdc;
		if (n > 0)
			av_buffer_unref(&p);
	}
};

class play_ctx
{
public:
	/* options specified by the user */
	const AVInputFormat* file_iformat = 0;
	const char* input_filename = 0;
	const char* window_title = 0;
	int default_width = 640;
	int default_height = 480;
	int screen_width = 0;
	int screen_height = 0;
	int screen_left = SDL_WINDOWPOS_CENTERED;
	int screen_top = SDL_WINDOWPOS_CENTERED;
	int audio_disable = 0;
	int video_disable = 0;
	int subtitle_disable = 0;
	const char* wanted_stream_spec[AVMEDIA_TYPE_NB] = { 0 };
	int seek_by_bytes = -1;
	float seek_interval = 10;
	int display_disable = 0;
	int borderless = 0;
	int alwaysontop = 0;
	int startup_volume = 100;
	int show_status = -1;
	int av_sync_type = AV_SYNC_AUDIO_MASTER;
	int64_t start_time = AV_NOPTS_VALUE;
	int64_t duration = AV_NOPTS_VALUE;
	int fast = 0;
	int genpts = 0;
	int lowres = 0;
	int decoder_reorder_pts = -1;
	int autoexit = 0;
	int exit_on_keydown = 0;
	int exit_on_mousedown = 0;
	int ploop = 1;
	int framedrop = -1;
	int infinite_buffer = -1;
	enum ShowMode show_mode = SHOW_MODE_NONE;
	const char* audio_codec_name = 0;
	const char* subtitle_codec_name = 0;
	const char* video_codec_name = 0;
	double rdftspeed = 0.02;
	int64_t cursor_last_shown = 0;
	int cursor_hidden = 0;
#if CONFIG_AVFILTER
	const char** vfilters_list = NULL;
	int nb_vfilters = 0;
	char* afilters = NULL;
#endif
	int autorotate = 1;
	int find_stream_info = 1;
	int filter_nbthreads = 0;
	bool ispushaudio = false;

	/* current context */
	bool is_full_screen = 0;
	// 复制到内存。也就是hwframe
	bool is_cp2mem = false;
	int64_t audio_callback_time = 0;

#define FF_QUIT_EVENT    (SDL_USEREVENT + 2)

	SDL_Window* window = {};
	SDL_Renderer* renderer = {};
	SDL_RendererInfo renderer_info = { 0 };
	SDL_AudioDeviceID audio_dev = {};


	int dummy;
	std::vector<OptionDef> options;
	std::vector<Uint8> abufv;
	// 音频线程
	std::thread audiopt;
	std::atomic_int16_t is_ctrl, thr_count;
	ctrl_data_t* pc = 0;
	VideoState* tis = 0;

	AVPixelFormat hw_device_pixel = {};
	hwdev_t* hwctx = {};
	AVFrame* hwframe = 0;
	// 事件
	int et = 1;
public:
	play_ctx() { is_ctrl = 0; }
	~play_ctx() {}
	int opt_add_vfilter(void* optctx, const char* opt, const char* arg);
	int decoder_decode_frame(Decoder* d, AVFrame* frame, AVSubtitle* sub);
	void fill_rectangle(int x, int y, int w, int h);
	int realloc_texture(SDL_Texture** texture, Uint32 new_format, int new_width, int new_height, SDL_BlendMode blendmode, int init_texture);
	int upload_texture(SDL_Texture** tex, AVFrame* frame, struct SwsContext** img_convert_ctx);
	void video_image_display(VideoState* is);
	void video_audio_display(VideoState* s);
	void video_image_display1(VideoState* is);
	void stream_component_close(VideoState* is, int stream_index);
	void do_exit(VideoState* is);
	void set_default_window_size(int width, int height, AVRational sar);
	int video_open(VideoState* is);
	void video_display(VideoState* is);
	void video_refresh(void* opaque, double* remaining_time);
	int queue_picture(VideoState* is, AVFrame* src_frame, double pts, double duration, int64_t pos, int serial);
	int get_video_frame(VideoState* is, AVFrame* frame);
	int configure_video_filters(AVFilterGraph* graph, VideoState* is, const char* vfilters, AVFrame* frame);
	int configure_audio_filters(VideoState* is, const char* afilters, int force_output_format);
	int audio_thread(void* arg);
	int video_thread(void* arg);
	int subtitle_thread(void* arg);
	int audio_decode_frame(VideoState* is);
	void sdl_audio_callback(void* opaque, Uint8* stream, int len);
	int audio_open(void* opaque, AVChannelLayout* wanted_channel_layout, int wanted_sample_rate, struct AudioParams* audio_hw_params);
	int stream_component_open(VideoState* is, int stream_index, const AVCodec* codec = nullptr);
	void stream_close(VideoState* is);
	int read_thread(void* arg);
	VideoState* stream_open(const char* filename, const AVInputFormat* iformat);
	void stream_cycle_channel(VideoState* is, int codec_type);
	void toggle_full_screen(VideoState* is);
	void toggle_audio_display(VideoState* is);
	void refresh_loop_wait_event(VideoState* is, SDL_Event* event);
	void refresh_wait_event(VideoState* is);
	void upctrl(ctrl_data_t* p);
	void event_loop(VideoState* cur_stream);
	void event_loop1(VideoState* cur_stream);
	int opt_width(void* optctx, const char* opt, const char* arg);
	int opt_height(void* optctx, const char* opt, const char* arg);
	int opt_format(void* optctx, const char* opt, const char* arg);
	int opt_sync(void* optctx, const char* opt, const char* arg);
	int opt_seek(void* optctx, const char* opt, const char* arg);
	int opt_duration(void* optctx, const char* opt, const char* arg);
	int opt_show_mode(void* optctx, const char* opt, const char* arg);
	void opt_input_file(void* optctx, const char* filename);
	int opt_codec(void* optctx, const char* opt, const char* arg);

	void new_display();

	void push_audio_thread();
};
static const struct TextureFormatEntry {
	enum AVPixelFormat format;
	int texture_fmt;
} sdl_texture_format_map[] =
{
	{ AV_PIX_FMT_RGB8,           SDL_PIXELFORMAT_RGB332 },
	{ AV_PIX_FMT_RGB444,         SDL_PIXELFORMAT_RGB444 },
	{ AV_PIX_FMT_RGB555,         SDL_PIXELFORMAT_RGB555 },
	{ AV_PIX_FMT_BGR555,         SDL_PIXELFORMAT_BGR555 },
	{ AV_PIX_FMT_RGB565,         SDL_PIXELFORMAT_RGB565 },
	{ AV_PIX_FMT_BGR565,         SDL_PIXELFORMAT_BGR565 },
	{ AV_PIX_FMT_RGB24,          SDL_PIXELFORMAT_RGB24 },
	{ AV_PIX_FMT_BGR24,          SDL_PIXELFORMAT_BGR24 },
	{ AV_PIX_FMT_0RGB32,         SDL_PIXELFORMAT_RGB888 },
	{ AV_PIX_FMT_0BGR32,         SDL_PIXELFORMAT_BGR888 },
	{ AV_PIX_FMT_NE(RGB0, 0BGR), SDL_PIXELFORMAT_RGBX8888 },
	{ AV_PIX_FMT_NE(BGR0, 0RGB), SDL_PIXELFORMAT_BGRX8888 },
	{ AV_PIX_FMT_RGB32,          SDL_PIXELFORMAT_ARGB8888 },
	{ AV_PIX_FMT_RGB32_1,        SDL_PIXELFORMAT_RGBA8888 },
	{ AV_PIX_FMT_BGR32,          SDL_PIXELFORMAT_ABGR8888 },
	{ AV_PIX_FMT_BGR32_1,        SDL_PIXELFORMAT_BGRA8888 },
	{ AV_PIX_FMT_YUV420P,        SDL_PIXELFORMAT_IYUV },
	{ AV_PIX_FMT_YUYV422,        SDL_PIXELFORMAT_YUY2 },
	{ AV_PIX_FMT_UYVY422,        SDL_PIXELFORMAT_UYVY },
	{ AV_PIX_FMT_NONE,           SDL_PIXELFORMAT_UNKNOWN },
};

#if CONFIG_AVFILTER
template<class T>
void GROW_ARRAY1(T& v, int nb_elems) { v = (T)grow_array(v, sizeof(*v), &nb_elems, nb_elems + 1); }

int play_ctx::opt_add_vfilter(void* optctx, const char* opt, const char* arg)
{
	GROW_ARRAY1(vfilters_list, nb_vfilters);
	vfilters_list[nb_vfilters - 1] = arg;
	return 0;
}
#endif

static inline
int cmp_audio_fmts(enum AVSampleFormat fmt1, int64_t channel_count1,
	enum AVSampleFormat fmt2, int64_t channel_count2)
{
	/* If channel count == 1, planar and non-planar formats are the same */
	if (channel_count1 == 1 && channel_count2 == 1)
		return av_get_packed_sample_fmt(fmt1) != av_get_packed_sample_fmt(fmt2);
	else
		return channel_count1 != channel_count2 || fmt1 != fmt2;
}

static int packet_queue_put_private(PacketQueue* q, AVPacket* pkt)
{
	MyAVPacketList pkt1;
	int ret;

	if (q->abort_request)
		return -1;


	pkt1.pkt = pkt;
	pkt1.serial = q->serial;

	ret = av_fifo_write(q->pkt_list, &pkt1, 1);
	if (ret < 0)
		return ret;
	q->nb_packets++;
	q->size += pkt1.pkt->size + sizeof(pkt1);
	q->duration += pkt1.pkt->duration;
	/* XXX: should duplicate packet data in DV case */
	SDL_CondSignal(q->cond);
	return 0;
}

static int packet_queue_put(PacketQueue* q, AVPacket* pkt)
{
	AVPacket* pkt1;
	int ret;

	pkt1 = av_packet_alloc();
	if (!pkt1) {
		av_packet_unref(pkt);
		return -1;
	}
	av_packet_move_ref(pkt1, pkt);

	SDL_LockMutex(q->mutex);
	ret = packet_queue_put_private(q, pkt1);
	SDL_UnlockMutex(q->mutex);

	if (ret < 0)
		av_packet_free(&pkt1);

	return ret;
}

static int packet_queue_put_nullpacket(PacketQueue* q, AVPacket* pkt, int stream_index)
{
	pkt->stream_index = stream_index;
	return packet_queue_put(q, pkt);
}

/* packet queue handling */
static int packet_queue_init(PacketQueue* q)
{
	memset(q, 0, sizeof(PacketQueue));
	q->pkt_list = av_fifo_alloc2(1, sizeof(MyAVPacketList), AV_FIFO_FLAG_AUTO_GROW);
	if (!q->pkt_list)
		return AVERROR(ENOMEM);
	q->mutex = SDL_CreateMutex();
	if (!q->mutex) {
		av_log(NULL, AV_LOG_FATAL, "SDL_CreateMutex(): %s\n", SDL_GetError());
		return AVERROR(ENOMEM);
	}
	q->cond = SDL_CreateCond();
	if (!q->cond) {
		av_log(NULL, AV_LOG_FATAL, "SDL_CreateCond(): %s\n", SDL_GetError());
		return AVERROR(ENOMEM);
	}
	q->abort_request = 1;
	return 0;
}

static void packet_queue_flush(PacketQueue* q)
{
	MyAVPacketList pkt1;

	SDL_LockMutex(q->mutex);
	while (av_fifo_read(q->pkt_list, &pkt1, 1) >= 0)
		av_packet_free(&pkt1.pkt);
	q->nb_packets = 0;
	q->size = 0;
	q->duration = 0;
	q->serial++;
	SDL_UnlockMutex(q->mutex);
}

static void packet_queue_destroy(PacketQueue* q)
{
	packet_queue_flush(q);
	av_fifo_freep2(&q->pkt_list);
	SDL_DestroyMutex(q->mutex);
	SDL_DestroyCond(q->cond);
}

static void packet_queue_abort(PacketQueue* q)
{
	SDL_LockMutex(q->mutex);

	q->abort_request = 1;

	SDL_CondSignal(q->cond);

	SDL_UnlockMutex(q->mutex);
}

static void packet_queue_start(PacketQueue* q)
{
	SDL_LockMutex(q->mutex);
	q->abort_request = 0;
	q->serial++;
	SDL_UnlockMutex(q->mutex);
}

/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
static int packet_queue_get(PacketQueue* q, AVPacket* pkt, int block, int* serial)
{
	MyAVPacketList pkt1;
	int ret;

	SDL_LockMutex(q->mutex);

	for (;;) {
		if (q->abort_request) {
			ret = -1;
			break;
		}

		if (av_fifo_read(q->pkt_list, &pkt1, 1) >= 0) {
			q->nb_packets--;
			q->size -= pkt1.pkt->size + sizeof(pkt1);
			q->duration -= pkt1.pkt->duration;
			av_packet_move_ref(pkt, pkt1.pkt);
			if (serial)
				*serial = pkt1.serial;
			av_packet_free(&pkt1.pkt);
			ret = 1;
			break;
		}
		else if (!block) {
			ret = 0;
			break;
		}
		else {
			SDL_CondWait(q->cond, q->mutex);
		}
	}
	SDL_UnlockMutex(q->mutex);
	return ret;
}

static int decoder_init(Decoder* d, AVCodecContext* avctx, PacketQueue* queue, SDL_cond* empty_queue_cond) {
	memset(d, 0, sizeof(Decoder));
	d->pkt = av_packet_alloc();
	if (!d->pkt)
		return AVERROR(ENOMEM);
	d->avctx = avctx;
	d->queue = queue;
	d->empty_queue_cond = empty_queue_cond;
	d->start_pts = AV_NOPTS_VALUE;
	d->pkt_serial = -1;
	return 0;
}

int play_ctx::decoder_decode_frame(Decoder* d, AVFrame* frame, AVSubtitle* sub) {
	int ret = AVERROR(EAGAIN);
	AVFrame* hw_frame = hwframe;
	AVFrame* tmp_frame = NULL;
	for (;;) {
		if (d->queue->serial == d->pkt_serial) {
			do {
				if (d->queue->abort_request)
					return -1;

				switch (d->avctx->codec_type) {
				case AVMEDIA_TYPE_VIDEO:
				{
					ret = avcodec_receive_frame(d->avctx, frame);
					if (ret >= 0) {
						if (decoder_reorder_pts == -1) {
							frame->pts = frame->best_effort_timestamp;
						}
						else if (!decoder_reorder_pts) {
							frame->pts = frame->pkt_dts;
						}
					}
					auto px = hw_device_pixel;
					px = (AVPixelFormat)frame->format;
					if (hwctx && is_cp2mem)
					{
						if (frame->format == hw_device_pixel) {
							// 如果采用的硬件加速剂，则调用avcodec_receive_frame()函数后，解码后的数据还在GPU中，所以需要通过此函数
							// 将GPU中的数据转移到CPU中来
							if ((ret = av_hwframe_transfer_data(hw_frame, frame, 0)) < 0) {
								//LOGD("av_hwframe_transfer_data fail %d", ret);
								return -2;
							}
							//LOGD("这里2222 解码成功 %d", sum);
							tmp_frame = frame;
						}
						else {
							//LOGD("这里1111 解码成功 %d", sum);
							tmp_frame = hw_frame;
						}
					}
				}
				break;
				case AVMEDIA_TYPE_AUDIO:
					ret = avcodec_receive_frame(d->avctx, frame);
					if (ret >= 0) {
						//AVRational tb = (AVRational){ 1, frame->sample_rate };
						AVRational tb = { 1, frame->sample_rate };
						if (frame->pts != AV_NOPTS_VALUE)
							frame->pts = av_rescale_q(frame->pts, d->avctx->pkt_timebase, tb);
						else if (d->next_pts != AV_NOPTS_VALUE)
							frame->pts = av_rescale_q(d->next_pts, d->next_pts_tb, tb);
						if (frame->pts != AV_NOPTS_VALUE) {
							d->next_pts = frame->pts + frame->nb_samples;
							d->next_pts_tb = tb;
						}
					}
					break;
				}
				if (ret == AVERROR_EOF) {
					d->finished = d->pkt_serial;
					avcodec_flush_buffers(d->avctx);
					return 0;
				}
				if (ret >= 0)
					return 1;
			} while (ret != AVERROR(EAGAIN));
		}

		do {
			if (d->queue->nb_packets == 0)
				SDL_CondSignal(d->empty_queue_cond);
			if (d->packet_pending) {
				d->packet_pending = 0;
			}
			else {
				int old_serial = d->pkt_serial;
				if (packet_queue_get(d->queue, d->pkt, 1, &d->pkt_serial) < 0)
					return -1;
				if (old_serial != d->pkt_serial) {
					avcodec_flush_buffers(d->avctx);
					d->finished = 0;
					d->next_pts = d->start_pts;
					d->next_pts_tb = d->start_pts_tb;
				}
			}
			if (d->queue->serial == d->pkt_serial)
				break;
			av_packet_unref(d->pkt);
		} while (1);

		if (d->avctx->codec_type == AVMEDIA_TYPE_SUBTITLE) {
			int got_frame = 0;
			ret = avcodec_decode_subtitle2(d->avctx, sub, &got_frame, d->pkt);
			if (ret < 0) {
				ret = AVERROR(EAGAIN);
			}
			else {
				if (got_frame && !d->pkt->data) {
					d->packet_pending = 1;
				}
				ret = got_frame ? 0 : (d->pkt->data ? AVERROR(EAGAIN) : AVERROR_EOF);
			}
			av_packet_unref(d->pkt);
		}
		else {
			if (avcodec_send_packet(d->avctx, d->pkt) == AVERROR(EAGAIN)) {
				av_log(d->avctx, AV_LOG_ERROR, "Receive_frame and send_packet both returned EAGAIN, which is an API violation.\n");
				d->packet_pending = 1;
			}
			else {
				av_packet_unref(d->pkt);
			}
		}
	}
}

static void decoder_destroy(Decoder* d) {
	av_packet_free(&d->pkt);
	avcodec_free_context(&d->avctx);
}

static void frame_queue_unref_item(Frame* vp)
{
	av_frame_unref(vp->frame);
	avsubtitle_free(&vp->sub);
}

static int frame_queue_init(FrameQueue* f, PacketQueue* pktq, int max_size, int keep_last)
{
	int i;
	memset(f, 0, sizeof(FrameQueue));
	if (!(f->mutex = SDL_CreateMutex())) {
		av_log(NULL, AV_LOG_FATAL, "SDL_CreateMutex(): %s\n", SDL_GetError());
		return AVERROR(ENOMEM);
	}
	if (!(f->cond = SDL_CreateCond())) {
		av_log(NULL, AV_LOG_FATAL, "SDL_CreateCond(): %s\n", SDL_GetError());
		return AVERROR(ENOMEM);
	}
	f->pktq = pktq;
	f->max_size = FFMIN(max_size, FRAME_QUEUE_SIZE);
	f->keep_last = !!keep_last;
	for (i = 0; i < f->max_size; i++)
		if (!(f->queue[i].frame = av_frame_alloc()))
			return AVERROR(ENOMEM);
	return 0;
}

static void frame_queue_destory(FrameQueue* f)
{
	int i;
	for (i = 0; i < f->max_size; i++) {
		Frame* vp = &f->queue[i];
		frame_queue_unref_item(vp);
		av_frame_free(&vp->frame);
	}
	SDL_DestroyMutex(f->mutex);
	SDL_DestroyCond(f->cond);
}

static void frame_queue_signal(FrameQueue* f)
{
	SDL_LockMutex(f->mutex);
	SDL_CondSignal(f->cond);
	SDL_UnlockMutex(f->mutex);
}

static Frame* frame_queue_peek(FrameQueue* f)
{
	return &f->queue[(f->rindex + f->rindex_shown) % f->max_size];
}

static Frame* frame_queue_peek_next(FrameQueue* f)
{
	return &f->queue[(f->rindex + f->rindex_shown + 1) % f->max_size];
}

static Frame* frame_queue_peek_last(FrameQueue* f)
{
	return &f->queue[f->rindex];
}

static Frame* frame_queue_peek_writable(FrameQueue* f)
{
	/* wait until we have space to put a new frame */
	SDL_LockMutex(f->mutex);
	while (f->size >= f->max_size &&
		!f->pktq->abort_request) {
		SDL_CondWait(f->cond, f->mutex);
	}
	SDL_UnlockMutex(f->mutex);

	if (f->pktq->abort_request)
		return NULL;

	return &f->queue[f->windex];
}

static Frame* frame_queue_peek_readable(FrameQueue* f)
{
	/* wait until we have a readable a new frame */
	SDL_LockMutex(f->mutex);
	while (f->size - f->rindex_shown <= 0 &&
		!f->pktq->abort_request) {
		SDL_CondWait(f->cond, f->mutex);
	}
	SDL_UnlockMutex(f->mutex);

	if (f->pktq->abort_request)
		return NULL;

	return &f->queue[(f->rindex + f->rindex_shown) % f->max_size];
}

static void frame_queue_push(FrameQueue* f)
{
	if (++f->windex == f->max_size)
		f->windex = 0;
	SDL_LockMutex(f->mutex);
	f->size++;
	SDL_CondSignal(f->cond);
	SDL_UnlockMutex(f->mutex);
}

static void frame_queue_next(FrameQueue* f)
{
	if (f->keep_last && !f->rindex_shown) {
		f->rindex_shown = 1;
		return;
	}
	frame_queue_unref_item(&f->queue[f->rindex]);
	if (++f->rindex == f->max_size)
		f->rindex = 0;
	SDL_LockMutex(f->mutex);
	f->size--;
	SDL_CondSignal(f->cond);
	SDL_UnlockMutex(f->mutex);
}

/* return the number of undisplayed frames in the queue */
static int frame_queue_nb_remaining(FrameQueue* f)
{
	return f->size - f->rindex_shown;
}

/* return last shown position */
static int64_t frame_queue_last_pos(FrameQueue* f)
{
	Frame* fp = &f->queue[f->rindex];
	if (f->rindex_shown && fp->serial == f->pktq->serial)
		return fp->pos;
	else
		return -1;
}

static void decoder_abort(Decoder* d, FrameQueue* fq)
{
	packet_queue_abort(d->queue);
	frame_queue_signal(fq);
	SDL_WaitThread(d->decoder_tid, NULL);
	d->decoder_tid = NULL;
	packet_queue_flush(d->queue);
}

void play_ctx::fill_rectangle(int x, int y, int w, int h)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	if (w && h)
		SDL_RenderFillRect(renderer, &rect);
}

int play_ctx::realloc_texture(SDL_Texture** texture, Uint32 new_format, int new_width, int new_height, SDL_BlendMode blendmode, int init_texture)
{
	Uint32 format;
	int access, w, h;
	if (!*texture || SDL_QueryTexture(*texture, &format, &access, &w, &h) < 0 || new_width != w || new_height != h || new_format != format) {
		void* pixels;
		int pitch;
		if (*texture)
			SDL_DestroyTexture(*texture);
		if (!(*texture = SDL_CreateTexture(renderer, new_format, SDL_TEXTUREACCESS_STREAMING, new_width, new_height)))
			return -1;
		if (SDL_SetTextureBlendMode(*texture, blendmode) < 0)
			return -1;
		if (init_texture) {
			if (SDL_LockTexture(*texture, NULL, &pixels, &pitch) < 0)
				return -1;
			memset(pixels, 0, pitch * new_height);
			SDL_UnlockTexture(*texture);
		}
		av_log(NULL, AV_LOG_VERBOSE, "Created %dx%d texture with %s.\n", new_width, new_height, SDL_GetPixelFormatName(new_format));
	}
	return 0;
}

static void calculate_display_rect(SDL_Rect* rect,
	int scr_xleft, int scr_ytop, int scr_width, int scr_height,
	int pic_width, int pic_height, AVRational pic_sar)
{
	AVRational aspect_ratio = pic_sar;
	int64_t width, height, x, y;

	if (av_cmp_q(aspect_ratio, av_make_q(0, 1)) <= 0)
		aspect_ratio = av_make_q(1, 1);

	aspect_ratio = av_mul_q(aspect_ratio, av_make_q(pic_width, pic_height));

	/* XXX: we suppose the screen has a 1.0 pixel ratio */
	height = scr_height;
	width = av_rescale(height, aspect_ratio.num, aspect_ratio.den) & ~1;
	if (width > scr_width) {
		width = scr_width;
		height = av_rescale(width, aspect_ratio.den, aspect_ratio.num) & ~1;
	}
	x = (scr_width - width) / 2;
	y = (scr_height - height) / 2;
	rect->x = scr_xleft + x;
	rect->y = scr_ytop + y;
	rect->w = FFMAX((int)width, 1);
	rect->h = FFMAX((int)height, 1);
}

static void get_sdl_pix_fmt_and_blendmode(int format, Uint32* sdl_pix_fmt, SDL_BlendMode* sdl_blendmode)
{
	int i;
	*sdl_blendmode = SDL_BLENDMODE_NONE;
	*sdl_pix_fmt = SDL_PIXELFORMAT_UNKNOWN;
	if (format == AV_PIX_FMT_RGB32 ||
		format == AV_PIX_FMT_RGB32_1 ||
		format == AV_PIX_FMT_BGR32 ||
		format == AV_PIX_FMT_BGR32_1)
		*sdl_blendmode = SDL_BLENDMODE_BLEND;
	for (i = 0; i < FF_ARRAY_ELEMS(sdl_texture_format_map) - 1; i++) {
		if (format == sdl_texture_format_map[i].format) {
			*sdl_pix_fmt = sdl_texture_format_map[i].texture_fmt;
			return;
		}
	}
}

int play_ctx::upload_texture(SDL_Texture** tex, AVFrame* frame, struct SwsContext** img_convert_ctx) {
	int ret = 0;
	Uint32 sdl_pix_fmt;
	SDL_BlendMode sdl_blendmode;
	get_sdl_pix_fmt_and_blendmode(frame->format, &sdl_pix_fmt, &sdl_blendmode);
	if (realloc_texture(tex, sdl_pix_fmt == SDL_PIXELFORMAT_UNKNOWN ? SDL_PIXELFORMAT_ARGB8888 : sdl_pix_fmt, frame->width, frame->height, sdl_blendmode, 0) < 0)
		return -1;
	switch (sdl_pix_fmt) {
	case SDL_PIXELFORMAT_UNKNOWN:
		/* This should only happen if we are not using avfilter... */
		*img_convert_ctx = sws_getCachedContext(*img_convert_ctx,
			frame->width, frame->height, (AVPixelFormat)frame->format, frame->width, frame->height,
			AV_PIX_FMT_BGRA, sws_flags, NULL, NULL, NULL);
		if (*img_convert_ctx != NULL) {
			uint8_t* pixels[4];
			int pitch[4];
			if (!SDL_LockTexture(*tex, NULL, (void**)pixels, pitch)) {
				sws_scale(*img_convert_ctx, (const uint8_t* const*)frame->data, frame->linesize,
					0, frame->height, pixels, pitch);
				SDL_UnlockTexture(*tex);
			}
		}
		else {
			av_log(NULL, AV_LOG_FATAL, "Cannot initialize the conversion context\n");
			ret = -1;
		}
		break;
	case SDL_PIXELFORMAT_IYUV:
		if (frame->linesize[0] > 0 && frame->linesize[1] > 0 && frame->linesize[2] > 0) {
			ret = SDL_UpdateYUVTexture(*tex, NULL, frame->data[0], frame->linesize[0],
				frame->data[1], frame->linesize[1],
				frame->data[2], frame->linesize[2]);
			bool isdown = false;
			if (isdown)
			{
				njson a;
				a["size"] = hz::v2to({ frame->width, frame->height });
				auto& ls = a["linesize"];
				for (size_t i = 0; i < AV_NUM_DATA_POINTERS; i++)
				{
					if (frame->linesize[i] == 0)break;
					ls.push_back(frame->linesize[i]);
				}
				hz::mfile_t mt;
				if (mt.open_m("temp/frame.data", false))
				{
					//(rect->w + 1) / 2, (rect->h + 1) / 2
					int h = (frame->height + 1) / 2;
					int ms3[] = { (frame->linesize[0] * frame->height) , (frame->linesize[1] * h) , (frame->linesize[2] * h) };// mt.get_file_size();
					int ms = ms3[0] + ms3[1] + ms3[2];
					auto md = mt.map(ms, 0);
					for (size_t i = 0; i < AV_NUM_DATA_POINTERS; i++)
					{
						if (frame->data[i] == 0)break;
						memcpy(md, frame->data[i], ms3[i]);
						a["ps"].push_back(ms3[i]);
						md += ms3[i];
					}
				}
				hz::save_json(a, "temp/frame.json", 2);
			}
		}
		else if (frame->linesize[0] < 0 && frame->linesize[1] < 0 && frame->linesize[2] < 0) {
			ret = SDL_UpdateYUVTexture(*tex, NULL, frame->data[0] + frame->linesize[0] * (frame->height - 1), -frame->linesize[0],
				frame->data[1] + frame->linesize[1] * (AV_CEIL_RSHIFT(frame->height, 1) - 1), -frame->linesize[1],
				frame->data[2] + frame->linesize[2] * (AV_CEIL_RSHIFT(frame->height, 1) - 1), -frame->linesize[2]);
		}
		else {
			av_log(NULL, AV_LOG_ERROR, "Mixed negative and positive linesizes are not supported.\n");
			return -1;
		}
		break;
	default:
		if (frame->linesize[0] < 0) {
			ret = SDL_UpdateTexture(*tex, NULL, frame->data[0] + frame->linesize[0] * (frame->height - 1), -frame->linesize[0]);
		}
		else {
			ret = SDL_UpdateTexture(*tex, NULL, frame->data[0], frame->linesize[0]);
		}
		break;
	}
	return ret;
}

static void set_sdl_yuv_conversion_mode(AVFrame* frame)
{
#if SDL_VERSION_ATLEAST(2,0,8)
	SDL_YUV_CONVERSION_MODE mode = SDL_YUV_CONVERSION_AUTOMATIC;
	if (frame && (frame->format == AV_PIX_FMT_YUV420P || frame->format == AV_PIX_FMT_YUYV422 || frame->format == AV_PIX_FMT_UYVY422)) {
		if (frame->color_range == AVCOL_RANGE_JPEG)
			mode = SDL_YUV_CONVERSION_JPEG;
		else if (frame->colorspace == AVCOL_SPC_BT709)
			mode = SDL_YUV_CONVERSION_BT709;
		else if (frame->colorspace == AVCOL_SPC_BT470BG || frame->colorspace == AVCOL_SPC_SMPTE170M)
			mode = SDL_YUV_CONVERSION_BT601;
	}
	SDL_SetYUVConversionMode(mode); /* FIXME: no support for linear transfer */
#endif
}

void play_ctx::video_image_display(VideoState* is)
{
	Frame* vp;
	Frame* sp = NULL;
	SDL_Rect rect;

	vp = frame_queue_peek_last(&is->pictq);
	if (is->subtitle_st) {
		if (frame_queue_nb_remaining(&is->subpq) > 0) {
			sp = frame_queue_peek(&is->subpq);

			if (vp->pts >= sp->pts + ((float)sp->sub.start_display_time / 1000)) {
				if (!sp->uploaded) {
					uint8_t* pixels[4];
					int pitch[4];
					int i;
					if (!sp->width || !sp->height) {
						sp->width = vp->width;
						sp->height = vp->height;
					}
					if (realloc_texture(&is->sub_texture, SDL_PIXELFORMAT_ARGB8888, sp->width, sp->height, SDL_BLENDMODE_BLEND, 1) < 0)
						return;

					for (i = 0; i < sp->sub.num_rects; i++) {
						AVSubtitleRect* sub_rect = sp->sub.rects[i];

						sub_rect->x = av_clip(sub_rect->x, 0, sp->width);
						sub_rect->y = av_clip(sub_rect->y, 0, sp->height);
						sub_rect->w = av_clip(sub_rect->w, 0, sp->width - sub_rect->x);
						sub_rect->h = av_clip(sub_rect->h, 0, sp->height - sub_rect->y);

						is->sub_convert_ctx = sws_getCachedContext(is->sub_convert_ctx,
							sub_rect->w, sub_rect->h, AV_PIX_FMT_PAL8,
							sub_rect->w, sub_rect->h, AV_PIX_FMT_BGRA,
							0, NULL, NULL, NULL);
						if (!is->sub_convert_ctx) {
							av_log(NULL, AV_LOG_FATAL, "Cannot initialize the conversion context\n");
							return;
						}
						if (!SDL_LockTexture(is->sub_texture, (SDL_Rect*)sub_rect, (void**)pixels, pitch)) {
							sws_scale(is->sub_convert_ctx, (const uint8_t* const*)sub_rect->data, sub_rect->linesize,
								0, sub_rect->h, pixels, pitch);
							SDL_UnlockTexture(is->sub_texture);
						}
					}
					sp->uploaded = 1;
				}
			}
			else
				sp = NULL;
		}
	}

	calculate_display_rect(&rect, is->xleft, is->ytop, is->width, is->height, vp->width, vp->height, vp->sar);
	set_sdl_yuv_conversion_mode(vp->frame);

	if (!vp->uploaded) {
		if (upload_texture(&is->vid_texture, vp->frame, &is->img_convert_ctx) < 0) {
			set_sdl_yuv_conversion_mode(NULL);
			return;
		}
		vp->uploaded = 1;
		vp->flip_v = vp->frame->linesize[0] < 0;
	}

	SDL_RenderCopyEx(renderer, is->vid_texture, NULL, &rect, 0, NULL, vp->flip_v ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
	set_sdl_yuv_conversion_mode(NULL);
	if (sp) {
#if USE_ONEPASS_SUBTITLE_RENDER
		SDL_RenderCopy(renderer, is->sub_texture, NULL, &rect);
#else
		int i;
		double xratio = (double)rect.w / (double)sp->width;
		double yratio = (double)rect.h / (double)sp->height;
		for (i = 0; i < sp->sub.num_rects; i++) {
			SDL_Rect* sub_rect = (SDL_Rect*)sp->sub.rects[i];
			SDL_Rect target = { .x = rect.x + sub_rect->x * xratio,
							   .y = rect.y + sub_rect->y * yratio,
							   .w = sub_rect->w * xratio,
							   .h = sub_rect->h * yratio
			};
			SDL_RenderCopy(renderer, is->sub_texture, sub_rect, &target);
		}
#endif
	}
}

void play_ctx::video_image_display1(VideoState* is)
{
	Frame* vp;
	Frame* sp = NULL;
	SDL_Rect rect;

	vp = frame_queue_peek_last(&is->pictq);
	if (is->subtitle_st) {
		if (frame_queue_nb_remaining(&is->subpq) > 0) {
			sp = frame_queue_peek(&is->subpq);

			if (vp->pts >= sp->pts + ((float)sp->sub.start_display_time / 1000)) {
				if (!sp->uploaded) {
					uint8_t* pixels[4];
					int pitch[4];
					int i;
					if (!sp->width || !sp->height) {
						sp->width = vp->width;
						sp->height = vp->height;
					}

					for (i = 0; i < sp->sub.num_rects; i++) {
						AVSubtitleRect* sub_rect = sp->sub.rects[i];

						sub_rect->x = av_clip(sub_rect->x, 0, sp->width);
						sub_rect->y = av_clip(sub_rect->y, 0, sp->height);
						sub_rect->w = av_clip(sub_rect->w, 0, sp->width - sub_rect->x);
						sub_rect->h = av_clip(sub_rect->h, 0, sp->height - sub_rect->y);

						is->sub_convert_ctx = sws_getCachedContext(is->sub_convert_ctx,
							sub_rect->w, sub_rect->h, AV_PIX_FMT_PAL8,
							sub_rect->w, sub_rect->h, AV_PIX_FMT_RGBA,//AV_PIX_FMT_BGRA,
							0, NULL, NULL, NULL);
						if (!is->sub_convert_ctx) {
							av_log(NULL, AV_LOG_FATAL, "Cannot initialize the conversion context\n");
							return;
						}
						if (!SDL_LockTexture(is->sub_texture, (SDL_Rect*)sub_rect, (void**)pixels, pitch)) {
							sws_scale(is->sub_convert_ctx, (const uint8_t* const*)sub_rect->data, sub_rect->linesize,
								0, sub_rect->h, pixels, pitch);
							SDL_UnlockTexture(is->sub_texture);
						}
					}
					sp->uploaded = 1;
				}
			}
			else
				sp = NULL;
		}
	}

	calculate_display_rect(&rect, is->xleft, is->ytop, is->width, is->height, vp->width, vp->height, vp->sar);
	set_sdl_yuv_conversion_mode(vp->frame);

	if (!vp->uploaded) {
		yuv_info_t yf = {};
		if (is->dcb && (vp->frame->linesize[0] > 0 && vp->frame->linesize[1] > 0 && vp->frame->linesize[2] > 0)) {
			int h = (vp->frame->height + 1) / 2;
			int ms3[] = { (vp->frame->linesize[0] * vp->frame->height) , (vp->frame->linesize[1] * h) , (vp->frame->linesize[2] * h) };// mt.get_file_size();
			int ms = ms3[0] + ms3[1] + ms3[2];

			yf.width = vp->frame->width;
			yf.height = vp->frame->height;
			for (size_t i = 0; i < AV_NUM_DATA_POINTERS; i++)
			{
				if (vp->frame->data[i] == 0)break;
				yf.data[i] = vp->frame->data[i];
				yf.size[i] = ms3[i];
			}
			is->dcb(&yf);
		}
		vp->uploaded = 1;
		vp->flip_v = vp->frame->linesize[0] < 0;
	}
}

static inline int compute_mod(int a, int b)
{
	return a < 0 ? a % b + b : a % b;
}

void play_ctx::video_audio_display(VideoState* s)
{
	int i, i_start, x, y1, y, ys, delay, n, nb_display_channels;
	int ch, channels, h, h2;
	int64_t time_diff;
	int rdft_bits, nb_freq;

	for (rdft_bits = 1; (1 << rdft_bits) < 2 * s->height; rdft_bits++)
		;
	nb_freq = 1 << (rdft_bits - 1);

	/* compute display index : center on currently output samples */
	channels = s->audio_tgt.ch_layout.nb_channels;
	nb_display_channels = channels;
	if (!s->paused) {
		int data_used = s->show_mode == SHOW_MODE_WAVES ? s->width : (2 * nb_freq);
		n = 2 * channels;
		delay = s->audio_write_buf_size;
		delay /= n;

		/* to be more precise, we take into account the time spent since
		   the last buffer computation */
		if (audio_callback_time) {
			time_diff = av_gettime_relative() - audio_callback_time;
			delay -= (time_diff * s->audio_tgt.freq) / 1000000;
		}

		delay += 2 * data_used;
		if (delay < data_used)
			delay = data_used;

		i_start = x = compute_mod(s->sample_array_index - delay * channels, SAMPLE_ARRAY_SIZE);
		if (s->show_mode == SHOW_MODE_WAVES) {
			h = INT_MIN;
			for (i = 0; i < 1000; i += channels) {
				int idx = (SAMPLE_ARRAY_SIZE + x - i) % SAMPLE_ARRAY_SIZE;
				int a = s->sample_array[idx];
				int b = s->sample_array[(idx + 4 * channels) % SAMPLE_ARRAY_SIZE];
				int c = s->sample_array[(idx + 5 * channels) % SAMPLE_ARRAY_SIZE];
				int d = s->sample_array[(idx + 9 * channels) % SAMPLE_ARRAY_SIZE];
				int score = a - d;
				if (h < score && (b ^ c) < 0) {
					h = score;
					i_start = idx;
				}
			}
		}

		s->last_i_start = i_start;
	}
	else {
		i_start = s->last_i_start;
	}

	if (s->show_mode == SHOW_MODE_WAVES) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		/* total height for one channel */
		h = s->height / nb_display_channels;
		/* graph height / 2 */
		h2 = (h * 9) / 20;
		for (ch = 0; ch < nb_display_channels; ch++) {
			i = i_start + ch;
			y1 = s->ytop + ch * h + (h / 2); /* position of center line */
			for (x = 0; x < s->width; x++) {
				y = (s->sample_array[i] * h2) >> 15;
				if (y < 0) {
					y = -y;
					ys = y1 - y;
				}
				else {
					ys = y1;
				}
				fill_rectangle(s->xleft + x, ys, 1, y);
				i += channels;
				if (i >= SAMPLE_ARRAY_SIZE)
					i -= SAMPLE_ARRAY_SIZE;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

		for (ch = 1; ch < nb_display_channels; ch++) {
			y = s->ytop + ch * h;
			fill_rectangle(s->xleft, y, s->width, 1);
		}
	}
	else {
		if (realloc_texture(&s->vis_texture, SDL_PIXELFORMAT_ARGB8888, s->width, s->height, SDL_BLENDMODE_NONE, 1) < 0)
			return;

		if (s->xpos >= s->width)
			s->xpos = 0;
		nb_display_channels = FFMIN(nb_display_channels, 2);
		if (rdft_bits != s->rdft_bits) {
			av_rdft_end(s->rdft);
			av_free(s->rdft_data);
			s->rdft = av_rdft_init(rdft_bits, DFT_R2C);
			s->rdft_bits = rdft_bits;
			s->rdft_data = (FFTSample*)av_malloc_array(nb_freq, 4 * sizeof(*s->rdft_data));
		}
		if (!s->rdft || !s->rdft_data) {
			av_log(NULL, AV_LOG_ERROR, "Failed to allocate buffers for RDFT, switching to waves display\n");
			s->show_mode = SHOW_MODE_WAVES;
		}
		else {
			FFTSample* data[2];
			SDL_Rect rect = { .x = s->xpos, .y = 0, .w = 1, .h = s->height };
			uint32_t* pixels;
			int pitch;
			for (ch = 0; ch < nb_display_channels; ch++) {
				data[ch] = s->rdft_data + 2 * nb_freq * ch;
				i = i_start + ch;
				for (x = 0; x < 2 * nb_freq; x++) {
					double w = (x - nb_freq) * (1.0 / nb_freq);
					data[ch][x] = s->sample_array[i] * (1.0 - w * w);
					i += channels;
					if (i >= SAMPLE_ARRAY_SIZE)
						i -= SAMPLE_ARRAY_SIZE;
				}
				av_rdft_calc(s->rdft, data[ch]);
			}
			/* Least efficient way to do this, we should of course
			 * directly access it but it is more than fast enough. */
			if (!SDL_LockTexture(s->vis_texture, &rect, (void**)&pixels, &pitch)) {
				pitch >>= 2;
				pixels += pitch * s->height;
				for (y = 0; y < s->height; y++) {
					double w = 1 / sqrt(nb_freq);
					int a = sqrt(w * sqrt(data[0][2 * y + 0] * data[0][2 * y + 0] + data[0][2 * y + 1] * data[0][2 * y + 1]));
					int b = (nb_display_channels == 2) ? sqrt(w * hypot(data[1][2 * y + 0], data[1][2 * y + 1]))
						: a;
					a = FFMIN(a, 255);
					b = FFMIN(b, 255);
					pixels -= pitch;
					*pixels = (a << 16) + (b << 8) + ((a + b) >> 1);
				}
				SDL_UnlockTexture(s->vis_texture);
			}
			SDL_RenderCopy(renderer, s->vis_texture, NULL, NULL);
		}
		if (!s->paused)
			s->xpos++;
	}
}

void play_ctx::stream_component_close(VideoState* is, int stream_index)
{
	AVFormatContext* ic = is->ic;
	AVCodecParameters* codecpar;

	if (stream_index < 0 || stream_index >= ic->nb_streams)
		return;
	codecpar = ic->streams[stream_index]->codecpar;

	switch (codecpar->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
		decoder_abort(&is->auddec, &is->sampq);
		SDL_CloseAudioDevice(audio_dev);
		decoder_destroy(&is->auddec);
		swr_free(&is->swr_ctx);
		av_freep(&is->audio_buf1);
		is->audio_buf1_size = 0;
		is->audio_buf = NULL;

		if (is->rdft) {
			av_rdft_end(is->rdft);
			av_freep(&is->rdft_data);
			is->rdft = NULL;
			is->rdft_bits = 0;
		}
		break;
	case AVMEDIA_TYPE_VIDEO:
		if (hwctx)
			hwctx->unref();
		decoder_abort(&is->viddec, &is->pictq);
		decoder_destroy(&is->viddec);
		break;
	case AVMEDIA_TYPE_SUBTITLE:
		decoder_abort(&is->subdec, &is->subpq);
		decoder_destroy(&is->subdec);
		break;
	default:
		break;
	}

	ic->streams[stream_index]->discard = AVDISCARD_ALL;
	switch (codecpar->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
		is->audio_st = NULL;
		is->audio_stream = -1;
		break;
	case AVMEDIA_TYPE_VIDEO:
		is->video_st = NULL;
		is->video_stream = -1;
		break;
	case AVMEDIA_TYPE_SUBTITLE:
		is->subtitle_st = NULL;
		is->subtitle_stream = -1;
		break;
	default:
		break;
	}
}

void play_ctx::stream_close(VideoState* is)
{
	/* XXX: use a special url_shutdown call to abort parse cleanly */
	is->abort_request = 1;
	SDL_WaitThread(is->read_tid, NULL);
	// 等待音频线程
	if (ispushaudio && audiopt.joinable())
		audiopt.join();

	/* close each stream */
	if (is->audio_stream >= 0)
		stream_component_close(is, is->audio_stream);
	if (is->video_stream >= 0)
		stream_component_close(is, is->video_stream);
	if (is->subtitle_stream >= 0)
		stream_component_close(is, is->subtitle_stream);

	avformat_close_input(&is->ic);

	packet_queue_destroy(&is->videoq);
	packet_queue_destroy(&is->audioq);
	packet_queue_destroy(&is->subtitleq);

	/* free all pictures */
	frame_queue_destory(&is->pictq);
	frame_queue_destory(&is->sampq);
	frame_queue_destory(&is->subpq);
	SDL_DestroyCond(is->continue_read_thread);
	sws_freeContext(is->img_convert_ctx);
	sws_freeContext(is->sub_convert_ctx);
	av_free(is->filename);
	if (is->vis_texture)
		SDL_DestroyTexture(is->vis_texture);
	if (is->vid_texture)
		SDL_DestroyTexture(is->vid_texture);
	if (is->sub_texture)
		SDL_DestroyTexture(is->sub_texture);
	av_free(is);
}

void play_ctx::do_exit(VideoState* is)
{
	et = 0;
	if (is) {
		stream_close(is);
	}
	if (renderer)
		SDL_DestroyRenderer(renderer);
	if (window)
		SDL_DestroyWindow(window);
#if CONFIG_AVFILTER
	av_freep(&vfilters_list);
#endif
	if (0)
	{
		uninit_opts();
		avformat_network_deinit();
		if (show_status)
			printf("\n");
		if (renderer && window)
			SDL_Quit();
	}
	av_log(NULL, AV_LOG_QUIET, "%s", "");
	tis = 0;
	//exit(0);
}

static void sigterm_handler(int sig)
{
	exit(123);
}

void play_ctx::set_default_window_size(int width, int height, AVRational sar)
{
	SDL_Rect rect;
	int max_width = screen_width ? screen_width : INT_MAX;
	int max_height = screen_height ? screen_height : INT_MAX;
	if (max_width == INT_MAX && max_height == INT_MAX)
		max_height = height;
	calculate_display_rect(&rect, 0, 0, max_width, max_height, width, height, sar);
	default_width = rect.w;
	default_height = rect.h;
}

int play_ctx::video_open(VideoState* is)
{
	int w, h;

	w = screen_width ? screen_width : default_width;
	h = screen_height ? screen_height : default_height;

	if (!window_title)
		window_title = input_filename;
	if (window)
	{
		SDL_SetWindowTitle(window, window_title);

		SDL_SetWindowSize(window, w, h);
		SDL_SetWindowPosition(window, screen_left, screen_top);
		if (is_full_screen)
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		SDL_ShowWindow(window);
	}

	is->width = w;
	is->height = h;

	return 0;
}

/* display the current picture, if any */
void play_ctx::video_display(VideoState* is)
{
	if (!is->width)
		video_open(is);

	if (renderer) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		if (is->audio_st && is->show_mode != SHOW_MODE_VIDEO)
			video_audio_display(is);
		else if (is->video_st)
			video_image_display(is);
		SDL_RenderPresent(renderer);
	}
	else {
		video_image_display1(is);
	}
}

static double get_clock(Clock* c)
{
	if (*c->queue_serial != c->serial)
		return NAN;
	if (c->paused) {
		return c->pts;
	}
	else {
		double time = av_gettime_relative() / 1000000.0;
		return c->pts_drift + time - (time - c->last_updated) * (1.0 - c->speed);
	}
}

static void set_clock_at(Clock* c, double pts, int serial, double time)
{
	c->pts = pts;
	c->last_updated = time;
	c->pts_drift = c->pts - time;
	c->serial = serial;
}

static void set_clock(Clock* c, double pts, int serial)
{
	double time = av_gettime_relative() / 1000000.0;
	set_clock_at(c, pts, serial, time);
}

static void set_clock_speed(Clock* c, double speed)
{
	set_clock(c, get_clock(c), c->serial);
	c->speed = speed;
}

static void init_clock(Clock* c, int* queue_serial)
{
	c->speed = 1.0;
	c->paused = 0;
	c->queue_serial = queue_serial;
	set_clock(c, NAN, -1);
}

static void sync_clock_to_slave(Clock* c, Clock* slave)
{
	double clock = get_clock(c);
	double slave_clock = get_clock(slave);
	if (!isnan(slave_clock) && (isnan(clock) || fabs(clock - slave_clock) > AV_NOSYNC_THRESHOLD))
		set_clock(c, slave_clock, slave->serial);
}

static int get_master_sync_type(VideoState* is) {
	if (is->av_sync_type == AV_SYNC_VIDEO_MASTER) {
		if (is->video_st)
			return AV_SYNC_VIDEO_MASTER;
		else
			return AV_SYNC_AUDIO_MASTER;
	}
	else if (is->av_sync_type == AV_SYNC_AUDIO_MASTER) {
		if (is->audio_st)
			return AV_SYNC_AUDIO_MASTER;
		else
			return AV_SYNC_EXTERNAL_CLOCK;
	}
	else {
		return AV_SYNC_EXTERNAL_CLOCK;
	}
}

/* get the current master clock value */
static double get_master_clock(VideoState* is)
{
	double val;

	switch (get_master_sync_type(is)) {
	case AV_SYNC_VIDEO_MASTER:
		val = get_clock(&is->vidclk);
		break;
	case AV_SYNC_AUDIO_MASTER:
		val = get_clock(&is->audclk);
		break;
	default:
		val = get_clock(&is->extclk);
		break;
	}
	return val;
}

static void check_external_clock_speed(VideoState* is) {
	if (is->video_stream >= 0 && is->videoq.nb_packets <= EXTERNAL_CLOCK_MIN_FRAMES ||
		is->audio_stream >= 0 && is->audioq.nb_packets <= EXTERNAL_CLOCK_MIN_FRAMES) {
		set_clock_speed(&is->extclk, FFMAX(EXTERNAL_CLOCK_SPEED_MIN, is->extclk.speed - EXTERNAL_CLOCK_SPEED_STEP));
	}
	else if ((is->video_stream < 0 || is->videoq.nb_packets > EXTERNAL_CLOCK_MAX_FRAMES) &&
		(is->audio_stream < 0 || is->audioq.nb_packets > EXTERNAL_CLOCK_MAX_FRAMES)) {
		set_clock_speed(&is->extclk, FFMIN(EXTERNAL_CLOCK_SPEED_MAX, is->extclk.speed + EXTERNAL_CLOCK_SPEED_STEP));
	}
	else {
		double speed = is->extclk.speed;
		if (speed != 1.0)
			set_clock_speed(&is->extclk, speed + EXTERNAL_CLOCK_SPEED_STEP * (1.0 - speed) / fabs(1.0 - speed));
	}
}

/* seek in the stream */
static void stream_seek(VideoState* is, int64_t pos, int64_t rel, int by_bytes)
{
	if (!is->seek_req) {
		is->seek_pos = pos;
		is->seek_rel = rel;
		is->seek_flags &= ~AVSEEK_FLAG_BYTE;
		if (by_bytes)
			is->seek_flags |= AVSEEK_FLAG_BYTE;
		is->seek_req = 1;
		SDL_CondSignal(is->continue_read_thread);
	}
}

/* pause or resume the video */
static void stream_toggle_pause(VideoState* is)
{
	if (is->paused) {
		is->frame_timer += av_gettime_relative() / 1000000.0 - is->vidclk.last_updated;
		if (is->read_pause_return != AVERROR(ENOSYS)) {
			is->vidclk.paused = 0;
		}
		set_clock(&is->vidclk, get_clock(&is->vidclk), is->vidclk.serial);
	}
	set_clock(&is->extclk, get_clock(&is->extclk), is->extclk.serial);
	bool isp = is->paused;
	is->paused = is->audclk.paused = is->vidclk.paused = is->extclk.paused = !isp;
}

static void toggle_pause(VideoState* is)
{
	stream_toggle_pause(is);
	is->step = 0;
}

static void toggle_mute(VideoState* is)
{
	bool is1 = is->muted;
	is->muted = !is1;
}

static void update_volume(VideoState* is, int sign, double step)
{
	double volume_level = is->audio_volume ? (20 * log(is->audio_volume / (double)SDL_MIX_MAXVOLUME) / log(10)) : -1000.0;
	int new_volume = lrint(SDL_MIX_MAXVOLUME * pow(10.0, (volume_level + sign * step) / 20.0));
	is->audio_volume = av_clip(is->audio_volume == new_volume ? (is->audio_volume + sign) : new_volume, 0, SDL_MIX_MAXVOLUME);
}
static void set_volume(VideoState* is, int v)
{
	is->audio_volume = av_clip(v, 0, SDL_MIX_MAXVOLUME);
}

static void step_to_next_frame(VideoState* is)
{
	/* if the stream is paused unpause it, then step */
	if (is->paused)
		stream_toggle_pause(is);
	is->step = 1;
}

static double compute_target_delay(double delay, VideoState* is)
{
	double sync_threshold, diff = 0;

	/* update delay to follow master synchronisation source */
	if (get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER) {
		/* if video is slave, we try to correct big delays by
		   duplicating or deleting a frame */
		diff = get_clock(&is->vidclk) - get_master_clock(is);

		/* skip or repeat frame. We take into account the
		   delay to compute the threshold. I still don't know
		   if it is the best guess */
		sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
		if (!isnan(diff) && fabs(diff) < is->max_frame_duration) {
			if (diff <= -sync_threshold)
				delay = FFMAX(0, delay + diff);
			else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)
				delay = delay + diff;
			else if (diff >= sync_threshold)
				delay = 2 * delay;
		}
	}

	av_log(NULL, AV_LOG_TRACE, "video: delay=%0.3f A-V=%f\n",
		delay, -diff);

	return delay;
}

static double vp_duration(VideoState* is, Frame* vp, Frame* nextvp) {
	if (vp->serial == nextvp->serial) {
		double duration = nextvp->pts - vp->pts;
		if (isnan(duration) || duration <= 0 || duration > is->max_frame_duration)
			return vp->duration;
		else
			return duration;
	}
	else {
		return 0.0;
	}
}

static void update_video_pts(VideoState* is, double pts, int64_t pos, int serial) {
	/* update current video pts */
	set_clock(&is->vidclk, pts, serial);
	sync_clock_to_slave(&is->extclk, &is->vidclk);
}

/* called to display each frame */
void play_ctx::video_refresh(void* opaque, double* remaining_time)
{
	VideoState* is = (VideoState*)opaque;
	double time;

	Frame* sp, * sp2;

	if (!is->paused && get_master_sync_type(is) == AV_SYNC_EXTERNAL_CLOCK && is->realtime)
		check_external_clock_speed(is);

	if (!display_disable && is->show_mode != SHOW_MODE_VIDEO && is->audio_st) {
		time = av_gettime_relative() / 1000000.0;
		if (is->force_refresh || is->last_vis_time + rdftspeed < time) {
			video_display(is);
			is->last_vis_time = time;
		}
		*remaining_time = FFMIN(*remaining_time, is->last_vis_time + rdftspeed - time);
	}

	if (is->video_st) {
	retry:
		if (frame_queue_nb_remaining(&is->pictq) == 0) {
			// nothing to do, no picture to display in the queue
		}
		else {
			double last_duration, duration, delay;
			Frame* vp, * lastvp;

			/* dequeue the picture */
			lastvp = frame_queue_peek_last(&is->pictq);
			vp = frame_queue_peek(&is->pictq);

			if (vp->serial != is->videoq.serial) {
				frame_queue_next(&is->pictq);
				goto retry;
			}

			if (lastvp->serial != vp->serial)
				is->frame_timer = av_gettime_relative() / 1000000.0;

			if (is->paused)
				goto display;

			/* compute nominal last_duration */
			last_duration = vp_duration(is, lastvp, vp);
			delay = compute_target_delay(last_duration, is);

			time = av_gettime_relative() / 1000000.0;
			if (time < is->frame_timer + delay) {
				*remaining_time = FFMIN(is->frame_timer + delay - time, *remaining_time);
				goto display;
			}

			is->frame_timer += delay;
			if (delay > 0 && time - is->frame_timer > AV_SYNC_THRESHOLD_MAX)
				is->frame_timer = time;

			SDL_LockMutex(is->pictq.mutex);
			if (!isnan(vp->pts))
				update_video_pts(is, vp->pts, vp->pos, vp->serial);
			SDL_UnlockMutex(is->pictq.mutex);

			if (frame_queue_nb_remaining(&is->pictq) > 1) {
				Frame* nextvp = frame_queue_peek_next(&is->pictq);
				duration = vp_duration(is, vp, nextvp);
				if (!is->step && (framedrop > 0 || (framedrop && get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER)) && time > is->frame_timer + duration) {
					is->frame_drops_late++;
					frame_queue_next(&is->pictq);
					goto retry;
				}
			}

			if (is->subtitle_st) {
				while (frame_queue_nb_remaining(&is->subpq) > 0) {
					sp = frame_queue_peek(&is->subpq);

					if (frame_queue_nb_remaining(&is->subpq) > 1)
						sp2 = frame_queue_peek_next(&is->subpq);
					else
						sp2 = NULL;

					if (sp->serial != is->subtitleq.serial
						|| (is->vidclk.pts > (sp->pts + ((float)sp->sub.end_display_time / 1000)))
						|| (sp2 && is->vidclk.pts > (sp2->pts + ((float)sp2->sub.start_display_time / 1000))))
					{
						if (sp->uploaded) {
							int i;
							for (i = 0; i < sp->sub.num_rects; i++) {
								AVSubtitleRect* sub_rect = sp->sub.rects[i];
								uint8_t* pixels;
								int pitch, j;

								if (!SDL_LockTexture(is->sub_texture, (SDL_Rect*)sub_rect, (void**)&pixels, &pitch)) {
									for (j = 0; j < sub_rect->h; j++, pixels += pitch)
										memset(pixels, 0, sub_rect->w << 2);
									SDL_UnlockTexture(is->sub_texture);
								}
							}
						}
						frame_queue_next(&is->subpq);
					}
					else {
						break;
					}
				}
			}

			frame_queue_next(&is->pictq);
			is->force_refresh = 1;

			if (is->step && !is->paused)
				stream_toggle_pause(is);
		}
	display:
		/* display picture */
		if (!display_disable && is->force_refresh && is->show_mode == SHOW_MODE_VIDEO && is->pictq.rindex_shown)
			video_display(is);
	}
	is->force_refresh = 0;
	if (show_status) {
		AVBPrint buf;
		static int64_t last_time;
		int64_t cur_time;
		int aqsize, vqsize, sqsize;
		double av_diff;

		cur_time = av_gettime_relative();
		if (!last_time || (cur_time - last_time) >= 30000) {
			aqsize = 0;
			vqsize = 0;
			sqsize = 0;
			if (is->audio_st)
				aqsize = is->audioq.size;
			if (is->video_st)
				vqsize = is->videoq.size;
			if (is->subtitle_st)
				sqsize = is->subtitleq.size;
			av_diff = 0;
			if (is->audio_st && is->video_st)
				av_diff = get_clock(&is->audclk) - get_clock(&is->vidclk);
			else if (is->video_st)
				av_diff = get_master_clock(is) - get_clock(&is->vidclk);
			else if (is->audio_st)
				av_diff = get_master_clock(is) - get_clock(&is->audclk);

			av_bprint_init(&buf, 0, AV_BPRINT_SIZE_AUTOMATIC);
			av_bprintf(&buf,
				"%7.2f %s:%7.3f fd=%4d aq=%5dKB vq=%5dKB sq=%5dB f=%lld/%lld   \r",
				get_master_clock(is),
				(is->audio_st && is->video_st) ? "A-V" : (is->video_st ? "M-V" : (is->audio_st ? "M-A" : "   ")),
				av_diff,
				is->frame_drops_early + is->frame_drops_late,
				aqsize / 1024,
				vqsize / 1024,
				sqsize,
				is->video_st ? is->viddec.avctx->pts_correction_num_faulty_dts : 0,
				is->video_st ? is->viddec.avctx->pts_correction_num_faulty_pts : 0);

			if (show_status == 1 && AV_LOG_INFO > av_log_get_level())
				fprintf(stderr, "%s", buf.str);
			else
				av_log(NULL, AV_LOG_INFO, "%s", buf.str);

			fflush(stderr);
			av_bprint_finalize(&buf, NULL);

			last_time = cur_time;
		}
	}
}

int play_ctx::queue_picture(VideoState* is, AVFrame* src_frame, double pts, double duration, int64_t pos, int serial)
{
	Frame* vp;

#if defined(DEBUG_SYNC)
	printf("frame_type=%c pts=%0.3f\n",
		av_get_picture_type_char(src_frame->pict_type), pts);
#endif

	if (!(vp = frame_queue_peek_writable(&is->pictq)))
		return -1;

	vp->sar = src_frame->sample_aspect_ratio;
	vp->uploaded = 0;

	vp->width = src_frame->width;
	vp->height = src_frame->height;
	vp->format = src_frame->format;

	vp->pts = pts;
	vp->duration = duration;
	vp->pos = pos;
	vp->serial = serial;

	set_default_window_size(vp->width, vp->height, vp->sar);

	av_frame_move_ref(vp->frame, src_frame);
	frame_queue_push(&is->pictq);
	return 0;
}

int play_ctx::get_video_frame(VideoState* is, AVFrame* frame)
{
	int got_picture;

	if ((got_picture = decoder_decode_frame(&is->viddec, frame, NULL)) < 0)
		return -1;

	if (got_picture) {
		double dpts = NAN;

		if (frame->pts != AV_NOPTS_VALUE)
			dpts = av_q2d(is->video_st->time_base) * frame->pts;

		frame->sample_aspect_ratio = av_guess_sample_aspect_ratio(is->ic, is->video_st, frame);

		if (framedrop > 0 || (framedrop && get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER)) {
			if (frame->pts != AV_NOPTS_VALUE) {
				double diff = dpts - get_master_clock(is);
				if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD &&
					diff - is->frame_last_filter_delay < 0 &&
					is->viddec.pkt_serial == is->vidclk.serial &&
					is->videoq.nb_packets) {
					is->frame_drops_early++;
					av_frame_unref(frame);
					got_picture = 0;
				}
			}
		}
	}

	return got_picture;
}

#if CONFIG_AVFILTER
static int configure_filtergraph(AVFilterGraph* graph, const char* filtergraph,
	AVFilterContext* source_ctx, AVFilterContext* sink_ctx)
{
	int ret, i;
	int nb_filters = graph->nb_filters;
	AVFilterInOut* outputs = NULL, * inputs = NULL;

	if (filtergraph) {
		outputs = avfilter_inout_alloc();
		inputs = avfilter_inout_alloc();
		if (!outputs || !inputs) {
			ret = AVERROR(ENOMEM);
			goto fail;
		}

		outputs->name = av_strdup("in");
		outputs->filter_ctx = source_ctx;
		outputs->pad_idx = 0;
		outputs->next = NULL;

		inputs->name = av_strdup("out");
		inputs->filter_ctx = sink_ctx;
		inputs->pad_idx = 0;
		inputs->next = NULL;

		if ((ret = avfilter_graph_parse_ptr(graph, filtergraph, &inputs, &outputs, NULL)) < 0)
			goto fail;
	}
	else {
		if ((ret = avfilter_link(source_ctx, 0, sink_ctx, 0)) < 0)
			goto fail;
	}

	/* Reorder the filters to ensure that inputs of the custom filters are merged first */
	for (i = 0; i < graph->nb_filters - nb_filters; i++)
		FFSWAP(AVFilterContext*, graph->filters[i], graph->filters[i + nb_filters]);

	ret = avfilter_graph_config(graph, NULL);
fail:
	avfilter_inout_free(&outputs);
	avfilter_inout_free(&inputs);
	return ret;
}

int play_ctx::configure_video_filters(AVFilterGraph* graph, VideoState* is, const char* vfilters, AVFrame* frame)
{
	enum AVPixelFormat pix_fmts[FF_ARRAY_ELEMS(sdl_texture_format_map)];
	char sws_flags_str[512] = "";
	char buffersrc_args[256];
	int ret;
	AVFilterContext* filt_src = NULL, * filt_out = NULL, * last_filter = NULL;
	AVCodecParameters* codecpar = is->video_st->codecpar;
	AVRational fr = av_guess_frame_rate(is->ic, is->video_st, NULL);
	const AVDictionaryEntry* e = NULL;
	int nb_pix_fmts = 0;
	int i, j;

	for (i = 0; i < renderer_info.num_texture_formats; i++) {
		for (j = 0; j < FF_ARRAY_ELEMS(sdl_texture_format_map) - 1; j++) {
			if (renderer_info.texture_formats[i] == sdl_texture_format_map[j].texture_fmt) {
				pix_fmts[nb_pix_fmts++] = sdl_texture_format_map[j].format;
				break;
			}
		}
	}
	pix_fmts[nb_pix_fmts] = AV_PIX_FMT_NONE;

	while ((e = av_dict_get(sws_dict, "", e, AV_DICT_IGNORE_SUFFIX))) {
		if (!strcmp(e->key, "sws_flags")) {
			av_strlcatf(sws_flags_str, sizeof(sws_flags_str), "%s=%s:", "flags", e->value);
		}
		else
			av_strlcatf(sws_flags_str, sizeof(sws_flags_str), "%s=%s:", e->key, e->value);
	}
	if (strlen(sws_flags_str))
		sws_flags_str[strlen(sws_flags_str) - 1] = '\0';

	graph->scale_sws_opts = av_strdup(sws_flags_str);

	snprintf(buffersrc_args, sizeof(buffersrc_args),
		"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
		frame->width, frame->height, frame->format,
		is->video_st->time_base.num, is->video_st->time_base.den,
		codecpar->sample_aspect_ratio.num, FFMAX(codecpar->sample_aspect_ratio.den, 1));
	if (fr.num && fr.den)
		av_strlcatf(buffersrc_args, sizeof(buffersrc_args), ":frame_rate=%d/%d", fr.num, fr.den);

	if ((ret = avfilter_graph_create_filter(&filt_src,
		avfilter_get_by_name("buffer"),
		"ffplay_buffer", buffersrc_args, NULL,
		graph)) < 0)
		goto fail;

	ret = avfilter_graph_create_filter(&filt_out,
		avfilter_get_by_name("buffersink"),
		"ffplay_buffersink", NULL, NULL, graph);
	if (ret < 0)
		goto fail;

	if ((ret = av_opt_set_int_list(filt_out, "pix_fmts", pix_fmts, AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN)) < 0)
		goto fail;

	last_filter = filt_out;

	/* Note: this macro adds a filter before the lastly added filter, so the
	 * processing order of the filters is in reverse */
#define INSERT_FILT(name, arg) do {                                          \
    AVFilterContext *filt_ctx;                                               \
                                                                             \
    ret = avfilter_graph_create_filter(&filt_ctx,                            \
                                       avfilter_get_by_name(name),           \
                                       "ffplay_" name, arg, NULL, graph);    \
    if (ret < 0)                                                             \
        goto fail;                                                           \
                                                                             \
    ret = avfilter_link(filt_ctx, 0, last_filter, 0);                        \
    if (ret < 0)                                                             \
        goto fail;                                                           \
                                                                             \
    last_filter = filt_ctx;                                                  \
} while (0)

	if (autorotate) {
		int32_t* displaymatrix = (int32_t*)av_stream_get_side_data(is->video_st, AV_PKT_DATA_DISPLAYMATRIX, NULL);
		double theta = get_rotation(displaymatrix);

		if (fabs(theta - 90) < 1.0) {
			INSERT_FILT("transpose", "clock");
		}
		else if (fabs(theta - 180) < 1.0) {
			INSERT_FILT("hflip", NULL);
			INSERT_FILT("vflip", NULL);
		}
		else if (fabs(theta - 270) < 1.0) {
			INSERT_FILT("transpose", "cclock");
		}
		else if (fabs(theta) > 1.0) {
			char rotate_buf[64];
			snprintf(rotate_buf, sizeof(rotate_buf), "%f*PI/180", theta);
			INSERT_FILT("rotate", rotate_buf);
		}
	}

	if ((ret = configure_filtergraph(graph, vfilters, filt_src, last_filter)) < 0)
		goto fail;

	is->in_video_filter = filt_src;
	is->out_video_filter = filt_out;

fail:
	return ret;
}

int play_ctx::configure_audio_filters(VideoState* is, const char* afilters, int force_output_format)
{
	static const enum AVSampleFormat sample_fmts[] = { AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_NONE };
	int sample_rates[2] = { 0, -1 };
	AVFilterContext* filt_asrc = NULL, * filt_asink = NULL;
	char aresample_swr_opts[512] = "";
	const AVDictionaryEntry* e = NULL;
	AVBPrint bp;
	char asrc_args[256];
	int ret;

	avfilter_graph_free(&is->agraph);
	if (!(is->agraph = avfilter_graph_alloc()))
		return AVERROR(ENOMEM);
	is->agraph->nb_threads = filter_nbthreads;

	av_bprint_init(&bp, 0, AV_BPRINT_SIZE_AUTOMATIC);

	while ((e = av_dict_get(swr_opts, "", e, AV_DICT_IGNORE_SUFFIX)))
		av_strlcatf(aresample_swr_opts, sizeof(aresample_swr_opts), "%s=%s:", e->key, e->value);
	if (strlen(aresample_swr_opts))
		aresample_swr_opts[strlen(aresample_swr_opts) - 1] = '\0';
	av_opt_set(is->agraph, "aresample_swr_opts", aresample_swr_opts, 0);

	av_channel_layout_describe_bprint(&is->audio_filter_src.ch_layout, &bp);

	ret = snprintf(asrc_args, sizeof(asrc_args),
		"sample_rate=%d:sample_fmt=%s:time_base=%d/%d:channel_layout=%s",
		is->audio_filter_src.freq, av_get_sample_fmt_name(is->audio_filter_src.fmt),
		1, is->audio_filter_src.freq, bp.str);

	ret = avfilter_graph_create_filter(&filt_asrc,
		avfilter_get_by_name("abuffer"), "ffplay_abuffer",
		asrc_args, NULL, is->agraph);
	if (ret < 0)
		goto end;


	ret = avfilter_graph_create_filter(&filt_asink,
		avfilter_get_by_name("abuffersink"), "ffplay_abuffersink",
		NULL, NULL, is->agraph);
	if (ret < 0)
		goto end;

	if ((ret = av_opt_set_int_list(filt_asink, "sample_fmts", sample_fmts, AV_SAMPLE_FMT_NONE, AV_OPT_SEARCH_CHILDREN)) < 0)
		goto end;
	if ((ret = av_opt_set_int(filt_asink, "all_channel_counts", 1, AV_OPT_SEARCH_CHILDREN)) < 0)
		goto end;

	if (force_output_format) {
		sample_rates[0] = is->audio_tgt.freq;
		if ((ret = av_opt_set_int(filt_asink, "all_channel_counts", 0, AV_OPT_SEARCH_CHILDREN)) < 0)
			goto end;
		if ((ret = av_opt_set(filt_asink, "ch_layouts", bp.str, AV_OPT_SEARCH_CHILDREN)) < 0)
			goto end;
		if ((ret = av_opt_set_int_list(filt_asink, "sample_rates", sample_rates, -1, AV_OPT_SEARCH_CHILDREN)) < 0)
			goto end;
	}


	if ((ret = configure_filtergraph(is->agraph, afilters, filt_asrc, filt_asink)) < 0)
		goto end;

	is->in_audio_filter = filt_asrc;
	is->out_audio_filter = filt_asink;

end:
	if (ret < 0)
		avfilter_graph_free(&is->agraph);
	av_bprint_finalize(&bp, NULL);

	return ret;
}
#endif  /* CONFIG_AVFILTER */

int play_ctx::audio_thread(void* arg)
{
	VideoState* is = (VideoState*)arg;
	AVFrame* frame = av_frame_alloc();
	Frame* af;
#if CONFIG_AVFILTER
	int last_serial = -1;
	int reconfigure;
#endif
	int got_frame = 0;
	AVRational tb;
	int ret = 0;

	if (!frame)
		return AVERROR(ENOMEM);

	do {
		if ((got_frame = decoder_decode_frame(&is->auddec, frame, NULL)) < 0)
			goto the_end;

		if (got_frame) {
			tb = { 1, frame->sample_rate };

#if CONFIG_AVFILTER
			reconfigure =
				cmp_audio_fmts(is->audio_filter_src.fmt, is->audio_filter_src.ch_layout.nb_channels,
					(AVSampleFormat)frame->format, frame->ch_layout.nb_channels) ||
				av_channel_layout_compare(&is->audio_filter_src.ch_layout, &frame->ch_layout) ||
				is->audio_filter_src.freq != frame->sample_rate ||
				is->auddec.pkt_serial != last_serial;

			if (reconfigure) {
				char buf1[1024], buf2[1024];
				av_channel_layout_describe(&is->audio_filter_src.ch_layout, buf1, sizeof(buf1));
				av_channel_layout_describe(&frame->ch_layout, buf2, sizeof(buf2));
				av_log(NULL, AV_LOG_DEBUG,
					"Audio frame changed from rate:%d ch:%d fmt:%s layout:%s serial:%d to rate:%d ch:%d fmt:%s layout:%s serial:%d\n",
					is->audio_filter_src.freq, is->audio_filter_src.ch_layout.nb_channels, av_get_sample_fmt_name(is->audio_filter_src.fmt), buf1, last_serial,
					frame->sample_rate, frame->ch_layout.nb_channels, av_get_sample_fmt_name((AVSampleFormat)frame->format), buf2, is->auddec.pkt_serial);

				is->audio_filter_src.fmt = (AVSampleFormat)frame->format;
				ret = av_channel_layout_copy(&is->audio_filter_src.ch_layout, &frame->ch_layout);
				if (ret < 0)
					goto the_end;
				is->audio_filter_src.freq = frame->sample_rate;
				last_serial = is->auddec.pkt_serial;

				if ((ret = configure_audio_filters(is, afilters, 1)) < 0)
					goto the_end;
			}

			if ((ret = av_buffersrc_add_frame(is->in_audio_filter, frame)) < 0)
				goto the_end;

			while ((ret = av_buffersink_get_frame_flags(is->out_audio_filter, frame, 0)) >= 0) {
				tb = av_buffersink_get_time_base(is->out_audio_filter);
#endif
				if (!(af = frame_queue_peek_writable(&is->sampq)))
					goto the_end;

				af->pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
				af->pos = frame->pkt_pos;
				af->serial = is->auddec.pkt_serial;
				af->duration = av_q2d(AVRational{ frame->nb_samples, frame->sample_rate });

				av_frame_move_ref(af->frame, frame);
				frame_queue_push(&is->sampq);

#if CONFIG_AVFILTER
				if (is->audioq.serial != is->auddec.pkt_serial)
					break;
			}
			if (ret == AVERROR_EOF)
				is->auddec.finished = is->auddec.pkt_serial;
#endif
		}
	} while (ret >= 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF);
the_end:
#if CONFIG_AVFILTER
	avfilter_graph_free(&is->agraph);
#endif
	av_frame_free(&frame);
	return ret;
}

static int decoder_start(Decoder* d, int (*fn)(void*), const char* thread_name, void* arg)
{
	packet_queue_start(d->queue);
	d->decoder_tid = SDL_CreateThread(fn, thread_name, arg);
	if (!d->decoder_tid) {
		av_log(NULL, AV_LOG_ERROR, "SDL_CreateThread(): %s\n", SDL_GetError());
		return AVERROR(ENOMEM);
	}
	return 0;
}

int play_ctx::video_thread(void* arg)
{
	VideoState* is = (VideoState*)arg;
	AVFrame* frame = av_frame_alloc();
	hwframe = av_frame_alloc();
	double pts;
	double duration;
	int ret;
	AVRational tb = is->video_st->time_base;
	AVRational frame_rate = av_guess_frame_rate(is->ic, is->video_st, NULL);

#if CONFIG_AVFILTER
	AVFilterGraph* graph = NULL;
	AVFilterContext* filt_out = NULL, * filt_in = NULL;
	int last_w = 0;
	int last_h = 0;
	enum AVPixelFormat last_format = (enum AVPixelFormat)-2;
	int last_serial = -1;
	int last_vfilter_idx = 0;
#endif

	if (!frame)
		return AVERROR(ENOMEM);

	for (;;) {
		ret = get_video_frame(is, frame);
		if (ret < 0)
			goto the_end;
		if (!ret)
			continue;


#if CONFIG_AVFILTER
		if (last_w != frame->width
			|| last_h != frame->height
			|| last_format != frame->format
			|| last_serial != is->viddec.pkt_serial
			|| last_vfilter_idx != is->vfilter_idx) {
			av_log(NULL, AV_LOG_DEBUG,
				"Video frame changed from size:%dx%d format:%s serial:%d to size:%dx%d format:%s serial:%d\n",
				last_w, last_h,
				(const char*)av_x_if_null(av_get_pix_fmt_name(last_format), "none"), last_serial,
				frame->width, frame->height,
				(const char*)av_x_if_null(av_get_pix_fmt_name((AVPixelFormat)frame->format), "none"), is->viddec.pkt_serial);
			avfilter_graph_free(&graph);
			graph = avfilter_graph_alloc();
			if (!graph) {
				ret = AVERROR(ENOMEM);
				goto the_end;
			}
			graph->nb_threads = filter_nbthreads;
			if ((ret = configure_video_filters(graph, is, vfilters_list ? vfilters_list[is->vfilter_idx] : NULL, frame)) < 0) {
				SDL_Event event;
				event.type = FF_QUIT_EVENT;
				event.user.data1 = is;
				SDL_PushEvent(&event);
				goto the_end;
			}
			filt_in = is->in_video_filter;
			filt_out = is->out_video_filter;
			last_w = frame->width;
			last_h = frame->height;
			last_format = (AVPixelFormat)frame->format;
			last_serial = is->viddec.pkt_serial;
			last_vfilter_idx = is->vfilter_idx;
			frame_rate = av_buffersink_get_frame_rate(filt_out);
		}

		ret = av_buffersrc_add_frame(filt_in, frame);
		if (ret < 0)
			goto the_end;

		while (ret >= 0) {
			is->frame_last_returned_time = av_gettime_relative() / 1000000.0;

			ret = av_buffersink_get_frame_flags(filt_out, frame, 0);
			if (ret < 0) {
				if (ret == AVERROR_EOF)
					is->viddec.finished = is->viddec.pkt_serial;
				ret = 0;
				break;
			}

			is->frame_last_filter_delay = av_gettime_relative() / 1000000.0 - is->frame_last_returned_time;
			if (fabs(is->frame_last_filter_delay) > AV_NOSYNC_THRESHOLD / 10.0)
				is->frame_last_filter_delay = 0;
			tb = av_buffersink_get_time_base(filt_out);
#endif
			duration = (frame_rate.num && frame_rate.den ? av_q2d(AVRational{ frame_rate.den, frame_rate.num }) : 0);
			pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
			ret = queue_picture(is, frame, pts, duration, frame->pkt_pos, is->viddec.pkt_serial);
			av_frame_unref(frame);
#if CONFIG_AVFILTER
			if (is->videoq.serial != is->viddec.pkt_serial)
				break;
		}
#endif

		if (ret < 0)
			goto the_end;
	}
the_end:
#if CONFIG_AVFILTER
	avfilter_graph_free(&graph);
#endif
	av_frame_free(&frame);
	av_frame_free(&hwframe);
	return 0;
}

int play_ctx::subtitle_thread(void* arg)
{
	VideoState* is = (VideoState*)arg;
	Frame* sp;
	int got_subtitle;
	double pts;

	for (;;) {
		if (!(sp = frame_queue_peek_writable(&is->subpq)))
			return 0;

		if ((got_subtitle = decoder_decode_frame(&is->subdec, NULL, &sp->sub)) < 0)
			break;

		pts = 0;
		std::string tk = (sp->sub.rects && sp->sub.rects[0]->ass) ? sp->sub.rects[0]->ass : "";
		//字幕if (tk.size())
		//	printf("%s\n", tk.c_str());
		if (got_subtitle && (sp->sub.format == 0)) {
			if (sp->sub.pts != AV_NOPTS_VALUE)
				pts = sp->sub.pts / (double)AV_TIME_BASE;
			sp->pts = pts;
			sp->serial = is->subdec.pkt_serial;
			sp->width = is->subdec.avctx->width;
			sp->height = is->subdec.avctx->height;
			sp->uploaded = 0;
			if (sp->sub.format != 0)
			{
				//if (!render)
				//{
				//	alloc_assrender(&render);
				//	if (!sp->width || !sp->height)
				//	{
				//		sp->width = is->viddec.avctx->width;
				//		sp->height = is->viddec.avctx->height;
				//	}
				//	initasspaintrender(render, is->subdec.avctx, sp->width, sp->height);
				//}
				//if (render)
				//{
				//	addsubtitlestotrack(render, &sp->sub);
				//}
			}
			/* now we can update the picture count */
			frame_queue_push(&is->subpq);
		}
		else if (got_subtitle) {
			avsubtitle_free(&sp->sub);
		}
	}
	return 0;
}

/* copy samples for viewing in editor window */
static void update_sample_display(VideoState* is, short* samples, int samples_size)
{
	int size, len;

	size = samples_size / sizeof(short);
	while (size > 0) {
		len = SAMPLE_ARRAY_SIZE - is->sample_array_index;
		if (len > size)
			len = size;
		memcpy(is->sample_array + is->sample_array_index, samples, len * sizeof(short));
		samples += len;
		is->sample_array_index += len;
		if (is->sample_array_index >= SAMPLE_ARRAY_SIZE)
			is->sample_array_index = 0;
		size -= len;
	}
}

/* return the wanted number of samples to get better sync if sync_type is video
 * or external master clock 判断音频同步到视频*/
static int synchronize_audio(VideoState* is, int nb_samples)
{
	int wanted_nb_samples = nb_samples;

	/* if not master, then we try to remove or add samples to correct the clock */
	if (get_master_sync_type(is) != AV_SYNC_AUDIO_MASTER) {
		double diff, avg_diff;
		int min_nb_samples, max_nb_samples;

		diff = get_clock(&is->audclk) - get_master_clock(is);

		if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD) {
			is->audio_diff_cum = diff + is->audio_diff_avg_coef * is->audio_diff_cum;
			if (is->audio_diff_avg_count < AUDIO_DIFF_AVG_NB) {
				/* not enough measures to have a correct estimate */
				is->audio_diff_avg_count++;
			}
			else {
				/* estimate the A-V difference */
				avg_diff = is->audio_diff_cum * (1.0 - is->audio_diff_avg_coef);

				if (fabs(avg_diff) >= is->audio_diff_threshold) {
					wanted_nb_samples = nb_samples + (int)(diff * is->audio_src.freq);
					min_nb_samples = ((nb_samples * (100 - SAMPLE_CORRECTION_PERCENT_MAX) / 100));
					max_nb_samples = ((nb_samples * (100 + SAMPLE_CORRECTION_PERCENT_MAX) / 100));
					wanted_nb_samples = av_clip(wanted_nb_samples, min_nb_samples, max_nb_samples);
				}
				av_log(NULL, AV_LOG_TRACE, "diff=%f adiff=%f sample_diff=%d apts=%0.3f %f\n",
					diff, avg_diff, wanted_nb_samples - nb_samples,
					is->audio_clock, is->audio_diff_threshold);
			}
		}
		else {
			/* too big difference : may be initial PTS errors, so
			   reset A-V filter */
			is->audio_diff_avg_count = 0;
			is->audio_diff_cum = 0;
		}
	}

	return wanted_nb_samples;
}

/**
 * Decode one audio frame and return its uncompressed size.
 *
 * The processed audio frame is decoded, converted if required, and
 * stored in is->audio_buf, with size in bytes given by the return
 * value.
 */
int play_ctx::audio_decode_frame(VideoState* is)
{
	int data_size, resampled_data_size;
	av_unused double audio_clock0;
	int wanted_nb_samples;
	Frame* af;

	if (is->paused)
		return -1;

	do {
#if defined(_WIN32)
		while (frame_queue_nb_remaining(&is->sampq) == 0) {
			if ((av_gettime_relative() - audio_callback_time) > 1000000LL * is->audio_hw_buf_size / is->audio_tgt.bytes_per_sec / 2)
				return -1;
			av_usleep(1000);
		}
#endif
		if (!(af = frame_queue_peek_readable(&is->sampq)))
			return -1;
		frame_queue_next(&is->sampq);
	} while (af->serial != is->audioq.serial);

	data_size = av_samples_get_buffer_size(NULL, af->frame->ch_layout.nb_channels,
		af->frame->nb_samples,
		(AVSampleFormat)af->frame->format, 1);

	wanted_nb_samples = synchronize_audio(is, af->frame->nb_samples);

	if (af->frame->format != is->audio_src.fmt ||
		av_channel_layout_compare(&af->frame->ch_layout, &is->audio_src.ch_layout) ||
		af->frame->sample_rate != is->audio_src.freq ||
		(wanted_nb_samples != af->frame->nb_samples && !is->swr_ctx)) {
		swr_free(&is->swr_ctx);
		swr_alloc_set_opts2(&is->swr_ctx,
			&is->audio_tgt.ch_layout, is->audio_tgt.fmt, is->audio_tgt.freq,
			&af->frame->ch_layout, (AVSampleFormat)af->frame->format, af->frame->sample_rate,
			0, NULL);
		if (!is->swr_ctx || swr_init(is->swr_ctx) < 0) {
			av_log(NULL, AV_LOG_ERROR,
				"Cannot create sample rate converter for conversion of %d Hz %s %d channels to %d Hz %s %d channels!\n",
				af->frame->sample_rate, av_get_sample_fmt_name((AVSampleFormat)af->frame->format), af->frame->ch_layout.nb_channels,
				is->audio_tgt.freq, av_get_sample_fmt_name(is->audio_tgt.fmt), is->audio_tgt.ch_layout.nb_channels);
			swr_free(&is->swr_ctx);
			return -1;
		}
		if (av_channel_layout_copy(&is->audio_src.ch_layout, &af->frame->ch_layout) < 0)
			return -1;
		is->audio_src.freq = af->frame->sample_rate;
		is->audio_src.fmt = (AVSampleFormat)af->frame->format;
	}

	if (is->swr_ctx) {
		const uint8_t** in = (const uint8_t**)af->frame->extended_data;
		uint8_t** out = &is->audio_buf1;
		int out_count = (int64_t)wanted_nb_samples * is->audio_tgt.freq / af->frame->sample_rate + 256;
		int out_size = av_samples_get_buffer_size(NULL, is->audio_tgt.ch_layout.nb_channels, out_count, is->audio_tgt.fmt, 0);
		int len2;
		if (out_size < 0) {
			av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size() failed\n");
			return -1;
		}
		if (wanted_nb_samples != af->frame->nb_samples) {
			if (swr_set_compensation(is->swr_ctx, (wanted_nb_samples - af->frame->nb_samples) * is->audio_tgt.freq / af->frame->sample_rate,
				wanted_nb_samples * is->audio_tgt.freq / af->frame->sample_rate) < 0) {
				av_log(NULL, AV_LOG_ERROR, "swr_set_compensation() failed\n");
				return -1;
			}
		}
		av_fast_malloc(&is->audio_buf1, &is->audio_buf1_size, out_size);
		if (!is->audio_buf1)
			return AVERROR(ENOMEM);
		len2 = swr_convert(is->swr_ctx, out, out_count, in, af->frame->nb_samples);
		if (len2 < 0) {
			av_log(NULL, AV_LOG_ERROR, "swr_convert() failed\n");
			return -1;
		}
		if (len2 == out_count) {
			av_log(NULL, AV_LOG_WARNING, "audio buffer is probably too small\n");
			if (swr_init(is->swr_ctx) < 0)
				swr_free(&is->swr_ctx);
		}
		is->audio_buf = is->audio_buf1;
		resampled_data_size = len2 * is->audio_tgt.ch_layout.nb_channels * av_get_bytes_per_sample(is->audio_tgt.fmt);
	}
	else {
		is->audio_buf = af->frame->data[0];
		resampled_data_size = data_size;
	}

	audio_clock0 = is->audio_clock;
	/* update the audio clock with the pts */
	if (!isnan(af->pts))
		is->audio_clock = af->pts + (double)af->frame->nb_samples / af->frame->sample_rate;
	else
		is->audio_clock = NAN;
	is->audio_clock_serial = af->serial;
#ifdef DEBUG
	{
		static double last_clock;
		printf("audio: delay=%0.3f clock=%0.3f clock0=%0.3f\n",
			is->audio_clock - last_clock,
			is->audio_clock, audio_clock0);
		last_clock = is->audio_clock;
	}
#endif
	return resampled_data_size;
}

/* prepare a new audio buffer */
void play_ctx::sdl_audio_callback(void* opaque, Uint8* stream, int len)
{
	VideoState* is = (VideoState*)opaque;
	int audio_size, len1;
	if (!is)return;
	audio_callback_time = av_gettime_relative();

	while (len > 0) {
		if (is->audio_buf_index >= is->audio_buf_size) {
			audio_size = audio_decode_frame(is);
			if (audio_size < 0) {
				/* if error, just output silence */
				is->audio_buf = NULL;
				is->audio_buf_size = SDL_AUDIO_MIN_BUFFER_SIZE / is->audio_tgt.frame_size * is->audio_tgt.frame_size;
			}
			else {
				if (is->show_mode != SHOW_MODE_VIDEO)
					update_sample_display(is, (int16_t*)is->audio_buf, audio_size);
				is->audio_buf_size = audio_size;
			}
			is->audio_buf_index = 0;
		}
		len1 = is->audio_buf_size - is->audio_buf_index;
		if (len1 > len)
			len1 = len;
		if (!is->muted && is->audio_buf && is->audio_volume == SDL_MIX_MAXVOLUME)
			memcpy(stream, (uint8_t*)is->audio_buf + is->audio_buf_index, len1);
		else {
			memset(stream, 0, len1);
			if (!is->muted && is->audio_buf)
				SDL_MixAudioFormat(stream, (uint8_t*)is->audio_buf + is->audio_buf_index, AUDIO_S16SYS, len1, is->audio_volume);
		}
		len -= len1;
		stream += len1;
		is->audio_buf_index += len1;
	}
	is->audio_write_buf_size = is->audio_buf_size - is->audio_buf_index;
	/* Let's assume the audio driver that is used by SDL has two periods. */
	if (!isnan(is->audio_clock)) {
		set_clock_at(&is->audclk, is->audio_clock - (double)(2 * is->audio_hw_buf_size + is->audio_write_buf_size) / is->audio_tgt.bytes_per_sec, is->audio_clock_serial, audio_callback_time / 1000000.0);
		sync_clock_to_slave(&is->extclk, &is->audclk);
	}
}
//int64_t kti = 0, dt = 0, dt1 = 0;
static void sdl_audio_callback1(void* opaque, Uint8* stream, int len)
{
	//dt = av_gettime_relative() - kti;
	auto p = (play_ctx*)((VideoState*)opaque)->userptr;
	auto qas = SDL_GetQueuedAudioSize(p->audio_dev);
	p->sdl_audio_callback(opaque, stream, len);
	//dt1 = av_gettime_relative() - p->audio_callback_time;

	//kti = av_gettime_relative();
}
void play_ctx::push_audio_thread()
{
	while (1)
	{
		auto is = tis;
		if (is->abort_request)
			break;
		double n = abufv.size();
		//auto dt = (av_gettime_relative() - kti);
		auto qas = SDL_GetQueuedAudioSize(audio_dev);
		sdl_audio_callback(is, abufv.data(), n);
		SDL_QueueAudio(audio_dev, abufv.data(), n);
		//kti = av_gettime_relative();
		//auto nk = 1000000LL * is->audio_hw_buf_size / is->audio_tgt.bytes_per_sec;
		auto nk1 = 1000000LL * qas / is->audio_tgt.bytes_per_sec / 2;
		av_usleep(nk1);
	}

}
int play_ctx::audio_open(void* opaque, AVChannelLayout* wanted_channel_layout, int wanted_sample_rate, struct AudioParams* audio_hw_params)
{
	SDL_AudioSpec wanted_spec, spec;
	const char* env;
	static const int next_nb_channels[] = { 0, 0, 1, 6, 2, 6, 4, 6 };
	static const int next_sample_rates[] = { 0, 44100, 48000, 96000, 192000 };
	int next_sample_rate_idx = FF_ARRAY_ELEMS(next_sample_rates) - 1;
	int wanted_nb_channels = wanted_channel_layout->nb_channels;

	env = SDL_getenv("SDL_AUDIO_CHANNELS");
	if (env) {
		wanted_nb_channels = atoi(env);
		av_channel_layout_uninit(wanted_channel_layout);
		av_channel_layout_default(wanted_channel_layout, wanted_nb_channels);
	}
	if (wanted_channel_layout->order != AV_CHANNEL_ORDER_NATIVE) {
		av_channel_layout_uninit(wanted_channel_layout);
		av_channel_layout_default(wanted_channel_layout, wanted_nb_channels);
	}
	wanted_nb_channels = wanted_channel_layout->nb_channels;
	wanted_spec.channels = wanted_nb_channels;
	wanted_spec.freq = wanted_sample_rate;
	if (wanted_spec.freq <= 0 || wanted_spec.channels <= 0) {
		av_log(NULL, AV_LOG_ERROR, "Invalid sample rate or channel count!\n");
		return -1;
	}
	while (next_sample_rate_idx && next_sample_rates[next_sample_rate_idx] >= wanted_spec.freq)
		next_sample_rate_idx--;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.silence = 0;
	wanted_spec.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE, 2 << av_log2(wanted_spec.freq / SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
	wanted_spec.callback = ispushaudio ? 0 : sdl_audio_callback1;
	wanted_spec.userdata = opaque;
	while (!(audio_dev = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, &spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE))) {
		av_log(NULL, AV_LOG_WARNING, "SDL_OpenAudio (%d channels, %d Hz): %s\n",
			wanted_spec.channels, wanted_spec.freq, SDL_GetError());
		wanted_spec.channels = next_nb_channels[FFMIN(7, wanted_spec.channels)];
		if (!wanted_spec.channels) {
			wanted_spec.freq = next_sample_rates[next_sample_rate_idx--];
			wanted_spec.channels = wanted_nb_channels;
			if (!wanted_spec.freq) {
				av_log(NULL, AV_LOG_ERROR,
					"No more combinations to try, audio open failed\n");
				return -1;
			}
		}
		av_channel_layout_default(wanted_channel_layout, wanted_spec.channels);
	}
	if (spec.size > 0)
	{
		abufv.resize(spec.size);
	}
	else
	{
		av_log(NULL, AV_LOG_ERROR,
			"SDL advised audio size %d is not supported!\n", spec.size);
		return -1;
	}
	if (spec.format != AUDIO_S16SYS) {
		av_log(NULL, AV_LOG_ERROR,
			"SDL advised audio format %d is not supported!\n", spec.format);
		return -1;
	}
	if (spec.channels != wanted_spec.channels) {
		av_channel_layout_uninit(wanted_channel_layout);
		av_channel_layout_default(wanted_channel_layout, spec.channels);
		if (wanted_channel_layout->order != AV_CHANNEL_ORDER_NATIVE) {
			av_log(NULL, AV_LOG_ERROR,
				"SDL advised channel count %d is not supported!\n", spec.channels);
			return -1;
		}
	}

	audio_hw_params->fmt = AV_SAMPLE_FMT_S16;
	audio_hw_params->freq = spec.freq;
	if (av_channel_layout_copy(&audio_hw_params->ch_layout, wanted_channel_layout) < 0)
		return -1;
	audio_hw_params->frame_size = av_samples_get_buffer_size(NULL, audio_hw_params->ch_layout.nb_channels, 1, audio_hw_params->fmt, 1);
	audio_hw_params->bytes_per_sec = av_samples_get_buffer_size(NULL, audio_hw_params->ch_layout.nb_channels, audio_hw_params->freq, audio_hw_params->fmt, 1);
	if (audio_hw_params->bytes_per_sec <= 0 || audio_hw_params->frame_size <= 0) {
		av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size failed\n");
		return -1;
	}
	return spec.size;
}

static int audio_thread1(void* arg)
{
	auto p = (play_ctx*)((VideoState*)arg)->userptr;
	return p->audio_thread(arg);
}
static int video_thread1(void* arg)
{
	auto p = (play_ctx*)((VideoState*)arg)->userptr;
	return p->video_thread(arg);
}
static int subtitle_thread1(void* arg)
{
	auto p = (play_ctx*)((VideoState*)arg)->userptr;
	return p->subtitle_thread(arg);
}
// hw
#if 1


typedef struct HWDevice {
	const char* name;
	enum AVHWDeviceType type;
	AVBufferRef* device_ref;
} HWDevice;


//#include "ffmpeg.h"
HWDevice* hw_device_get_by_name(const char* name);
int hw_device_init_from_string(const char* arg, HWDevice** dev);
void hw_device_free_all(void);
#ifdef InputStream
int hw_device_setup_for_decode(InputStream* ist);
int hw_device_setup_for_encode(OutputStream* ost);
int hw_device_setup_for_filter(FilterGraph* fg);
#endif

static int nb_hw_devices;
static HWDevice** hw_devices;

static HWDevice* hw_device_get_by_type(enum AVHWDeviceType type)
{
	HWDevice* found = NULL;
	int i;
	for (i = 0; i < nb_hw_devices; i++) {
		if (hw_devices[i]->type == type) {
			if (found)
				return NULL;
			found = hw_devices[i];
		}
	}
	return found;
}

HWDevice* hw_device_get_by_name(const char* name)
{
	int i;
	for (i = 0; i < nb_hw_devices; i++) {
		if (!strcmp(hw_devices[i]->name, name))
			return hw_devices[i];
	}
	return NULL;
}

static HWDevice* hw_device_add(void)
{
	int err;
	err = av_reallocp_array(&hw_devices, nb_hw_devices + 1,
		sizeof(*hw_devices));
	if (err) {
		nb_hw_devices = 0;
		return NULL;
	}
	hw_devices[nb_hw_devices] = (HWDevice*)av_mallocz(sizeof(HWDevice));
	if (!hw_devices[nb_hw_devices])
		return NULL;
	return hw_devices[nb_hw_devices++];
}

static char* hw_device_default_name(enum AVHWDeviceType type)
{
	// Make an automatic name of the form "type%d".  We arbitrarily
	// limit at 1000 anonymous devices of the same type - there is
	// probably something else very wrong if you get to this limit.
	const char* type_name = av_hwdevice_get_type_name(type);
	char* name;
	size_t index_pos;
	int index, index_limit = 1000;
	index_pos = strlen(type_name);
	name = (char*)av_malloc(index_pos + 4);
	if (!name)
		return NULL;
	for (index = 0; index < index_limit; index++) {
		snprintf(name, index_pos + 4, "%s%d", type_name, index);
		if (!hw_device_get_by_name(name))
			break;
	}
	if (index >= index_limit) {
		av_freep(&name);
		return NULL;
	}
	return name;
}

int hw_device_init_from_string(const char* arg, HWDevice** dev_out)
{
	// "type=name"
	// "type=name,key=value,key2=value2"
	// "type=name:device,key=value,key2=value2"
	// "type:device,key=value,key2=value2"
	// -> av_hwdevice_ctx_create()
	// "type=name@name"
	// "type@name"
	// -> av_hwdevice_ctx_create_derived()

	AVDictionary* options = NULL;
	const char* type_name = NULL, * name = NULL, * device = NULL;
	enum AVHWDeviceType type;
	HWDevice* dev, * src;
	AVBufferRef* device_ref = NULL;
	int err;
	const char* errmsg, * p, * q;
	size_t k;

	k = strcspn(arg, ":=@");
	p = arg + k;

	type_name = av_strndup(arg, k);
	if (!type_name) {
		err = AVERROR(ENOMEM);
		goto fail;
	}
	type = av_hwdevice_find_type_by_name(type_name);
	if (type == AV_HWDEVICE_TYPE_NONE) {
		errmsg = "unknown device type";
		goto invalid;
	}

	if (*p == '=') {
		k = strcspn(p + 1, ":@,");

		name = av_strndup(p + 1, k);
		if (!name) {
			err = AVERROR(ENOMEM);
			goto fail;
		}
		if (hw_device_get_by_name(name)) {
			errmsg = "named device already exists";
			goto invalid;
		}

		p += 1 + k;
	}
	else {
		name = hw_device_default_name(type);
		if (!name) {
			err = AVERROR(ENOMEM);
			goto fail;
		}
	}

	if (!*p) {
		// New device with no parameters.
		err = av_hwdevice_ctx_create(&device_ref, type,
			NULL, NULL, 0);
		if (err < 0)
			goto fail;

	}
	else if (*p == ':') {
		// New device with some parameters.
		++p;
		q = strchr(p, ',');
		if (q) {
			if (q - p > 0) {
				device = av_strndup(p, q - p);
				if (!device) {
					err = AVERROR(ENOMEM);
					goto fail;
				}
			}
			err = av_dict_parse_string(&options, q + 1, "=", ",", 0);
			if (err < 0) {
				errmsg = "failed to parse options";
				goto invalid;
			}
		}

		err = av_hwdevice_ctx_create(&device_ref, type,
			q ? device : p[0] ? p : NULL,
			options, 0);
		if (err < 0)
			goto fail;

	}
	else if (*p == '@') {
		// Derive from existing device.

		src = hw_device_get_by_name(p + 1);
		if (!src) {
			errmsg = "invalid source device name";
			goto invalid;
		}

		err = av_hwdevice_ctx_create_derived(&device_ref, type,
			src->device_ref, 0);
		if (err < 0)
			goto fail;
	}
	else if (*p == ',') {
		err = av_dict_parse_string(&options, p + 1, "=", ",", 0);

		if (err < 0) {
			errmsg = "failed to parse options";
			goto invalid;
		}

		err = av_hwdevice_ctx_create(&device_ref, type,
			NULL, options, 0);
		if (err < 0)
			goto fail;
	}
	else {
		errmsg = "parse error";
		goto invalid;
	}

	dev = hw_device_add();
	if (!dev) {
		err = AVERROR(ENOMEM);
		goto fail;
	}

	dev->name = name;
	dev->type = type;
	dev->device_ref = device_ref;

	if (dev_out)
		*dev_out = dev;

	name = NULL;
	err = 0;
done:
	av_freep(&type_name);
	av_freep(&name);
	av_freep(&device);
	av_dict_free(&options);
	return err;
invalid:
	av_log(NULL, AV_LOG_ERROR,
		"Invalid device specification \"%s\": %s\n", arg, errmsg);
	err = AVERROR(EINVAL);
	goto done;
fail:
	av_log(NULL, AV_LOG_ERROR,
		"Device creation failed: %d.\n", err);
	av_buffer_unref(&device_ref);
	goto done;
}

static int hw_device_init_from_type(enum AVHWDeviceType type,
	const char* device,
	HWDevice** dev_out)
{
	AVBufferRef* device_ref = NULL;
	HWDevice* dev;
	char* name;
	int err;

	name = hw_device_default_name(type);
	if (!name) {
		err = AVERROR(ENOMEM);
		goto fail;
	}

	err = av_hwdevice_ctx_create(&device_ref, type, device, NULL, 0);
	if (err < 0) {
		av_log(NULL, AV_LOG_ERROR,
			"Device creation failed: %d.\n", err);
		goto fail;
	}

	dev = hw_device_add();
	if (!dev) {
		err = AVERROR(ENOMEM);
		goto fail;
	}

	dev->name = name;
	dev->type = type;
	dev->device_ref = device_ref;

	if (dev_out)
		*dev_out = dev;

	return 0;

fail:
	av_freep(&name);
	av_buffer_unref(&device_ref);
	return err;
}

void hw_device_free_all(void)
{
	int i;
	for (i = 0; i < nb_hw_devices; i++) {
		av_freep(&hw_devices[i]->name);
		av_buffer_unref(&hw_devices[i]->device_ref);
		av_freep(&hw_devices[i]);
	}
	av_freep(&hw_devices);
	nb_hw_devices = 0;
}

static HWDevice* hw_device_match_by_codec(const AVCodec* codec)
{
	const AVCodecHWConfig* config;
	HWDevice* dev;
	int i;
	for (i = 0;; i++) {
		config = avcodec_get_hw_config(codec, i);
		if (!config)
			return NULL;
		if (!(config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX))
			continue;
		dev = hw_device_get_by_type(config->device_type);
		if (dev)
			return dev;
	}
}
#ifdef InputStream
int hw_device_setup_for_decode(InputStream* ist)
{
	const AVCodecHWConfig* config;
	enum AVHWDeviceType type;
	HWDevice* dev = NULL;
	int err, auto_device = 0;

	if (ist->hwaccel_device) {
		dev = hw_device_get_by_name(ist->hwaccel_device);
		if (!dev) {
			if (ist->hwaccel_id == HWACCEL_AUTO) {
				auto_device = 1;
			}
			else if (ist->hwaccel_id == HWACCEL_GENERIC) {
				type = ist->hwaccel_device_type;
				err = hw_device_init_from_type(type, ist->hwaccel_device,
					&dev);
			}
			else {
				// This will be dealt with by API-specific initialisation
				// (using hwaccel_device), so nothing further needed here.
				return 0;
			}
		}
		else {
			if (ist->hwaccel_id == HWACCEL_AUTO) {
				ist->hwaccel_device_type = dev->type;
			}
			else if (ist->hwaccel_device_type != dev->type) {
				av_log(NULL, AV_LOG_ERROR, "Invalid hwaccel device "
					"specified for decoder: device %s of type %s is not "
					"usable with hwaccel %s.\n", dev->name,
					av_hwdevice_get_type_name(dev->type),
					av_hwdevice_get_type_name(ist->hwaccel_device_type));
				return AVERROR(EINVAL);
			}
		}
	}
	else {
		if (ist->hwaccel_id == HWACCEL_AUTO) {
			auto_device = 1;
		}
		else if (ist->hwaccel_id == HWACCEL_GENERIC) {
			type = ist->hwaccel_device_type;
			dev = hw_device_get_by_type(type);

			// When "-qsv_device device" is used, an internal QSV device named
			// as "__qsv_device" is created. Another QSV device is created too
			// if "-init_hw_device qsv=name:device" is used. There are 2 QSV devices
			// if both "-qsv_device device" and "-init_hw_device qsv=name:device"
			// are used, hw_device_get_by_type(AV_HWDEVICE_TYPE_QSV) returns NULL.
			// To keep back-compatibility with the removed ad-hoc libmfx setup code,
			// call hw_device_get_by_name("__qsv_device") to select the internal QSV
			// device.
			if (!dev && type == AV_HWDEVICE_TYPE_QSV)
				dev = hw_device_get_by_name("__qsv_device");

			if (!dev)
				err = hw_device_init_from_type(type, NULL, &dev);
		}
		else {
			dev = hw_device_match_by_codec(ist->dec);
			if (!dev) {
				// No device for this codec, but not using generic hwaccel
				// and therefore may well not need one - ignore.
				return 0;
			}
		}
	}

	if (auto_device) {
		int i;
		if (!avcodec_get_hw_config(ist->dec, 0)) {
			// Decoder does not support any hardware devices.
			return 0;
		}
		for (i = 0; !dev; i++) {
			config = avcodec_get_hw_config(ist->dec, i);
			if (!config)
				break;
			type = config->device_type;
			dev = hw_device_get_by_type(type);
			if (dev) {
				av_log(NULL, AV_LOG_INFO, "Using auto "
					"hwaccel type %s with existing device %s.\n",
					av_hwdevice_get_type_name(type), dev->name);
			}
		}
		for (i = 0; !dev; i++) {
			config = avcodec_get_hw_config(ist->dec, i);
			if (!config)
				break;
			type = config->device_type;
			// Try to make a new device of this type.
			err = hw_device_init_from_type(type, ist->hwaccel_device,
				&dev);
			if (err < 0) {
				// Can't make a device of this type.
				continue;
			}
			if (ist->hwaccel_device) {
				av_log(NULL, AV_LOG_INFO, "Using auto "
					"hwaccel type %s with new device created "
					"from %s.\n", av_hwdevice_get_type_name(type),
					ist->hwaccel_device);
			}
			else {
				av_log(NULL, AV_LOG_INFO, "Using auto "
					"hwaccel type %s with new default device.\n",
					av_hwdevice_get_type_name(type));
			}
		}
		if (dev) {
			ist->hwaccel_device_type = type;
		}
		else {
			av_log(NULL, AV_LOG_INFO, "Auto hwaccel "
				"disabled: no device found.\n");
			ist->hwaccel_id = HWACCEL_NONE;
			return 0;
		}
	}

	if (!dev) {
		av_log(NULL, AV_LOG_ERROR, "No device available "
			"for decoder: device type %s needed for codec %s.\n",
			av_hwdevice_get_type_name(type), ist->dec->name);
		return err;
	}

	ist->dec_ctx->hw_device_ctx = av_buffer_ref(dev->device_ref);
	if (!ist->dec_ctx->hw_device_ctx)
		return AVERROR(ENOMEM);

	return 0;
}

int hw_device_setup_for_encode(OutputStream* ost)
{
	const AVCodecHWConfig* config;
	HWDevice* dev = NULL;
	AVBufferRef* frames_ref = NULL;
	int i;

	if (ost->filter) {
		frames_ref = av_buffersink_get_hw_frames_ctx(ost->filter->filter);
		if (frames_ref &&
			((AVHWFramesContext*)frames_ref->data)->format ==
			ost->enc_ctx->pix_fmt) {
			// Matching format, will try to use hw_frames_ctx.
		}
		else {
			frames_ref = NULL;
		}
	}

	for (i = 0;; i++) {
		config = avcodec_get_hw_config(ost->enc, i);
		if (!config)
			break;

		if (frames_ref &&
			config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_FRAMES_CTX &&
			(config->pix_fmt == AV_PIX_FMT_NONE ||
				config->pix_fmt == ost->enc_ctx->pix_fmt)) {
			av_log(ost->enc_ctx, AV_LOG_VERBOSE, "Using input "
				"frames context (format %s) with %s encoder.\n",
				av_get_pix_fmt_name(ost->enc_ctx->pix_fmt),
				ost->enc->name);
			ost->enc_ctx->hw_frames_ctx = av_buffer_ref(frames_ref);
			if (!ost->enc_ctx->hw_frames_ctx)
				return AVERROR(ENOMEM);
			return 0;
		}

		if (!dev &&
			config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX)
			dev = hw_device_get_by_type(config->device_type);
	}

	if (dev) {
		av_log(ost->enc_ctx, AV_LOG_VERBOSE, "Using device %s "
			"(type %s) with %s encoder.\n", dev->name,
			av_hwdevice_get_type_name(dev->type), ost->enc->name);
		ost->enc_ctx->hw_device_ctx = av_buffer_ref(dev->device_ref);
		if (!ost->enc_ctx->hw_device_ctx)
			return AVERROR(ENOMEM);
	}
	else {
		// No device required, or no device available.
	}
	return 0;
}

static int hwaccel_retrieve_data(AVCodecContext* avctx, AVFrame* input)
{
	InputStream* ist = (InputStream*)avctx->opaque;
	AVFrame* output = NULL;
	enum AVPixelFormat output_format = ist->hwaccel_output_format;
	int err;

	if (input->format == output_format) {
		// Nothing to do.
		return 0;
	}

	output = av_frame_alloc();
	if (!output)
		return AVERROR(ENOMEM);

	output->format = output_format;

	err = av_hwframe_transfer_data(output, input, 0);
	if (err < 0) {
		av_log(avctx, AV_LOG_ERROR, "Failed to transfer data to "
			"output frame: %d.\n", err);
		goto fail;
	}

	err = av_frame_copy_props(output, input);
	if (err < 0) {
		av_frame_unref(output);
		goto fail;
	}

	av_frame_unref(input);
	av_frame_move_ref(input, output);
	av_frame_free(&output);

	return 0;

fail:
	av_frame_free(&output);
	return err;
}

int hwaccel_decode_init(AVCodecContext* avctx)
{
	InputStream* ist = (InputStream*)avctx->opaque;

	ist->hwaccel_retrieve_data = &hwaccel_retrieve_data;

	return 0;
}

int hw_device_setup_for_filter(FilterGraph* fg, HWDevice* filter_hw_device)
{
	HWDevice* dev;
	int i;

	// Pick the last hardware device if the user doesn't pick the device for
	// filters explicitly with the filter_hw_device option.
	if (filter_hw_device)
		dev = filter_hw_device;
	else if (nb_hw_devices > 0) {
		dev = hw_devices[nb_hw_devices - 1];

		if (nb_hw_devices > 1)
			av_log(NULL, AV_LOG_WARNING, "There are %d hardware devices. device "
				"%s of type %s is picked for filters by default. Set hardware "
				"device explicitly with the filter_hw_device option if device "
				"%s is not usable for filters.\n",
				nb_hw_devices, dev->name,
				av_hwdevice_get_type_name(dev->type), dev->name);
	}
	else
		dev = NULL;

	if (dev) {
		for (i = 0; i < fg->graph->nb_filters; i++) {
			fg->graph->filters[i]->hw_device_ctx =
				av_buffer_ref(dev->device_ref);
			if (!fg->graph->filters[i]->hw_device_ctx)
				return AVERROR(ENOMEM);
		}
	}

	return 0;
}
#endif

#endif // 1

enum AVPixelFormat hw_get_format(AVCodecContext* ctx, const enum AVPixelFormat* fmts)
{
	auto pc = (play_ctx*)ctx->opaque;
	const enum AVPixelFormat* p;
	for (p = fmts; *p != AV_PIX_FMT_NONE; p++) {
		if (*p == pc->hw_device_pixel) {
			return *p;
		}
	}

	return AV_PIX_FMT_NONE;
}
//
//void init_dxva2(VideoState* is, const AVCodec* codec, AVCodecContext* avctx)
//{
//	if (is->hwaccel == AC_HARDWAREACCELERATETYPE_AUTO || is->hwaccel == AC_HARDWAREACCELERATETYPE_DXVA)
//	{
//		switch (codec->id)
//			//dxva2支持的格式
//		{
//		case AV_CODEC_ID_MPEG2VIDEO:
//		case AV_CODEC_ID_H264:
//		case AV_CODEC_ID_VC1:
//		case AV_CODEC_ID_WMV3:
//		case AV_CODEC_ID_HEVC:
//		case AV_CODEC_ID_VP9:
//			//while (1)
//		{
//		
//		}
//		break;
//		}
//	}
//}


/* open a given stream. Return 0 if OK */
int play_ctx::stream_component_open(VideoState* is, int stream_index, const AVCodec* codec)
{
	AVFormatContext* ic = is->ic;
	AVCodecContext* avctx;
	const char* forced_codec_name = NULL;
	AVDictionary* opts = NULL;
	const AVDictionaryEntry* t = NULL;
	int sample_rate;
	AVChannelLayout ch_layout = {};
	int ret = 0;
	int stream_lowres = lowres;

	if (stream_index < 0 || stream_index >= ic->nb_streams)
		return -1;

	avctx = avcodec_alloc_context3(codec);
	if (!avctx)
		return AVERROR(ENOMEM);
	bool fail = true;
	ret = avcodec_parameters_to_context(avctx, ic->streams[stream_index]->codecpar);
	do {
		if (ret < 0)
			break;
		avctx->pkt_timebase = ic->streams[stream_index]->time_base;
		if (!codec)
			codec = avcodec_find_decoder(avctx->codec_id);

		switch (avctx->codec_type) {
		case AVMEDIA_TYPE_AUDIO: is->last_audio_stream = stream_index; forced_codec_name = audio_codec_name; break;
		case AVMEDIA_TYPE_SUBTITLE: is->last_subtitle_stream = stream_index; forced_codec_name = subtitle_codec_name; break;
		case AVMEDIA_TYPE_VIDEO:
		{
			if (hwctx)
			{
				auto fc = av_hwdevice_get_hwframe_constraints(hwctx->hdc, 0);
				//auto kcodec = avcodec_find_decoder_by_name("");
				avctx->opaque = this;
				avctx->get_format = hw_get_format;
				avctx->hw_device_ctx = hwctx->a();
				for (int i = 0;; i++) {
					const AVCodecHWConfig* hwcodec = avcodec_get_hw_config(codec, i);
					if (hwcodec == NULL) break;

					// 可能一个解码器对应着多个硬件加速方式，所以这里将其挑选出来
					if (hwcodec->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX && hwcodec->device_type == hwctx->t) {
						hw_device_pixel = hwcodec->pix_fmt;
					}
				}
				avctx->thread_count = 1;
			}
			is->last_video_stream = stream_index; forced_codec_name = video_codec_name;
		}
		break;
		}

		if (forced_codec_name)
			codec = avcodec_find_decoder_by_name(forced_codec_name);
		if (!codec) {
			if (forced_codec_name) av_log(NULL, AV_LOG_WARNING,
				"No codec could be found with name '%s'\n", forced_codec_name);
			else                   av_log(NULL, AV_LOG_WARNING,
				"No decoder could be found for codec %s\n", avcodec_get_name(avctx->codec_id));
			ret = AVERROR(EINVAL);
			break;
		}
		if (!hwctx || avctx->codec_type != AVMEDIA_TYPE_VIDEO)
			avctx->codec_id = codec->id;
		if (stream_lowres > codec->max_lowres) {
			av_log(avctx, AV_LOG_WARNING, "The maximum value for lowres supported by the decoder is %d\n",
				codec->max_lowres);
			stream_lowres = codec->max_lowres;
		}
		avctx->lowres = stream_lowres;

		if (fast)
			avctx->flags2 |= AV_CODEC_FLAG2_FAST;

		opts = filter_codec_opts(codec_opts, avctx->codec_id, ic, ic->streams[stream_index], codec);
		if (!av_dict_get(opts, "threads", NULL, 0))
			av_dict_set(&opts, "threads", "auto", 0);
		if (stream_lowres)
			av_dict_set_int(&opts, "lowres", stream_lowres, 0);
		if ((ret = avcodec_open2(avctx, codec, &opts)) < 0) {
			break;
		}
		if ((t = av_dict_get(opts, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
			av_log(NULL, AV_LOG_ERROR, "Option %s not found.\n", t->key);
			ret = AVERROR_OPTION_NOT_FOUND;
			break;
		}

		fail = false;
		is->eof = 0;
		ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
		switch (avctx->codec_type) {
		case AVMEDIA_TYPE_AUDIO:
		{
#if CONFIG_AVFILTER
			AVFilterContext* sink;

			is->audio_filter_src.freq = avctx->sample_rate;
			ret = av_channel_layout_copy(&is->audio_filter_src.ch_layout, &avctx->ch_layout);
			if (ret < 0)
			{
				fail = true;
				break;
			}
			is->audio_filter_src.fmt = avctx->sample_fmt;
			if ((ret = configure_audio_filters(is, afilters, 0)) < 0)
			{
				fail = true;
				break;
			}
			sink = is->out_audio_filter;
			sample_rate = av_buffersink_get_sample_rate(sink);
			ret = av_buffersink_get_ch_layout(sink, &ch_layout);
			if (ret < 0)
			{
				fail = true;
				break;
			}
#else
			sample_rate = avctx->sample_rate;
			ret = av_channel_layout_copy(&ch_layout, &avctx->ch_layout);
			if (ret < 0)
				break;
#endif


			/* prepare audio output */
			if ((ret = audio_open(is, &ch_layout, sample_rate, &is->audio_tgt)) < 0)
			{
				fail = true;
				break;
			}
			is->audio_hw_buf_size = ret;
			is->audio_src = is->audio_tgt;
			is->audio_buf_size = 0;
			is->audio_buf_index = 0;

			/* init averaging filter */
			is->audio_diff_avg_coef = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
			is->audio_diff_avg_count = 0;
			/* since we do not have a precise anough audio FIFO fullness,
			   we correct audio sync only if larger than this threshold */
			is->audio_diff_threshold = (double)(is->audio_hw_buf_size) / is->audio_tgt.bytes_per_sec;

			is->audio_stream = stream_index;
			is->audio_st = ic->streams[stream_index];

			if ((ret = decoder_init(&is->auddec, avctx, &is->audioq, is->continue_read_thread)) < 0)
			{
				fail = true;
				break;
			}
			if ((is->ic->iformat->flags & (AVFMT_NOBINSEARCH | AVFMT_NOGENSEARCH | AVFMT_NO_BYTE_SEEK)) && !is->ic->iformat->read_seek) {
				is->auddec.start_pts = is->audio_st->start_time;
				is->auddec.start_pts_tb = is->audio_st->time_base;
			}
			if ((ret = decoder_start(&is->auddec, audio_thread1, "audio_decoder", is)) < 0)
			{
				fail = false;
				break;
			}
			thr_count++;
			SDL_PauseAudioDevice(audio_dev, 0);
			tis = is;
			if (ispushaudio)
			{
				assert(!audiopt.joinable());
				thr_count++;
				std::thread at1([=]() {
					push_audio_thread();
					});
				audiopt.swap(at1);
			}
		}
		break;
		case AVMEDIA_TYPE_VIDEO:
			is->video_stream = stream_index;
			is->video_st = ic->streams[stream_index];

			if ((ret = decoder_init(&is->viddec, avctx, &is->videoq, is->continue_read_thread)) < 0)
			{
				fail = true;
				break;
			}
			if ((ret = decoder_start(&is->viddec, video_thread1, "video_decoder", is)) < 0)
			{
				fail = false;
				break;
			}
			thr_count++;
			is->queue_attachments_req = 1;
			break;
		case AVMEDIA_TYPE_SUBTITLE:
			is->subtitle_stream = stream_index;
			is->subtitle_st = ic->streams[stream_index];

			if ((ret = decoder_init(&is->subdec, avctx, &is->subtitleq, is->continue_read_thread)) < 0)
			{
				fail = true;
				break;
			}
			if ((ret = decoder_start(&is->subdec, subtitle_thread1, "subtitle_decoder", is)) < 0)
			{
				fail = false;
				break;
			}
			thr_count++;
			break;
		default:
			break;
		}
		ret;
	} while (0);

	if (fail)
	{
		avcodec_free_context(&avctx);
	}
	av_channel_layout_uninit(&ch_layout);
	av_dict_free(&opts);

	return ret;
}

static int decode_interrupt_cb(void* ctx)
{
	VideoState* is = (VideoState*)ctx;
	return is->abort_request;
}

static int stream_has_enough_packets(AVStream* st, int stream_id, PacketQueue* queue) {
	return stream_id < 0 ||
		queue->abort_request ||
		(st->disposition & AV_DISPOSITION_ATTACHED_PIC) ||
		queue->nb_packets > MIN_FRAMES && (!queue->duration || av_q2d(st->time_base) * queue->duration > 1.0);
}

static int is_realtime(AVFormatContext* s)
{
	if (!strcmp(s->iformat->name, "rtp")
		|| !strcmp(s->iformat->name, "rtsp")
		|| !strcmp(s->iformat->name, "sdp")
		)
		return 1;

	if (s->pb && (!strncmp(s->url, "rtp:", 4)
		|| !strncmp(s->url, "udp:", 4)
		)
		)
		return 1;
	return 0;
}

/* this thread gets the stream from the disk or the network */
int play_ctx::read_thread(void* arg)
{
	VideoState* is = (VideoState*)arg;
	AVFormatContext* ic = NULL;
	int err, i, ret;
	int st_index[AVMEDIA_TYPE_NB];
	AVPacket* pkt = NULL;
	int64_t stream_start_time;
	int pkt_in_play_range = 0;
	const AVDictionaryEntry* t;
	SDL_mutex* wait_mutex = SDL_CreateMutex();
	int scan_all_pmts_set = 0;
	int64_t pkt_ts;
	const AVCodec* decoder = NULL;

	if (!wait_mutex) {
		av_log(NULL, AV_LOG_FATAL, "SDL_CreateMutex(): %s\n", SDL_GetError());
		ret = AVERROR(ENOMEM);
		goto fail;
	}

	memset(st_index, -1, sizeof(st_index));
	is->eof = 0;

	pkt = av_packet_alloc();
	if (!pkt) {
		av_log(NULL, AV_LOG_FATAL, "Could not allocate packet.\n");
		ret = AVERROR(ENOMEM);
		goto fail;
	}
	ic = avformat_alloc_context();
	if (!ic) {
		av_log(NULL, AV_LOG_FATAL, "Could not allocate context.\n");
		ret = AVERROR(ENOMEM);
		goto fail;
	}
	ic->interrupt_callback.callback = decode_interrupt_cb;
	ic->interrupt_callback.opaque = is;
	if (!av_dict_get(format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE)) {
		av_dict_set(&format_opts, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
		scan_all_pmts_set = 1;
	}
	err = avformat_open_input(&ic, is->filename, is->iformat, &format_opts);
	if (err < 0) {
		print_error(is->filename, err);
		ret = -1;
		goto fail;
	}
	if (scan_all_pmts_set)
		av_dict_set(&format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE);

	if ((t = av_dict_get(format_opts, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
		av_log(NULL, AV_LOG_ERROR, "Option %s not found.\n", t->key);
		ret = AVERROR_OPTION_NOT_FOUND;
		goto fail;
	}
	is->ic = ic;

	if (genpts)
		ic->flags |= AVFMT_FLAG_GENPTS;

	av_format_inject_global_side_data(ic);

	if (find_stream_info) {
		AVDictionary** opts = setup_find_stream_info_opts(ic, codec_opts);
		int orig_nb_streams = ic->nb_streams;

		err = avformat_find_stream_info(ic, opts);

		for (i = 0; i < orig_nb_streams; i++)
			av_dict_free(&opts[i]);
		av_freep(&opts);

		if (err < 0) {
			av_log(NULL, AV_LOG_WARNING,
				"%s: could not find codec parameters\n", is->filename);
			ret = -1;
			goto fail;
		}
	}

	if (ic->pb)
		ic->pb->eof_reached = 0; // FIXME hack, ffplay maybe should not use avio_feof() to test for the end

	if (seek_by_bytes < 0)
	{
		//seek_by_bytes = !(ic->iformat->flags & AVFMT_NO_BYTE_SEEK) &&
		//	!!(ic->iformat->flags & AVFMT_TS_DISCONT) &&
		//	strcmp("ogg", ic->iformat->name);
		seek_by_bytes = (!(ic->iformat->flags & AVFMT_NO_BYTE_SEEK) && (ic->iformat->flags & AVFMT_TS_DISCONT) && (0 != strcmp("ogg", ic->iformat->name)));
	}

	is->max_frame_duration = (ic->iformat->flags & AVFMT_TS_DISCONT) ? 10.0 : 3600.0;

	if (!window_title && (t = av_dict_get(ic->metadata, "title", NULL, 0)))
		window_title = av_asprintf("%s - %s", t->value, input_filename);

	/* if seeking requested, we execute it */
	if (start_time != AV_NOPTS_VALUE) {
		int64_t timestamp;

		timestamp = start_time;
		/* add the stream start time */
		if (ic->start_time != AV_NOPTS_VALUE)
			timestamp += ic->start_time;
		ret = avformat_seek_file(ic, -1, INT64_MIN, timestamp, INT64_MAX, 0);
		if (ret < 0) {
			av_log(NULL, AV_LOG_WARNING, "%s: could not seek to position %0.3f\n",
				is->filename, (double)timestamp / AV_TIME_BASE);
		}
	}

	is->realtime = is_realtime(ic);

	if (show_status)
		av_dump_format(ic, 0, is->filename, 0);

	for (i = 0; i < ic->nb_streams; i++) {
		AVStream* st = ic->streams[i];
		enum AVMediaType type = st->codecpar->codec_type;
		st->discard = AVDISCARD_ALL;
		if (type >= 0 && wanted_stream_spec[type] && st_index[type] == -1)
			if (avformat_match_stream_specifier(ic, st, wanted_stream_spec[type]) > 0)
				st_index[type] = i;
	}
	for (i = 0; i < AVMEDIA_TYPE_NB; i++) {
		if (wanted_stream_spec[i] && st_index[i] == -1) {
			av_log(NULL, AV_LOG_ERROR, "Stream specifier %s does not match any %s stream\n", wanted_stream_spec[i], av_get_media_type_string((AVMediaType)i));
			st_index[i] = INT_MAX;
		}
	}
	if (!video_disable)
	{
		st_index[AVMEDIA_TYPE_VIDEO] =
			av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO,
				st_index[AVMEDIA_TYPE_VIDEO], -1, &decoder, 0);

	}

	if (!audio_disable)
		st_index[AVMEDIA_TYPE_AUDIO] =
		av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO,
			st_index[AVMEDIA_TYPE_AUDIO],
			st_index[AVMEDIA_TYPE_VIDEO],
			NULL, 0);
	if (!video_disable && !subtitle_disable)
		st_index[AVMEDIA_TYPE_SUBTITLE] =
		av_find_best_stream(ic, AVMEDIA_TYPE_SUBTITLE,
			st_index[AVMEDIA_TYPE_SUBTITLE],
			(st_index[AVMEDIA_TYPE_AUDIO] >= 0 ?
				st_index[AVMEDIA_TYPE_AUDIO] :
				st_index[AVMEDIA_TYPE_VIDEO]),
			NULL, 0);

	is->show_mode = show_mode;
	if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
		AVStream* st = ic->streams[st_index[AVMEDIA_TYPE_VIDEO]];
		AVCodecParameters* codecpar = st->codecpar;
		AVRational sar = av_guess_sample_aspect_ratio(ic, st, NULL);
		if (codecpar->width)
			set_default_window_size(codecpar->width, codecpar->height, sar);
	}

	/* open the streams */
	if (st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
		stream_component_open(is, st_index[AVMEDIA_TYPE_AUDIO]);
	}

	ret = -1;
	if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
		ret = stream_component_open(is, st_index[AVMEDIA_TYPE_VIDEO], decoder);
	}
	if (is->show_mode == SHOW_MODE_NONE)
		is->show_mode = ret >= 0 ? SHOW_MODE_VIDEO : SHOW_MODE_RDFT;

	if (st_index[AVMEDIA_TYPE_SUBTITLE] >= 0) {
		stream_component_open(is, st_index[AVMEDIA_TYPE_SUBTITLE]);
	}

	if (is->video_stream < 0 && is->audio_stream < 0) {
		av_log(NULL, AV_LOG_FATAL, "Failed to open file '%s' or configure filtergraph\n",
			is->filename);
		ret = -1;
		goto fail;
	}

	if (infinite_buffer < 0 && is->realtime)
		infinite_buffer = 1;

	for (;;) {
		if (is->abort_request)
			break;
		if (is->paused != is->last_paused) {
			is->last_paused = is->paused;
			if (is->paused)
				is->read_pause_return = av_read_pause(ic);
			else
				av_read_play(ic);
		}
#if CONFIG_RTSP_DEMUXER || CONFIG_MMSH_PROTOCOL
		if (is->paused &&
			(!strcmp(ic->iformat->name, "rtsp") ||
				(ic->pb && !strncmp(input_filename, "mmsh:", 5)))) {
			/* wait 10 ms to avoid trying to get another packet */
			/* XXX: horrible */
			SDL_Delay(10);
			continue;
		}
#endif
		if (is->seek_req) {
			int64_t seek_target = is->seek_pos;
			int64_t seek_min = is->seek_rel > 0 ? seek_target - is->seek_rel + 2 : INT64_MIN;
			int64_t seek_max = is->seek_rel < 0 ? seek_target - is->seek_rel - 2 : INT64_MAX;
			// FIXME the +-2 is due to rounding being not done in the correct direction in generation
			//      of the seek_pos/seek_rel variables

			ret = avformat_seek_file(is->ic, -1, seek_min, seek_target, seek_max, is->seek_flags);
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR,
					"%s: error while seeking\n", is->ic->url);
			}
			else {
				if (is->audio_stream >= 0)
					packet_queue_flush(&is->audioq);
				if (is->subtitle_stream >= 0)
					packet_queue_flush(&is->subtitleq);
				if (is->video_stream >= 0)
					packet_queue_flush(&is->videoq);
				if (is->seek_flags & AVSEEK_FLAG_BYTE) {
					set_clock(&is->extclk, NAN, 0);
				}
				else {
					set_clock(&is->extclk, seek_target / (double)AV_TIME_BASE, 0);
				}
			}
			is->seek_req = 0;
			is->queue_attachments_req = 1;
			is->eof = 0;
			if (is->paused)
				step_to_next_frame(is);
		}
		if (is->queue_attachments_req) {
			if (is->video_st && is->video_st->disposition & AV_DISPOSITION_ATTACHED_PIC) {
				if ((ret = av_packet_ref(pkt, &is->video_st->attached_pic)) < 0)
					goto fail;
				packet_queue_put(&is->videoq, pkt);
				packet_queue_put_nullpacket(&is->videoq, pkt, is->video_stream);
			}
			is->queue_attachments_req = 0;
		}

		/* if the queue are full, no need to read more */
		if (infinite_buffer < 1 &&
			(is->audioq.size + is->videoq.size + is->subtitleq.size > MAX_QUEUE_SIZE
				|| (stream_has_enough_packets(is->audio_st, is->audio_stream, &is->audioq) &&
					stream_has_enough_packets(is->video_st, is->video_stream, &is->videoq) &&
					stream_has_enough_packets(is->subtitle_st, is->subtitle_stream, &is->subtitleq)))) {
			/* wait 10 ms */
			SDL_LockMutex(wait_mutex);
			SDL_CondWaitTimeout(is->continue_read_thread, wait_mutex, 10);
			SDL_UnlockMutex(wait_mutex);
			continue;
		}
		if (!is->paused &&
			(!is->audio_st || (is->auddec.finished == is->audioq.serial && frame_queue_nb_remaining(&is->sampq) == 0)) &&
			(!is->video_st || (is->viddec.finished == is->videoq.serial && frame_queue_nb_remaining(&is->pictq) == 0))) {
			if (ploop != 1 && (!ploop || --ploop)) {
				stream_seek(is, start_time != AV_NOPTS_VALUE ? start_time : 0, 0, 0);
			}
			else if (autoexit) {
				ret = AVERROR_EOF;
				goto fail;
			}
		}
		ret = av_read_frame(ic, pkt);
		if (ret < 0) {
			if ((ret == AVERROR_EOF || avio_feof(ic->pb)) && !is->eof) {
				if (is->video_stream >= 0)
					packet_queue_put_nullpacket(&is->videoq, pkt, is->video_stream);
				if (is->audio_stream >= 0)
					packet_queue_put_nullpacket(&is->audioq, pkt, is->audio_stream);
				if (is->subtitle_stream >= 0)
					packet_queue_put_nullpacket(&is->subtitleq, pkt, is->subtitle_stream);
				is->eof = 1;
			}
			if (ic->pb && ic->pb->error) {
				if (autoexit)
					goto fail;
				else
					break;
			}
			SDL_LockMutex(wait_mutex);
			SDL_CondWaitTimeout(is->continue_read_thread, wait_mutex, 10);
			SDL_UnlockMutex(wait_mutex);
			continue;
		}
		else {
			is->eof = 0;
		}
		/* check if packet is in play range specified by user, then queue, otherwise discard */
		stream_start_time = ic->streams[pkt->stream_index]->start_time;
		pkt_ts = pkt->pts == AV_NOPTS_VALUE ? pkt->dts : pkt->pts;
		pkt_in_play_range = duration == AV_NOPTS_VALUE ||
			(pkt_ts - (stream_start_time != AV_NOPTS_VALUE ? stream_start_time : 0)) *
			av_q2d(ic->streams[pkt->stream_index]->time_base) -
			(double)(start_time != AV_NOPTS_VALUE ? start_time : 0) / 1000000
			<= ((double)duration / 1000000);
		if (pkt->stream_index == is->audio_stream && pkt_in_play_range) {
			packet_queue_put(&is->audioq, pkt);
		}
		else if (pkt->stream_index == is->video_stream && pkt_in_play_range
			&& !(is->video_st->disposition & AV_DISPOSITION_ATTACHED_PIC)) {
			packet_queue_put(&is->videoq, pkt);
		}
		else if (pkt->stream_index == is->subtitle_stream && pkt_in_play_range) {
			packet_queue_put(&is->subtitleq, pkt);
		}
		else {
			av_packet_unref(pkt);
		}
	}

	ret = 0;
fail:
	if (ic && !is->ic)
		avformat_close_input(&ic);

	av_packet_free(&pkt);
	if (ret != 0) {
		SDL_Event event;

		event.type = FF_QUIT_EVENT;
		event.user.data1 = is;
		SDL_PushEvent(&event);
	}
	SDL_DestroyMutex(wait_mutex);
	return 0;
}

static int read_thread1(void* arg)
{
	auto p = (play_ctx*)((VideoState*)arg)->userptr;
	return p->read_thread(arg);
}
VideoState* play_ctx::stream_open(const char* filename,
	const AVInputFormat* iformat)
{
	VideoState* is;

	is = (VideoState*)av_mallocz(sizeof(VideoState));
	if (!is)
		return NULL;
	is->last_video_stream = is->video_stream = -1;
	is->last_audio_stream = is->audio_stream = -1;
	is->last_subtitle_stream = is->subtitle_stream = -1;
	is->filename = av_strdup(filename);
	if (!is->filename)
		goto fail;
	input_filename = filename;
	is->iformat = iformat;
	is->ytop = 0;
	is->xleft = 0;

	/* start video display */
	if (frame_queue_init(&is->pictq, &is->videoq, VIDEO_PICTURE_QUEUE_SIZE, 1) < 0)
		goto fail;
	if (frame_queue_init(&is->subpq, &is->subtitleq, SUBPICTURE_QUEUE_SIZE, 0) < 0)
		goto fail;
	if (frame_queue_init(&is->sampq, &is->audioq, SAMPLE_QUEUE_SIZE, 1) < 0)
		goto fail;

	if (packet_queue_init(&is->videoq) < 0 ||
		packet_queue_init(&is->audioq) < 0 ||
		packet_queue_init(&is->subtitleq) < 0)
		goto fail;

	if (!(is->continue_read_thread = SDL_CreateCond())) {
		av_log(NULL, AV_LOG_FATAL, "SDL_CreateCond(): %s\n", SDL_GetError());
		goto fail;
	}

	init_clock(&is->vidclk, &is->videoq.serial);
	init_clock(&is->audclk, &is->audioq.serial);
	init_clock(&is->extclk, &is->extclk.serial);
	is->audio_clock_serial = -1;
	if (startup_volume < 0)
		av_log(NULL, AV_LOG_WARNING, "-volume=%d < 0, setting to 0\n", startup_volume);
	if (startup_volume > 100)
		av_log(NULL, AV_LOG_WARNING, "-volume=%d > 100, setting to 100\n", startup_volume);
	startup_volume = av_clip(startup_volume, 0, 100);
	startup_volume = av_clip(SDL_MIX_MAXVOLUME * startup_volume / 100, 0, SDL_MIX_MAXVOLUME);
	is->audio_volume = startup_volume;
	is->muted = 0;
	is->av_sync_type = av_sync_type;
	is->userptr = this;
	is->read_tid = SDL_CreateThread(read_thread1, "read_thread", is);
	if (!is->read_tid) {
		av_log(NULL, AV_LOG_FATAL, "SDL_CreateThread(): %s\n", SDL_GetError());
	fail:
		stream_close(is);
		return NULL;
	}
	return is;
}

void play_ctx::stream_cycle_channel(VideoState* is, int codec_type)
{
	AVFormatContext* ic = is->ic;
	int start_index, stream_index;
	int old_index;
	AVStream* st;
	AVProgram* p = NULL;
	int nb_streams = is->ic->nb_streams;

	if (codec_type == AVMEDIA_TYPE_VIDEO) {
		start_index = is->last_video_stream;
		old_index = is->video_stream;
	}
	else if (codec_type == AVMEDIA_TYPE_AUDIO) {
		start_index = is->last_audio_stream;
		old_index = is->audio_stream;
	}
	else {
		start_index = is->last_subtitle_stream;
		old_index = is->subtitle_stream;
	}
	stream_index = start_index;

	if (codec_type != AVMEDIA_TYPE_VIDEO && is->video_stream != -1) {
		p = av_find_program_from_stream(ic, NULL, is->video_stream);
		if (p) {
			nb_streams = p->nb_stream_indexes;
			for (start_index = 0; start_index < nb_streams; start_index++)
				if (p->stream_index[start_index] == stream_index)
					break;
			if (start_index == nb_streams)
				start_index = -1;
			stream_index = start_index;
		}
	}

	for (;;) {
		if (++stream_index >= nb_streams)
		{
			if (codec_type == AVMEDIA_TYPE_SUBTITLE)
			{
				stream_index = -1;
				is->last_subtitle_stream = -1;
				goto the_end;
			}
			if (start_index == -1)
				return;
			stream_index = 0;
		}
		if (stream_index == start_index)
			return;
		st = is->ic->streams[p ? p->stream_index[stream_index] : stream_index];
		if (st->codecpar->codec_type == codec_type) {
			/* check that parameters are OK */
			switch (codec_type) {
			case AVMEDIA_TYPE_AUDIO:
				if (st->codecpar->sample_rate != 0 &&
					st->codecpar->ch_layout.nb_channels != 0)
					goto the_end;
				break;
			case AVMEDIA_TYPE_VIDEO:
			case AVMEDIA_TYPE_SUBTITLE:
				goto the_end;
			default:
				break;
			}
		}
	}
the_end:
	if (p && stream_index != -1)
		stream_index = p->stream_index[stream_index];
	av_log(NULL, AV_LOG_INFO, "Switch %s stream from #%d to #%d\n",
		av_get_media_type_string((AVMediaType)codec_type),
		old_index,
		stream_index);

	stream_component_close(is, old_index);
	stream_component_open(is, stream_index);
}


void play_ctx::toggle_full_screen(VideoState* is)
{
	is_full_screen = !is_full_screen;
	SDL_SetWindowFullscreen(window, is_full_screen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

void play_ctx::toggle_audio_display(VideoState* is)
{
	int next = is->show_mode;
	do {
		next = (next + 1) % SHOW_MODE_NB;
	} while (next != is->show_mode && (next == SHOW_MODE_VIDEO && !is->video_st || next != SHOW_MODE_VIDEO && !is->audio_st));
	if (is->show_mode != next) {
		is->force_refresh = 1;
		is->show_mode = (ShowMode)next;
	}
}

void play_ctx::refresh_loop_wait_event(VideoState* is, SDL_Event* event) {
	double remaining_time = 0.0;
	SDL_PumpEvents();
	while (!SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT)) {
		if (!cursor_hidden && av_gettime_relative() - cursor_last_shown > CURSOR_HIDE_DELAY) {
			SDL_ShowCursor(0);
			cursor_hidden = 1;
		}
		if (remaining_time > 0.0)
		{
			int64_t us = (remaining_time * 1000000.0);
			av_usleep(us);
		}
		remaining_time = REFRESH_RATE;
		if (is->show_mode != SHOW_MODE_NONE && (!is->paused || is->force_refresh))
			video_refresh(is, &remaining_time);
		SDL_PumpEvents();
	}
}
#ifdef AV_TIME_BASE_Q
#undef AV_TIME_BASE_Q
#endif // AV_TIME_BASE_Q
#define AV_TIME_BASE_Q          AVRational{1, AV_TIME_BASE}
static void seek_chapter(VideoState* is, int incr)
{
	int64_t pos = get_master_clock(is) * AV_TIME_BASE;
	int i;

	if (!is->ic->nb_chapters)
		return;

	/* find the current chapter */
	for (i = 0; i < is->ic->nb_chapters; i++) {
		AVChapter* ch = is->ic->chapters[i];
		if (av_compare_ts(pos, AV_TIME_BASE_Q, ch->start, ch->time_base) < 0) {
			i--;
			break;
		}
	}

	i += incr;
	i = FFMAX(i, 0);
	if (i >= is->ic->nb_chapters)
		return;

	av_log(NULL, AV_LOG_VERBOSE, "Seeking to chapter %d.\n", i);
	stream_seek(is, av_rescale_q(is->ic->chapters[i]->start, is->ic->chapters[i]->time_base,
		AV_TIME_BASE_Q), 0, 0);
}

void play_ctx::refresh_wait_event(VideoState* is)
{
	double remaining_time = 0.0;

	while (et) {
		if (!cursor_hidden && av_gettime_relative() - cursor_last_shown > CURSOR_HIDE_DELAY) {
			//SDL_ShowCursor(0);
			cursor_hidden = 1;
		}
		if (remaining_time > 0.0)
		{
			int64_t us = (remaining_time * 1000000.0);
			av_usleep(us);
		}
		remaining_time = REFRESH_RATE;
		if (is->show_mode != SHOW_MODE_NONE && (!is->paused || is->force_refresh))
			video_refresh(is, &remaining_time);
		if (pc && is_ctrl > 0)
		{
			upctrl(pc); is_ctrl = 0; pc = 0;
		}
	}
}
void goto_inc(VideoState* cur_stream, double incr, int seek_by_bytes, double pos = -1)
{
	if (seek_by_bytes) {
		if (pos < 0) {
			if (pos < 0 && cur_stream->video_stream >= 0)
				pos = frame_queue_last_pos(&cur_stream->pictq);
			if (pos < 0 && cur_stream->audio_stream >= 0)
				pos = frame_queue_last_pos(&cur_stream->sampq);
			if (pos < 0)
				pos = avio_tell(cur_stream->ic->pb);
		}
		if (cur_stream->ic->bit_rate)
			incr *= cur_stream->ic->bit_rate / 8.0;
		else
			incr *= 180000.0;
		pos += incr;
		stream_seek(cur_stream, pos, incr, 1);
	}
	else {
		if (pos < 0)
			pos = get_master_clock(cur_stream);
		if (isnan(pos))
			pos = (double)cur_stream->seek_pos / AV_TIME_BASE;
		pos += incr;
		if (cur_stream->ic->start_time != AV_NOPTS_VALUE && pos < cur_stream->ic->start_time / (double)AV_TIME_BASE)
			pos = cur_stream->ic->start_time / (double)AV_TIME_BASE;
		stream_seek(cur_stream, (int64_t)(pos * AV_TIME_BASE), (int64_t)(incr * AV_TIME_BASE), 0);
	}
}
// 设置偏移百分比0-1.0
void seek_pos(VideoState* cur_stream, double xs, int seek_by_bytes)
{
	double frac;
	if (seek_by_bytes || cur_stream->ic->duration <= 0) {
		uint64_t size = avio_size(cur_stream->ic->pb);
		stream_seek(cur_stream, size * xs, 0, 1);
	}
	else {
		int64_t ts;
		int ns, hh, mm, ss;
		int tns, thh, tmm, tss;
		tns = cur_stream->ic->duration / 1000000LL;
		thh = tns / 3600;
		tmm = (tns % 3600) / 60;
		tss = (tns % 60);
		frac = xs;
		ns = frac * tns;
		hh = ns / 3600;
		mm = (ns % 3600) / 60;
		ss = (ns % 60);
		av_log(NULL, AV_LOG_INFO,
			"Seek to %2.0f%% (%2d:%02d:%02d) of total duration (%2d:%02d:%02d)       \n", frac * 100,
			hh, mm, ss, thh, tmm, tss);
		ts = frac * cur_stream->ic->duration;
		if (cur_stream->ic->start_time != AV_NOPTS_VALUE)
			ts += cur_stream->ic->start_time;
		stream_seek(cur_stream, ts, 0, 0);
	}
}
/* handle an event sent by the GUI */
void play_ctx::event_loop(VideoState* cur_stream)
{
	SDL_Event event = {};
	double incr, pos, frac;
	bool xt = true;
	for (; xt;) {
		double x;
		refresh_wait_event(cur_stream);
		xt = tis ? true : false;
	}
}
void play_ctx::upctrl(ctrl_data_t* p)
{
	auto cur_stream = tis;
	do
	{
		if (!p || !tis)break;
		if (p->is_exit)
		{
			do_exit(cur_stream);
			break;
		}
		if (p->full_screen)
		{
			toggle_full_screen(tis);
			tis->force_refresh = 1;
		}
		if (p->pause)
			toggle_pause(cur_stream);
		if (p->mute)
			toggle_mute(cur_stream);
		if (p->volume >= 0)
			set_volume(cur_stream, p->volume);
		if (p->volume_inc != 0)
			update_volume(cur_stream, p->volume_inc > 0 ? 1 : -1, SDL_VOLUME_STEP);
		if (p->next_frame)
		{
			//Step to next frame
			step_to_next_frame(cur_stream);
		}
		if (p->ploop > 0)
		{
			ploop = p->ploop;
			//SDL_CondSignal(cur_stream->continue_read_thread);
		}
		if (p->chapter != 0) {
			int inc = p->chapter > 0 ? 1 : -1;
			if (cur_stream->ic->nb_chapters <= 1) {
				double	incr = 600.0;
				incr *= inc;
				goto_inc(cur_stream, incr, seek_by_bytes, -1);
			}
			seek_chapter(cur_stream, inc);
		}
		if (p->seek_inc != 0)
		{
			goto_inc(cur_stream, p->seek_inc, seek_by_bytes, p->seek_pos);
		}
		if (!(p->seek_scale < 0))
		{
			seek_pos(cur_stream, p->seek_scale, seek_by_bytes);
		}
		//end
	} while (0);
}

void play_ctx::event_loop1(VideoState* cur_stream)
{
	SDL_Event event = {};
	double incr, pos, frac;
	bool xt = true;
	for (; xt;) {
		double x;
		refresh_loop_wait_event(cur_stream, &event);
		switch (event.type) {
		case SDL_KEYDOWN:
			if (exit_on_keydown || event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_q) {
				do_exit(cur_stream);
				xt = false;
				break;
			}
			// If we don't yet have a window, skip all key events, because read_thread might still be initializing...
			if (!cur_stream->width)
				continue;
			switch (event.key.keysym.sym) {
			case SDLK_f:
				toggle_full_screen(cur_stream);
				cur_stream->force_refresh = 1;
				break;
			case SDLK_p:
			case SDLK_SPACE:
				toggle_pause(cur_stream);
				break;
			case SDLK_m:
				toggle_mute(cur_stream);
				break;
			case SDLK_KP_MULTIPLY:
			case SDLK_0:
				update_volume(cur_stream, 1, SDL_VOLUME_STEP);
				break;
			case SDLK_KP_DIVIDE:
			case SDLK_9:
				update_volume(cur_stream, -1, SDL_VOLUME_STEP);
				break;
			case SDLK_s: // S: Step to next frame
				step_to_next_frame(cur_stream);
				break;
			case SDLK_a:
				stream_cycle_channel(cur_stream, AVMEDIA_TYPE_AUDIO);
				break;
			case SDLK_v:
				stream_cycle_channel(cur_stream, AVMEDIA_TYPE_VIDEO);
				break;
			case SDLK_c:
				stream_cycle_channel(cur_stream, AVMEDIA_TYPE_VIDEO);
				stream_cycle_channel(cur_stream, AVMEDIA_TYPE_AUDIO);
				stream_cycle_channel(cur_stream, AVMEDIA_TYPE_SUBTITLE);
				break;
			case SDLK_t:
				stream_cycle_channel(cur_stream, AVMEDIA_TYPE_SUBTITLE);
				break;
			case SDLK_w:
#if CONFIG_AVFILTER
				if (cur_stream->show_mode == SHOW_MODE_VIDEO && cur_stream->vfilter_idx < nb_vfilters - 1) {
					if (++cur_stream->vfilter_idx >= nb_vfilters)
						cur_stream->vfilter_idx = 0;
				}
				else {
					cur_stream->vfilter_idx = 0;
					toggle_audio_display(cur_stream);
				}
#else
				toggle_audio_display(cur_stream);
#endif
				break;
			case SDLK_PAGEUP:
				if (cur_stream->ic->nb_chapters <= 1) {
					incr = 600.0;
					//goto do_seek;
					goto_inc(cur_stream, incr, seek_by_bytes);
				}
				seek_chapter(cur_stream, 1);
				break;
			case SDLK_PAGEDOWN:
				if (cur_stream->ic->nb_chapters <= 1) {
					incr = -600.0;
					goto_inc(cur_stream, incr, seek_by_bytes);
					//goto do_seek;
				}
				seek_chapter(cur_stream, -1);
				break;
			case SDLK_LEFT:
				incr = seek_interval ? -seek_interval : -10.0;
				goto_inc(cur_stream, incr, seek_by_bytes);
				//goto do_seek;
				break;
			case SDLK_RIGHT:
				incr = seek_interval ? seek_interval : 10.0;
				goto_inc(cur_stream, incr, seek_by_bytes);
				//goto do_seek;
				break;
			case SDLK_UP:
				incr = 60.0; goto_inc(cur_stream, incr, seek_by_bytes);
				//goto do_seek;
				break;
			case SDLK_DOWN:
				incr = -60.0;
				//do_seek:
				goto_inc(cur_stream, incr, seek_by_bytes);
				break;
			default:
				break;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (exit_on_mousedown) {
				do_exit(cur_stream);
				xt = false;
				break;
			}
			if (event.button.button == SDL_BUTTON_LEFT) {
				static int64_t last_mouse_left_click = 0;
				if (av_gettime_relative() - last_mouse_left_click <= 500000) {
					toggle_full_screen(cur_stream);
					cur_stream->force_refresh = 1;
					last_mouse_left_click = 0;
				}
				else {
					last_mouse_left_click = av_gettime_relative();
				}
			}
		case SDL_MOUSEMOTION:
			if (cursor_hidden) {
				SDL_ShowCursor(1);
				cursor_hidden = 0;
			}
			cursor_last_shown = av_gettime_relative();
			if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button != SDL_BUTTON_RIGHT)
					break;
				x = event.button.x;
			}
			else {
				if (!(event.motion.state & SDL_BUTTON_RMASK))
					break;
				x = event.motion.x;
			}
			if (seek_by_bytes || cur_stream->ic->duration <= 0) {
				uint64_t size = avio_size(cur_stream->ic->pb);
				stream_seek(cur_stream, size * x / cur_stream->width, 0, 1);
			}
			else {
				int64_t ts;
				int ns, hh, mm, ss;
				int tns, thh, tmm, tss;
				tns = cur_stream->ic->duration / 1000000LL;
				thh = tns / 3600;
				tmm = (tns % 3600) / 60;
				tss = (tns % 60);
				frac = x / cur_stream->width;
				ns = frac * tns;
				hh = ns / 3600;
				mm = (ns % 3600) / 60;
				ss = (ns % 60);
				av_log(NULL, AV_LOG_INFO,
					"Seek to %2.0f%% (%2d:%02d:%02d) of total duration (%2d:%02d:%02d)       \n", frac * 100,
					hh, mm, ss, thh, tmm, tss);
				ts = frac * cur_stream->ic->duration;
				if (cur_stream->ic->start_time != AV_NOPTS_VALUE)
					ts += cur_stream->ic->start_time;
				stream_seek(cur_stream, ts, 0, 0);
			}
			break;
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				screen_width = cur_stream->width = event.window.data1;
				screen_height = cur_stream->height = event.window.data2;
				if (cur_stream->vis_texture) {
					SDL_DestroyTexture(cur_stream->vis_texture);
					cur_stream->vis_texture = NULL;
				}
			case SDL_WINDOWEVENT_EXPOSED:
				cur_stream->force_refresh = 1;
			}
			break;
		case SDL_QUIT:
		case FF_QUIT_EVENT:
			do_exit(cur_stream);
			xt = false;
			break;
		default:
			break;
		}
	}
}

int play_ctx::opt_width(void* optctx, const char* opt, const char* arg)
{
	screen_width = parse_number_or_die(opt, arg, OPT_INT64, 1, INT_MAX);
	return 0;
}

int play_ctx::opt_height(void* optctx, const char* opt, const char* arg)
{
	screen_height = parse_number_or_die(opt, arg, OPT_INT64, 1, INT_MAX);
	return 0;
}

int play_ctx::opt_format(void* optctx, const char* opt, const char* arg)
{
	file_iformat = av_find_input_format(arg);
	if (!file_iformat) {
		av_log(NULL, AV_LOG_FATAL, "Unknown input format: %s\n", arg);
		return AVERROR(EINVAL);
	}
	return 0;
}

int play_ctx::opt_sync(void* optctx, const char* opt, const char* arg)
{
	if (!strcmp(arg, "audio"))
		av_sync_type = AV_SYNC_AUDIO_MASTER;
	else if (!strcmp(arg, "video"))
		av_sync_type = AV_SYNC_VIDEO_MASTER;
	else if (!strcmp(arg, "ext"))
		av_sync_type = AV_SYNC_EXTERNAL_CLOCK;
	else {
		av_log(NULL, AV_LOG_ERROR, "Unknown value for %s: %s\n", opt, arg);
		exit(1);
	}
	return 0;
}

int play_ctx::opt_seek(void* optctx, const char* opt, const char* arg)
{
	start_time = parse_time_or_die(opt, arg, 1);
	return 0;
}

int play_ctx::opt_duration(void* optctx, const char* opt, const char* arg)
{
	duration = parse_time_or_die(opt, arg, 1);
	return 0;
}

int play_ctx::opt_show_mode(void* optctx, const char* opt, const char* arg)
{
	show_mode = (ShowMode)(!strcmp(arg, "video") ? SHOW_MODE_VIDEO :
		!strcmp(arg, "waves") ? SHOW_MODE_WAVES :
		!strcmp(arg, "rdft") ? SHOW_MODE_RDFT :
		parse_number_or_die(opt, arg, OPT_INT, 0, SHOW_MODE_NB - 1));
	return 0;
}

void play_ctx::opt_input_file(void* optctx, const char* filename)
{
	if (input_filename) {
		av_log(NULL, AV_LOG_FATAL,
			"Argument '%s' provided as input filename, but '%s' was already specified.\n",
			filename, input_filename);
		exit(1);
	}
	if (!strcmp(filename, "-"))
		filename = "pipe:";
	input_filename = filename;
}

int play_ctx::opt_codec(void* optctx, const char* opt, const char* arg)
{
	const char* spec = strchr(opt, ':');
	if (!spec) {
		av_log(NULL, AV_LOG_ERROR,
			"No media specifier was specified in '%s' in option '%s'\n",
			arg, opt);
		return AVERROR(EINVAL);
	}
	spec++;
	switch (spec[0]) {
	case 'a':    audio_codec_name = arg; break;
	case 's': subtitle_codec_name = arg; break;
	case 'v':    video_codec_name = arg; break;
	default:
		av_log(NULL, AV_LOG_ERROR,
			"Invalid media specifier '%s' in option '%s'\n", spec, opt);
		return AVERROR(EINVAL);
	}
	return 0;
}
#if 1

int opt_add_vfilter1(void* optctx, const char* opt, const char* arg) {
	auto p = (play_ctx*)optctx;
	return p->opt_add_vfilter(optctx, opt, arg);
}

int opt_width1(void* optctx, const char* opt, const char* arg) {
	auto p = (play_ctx*)optctx;
	return p->opt_width(optctx, opt, arg);
}
int opt_height1(void* optctx, const char* opt, const char* arg) {
	auto p = (play_ctx*)optctx;
	return p->opt_height(optctx, opt, arg);
}
int opt_format1(void* optctx, const char* opt, const char* arg) {
	auto p = (play_ctx*)optctx;
	return p->opt_format(optctx, opt, arg);
}
int opt_sync1(void* optctx, const char* opt, const char* arg) {
	auto p = (play_ctx*)optctx;
	return p->opt_sync(optctx, opt, arg);
}
int opt_seek1(void* optctx, const char* opt, const char* arg) {
	auto p = (play_ctx*)optctx;
	return p->opt_seek(optctx, opt, arg);
}
int opt_duration1(void* optctx, const char* opt, const char* arg) {
	auto p = (play_ctx*)optctx;
	return p->opt_duration(optctx, opt, arg);
}
int opt_show_mode1(void* optctx, const char* opt, const char* arg) {
	auto p = (play_ctx*)optctx;
	return p->opt_show_mode(optctx, opt, arg);
}
void opt_input_file1(void* optctx, const char* filename) {
	auto p = (play_ctx*)optctx;
	return p->opt_input_file(optctx, filename);
}
int opt_codec1(void* optctx, const char* opt, const char* arg) {
	auto p = (play_ctx*)optctx;
	return p->opt_codec(optctx, opt, arg);
}
#if 1

enum show_muxdemuxers {
	SHOW_DEFAULT,
	SHOW_DEMUXERS,
	SHOW_MUXERS,
};

static FILE* report_file;
static int report_file_level = AV_LOG_DEBUG;

int show_license(void* optctx, const char* opt, const char* arg)
{
#if CONFIG_NONFREE
	printf(
		"This version of %s has nonfree parts compiled in.\n"
		"Therefore it is not legally redistributable.\n",
		program_name);
#elif CONFIG_GPLV3
	printf(
		"%s is free software; you can redistribute it and/or modify\n"
		"it under the terms of the GNU General Public License as published by\n"
		"the Free Software Foundation; either version 3 of the License, or\n"
		"(at your option) any later version.\n"
		"\n"
		"%s is distributed in the hope that it will be useful,\n"
		"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
		"GNU General Public License for more details.\n"
		"\n"
		"You should have received a copy of the GNU General Public License\n"
		"along with %s.  If not, see <http://www.gnu.org/licenses/>.\n",
		program_name, program_name, program_name);
#elif CONFIG_GPL
	printf(
		"%s is free software; you can redistribute it and/or modify\n"
		"it under the terms of the GNU General Public License as published by\n"
		"the Free Software Foundation; either version 2 of the License, or\n"
		"(at your option) any later version.\n"
		"\n"
		"%s is distributed in the hope that it will be useful,\n"
		"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
		"GNU General Public License for more details.\n"
		"\n"
		"You should have received a copy of the GNU General Public License\n"
		"along with %s; if not, write to the Free Software\n"
		"Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA\n",
		program_name, program_name, program_name);
#elif CONFIG_LGPLV3
	printf(
		"%s is free software; you can redistribute it and/or modify\n"
		"it under the terms of the GNU Lesser General Public License as published by\n"
		"the Free Software Foundation; either version 3 of the License, or\n"
		"(at your option) any later version.\n"
		"\n"
		"%s is distributed in the hope that it will be useful,\n"
		"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
		"GNU Lesser General Public License for more details.\n"
		"\n"
		"You should have received a copy of the GNU Lesser General Public License\n"
		"along with %s.  If not, see <http://www.gnu.org/licenses/>.\n",
		program_name, program_name, program_name);
#else
	printf(
		"%s is free software; you can redistribute it and/or\n"
		"modify it under the terms of the GNU Lesser General Public\n"
		"License as published by the Free Software Foundation; either\n"
		"version 2.1 of the License, or (at your option) any later version.\n"
		"\n"
		"%s is distributed in the hope that it will be useful,\n"
		"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
		"Lesser General Public License for more details.\n"
		"\n"
		"You should have received a copy of the GNU Lesser General Public\n"
		"License along with %s; if not, write to the Free Software\n"
		"Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA\n",
		program_name, program_name, program_name);
#endif

	return 0;
}

static int warned_cfg = 0;

#define INDENT        1
#define SHOW_VERSION  2
#define SHOW_CONFIG   4
#define SHOW_COPYRIGHT 8

#define PRINT_LIB_INFO(libname, LIBNAME, flags, level)                  \
    if (CONFIG_##LIBNAME) {                                             \
        const char *indent = flags & INDENT? "  " : "";                 \
        if (flags & SHOW_VERSION) {                                     \
            unsigned int version = libname##_version();                 \
            av_log(NULL, level,                                         \
                   "%slib%-11s %2d.%3d.%3d / %2d.%3d.%3d\n",            \
                   indent, #libname,                                    \
                   LIB##LIBNAME##_VERSION_MAJOR,                        \
                   LIB##LIBNAME##_VERSION_MINOR,                        \
                   LIB##LIBNAME##_VERSION_MICRO,                        \
                   AV_VERSION_MAJOR(version), AV_VERSION_MINOR(version),\
                   AV_VERSION_MICRO(version));                          \
        }                                                               \
        if (flags & SHOW_CONFIG) {                                      \
            const char *cfg = libname##_configuration();                \
            if (strcmp(FFMPEG_CONFIGURATION, cfg)) {                    \
                if (!warned_cfg) {                                      \
                    av_log(NULL, level,                                 \
                            "%sWARNING: library configuration mismatch\n", \
                            indent);                                    \
                    warned_cfg = 1;                                     \
                }                                                       \
                av_log(NULL, level, "%s%-11s configuration: %s\n",      \
                        indent, #libname, cfg);                         \
            }                                                           \
        }                                                               \
    }                                                                   \

static void print_all_libs_info(int flags, int level)
{
	//PRINT_LIB_INFO(avutil, AVUTIL, flags, level);
	//PRINT_LIB_INFO(avcodec, AVCODEC, flags, level);
	//PRINT_LIB_INFO(avformat, AVFORMAT, flags, level);
	//PRINT_LIB_INFO(avdevice, AVDEVICE, flags, level);
	//PRINT_LIB_INFO(avfilter, AVFILTER, flags, level);
	//PRINT_LIB_INFO(swscale, SWSCALE, flags, level);
	//PRINT_LIB_INFO(swresample, SWRESAMPLE, flags, level);
	//PRINT_LIB_INFO(postproc, POSTPROC, flags, level);
}

static void print_program_info(int flags, int level)
{
	const char* indent = flags & INDENT ? "  " : "";
#if 0
	av_log(NULL, level, "%s version " FFMPEG_VERSION, program_name);
	if (flags & SHOW_COPYRIGHT)
		av_log(NULL, level, " Copyright (c) %d-%d the FFmpeg developers",
			program_birth_year, CONFIG_THIS_YEAR);
	av_log(NULL, level, "\n");
	av_log(NULL, level, "%sbuilt with %s\n", indent, CC_IDENT);

	av_log(NULL, level, "%sconfiguration: " FFMPEG_CONFIGURATION "\n", indent);
#endif
}

static void print_buildconf(int flags, int level)
{
#if 0
	const char* indent = flags & INDENT ? "  " : "";
	char str[] = { FFMPEG_CONFIGURATION };
	char* conflist, * remove_tilde, * splitconf;

	// Change all the ' --' strings to '~--' so that
	// they can be identified as tokens.
	while ((conflist = strstr(str, " --")) != NULL) {
		conflist[0] = '~';
	}

	// Compensate for the weirdness this would cause
	// when passing 'pkg-config --static'.
	while ((remove_tilde = strstr(str, "pkg-config~")) != NULL) {
		remove_tilde[sizeof("pkg-config~") - 2] = ' ';
	}

	splitconf = strtok(str, "~");
	av_log(NULL, level, "\n%sconfiguration:\n", indent);
	while (splitconf != NULL) {
		av_log(NULL, level, "%s%s%s\n", indent, indent, splitconf);
		splitconf = strtok(NULL, "~");
	}
#endif
}

void show_banner(int argc, char** argv, const OptionDef* options)
{
	int idx = locate_option(argc, argv, options, "version");
	if (hide_banner || idx)
		return;

	print_program_info(INDENT | SHOW_COPYRIGHT, AV_LOG_INFO);
	print_all_libs_info(INDENT | SHOW_CONFIG, AV_LOG_INFO);
	print_all_libs_info(INDENT | SHOW_VERSION, AV_LOG_INFO);
}

int show_version(void* optctx, const char* opt, const char* arg)
{
	av_log_set_callback(log_callback_help);
	print_program_info(SHOW_COPYRIGHT, AV_LOG_INFO);
	print_all_libs_info(SHOW_VERSION, AV_LOG_INFO);

	return 0;
}

int show_buildconf(void* optctx, const char* opt, const char* arg)
{
	av_log_set_callback(log_callback_help);
	print_buildconf(INDENT | 0, AV_LOG_INFO);

	return 0;
}

#define PRINT_CODEC_SUPPORTED(codec, field, type, list_name, term, get_name) \
    if (codec->field) {                                                      \
        const type *p = codec->field;                                        \
                                                                             \
        printf("    Supported " list_name ":");                              \
        while (*p != term) {                                                 \
            get_name(*p);                                                    \
            printf(" %s", name);                                             \
            p++;                                                             \
        }                                                                    \
        printf("\n");                                                        \
    }                                                                        \

static void print_codec(const AVCodec* c)
{
	int encoder = av_codec_is_encoder(c);

	printf("%s %s [%s]:\n", encoder ? "Encoder" : "Decoder", c->name,
		c->long_name ? c->long_name : "");

	printf("    General capabilities: ");
	if (c->capabilities & AV_CODEC_CAP_DRAW_HORIZ_BAND)
		printf("horizband ");
	if (c->capabilities & AV_CODEC_CAP_DR1)
		printf("dr1 ");
	if (c->capabilities & AV_CODEC_CAP_DELAY)
		printf("delay ");
	if (c->capabilities & AV_CODEC_CAP_SMALL_LAST_FRAME)
		printf("small ");
	if (c->capabilities & AV_CODEC_CAP_SUBFRAMES)
		printf("subframes ");
	if (c->capabilities & AV_CODEC_CAP_EXPERIMENTAL)
		printf("exp ");
	if (c->capabilities & AV_CODEC_CAP_CHANNEL_CONF)
		printf("chconf ");
	if (c->capabilities & AV_CODEC_CAP_PARAM_CHANGE)
		printf("paramchange ");
	if (c->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
		printf("variable ");
	if (c->capabilities & (AV_CODEC_CAP_FRAME_THREADS |
		AV_CODEC_CAP_SLICE_THREADS |
		AV_CODEC_CAP_OTHER_THREADS))
		printf("threads ");
	if (c->capabilities & AV_CODEC_CAP_AVOID_PROBING)
		printf("avoidprobe ");
	if (c->capabilities & AV_CODEC_CAP_HARDWARE)
		printf("hardware ");
	if (c->capabilities & AV_CODEC_CAP_HYBRID)
		printf("hybrid ");
	if (!c->capabilities)
		printf("none");
	printf("\n");

	if (c->type == AVMEDIA_TYPE_VIDEO ||
		c->type == AVMEDIA_TYPE_AUDIO) {
		printf("    Threading capabilities: ");
		switch (c->capabilities & (AV_CODEC_CAP_FRAME_THREADS |
			AV_CODEC_CAP_SLICE_THREADS |
			AV_CODEC_CAP_OTHER_THREADS)) {
			case AV_CODEC_CAP_FRAME_THREADS |
			AV_CODEC_CAP_SLICE_THREADS: printf("frame and slice"); break;
			case AV_CODEC_CAP_FRAME_THREADS: printf("frame");           break;
			case AV_CODEC_CAP_SLICE_THREADS: printf("slice");           break;
			case AV_CODEC_CAP_OTHER_THREADS: printf("other");           break;
			default:                         printf("none");            break;
		}
		printf("\n");
	}

	if (avcodec_get_hw_config(c, 0)) {
		printf("    Supported hardware devices: ");
		for (int i = 0;; i++) {
			const AVCodecHWConfig* config = avcodec_get_hw_config(c, i);
			const char* name;
			if (!config)
				break;
			name = av_hwdevice_get_type_name(config->device_type);
			if (name)
				printf("%s ", name);
		}
		printf("\n");
	}

	if (c->supported_framerates) {
		const AVRational* fps = c->supported_framerates;

		printf("    Supported framerates:");
		while (fps->num) {
			printf(" %d/%d", fps->num, fps->den);
			fps++;
		}
		printf("\n");
	}
	PRINT_CODEC_SUPPORTED(c, pix_fmts, enum AVPixelFormat, "pixel formats",
		AV_PIX_FMT_NONE, GET_PIX_FMT_NAME);
	PRINT_CODEC_SUPPORTED(c, supported_samplerates, int, "sample rates", 0,
		GET_SAMPLE_RATE_NAME);
	PRINT_CODEC_SUPPORTED(c, sample_fmts, enum AVSampleFormat, "sample formats",
		AV_SAMPLE_FMT_NONE, GET_SAMPLE_FMT_NAME);

	if (c->ch_layouts) {
		const AVChannelLayout* p = c->ch_layouts;

		printf("    Supported channel layouts:");
		while (p->nb_channels) {
			char name[128];
			av_channel_layout_describe(p, name, sizeof(name));
			printf(" %s", name);
			p++;
		}
		printf("\n");
	}

	if (c->priv_class) {
		show_help_children(c->priv_class,
			AV_OPT_FLAG_ENCODING_PARAM |
			AV_OPT_FLAG_DECODING_PARAM);
	}
}

static const AVCodec* next_codec_for_id(enum AVCodecID id, void** iter,
	int encoder)
{
	const AVCodec* c;
	while ((c = av_codec_iterate(iter))) {
		if (c->id == id &&
			(encoder ? av_codec_is_encoder(c) : av_codec_is_decoder(c)))
			return c;
	}
	return NULL;
}

static void show_help_codec(const char* name, int encoder)
{
	const AVCodecDescriptor* desc;
	const AVCodec* codec;

	if (!name) {
		av_log(NULL, AV_LOG_ERROR, "No codec name specified.\n");
		return;
	}

	codec = encoder ? avcodec_find_encoder_by_name(name) :
		avcodec_find_decoder_by_name(name);

	if (codec)
		print_codec(codec);
	else if ((desc = avcodec_descriptor_get_by_name(name))) {
		void* iter = NULL;
		int printed = 0;

		while ((codec = next_codec_for_id(desc->id, &iter, encoder))) {
			printed = 1;
			print_codec(codec);
		}

		if (!printed) {
			av_log(NULL, AV_LOG_ERROR, "Codec '%s' is known to FFmpeg, "
				"but no %s for it are available. FFmpeg might need to be "
				"recompiled with additional external libraries.\n",
				name, encoder ? "encoders" : "decoders");
		}
	}
	else {
		av_log(NULL, AV_LOG_ERROR, "Codec '%s' is not recognized by FFmpeg.\n",
			name);
	}
}

static void show_help_demuxer(const char* name)
{
	const AVInputFormat* fmt = av_find_input_format(name);

	if (!fmt) {
		av_log(NULL, AV_LOG_ERROR, "Unknown format '%s'.\n", name);
		return;
	}

	printf("Demuxer %s [%s]:\n", fmt->name, fmt->long_name);

	if (fmt->extensions)
		printf("    Common extensions: %s.\n", fmt->extensions);

	if (fmt->priv_class)
		show_help_children(fmt->priv_class, AV_OPT_FLAG_DECODING_PARAM);
}

static void show_help_protocol(const char* name)
{
	const AVClass* proto_class;

	if (!name) {
		av_log(NULL, AV_LOG_ERROR, "No protocol name specified.\n");
		return;
	}

	proto_class = avio_protocol_get_class(name);
	if (!proto_class) {
		av_log(NULL, AV_LOG_ERROR, "Unknown protocol '%s'.\n", name);
		return;
	}

	show_help_children(proto_class, AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_ENCODING_PARAM);
}

static void show_help_muxer(const char* name)
{
	const AVCodecDescriptor* desc;
	const AVOutputFormat* fmt = av_guess_format(name, NULL, NULL);

	if (!fmt) {
		av_log(NULL, AV_LOG_ERROR, "Unknown format '%s'.\n", name);
		return;
	}

	printf("Muxer %s [%s]:\n", fmt->name, fmt->long_name);

	if (fmt->extensions)
		printf("    Common extensions: %s.\n", fmt->extensions);
	if (fmt->mime_type)
		printf("    Mime type: %s.\n", fmt->mime_type);
	if (fmt->video_codec != AV_CODEC_ID_NONE &&
		(desc = avcodec_descriptor_get(fmt->video_codec))) {
		printf("    Default video codec: %s.\n", desc->name);
	}
	if (fmt->audio_codec != AV_CODEC_ID_NONE &&
		(desc = avcodec_descriptor_get(fmt->audio_codec))) {
		printf("    Default audio codec: %s.\n", desc->name);
	}
	if (fmt->subtitle_codec != AV_CODEC_ID_NONE &&
		(desc = avcodec_descriptor_get(fmt->subtitle_codec))) {
		printf("    Default subtitle codec: %s.\n", desc->name);
	}

	if (fmt->priv_class)
		show_help_children(fmt->priv_class, AV_OPT_FLAG_ENCODING_PARAM);
}

#if CONFIG_AVFILTER
static void show_help_filter(const char* name)
{
#if CONFIG_AVFILTER
	const AVFilter* f = avfilter_get_by_name(name);
	int i, count;

	if (!name) {
		av_log(NULL, AV_LOG_ERROR, "No filter name specified.\n");
		return;
	}
	else if (!f) {
		av_log(NULL, AV_LOG_ERROR, "Unknown filter '%s'.\n", name);
		return;
	}

	printf("Filter %s\n", f->name);
	if (f->description)
		printf("  %s\n", f->description);

	if (f->flags & AVFILTER_FLAG_SLICE_THREADS)
		printf("    slice threading supported\n");

	printf("    Inputs:\n");
	count = avfilter_filter_pad_count(f, 0);
	for (i = 0; i < count; i++) {
		printf("       #%d: %s (%s)\n", i, avfilter_pad_get_name(f->inputs, i),
			av_get_media_type_string(avfilter_pad_get_type(f->inputs, i)));
	}
	if (f->flags & AVFILTER_FLAG_DYNAMIC_INPUTS)
		printf("        dynamic (depending on the options)\n");
	else if (!count)
		printf("        none (source filter)\n");

	printf("    Outputs:\n");
	count = avfilter_filter_pad_count(f, 1);
	for (i = 0; i < count; i++) {
		printf("       #%d: %s (%s)\n", i, avfilter_pad_get_name(f->outputs, i),
			av_get_media_type_string(avfilter_pad_get_type(f->outputs, i)));
	}
	if (f->flags & AVFILTER_FLAG_DYNAMIC_OUTPUTS)
		printf("        dynamic (depending on the options)\n");
	else if (!count)
		printf("        none (sink filter)\n");

	if (f->priv_class)
		show_help_children(f->priv_class, AV_OPT_FLAG_VIDEO_PARAM | AV_OPT_FLAG_FILTERING_PARAM |
			AV_OPT_FLAG_AUDIO_PARAM);
	if (f->flags & AVFILTER_FLAG_SUPPORT_TIMELINE)
		printf("This filter has support for timeline through the 'enable' option.\n");
#else
	av_log(NULL, AV_LOG_ERROR, "Build without libavfilter; "
		"can not to satisfy request\n");
#endif
}
#endif

static void show_help_bsf(const char* name)
{
	const AVBitStreamFilter* bsf = av_bsf_get_by_name(name);

	if (!name) {
		av_log(NULL, AV_LOG_ERROR, "No bitstream filter name specified.\n");
		return;
	}
	else if (!bsf) {
		av_log(NULL, AV_LOG_ERROR, "Unknown bit stream filter '%s'.\n", name);
		return;
	}

	printf("Bit stream filter %s\n", bsf->name);
	PRINT_CODEC_SUPPORTED(bsf, codec_ids, enum AVCodecID, "codecs",
		AV_CODEC_ID_NONE, GET_CODEC_NAME);
	if (bsf->priv_class)
		show_help_children(bsf->priv_class, AV_OPT_FLAG_BSF_PARAM);
}

int show_help(void* optctx, const char* opt, const char* arg)
{
	char* topic, * par;
	av_log_set_callback(log_callback_help);

	topic = av_strdup(arg ? arg : "");
	if (!topic)
		return AVERROR(ENOMEM);
	par = strchr(topic, '=');
	if (par)
		*par++ = 0;

	if (!*topic) {
		show_help_default(topic, par);
	}
	else if (!strcmp(topic, "decoder")) {
		show_help_codec(par, 0);
	}
	else if (!strcmp(topic, "encoder")) {
		show_help_codec(par, 1);
	}
	else if (!strcmp(topic, "demuxer")) {
		show_help_demuxer(par);
	}
	else if (!strcmp(topic, "muxer")) {
		show_help_muxer(par);
	}
	else if (!strcmp(topic, "protocol")) {
		show_help_protocol(par);
#if CONFIG_AVFILTER
	}
	else if (!strcmp(topic, "filter")) {
		show_help_filter(par);
#endif
	}
	else if (!strcmp(topic, "bsf")) {
		show_help_bsf(par);
	}
	else {
		show_help_default(topic, par);
	}

	av_freep(&topic);
	return 0;
}

static void print_codecs_for_id(enum AVCodecID id, int encoder)
{
	void* iter = NULL;
	const AVCodec* codec;

	printf(" (%s: ", encoder ? "encoders" : "decoders");

	while ((codec = next_codec_for_id(id, &iter, encoder)))
		printf("%s ", codec->name);

	printf(")");
}

static int compare_codec_desc(const void* a, const void* b)
{
	const AVCodecDescriptor* const* da = (AVCodecDescriptor**)a;
	const AVCodecDescriptor* const* db = (AVCodecDescriptor**)b;

	return (*da)->type != (*db)->type ? FFDIFFSIGN((*da)->type, (*db)->type) :
		strcmp((*da)->name, (*db)->name);
}

static unsigned get_codecs_sorted(const AVCodecDescriptor*** rcodecs)
{
	const AVCodecDescriptor* desc = NULL;
	const AVCodecDescriptor** codecs;
	unsigned nb_codecs = 0, i = 0;

	while ((desc = avcodec_descriptor_next(desc)))
		nb_codecs++;
	if (!(codecs = (const AVCodecDescriptor**)av_calloc(nb_codecs, sizeof(*codecs))))
	{
		return 0;
		//report_and_exit(AVERROR(ENOMEM));
	}
	desc = NULL;
	while ((desc = avcodec_descriptor_next(desc)))
		codecs[i++] = desc;
	av_assert0(i == nb_codecs);
	qsort(codecs, nb_codecs, sizeof(*codecs), compare_codec_desc);
	*rcodecs = codecs;
	return nb_codecs;
}

static char get_media_type_char(enum AVMediaType type)
{
	switch (type) {
	case AVMEDIA_TYPE_VIDEO:    return 'V';
	case AVMEDIA_TYPE_AUDIO:    return 'A';
	case AVMEDIA_TYPE_DATA:     return 'D';
	case AVMEDIA_TYPE_SUBTITLE: return 'S';
	case AVMEDIA_TYPE_ATTACHMENT:return 'T';
	default:                    return '?';
	}
}

int show_codecs(void* optctx, const char* opt, const char* arg)
{
	const AVCodecDescriptor** codecs;
	unsigned i, nb_codecs = get_codecs_sorted(&codecs);

	printf("Codecs:\n"
		" D..... = Decoding supported\n"
		" .E.... = Encoding supported\n"
		" ..V... = Video codec\n"
		" ..A... = Audio codec\n"
		" ..S... = Subtitle codec\n"
		" ..D... = Data codec\n"
		" ..T... = Attachment codec\n"
		" ...I.. = Intra frame-only codec\n"
		" ....L. = Lossy compression\n"
		" .....S = Lossless compression\n"
		" -------\n");
	for (i = 0; i < nb_codecs; i++) {
		const AVCodecDescriptor* desc = codecs[i];
		const AVCodec* codec;
		void* iter = NULL;

		if (strstr(desc->name, "_deprecated"))
			continue;

		printf(" ");
		printf(avcodec_find_decoder(desc->id) ? "D" : ".");
		printf(avcodec_find_encoder(desc->id) ? "E" : ".");

		printf("%c", get_media_type_char(desc->type));
		printf((desc->props & AV_CODEC_PROP_INTRA_ONLY) ? "I" : ".");
		printf((desc->props & AV_CODEC_PROP_LOSSY) ? "L" : ".");
		printf((desc->props & AV_CODEC_PROP_LOSSLESS) ? "S" : ".");

		printf(" %-20s %s", desc->name, desc->long_name ? desc->long_name : "");

		/* print decoders/encoders when there's more than one or their
		 * names are different from codec name */
		while ((codec = next_codec_for_id(desc->id, &iter, 0))) {
			if (strcmp(codec->name, desc->name)) {
				print_codecs_for_id(desc->id, 0);
				break;
			}
		}
		iter = NULL;
		while ((codec = next_codec_for_id(desc->id, &iter, 1))) {
			if (strcmp(codec->name, desc->name)) {
				print_codecs_for_id(desc->id, 1);
				break;
			}
		}

		printf("\n");
	}
	av_free(codecs);
	return 0;
}

static void print_codecs(int encoder)
{
	const AVCodecDescriptor** codecs;
	unsigned i, nb_codecs = get_codecs_sorted(&codecs);

	printf("%s:\n"
		" V..... = Video\n"
		" A..... = Audio\n"
		" S..... = Subtitle\n"
		" .F.... = Frame-level multithreading\n"
		" ..S... = Slice-level multithreading\n"
		" ...X.. = Codec is experimental\n"
		" ....B. = Supports draw_horiz_band\n"
		" .....D = Supports direct rendering method 1\n"
		" ------\n",
		encoder ? "Encoders" : "Decoders");
	for (i = 0; i < nb_codecs; i++) {
		const AVCodecDescriptor* desc = codecs[i];
		const AVCodec* codec;
		void* iter = NULL;

		while ((codec = next_codec_for_id(desc->id, &iter, encoder))) {
			printf(" %c", get_media_type_char(desc->type));
			printf((codec->capabilities & AV_CODEC_CAP_FRAME_THREADS) ? "F" : ".");
			printf((codec->capabilities & AV_CODEC_CAP_SLICE_THREADS) ? "S" : ".");
			printf((codec->capabilities & AV_CODEC_CAP_EXPERIMENTAL) ? "X" : ".");
			printf((codec->capabilities & AV_CODEC_CAP_DRAW_HORIZ_BAND) ? "B" : ".");
			printf((codec->capabilities & AV_CODEC_CAP_DR1) ? "D" : ".");

			printf(" %-20s %s", codec->name, codec->long_name ? codec->long_name : "");
			if (strcmp(codec->name, desc->name))
				printf(" (codec %s)", desc->name);

			printf("\n");
		}
	}
	av_free(codecs);
}

int show_decoders(void* optctx, const char* opt, const char* arg)
{
	print_codecs(0);
	return 0;
}

int show_encoders(void* optctx, const char* opt, const char* arg)
{
	print_codecs(1);
	return 0;
}

int show_bsfs(void* optctx, const char* opt, const char* arg)
{
	const AVBitStreamFilter* bsf = NULL;
	void* opaque = NULL;

	printf("Bitstream filters:\n");
	while ((bsf = av_bsf_iterate(&opaque)))
		printf("%s\n", bsf->name);
	printf("\n");
	return 0;
}

int show_filters(void* optctx, const char* opt, const char* arg)
{
#if CONFIG_AVFILTER
	const AVFilter* filter = NULL;
	char descr[64], * descr_cur;
	void* opaque = NULL;
	int i, j;
	const AVFilterPad* pad;

	printf("Filters:\n"
		"  T.. = Timeline support\n"
		"  .S. = Slice threading\n"
		"  ..C = Command support\n"
		"  A = Audio input/output\n"
		"  V = Video input/output\n"
		"  N = Dynamic number and/or type of input/output\n"
		"  | = Source or sink filter\n");
	while ((filter = av_filter_iterate(&opaque))) {
		descr_cur = descr;
		for (i = 0; i < 2; i++) {
			unsigned nb_pads;
			if (i) {
				*(descr_cur++) = '-';
				*(descr_cur++) = '>';
			}
			pad = i ? filter->outputs : filter->inputs;
			nb_pads = avfilter_filter_pad_count(filter, i);
			for (j = 0; j < nb_pads; j++) {
				if (descr_cur >= descr + sizeof(descr) - 4)
					break;
				*(descr_cur++) = get_media_type_char(avfilter_pad_get_type(pad, j));
			}
			if (!j)
				*(descr_cur++) = ((!i && (filter->flags & AVFILTER_FLAG_DYNAMIC_INPUTS)) ||
					(i && (filter->flags & AVFILTER_FLAG_DYNAMIC_OUTPUTS))) ? 'N' : '|';
		}
		*descr_cur = 0;
		printf(" %c%c%c %-17s %-10s %s\n",
			filter->flags & AVFILTER_FLAG_SUPPORT_TIMELINE ? 'T' : '.',
			filter->flags & AVFILTER_FLAG_SLICE_THREADS ? 'S' : '.',
			filter->process_command ? 'C' : '.',
			filter->name, descr, filter->description);
	}
#else
	printf("No filters available: libavfilter disabled\n");
#endif
	return 0;
}

static int is_device(const AVClass* avclass)
{
	if (!avclass)
		return 0;
	return AV_IS_INPUT_DEVICE(avclass->category) || AV_IS_OUTPUT_DEVICE(avclass->category);
}

static int show_formats_devices(void* optctx, const char* opt, const char* arg, int device_only, int muxdemuxers)
{
	void* ifmt_opaque = NULL;
	const AVInputFormat* ifmt = NULL;
	void* ofmt_opaque = NULL;
	const AVOutputFormat* ofmt = NULL;
	const char* last_name;
	int is_dev;

	printf("%s\n"
		" D. = Demuxing supported\n"
		" .E = Muxing supported\n"
		" --\n", device_only ? "Devices:" : "File formats:");
	last_name = "000";
	for (;;) {
		int decode = 0;
		int encode = 0;
		const char* name = NULL;
		const char* long_name = NULL;

		if (muxdemuxers != SHOW_DEMUXERS) {
			ofmt_opaque = NULL;
			while ((ofmt = av_muxer_iterate(&ofmt_opaque))) {
				is_dev = is_device(ofmt->priv_class);
				if (!is_dev && device_only)
					continue;
				if ((!name || strcmp(ofmt->name, name) < 0) &&
					strcmp(ofmt->name, last_name) > 0) {
					name = ofmt->name;
					long_name = ofmt->long_name;
					encode = 1;
				}
			}
		}
		if (muxdemuxers != SHOW_MUXERS) {
			ifmt_opaque = NULL;
			while ((ifmt = av_demuxer_iterate(&ifmt_opaque))) {
				is_dev = is_device(ifmt->priv_class);
				if (!is_dev && device_only)
					continue;
				if ((!name || strcmp(ifmt->name, name) < 0) &&
					strcmp(ifmt->name, last_name) > 0) {
					name = ifmt->name;
					long_name = ifmt->long_name;
					encode = 0;
				}
				if (name && strcmp(ifmt->name, name) == 0)
					decode = 1;
			}
		}
		if (!name)
			break;
		last_name = name;

		printf(" %c%c %-15s %s\n",
			decode ? 'D' : ' ',
			encode ? 'E' : ' ',
			name,
			long_name ? long_name : " ");
	}
	return 0;
}

int show_formats(void* optctx, const char* opt, const char* arg)
{
	return show_formats_devices(optctx, opt, arg, 0, SHOW_DEFAULT);
}

int show_muxers(void* optctx, const char* opt, const char* arg)
{
	return show_formats_devices(optctx, opt, arg, 0, SHOW_MUXERS);
}

int show_demuxers(void* optctx, const char* opt, const char* arg)
{
	return show_formats_devices(optctx, opt, arg, 0, SHOW_DEMUXERS);
}

int show_devices(void* optctx, const char* opt, const char* arg)
{
	return show_formats_devices(optctx, opt, arg, 1, SHOW_DEFAULT);
}

int show_protocols(void* optctx, const char* opt, const char* arg)
{
	void* opaque = NULL;
	const char* name;

	printf("Supported file protocols:\n"
		"Input:\n");
	while ((name = avio_enum_protocols(&opaque, 0)))
		printf("  %s\n", name);
	printf("Output:\n");
	while ((name = avio_enum_protocols(&opaque, 1)))
		printf("  %s\n", name);
	return 0;
}

int show_colors(void* optctx, const char* opt, const char* arg)
{
	const char* name;
	const uint8_t* rgb;
	int i;

	printf("%-32s #RRGGBB\n", "name");

	for (i = 0; name = av_get_known_color_name(i, &rgb); i++)
		printf("%-32s #%02x%02x%02x\n", name, rgb[0], rgb[1], rgb[2]);

	return 0;
}

int show_pix_fmts(void* optctx, const char* opt, const char* arg)
{
	const AVPixFmtDescriptor* pix_desc = NULL;

	printf("Pixel formats:\n"
		"I.... = Supported Input  format for conversion\n"
		".O... = Supported Output format for conversion\n"
		"..H.. = Hardware accelerated format\n"
		"...P. = Paletted format\n"
		"....B = Bitstream format\n"
		"FLAGS NAME            NB_COMPONENTS BITS_PER_PIXEL BIT_DEPTHS\n"
		"-----\n");

#if !CONFIG_SWSCALE
#   define sws_isSupportedInput(x)  0
#   define sws_isSupportedOutput(x) 0
#endif

	while ((pix_desc = av_pix_fmt_desc_next(pix_desc))) {
		enum AVPixelFormat av_unused pix_fmt = av_pix_fmt_desc_get_id(pix_desc);
		printf("%c%c%c%c%c %-16s       %d            %3d      %d",
			sws_isSupportedInput(pix_fmt) ? 'I' : '.',
			sws_isSupportedOutput(pix_fmt) ? 'O' : '.',
			pix_desc->flags & AV_PIX_FMT_FLAG_HWACCEL ? 'H' : '.',
			pix_desc->flags & AV_PIX_FMT_FLAG_PAL ? 'P' : '.',
			pix_desc->flags & AV_PIX_FMT_FLAG_BITSTREAM ? 'B' : '.',
			pix_desc->name,
			pix_desc->nb_components,
			av_get_bits_per_pixel(pix_desc),
			pix_desc->comp[0].depth);

		for (unsigned i = 1; i < pix_desc->nb_components; i++)
			printf("-%d", pix_desc->comp[i].depth);
		printf("\n");
	}
	return 0;
}

int show_layouts(void* optctx, const char* opt, const char* arg)
{
	const AVChannelLayout* ch_layout;
	void* iter = NULL;
	char buf[128], buf2[128];
	int i = 0;

	printf("Individual channels:\n"
		"NAME           DESCRIPTION\n");
	for (i = 0; i < 63; i++) {
		av_channel_name(buf, sizeof(buf), (AVChannel)i);
		if (strstr(buf, "USR"))
			continue;
		av_channel_description(buf2, sizeof(buf2), (AVChannel)i);
		printf("%-14s %s\n", buf, buf2);
	}
	printf("\nStandard channel layouts:\n"
		"NAME           DECOMPOSITION\n");
	while (ch_layout = av_channel_layout_standard(&iter)) {
		av_channel_layout_describe(ch_layout, buf, sizeof(buf));
		printf("%-14s ", buf);
		for (i = 0; i < 63; i++) {
			int idx = av_channel_layout_index_from_channel(ch_layout, (AVChannel)i);
			if (idx >= 0) {
				av_channel_name(buf2, sizeof(buf2), (AVChannel)i);
				printf("%s%s", idx ? "+" : "", buf2);
			}
		}
		printf("\n");
	}
	return 0;
}

int show_sample_fmts(void* optctx, const char* opt, const char* arg)
{
	int i;
	char fmt_str[128];
	for (i = -1; i < AV_SAMPLE_FMT_NB; i++)
		printf("%s\n", av_get_sample_fmt_string(fmt_str, sizeof(fmt_str), (AVSampleFormat)i));
	return 0;
}

int show_dispositions(void* optctx, const char* opt, const char* arg)
{
	for (int i = 0; i < 32; i++) {
		const char* str = av_disposition_to_string(1U << i);
		if (str)
			printf("%s\n", str);
	}
	return 0;
}

int opt_cpuflags(void* optctx, const char* opt, const char* arg)
{
	int ret;
	unsigned flags = av_get_cpu_flags();

	if ((ret = av_parse_cpu_caps(&flags, arg)) < 0)
		return ret;

	av_force_cpu_flags(flags);
	return 0;
}

int opt_cpucount(void* optctx, const char* opt, const char* arg)
{
	int ret;
	int count;

	static const AVOption opts[] = {
		{"count", NULL, 0, AV_OPT_TYPE_INT, {.i64 = -1}, -1, INT_MAX},
		{NULL},
	};
	static const AVClass c = {
		.class_name = "cpucount",
		.item_name = av_default_item_name,
		.option = opts,
		.version = LIBAVUTIL_VERSION_INT,
	};
	const AVClass* pclass = &c;

	ret = av_opt_eval_int(&pclass, opts, arg, &count);

	if (!ret) {
		av_cpu_force_count(count);
	}

	return ret;
}

static void expand_filename_template(AVBPrint* bp, const char* template1,
	struct tm* tm)
{
	int c;

	while ((c = *(template1++))) {
		if (c == '%') {
			if (!(c = *(template1++)))
				break;
			switch (c) {
			case 'p':
				av_bprintf(bp, "%s", program_name);
				break;
			case 't':
				av_bprintf(bp, "%04d%02d%02d-%02d%02d%02d",
					tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
					tm->tm_hour, tm->tm_min, tm->tm_sec);
				break;
			case '%':
				av_bprint_chars(bp, c, 1);
				break;
			}
		}
		else {
			av_bprint_chars(bp, c, 1);
		}
	}
}

static void log_callback_report(void* ptr, int level, const char* fmt, va_list vl)
{
	va_list vl2;
	char line[1024];
	static int print_prefix = 1;

	va_copy(vl2, vl);
	av_log_default_callback(ptr, level, fmt, vl);
	av_log_format_line(ptr, level, fmt, vl2, line, sizeof(line), &print_prefix);
	va_end(vl2);
	if (report_file_level >= level) {
		fputs(line, report_file);
		fflush(report_file);
	}
}
char* av_err2str_(int ret)
{
	static char ebuf[AV_ERROR_MAX_STRING_SIZE] = {};
	return av_make_error_string(ebuf, AV_ERROR_MAX_STRING_SIZE, ret);
}
int init_report(const char* env, FILE** file)
{
	char* filename_template = NULL;
	char* key, * val;
	int ret, count = 0;
	int prog_loglevel, envlevel = 0;
	time_t now;
	struct tm* tm;
	AVBPrint filename;

	if (report_file) /* already opened */
		return 0;
	time(&now);
	tm = localtime(&now);
	while (env && *env) {
		if ((ret = av_opt_get_key_value(&env, "=", ":", 0, &key, &val)) < 0) {
			if (count)
			{
				av_log(NULL, AV_LOG_ERROR,
					"Failed to parse FFREPORT environment variable: %s\n",
					av_err2str_(ret));
			}
			break;
		}
		if (*env)
			env++;
		count++;
		if (!strcmp(key, "file")) {
			av_free(filename_template);
			filename_template = val;
			val = NULL;
		}
		else if (!strcmp(key, "level")) {
			char* tail;
			report_file_level = strtol(val, &tail, 10);
			if (*tail) {
				av_log(NULL, AV_LOG_FATAL, "Invalid report file level\n");
				exit_program(1);
			}
			envlevel = 1;
		}
		else {
			av_log(NULL, AV_LOG_ERROR, "Unknown key '%s' in FFREPORT\n", key);
		}
		av_free(val);
		av_free(key);
	}

	av_bprint_init(&filename, 0, AV_BPRINT_SIZE_AUTOMATIC);
	expand_filename_template(&filename,
		(const char*)av_x_if_null(filename_template, "%p-%t.log"), tm);
	av_free(filename_template);
	if (!av_bprint_is_complete(&filename)) {
		av_log(NULL, AV_LOG_ERROR, "Out of memory building report file name\n");
		return AVERROR(ENOMEM);
	}

	prog_loglevel = av_log_get_level();
	if (!envlevel)
		report_file_level = FFMAX(report_file_level, prog_loglevel);

	report_file = fopen(filename.str, "w");
	if (!report_file) {
		int ret = AVERROR(errno);
		av_log(NULL, AV_LOG_ERROR, "Failed to open report \"%s\": %s\n",
			filename.str, strerror(errno));
		return ret;
	}
	av_log_set_callback(log_callback_report);
	av_log(NULL, AV_LOG_INFO,
		"%s started on %04d-%02d-%02d at %02d:%02d:%02d\n"
		"Report written to \"%s\"\n"
		"Log level: %d\n",
		program_name,
		tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec,
		filename.str, report_file_level);
	av_bprint_finalize(&filename, NULL);

	if (file)
		*file = report_file;

	return 0;
}

int opt_report(void* optctx, const char* opt, const char* arg)
{
	return init_report(NULL, NULL);
}

int opt_max_alloc(void* optctx, const char* opt, const char* arg)
{
	char* tail;
	size_t max;

	max = strtol(arg, &tail, 10);
	if (*tail) {
		av_log(NULL, AV_LOG_FATAL, "Invalid max_alloc \"%s\".\n", arg);
		exit_program(1);
	}
	av_max_alloc(max);
	return 0;
}

int opt_loglevel(void* optctx, const char* opt, const char* arg)
{
	const struct { const char* name; int level; } log_levels[] = {
		{ "quiet"  , AV_LOG_QUIET   },
		{ "panic"  , AV_LOG_PANIC   },
		{ "fatal"  , AV_LOG_FATAL   },
		{ "error"  , AV_LOG_ERROR   },
		{ "warning", AV_LOG_WARNING },
		{ "info"   , AV_LOG_INFO    },
		{ "verbose", AV_LOG_VERBOSE },
		{ "debug"  , AV_LOG_DEBUG   },
		{ "trace"  , AV_LOG_TRACE   },
	};
	const char* token;
	char* tail;
	int flags = av_log_get_flags();
	int level = av_log_get_level();
	int cmd, i = 0;

	av_assert0(arg);
	while (*arg) {
		token = arg;
		if (*token == '+' || *token == '-') {
			cmd = *token++;
		}
		else {
			cmd = 0;
		}
		if (!i && !cmd) {
			flags = 0;  /* missing relative prefix, build absolute value */
		}
		if (av_strstart(token, "repeat", &arg)) {
			if (cmd == '-') {
				flags |= AV_LOG_SKIP_REPEATED;
			}
			else {
				flags &= ~AV_LOG_SKIP_REPEATED;
			}
		}
		else if (av_strstart(token, "level", &arg)) {
			if (cmd == '-') {
				flags &= ~AV_LOG_PRINT_LEVEL;
			}
			else {
				flags |= AV_LOG_PRINT_LEVEL;
			}
		}
		else {
			break;
		}
		i++;
	}
	if (!*arg) {
		goto end;
	}
	else if (*arg == '+') {
		arg++;
	}
	else if (!i) {
		flags = av_log_get_flags();  /* level value without prefix, reset flags */
	}

	for (i = 0; i < FF_ARRAY_ELEMS(log_levels); i++) {
		if (!strcmp(log_levels[i].name, arg)) {
			level = log_levels[i].level;
			goto end;
		}
	}

	level = strtol(arg, &tail, 10);
	if (*tail) {
		av_log(NULL, AV_LOG_FATAL, "Invalid loglevel \"%s\". "
			"Possible levels are numbers or:\n", arg);
		for (i = 0; i < FF_ARRAY_ELEMS(log_levels); i++)
			av_log(NULL, AV_LOG_FATAL, "\"%s\"\n", log_levels[i].name);
		exit_program(1);
	}

end:
	av_log_set_flags(flags);
	av_log_set_level(level);
	return 0;
}

#if CONFIG_AVDEVICE
static void print_device_list(const AVDeviceInfoList* device_list)
{
	// print devices
	for (int i = 0; i < device_list->nb_devices; i++) {
		const AVDeviceInfo* device = device_list->devices[i];
		printf("%c %s [%s] (", device_list->default_device == i ? '*' : ' ',
			device->device_name, device->device_description);
		if (device->nb_media_types > 0) {
			for (int j = 0; j < device->nb_media_types; ++j) {
				const char* media_type = av_get_media_type_string(device->media_types[j]);
				if (j > 0)
					printf(", ");
				printf("%s", media_type ? media_type : "unknown");
			}
		}
		else {
			printf("none");
		}
		printf(")\n");
	}
}

static int print_device_sources(const AVInputFormat* fmt, AVDictionary* opts)
{
	int ret;
	AVDeviceInfoList* device_list = NULL;

	if (!fmt || !fmt->priv_class || !AV_IS_INPUT_DEVICE(fmt->priv_class->category))
		return AVERROR(EINVAL);

	printf("Auto-detected sources for %s:\n", fmt->name);
	if ((ret = avdevice_list_input_sources(fmt, NULL, opts, &device_list)) < 0) {
		printf("Cannot list sources: %s\n", av_err2str_(ret));
		goto fail;
	}

	print_device_list(device_list);

fail:
	avdevice_free_list_devices(&device_list);
	return ret;
}

static int print_device_sinks(const AVOutputFormat* fmt, AVDictionary* opts)
{
	int ret;
	AVDeviceInfoList* device_list = NULL;

	if (!fmt || !fmt->priv_class || !AV_IS_OUTPUT_DEVICE(fmt->priv_class->category))
		return AVERROR(EINVAL);

	printf("Auto-detected sinks for %s:\n", fmt->name);
	if ((ret = avdevice_list_output_sinks(fmt, NULL, opts, &device_list)) < 0) {
		printf("Cannot list sinks: %s\n", av_err2str_(ret));
		goto fail;
	}

	print_device_list(device_list);

fail:
	avdevice_free_list_devices(&device_list);
	return ret;
}

static int show_sinks_sources_parse_arg(const char* arg, char** dev, AVDictionary** opts)
{
	int ret;
	if (arg) {
		char* opts_str = NULL;
		av_assert0(dev && opts);
		*dev = av_strdup(arg);
		if (!*dev)
			return AVERROR(ENOMEM);
		if ((opts_str = strchr(*dev, ','))) {
			*(opts_str++) = '\0';
			if (opts_str[0] && ((ret = av_dict_parse_string(opts, opts_str, "=", ":", 0)) < 0)) {
				av_freep(dev);
				return ret;
			}
		}
	}
	else
		printf("\nDevice name is not provided.\n"
			"You can pass devicename[,opt1=val1[,opt2=val2...]] as an argument.\n\n");
	return 0;
}

int show_sources(void* optctx, const char* opt, const char* arg)
{
	const AVInputFormat* fmt = NULL;
	char* dev = NULL;
	AVDictionary* opts = NULL;
	int ret = 0;
	int error_level = av_log_get_level();

	av_log_set_level(AV_LOG_WARNING);

	if ((ret = show_sinks_sources_parse_arg(arg, &dev, &opts)) < 0)
		goto fail;

	do {
		fmt = av_input_audio_device_next(fmt);
		if (fmt) {
			if (!strcmp(fmt->name, "lavfi"))
				continue; //it's pointless to probe lavfi
			if (dev && !av_match_name(dev, fmt->name))
				continue;
			print_device_sources(fmt, opts);
		}
	} while (fmt);
	do {
		fmt = av_input_video_device_next(fmt);
		if (fmt) {
			if (dev && !av_match_name(dev, fmt->name))
				continue;
			print_device_sources(fmt, opts);
		}
	} while (fmt);
fail:
	av_dict_free(&opts);
	av_free(dev);
	av_log_set_level(error_level);
	return ret;
}

int show_sinks(void* optctx, const char* opt, const char* arg)
{
	const AVOutputFormat* fmt = NULL;
	char* dev = NULL;
	AVDictionary* opts = NULL;
	int ret = 0;
	int error_level = av_log_get_level();

	av_log_set_level(AV_LOG_WARNING);

	if ((ret = show_sinks_sources_parse_arg(arg, &dev, &opts)) < 0)
		goto fail;

	do {
		fmt = av_output_audio_device_next(fmt);
		if (fmt) {
			if (dev && !av_match_name(dev, fmt->name))
				continue;
			print_device_sinks(fmt, opts);
		}
	} while (fmt);
	do {
		fmt = av_output_video_device_next(fmt);
		if (fmt) {
			if (dev && !av_match_name(dev, fmt->name))
				continue;
			print_device_sinks(fmt, opts);
		}
	} while (fmt);
fail:
	av_dict_free(&opts);
	av_free(dev);
	av_log_set_level(error_level);
	return ret;
}
#endif /* CONFIG_AVDEVICE */
#endif // 1

void init_options(play_ctx* ctx)
{
	std::vector<OptionDef> options = {
			{ "L"          , OPT_EXIT, {.func_arg = show_license},      "show license" },
			{ "h"          , OPT_EXIT, {.func_arg = show_help},         "show help", "topic" },
			{ "?"          , OPT_EXIT, {.func_arg = show_help},         "show help", "topic" },
			{ "help"       , OPT_EXIT, {.func_arg = show_help},         "show help", "topic" },
			{ "-help"      , OPT_EXIT, {.func_arg = show_help},         "show help", "topic" },
			{ "version"    , OPT_EXIT, {.func_arg = show_version},      "show version" },
			{ "buildconf"  , OPT_EXIT, {.func_arg = show_buildconf},    "show build configuration" },
			{ "formats"    , OPT_EXIT, {.func_arg = show_formats  },    "show available formats" },
			{ "devices"    , OPT_EXIT, {.func_arg = show_devices  },    "show available devices" },
			{ "codecs"     , OPT_EXIT, {.func_arg = show_codecs   },    "show available codecs" },
			{ "decoders"   , OPT_EXIT, {.func_arg = show_decoders },    "show available decoders" },
			{ "encoders"   , OPT_EXIT, {.func_arg = show_encoders },    "show available encoders" },
			{ "bsfs"       , OPT_EXIT, {.func_arg = show_bsfs     },    "show available bit stream filters" },
			{ "protocols"  , OPT_EXIT, {.func_arg = show_protocols},    "show available protocols" },
			{ "filters"    , OPT_EXIT, {.func_arg = show_filters  },    "show available filters" },
			{ "pix_fmts"   , OPT_EXIT, {.func_arg = show_pix_fmts },    "show available pixel formats" },
			{ "layouts"    , OPT_EXIT, {.func_arg = show_layouts  },    "show standard channel layouts" },
			{ "sample_fmts", OPT_EXIT, {.func_arg = show_sample_fmts }, "show available audio sample formats" },
			{ "colors"     , OPT_EXIT, {.func_arg = show_colors },      "show available color names" },
			{ "loglevel"   , HAS_ARG,  {.func_arg = opt_loglevel},      "set logging level", "loglevel" },
			{ "v",           HAS_ARG,  {.func_arg = opt_loglevel},      "set logging level", "loglevel" },
			{ "report"     , 0,        {(void*)opt_report}, "generate a report" },
			{ "max_alloc"  , HAS_ARG,  {.func_arg = opt_max_alloc},     "set maximum size of a single allocated block", "bytes" },
			{ "cpuflags"   , HAS_ARG | OPT_EXPERT, {.func_arg = opt_cpuflags }, "force specific cpu flags", "flags" },
			{ "hide_banner", OPT_BOOL | OPT_EXPERT, {&hide_banner},     "do not show program banner", "hide_banner" },
		#if CONFIG_OPENCL
			{ "opencl_bench", OPT_EXIT, {.func_arg = opt_opencl_bench}, "run benchmark on all OpenCL devices and show results" },
			{ "opencl_options", HAS_ARG, {.func_arg = opt_opencl},      "set OpenCL environment options" },
		#endif
		#if CONFIG_AVDEVICE
			{ "sources"    , OPT_EXIT | HAS_ARG, {.func_arg = show_sources },
			  "list sources of the input device", "device" },
			{ "sinks"      , OPT_EXIT | HAS_ARG, {.func_arg = show_sinks },
			  "list sinks of the output device", "device" },
		#endif
			  // CMDUTILS_COMMON_OPTIONS
			   { "x", HAS_ARG, {.func_arg = opt_width1 }, "force displayed width", "width" },
			   { "y", HAS_ARG, {.func_arg = opt_height1 }, "force displayed height", "height" },
			   { "fs", OPT_BOOL, { &ctx->is_full_screen }, "force full screen" },
			   { "an", OPT_BOOL, { &ctx->audio_disable }, "disable audio" },
			   { "vn", OPT_BOOL, { &ctx->video_disable }, "disable video" },
			   { "sn", OPT_BOOL, { &ctx->subtitle_disable }, "disable subtitling" },
			   { "ast", OPT_STRING | HAS_ARG | OPT_EXPERT, { &ctx->wanted_stream_spec[AVMEDIA_TYPE_AUDIO] }, "select desired audio stream", "stream_specifier" },
			   { "vst", OPT_STRING | HAS_ARG | OPT_EXPERT, { &ctx->wanted_stream_spec[AVMEDIA_TYPE_VIDEO] }, "select desired video stream", "stream_specifier" },
			   { "sst", OPT_STRING | HAS_ARG | OPT_EXPERT, { &ctx->wanted_stream_spec[AVMEDIA_TYPE_SUBTITLE] }, "select desired subtitle stream", "stream_specifier" },
			   { "ss", HAS_ARG, {.func_arg = opt_seek1 }, "seek to a given position in seconds", "pos" },
			   { "t", HAS_ARG, {.func_arg = opt_duration1 }, "play  \"duration\" seconds of audio/video", "duration" },
			   { "bytes", OPT_INT | HAS_ARG, { &ctx->seek_by_bytes }, "seek by bytes 0=off 1=on -1=auto", "val" },
			   { "seek_interval", OPT_FLOAT | HAS_ARG, { &ctx->seek_interval }, "set seek interval for left/right keys, in seconds", "seconds" },
			   { "nodisp", OPT_BOOL, { &ctx->display_disable }, "disable graphical display" },
			   { "noborder", OPT_BOOL, { &ctx->borderless }, "borderless window" },
			   { "alwaysontop", OPT_BOOL, { &ctx->alwaysontop }, "window always on top" },
			   { "volume", OPT_INT | HAS_ARG, { &ctx->startup_volume}, "set startup volume 0=min 100=max", "volume" },
			   { "f", HAS_ARG, {.func_arg = opt_format1 }, "force format", "fmt" },
			   { "stats", OPT_BOOL | OPT_EXPERT, { &ctx->show_status }, "show status", "" },
			   { "fast", OPT_BOOL | OPT_EXPERT, { &ctx->fast }, "non spec compliant optimizations", "" },
			   { "genpts", OPT_BOOL | OPT_EXPERT, { &ctx->genpts }, "generate pts", "" },
			   { "drp", OPT_INT | HAS_ARG | OPT_EXPERT, { &ctx->decoder_reorder_pts }, "let decoder reorder pts 0=off 1=on -1=auto", ""},
			   { "lowres", OPT_INT | HAS_ARG | OPT_EXPERT, { &ctx->lowres }, "", "" },
			   { "sync", HAS_ARG | OPT_EXPERT, {.func_arg = opt_sync1 }, "set audio-video sync. type (type=audio/video/ext)", "type" },
			   { "autoexit", OPT_BOOL | OPT_EXPERT, { &ctx->autoexit }, "exit at the end", "" },
			   { "exitonkeydown", OPT_BOOL | OPT_EXPERT, { &ctx->exit_on_keydown }, "exit on key down", "" },
			   { "exitonmousedown", OPT_BOOL | OPT_EXPERT, { &ctx->exit_on_mousedown }, "exit on mouse down", "" },
			   { "loop", OPT_INT | HAS_ARG | OPT_EXPERT, { &ctx->ploop }, "set number of times the playback shall be looped", "loop count" },
			   { "framedrop", OPT_BOOL | OPT_EXPERT, { &ctx->framedrop }, "drop frames when cpu is too slow", "" },
			   { "infbuf", OPT_BOOL | OPT_EXPERT, { &ctx->infinite_buffer }, "don't limit the input buffer size (useful with realtime streams)", "" },
			   { "window_title", OPT_STRING | HAS_ARG, { &ctx->window_title }, "set window title", "window title" },
			   { "left", OPT_INT | HAS_ARG | OPT_EXPERT, { &ctx->screen_left }, "set the x position for the left of the window", "x pos" },
			   { "top", OPT_INT | HAS_ARG | OPT_EXPERT, { &ctx->screen_top }, "set the y position for the top of the window", "y pos" },
		   #if CONFIG_AVFILTER
			   { "vf", OPT_EXPERT | HAS_ARG, {.func_arg = opt_add_vfilter1 }, "set video filters", "filter_graph" },
			   { "af", OPT_STRING | HAS_ARG, { &ctx->afilters }, "set audio filters", "filter_graph" },
		   #endif
			   { "rdftspeed", OPT_INT | HAS_ARG | OPT_AUDIO | OPT_EXPERT, { &ctx->rdftspeed }, "rdft speed", "msecs" },
			   { "showmode", HAS_ARG, {.func_arg = opt_show_mode1}, "select show mode (0 = video, 1 = waves, 2 = RDFT)", "mode" },
			   { "i", OPT_BOOL, { &ctx->dummy}, "read specified file", "input_file"},
			   { "codec", HAS_ARG, {.func_arg = opt_codec1}, "force decoder", "decoder_name" },
			   { "acodec", HAS_ARG | OPT_STRING | OPT_EXPERT, {    &ctx->audio_codec_name }, "force audio decoder",    "decoder_name" },
			   { "scodec", HAS_ARG | OPT_STRING | OPT_EXPERT, { &ctx->subtitle_codec_name }, "force subtitle decoder", "decoder_name" },
			   { "vcodec", HAS_ARG | OPT_STRING | OPT_EXPERT, {    &ctx->video_codec_name }, "force video decoder",    "decoder_name" },
			   { "autorotate", OPT_BOOL, { &ctx->autorotate }, "automatically rotate video", "" },
			   { "find_stream_info", OPT_BOOL | OPT_INPUT | OPT_EXPERT, { &ctx->find_stream_info },
				   "read and decode the streams to fill missing information with heuristics" },
			   { "filter_threads", HAS_ARG | OPT_INT | OPT_EXPERT, { &ctx->filter_nbthreads }, "number of filter threads per graph" },
			   { NULL, },
	};
	ctx->options.swap(options);
}
#endif
static void show_usage(void)
{
	av_log(NULL, AV_LOG_INFO, "Simple media player\n");
	av_log(NULL, AV_LOG_INFO, "usage: %s [options] input_file\n", program_name);
	av_log(NULL, AV_LOG_INFO, "\n");
}

void show_help_default(const char* opt, const char* arg)
{
	av_log_set_callback(log_callback_help);
	show_usage();
	//show_help_options(options, "Main options:", 0, OPT_EXPERT, 0);
	//show_help_options(options, "Advanced options:", OPT_EXPERT, 0, 0);
	printf("\n");
	show_help_children(avcodec_get_class(), AV_OPT_FLAG_DECODING_PARAM);
	show_help_children(avformat_get_class(), AV_OPT_FLAG_DECODING_PARAM);
#if !CONFIG_AVFILTER
	show_help_children(sws_get_class(), AV_OPT_FLAG_ENCODING_PARAM);
#else
	show_help_children(avfilter_get_class(), AV_OPT_FLAG_FILTERING_PARAM);
#endif
	printf("\nWhile playing:\n"
		"q, ESC              quit\n"
		"f                   toggle full screen\n"
		"p, SPC              pause\n"
		"m                   toggle mute\n"
		"9, 0                decrease and increase volume respectively\n"
		"/, *                decrease and increase volume respectively\n"
		"a                   cycle audio channel in the current program\n"
		"v                   cycle video channel\n"
		"t                   cycle subtitle channel in the current program\n"
		"c                   cycle program\n"
		"w                   cycle video filters or show modes\n"
		"s                   activate frame-step mode\n"
		"left/right          seek backward/forward 10 seconds or to custom interval if -seek_interval is set\n"
		"down/up             seek backward/forward 1 minute\n"
		"page down/page up   seek backward/forward 10 minutes\n"
		"right mouse click   seek to percentage in file corresponding to fraction of width\n"
		"left double-click   toggle full screen\n"
	);
}
void play_ctx::new_display()
{
	int flags;
	if (display_disable) {
		video_disable = 1;
	}
	flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;
	if (audio_disable)
		flags &= ~SDL_INIT_AUDIO;
	else {
		/* Try to work around an occasional ALSA buffer underflow issue when the
		 * period size is NPOT due to ALSA resampling by forcing the buffer size. */
		if (!SDL_getenv("SDL_AUDIO_ALSA_SET_BUFFER_SIZE"))
			SDL_setenv("SDL_AUDIO_ALSA_SET_BUFFER_SIZE", "1", 1);
	}
	if (display_disable)
		flags &= ~SDL_INIT_VIDEO;
	if (SDL_Init(flags)) {
		av_log(NULL, AV_LOG_FATAL, "Could not initialize SDL - %s\n", SDL_GetError());
		av_log(NULL, AV_LOG_FATAL, "(Did you set the DISPLAY variable?)\n");
		exit(1);
	}

	SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
	SDL_EventState(SDL_USEREVENT, SDL_IGNORE);

	if (!display_disable) {
		int flags = SDL_WINDOW_HIDDEN;
		if (alwaysontop)
#if SDL_VERSION_ATLEAST(2,0,5)
			flags |= SDL_WINDOW_ALWAYS_ON_TOP;
#else
			av_log(NULL, AV_LOG_WARNING, "Your SDL version doesn't support SDL_WINDOW_ALWAYS_ON_TOP. Feature will be inactive.\n");
#endif
		if (borderless)
			flags |= SDL_WINDOW_BORDERLESS;
		else
			flags |= SDL_WINDOW_RESIZABLE;

#ifdef SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR
		SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
#endif
		window = SDL_CreateWindow(program_name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, default_width, default_height, flags);
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
		if (window) {
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (!renderer) {
				av_log(NULL, AV_LOG_WARNING, "Failed to initialize a hardware accelerated renderer: %s\n", SDL_GetError());
				renderer = SDL_CreateRenderer(window, -1, 0);
			}
			if (renderer) {
				if (!SDL_GetRendererInfo(renderer, &renderer_info))
					av_log(NULL, AV_LOG_VERBOSE, "Initialized %s renderer.\n", renderer_info.name);
			}
		}
		if (!window || !renderer || !renderer_info.num_texture_formats) {
			av_log(NULL, AV_LOG_FATAL, "Failed to create window or renderer: %s", SDL_GetError());
			do_exit(NULL);
		}
	}
}

static std::map<std::string, hwdev_t> mhw;

void* ff_open(const char* url, void(*dcb)(yuv_info_t*))//, int (*ctrl_cb)(ctrl_data_t*))
{
	if (!url)return 0;
	static std::once_flag flg;
	std::call_once(flg, [=]() {
		init_dynload();
		/* register all codecs, demux and protocols */
#if CONFIG_AVDEVICE
		avdevice_register_all();
#endif
		avformat_network_init();

		AVHWDeviceType type = AV_HWDEVICE_TYPE_NONE;
		AVHWDeviceContext* device_ctx = 0;
		// ffmepg 支持的加速器列表
		while ((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE)
		{
			auto c = av_hwdevice_get_type_name(type);
			mhw[c].t = type;
			auto& it = mhw[c];

			// 创建硬件加速器的缓冲区
			if (av_hwdevice_ctx_create(&it.hdc, type, NULL, NULL, 0) < 0) {
				mhw.erase(c);
			}
			else
			{
				device_ctx = reinterpret_cast<AVHWDeviceContext*>(it.hdc->data);
				it.name = c;
			}
			/** 如果使用软解码则默认有一个软解码的缓冲区(获取AVFrame的)，而硬解码则需要额外创建硬件解码的缓冲区
			 *  这个缓冲区变量为hw_frames_ctx，不手动创建，则在调用avcodec_send_packet()函数内部自动创建一个
			 *  但是必须手动赋值硬件解码缓冲区引用hw_device_ctx(它是一个AVBufferRef变量)
			 */
			 // 即hw_device_ctx有值则使用硬件解码

		}
		bool testb = false;
		if (testb)
		{
			AVCodecID codec_id = AV_CODEC_ID_H264;

			// 1. 查找解码器
			auto codec = avcodec_find_decoder(codec_id);

			// 2. 根据解码器创建解码器上下文
			auto c = avcodec_alloc_context3(codec);
			// 初始化硬件加速上下文
			AVBufferRef* hw_ctx = nullptr;
			std::vector<std::string> hwv;
			{


				//////////////////////////////////////////////////
				/// 硬件加速部分

				auto hw_type = AV_HWDEVICE_TYPE_DXVA2;
				// 打印所有支持的硬件加速方式
				for (int i = 0;; i++)
				{
					auto config = avcodec_get_hw_config(codec, i);
					if (!config) break;
					if (config->device_type)
						hwv.push_back(av_hwdevice_get_type_name(config->device_type));
				}

				av_hwdevice_ctx_create(&hw_ctx, hw_type, nullptr, nullptr, 0);

			}
			std::vector<const AVCodec*> avc;
			std::map<std::string, std::vector<const AVCodec*>> avn;
			AVCodec* prev = NULL, * p;
			void* i = 0;
			while ((p = (AVCodec*)av_codec_iterate(&i))) {
				//avc.push_back(p);
				avn[p->name].push_back(p);
				prev = p;
			}
			for (auto& [k, v] : mhw)
			{
				avc.push_back(avcodec_find_decoder_by_name(k.c_str()));
			}
			auto codec1 = avcodec_find_decoder_by_name("h264_cuvid");
		}
		int n = mhw.size();
		printf("hw count:\t%d\n", n);
		});

	play_ctx* ctx = new play_ctx();
	ctx->input_filename = url;

	if (mhw.size())
	{
		for (auto& [k, v] : mhw)
		{
			if (k.find("d3d11") != std::string::npos)
			{
				ctx->hwctx = &v;
			}
		}
		if (!ctx->hwctx)
		{
			auto& hw = *mhw.begin();
			ctx->hwctx = &hw.second;
		}
		//ctx->video_codec_name = hw.first.c_str();
	}

	SDL_sem* sem = SDL_CreateSemaphore(0);
	auto cb = [=]() {
		VideoState* is;
		init_options(ctx);
		ctx->ispushaudio = true;
		//ctx->infinite_buffer = 0;
		ctx->av_sync_type = 0;// AV_SYNC_VIDEO_MASTER;// AV_SYNC_EXTERNAL_CLOCK;// 同步到外部时钟
		av_log_set_flags(AV_LOG_SKIP_REPEATED);
		auto options = ctx->options.data();
		//parse_loglevel(argc, argv, options);


		signal(SIGINT, sigterm_handler); /* Interrupt (ANSI).    */
		signal(SIGTERM, sigterm_handler); /* Termination (ANSI).  */

		//show_banner(argc, argv, options);

		//parse_options(ctx, argc, argv, options, opt_input_file1);

		if (!ctx->input_filename) {
			show_usage();
			av_log(NULL, AV_LOG_FATAL, "An input file must be specified\n");
			av_log(NULL, AV_LOG_FATAL,
				"Use -h to get full help or, even better, run 'man %s'\n", program_name);
			exit(1);
		}
		is = ctx->stream_open(ctx->input_filename, ctx->file_iformat);
		if (is)
		{
			//ctx->ctrl_cb = ctrl_cb;
			ctx->tis = is;
			is->dcb = dcb;
			SDL_SemPost(sem);
			ctx->event_loop(is);
		}
		else
		{
			av_log(NULL, AV_LOG_FATAL, "Failed to initialize VideoState!\n");
			ctx->do_exit(NULL);
		}

		delete ctx;
	};

	std::thread th(cb);
	{
#ifdef _DEBUG
#ifdef __print_time_h__
		print_time a("sem wait");
#endif
#endif
		SDL_SemWaitTimeout(sem, 1000);
	}
	if (sem)
		SDL_DestroySemaphore(sem);
	th.detach();
	return ctx;
}
void ff_set(void* p, ctrl_data_t* c)
{
	auto ctx = (play_ctx*)p;
	if (p && c)
	{
		ctx->pc = c;
		ctx->is_ctrl = 1;
	}
}
/* Called from the main */
int ff_play(int argc, char** argv, bool isdisplay, void(*dcb)(yuv_info_t*))
{
	auto cb = [=]() {
		VideoState* is;
		play_ctx* ctx = new play_ctx();
		init_dynload();
		init_options(ctx);
		ctx->ispushaudio = true;
		//ctx->infinite_buffer = 0;
		ctx->av_sync_type = 0;// AV_SYNC_VIDEO_MASTER;// AV_SYNC_EXTERNAL_CLOCK;// 同步到外部时钟
		av_log_set_flags(AV_LOG_SKIP_REPEATED);
		auto options = ctx->options.data();
		parse_loglevel(argc, argv, options);

		/* register all codecs, demux and protocols */
#if CONFIG_AVDEVICE
		avdevice_register_all();
#endif
		avformat_network_init();

		signal(SIGINT, sigterm_handler); /* Interrupt (ANSI).    */
		signal(SIGTERM, sigterm_handler); /* Termination (ANSI).  */

		//show_banner(argc, argv, options);

		parse_options(ctx, argc, argv, options, opt_input_file1);

		if (!ctx->input_filename) {
			show_usage();
			av_log(NULL, AV_LOG_FATAL, "An input file must be specified\n");
			av_log(NULL, AV_LOG_FATAL,
				"Use -h to get full help or, even better, run 'man %s'\n", program_name);
			exit(1);
		}
		if (isdisplay)
			ctx->new_display();
		is = ctx->stream_open(ctx->input_filename, ctx->file_iformat);
		if (is)
		{
			is->dcb = dcb;

			if (isdisplay)
				ctx->event_loop1(is);
			else
				ctx->event_loop(is);
		}
		else
		{
			av_log(NULL, AV_LOG_FATAL, "Failed to initialize VideoState!\n");
			ctx->do_exit(NULL);
		}

		delete ctx; ctx = 0;
	};
	if (isdisplay)
	{
		cb();
	}
	else {
		std::thread th(cb);
		th.detach();
	}
	/* never returns */
	return 0;
}
