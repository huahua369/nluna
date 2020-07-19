

#include <data/json_helper.h>
#include "file.h"
#include "dirWatch.h"
namespace hz
{
#ifdef _WIN32
	// 一个通过内存地址取得模块句柄的帮助函数

	HMODULE __stdcall File::ModuleFromAddress(PVOID pv)
	{
		MEMORY_BASIC_INFORMATION mbi;
		if (::VirtualQuery(pv, &mbi, sizeof(mbi)) != 0)
		{
			return (HMODULE)mbi.AllocationBase;
		}
		else
		{
			return NULL;
		}
	}
#endif
	std::string File::getAP()
	{
		std::string ret;
		//static std::once_flag flag;
		//std::call_once(flag, [&]() {
#ifdef _WIN32
#ifdef __EXEFILE__
		char szAppPath[1024] = { 0 };
		GetModuleFileNameA(GetModuleHandleA(""), (char*)szAppPath, 1024);
		(strrchr((char*)szAppPath, '\\'))[1] = 0;
		ret = szAppPath;
#else
		//可以获得DLL自身的路径
		char dllpath[MAX_PATH];
		::memset(dllpath, 0, sizeof(dllpath));
		::GetModuleFileNameA(ModuleFromAddress(ModuleFromAddress), dllpath, MAX_PATH);
		ret = getDic(dllpath);
#endif // __EXEFILE__
#else
		// linux获取本进程目录
		size_t len = PATH_MAX;
		char processdir[PATH_MAX];
		char processname[1024];
		char* path_end;
		std::vector<std::string> vs;
		if (readlink("/proc/self/exe", processdir, len) <= 0)
			return ret;
		path_end = strrchr(processdir, '/');
		if (path_end == NULL)
			return ret;
		++path_end;
		strcpy(processname, path_end);
		*path_end = '\0';
		size_t l = (size_t)(path_end - processdir);
		vs.push_back(processdir);
		vs.push_back(processname);
		ret = processdir;
		//ret = getenv("HOME");
		//ret += "/";
#endif
		//	});
		return ret;
	}


	//----------------------------------------------------------------------------------------------------

	inotify_d::inotify_d()
	{
	}

	inotify_d::~inotify_d()
	{
	}
#ifdef _WIN32
	void inotify_d::init(bool nonblock)
	{

	}
#ifdef _WATCH_H_
	int  inotify_d::inotify(const std::vector<std::string>& pathname, ty mask)
	{
		CDirectoryWatch::createWatchFile(gbk_w(pathname[0]));
		return 0;
	}
#endif
#else
	void inotify_d::init(bool nonblock)
	{
		if (nonblock)
		{
			_fd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
		}
		else {
			_fd = inotify_init();
		}
	}

	/*
	func返回非0退出监控
	*/
	int  inotify_d::inotify(const std::vector<std::string>& pathname, ty mk)
	{
		uint32_t mask = 0;
		if (mask == 0)
		{
			mask = IN_ALL_EVENTS;
		}
#define EVENT_NUM 12  
		/*
		位值			In	Out	描述
		IN_ACCESS	*	*	文件被访问(read)
		IN_ATTRIB	*	*	文件元数据改变
		IN_CLOSE_WRITE	*	*	关闭为了写入而打开的文件
		IN_CLOSE_NOWRITE	*	*	关闭以只读方式打开的文件
		IN_CREATE	*	*	在受监控目录内创建了文件/目录
		IN_DELETE	*	*	在受监控目录内删除了文件/目录
		IN_DELETE_SELF	*	*	删除受监控目录/文件本身
		IN_MODIFY	*	*	文件被修改
		IN_MOVE_SELF	*	*	移动受监控目录/文件本身
		IN_MOVED_FROM	*	*	文件移出受监控目录之外
		IN_MOVED_TO	*	*	将文件移入受监控目录
		IN_OPEN	*	*	文件被打开
		IN_ALL_EVENTS	*		以上所有输出事件的统称
		IN_MOVE	*		IN_MOVED_FORM/IN_MOVED_TO事件统称
		IN_CLOSE	*		IN_CLOSE_WRITE/IN_CLOSE_NOWRITE事件统称
		IN_DONT_FOLLOW	*		不对符号链接解引用
		IN_MASK_ADD	*		将事件追加到当前掩码
		IN_ONESHOT	*		只监控一个事件
		IN_ONLYDIR	*		pathname不为目录时会失败
		IN_IGNORED		*	监控项为内核或应用所移除
		IN_ISDIR		*	name中所返回的文件为路径
		IN_Q_OVERFLOW		*	事件队列溢出
		IN_UNMOUNT		*	包含对象的文件系统遭卸载
		————————————————
		*/
		static const char* event_str[EVENT_NUM] =
		{
			"IN_ACCESS",
			"IN_MODIFY",
			"IN_ATTRIB",
			"IN_CLOSE_WRITE",
			"IN_CLOSE_NOWRITE",
			"IN_OPEN",
			"IN_MOVED_FROM",
			"IN_MOVED_TO",
			"IN_CREATE",
			"IN_DELETE",
			"IN_DELETE_SELF",
			"IN_MOVE_SELF"
		};
		int fd = _fd;
		int wd;
		int len;
		int nread;
		char buf[BUFSIZ];
		struct inotify_event* event;

		if (fd < 0)
		{
			fprintf(stderr, "inotify_init failed\n");
			return -1;
		}
		for (auto it : pathname)
		{
			wd = inotify_add_watch(fd, it.c_str(), mask);

			if (wd < 0)
			{
				//fprintf(stderr, "inotify_add_watch %s failed\n", it.c_str());
				//return -1;
			}
			else
			{
				_wds.push_back(wd);
			}
		}

		buf[sizeof(buf) - 1] = 0;
		int rt = 0;
		while ((len = read(fd, buf, sizeof(buf) - 1)) > 0)
		{
			nread = 0;
			while (len > 0)
			{
				event = (struct inotify_event*)&buf[nread];
				for (int i = 0; i < EVENT_NUM; i++)
				{
					if ((event->mask >> i) & 1)
					{
						if (_func)
						{
							rt = _func(event->name, event->len, event->mask);
						}
						if (event->len > 0)
							fprintf(stdout, "%s --- %s\n", event->name, event_str[i]);
						else
							fprintf(stdout, "%s --- %s\n", " ", event_str[i]);
					}
				}
				nread = nread + sizeof(struct inotify_event) + event->len;
				len = len - sizeof(struct inotify_event) - event->len;
			}
			if (rt)
			{
				break;
			}
		}
		for (auto it : _wds)
		{
			if (it)
				inotify_rm_watch(_fd, it);
		}
		return 0;
	}

#endif 	// _WIN32

	void inotify_d::set_func(std::function<int(const char* name, int nlen, uint32_t m)> func)
	{
		_func = func;
	}


}//!hz
