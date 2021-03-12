#ifndef __H_CONSOLE__
#define __H_CONSOLE__
#include <fstream>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
namespace lacus{
	enum nfo_type{
		EE,//fail
		II,//information
		WW,//warn
		OK,//ok
    IG //in progress
	};
  template<typename Function,typename... W> void invoke_parameterized(Function fref, const W&... messages){
    using std::stringstream;
    stringstream ssm;
    std::forward_as_tuple((ssm<<messages<<" ")...);
    fref(ssm.str());
  };
	template<typename W> void info(const W& message){
		echo(II,message,std::cout);
	};
  template<typename... M> void info(const M&... messages){ invoke_parameterized(info,messages...); };
	template<typename W> void warn(const W& message){
		echo(WW,message,std::cout);
	};
  template<typename... M> void warn(const M&... messages){ invoke_parameterized(warn,messages...); };
	template<typename W> void fail(const W& message){
		echo(EE,message,std::cerr);
	};
	template<typename W> void okay(const W& message){
		echo(OK,message,std::cout);
	};
  template<typename... W> void okay(const W&... messages){ invoke_parameterized(okay,messages...); };
	template<typename W> void echo(nfo_type type,const W& message){
		echo(type,message,std::cout);
	};
  template<typename W> void update(const W& message){
    echo(IG,message,std::cout,'\r');
    std::cout.flush();
  };
  template<typename... W> void update(const W&... messages){ invoke_parameterized(update,messages...); };
  
  template<typename S> S wrap_escape_code(const S& instructions, const S& message){ 
    return S("\e[")+instructions+S("m")+message+S("\e[0m");
  };
  template<typename S> S bold(const S& ins){ return wrap_escape_code(S("1"),ins);  };
  template<typename S> S red(const S& ins){ return wrap_escape_code(S("31"),ins);  };
  template<typename S> S green(const S& ins){ return wrap_escape_code(S("92"),ins);  };
  template<typename S> S blue(const S& ins){ return wrap_escape_code(S("34"),ins);  };
  template<typename S> S yellow(const S& ins){ return wrap_escape_code(S("33"),ins);  };
  template<typename S> S magenta(const S& ins){ return wrap_escape_code(S("95"),ins);  };
  template<typename S> S cyan(const S& ins){ return wrap_escape_code(S("96"),ins);  };

	extern std::chrono::system_clock::time_point start;
	template<typename S,typename W> void echo(nfo_type type,const W& message, S& stream, char terminator='\n'){
		using namespace std;
		using namespace chrono;
		typedef duration<double> fpseconds;
		switch(type){
			case II:
				stream<<"\e[1;36m[I\e[0m"<<setw(8)<<duration_cast<fpseconds>(system_clock::now()-start).count()<<"\e[1;36m]\e[0m ";
				break;
			case EE:
				stream<<"\e[1;31m[E\e[0m"<<setw(8)<<duration_cast<fpseconds>(system_clock::now()-start).count()<<"\e[1;31m]\e[0m ";
				break;
			case WW:
				stream<<"\e[1;93m[W\e[0m"<<setw(8)<<duration_cast<fpseconds>(system_clock::now()-start).count()<<"\e[1;93m]\e[0m ";
				break;
			case OK:
				stream<<"\e[1;92m[O\e[0m"<<setw(8)<<duration_cast<fpseconds>(system_clock::now()-start).count()<<"\e[1;92m]\e[0m ";
				break;
      case IG:

				stream<<"\e[1;95m[~\e[0m"<<setw(8)<<duration_cast<fpseconds>(system_clock::now()-start).count()<<"\e[1;95m]\e[0m ";
        break;
		};
		stream<<message<<terminator;
	};
};
#endif
