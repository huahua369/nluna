#include <windows.h>
#include <stdlib.h>
#include "font_box.h"
int main()
{
	FontBox<Image> _box;
	//设置默认块大小
	_box.set_defmax({ 1024,1024 });

	//把一个图像矩形装箱返回位置
	Image* img = 0;//单个字符图像
	Image* oi = nullptr;//返回字体箱子的图像
	glm::ivec4 rs = _box.push(img, &oi);

	//清空所有
	_box.clear();
	system("pause");
	return 0;
}