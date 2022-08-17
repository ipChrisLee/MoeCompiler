#pragma once

#include <string>
#include <string_view>
#include <memory>

#include "cprt.hpp"

// https://zhuanlan.zhihu.com/p/141690132
#ifndef CODEPOS //  prevent redefinition from common.hpp
#define STR(t) #t 
#define LINE_STR(v) STR(v)
#define CODEPOS "File["  __FILE__  "] Line["  LINE_STR(__LINE__) "] "
#endif
//  https://stackoverflow.com/questions/733056/is-there-a-way-to-get-function-name-inside-a-c-function
#define FUNINFO __PRETTY_FUNCTION__
#define NVPAIR(v) std::pair(#v,v) // labelName-variable pair


/*  Moe debug tool. You can disable some mdb tool for any time.
 *  When you're initializing one instance, you should consider WHERE and print format
 *  by setting `std::unique_ptr<com::cprt>` variable. 
 *  However, when you're using this model, you should only think of WHAT to debug.
 * */
namespace com {
class mdb {
	/*  我在设计这个类的时候花费了非常多的时间，最主要的问题点在于：
	 *  传入的cprt到底是不是归mdb管。这点非常重要。
	 *  如果是文件流构造的cprt，我希望在mdb析构的时候相应文件流构造的cprt可以也可以不析构。
	 *  但是如果是使用别的ostream的引用构造的，我希望在mdb析构的时候相应cprt不析构。
	 *  这造成了特别大的矛盾。。。
	 *  最后解决方式是限制mdb的使用方式，现在一个mdb变量管一个cprt。
	 *  也就是说如果你用文件初始化mdb的话，你必须保证：这个文件只为这个mdb存在。
	 * */
  private:
	const std::unique_ptr<com::cprt> printer;
	bool enable;
	const std::string headstr;
	static bool sysEnable;
  public:
	/*  You should use `std::unique_prt(new XX);` to provide a totally new
	 *  `com::cprt` printer. You should NOT use `com::cprt *` get from another
	 *  `com::cprt`. For example, `&com::ccout` is NOT allowed for first argument.
	 * */
	explicit mdb(
		std::unique_ptr<com::cprt> printer,
		bool enable = true,
		std::string_view header = "[mdb] "
	) : printer(std::move(printer)), enable(enable), headstr(header) {
	}

	mdb(const mdb &) = delete;
	mdb & operator=(const mdb &) = delete;

	/*  method to enable mdb or disable it. */
	bool & setEnable() { return enable; }

	/*  method to enable all mdb or disable them. */
	static bool & setSysEnable() { return sysEnable; }

	/*  Print messages to outs stream. It will detect '\n' and print
	 *  newline (and flush stream) and head string (e.get. '[mdb] ') for every '\n'
	 *  and {at the end of printing}.
	 *  if you don't want text to be decorated, set decorated=false;
	 *  detect enable flag automatically.
	 *  to print the location of message, let printLoc=CODEPOS. (see definition of CODELOC)
	 * */
	void msgPrint(
		const std::string & msg, /* Message to print. */
		const std::string & printLoc = "", /* If not empty, print location at beginning */
		bool decorated = true
	) {
		if (enable && sysEnable) {
			if (decorated) {
				if (printLoc.length()) { printer->cprintLn(headstr + printLoc); }
			} else {
				if (printLoc.length()) { printer->printLn(headstr + printLoc); }
			}
			std::string buf = headstr;
			for (auto ch: msg) {
				if (ch == '\n') {
					if (decorated) { printer->cprintLn(buf); }
					else { printer->printLn(buf); }
				} else {
					buf.push_back(ch);
				}
			}
			if (decorated) { printer->cprintLn(buf); }
			else { printer->printLn(buf); }
		}

	}

	/*  Method to print 'list' of 'pair' of string and object. One line one object.
	 *  It will print head string (e.get. '[mdb] ') for every line.
	 *  It will NOT print newline after the end of string.
	 *  You should limit the count of the lines of S.to_asm() to one.
	 *  If you don't want text to be decorated, set decorated=false;
	 *  detect enable flag automatically.
	 *  to print the location of message, let printLoc=CODEPOS. (see definition of CODELOC)
	 *  usage :
	 *      mdb::getStatic()
	 *          .infoPrint(std::tuple( NVPAIR("n",n), NVPAIR("t",t) ),CODEPOS);
	 * */
	template<typename Type>
	void infoPrint(
		std::pair<const char *, Type> t, /* Message to print. */
		const std::string & printLoc = "", /* If not empty, print location at beginning */
		bool decorated = true
	) {
		if (enable && sysEnable) {
			if (decorated) {
				if (printLoc.length()) { printer->cprintLn(headstr + printLoc); }
				printer
					->cprintLn(headstr + std::string(t.first) + " : " + cprt::tos(t.second));
			} else {
				if (printLoc.length()) { printer->printLn(headstr + printLoc); }
				printer->printLn(headstr + std::string(t.first) + " : " + cprt::tos(t.second));
			}
		}
	}

	/*  method to print 'list' of 'pair' of string and object. One line one object.
	 *  it will print head string (e.get. '[mdb] ') for every line.
	 *  it will noT print newline after the end of string.
	 *  you should limit the count of the lines of S.to_asm() to one.
	 *  if you don't want text to be decorated, set decorated=false;
	 *  detect enable flag automatically.
	 *  to print the location of message, let printLoc=CODEPOS. (see definition of CODELOC)
	 *  usage :
	 *      mdb::getStatic()
	 *          .infoPrint(std::tuple( std::pair("n",n),(std::pair("t",t)) ),CODEPOS);
	 * */
	template<typename ... Types>
	void infoPrint(
		\
        std::tuple<std::pair<const char *, Types>...> t, /* Message to print. */\
        const std::string & printLoc = "", /* If not empty, print location at beginning */ \
        bool decorated = true\

	) {
		if (!enable || !sysEnable) { return; }
		if (decorated) {
			if (printLoc.length()) { printer->cprintLn(headstr + printLoc); }
			apply(
				[&](auto && ... args) {
					((printer->cprintLn(
						headstr + std::string(args.first) + " : " +
							cprt::tos(args.second))), ...);
				}, t
			);
		} else {
			if (printLoc.length()) { printer->printLn(headstr + printLoc); }
			apply(
				[&](auto && ... args) {
					((printer->printLn(
						headstr + std::string(args.first) + " : " +
							cprt::tos(args.second))), ...);
				}, t
			);
		}
	}
};

extern mdb ccdbg;

}
