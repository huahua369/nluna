#include <windows.h>
#include <stdlib.h>
#define _STD_STR_

#define STB_IMPLEMENTATION
#include "tools/base/camera.hpp"
#include "tools/image_packer.h"
#include "tools/font.h"

static void print_json(const njson& n, int idxs)
{
	std::string str;
	if (idxs > 0)
	{
		str = n.dump(idxs);
	}
	else
	{
		str = n.dump();
	}
	//str = u8toA(str);
	printf("%s\n", str.c_str());
}
void load_font(hz::Fonts* fts)
{
	std::string dirss = R"(C:\Windows\Fonts\)";
	std::vector<std::string> fns = { "simsun.ttc","consola.ttf","consolab.ttf","consolaz.ttf","consolai.ttf" // , R"(D:\mfyy.ttf)"
	};
	std::vector<std::string> fnd;
	stbtt_fontinfo fonts[64];
	stbtt_fontinfo* ft = &fonts[0];
	njson ftn;
	int i = 0;
	hz::Fonts::Glyph* glyph = 0;
	int ss1 = sizeof(stbtt_vertex);
	for (auto it : fns)
	{
		if (it[1] != ':') it = dirss + it;
		fts->add_font_file(it);
	}
	print_json(fts->get_font_info(), 1);
}
void test_font(int w = 512, int h = 512)
{
	hz::Fonts fts;
	hz::FontBox<hz::Image> packer;							//缓存
	packer.set_defmax({ w, h });
	std::set<hz::Image*> out;
	hz::Image* img = hz::Image::create_null(16, 16);
	hz::Image* cav = hz::Image::create_null(512, 128);
	load_font(&fts);
	auto nsimsun = fts.get_font(u8"新宋体");
	auto decd = nsimsun->new_SBitDecoder();
	nsimsun->get_glyph_bitmap(u8"新宋体，你好！", 12, decd);
	return;
}
int main()
{
#if 0
	FontBox<Image> _box;
	//设置默认块大小
	_box.set_defmax({ 1024,1024 });

	//把一个图像矩形装箱返回位置
	Image* img = 0;//单个字符图像
	Image* oi = nullptr;//返回字体箱子的图像
	glm::ivec4 rs = _box.push(img, &oi);

	//清空所有
	_box.clear();
#endif
	system("pause");
	return 0;
}