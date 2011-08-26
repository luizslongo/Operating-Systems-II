/*
    Mateus Krepsky Ludwich.
*/

#ifndef DUMPER_H_
#define DUMPER_H_

#include "../mec_picture.h"
#include "../distributed/dmec_picture.h"
#include "../distributed/lazy_picture.h"

#include "../error.h"

#include <string>


using namespace std;


class Dumper
{
public:
    static void dumpMEC_Picture(const char * outputFileName, MEC_Picture * picture);

    static void dumpDMEC_Picture(const char * outputFileName, DMEC_Picture * picture);

    static void dumpLazyPicture(const char * outputFileName, LazyPicture * picture);

    template<typename T>
    static void dumpExpectedObtained(const char * outputFileName, T expected, T obtained)
    {
        // TODO
        TODO();
    }


    template<typename T>
    static void dump(string& outputFileName, T a)
    {
    	string rep = T::str(a);
    	dumpString(outputFileName, rep);
    }


private:
    static void  dumpString(string& outputFileName, string& rep);

};


#endif /* DUMPER_H_ */
