/**
 * mt_dictionary.cpp
 * Created 2010.4.19
 *      Author yuandong.xu
 */

#include "mt_dictionary.h"
#include "mt_string_operation.h"
#include <iostream>
#include <fstream>

using namespace std;

namespace mt_parallel{

static const char* en_cn_dict_file = "dict/dict.utf";
static const char* en_stop_words_file = "dict/E.stoplist";
static const char* en_common_words_file = "dict/E.common";
//static const char* jp_cn_dict_file = "dict/chn-jpn-dict-all.txt";
static const char* jp_cn_dict_file = "dict/chn-jap-dict-single-word-alimt.txt";

auto_ptr<MtDictionary> MtDictionary::m_dict;
    
MtDictionary::MtDictionary()
{
    enToch.clear();
    jpToch.clear();
    chToen.clear();
    chTojp.clear();
    enCommonWords.clear();
    enStopWords.clear();

    // initialize dictionary maps
    string line;
    
    // en - chi
    ifstream fin;
    fin.open(en_cn_dict_file);
    if(fin.fail()){
        cout << "fail to open English-Chinese dictionary" << endl;
        return ;
    }
    while(getline(fin, line))
    {
        vector<string> words = StringOperation::split(line, "\t");
        if (words.size() >= 2) 
        {
            
            string en = words[0];
            string cn = words[1];
            en = StringOperation::tolower(en);
            if (StringOperation::isDigit(en) && StringOperation::isDigit(cn))
                continue;
            if (en != cn) 
            {
                enToch[en].insert(cn);
                chToen[cn].insert(en);
            }
        }
    }
    fin.close();

    // jp - ch
    ifstream fin2;
    fin2.open(jp_cn_dict_file);
    if(fin2.fail()){
        cout << "fail to open Japanese-Chinese dictionary" << endl;
        return ;
    }
    while(getline(fin2, line))
    {
        vector<string> words = StringOperation::split(line, "\t");
        if (words.size() >= 2) 
        {
            
            string cn = words[0];
            string jp = words[1];
            if (jp.size() > 0 && jp[jp.size() - 1] == '\r')
                jp = jp.substr(0, jp.size() - 1);
            jpToch[jp].insert(cn);
            chTojp[cn].insert(jp);
        }
    }
    fin2.close();

    // en stop words
    ifstream fin3;
    fin3.open(en_stop_words_file);
    if(fin3.fail()){
        cout << "fail to open English stop words file" << endl;
        return;
    }
    while(getline(fin3, line))
    {
        line = StringOperation::tolower(line);
        enStopWords.insert(line);
    }
    fin3.close();

    // en common words
    ifstream fin4;
    fin4.open(en_common_words_file);
    if(fin4.fail()){
        cout << "fail to open English common words file" << endl;
        return;
    }
    while(getline(fin4, line))
    {
        line = StringOperation::tolower(line);
        enCommonWords.insert(line);
    }
    fin4.close();

    // output result info
    //cout << "EN " << enToch.size() << endl;
    //cout << "JP " << jpToch.size() << endl;
    //cout << "EN STOP " << enStopWords.size() << endl;
    //cout << "EN COMMON " << enCommonWords.size() << endl;
}

MtDictionary::~MtDictionary()
{
    enToch.clear();
    jpToch.clear();
    chToen.clear();
    chTojp.clear();
    enCommonWords.clear();
    enStopWords.clear();
}

}; // end of namespace

