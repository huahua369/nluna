#ifndef __stb_inc_h__
#define __stb_inc_h__

//只能在一个cpp文件定义STB_IMPLEMENTATION
//#define STB_IMPLEMENTATION

//#define STB_C_LEXER_IMPLEMENTATION

#ifdef STB_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION
#define STB_ONLY
#endif
#include "stb_image.h"
#include "stb_image_resize.h"
#include "stb_image_write.h"
#include "stb_rect_pack.h"
#include "stb_truetype.h"
#endif // !__stb_inc_h__