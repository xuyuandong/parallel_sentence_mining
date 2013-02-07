#ifndef ENGLISH_NUM_TO_DIGITS
#define ENGLISH_NUM_TO_DIGITS

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <set>
#include "mt_string_operation.h"

using namespace mt_parallel;

class EnglishNumToDigit
{
  public:
    map<string, int> dict;

    EnglishNumToDigit()
    {
      dict["zero"] = 0;
      dict["one"] = 1;
      dict["a"] = 1;
      dict["two"] = 2;
      dict["three"] = 3;
      dict["four"] = 4;
      dict["five"] = 5;
      dict["six"] = 6;
      dict["seven"] = 7;
      dict["eight"] = 8;
      dict["nine"] = 9;
      dict["ten"] = 10;
      dict["eleven"] = 11;
      dict["twelve"] = 12;
      dict["thirteen"] = 13;
      dict["fourteen"] = 14;
      dict["fifteen"] = 15;
      dict["sixteen"] = 16;
      dict["seventeen"] = 17;
      dict["eighteen"] = 18;
      dict["nineteen"] = 19;
      dict["twenty"] = 20;
      dict["thirty"] = 30;
      dict["forty"] = 40;
      dict["fifty"] = 50;
      dict["sixty"] = 60;
      dict["seventy"] = 70;
      dict["eighty"] = 80;
      dict["ninety"] = 90;

      map<int, string> reverse;
      reverse[20] = "twenty";
      reverse[30] = "thirty";
      reverse[40] = "forty";
      reverse[50] = "fifty";
      reverse[60] = "sixty";
      reverse[70] = "seventy";
      reverse[80] = "eighty";
      reverse[90] = "ninety";
      reverse[1] = "one";
      reverse[2] = "two";
      reverse[3] = "three";
      reverse[4] = "four";
      reverse[5] = "five";
      reverse[6] = "six";
      reverse[7] = "seven";
      reverse[8] = "eight";
      reverse[9] = "nine";

      for (int i = 2; i <= 9; ++i)
        for (int j = 1; j <= 9; ++j)
          dict[reverse[i*10] + "-" + reverse[j]] = i * 10 + j;

      dict["hundred"] = 100;
      dict["thousand"] = 1000;
      dict["million"] = 1000000;
      dict["billion"] = 1000000000;
    }

    inline string toString(int x)
    {
      char temp[20];
      sprintf(temp, "%d", x);
      return string(temp);
    }

    inline bool isNum(string & input)
    {
      if (input == ".") return false;
      if (input == "") return false;
      for (int i = 0; i < input.size(); ++i) 
        if (input[i] >= '0' && input[i] <= '9' || input[i] == '.');
        else return false;
      return true;
    }

    map<string, int> getNums(vector<string> & words) 
    {
      map<string, int> answer;
      int state = 0;
      int nownum = 0;
      int lastnum = 0;
      for (int i = 0; i < words.size(); ++i) {
        if (isNum(words[i])) 
          answer[words[i]]++;
        string lowerWord = StringOperation::tolower(words[i]);
        if (dict.find(lowerWord) == dict.end()) {
          if (lowerWord == "and") continue;
          if (state == 1) 
            answer[toString(nownum)]++;
          state = 0;
          nownum = 0;
          lastnum = 0;
        }
        else {
          if (state == 0) {
            state = 1;
            nownum = dict[lowerWord];
            lastnum = nownum;
          }
          else {
            int curNum = dict[lowerWord];
            if (curNum >= 100) {
              nownum -= lastnum;
              nownum += lastnum * curNum;
            }
            else {
              nownum += curNum;
              lastnum = curNum;
            }
          }
        }
      }
      if (nownum > 0) 
        answer[toString(nownum)]++;
      return answer;
    }
};

#endif
