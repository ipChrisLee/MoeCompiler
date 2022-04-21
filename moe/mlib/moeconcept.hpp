#pragma once 

#include <fstream>
#include <string>
#include <functional>

namespace moeconcept{

struct Stringable{
    virtual std::string toString() const = 0;
};

/*  Interface of storing object to string.
 *  e.g. : 
 *      struct S : public storable{
 *          int x[2];
 *          S(){ x[0]=rand();x[1]=rand(); }
 *          virtual std::string defaultGetContent() override {
 *              return std::to_string(x[0])+","+std::to_string(x[1]);
 *          }
 *          static int Main(){ S s;s.store("x.log"); }
 *      };
 * */
struct Storable{
    virtual bool store(
        const std::string & filePath,
        std::function<std::string(Storable&)>getContent=&Storable::defaultGetContent
    ) final {
        std::ofstream ofs(filePath);
        if(!ofs.is_open()) return false;
        ofs<<getContent(*this);
        ofs.close();
        return ofs.good();
    }
    virtual std::string defaultGetContent(){
        return "Storable::defaultGetContent\n";
    }
};

}
