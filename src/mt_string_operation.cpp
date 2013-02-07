/*
 * mt_string_operation.cpp
 *
 *  Created on: 2010-4-13
 *      Author: yuandong.xu
 */

#include "mt_string_operation.h"
#include <vector>
#include <string>
#include <set>

using namespace std;

namespace mt_parallel{

namespace StringOperation{

/** Split the given string by delimit */
vector<string> split(const string & src, const string & delimit)    
{
    const string & null_subst = "";
    
	if (src.empty() || delimit.empty())
		return vector<string> ();

	vector<string> v;
	size_t deli_len = delimit.size();
	long index = string::npos, last_search_position = 0;
	while ((index = src.find(delimit, last_search_position)) != string::npos)
	{
		if (index == last_search_position)
			v.push_back(null_subst);
		else
			v.push_back(src.substr(last_search_position, index
					- last_search_position));
		last_search_position = index + deli_len;
	}
	string last_one = src.substr(last_search_position);
	v.push_back(last_one.empty() ? null_subst : last_one);
	return v;
}

/** Remove the empty character in two ends of string ,return the modified string */
string trim(const string& s)
{
	int i, j;
	for (i = 0; i < s.length(); ++i)
		if (s[i] != ' ' && s[i] != '\t')
			break;

	for (j = s.length() - 1; j >= 0; --j)
		if (s[j] != ' ' && s[j] != '\t')
			break;

	return s.substr(i, j - i + 1);
}

string tolower(const string& s)
{
	string ret(s);
	for (int i = 0; i < ret.length(); ++i)
		if (ret[i] >= 'A' && ret[i] <= 'Z')
			ret[i] = ret[i] - 'A' + 'a';
	return ret;
}

string toupper(const string & s)
{
	string ret(s);
	for (int i = 0; i < ret.length(); ++i)
		if (ret[i] >= 'a' && ret[i] <= 'z')
			ret[i] = ret[i] - 'a' + 'A';
	return ret;
}

/** Get all the substring between string start and stop */
vector<string> getAllMidString(const string & input,
		const string & start, const string & stop, bool containOther = false)
{
	vector<string> result;

	size_t startLen = start.length();
	size_t stopLen = stop.length();
	size_t nowstart = 0;
	size_t nextstart = 0;
	while (nextstart < input.length())
	{
		nowstart = input.find(start, nextstart);
		if (containOther)
		{
			if (nowstart == string::npos)
				nowstart = input.length();
			result.push_back(input.substr(nextstart, nowstart - nextstart));
		}
		if (nowstart == string::npos)
			break;
		size_t datastart = nowstart + startLen;
		size_t datastop = input.find(stop, datastart);
		if (datastop == string::npos)
			break;
		string data = input.substr(datastart, datastop - datastart);
		result.push_back(data);
		nextstart = datastop + stopLen;
	}
	return result;
}

/** Whether a string is a biaodian or not */
bool isDots(const string & str) {
	static string chiDots[] = {
                "。", "，", "；", "：", "？", "！", "……", "—",
                "～", "〔", "〕",
                "《", "》", "‘", "’", "“", "”", "`", "．",
                "【", "】"
            };

	static string engDots[] = { ".", ",", ";", ":", "?", "!", "鈥??", "-", "~",
			"(", ")", "<", ">", "'", "'", "\"", "\"", "'", ".", "[", "]" };
	static set<string> dots;
	static bool first = true;
	if (first)
	{
		dots.insert(chiDots, chiDots + 21);
		dots.insert(engDots, engDots + 21);
		dots.insert("銆??");
		first = false;
	}

	if (dots.find(str) != dots.end())
		return true;
	return false;
}

/** Remove useless char */
string getShrinkString(const string & input)
{
	string ret = input;
	for (int i = 0; i < ret.size(); ++i)
		if (ret[i] == '\t')
			ret[i] == ' ';
	string result = "";
	vector<string> words = split(input, " ");
	for (int i = 0; i < words.size(); ++i)
		if (words[i] != "" && !isDots(words[i]))
			result += words[i];
	return result;
}

string getURLHost(const string & url)
{
	int startPosi = url.find("://");
	int stopPosi = url.find("/", startPosi + 3);
	if (startPosi == string::npos || stopPosi == string::npos)
		return "";
	string ret = url.substr(startPosi + 3, stopPosi - startPosi - 3);
	return ret;
}

bool isDigit(const string & str)
{
	if (str == ".")
		return false;
	for (int i = 0; i < str.length(); ++i)
    {
		if (str[i] >= '0' && str[i] <= '9' || str[i] == '.')
			;
		else
			return false;
    }
	return true;
}

bool isSpecialCase(const string & str)
{
	if (str == ".")
		return false;
	if (str == "I")
		return false;
	for (int i = 0; i < str.length(); ++i)
		if (str[i] >= 'A' && str[i] <= 'Z' || str[i] == '.')
			;
		else
			return false;
	return true;
}

bool isWord(const string & str)
{
	for (int i = 0; i < str.length(); ++i)
		if (str[i] >= 'a' && str[i] <= 'z' || str[i] >= 'A' && str[i] <= 'Z')
			;
		else
			return false;
	return true;
}

int getWordLang(const string & str)
{
	if (isDots(str))
		return 0;
	for (int i = 0; i < str.length(); ++i)
		if (str[i] < 0)
			return 1;
	return -1;
}

string toString(int x)
{
	char temp[100];
	sprintf(temp, "%d", x);
	return string(temp);
}

string toString(double x)
{
	char temp[100];
	sprintf(temp, "%lf", x);
	return string(temp);
}

};//end of namespace StringOperation


}; //end of namespace mt_parallel
