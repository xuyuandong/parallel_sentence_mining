#include <string>
#include <iconv.h>
#include <cctype>
#include "mt_code_converter.h"

using namespace std;

namespace mt_parallel{

string MtCodeConverter::convertToUTF8(string & input)
{
    string utf8Doc = "";
    try {
        Encoding encoding = codeDetector.Detect(input.data(), input.length(), 1024);
        if (encoding.first == UNDEF_ENCODING) 
            encoding.first = UTF_8;
        utf8Doc = htmlEscConverter.HtmlEscConvert(encodingConvert.ConvertToUtf8(input, encoding.first));
    }
    catch (...) {
        utf8Doc = "";
    }
    return utf8Doc; 
}

int MtCodeConverter::code_convert(const char *from_charset, const char *to_charset,char *inbuf,size_t
        inlen,char *outbuf,size_t outlen)
{
    iconv_t cd;
    char **pin = &inbuf;
    char **pout = &outbuf;

    cd = iconv_open(to_charset,from_charset);
    if(cd==0) return -1;

    memset(outbuf,0,outlen);
    size_t oldoutlen = outlen;
    if(iconv(cd,pin,&inlen,pout,&outlen)==-1) {
        iconv_close(cd);
        return -1;
    }

    iconv_close(cd);

    return 0;
}


string MtCodeConverter::convertUTF8toGB2312(const string & str)
{
    if (str.length() >= 65536) return "";
    static char inbuff[65536];
    static char outbuff2[4096];
    static char outbuff3[32767];
    static char outbuff4[150000];
    static char * outbuff;

    int outlen = 4096;
    outbuff = outbuff2;
    if (str.length() >= 2048) {
        outbuff = outbuff3;
        outlen = 32767;
    }
    if (str.length() >= 16000) {
        outbuff = outbuff4;
        outlen = 150000;
    }

    strcpy(inbuff,str.c_str());

    int inlen = str.length();

    if(code_convert("utf-8","gb2312",inbuff,inlen,outbuff,outlen)==0)
//    if (u2g(inbuff , inlen , outbuff , outlen) == 0)
        return string(outbuff);
    else
        return "";
}

string MtCodeConverter::convertGB2312toUTF8(const string & str)
{
    if (str.length() >= 65536) return "";
    static char inbuff[65536];
    static char outbuff2[4096];
    static char outbuff3[32767];
    static char outbuff4[150000];
    static char * outbuff;

    int outlen = 4096;
    outbuff = outbuff2;
    if (str.length() >= 2048) {
        outbuff = outbuff3;
        outlen = 32767;
    }
    if (str.length() >= 16000) {
        outbuff = outbuff4;
        outlen = 150000;
    }

    strcpy(inbuff,str.c_str());

    int inlen = str.length();

    if(code_convert("gb2312","utf-8",inbuff,inlen,outbuff,outlen)==0)
        return string(outbuff);
    else
        return "";
}


};//end of namespace
