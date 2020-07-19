#ifndef _HLIME_H_
#define _HLIME_H_
#include <functional>
#ifdef _WIN32
#include "dropdrag.h"
#include <imm.h>
#pragma comment(lib,"imm32.lib")
#endif
namespace hz {
	//-----------------------------------------------------------------------------
	class IMEDelegate :public Res
	{
	public:
		static IMEDelegate* create()
		{
			return new IMEDelegate();
		}
	protected:
		IMEDelegate() {}
	public:
		~IMEDelegate() {}
		void setEnable(bool eb)
		{
			LOCK(_lck);
			_isDisable = eb;
		}
		void ImeEnter()
		{
			LOCK(_lck);
			_enter = true;
		}
		void ImeExit()
		{
			LOCK(_lck);
			_enter = false;
		}
		void InsertText(const char *text, size_t len, std::wstring * utext)
		{
			LOCK(_lck);
			if (!_isDisable && (pOnChar))
			{
				pOnChar(text, len, utext);
			}
		}
		std::function<void(const char *, size_t, std::wstring * utext)> pOnChar;           //设置获取字符回调函数
#if 1
		//返回输入法坐标
		std::function<void(int *x, int *y)>		   onSetIme;

#endif
		bool _enter = false;
	private:
		bool             _isDisable = false;
		Lock _lck;
	};
	//-----------------------------------------------------------------------------


	//-----------------------------------------------------------------------------

}
#endif //end _HLIME_H_