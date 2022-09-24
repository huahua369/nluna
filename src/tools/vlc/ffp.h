

#ifdef __cplusplus
extern "C" {
#endif
	int ff_play0(int argc, char** argv);

#ifdef __cplusplus
}
#endif
struct yuv_info_t;
// 在回调函数修改一次设置成功后自动还原默认值
struct ctrl_data_t
{
	void* ctx = 0;
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
};
// dcb设置显示回调函数，ctrl_cb返回1修改
void* ff_open(const char* url, void(*dcb)(yuv_info_t*));
void ff_set(void* p, ctrl_data_t* c);

int ff_play(int argc, char** argv, bool isdisplay, void(*dcb)(yuv_info_t*));
void vlc_play(const char* fn, bool* isplay);

