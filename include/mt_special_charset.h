/**
 * mt_special_charset.h
 * Created on: 2010.4.14
 *      Author: yuandong.xu
 */

#ifndef MT_SPECIAL_CHARSET_H_
#define MT_SPECIAL_CHARSET_H_

#include <string>
#include <set>

using namespace std;

namespace mt_parallel{

class SpecialCaseOfSplit
{
public:
	// special case in English, not split
	static bool judge(const string & word);
    // key label words which need to be filtered
    static bool hasKeyPhrase(const string & sentence);
	
private:
	static set<string> specialWordSet;
    static set<string> keyPhraseSet;
};

}; //end of namespace

#endif 
