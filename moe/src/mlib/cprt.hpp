#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <utility>

#include "moeconcept.hpp"


using std::to_string;

inline std::string to_string(char ch) {
	return {1, ch};
}

inline std::string to_string(std::string_view sv) {
	return std::string(sv);
}

inline std::string to_string(const char * str) {
	return {str};
}

template<typename T>
std::string to_string(const T * p) {
	return std::to_string((unsigned long long) p);
}

template<typename T>
std::string to_string(T * p) {
	return std::to_string((unsigned long long) p);
}

inline std::string to_string(const moeconcept::Stringable & o) {
	return o.toString();
}

namespace com {

namespace color {
extern const std::string black;
extern const std::string red;
extern const std::string err;
extern const std::string green;
extern const std::string fin;
extern const std::string yellow;
extern const std::string iterct;
extern const std::string blue;
extern const std::string purple;
extern const std::string cyan;
extern const std::string working;
extern const std::string bpurple;
extern const std::string warning;
extern const std::string noc;
}

/*  We will use `cprt` to avoid using ostream explicitly.
 *  You can specify default color and `ostream` when creating a `cprt`.
 *  You can NOT change the `ostream` after creating such `cprt`, but you can
 *  change color by passing argument.
 *  The instance of this class can NOT be constructed from another instance.
 *  i.e. the copy constructor and operator= are both deleted.
 *  `com::ccout` and `com::ccerr` are used to replace `cout` and `cerr`.
 *
 *  Constructor:
 *  explicit cprt(const std::string & filePath,const std::string & defaultColor=""):
 *      outFileStream(filePath),outStream(outFileStream),defaultColor(defaultColor){
 *  }
 *  explicit cprt(std::ostream & outStream,const std::string & defaultColor=""):
 *      outStream(outStream),defaultColor(defaultColor){
 *  }
 *
 *  ( cprt = colored print, ccout = colored cout, ccerr = colored cerr)
 *  Methods defined here mimics java io unit design.
 *  Defect :
 *      1.  cprt cannot handle pointer except char pointer. (e.g. `int *`,`double *`)
 *      2.  cprt cannot handle classes provided by other libs. (e.g. `vector`,`queue`)
 * */
class cprt {
  private:
	std::ofstream outFileStream;
	std::ostream & outStream;
	const std::string defaultColor;
  public:
	struct TOS {
		//  Thank zx who teaches me about this method.
		template<typename T>
		auto operator()(T && t) -> std::string {
			if constexpr (std::is_convertible_v<T, std::string>) {
				return std::string(t);
			} else {
				return to_string(t);
			}
		}
	};
	
	static TOS tos;
	
	explicit cprt(const std::string & filePath, std::string defaultColor = "") :
			outFileStream(filePath), outStream(outFileStream), defaultColor(std::move(defaultColor)) {
	}
	
	explicit cprt(std::ostream & outStream, std::string defaultColor = "") :
			outStream(outStream), defaultColor(std::move(defaultColor)) {
	}
	
	cprt(cprt & other) = delete;  //  It is too hard to code, so I delete it.
	void operator=(cprt & other) = delete;  //  It is too hard to code, so I delete it too.
	
	/*  Colored output method. Print text decorated with `defaultColor` or argument
	 *  `col`.
	 *
	 *  e.g. :
	 *      com::ccout.cprint(tuple(1,2,'a',"S",0.5,S()));
	 *      com::ccout.cprint(tuple(1,2,'a',"S",0.5,S()),cprt::red);
	 *  Ref :
	 *      https://stackoverflow.com/questions/1198260/how-can-you-iterate-over-the-elements-of-an-stdtuple
	 * */
	template<class ... Types>
	void cprint(
			std::tuple<Types...> t       /* Use std::tuple to package things to print. */
	) {
		cprint(t, defaultColor);
	}
	
	template<class ... Types>
	void cprint(
			std::tuple<Types...> t,      /* Use std::tuple to package things to print. */
			const std::string & col     /* Decoration of text. */
	) {
		outStream << col;
		apply([&](auto && ... args) { ((outStream << tos(args)), ...); }, t);
		outStream << com::color::noc;
	}
	
	/*  Colored output with `endl`. Print text decorated with `defaultColor` or argument
	 *  `col`.
	 * */
	template<class ... Types>
	void cprintLn(
			std::tuple<Types...> t       /* Use std::tuple to package things to print. */
	) {
		cprintLn(t, defaultColor);
	}
	
	template<class ... Types>
	void cprintLn(
			std::tuple<Types...> t,      /* Use std::tuple to package things to print. */
			const std::string & col     /* Decoration of text. */
	) {
		outStream << col;
		apply([&](auto && ... args) { ((outStream << tos(args)), ...); }, t);
		outStream << color::noc << std::endl;
	}
	
	/*  Colored output for one argument printing.
	 * */
	template<typename T>
	void cprint(
			T v                        /* Text to print. */
	) {
		cprint<T>(v, defaultColor);
	}
	
	template<typename T>
	void cprint(
			T v,                       /* Text to print. */
			const std::string & col    /* Decoration of text. */
	) {
		outStream << col << tos(v) << color::noc;
	}
	
	/*  Colored output for one argument printing with `endl`;
	 * */
	void cprintLn() { cprintLn("", defaultColor); }
	
	template<typename T>
	void cprintLn(
			T v                        /* Text to print. */
	) {
		cprintLn(v, defaultColor);
	}
	
	template<typename T>
	void cprintLn(
			T v,                        /* Text to print. */
			const std::string & col       /* Decoration of text. */
	) {
		outStream << col << tos(v) << color::noc << std::endl;
	}
	
	/*  Output method without decoration.
	 * */
	template<class ... Types>
	void print(
			std::tuple<Types...> t      /* Use std::tuple to package things to print. */
	) {
		apply([&](auto && ... args) { ((outStream << tos(args)), ...); }, t);
	}
	
	/*  Output with `endl` without decoration.
	 * */
	template<class ... Types>
	void printLn(
			std::tuple<Types...> t      /* Use std::tuple to package things to print. */
	) {
		apply([&](auto && ... args) { ((outStream << tos(args)), ...); }, t);
		outStream << std::endl;
	}
	
	/*  Output for one argument without decoration.
	 * */
	template<typename T>
	void print(
			T v                       /* Text to print. */
	) {
		outStream << tos(v);
	}
	
	/*  Output for one argument printing with `endl` without decoration;
	 * */
	template<typename T>
	void printLn(
			T v                        /* Text to print. */
	) {
		outStream << tos(v) << std::endl;
	}
	
	
};

extern cprt ccout;    //  Replace `cout`
extern cprt ccerr;    //  Replace `cerr`
extern const std::string nullFilePath;
extern cprt cnull;    //  To print nothing.
}
