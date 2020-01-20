#ifndef __hl_time__h__
#define __hl_time__h__
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <iomanip>
#include <sstream>
#include <chrono>

namespace hz {
#define TM_YEAR_BASE 1900

	/*
	* We do not implement alternate representations. However, we always
	* check whether a given modifier is allowed for a certain conversion.
	*/
#define ALT_E     0x01
#define ALT_O     0x02
#define LEGAL_ALT(x)    { if (alt_format & ~(x)) return (0); }

	class times
	{
	public:
		times()
		{
		}

		~times()
		{
		}

	private:



		//static  int conv_num(const char **, int *, int, int);

	public:

		//window上自己实现strptime函数，linux已经提供strptime
		//strptime函数windows平台上实现
		static char* strptime(const char* buf, const char* fmt, struct tm* tm)
		{
			static const char* day[7] = {
				"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
				"Friday", "Saturday"
			};
			static const char* abday[7] = {
				"Sun","Mon","Tue","Wed","Thu","Fri","Sat"
			};
			static const char* mon[12] = {
				"January", "February", "March", "April", "May", "June", "July",
				"August", "September", "October", "November", "December"
			};
			static const char* abmon[12] = {
				"Jan", "Feb", "Mar", "Apr", "May", "Jun",
				"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
			};
			static const char* am_pm[2] = {
				"AM", "PM"
			};
			char c;
			const char* bp;
			size_t len = 0;
			int alt_format, i, split_year = 0;

			bp = buf;

			while ((c = *fmt) != '\0') {
				/* Clear `alternate' modifier prior to new conversion. */
				alt_format = 0;

				/* Eat up white-space. */
				if (isspace(c)) {
					while (isspace(*bp))
						bp++;

					fmt++;
					continue;
				}

				if ((c = *fmt++) != '%')
					goto literal;


			again:    switch (c = *fmt++) {
			case '%': /* "%%" is converted to "%". */
				literal :
					if (c != *bp++)
						return (0);
				break;

				/*
				* "Alternative" modifiers. Just set the appropriate flag
				* and start over again.
				*/
			case 'E': /* "%E?" alternative conversion modifier. */
				LEGAL_ALT(0);
				alt_format |= ALT_E;
				goto again;

			case 'O': /* "%O?" alternative conversion modifier. */
				LEGAL_ALT(0);
				alt_format |= ALT_O;
				goto again;

				/*
				* "Complex" conversion rules, implemented through recursion.
				*/
			case 'c': /* Date and time, using the locale's format. */
				LEGAL_ALT(ALT_E);
				if (!(bp = strptime(bp, "%x %X", tm)))
					return (0);
				break;

			case 'D': /* The date as "%m/%d/%y". */
				LEGAL_ALT(0);
				if (!(bp = strptime(bp, "%m/%d/%y", tm)))
					return (0);
				break;

			case 'R': /* The time as "%H:%M". */
				LEGAL_ALT(0);
				if (!(bp = strptime(bp, "%H:%M", tm)))
					return (0);
				break;

			case 'r': /* The time in 12-hour clock representation. */
				LEGAL_ALT(0);
				if (!(bp = strptime(bp, "%I:%M:%S %p", tm)))
					return (0);
				break;

			case 'T': /* The time as "%H:%M:%S". */
				LEGAL_ALT(0);
				if (!(bp = strptime(bp, "%H:%M:%S", tm)))
					return (0);
				break;

			case 'X': /* The time, using the locale's format. */
				LEGAL_ALT(ALT_E);
				if (!(bp = strptime(bp, "%H:%M:%S", tm)))
					return (0);
				break;

			case 'x': /* The date, using the locale's format. */
				LEGAL_ALT(ALT_E);
				if (!(bp = strptime(bp, "%m/%d/%y", tm)))
					return (0);
				break;

				/*
				* "Elementary" conversion rules.
				*/
			case 'A': /* The day of week, using the locale's form. */
			case 'a':
				LEGAL_ALT(0);
				for (i = 0; i < 7; i++) {
					/* Full name. */
					len = strlen(day[i]);
					if (strncmp(day[i], bp, len) == 0)
						break;

					/* Abbreviated name. */
					len = strlen(abday[i]);
					if (strncmp(abday[i], bp, len) == 0)
						break;
				}

				/* Nothing matched. */
				if (i == 7)
					return (0);

				tm->tm_wday = i;
				bp += len;
				break;

			case 'B': /* The month, using the locale's form. */
			case 'b':
			case 'h':
				LEGAL_ALT(0);
				for (i = 0; i < 12; i++) {
					/* Full name. */
					len = strlen(mon[i]);
					if (strncmp(mon[i], bp, len) == 0)
						break;

					/* Abbreviated name. */
					len = strlen(abmon[i]);
					if (strncmp(abmon[i], bp, len) == 0)
						break;
				}

				/* Nothing matched. */
				if (i == 12)
					return (0);

				tm->tm_mon = i;
				bp += len;
				break;

			case 'C': /* The century number. */
				LEGAL_ALT(ALT_E);
				if (!(conv_num(&bp, &i, 0, 99)))
					return (0);

				if (split_year) {
					tm->tm_year = (tm->tm_year % 100) + (i * 100);
				}
				else {
					tm->tm_year = i * 100;
					split_year = 1;
				}
				break;

			case 'd': /* The day of month. */
			case 'e':
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_mday, 1, 31)))
					return (0);
				break;

			case 'k': /* The hour (24-hour clock representation). */
				LEGAL_ALT(0);
				/* FALLTHROUGH */
			case 'H':
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_hour, 0, 23)))
					return (0);
				break;

			case 'l': /* The hour (12-hour clock representation). */
				LEGAL_ALT(0);
				/* FALLTHROUGH */
			case 'I':
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_hour, 1, 12)))
					return (0);
				if (tm->tm_hour == 12)
					tm->tm_hour = 0;
				break;

			case 'j': /* The day of year. */
				LEGAL_ALT(0);
				if (!(conv_num(&bp, &i, 1, 366)))
					return (0);
				tm->tm_yday = i - 1;
				break;

			case 'M': /* The minute. */
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_min, 0, 59)))
					return (0);
				break;

			case 'm': /* The month. */
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &i, 1, 12)))
					return (0);
				tm->tm_mon = i - 1;
				break;

			case 'p': /* The locale's equivalent of AM/PM. */
				LEGAL_ALT(0);
				/* AM? */
				if (strcmp(am_pm[0], bp) == 0) {
					if (tm->tm_hour > 11)
						return (0);

					bp += strlen(am_pm[0]);
					break;
				}
				/* PM? */
				else if (strcmp(am_pm[1], bp) == 0) {
					if (tm->tm_hour > 11)
						return (0);

					tm->tm_hour += 12;
					bp += strlen(am_pm[1]);
					break;
				}

				/* Nothing matched. */
				return (0);

			case 'S': /* The seconds. */
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_sec, 0, 61)))
					return (0);
				break;

			case 'U': /* The week of year, beginning on sunday. */
			case 'W': /* The week of year, beginning on monday. */
				LEGAL_ALT(ALT_O);
				/*
				* XXX This is bogus, as we can not assume any valid
				* information present in the tm structure at this
				* point to calculate a real value, so just check the
				* range for now.
				*/
				if (!(conv_num(&bp, &i, 0, 53)))
					return (0);
				break;

			case 'w': /* The day of week, beginning on sunday. */
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_wday, 0, 6)))
					return (0);
				break;

			case 'Y': /* The year. */
				LEGAL_ALT(ALT_E);
				if (!(conv_num(&bp, &i, 0, 9999)))
					return (0);

				tm->tm_year = i - TM_YEAR_BASE;
				break;

			case 'y': /* The year within 100 years of the epoch. */
				LEGAL_ALT(ALT_E | ALT_O);
				if (!(conv_num(&bp, &i, 0, 99)))
					return (0);

				if (split_year) {
					tm->tm_year = ((tm->tm_year / 100) * 100) + i;
					break;
				}
				split_year = 1;
				if (i <= 68)
					tm->tm_year = i + 2000 - TM_YEAR_BASE;
				else
					tm->tm_year = i + 1900 - TM_YEAR_BASE;
				break;

				/*
				* Miscellaneous conversions.
				*/
			case 'n': /* Any kind of white-space. */
			case 't':
				LEGAL_ALT(0);
				while (isspace(*bp))
					bp++;
				break;


			default:  /* Unknown/unsupported conversion. */
				return (0);
			}


			}

			/* LINTED functional specification */
			return ((char*)bp);
		}


		static int conv_num(const char** buf, int* dest, int llim, int ulim)
		{
			int result = 0;

			/* The limit also determines the number of valid digits. */
			int rulim = ulim;

			if (**buf < '0' || **buf > '9')
				return (0);

			do {
				result *= 10;
				result += *(*buf)++ - '0';
				rulim /= 10;
			} while ((result * 10 <= ulim) && rulim && **buf >= '0' && **buf <= '9');

			if (result < llim || result > ulim)
				return (0);

			*dest = result;
			return (1);
		}
		static char* strptime_(const char* s,
			const char* f,
			struct tm* tm) {
			// Isn't the C++ standard lib nice? std::get_time is defined such that its
			// format parameters are the exact same as strptime. Of course, we have to
			// create a string stream first, and imbue it with the current C locale, and
			// we also have to make sure we return the right things if it fails, or
			// if it succeeds, but this is still far simpler an implementation than any
			// of the versions in any of the C standard libraries.
			std::istringstream input(s);
			input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
			input >> std::get_time(tm, f);
			if (input.fail()) {
				return nullptr;
			}
			return (char*)(s + input.tellg());
		}
		//获取时间
		static std::string getFormat(time_t t, const std::string& fmt = "%Y-%m-%d %H:%M:%S", bool ms = false)
		{
			struct tm* p;
			if (ms)
				t *= 0.001;
			p = gmtime(&t);
			char s[80] = { 0 };
			//strftime(s, 80, "%Y-%m-%d %H:%M:%S::%Z", p);
			//printf("%d: %s\n", (int)t, jsont::AtoA(s).c_str());
			t += 28800;
			p = gmtime(&t);
			strftime(s, 80, fmt.c_str(), p);
			//printf("%d: %s\n", (int)t, jsont::AtoA(s).c_str());
			return s;
		}
		static std::string getFormat(struct tm* p, const std::string& fmt = "%Y-%m-%d %H:%M:%S")
		{
			char s[80] = { 0 };
			strftime(s, 80, fmt.c_str(), p);
			return s;
		}

#define ARDRONE_DATE_MAXSIZE    32
#define ARDRONE_FILE_DATE_FORMAT  "%Y%m%d_%H%M%S"
#define ARDRONE_DEFAULT_DATE        "19700101_000000"

		void yunshouhu_time2date(time_t time, const char* format, char* date)
		{
			if (date)
			{
				struct tm* tm = localtime(&time);
				strcpy(date, ARDRONE_DEFAULT_DATE);
				if (tm != NULL)
					strftime(date, ARDRONE_DATE_MAXSIZE, format ? format : ARDRONE_FILE_DATE_FORMAT, tm);
			}
		}

		void yunshouhu_date2time(char* date, const char* format, time_t* time)
		{
			struct tm tm;

			if (date != NULL)
			{
				*time = 0;
				if (strptime(date, (NULL != format) ? format : ARDRONE_FILE_DATE_FORMAT, &tm) != NULL)
					*time = mktime(&tm);
			}
		}

		//https://github.com/phdesign/pebble-phd/blob/f72313800357fd509def6abdde379067438fb3c1/src/utils.c
		char* format_date_time(time_t datetime, const char* format) {
			struct tm* temp_time = localtime(&datetime);
			static char str_time[512] = { 0 };
			strftime(str_time, sizeof(str_time), format, temp_time);
			return str_time;
		}

		int main(int argc, char const* argv[])
		{
			time_t nowTime = ::time(NULL);
			char line[1024] = { 0 };
			const char* format = "%Y-%m-%d %H:%M:%S";
			yunshouhu_time2date(nowTime, format, line);
			printf("%s\n", line);

			time_t secondTime;
			strcpy(line, "2018-06-29 23:20:30");
			yunshouhu_date2time(line, format, &secondTime);

			printf("ctime is %s\n", ctime(&secondTime));//得到日历时间

			yunshouhu_time2date(nowTime, NULL, line);
			printf("%s\n", line);

			char* data = format_date_time(::time(NULL), (char*)"%Y-%m-%d %H:%M:%S");
			printf("%s\n", data);

			const char* format2 = "%Y year %m month %d date %H hour %M minute %S seconds ";//还不支持中文 要使用unicode编码的中文
			data = format_date_time(::time(NULL), format2);
			printf("%s\n", data);

			format2 = "%Y\u5e74%m\u6708%d\u65e5 %H\u65f6%M\u5206%S\u79d2";//还不支持中文 要使用unicode编码的中文
			data = format_date_time(::time(NULL), format2);
			printf("%s\n", data);


			return 0;
		}
	};

	//---------------------------------------------------------------------------------------------------
	/*

	函数strftime()的操作有些类似于sprintf()：识别以百分号(%)开始的格式命令集合，格式化输出结果放在一个字符串中。
	格式化命令说明串strDest中各种日期和时间信息的确切表示方法。格式串中的其他字符原样放进串中。格式命令列在下面，它们是区分大小写的。

	%a 星期几的简写
	%A 星期几的全称
	%b 月分的简写
	%B 月份的全称
	%c 标准的日期的时间串
	%C 年份的后两位数字
	%d 十进制表示的每月的第几天
	%D 月/天/年
	%e 在两字符域中，十进制表示的每月的第几天
	%F 年-月-日
	%g 年份的后两位数字，使用基于周的年
	%G 年分，使用基于周的年
	%h 简写的月份名
	%H 24小时制的小时
	%I 12小时制的小时
	%j 十进制表示的每年的第几天
	%m 十进制表示的月份
	%M 十时制表示的分钟数
	%n 新行符
	%p 本地的AM或PM的等价显示
	%r 12小时的时间
	%R 显示小时和分钟：hh:mm
	%S 十进制的秒数
	%t 水平制表符
	%T 显示时分秒：hh:mm:ss
	%u 每周的第几天，星期一为第一天 （值从0到6，星期一为0）
	%U 第年的第几周，把星期日做为第一天（值从0到53）
	%V 每年的第几周，使用基于周的年
	%w 十进制表示的星期几（值从0到6，星期天为0）
	%W 每年的第几周，把星期一做为第一天（值从0到53）
	%x 标准的日期串
	%X 标准的时间串
	%y 不带世纪的十进制年份（值从0到99）
	%Y 带世纪部分的十进制年份
	%z，%Z 时区名称，如果不能得到时区名称则返回空字符。
	%% 百分号
	*/
	class Date
	{
		class tm_0
		{
		public:
			int tm_sec = 0;   // 秒seconds after the minute - [0, 60] including leap second
			int tm_min = 0;   // 分minutes after the hour - [0, 59]
			int tm_hour = 0;  // 时hours since midnight - [0, 23]
			int tm_mday = 1;  // 日day of the month - [1, 31]
			int tm_mon = 0;   // 月months since January - [0, 11]
			int tm_year = 0;  // 年years since 1900
			int tm_wday = 0;  // 星期days since Sunday - [0, 6]
			int tm_yday = 0;  // 第几天days since January 1 - [0, 365]
			int tm_isdst = 0; // daylight savings time flag
		};

	private:
		std::string _str, _fmt = "%Y-%m-%d %H:%M:%S";
		struct tm _p = { 0 };
		// 毫秒级
		time_t _t = 0;
		// 保存毫秒
		time_t _milliseconds = 0;

	public:
		Date()
		{
		}

		Date(const std::string& dstr, const std::string fmt = "")
		{
			_fmt = fmt;
			_t = getDate2Time(dstr, "", (struct tm*) & _p, true, &_fmt);
			_str = getFormat(_t, _fmt, true, (struct tm*) & _p);
		}
		Date(time_t t, const std::string fmt = "%Y-%m-%d %H:%M:%S") :_t(t)
		{
			_fmt = fmt;
			_str = getFormat(_t, fmt, true, (struct tm*) & _p);
			_t = ::mktime((struct tm*) & _p) * 1000;
		}
		virtual ~Date()
		{
		}
	public:
	public:
		const char* getFmt()
		{
			return _fmt.c_str();
		}

		void set(time_t t, bool isms = true)
		{
			int tms[] = { sizeof(struct tm),sizeof(struct tm_0) };
			_t = t;
			_str = getFormat(t, "%Y-%m-%d %H:%M:%S", isms, (struct tm*) & _p);
			if (!isms)
			{
				_t *= 1000;
			}
			_milliseconds = _t % 1000;
		}
		void set(const std::string& dstr)
		{
			_str = dstr;
			_t = getDate2Time(dstr, "", (struct tm*) & _p, true, &_fmt);
			_milliseconds = _t % 1000;
		}
		template <typename T>
		T get()
		{
			return (T)*this;
		}
		const char* c_str() const
		{
			return _str.c_str();
		}
		std::string str() const
		{
			return _str.c_str();
		}
		std::string getYM()const
		{
			return _str.substr(0, 7);
		}

#if 1
		int64_t getDate() const//从 Date 对象返回一个月中的某一天 (1 ~ 31)。
		{
			return _p.tm_mday;
		}
		int64_t getWDay()const //从 Date 对象返回一周中的某一天 (0 ~ 6)。
		{
			return _p.tm_wday;
		}
		int64_t getMonth() const//从 Date 对象返回月份 (0 ~ 11)。
		{
			return _p.tm_mon;
		}
		int64_t getFullYear()const //从 Date 对象以四位数字返回年份。
		{
			return _p.tm_year + 1900;
		}
		int64_t getYear() const//请使用 getFullYear( ) 方法代替。
		{
			return _p.tm_year;
		}
		int64_t getHours() const//返回 Date 对象的小时 (0 ~ 23)。
		{
			return _p.tm_hour;
		}
		int64_t getMinutes() const//返回 Date 对象的分钟 (0 ~ 59)。
		{
			return _p.tm_min;
		}
		int64_t getSeconds()const //返回 Date 对象的秒数 (0 ~ 59)。
		{
			return _p.tm_sec;
		}
		int64_t getMilliseconds()const //返回 Date 对象的毫秒(0 ~ 999)。
		{
			return std::max(_t % 1000, _milliseconds);
		}
		int64_t getTime()const //返回 1970 年 1 月 1 日至今的毫秒数。
		{
			return _t;
		}
		uint64_t getTimezoneOffset() //返回本地时间与格林威治标准时间 (GMT) 的分钟差。
		{
			return 0;
		}
		uint64_t getUTCDate() //根据世界时从 Date 对象返回月中的一天 (1 ~ 31)。
		{
			return 0;
		}
		uint64_t getUTCDay() //根据世界时从 Date 对象返回周中的一天 (0 ~ 6)。
		{
			return 0;
		}
		uint64_t getUTCMonth() //根据世界时从 Date 对象返回月份 (0 ~ 11)。
		{
			return 0;
		}
		uint64_t getUTCFullYear() //根据世界时从 Date 对象返回四位数的年份。
		{
			return 0;
		}
		uint64_t getUTCHours() //根据世界时返回 Date 对象的小时 (0 ~ 23)。
		{
			return 0;
		}
		uint64_t getUTCMinutes() //根据世界时返回 Date 对象的分钟 (0 ~ 59)。
		{
			return 0;
		}
		uint64_t getUTCSeconds() //根据世界时返回 Date 对象的秒钟 (0 ~ 59)。
		{
			return 0;
		}
		uint64_t getUTCMilliseconds() //根据世界时返回 Date 对象的毫秒(0 ~ 999)。
		{
			return 0;
		}
		const char* parse() //返回1970年1月1日午夜到指定日期（字符串）的毫秒数。
		{
			return _str.c_str();
		}
		uint64_t setDay(uint64_t s, bool ism = true) //设置 Date 对象中月的某一天 (1 ~ 31)。
		{
			int cm = get_month_days(_p.tm_year + 1900, _p.tm_mon + 1);
			s = std::max(std::min((int)s, 31), 1);
			_p.tm_mday = s;
			if (ism)
			{
				make();
			}
			return 0;
		}
		uint64_t incDay(uint64_t s, bool ism = true) //设置 Date 对象中月的某一天 (1 ~ 31)。
		{
			auto n = Date::dateChange("2018-01-20", s);
			time_t t1 = _t / 1000;
			struct tm tmin = { 0 };
			struct tm* tminp = &tmin;
			struct tm tmout = { 0 };
			struct tm* tmoutp = &tmout;

			/*将tm结构数据转换成1970年1月1日开始计算的秒数*/
			//t1 = mktime(tminp);
			/*计算需要增加或者减少天数对应的秒数，结果是最终日期对应1970年1月1日开始计算的秒数*/
			t1 += s * 60 * 60 * 24;
			set(t1, false);
			/*将time_t的信息转化真实世界的时间日期表示，结果由结构tm返回*/
			//tmoutp = localtime(&t1);
			return 0;
		}
		uint64_t setMonth(uint64_t s, bool ism = true) //设置 Date 对象中月份 (0 ~ 11)。
		{
			s = std::max(std::min((int)s, 11), 0);
			_p.tm_mon = s;
			if (ism)
			{
				make();
			}
			return 0;
		}
		uint64_t setFullYear(uint64_t s, bool ism = true) //设置 Date 对象中的年份（四位数字）。
		{
			_p.tm_year = s - 1900;
			if (ism)make();
			return 0;
		}
		uint64_t setYear(uint64_t s, bool ism = true) //请使用 setFullYear( ) 方法代替。
		{
			_p.tm_year = s;
			if (ism)make();
			return 0;
		}
		uint64_t setHours(uint64_t s) //设置 Date 对象中的小时 (0 ~ 23)。
		{
			_p.tm_hour = s; make();
			return 0;
		}
		uint64_t setMinutes(uint64_t s) //设置 Date 对象中的分钟 (0 ~ 59)。
		{
			_p.tm_min = s; make();
			return 0;
		}
		uint64_t setSeconds(uint64_t s) //设置 Date 对象中的秒钟 (0 ~ 59)。
		{
			_p.tm_sec = s; make();
			return 0;
		}
		uint64_t setMilliseconds(uint64_t s) //设置 Date 对象中的毫秒 (0 ~ 999)。
		{
			_milliseconds = s;
			return 0;
		}
		uint64_t setTime(uint64_t s) //以毫秒设置 Date 对象。
		{
			set(s);
			return 0;
		}
		uint64_t setUTCDate(uint64_t s) //根据世界时设置 Date 对象中月份的一天 (1 ~ 31)。
		{
			return 0;
		}
		uint64_t setUTCMonth(uint64_t s) //根据世界时设置 Date 对象中的月份 (0 ~ 11)。
		{
			return 0;
		}
		uint64_t setUTCFullYear(uint64_t s) //根据世界时设置 Date 对象中的年份（四位数字）。
		{
			return 0;
		}
		uint64_t setUTCHours(uint64_t s) //根据世界时设置 Date 对象中的小时 (0 ~ 23)。
		{
			return 0;
		}
		uint64_t setUTCMinutes(uint64_t s) //根据世界时设置 Date 对象中的分钟 (0 ~ 59)。
		{
			return 0;
		}
		uint64_t setUTCSeconds(uint64_t s) //根据世界时设置 Date 对象中的秒钟 (0 ~ 59)。
		{
			return 0;
		}
		uint64_t setUTCMilliseconds(uint64_t s) //根据世界时设置 Date 对象中的毫秒 (0 ~ 999)。
		{
			return 0;
		}

#endif
	public:
		operator time_t() const
		{
			return _t;
		}
		// 		operator std::string() const
		// 		{
		// 			return _str;
		// 		}
#if 0
		operator const std::string& () const
		{
			return _str;
		}
#ifdef _WIN32 
		operator const char* () const
		{
			return _str.c_str();
		}
#endif
#endif
		operator struct tm* () const
		{
			return (struct tm*) & _p;
		}
		/*
		*Durations:时长
		const nMS = 1320; //以毫秒单位表示的差值时间
		var nD = Math.floor(nMS/(1000 * 60 * 60 * 24));
		var nH = Math.floor(nMS/(1000*60*60)) % 24;
		var nM = Math.floor(nMS/(1000*60)) % 60;
		var nS = Math.floor(nMS/1000) % 60;
		**/
		void make()
		{
			_str = getFormat((struct tm*) & _p, _fmt.c_str());
			_t = ::mktime((struct tm*) & _p) * 1000;
		}

		static int calc_week_day(int y, int m, int d)
		{
			if (m == 1 || m == 2)
			{
				m += 12;
				y--;
			}
			int iWeek = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) % 7;
			iWeek++;
			// 1-7 "星期一","星期二","星期三","星期四","星期五","星期六","星期日",;
			return iWeek;
		}
		// 获取一个月天数
		static int get_month_days(int y, int m)
		{
			int d;
			int day[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
			if (m < 1)
			{
				m = 1;
			}
			if (2 == m)
			{
				d = (((0 == y % 4) && (0 != y % 100) || (0 == y % 400)) ? 29 : 28);
			}
			else
			{
				if (m > 12)
					m = m % 12;
				d = day[m - 1];
			}
			return d;
		}
		static std::string dateChange(const char* cDateIn, int n)
		{
			time_t t1 = 0;
			struct tm tmin = { 0 };
			struct tm* tminp = &tmin;
			struct tm tmout = { 0 };
			struct tm* tmoutp = &tmout;

			char tmp[8 + 32];
			memset(tmp, 0, sizeof(tmp));

			hz::times::strptime_(cDateIn, "%Y-%m-%d", tminp);

			/*将tm结构数据转换成1970年1月1日开始计算的秒数*/
			t1 = mktime(tminp);
			/*计算需要增加或者减少天数对应的秒数，结果是最终日期对应1970年1月1日开始计算的秒数*/
			t1 += n * 60 * 60 * 24;
			/*将time_t的信息转化真实世界的时间日期表示，结果由结构tm返回*/
			tmoutp = localtime(&t1);

			/*tm类型的时间转换。将tm按照%Y%m%d格式转化赋值到输出中，最大长度8+1*/
			strftime(tmp, 8 + 32, "%Y-%m-%d", tmoutp);
			return tmp;
		}

		static int daystotal(int y, int m, int d)
		{
			static char daytab[2][13] =
			{
				{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
				{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
			};
			int daystotal = d;
			for (int year = 1; year <= y; year++)
			{
				int max_month = (year < y ? 12 : m - 1);
				int leap = (year % 4 == 0);
				if (year % 100 == 0 && year % 400 != 0)
					leap = 0;
				for (int month = 1; month <= max_month; month++)
				{
					daystotal += daytab[leap][month];
				}
			}
			return daystotal;
		}
		// 获取本星期日期
		std::vector<std::string> get_wday_days()
		{
			std::vector<std::string> ret;
			Date ad = *this;
			int wds = ad.getWDay();
			ad.incDay(-wds);
			for (int i = 0; i < 7; i++)
			{
				ad.incDay(1);
				ret.push_back(ad._str.substr(0, 10));
			}
			return ret;
		}
#if 0
		for (int yy = 0; yy < 10; yy++)
		{
			printf("%d\t", 2015 + yy);
			for (int i = 0; i < 12; i++)
			{
				printf("%d->%d\t", i + 1, Date::get_month_days(2015 + yy, i + 1));
			}
			printf("\n");
		}
		std::map<int, int> wd;
		for (int i = 0; i < 20; i++)
		{
			wd[15 + i] = (Date::calc_week_day(2018, 11, 15 + i));
		}
#endif
	public:

		//获取时间字符串
		static std::string getFormat(time_t t, const std::string& fmt, bool ms, struct tm* outtm = nullptr, time_t utc = 8)
		{
			struct tm* p;
			struct tm p1 = {};

			if (ms)
				t *= 0.001;
			char s[80] = { 0 };
			//p = gmtime(&t);
			//strftime(s, 80, "%Y-%m-%d %H:%M:%S::%Z", p);
			//printf("%d: %s\n", (int)t, jsont::AtoA(s).c_str());
			t += utc * 60 * 60;
			p = gmtime(&t);
			strftime(s, 80, fmt.c_str(), p);

			auto nt = getDate2Time(s, "", (struct tm*) & p1, true);
			if (outtm)
			{
				memcpy(outtm, &p1, sizeof(struct tm));
			}
			//printf("%d: %s\n", (int)t, jsont::AtoA(s).c_str());
			return s;
		}
		static std::string getFormat(struct tm* p, const std::string& fmt)
		{
			char s[256] = { 0 };
			strftime(s, 256, fmt.c_str(), p);
			return s;
		}
		//获取本机时间
		static std::string getDateNow()
		{
			time_t t = 0;
			::time(&t);
			return getFormat(t, "%Y-%m-%d %H:%M:%S", false);
		}
		static std::string Now(std::string fmt = "%Y-%m-%d %H:%M:%S")
		{
			time_t t = 0;
			::time(&t);
			return getFormat(t, fmt, false);
		}
		static Date now(std::string fmt = "%Y-%m-%d %H:%M:%S")
		{
			return Now(fmt);
		}
		static uint64_t getDate2Time(const std::string& s, std::string fmt = "", struct tm* stms = nullptr, bool isms = true, std::string* ofmt = 0)
		{
			static std::string sfmt = "%Y-%m-%d %H:%M:%S";
			if (fmt.empty())
			{
				std::string chs = " ";
				size_t y = 0;
				if (s.find(':') != -1)
				{
					y = 8;
					chs.push_back(':');
				}
				if (s.find('-') != -1)
				{
					y = 0;
					chs.push_back('-');
				}
				auto ms = hstring::split_m(s, "- :");
				fmt = sfmt.substr(y, ms.size() * 3 - 1);
			}
			struct tm stmss = { 0 };
			if (!stms)
			{
				stms = &stmss;
			}
			hz::times::strptime_((char*)s.c_str(), fmt.c_str(), stms);
			uint64_t tt = ::mktime(stms);
			if (isms)
			{
				tt *= 1000;
			}
			if (ofmt)
			{
				*ofmt = fmt;
			}
			return tt;
		}
		static int64_t dateDiff(const Date& _left, const Date& _right, char strInterval)
		{
			double tempDate = 0;
			char ch = strInterval;
			static std::string ms = "snhdwmy";
			if (ms.find(strInterval) == std::string::npos)
			{
				ch = _left.get2_ch(_right);
			}
			switch (ch) {
			case 's': // 秒
				tempDate = _left.getTime() / 1000 - _right.getTime() / 1000;
				break;
			case 'n': // 分
				tempDate = _left.getTime() / 60000 - _right.getTime() / 60000;
				break;
			case 'h': // 时
				tempDate = _left.getTime() / 3600000 - _right.getTime() / 3600000;
				break;
			case 'd': // 天
			{
				auto t1 = Date(_left.getTime(), "%Y-%m-%d");
				auto t2 = Date(_right.getTime(), "%Y-%m-%d");
				tempDate = difftime(t1, t2) / 86400000;
				break;
			}
			case 'w': // 周
				tempDate = _left.getTime() - _right.getTime();
				tempDate /= (86400000 * 7);
				break;
			case 'm': // 月
				tempDate = ((_left.getMonth() + 1) + ((_left.getFullYear() - _right.getFullYear()) * 12) - (_right.getMonth() + 1));
				break;
			case 'y': // 年
				tempDate = (_left.getFullYear() - _right.getFullYear());
				break;
			}
			return tempDate;
		}

		static int64_t dateDiff_s(const std::string& _left, const std::string& _right, char strInterval)
		{
			return dateDiff(Date(_left), Date(_right), strInterval);
		}

		static std::string get_n(const Date& d)
		{
			auto v = hstring::split_m(d.str(), "- :");
			std::string ret;
			for (auto& it : v)
				ret += it;
			return ret;
		}

		//%Y4-%m2-%d2 %H2:%M2:%S2
		char get2_ch(const Date& r) const
		{
			char ret = 's';
			auto s = std::min(get_n(*this).size(), get_n(r).size());
			if (s >= 14)
			{
				ret = 's';
			}
			else if (s >= 12)
			{
				ret = 'n';
			}
			else if (s >= 10)
			{
				ret = 'h';
			}
			else if (s >= 8)
			{
				ret = 'd';
			}
			else if (s >= 6)
			{
				ret = 'm';
			}
			else if (s >= 4)
			{
				ret = 'y';
			}
			return ret;
		}
		int64_t compare(const std::string& right)const
		{
			return dateDiff(*this, right, 0);
		}
		int64_t compare(const Date& right)const
		{
			return dateDiff(*this, right, 0);
		}
		// 等于
		bool operator==(const std::string& right) {
			return compare(right) == 0;
		}
		bool operator==(const char* const right) {
			return compare(right) == 0;
		}
		bool operator==(const Date& right) {
			return compare(right) == 0;
		}
		// 不等于
		bool operator!=(const std::string& right) {
			return compare(right) != 0;
		}
		bool operator!=(const char* const right) {
			return compare(right) != 0;
		}
		bool operator!=(const Date& right) {
			return compare(right) != 0;
		}
		// 小于
		bool operator<(const std::string& right) {
			return compare(right) < 0;
		}
		bool operator<(const char* const right) {
			return compare(right) < 0;
		}
		bool operator<(const Date& right) {
			return compare(right) < 0;
		}
		// 大于
		bool operator>(const std::string& right) {
			return compare(right) > 0;
		}
		bool operator>(const char* const right) {
			return compare(right) > 0;
		}
		bool operator>(const Date& right) {
			return compare(right) > 0;
		}
		// 获取网络时间
#if 0
		static std::string getDateNet(const std::string& url = "http://time.tianqi.com/")
		{
			nlohmann::json res;
			auto str = Net::get(url, &res);
			struct tm stms = { 0 };
			std::string ts;
			if (res.is_object() && res["headers"].is_object() && res["headers"]["Date"].is_string())
			{
				ts = res["headers"]["Date"].get<std::string>();
			}
			hz::time::strptime_((char*)ts.c_str(), "%a, %d %b %Y %T %Z", &stms);
			uint64_t tt = ::mktime(&stms);
			tt += 28800;
			return hz::time::getFormat(tt, "%Y-%m-%d %H:%M:%S", false);
		}
#endif
		/*判断是平年还是闰年*/
		static int IsLeap(int year) {
			if ((year % 4 == 0) && (year % 100 != 0) || (year % 400 == 0))
				return 1;
			else
				return 0;
		}
		/*计算今年的第一天是星期几*/
		static int FirstWeeday(int year) {
			int days = (year - 1) * 365 + (year - 1) / 4 - (year - 100) / 100 + (year - 1) / 400;
			return days % 7;
		}
		/*打印星期*/
		static void PrintWeedTitl() {
			int i;
			for (i = 0; i < 7; i++) {
				switch (i) {
				case 0: printf("Sun  "); break;
				case 1: printf("Mon  "); break;
				case 2: printf("Tue  "); break;
				case 3: printf("Wed  "); break;
				case 4: printf("Thu  "); break;
				case 5: printf("Fri  "); break;
				case 6: printf("Sat  "); break;
				default: break;
				}
			}
			printf("\n");
		}
		/*打印月份*/
		static void PrintMonTitl(int month) {
			static const char* mstr[] = { "Jan一", "Feb二", "Mar三", "Apr四",
				"May五", "Jun六", "Jul七", "Aug八", "Sep九", "Oct十", "Nov十一", "Dec十二", };
			if (month < 12 && month >= 0)
			{
				printf("%s月\n", mstr[month]);
			}
		}
		/*格式化输出日历*/
		static void PrintYear(int year) {
			int m, i, d;
			char c = ' ';
			int MONTH[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
			if (IsLeap(year)) MONTH[1] = 29;
			int firstwed = FirstWeeday(year);
			printf("Year: %d\n", year);
			printf("========================\n");
			for (m = 0; m < 12; m++) {
				PrintMonTitl(m);
				PrintWeedTitl();
				for (d = 1; d <= MONTH[m]; d++) {
					if (d == 1) {
						for (i = 0; i < firstwed; i++)
							printf("%5c", c);
						printf("%5d", d);
					}
					else {
						printf("%5d", d);
					}
					if ((firstwed + d - 1) % 7 == 6) printf("\n");
				}
				firstwed = (firstwed + MONTH[m]) % 7;
				printf("\n");
			}

		}
		int CaculateWeekDay(int y, int m, int d)
		{
			if (m == 1 || m == 2)
			{
				m += 12;
				y--;
			}
			int iWeek = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) % 7;
			switch (iWeek)
			{
			case 0:  "星期一";
			case 1:  "星期二";
			case 2:  "星期三";
			case 3:  "星期四";
			case 4:  "星期五";
			case 5:  "星期六";
			case 6:  "星期日";
			}
			return iWeek;
		}
	private:

	};
#if 0
	// 小于
	static bool operator<(const Date& left, const std::string& right) {
		return left.compare(right) < 0;
	}
	static bool operator<(const std::string& left, const Date& right) {
		return right.compare(left) > 0;
	}
	static bool operator<(const Date& left, const char* const right) {
		return left.compare(right) < 0;
	}
	static bool operator<(const char* const left, const Date& right) {
		return right.compare(left) > 0;
	}
	static bool operator<(const Date& left, const Date& right) {
		return left.compare(right) < 0;
	}
	// 大于
	static bool operator>(const Date& left, const std::string& right) {
		return left.compare(right) > 0;
	}
	static bool operator>(const std::string& left, const Date& right) {
		return right.compare(left) < 0;
	}
	static bool operator>(const Date& left, const char* const right) {
		return left.compare(right) > 0;
	}
	static bool operator>(const char* const left, const Date& right) {
		return right.compare(left) < 0;
	}
	static bool operator>(const Date& left, const Date& right) {
		return left.compare(right) > 0;
	}
	static bool operator==(const Date& _Left, const char* _Right) {
		return Date::dateDiff(_Left, _Right, 'c') == 0;
	}
	static bool operator==(const char* _Left, const Date& _Right) {
		return Date::dateDiff(_Left, _Right, 'c') == 0;
	}
	static bool operator==(const Date& _Left, const Date& _Right) {
		return Date::dateDiff(_Left, _Right, 'c') == 0;
	}

	static bool operator<(const Date& _Left, const char* _Right) {
		return Date::dateDiff(_Left, _Right, 'c') < 0;
	}
	static bool operator<(const char* _Left, const Date& _Right) {
		return Date::dateDiff(_Left, _Right, 'c') < 0;
	}
	static bool operator<(const Date& _Left, const Date& _Right) {
		return Date::dateDiff(_Left, _Right, 'c') < 0;
	}

	static bool operator>(const Date& _Left, const char* _Right) {
		return Date::dateDiff(_Left, _Right, 'c') > 0;
	}
	static bool operator>(const char* _Left, const Date& _Right) {
		return Date::dateDiff(_Left, _Right, 'c') > 0;
	}
	static bool operator>(const Date& _Left, const Date& _Right) {
		return Date::dateDiff(_Left, _Right, 'c') > 0;
	}
#endif
	/*
	Timer返回两个时间差
	reset函数重置

	void fun()
	{
	cout << ”hello word” << endl;
	}
	int main()
	{
	Timer t; //开始计时
	fun();
	cout << t.elapsed() << endl; //打印fun函数耗时多少毫秒
	cout << t.elapsed_micro() << endl; //打印微秒
	cout << t.elapsed_nano() << endl; //打印纳秒
	cout << t.elapsed_seconds() << endl; //打印秒
	cout << t.elapsed_minutes() << endl; //打印分钟
	cout << t.elapsed_hours() << endl; //打印小时
	}
	*/

	class Timer
	{
	public:
		std::time_t getTimeStamp()
		{
			std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::milliseconds> tp =
				std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now());
			return tp.time_since_epoch().count();
		}
	public:
		Timer() : m_begin(std::chrono::high_resolution_clock::now()) {}
		void reset() { m_begin = std::chrono::high_resolution_clock::now(); }
#if 0
		//默认输出毫秒
		uint64_t elapsed() const
		{
			return (std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now())
				- std::chrono::time_point_cast<std::chrono::milliseconds>(m_begin)).count();
		}
		//微秒
		uint64_t elapsed_micro()const
		{
			return (std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now())
				- std::chrono::time_point_cast<std::chrono::microseconds>(m_begin)).count();
		}
		//纳秒
		uint64_t elapsed_nano()const
		{
			return (std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now())
				- std::chrono::time_point_cast<std::chrono::nanoseconds>(m_begin)).count();
		}
		//秒
		uint64_t elapsed_seconds()const
		{
			return (std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now())
				- std::chrono::time_point_cast<std::chrono::seconds>(m_begin)).count();
		}
		//分
		uint64_t elapsed_minutes()const
		{
			return (std::chrono::time_point_cast<std::chrono::minutes>(std::chrono::high_resolution_clock::now())
				- std::chrono::time_point_cast<std::chrono::minutes>(m_begin)).count();
		}
		//时
		uint64_t elapsed_hours()const
		{
#ifdef _WIN32
			return std::chrono::duration_cast<std::chrono::hours>(std::chrono::high_resolution_clock::now() - m_begin).count();
#else
			return (std::chrono::time_point_cast<std::chrono::hours>(std::chrono::high_resolution_clock::now())
				- std::chrono::time_point_cast<std::chrono::hours>(m_begin)).count();
#endif
		}
#else
		//默认输出毫秒
		uint64_t elapsed() const
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}
		//微秒
		uint64_t elapsed_micro() const
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}
		//纳秒
		uint64_t elapsed_nano() const
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}
		//秒
		uint64_t elapsed_seconds() const
		{
			return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}
		//分
		uint64_t elapsed_minutes() const
		{
			return std::chrono::duration_cast<std::chrono::minutes>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}
		//时
		uint64_t elapsed_hours() const
		{
			return std::chrono::duration_cast<std::chrono::hours>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}

#endif
#if 1
		//毫秒
		static uint64_t get_ms()
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}
		//微秒
		static uint64_t get_micro()
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}
		//纳秒
		static uint64_t get_nano()
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}
		//秒
		static uint64_t get_seconds()
		{
			return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}
		//分
		static uint64_t get_minutes()
		{
			return std::chrono::duration_cast<std::chrono::minutes>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}
		//时
		static uint64_t get_hours()
		{
			return std::chrono::duration_cast<std::chrono::hours>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}
		static uint64_t get_now()
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		}

		static std::string get_now_str(int rc = 0)
		{
			auto ut = std::chrono::system_clock::now().time_since_epoch().count();
			std::string ret = std::to_string(ut);
			if (rc > 0 && rc < ret.size())
			{
				ret = ret.substr(ret.size() - rc);
			}
			return ret;
		}

#endif
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;
	};
	class PTimer
	{
	public:
		PTimer(std::string s = "", int n = 0)
		{
			if (s != "")
			{
				str = s + "\t" + std::to_string(n);
			}
		}

		~PTimer()
		{
			double aa = t.elapsed_micro();
			aa *= 0.001;
			printf("time:\t\x1b[32;1m%lf\x1b[0m\t(\x1b[33m%s\x1b[0m)\t\n", aa, str.c_str());
		}

	private:
		hz::Timer t;
		std::string str = "time";
	};
#define PTIMER hz::PTimer _ptimer_s(__FUNCDNAME__,__LINE__)
#define PTIMER_STR(s) hz::PTimer _ptimer_s(s)
};//!hz
#endif //!__hl_time__h__
