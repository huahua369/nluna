

#ifdef __cplusplus
extern "C" {
#endif
	int ff_play0(int argc, char** argv);

#ifdef __cplusplus
}
#endif
struct yuv_info_t;
// �ڻص������޸�һ�����óɹ����Զ���ԭĬ��ֵ
struct ctrl_data_t
{
	void* ctx = 0;
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
};
// dcb������ʾ�ص�������ctrl_cb����1�޸�
void* ff_open(const char* url, void(*dcb)(yuv_info_t*));
void ff_set(void* p, ctrl_data_t* c);

int ff_play(int argc, char** argv, bool isdisplay, void(*dcb)(yuv_info_t*));
void vlc_play(const char* fn, bool* isplay);

