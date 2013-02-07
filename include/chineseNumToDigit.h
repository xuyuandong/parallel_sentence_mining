#ifndef CHINESE_NUM_TO_DIGIT
#define CHINESE_NUM_TO_DIGIT

#include<vector>
#include<string>
#include<set>
#include<iostream>
#include<map>

using namespace std;

//[type]: 0 is common word, 1 is number, 2 is quantifier
class SpecialTrans{
  public:	
    string translation;
    int type;
    SpecialTrans(string trans,int t){
      translation=trans;
      type=t;
    }
    SpecialTrans & operator=(const SpecialTrans &st){
      translation=st.translation;
      type=st.type;
      return *this;
    }
};

class ChineseNumToDigit
{
  public:
    const int static Chinese_Word_Length=3,type_Num=-11,type_Date=-12,type_Time=-13;
    string num_tag,date_tag,time_tag;
    map<string,int> SBC_Num,Chinese_Num,Chinese_Complex_Num,Chinese_Quanti;
    map<string,int> English_Num,English_Ordinal,English_Quanti;
    set<string> Chinese_Date,Chinese_Time;
    set<string> English_Date,English_Time;
    vector<SpecialTrans> result;

    void init()
    {
      result.clear();
    }

    ChineseNumToDigit(){
      num_tag="/num",date_tag="/date",time_tag="/time";
      result.clear();
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
      Chinese_Num["零"]=0;
      Chinese_Num["一"]=1;
      Chinese_Num["两"]=2;
      Chinese_Num["二"]=2;
      Chinese_Num["三"]=3;
      Chinese_Num["四"]=4;
      Chinese_Num["五"]=5;
      Chinese_Num["六"]=6;
      Chinese_Num["七"]=7;
      Chinese_Num["八"]=8;
      Chinese_Num["九"]=9;
      Chinese_Num["十"]=10;
      Chinese_Complex_Num["○"]=0;
      Chinese_Complex_Num["壹"]=1;
      Chinese_Complex_Num["贰"]=2;
      Chinese_Complex_Num["叁"]=3;
      Chinese_Complex_Num["肆"]=4;
      Chinese_Complex_Num["伍"]=5;
      Chinese_Complex_Num["陆"]=6;
      Chinese_Complex_Num["柒"]=7;
      Chinese_Complex_Num["捌"]=8;
      Chinese_Complex_Num["玖"]=9;
      Chinese_Quanti["个"]=1;
      Chinese_Quanti["十"]=10;
      Chinese_Quanti["百"]=100;
      Chinese_Quanti["千"]=1000;
      Chinese_Quanti["万"]=10000;
      Chinese_Quanti["千万"]=(int)1e7;
      Chinese_Quanti["亿"]=(int)1e8;
      Chinese_Date.insert("世纪");
      Chinese_Date.insert("年代");
      Chinese_Date.insert("年");
      Chinese_Date.insert("月");
      Chinese_Date.insert("月份");
      Chinese_Date.insert("日");
      Chinese_Date.insert("号");
      Chinese_Time.insert("时");
      Chinese_Time.insert("点");
      Chinese_Time.insert("点钟");
      Chinese_Time.insert("点整");
      Chinese_Time.insert("分");
      Chinese_Time.insert("秒");
      English_Num["one"]=1;
      English_Num["two"]=2;
      English_Num["three"]=3;
      English_Num["four"]=4;
      English_Num["five"]=5;
      English_Num["six"]=6;
      English_Num["seven"]=7;
      English_Num["eight"]=8;
      English_Num["nine"]=9;
      English_Num["ten"]=10;
      English_Num["eleven"]=11;
      English_Num["twelve"]=12;
      English_Num["thirteen"]=13;
      English_Num["fourteen"]=14;
      English_Num["fifteen"]=15;
      English_Num["sixteen"]=16;
      English_Num["seventeen"]=17;
      English_Num["eighteen"]=18;
      English_Num["nineteen"]=19;
      English_Num["twenty"]=20;
      English_Num["tirty"]=30;
      English_Num["fourty"]=40;
      English_Num["fifty"]=50;
      English_Num["sixty"]=60;
      English_Num["seventy"]=70;
      English_Num["eighty"]=80;
      English_Num["ninety"]=90;
      English_Ordinal["first"]=1;
      English_Ordinal["second"]=2;
      English_Ordinal["third"]=3;
      English_Ordinal["fourth"]=4;
      English_Ordinal["fifth"]=5;
      English_Ordinal["sixth"]=6;
      English_Ordinal["seventh"]=7;
      English_Ordinal["eighth"]=8;
      English_Ordinal["nineth"]=9;
      English_Ordinal["tenth"]=10;
      English_Ordinal["eleventh"]=11;
      English_Ordinal["twelfth"]=12;
      English_Ordinal["thirteenth"]=13;
      English_Ordinal["fourteenth"]=14;
      English_Ordinal["fifteenth"]=15;
      English_Ordinal["sixteenth"]=16;
      English_Ordinal["seventeenth"]=17;
      English_Ordinal["eighteenth"]=18;
      English_Ordinal["nineteenth"]=19;
      English_Ordinal["twentieth"]=20;
      English_Ordinal["tirtieth"]=30;
      English_Ordinal["fourtieth"]=40;
      English_Ordinal["fiftieth"]=50;
      English_Ordinal["sixtieth"]=60;
      English_Ordinal["seventieth"]=70;
      English_Ordinal["eightieth"]=80;
      English_Ordinal["ninetieth"]=90;
      English_Quanti["hundred"]=100;
      English_Quanti["hundreds"]=100;
      English_Quanti["thousand"]=1000;
      English_Quanti["thousands"]=1000;
      English_Quanti["million"]=(int)1e6;
      English_Quanti["millions"]=(int)1e6;
      English_Quanti["billion"]=(int)1e9;
      English_Quanti["billions"]=(int)1e9;
      English_Date.insert("century");
      English_Date.insert("centuries");
      English_Date.insert("year");
      English_Date.insert("years");
      English_Date.insert("January");
      English_Date.insert("February");
      English_Date.insert("March");
      English_Date.insert("April");
      English_Date.insert("May");
      English_Date.insert("June");
      English_Date.insert("July");
      English_Date.insert("Augst");
      English_Date.insert("September");
      English_Date.insert("October");
      English_Date.insert("November");
      English_Date.insert("December");
      English_Date.insert("months");
      English_Date.insert("Sunday");
      English_Date.insert("Monday");
      English_Date.insert("Tuesday");
      English_Date.insert("Wednesday");
      English_Date.insert("Thursday");
      English_Date.insert("Friday");
      English_Date.insert("Saturday");
      English_Time.insert("seconds");
      English_Time.insert("minute");
      English_Time.insert("minutes");
      English_Time.insert("hour");
      English_Time.insert("hours");
    }

    bool isPunctuation(string s){
      if(s=="·")
        return true;
      else
        return false;
    }

    bool isArabiaNum(string s){
      for(int i=0;i<s.length();i++){
        if(s[i]>='0' && s[i]<='9')
          return true;
        if( i%Chinese_Word_Length==0 && i+Chinese_Word_Length<=s.length() ){
          string substring=s.substr(i,Chinese_Word_Length);
          if(SBC_Num.find(substring)!=SBC_Num.end())
            return true;
        }
      }
      return false;
    }

    void translateArabia(vector<string> sentence,int &pos){
      if(pos+1 >= sentence.size() ){
        result.push_back(SpecialTrans(sentence[pos],type_Num));
        return;
      }
      bool hasQuanti=true;
      SpecialTrans st=SpecialTrans("null",-11);
      int type=type_Num;
      string nextWord;
      if(pos+1<sentence.size())
        nextWord=sentence[pos+1];
      if(Chinese_Date.find(nextWord)!=Chinese_Date.end())
        type=type_Date;
      else if(Chinese_Time.find(nextWord)!=Chinese_Time.end())
        type=type_Time;
      if(sentence[pos+1]=="十")
        result.push_back(st=SpecialTrans(sentence[pos]+"0",type));
      else if(sentence[pos+1]=="百")
        result.push_back(st=SpecialTrans(sentence[pos]+"00",type));
      else if(sentence[pos+1]=="千")
        result.push_back(st=SpecialTrans(sentence[pos]+",000",type));
      else if(sentence[pos+1]=="万")
        result.push_back(st=SpecialTrans(sentence[pos]+"0,000",type));
      else if(sentence[pos+1]=="亿")
        result.push_back(st=SpecialTrans(sentence[pos]+"00,000,000",type));
      else{
        result.push_back(st=SpecialTrans(sentence[pos],type));
        hasQuanti=false;
      }	
      if(hasQuanti){
        result.push_back(st=SpecialTrans("null",2));
        pos++;
      }	
    }

    bool isQuanti(string s){
      if(s=="十" || s=="什"
          || s=="百" || s=="佰"
          || s=="千" || s=="仟"
          || s=="万" || s=="亿")
        return true; 
      else
        return false;
    }

    //used in reconstruct sentence
    bool isChineseNum(string s){
      int countComplex=0;
      if(s=="一五一十")
        return false;
      else if(s.length()<Chinese_Word_Length)
        return false;
      else if(isPunctuation(s))
        return false;
      for(int i=0; i+Chinese_Word_Length<=s.length(); i+=Chinese_Word_Length){
        string substring=s.substr(i,Chinese_Word_Length);
        bool flag=false;	//if it's not num or quanti, then it's not num
        if(Chinese_Num.find(substring)!=Chinese_Num.end()
            || Chinese_Complex_Num.find(substring)!=Chinese_Complex_Num.end())
          flag=true;
        if(flag==false && !isQuanti(substring))
          return false;
      }
      return true;
    }

    bool isChineseDateTime(string s){
      int countNum=0,countDateTime=0;
      for(int i=0;i+Chinese_Word_Length<=s.length();){
        bool isArabiaNum=false;
        string substring,doubleSubstring;
        if(s[i]&0x80){
          substring=s.substr(i,Chinese_Word_Length);
          i+=Chinese_Word_Length;
        }
        else if(s[i]>='0' && s[i]<='9'){
          substring=s.substr(i,1);
          isArabiaNum=true;
          i++;
        }
        //for the word "世纪","年代"
        if(i+2*Chinese_Word_Length<=s.length())
          doubleSubstring=s.substr(i,2*Chinese_Word_Length);
        //if it's not date, time ,num word, then it's false
        if(Chinese_Num.find(substring)!=Chinese_Num.end()
            || Chinese_Complex_Num.find(substring)!=Chinese_Complex_Num.end()
            || isArabiaNum)
          countNum++;
        else if(Chinese_Date.find(doubleSubstring)!=Chinese_Date.end()
            || Chinese_Time.find(doubleSubstring)!=Chinese_Time.end()){
          countDateTime++;
          i+=Chinese_Word_Length;
        }	
        else if(Chinese_Date.find(substring)!=Chinese_Date.end()
            || Chinese_Time.find(substring)!=Chinese_Time.end())
          countDateTime++;
        else
          return false;
      }
      if(countNum>0 && countDateTime>0)
        return true;
      else
        return false;
    }

    //segment "十九世纪" to "十九 世纪"
    vector<string> convertChineseDateTime(string s){
      vector<string> result;
      string num="";
      for(int i=0;i+Chinese_Word_Length<=s.length();){
        bool isArabiaNum=false;
        string substring,doubleSubstring;
        if(s[i]&0x80){
          substring=s.substr(i,Chinese_Word_Length);
          i+=Chinese_Word_Length;
        }
        else if(s[i]>='0' && s[i]<='9'){
          substring=s.substr(i,1);
          isArabiaNum=true;
          i++;
        }
        //for the word "世纪","年代"
        if(i+2*Chinese_Word_Length<=s.length())
          doubleSubstring=s.substr(i,2*Chinese_Word_Length);
        //if it's not date, time ,num word, then it's false
        if(Chinese_Num.find(substring)!=Chinese_Num.end()
            || Chinese_Complex_Num.find(substring)!=Chinese_Complex_Num.end()
            || isArabiaNum)
          num+=substring;		
        else if(Chinese_Date.find(doubleSubstring)!=Chinese_Date.end()
            || Chinese_Time.find(doubleSubstring)!=Chinese_Time.end()){
          result.push_back(num);
          num="";
          result.push_back(doubleSubstring);	
          i+=Chinese_Word_Length;
        }
        else if(Chinese_Date.find(substring)!=Chinese_Date.end()
            || Chinese_Time.find(substring)!=Chinese_Time.end()){
          result.push_back(num);
          num="";
          result.push_back(substring);
        }
      }
      return result;
    }

    //resegment Chinese sentence
    vector<string> convertChineseSen(vector<string> initSen){
      //combine those like "五 十 六"
      vector<string> combineResult;
      for(int i=0;i<initSen.size();i++){
        string word=initSen[i];
        if(!isChineseNum(word) && !isQuanti(word)){
          combineResult.push_back(word);
          continue;
        }
        bool combine=false;
        int p;
        for(p=i+1;p<initSen.size();p++){
          string nextWord=initSen[p];
          if(!isArabiaNum(nextWord) && !isChineseNum(nextWord)
              && !isQuanti(nextWord))
            break;
          word+=nextWord;
          combine=true;
        }
        combineResult.push_back(word);
        if(combine)
          i=p-1;
      }

      //reconstruct "一百多万" , "二十世纪"
      vector<string> result;
      for(int i=0;i<combineResult.size();i++)
      {
        string word=combineResult[i],next,evenNext;
        //process the Date and Time
        if(isChineseDateTime(word))
        {
          vector<string> convertedWord=convertChineseDateTime(word);
          for(int t=0;t<convertedWord.size();t++)
          {
            result.push_back(convertedWord[t]);
          }
          continue;
        }

        if(i+1<combineResult.size())
          next=combineResult[i+1];
        if(i+2<combineResult.size())
          evenNext=combineResult[i+2];
        //if(next!="多")	
        result.push_back(word);
        //else
        //{
        //  if(Chinese_Quanti.find(evenNext)!=Chinese_Quanti.end())
        //  {
        //    result.push_back(word+evenNext);
        //    result.push_back(next);
        //    i+=2;
        //  }
        //}
      }
      return result;
    }

    //used to decide whether to translate
    bool isPureChineseNum(vector<string> sentence,int pos){
      //delete those like "千万"
      string s=sentence[pos];
      string firstWord;
      if(s.length()>=Chinese_Word_Length)
        firstWord=s.substr(0,Chinese_Word_Length);
      int countChineseNum=0;
      for(int i=0;i+Chinese_Word_Length<=s.length(); i+=Chinese_Word_Length){
        string substring=s.substr(i,Chinese_Word_Length);
        if(Chinese_Num.find(substring)!=Chinese_Num.end()
            || Chinese_Complex_Num.find(substring)!=Chinese_Complex_Num.end())
          countChineseNum++;
      }
      if(countChineseNum==0 && firstWord!="十")
        return false;
      //delete those like "尝一尝"
      string former="",next="";
      if(pos>0)
        former=sentence[pos-1];
      if(pos+1<sentence.size())
        next=sentence[pos+1];
      //but we should avoid "每年九月十月", here "十" is num
      if(former==next && former!="" 
          && Chinese_Date.find(former)!=Chinese_Date.end()
          && Chinese_Time.find(former)!=Chinese_Time.end())
        return false;
      //delete thoes like "其中之一" and "几十个"
      if(former=="之" || former=="几")
        return false;

      return true;
    }

    void translateChinese(vector<string> sentence,int pos){
      string ChineseNum=sentence[pos];
      int finalNum=0,tempNum=0;
      for(int i=0; i+Chinese_Word_Length<=ChineseNum.length(); i+=Chinese_Word_Length){
        string substring=ChineseNum.substr(i,Chinese_Word_Length);
        string subFormerString="",subNextString="";
        //process the number begin with "十"
        if(i==0 && substring=="十"){
          tempNum=10;
          if(i+Chinese_Word_Length<=ChineseNum.length()){
            subNextString=ChineseNum.substr(i+Chinese_Word_Length,Chinese_Word_Length);
            if(Chinese_Num.find(subNextString)!=Chinese_Num.end()){
              tempNum+=Chinese_Num[subNextString];
              i+=Chinese_Word_Length;	
            }		
          }
          if(i+Chinese_Word_Length>=ChineseNum.length())
            finalNum=tempNum;
          continue;
        }
        if(i>0)
          subFormerString=ChineseNum.substr(i-Chinese_Word_Length,Chinese_Word_Length);
        if(i+Chinese_Word_Length<=ChineseNum.length())
          subNextString=ChineseNum.substr(i+Chinese_Word_Length,Chinese_Word_Length);
        if(isChineseNum(substring) && !isQuanti(substring)){
          if(isQuanti(subFormerString))
            finalNum+=tempNum;
          tempNum=0; 
          if(Chinese_Num.find(substring)!=Chinese_Num.end())
            tempNum=Chinese_Num.find(substring)->second; 
          else if(Chinese_Complex_Num.find(substring)!=Chinese_Complex_Num.end())
            tempNum=Chinese_Complex_Num.find(substring)->second;
          if(!isQuanti(subFormerString)){
            finalNum*=10;
            //if it's 一千零三，then we divide 10 
            if(subFormerString=="零" && i-Chinese_Word_Length>0){
              string subEvenFormer=
                ChineseNum.substr(i-2*Chinese_Word_Length,
                    Chinese_Word_Length);
              if(subEvenFormer=="千" || subEvenFormer=="万"
                  || subEvenFormer=="亿")
                finalNum/=10;
            }
          }
          if(!isQuanti(subNextString) || subNextString==""){
            if(isQuanti(subFormerString))
              if(subFormerString=="百" || subFormerString=="佰")
                tempNum*=10;
              else if(subFormerString=="千" || subFormerString=="仟")
                tempNum*=100;
              else if(subFormerString=="万")
                tempNum*=1000;
              else if(subFormerString=="亿")
                tempNum*=10000000;
            finalNum+=tempNum;
          }	
        }	
        else if(isQuanti(substring)){
          if(substring=="十" || substring=="什")
            tempNum*=10;
          else if(substring=="百" || substring=="佰")
            tempNum*=100;
          else if(substring=="千" || substring=="仟")
            tempNum*=1000;
          else if(substring=="万")
            tempNum*=10000;
          else if(substring=="亿")
            tempNum*=100000000;
          if(subNextString=="")
            finalNum+=tempNum;
        }
        //cout<<"i="<<i<<" word="<<substring<<" finalNum="<<finalNum<<" tempNum="<<tempNum<<endl;
      }
      //cout<<"finalNum = "<<finalNum<<endl;
      char resultNum[1000],tempResult[1000];
      sprintf(resultNum,"%d",finalNum);

      string nextWord;
      if(pos+1<sentence.size())
        nextWord=sentence[pos+1];
      if(Chinese_Date.find(nextWord)!=Chinese_Date.end())
        result.push_back(SpecialTrans(resultNum,type_Date));
      else if(Chinese_Time.find(nextWord)!=Chinese_Time.end())
        result.push_back(SpecialTrans(resultNum,type_Time));
      else
        result.push_back(SpecialTrans(resultNum,type_Num));
    }

    bool isEnglishNum(string s){
      if(English_Num.find(s)!=English_Num.end()
          || English_Ordinal.find(s)!=English_Ordinal.end()
          || English_Quanti.find(s)!=English_Quanti.end())
        return true;
      else
        return false;
    }

    bool isEnglishDate(string s){
      if(English_Date.find(s)!=English_Date.end())
        return true;
      else
        return false;
    }

    bool isEnglishTime(string s){
      if(English_Time.find(s)!=English_Time.end())
        return true;
      else
        return false;
    }

    //combine english number to one word
    //and denote num,date,time 
    vector<string> convertEnglishSen(vector<string> initSen){
      init();
      vector<string> result;
      bool formerIsNum=false;	
      for(int i=0;i<initSen.size();i++){
        if(!isEnglishNum(initSen[i])){
          formerIsNum=false;
          result.push_back(initSen[i]);
        }
        else{
          if(formerIsNum)
            continue;
          else{
            formerIsNum=true;
            if( i+1<initSen.size() && isEnglishDate(initSen[i+1])){
              formerIsNum=false;
              result.push_back(date_tag);
            }	
            else if( i+1<initSen.size() && isEnglishTime(initSen[i+1])){
              formerIsNum=false;
              result.push_back(time_tag);
            }	
            else
              result.push_back(num_tag);
          }
        }
      }
      return result;
    }

    vector<SpecialTrans> transnum(vector<string> initSen){
      init();
      vector<string> sentence=convertChineseSen(initSen);
      for(int i=0;i<sentence.size();i++){
        if( isArabiaNum(sentence[i]) ){
          translateArabia(sentence,i);
        }	
        else if( isChineseNum(sentence[i]) && isPureChineseNum(sentence,i) ){
          translateChinese(sentence,i);
        }		
        else{
          SpecialTrans st=SpecialTrans(sentence[i],0);
          result.push_back(st);
        }
      }
      return result;
    }

    map<string, int> getNums(vector<string> & words)
    {
      vector<SpecialTrans> ret = transnum(words);
      map<string, int> result;

      for (int i = 0; i < ret.size(); ++i) 
        if (ret[i].type != 0) {
          result[ret[i].translation]++;
        }

      return result;
    }

};



#endif
