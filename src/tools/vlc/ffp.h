

#ifdef __cplusplus
extern "C" {
#endif
	int ff_play0(int argc, char** argv);

#ifdef __cplusplus
}
#endif
struct yuv_info_t;


int ff_play(int argc, char** argv, bool isdisplay, void(*dcb)(yuv_info_t*));
void vlc_play(const char* fn, bool* isplay);
