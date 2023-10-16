#pragma once
#include <lua/reghelper.h>
// c
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
// cpp
namespace hz
{

	class gui_lua
	{
	private:
		lua_cx* ctx = 0;
	public:
		gui_lua(lua_cx* p);
		~gui_lua();
	public:
		void init();
		void makedata();
	};
}
//!hz
#endif
