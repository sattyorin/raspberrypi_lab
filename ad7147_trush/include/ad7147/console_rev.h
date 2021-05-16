#ifndef __H_CONSOLE__
#define __H_CONSOLE__
#include <fstream>
#include <iostream>
#include <iomanip>
#include <chrono>
namespace lacus{
	enum nfo_type{
		EE,
		II,
		WW,
		OK
	};

	typedef std::chrono::duration<double> fpseconds;
  template<typename Q> std::string bold(const Q& str){
    using namespace std;
    stringstream s1;
    s1<<"\e[1m"<<str<<"\e[0m";
    return s1.str();
  };
	template<typename W> void info(const W& message){
		echo(II,message,std::cout);
	};
	template<typename W> void warn(const W& message){
		echo(WW,message,std::cout);
	};
	template<typename W> void fail(const W& message){
		echo(EE,message,std::cout);
	};
	template<typename W> void okay(const W& message){
		echo(OK,message,std::cout);
	};
	template<typename W> void echo(nfo_type type,const W& message){
		echo(type,message,std::cout);
	};
	extern std::chrono::system_clock::time_point start;
	template<typename S,typename W> void echo(nfo_type type,const W& message, S& stream){
		using namespace std;
		using namespace chrono;
		switch(type){
			case II:
				stream<<"\e[1;36m[I\e[0m"<<setw(10)<<duration_cast<fpseconds>(system_clock::now()-start).count()<<"\e[1;36m]\e[0m ";
				break;
			case EE:
				stream<<"\e[1;31m[E\e[0m"<<setw(10)<<duration_cast<fpseconds>(system_clock::now()-start).count()<<"\e[1;31m]\e[0m ";
				break;
			case WW:
				stream<<"\e[1;93m[W\e[0m"<<setw(10)<<duration_cast<fpseconds>(system_clock::now()-start).count()<<"\e[1;93m]\e[0m ";
				break;
			case OK:
				stream<<"\e[1;92m[O\e[0m"<<setw(10)<<duration_cast<fpseconds>(system_clock::now()-start).count()<<"\e[1;92m]\e[0m ";
				break;
		};
		stream<<message<<std::endl;
	};
};
#endif
