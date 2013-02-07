#ifndef MT_CODE_CONVERTER
#define MT_CODE_CONVERTER

#include <string>
//#include <iostream>
//#include <sstream>
//#include <fstream>
//#include <algorithm>
#include <iconv.h>
#include <cctype>
#include "../codedetector/code_detector.h"
#include "../codedetector/encoding_convert.h"
#include "../mt_htmlparser/html_esc_converter.h"

using namespace galaxy::utility::mthtmlparser;
using namespace galaxy::utility::codedetector;
using namespace std;

namespace mt_parallel{

/** Code Converter */
class MtCodeConverter {
public:
    string convertUTF8toGB2312(const string & str);
      
    string convertGB2312toUTF8(const string & str);

 	/** Auto Detect the input str encoding , convert it to UTF8, remove HTML excape character */
	string convertToUTF8(string & str);

private:
    int code_convert(const char *from_charset, const char *to_charset,char *inbuf,size_t
                inlen,char *outbuf,size_t outlen);

//    int u2g(char *inbuf,size_t inlen,char *outbuf,size_t outlen);

//    int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen);

private:
    CodeDetector codeDetector;
    EncodingConvert encodingConvert;
    HtmlEscConverter htmlEscConverter;
};

};//end of namespace

#endif
