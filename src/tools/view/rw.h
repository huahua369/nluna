#pragma once
namespace hz
{
	class rw_t
	{
	public:
		enum class seek_s
		{
			set = 0,
			cur = 1,
			end = 2
		};

	private:
		char* _data = 0;
		int64_t _pos = 0, _end = 0, _last_size = 0;

	public:
		int64_t tell()
		{
			return _pos;
		}
		int seek(int64_t _offset, int _origin = 0)
		{
			switch (_origin)
			{
			case 0:
				_pos = _offset;
				break;
			case 1:
				_pos += _offset;
				break;
			case 2:
				if (_offset > 0)
				{
					_offset = 0;
				}
				_pos = _end + _offset;
				break;
			default:
				break;
			}
			return _pos <= _end ? _pos : -1;
		}
		bool iseof()
		{
			return _pos >= _end;
		}
		virtual int64_t read(void* buf, int64_t len)
		{
			int64_t ret = 0;
			if (_end > 0 && buf)
			{
				char* d = (char*)_data;
				auto s = _end - _pos;
				ret = len;
				if (ret > s)
				{
					ret = s;
				}
				memcpy(buf, d + _pos, ret);
				_pos += ret;
				if (_pos > _end)
				{
					_pos = _end;
				}
			}
			return ret;
		}
		virtual int64_t write(const void* buf, int64_t len)
		{
			int64_t ret = 0, ns = _pos + len;
			if (_last_size < ns)
				_last_size = ns;
			if (_end < _last_size)
			{
				//resize(_last_size);
			}
			if (_end > 0 && buf)
			{
				char* d = (char*)_data;
				auto s = _end - _pos;
				ret = len;
				if (ret > s)
				{
					ret = s;
				}
				memcpy(d + _pos, buf, ret);
				_pos += ret;
				if (_pos > _end)
				{
					_pos = _end;
				}
			}
			return ret;
		}
		virtual int resize(int64_t s)
		{
			return 0;
		}
		int64_t size()
		{
			return _end;
		}
		void set_data(void* d)
		{
			_data = (char*)d;
		}
		void set_end(int64_t e)
		{
			_end = e;
		}
		void set(void* d, int64_t e)
		{
			_data = (char*)d;
			_end = e;
		}
		char* data() { return _data; }
	public:
		rw_t()
		{
		}
		rw_t(char* d, int64_t e) :_data(d), _end(e)
		{
		}

		virtual	~rw_t()
		{
		}

	private:

	};
}//!hz


#ifndef RWread


#define RWread(p,v,s,c) ((p)->read((void*)v,s*c))
#define RWtell(p) (p)->tell()
#define RWseek(p,pos,o) (p)->seek(pos,o)

#endif // !RWread
