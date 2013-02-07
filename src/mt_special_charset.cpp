/**
 * mt_special_charset.cpp
 * Created on 2010.4.14
 *      Author yuandong.xu
 */

#include "mt_special_charset.h"
#include <fstream>
#include <iostream>

using namespace std;

namespace mt_parallel{

set<string> SpecialCaseOfSplit::specialWordSet;
set<string> SpecialCaseOfSplit::keyPhraseSet;

bool SpecialCaseOfSplit::judge(const string & word)
{
    static bool flag = true;
    if(flag)
    {
        ifstream fin("special_words.txt");
        if(fin.fail()){
            cout<<"fail to open special_words.txt"<<endl;
            return false;
        }
        string str;
        while(getline(fin, str))
        {
            specialWordSet.insert(str);
        }
        fin.close();
        flag = false;
    }

    if (word.size() == 1)
    {
        if (word[0] >= 'a' && word[0] <= 'z')
            return true;
        if (word[0] >= 'A' && word[0] <= 'Z')
            return true;
    }

    if (specialWordSet.find(word) != specialWordSet.end())
        return true;

    return false;
}

bool SpecialCaseOfSplit::hasKeyPhrase(const string & sentence)
{
    static bool flag = true;
    if (flag)
    {
        ifstream fin("key_phrase_list.txt");
        if (fin.fail()){
            cout << "fail to open key_phrase_list.txt" << endl;
            return false;
        }
        string str;
        while(getline(fin,str))
        {
            keyPhraseSet.insert(str);
        }
        fin.close();
        flag = false;
    }

    set<string>::iterator iter;
    for (iter = keyPhraseSet.begin(); iter != keyPhraseSet.end(); ++iter)
    {
        if (sentence.find(*iter) != string::npos)
            return true;
    }
    
    return false;
}

    
}; //end of namespace

