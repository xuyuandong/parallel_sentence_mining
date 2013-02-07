/*
 * mt_string_operation.h
 *
 *  Created on: 2010-4-13
 *      Author: yuandong.xu
 */

#ifndef MT_STRING_OPERATION_H_
#define MT_STRING_OPERATION_H_

#include <vector>
#include <string>
using namespace std;

namespace mt_parallel {

namespace StringOperation{

/** Split the given string by delimit */
vector<string> split(const string & src, const string & delimit);

/** Remove the empty character in two ends of string ,return the modified string */
string trim(const string& s);

string tolower(const string& s);

string toupper(const string & s);

/** Get all the substring between string start and stop */
vector<string> getAllMidString(const string & input, const string & start,
		const string & stop, bool containOther);

/** Whether a string is a biaodian or not */
bool isDots(const string & str);

/** Remove useless char */
string getShrinkString(const string & input);

string getURLHost(const string & url);

bool isDigit(const string & str);

bool isSpecialCase(const string & str);

bool isWord(const string & str);

int getWordLang(const string & str);

string toString(int x);

string toString(double x);

};//end of namespace StringOperation

}
; //end of namespace mt_parallel


#endif /* MT_STRING_OPERATION_H_ */
