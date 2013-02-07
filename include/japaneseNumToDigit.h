/*
 * japaneseNumToDigit.h
 * Created on 2010.5.12
 *      Author yuandong.xu
 */

#ifndef JAPANESE_NUM_TO_DIGIT
#define JAPANESE_NUM_TO_DIGIT

#include<vector>
#include<string>
#include<set>
#include<iostream>
#include<map>

using namespace std;

class JapaneseNumToDigit
{
  private:
    map<string,int> SBC_Num;
    map<string,int> Japanese_Num;
    map<string,int> Japanese_Quanti;
    map<string, int> result;
  public:
    JapaneseNumToDigit()
    {
      SBC_Num["１"]=1;
      SBC_Num["２"]=2;
      SBC_Num["３"]=3;
      SBC_Num["４"]=4;
      SBC_Num["５"]=5;
      SBC_Num["６"]=6;
      SBC_Num["７"]=7;
      SBC_Num["８"]=8;
      SBC_Num["９"]=9;
      SBC_Num["０"]=0;
      
      Japanese_Num["○"] = 0;
      Japanese_Num["零"] = 0;
      Japanese_Num["ゼロ"]=0;
      Japanese_Num["一"]=1;
      Japanese_Num["二"]=2;
      Japanese_Num["三"]=3;
      Japanese_Num["四"]=4;
      Japanese_Num["五"]=5;
      Japanese_Num["六"]=6;
      Japanese_Num["七"]=7;
      Japanese_Num["八"]=8;
      Japanese_Num["九"]=9;
      Japanese_Num["十"]=10;
      
      Japanese_Quanti["十"]=10;
      Japanese_Quanti["百"]=100;
      Japanese_Quanti["千"]=1000;
      Japanese_Quanti["万"]=10000;
      Japanese_Quanti["億"]=(int)1e8;
    }

    map<string, int> getNums(vector<string> & words)
    {
        result.clear();
        
        vector<string> combine;
        for (int i = 0; i < words.size(); ++i)
        {
            //cout << "word:" << words[i] << endl;
            if (Japanese_Num.find(words[i]) != Japanese_Num.end())
            {
                int j;
                for (j = i; j < words.size(); ++j)
                {
                    if (Japanese_Num.find(words[j])!=Japanese_Num.end() || isQuanti(words[j]))
                    {
                        combine.push_back( words[j] );
                    }
                    else
                    {
                        break;
                    }
                }
               // for (int k = 0; k < combine.size(); ++k)
               //     cout << combine[k] << " ";
               // cout << endl;
                string tnumstr = translateJapanese(combine);
                combine.clear();
                
                //cout << "ok: " << tnumstr << endl;
                result[tnumstr]++;
                i = j ;
                //cout <<"index: " << i << " " << j << " " << words.size() << endl;
                
            }
        }

        return result;
    }
    
  private:
    string translateJapanese(vector<string> & NumStr)
    {
        int tNum = 0;
        for(int i = 0; i < NumStr.size(); ++i)
        {
            if (i == 0 && Japanese_Num.find(NumStr[i]) != Japanese_Num.end())
            {
                tNum = Japanese_Num[NumStr[i]];
                //cout <<tNum << endl;
                continue;
            }
            if (Japanese_Num.find(NumStr[i]) != Japanese_Num.end())
            {
                if (NumStr[i] != "十")
                    tNum += Japanese_Num[NumStr[i]];               
            }
            if (isQuanti(NumStr[i]))
            {
                tNum *= Japanese_Quanti[NumStr[i]]; 
            }
        }
      
        char buf[50];
        sprintf(buf, "%d", tNum);
        
        return string(buf);
    }

    bool isQuanti(string s)
    {
      if(s=="十" || s=="百" || s=="千" || s=="万" || s=="億")
        return true; 
      else
        return false;
    }

    bool isPunctuation(string s)
    {
      if(s=="·" || s == "．")
        return true;
      else
        return false;
    }

    
};



#endif
