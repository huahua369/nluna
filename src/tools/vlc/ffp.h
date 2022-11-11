#ifdef __cplusplus
extern "C" {
#endif
	int ff_play0(int argc, char** argv);

#ifdef __cplusplus
}
#endif

#ifndef YUV_INFO_ST
#define YUV_INFO_ST
struct yuv_info_t {
	void* ctx = 0;
	void* data[3] = {};
	uint32_t size[3] = {};
	uint32_t width = 0, height = 0;
	int8_t format;			// 0=420, 1=422, 2=444
	int8_t b = 8;			// bpp=8,10,12,16
	int8_t t = 0;			// 1plane时422才有0=gbr, 1=brg
	int8_t plane = 0;		// 1 2 3
};
#else
struct yuv_info_t;
#endif
//
struct av_time_t
{
	int second = 0;		// 返回秒数
	int hms[3] = {};	// Hour minute second
};
// 在回调函数修改一次设置成功后自动还原默认值
struct ctrl_data_t
{
	void* ctx = 0;
	// set
	int ploop = 0;		// 循环次数
	int volume = -1;	// 0-128
	int volume_inc = 0;	// 音量平滑步进1，
	int seek_pos = -1;	// -1当前位置
	int seek_inc = 0;	// 步进秒
	float seek_scale = -1;	// 偏移比例0-1.0
	int chapter = 0;	// 翻页1，-1，或者10分钟
	bool pause = false;	// 暂停
	bool mute = false;	// 静音
	bool full_screen = false;//全屏
	bool next_frame = false;//显示一帧
	bool is_exit = false;	// 退出，需要重新ff_open
};
// 打开一个媒体文件，dcb设置显示回调函数
void* ff_open(const char* url, void(*dcb)(yuv_info_t*));
// ff_set可以多线程调用
void ff_set(void* p, ctrl_data_t* c);
// 获取时间
av_time_t ff_get_time(void* p);

// test api
int ff_play(int argc, char** argv, bool isdisplay, void(*dcb)(yuv_info_t*));
void vlc_play(const char* fn, bool* isplay);

