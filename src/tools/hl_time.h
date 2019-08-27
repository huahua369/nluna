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

		//window���Լ�ʵ��strptime������linux�Ѿ��ṩstrptime
		//strptime����windowsƽ̨��ʵ��
		static char* strptime(const char *buf, const char *fmt, struct tm *tm)
		{
			static const char *day[7] = {
				"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
				"Friday", "Saturday"
			};
			static const char *abday[7] = {
				"Sun","Mon","Tue","Wed","Thu","Fri","Sat"
			};
			static const char *mon[12] = {
				"January", "February", "March", "April", "May", "June", "July",
				"August", "September", "October", "November", "December"
			};
			static const char *abmon[12] = {
				"Jan", "Feb", "Mar", "Apr", "May", "Jun",
				"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
			};
			static const char *am_pm[2] = {
				"AM", "PM"
			};
			char c;
			const char *bp;
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
			return ((char *)bp);
		}


		static int conv_num(const char **buf, int *dest, int llim, int ulim)
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
		//��ȡʱ��
		static std::string getFormat(time_t t, const std::string &fmt = "%Y-%m-%d %H:%M:%S", bool ms = false)
		{
			struct tm *p;
			if (ms)
				t *= 0.001;
			p = gmtime(&t);
			char s[80] = { 0 };
			//strftime(s, 80, "%Y-%m-%d %H:%M:%S::%Z", p);
			//printf("%d: %s\n", (int)t, jsonT::AtoA(s).c_str());
			t += 28800;
			p = gmtime(&t);
			strftime(s, 80, fmt.c_str(), p);
			//printf("%d: %s\n", (int)t, jsonT::AtoA(s).c_str());
			return s;
		}
		static std::string getFormat(struct tm *p, const std::string &fmt = "%Y-%m-%d %H:%M:%S")
		{
			char s[80] = { 0 };
			strftime(s, 80, fmt.c_str(), p);
			return s;
		}

#define ARDRONE_DATE_MAXSIZE    32
#define ARDRONE_FILE_DATE_FORMAT  "%Y%m%d_%H%M%S"
#define ARDRONE_DEFAULT_DATE        "19700101_000000"

		void yunshouhu_time2date(time_t time, const char *format, char *date)
		{
			if (date)
			{
				struct tm *tm = localtime(&time);
				strcpy(date, ARDRONE_DEFAULT_DATE);
				if (tm != NULL)
					strftime(date, ARDRONE_DATE_MAXSIZE, format ? format : ARDRONE_FILE_DATE_FORMAT, tm);
			}
		}

		void yunshouhu_date2time(char *date, const char *format, time_t *time)
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
		char* format_date_time(time_t datetime, const char *format) {
			struct tm *temp_time = localtime(&datetime);
			static char str_time[512] = { 0 };
			strftime(str_time, sizeof(str_time), format, temp_time);
			return str_time;
		}

		int main(int argc, char const *argv[])
		{
			time_t nowTime = ::time(NULL);
			char line[1024] = { 0 };
			const char* format = "%Y-%m-%d %H:%M:%S";
			yunshouhu_time2date(nowTime, format, line);
			printf("%s\n", line);

			time_t secondTime;
			strcpy(line, "2018-06-29 23:20:30");
			yunshouhu_date2time(line, format, &secondTime);

			printf("ctime is %s\n", ctime(&secondTime));//�õ�����ʱ��

			yunshouhu_time2date(nowTime, NULL, line);
			printf("%s\n", line);

			char* data = format_date_time(::time(NULL), (char*)"%Y-%m-%d %H:%M:%S");
			printf("%s\n", data);

			const char *format2 = "%Y year %m month %d date %H hour %M minute %S seconds ";//����֧������ Ҫʹ��unicode���������
			data = format_date_time(::time(NULL), format2);
			printf("%s\n", data);

			format2 = "%Y\u5e74%m\u6708%d\u65e5 %H\u65f6%M\u5206%S\u79d2";//����֧������ Ҫʹ��unicode���������
			data = format_date_time(::time(NULL), format2);
			printf("%s\n", data);


			return 0;
		}
	};

	//---------------------------------------------------------------------------------------------------
	/*

	����strftime()�Ĳ�����Щ������sprintf()��ʶ���԰ٷֺ�(%)��ʼ�ĸ�ʽ����ϣ���ʽ������������һ���ַ����С�
	��ʽ������˵����strDest�и������ں�ʱ����Ϣ��ȷ�б�ʾ��������ʽ���е������ַ�ԭ���Ž����С���ʽ�����������棬���������ִ�Сд�ġ�

	%a ���ڼ��ļ�д
	%A ���ڼ���ȫ��
	%b �·ֵļ�д
	%B �·ݵ�ȫ��
	%c ��׼�����ڵ�ʱ�䴮
	%C ��ݵĺ���λ����
	%d ʮ���Ʊ�ʾ��ÿ�µĵڼ���
	%D ��/��/��
	%e �����ַ����У�ʮ���Ʊ�ʾ��ÿ�µĵڼ���
	%F ��-��-��
	%g ��ݵĺ���λ���֣�ʹ�û����ܵ���
	%G ��֣�ʹ�û����ܵ���
	%h ��д���·���
	%H 24Сʱ�Ƶ�Сʱ
	%I 12Сʱ�Ƶ�Сʱ
	%j ʮ���Ʊ�ʾ��ÿ��ĵڼ���
	%m ʮ���Ʊ�ʾ���·�
	%M ʮʱ�Ʊ�ʾ�ķ�����
	%n ���з�
	%p ���ص�AM��PM�ĵȼ���ʾ
	%r 12Сʱ��ʱ��
	%R ��ʾСʱ�ͷ��ӣ�hh:mm
	%S ʮ���Ƶ�����
	%t ˮƽ�Ʊ��
	%T ��ʾʱ���룺hh:mm:ss
	%u ÿ�ܵĵڼ��죬����һΪ��һ�� ��ֵ��0��6������һΪ0��
	%U ����ĵڼ��ܣ�����������Ϊ��һ�죨ֵ��0��53��
	%V ÿ��ĵڼ��ܣ�ʹ�û����ܵ���
	%w ʮ���Ʊ�ʾ�����ڼ���ֵ��0��6��������Ϊ0��
	%W ÿ��ĵڼ��ܣ�������һ��Ϊ��һ�죨ֵ��0��53��
	%x ��׼�����ڴ�
	%X ��׼��ʱ�䴮
	%y �������͵�ʮ������ݣ�ֵ��0��99��
	%Y �����Ͳ��ֵ�ʮ�������
	%z��%Z ʱ�����ƣ�������ܵõ�ʱ�������򷵻ؿ��ַ���
	%% �ٷֺ�
	*/
	class Date
	{
		class tm_0
		{
		public:
			int tm_sec = 0;   // ��seconds after the minute - [0, 60] including leap second
			int tm_min = 0;   // ��minutes after the hour - [0, 59]
			int tm_hour = 0;  // ʱhours since midnight - [0, 23]
			int tm_mday = 1;  // ��day of the month - [1, 31]
			int tm_mon = 0;   // ��months since January - [0, 11]
			int tm_year = 0;  // ��years since 1900
			int tm_wday = 0;  // ����days since Sunday - [0, 6]
			int tm_yday = 0;  // �ڼ���days since January 1 - [0, 365]
			int tm_isdst = 0; // daylight savings time flag
		};

	private:
		std::string _str, _fmt = "%Y-%m-%d %H:%M:%S";
		struct tm _p = { 0 };
		// ���뼶
		time_t _t = 0;
		// �������
		time_t _milliseconds = 0;

	public:
		Date()
		{
		}

		Date(const std::string &dstr, const std::string fmt = "") :_str(dstr)
		{
			_t = getDate2Time(dstr, "", (struct tm*)&_p, true, &_fmt);
		}
		Date(time_t t, const std::string fmt = "%Y-%m-%d %H:%M:%S") :_t(t)
		{
			_fmt = fmt;
			_str = getFormat(_t, fmt, true, (struct tm*)&_p);
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
			_str = getFormat(t, "%Y-%m-%d %H:%M:%S", isms, (struct tm*)&_p);
			if (!isms)
			{
				_t *= 1000;
			}
			_milliseconds = _t % 1000;
		}
		void set(const std::string &dstr)
		{
			_str = dstr;
			_t = getDate2Time(dstr, "", (struct tm*)&_p, true, &_fmt);
			_milliseconds = _t % 1000;
		}
		template <typename T>
		T get()
		{
			return (T)*this;
		}
		const char* c_str()
		{
			return _str.c_str();
		}
		std::string getYM()
		{
			return _str.substr(0, 7);
		}
#if 1
		int64_t getDate() //�� Date ���󷵻�һ�����е�ĳһ�� (1 ~ 31)��
		{
			return _p.tm_mday;
		}
		int64_t getWDay() //�� Date ���󷵻�һ���е�ĳһ�� (0 ~ 6)��
		{
			return _p.tm_wday;
		}
		int64_t getMonth() //�� Date ���󷵻��·� (0 ~ 11)��
		{
			return _p.tm_mon;
		}
		int64_t getFullYear() //�� Date ��������λ���ַ�����ݡ�
		{
			return _p.tm_year + 1900;
		}
		int64_t getYear() //��ʹ�� getFullYear( ) �������档
		{
			return _p.tm_year;
		}
		int64_t getHours() //���� Date �����Сʱ (0 ~ 23)��
		{
			return _p.tm_hour;
		}
		int64_t getMinutes() //���� Date ����ķ��� (0 ~ 59)��
		{
			return _p.tm_min;
		}
		int64_t getSeconds() //���� Date ��������� (0 ~ 59)��
		{
			return _p.tm_sec;
		}
		int64_t getMilliseconds() //���� Date ����ĺ���(0 ~ 999)��
		{
			return std::max(_t % 1000, _milliseconds);
		}
		int64_t getTime() //���� 1970 �� 1 �� 1 ������ĺ�������
		{
			return _t;
		}
		int64_t getTimezoneOffset() //���ر���ʱ����������α�׼ʱ�� (GMT) �ķ��Ӳ
		{
			return 0;
		}
		int64_t getUTCDate() //��������ʱ�� Date ���󷵻����е�һ�� (1 ~ 31)��
		{
			return 0;
		}
		int64_t getUTCDay() //��������ʱ�� Date ���󷵻����е�һ�� (0 ~ 6)��
		{
			return 0;
		}
		int64_t getUTCMonth() //��������ʱ�� Date ���󷵻��·� (0 ~ 11)��
		{
			return 0;
		}
		int64_t getUTCFullYear() //��������ʱ�� Date ���󷵻���λ������ݡ�
		{
			return 0;
		}
		int64_t getUTCHours() //��������ʱ���� Date �����Сʱ (0 ~ 23)��
		{
			return 0;
		}
		int64_t getUTCMinutes() //��������ʱ���� Date ����ķ��� (0 ~ 59)��
		{
			return 0;
		}
		int64_t getUTCSeconds() //��������ʱ���� Date ��������� (0 ~ 59)��
		{
			return 0;
		}
		int64_t getUTCMilliseconds() //��������ʱ���� Date ����ĺ���(0 ~ 999)��
		{
			return 0;
		}
		const char* parse() //����1970��1��1����ҹ��ָ�����ڣ��ַ������ĺ�������
		{
			return _str.c_str();
		}
		int64_t setDay(int64_t s, bool ism = true) //���� Date �������µ�ĳһ�� (1 ~ 31)��
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
		int64_t incDay(int64_t s, bool ism = true) //���� Date �������µ�ĳһ�� (1 ~ 31)��
		{
			auto n = Date::dateChange("2018-01-20", s);
			time_t t1 = _t / 1000;
			struct tm tmin = { 0 };
			struct tm *tminp = &tmin;
			struct tm tmout = { 0 };
			struct tm *tmoutp = &tmout;

			/*��tm�ṹ����ת����1970��1��1�տ�ʼ���������*/
			//t1 = mktime(tminp);
			/*������Ҫ���ӻ��߼���������Ӧ��������������������ڶ�Ӧ1970��1��1�տ�ʼ���������*/
			t1 += s * 60 * 60 * 24;
			set(t1, false);
			/*��time_t����Ϣת����ʵ�����ʱ�����ڱ�ʾ������ɽṹtm����*/
			//tmoutp = localtime(&t1);
			return 0;
		}
		int64_t setMonth(int64_t s, bool ism = true) //���� Date �������·� (0 ~ 11)��
		{
			s = std::max(std::min((int)s, 11), 0);
			_p.tm_mon = s;
			if (ism)
			{
				make();
			}
			return 0;
		}
		int64_t setFullYear(int64_t s, bool ism = true) //���� Date �����е���ݣ���λ���֣���
		{
			_p.tm_year = s - 1900;
			if (ism)make();
			return 0;
		}
		int64_t setYear(int64_t s, bool ism = true) //��ʹ�� setFullYear( ) �������档
		{
			_p.tm_year = s;
			if (ism)make();
			return 0;
		}
		int64_t setHours(int64_t s) //���� Date �����е�Сʱ (0 ~ 23)��
		{
			_p.tm_hour = s; make();
			return 0;
		}
		int64_t setMinutes(int64_t s) //���� Date �����еķ��� (0 ~ 59)��
		{
			_p.tm_min = s; make();
			return 0;
		}
		int64_t setSeconds(int64_t s) //���� Date �����е����� (0 ~ 59)��
		{
			_p.tm_sec = s; make();
			return 0;
		}
		int64_t setMilliseconds(int64_t s) //���� Date �����еĺ��� (0 ~ 999)��
		{
			_milliseconds = s;
			return 0;
		}
		int64_t setTime(int64_t s) //�Ժ������� Date ����
		{
			set(s);
			return 0;
		}
		int64_t setUTCDate(int64_t s) //��������ʱ���� Date �������·ݵ�һ�� (1 ~ 31)��
		{
			return 0;
		}
		int64_t setUTCMonth(int64_t s) //��������ʱ���� Date �����е��·� (0 ~ 11)��
		{
			return 0;
		}
		int64_t setUTCFullYear(int64_t s) //��������ʱ���� Date �����е���ݣ���λ���֣���
		{
			return 0;
		}
		int64_t setUTCHours(int64_t s) //��������ʱ���� Date �����е�Сʱ (0 ~ 23)��
		{
			return 0;
		}
		int64_t setUTCMinutes(int64_t s) //��������ʱ���� Date �����еķ��� (0 ~ 59)��
		{
			return 0;
		}
		int64_t setUTCSeconds(int64_t s) //��������ʱ���� Date �����е����� (0 ~ 59)��
		{
			return 0;
		}
		int64_t setUTCMilliseconds(int64_t s) //��������ʱ���� Date �����еĺ��� (0 ~ 999)��
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
		operator const std::string&() const
		{
			return _str;
		}
#ifdef _WIN32 
		operator const char*() const
		{
			return _str.c_str();
		}
#endif
		operator struct tm*() const
		{
			return (struct tm*)&_p;
		}
		/*
		*Durations:ʱ��
		const nMS = 1320; //�Ժ��뵥λ��ʾ�Ĳ�ֵʱ��
		var nD = Math.floor(nMS/(1000 * 60 * 60 * 24));
		var nH = Math.floor(nMS/(1000*60*60)) % 24;
		var nM = Math.floor(nMS/(1000*60)) % 60;
		var nS = Math.floor(nMS/1000) % 60;
		**/
		void make()
		{
			_str = getFormat((struct tm*)&_p, _fmt.c_str());
			_t = ::mktime((struct tm*)&_p) * 1000;
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
			// 1-7 "����һ","���ڶ�","������","������","������","������","������",;
			return iWeek;
		}
		// ��ȡһ��������
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
		static std::string dateChange(const char *cDateIn, int n)
		{
			time_t t1 = 0;
			struct tm tmin = { 0 };
			struct tm *tminp = &tmin;
			struct tm tmout = { 0 };
			struct tm *tmoutp = &tmout;

			char tmp[8 + 32];
			memset(tmp, 0, sizeof(tmp));

			hz::times::strptime_(cDateIn, "%Y-%m-%d", tminp);

			/*��tm�ṹ����ת����1970��1��1�տ�ʼ���������*/
			t1 = mktime(tminp);
			/*������Ҫ���ӻ��߼���������Ӧ��������������������ڶ�Ӧ1970��1��1�տ�ʼ���������*/
			t1 += n * 60 * 60 * 24;
			/*��time_t����Ϣת����ʵ�����ʱ�����ڱ�ʾ������ɽṹtm����*/
			tmoutp = localtime(&t1);

			/*tm���͵�ʱ��ת������tm����%Y%m%d��ʽת����ֵ������У���󳤶�8+1*/
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
		// ��ȡ����������
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

		//��ȡʱ���ַ���
		static std::string getFormat(time_t t, const std::string &fmt, bool ms, struct tm *outtm = nullptr, time_t utc = 8)
		{
			struct tm *p;

			if (ms)
				t *= 0.001;
			char s[80] = { 0 };
			//p = gmtime(&t);
			//strftime(s, 80, "%Y-%m-%d %H:%M:%S::%Z", p);
			//printf("%d: %s\n", (int)t, jsonT::AtoA(s).c_str());
			t += utc * 60 * 60;
			p = gmtime(&t);
			if (outtm)
			{
				memcpy(outtm, p, sizeof(struct tm));
			}
			strftime(s, 80, fmt.c_str(), p);
			//printf("%d: %s\n", (int)t, jsonT::AtoA(s).c_str());
			return s;
		}
		static std::string getFormat(struct tm *p, const std::string &fmt)
		{
			char s[256] = { 0 };
			strftime(s, 256, fmt.c_str(), p);
			return s;
		}
		//��ȡ����ʱ��
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
		static int64_t getDate2Time(const std::string &s, std::string fmt = "", struct tm *stms = nullptr, bool isms = true, std::string *ofmt = 0)
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
			int64_t tt = ::mktime(stms);
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
		static int64_t dateDiff(const Date &dtStart, const Date &dtEnd, char strInterval)
		{
			int64_t tempDate = 0;
			auto dtS = (dtStart);
			auto dtE = (dtEnd);
			// Math.abs
			// console.log('\n---------------------dtStart', dtS, dtE);
			switch (strInterval) {
			case 's': // ��
				tempDate = (((dtE.getTime() - dtS.getTime()) / 1000));
				break;
			case 'n': // ��
				tempDate = (((dtE.getTime() - dtS.getTime()) / 60000));
				break;
			case 'h': // ʱ
				tempDate = (((dtE.getTime() - dtS.getTime()) / 3600000));
				break;
			case 'd': // ��
				tempDate = (((dtE.getTime() - dtS.getTime()) / 86400000));
				break;
			case 'w': // ��
				tempDate = (((dtE.getTime() - dtS.getTime()) / (86400000 * 7)));
				break;
			case 'm': // ��
				tempDate = ((dtE.getMonth() + 1) + ((dtE.getFullYear() - dtS.getFullYear()) * 12) - (dtS.getMonth() + 1));
				break;
			case 'y': // ��
				tempDate = (dtE.getFullYear() - dtS.getFullYear());
				break;
			}
			return tempDate;

		}

		static int64_t dateDiff(const std::string &dtStart, const std::string & dtEnd, char strInterval)
		{
			int64_t tempDate = 0;
			Date dtS(dtStart);
			Date dtE(dtEnd);
			// Math.abs
			// console.log('\n---------------------dtStart', dtS, dtE);
			switch (strInterval) {
			case 's': // ��
				tempDate = (((dtE.getTime() - dtS.getTime()) / 1000));
				break;
			case 'n': // ��
				tempDate = (((dtE.getTime() - dtS.getTime()) / 60000));
				break;
			case 'h': // ʱ
				tempDate = (((dtE.getTime() - dtS.getTime()) / 3600000));
				break;
			case 'd': // ��
				tempDate = (((dtE.getTime() - dtS.getTime()) / 86400000));
				break;
			case 'w': // ��
				tempDate = (((dtE.getTime() - dtS.getTime()) / (86400000 * 7)));
				break;
			case 'm': // ��
				tempDate = ((dtE.getMonth() + 1) + ((dtE.getFullYear() - dtS.getFullYear()) * 12) - (dtS.getMonth() + 1));
				break;
			case 'y': // ��
				tempDate = (dtE.getFullYear() - dtS.getFullYear());
				break;
			}
			return tempDate;

		}

		// ��ȡ����ʱ��
#if 0
		static std::string getDateNet(const std::string &url = "http://time.tianqi.com/")
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
			int64_t tt = ::mktime(&stms);
			tt += 28800;
			return hz::time::getFormat(tt, "%Y-%m-%d %H:%M:%S", false);
		}
#endif
		/*�ж���ƽ�껹������*/
		static int IsLeap(int year) {
			if ((year % 4 == 0) && (year % 100 != 0) || (year % 400 == 0))
				return 1;
			else
				return 0;
		}
		/*�������ĵ�һ�������ڼ�*/
		static int FirstWeeday(int year) {
			int days = (year - 1) * 365 + (year - 1) / 4 - (year - 100) / 100 + (year - 1) / 400;
			return days % 7;
		}
		/*��ӡ����*/
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
		/*��ӡ�·�*/
		static void PrintMonTitl(int month) {
			static const char* mstr[] = { "Janһ", "Feb��", "Mar��", "Apr��",
				"May��", "Jun��", "Jul��", "Aug��", "Sep��", "Octʮ", "Novʮһ", "Decʮ��", };
			if (month < 12 && month >= 0)
			{
				printf("%s��\n", mstr[month]);
			}
		}
		/*��ʽ���������*/
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
			case 0:  "����һ";
			case 1:  "���ڶ�";
			case 2:  "������";
			case 3:  "������";
			case 4:  "������";
			case 5:  "������";
			case 6:  "������";
			}
			return iWeek;
		}
	private:

	};

	/*
	Timer��������ʱ���
	reset��������

	void fun()
	{
	cout << ��hello word�� << endl;
	}
	int main()
	{
	Timer t; //��ʼ��ʱ
	fun();
	cout << t.elapsed() << endl; //��ӡfun������ʱ���ٺ���
	cout << t.elapsed_micro() << endl; //��ӡ΢��
	cout << t.elapsed_nano() << endl; //��ӡ����
	cout << t.elapsed_seconds() << endl; //��ӡ��
	cout << t.elapsed_minutes() << endl; //��ӡ����
	cout << t.elapsed_hours() << endl; //��ӡСʱ
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
		//Ĭ���������
		int64_t elapsed() const
		{
			return (std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now())
				- std::chrono::time_point_cast<std::chrono::milliseconds>(m_begin)).count();
		}
		//΢��
		int64_t elapsed_micro()const
		{
			return (std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now())
				- std::chrono::time_point_cast<std::chrono::microseconds>(m_begin)).count();
		}
		//����
		int64_t elapsed_nano()const
		{
			return (std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now())
				- std::chrono::time_point_cast<std::chrono::nanoseconds>(m_begin)).count();
		}
		//��
		int64_t elapsed_seconds()const
		{
			return (std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now())
				- std::chrono::time_point_cast<std::chrono::seconds>(m_begin)).count();
		}
		//��
		int64_t elapsed_minutes()const
		{
			return (std::chrono::time_point_cast<std::chrono::minutes>(std::chrono::high_resolution_clock::now())
				- std::chrono::time_point_cast<std::chrono::minutes>(m_begin)).count();
		}
		//ʱ
		int64_t elapsed_hours()const
		{
#ifdef _WIN32
			return std::chrono::duration_cast<std::chrono::hours>(std::chrono::high_resolution_clock::now() - m_begin).count();
#else
			return (std::chrono::time_point_cast<std::chrono::hours>(std::chrono::high_resolution_clock::now())
				- std::chrono::time_point_cast<std::chrono::hours>(m_begin)).count();
#endif
		}
#else
		//Ĭ���������
		int64_t elapsed() const
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}
		//΢��
		int64_t elapsed_micro() const
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}
		//����
		int64_t elapsed_nano() const
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}
		//��
		int64_t elapsed_seconds() const
		{
			return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}
		//��
		int64_t elapsed_minutes() const
		{
			return std::chrono::duration_cast<std::chrono::minutes>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}
		//ʱ
		int64_t elapsed_hours() const
		{
			return std::chrono::duration_cast<std::chrono::hours>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}

#endif
#if 1
		//����
		static int64_t get_ms()
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}
		//΢��
		static int64_t get_micro()
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}
		//����
		static int64_t get_nano()
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}
		//��
		static int64_t get_seconds()
		{
			return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}
		//��
		static int64_t get_minutes()
		{
			return std::chrono::duration_cast<std::chrono::minutes>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}
		//ʱ
		static int64_t get_hours()
		{
			return std::chrono::duration_cast<std::chrono::hours>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}
		static int64_t get_now()
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
