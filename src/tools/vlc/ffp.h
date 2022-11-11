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
	int8_t t = 0;			// 1planeʱ422����0=gbr, 1=brg
	int8_t plane = 0;		// 1 2 3
};
#else
struct yuv_info_t;
#endif
//
struct av_time_t
{
	int second = 0;		// ��������
	int hms[3] = {};	// Hour minute second
};
// �ڻص������޸�һ�����óɹ����Զ���ԭĬ��ֵ
struct ctrl_data_t
{
	void* ctx = 0;
	// set
	int ploop = 0;		// ѭ������
	int volume = -1;	// 0-128
	int volume_inc = 0;	// ����ƽ������1��
	int seek_pos = -1;	// -1��ǰλ��
	int seek_inc = 0;	// ������
	float seek_scale = -1;	// ƫ�Ʊ���0-1.0
	int chapter = 0;	// ��ҳ1��-1������10����
	bool pause = false;	// ��ͣ
	bool mute = false;	// ����
	bool full_screen = false;//ȫ��
	bool next_frame = false;//��ʾһ֡
	bool is_exit = false;	// �˳�����Ҫ����ff_open
};
// ��һ��ý���ļ���dcb������ʾ�ص�����
void* ff_open(const char* url, void(*dcb)(yuv_info_t*));
// ff_set���Զ��̵߳���
void ff_set(void* p, ctrl_data_t* c);
// ��ȡʱ��
av_time_t ff_get_time(void* p);

// test api
int ff_play(int argc, char** argv, bool isdisplay, void(*dcb)(yuv_info_t*));
void vlc_play(const char* fn, bool* isplay);

