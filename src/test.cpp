#include <windows.h>
#include <stdlib.h>
#include "font_box.h"
int main()
{
	FontBox<Image> _box;
	//����Ĭ�Ͽ��С
	_box.set_defmax({ 1024,1024 });

	//��һ��ͼ�����װ�䷵��λ��
	Image* img = 0;//�����ַ�ͼ��
	Image* oi = nullptr;//�����������ӵ�ͼ��
	glm::ivec4 rs = _box.push(img, &oi);

	//�������
	_box.clear();
	system("pause");
	return 0;
}