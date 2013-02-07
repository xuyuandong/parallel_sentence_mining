/*
 * mt_sentence.cpp
 *
 *  Created on: Apr 11, 2010
 *      Author: yuandong.xu
 */
#include "mt_sentence.h"
#include "mt_dictionary.h"
#include "mt_similarity_model.h"
#include "mt_special_charset.h"
#include "mt_string_operation.h"
#include "mt_log_file.h"
#include "japaneseNumToDigit.h"
#include "chineseNumToDigit.h"
#include "englishNumToDigit.h"
#include "codedetector/language_detector.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <cmath>

using namespace galaxy::utility::codedetector;
using namespace std;

namespace mt_parallel
{

static JapaneseNumToDigit japConvert;
static ChineseNumToDigit chiConvert;
static EnglishNumToDigit engConvert;

/** for post-sentence processing ... */
static map<string, string> bigTosmall;
static set<string> newSentenceStr;
static set<string> leadingWord;
static set<string> openSet;
static set<string> closeSet;
static const string care[] = { "t", "m", "re", "ll", "ve", "s", "d" };

void initNonsenseCharSet()
{
	// special charset
	static const string newSentence[] = { "、", "：" , ":", "/", ","  };
	
	static const string openString[] = { "{", "｛", "(", "[", "（", "［", "〖", "〘", "〚", "【", "〔" };
	static const string closeString[] = { "}", "｝", ")", "]", "）", "］", "〗", "〙", "〛", "】", "〕"};
//	static const string bigdigit[] = { "０", "１", "２", "３", "４", "５", "６", "７", "８", "９" };
    static const string bigdigit[] = {"０", "１", "２","３","４","５", "６", "７", "８", "９"};
    
	static const string bigdigit2[] = {  "①","②","③","④","⑤","⑥","⑦","⑧","⑨","⑩" };
	static const string bigdigit3[] = {  "⑴", "⑵", "⑶", "⑷", "⑸", "⑹", "⑺", "⑻", "⑼", "⑽" };
	static const string bigdigit4[] = { "⒈", "⒉", "⒊", "⒋", "⒌", "⒍", "⒎", "⒏", "⒐", "⒑" };
	static const string smalldigit[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };
				
	// initialize
	static bool flag = true;
	if (flag) 
	{
		flag = false;
		for (int i = 0; i < 10; ++i) 
		{
			bigTosmall[bigdigit[i]] = smalldigit[i];
			bigTosmall[bigdigit2[i]] = smalldigit[i + 1];
			bigTosmall[bigdigit3[i]] = smalldigit[i + 1];
			bigTosmall[bigdigit4[i]] = smalldigit[i + 1];
			leadingWord.insert(bigdigit2[i]);
			leadingWord.insert(bigdigit3[i]);
			leadingWord.insert(bigdigit4[i]);
		}
		for (int i = 0; i < 5; ++i)
			newSentenceStr.insert(newSentence[i]);
		for (int i = 0; i < 11; ++i) 
		{
			openSet.insert(openString[i]);
			closeSet.insert(closeString[i]);
		}
	}

}

/** for post-sentence processing */
static map<string, string> dotChToEn;
static map<string, string> dotEnToCh;

void initDotSet()
{
	static string chiDots[] = { "。", "，", "；", "：", "？", "！", "……", "—",
			"～", "〔", "〕", "《", "》", "‘", "’", "“", "”", "`", "．", "【",
			"】", "、" };

	static string engDots[] = { ".", ",", ";", ":", "?", "!", "…", "-",
			"~", "(", ")", "<", ">", "'", "'", "\"", "\"", "'", ".", "[",
			"]", "、" };

	static bool flag = true;
	if (flag) 
	{
		flag = false;
		for (int i = 0; i < 22; ++i) 
		{
			dotChToEn[chiDots[i]] = engDots[i];
			dotEnToCh[engDots[i]] = chiDots[i];
		}
	}
}

/** For sentence similarity calculation */

int intersectSize(const map<string, int>& a, const map<string, int>& b)
{
    int ret = 0;
    for (map<string, int>::const_iterator iter = a.begin(); iter != a.end(); ++iter)
    {
        map<string,int>::const_iterator itb = b.find(iter->first);
        if (itb != b.end())
            ret += min(itb->second, iter->second);
    }
    return ret;
}

int unionSize(const map<string, int>& a, const map<string, int>& b)
{
    int ret = 0;
    for (map<string, int>::const_iterator iter = a.begin(); iter != a.end(); ++iter)
    {
        map<string,int>::const_iterator itb = b.find(iter->first);
        if (itb != b.end())
            ret += max(itb->second, iter->second);
        else
            ret += iter->second;
    }
    
    for (map<string, int>::const_iterator iter = b.begin(); iter != b.end(); ++iter)
    {
        if(a.find(iter->first) == a.end())
            ret += iter->second;
    }
    
    return ret;
}

/** MtSentence Class : */
    
MtSentence MtSentence::mZero;

MtDictionary* MtSentence::dict;
MtSimilarityModel* MtSentence::model;

MtSentence::MtSentence()
{
    dict = MtDictionary::Instance();
    model = MtSimilarityModel::Instance();
}

MtSentence::~MtSentence()
{
}

MtSentence::MtSentence(const vector<string> & wordlist, const vector<int> & dompathlist)
{
    dict = MtDictionary::Instance();
    model = MtSimilarityModel::Instance();

    words = wordlist;
    dompaths = dompathlist;

    normalize(); 

    m_language = UNDEFINED;
    detectLanguage();

    featureExtract();
}

MtSentence & MtSentence::operator +=(const MtSentence& rhs)
{
	//TODO
	return (*this);
}

//friend function
MtSentence operator +(const MtSentence& lhs, const MtSentence& rhs)
{
    MtSentence result;
    
    result.m_language = lhs.m_language;
    result.m_position = 0.5 * (lhs.m_position + rhs.m_position);

    result.words = lhs.words;
    for (int i = 0; i < rhs.words.size(); ++i)
        result.words.push_back(rhs.words[i]);

    result.dompaths = lhs.dompaths;

    //normalize();

    result.featureExtract();
    
	return result;
}

double MtSentence::calcuSimilarity(const MtSentence & other, bool exportFeature)
{
    if (length() == 0 || other.length() == 0)
        return 0.0;
    if (m_language == other.m_language)
        return 0.0;
   
    // ratio of sentence byte length: Gaussian distribution & scored
    /* old : words number as length */
    double aOldLen = length();
    double bOldLen = other.length();
    double ratioOldLength = aOldLen / max(1.0, bOldLen);
    if (ratioOldLength > 3.0 || ratioOldLength < 0.34) return 0;
   
    double aLen = toString().size();
    double bLen = other.toString().size();

    double ratioCJ = bLen / max(1.0, aLen);
    if (ratioCJ > 2.0 || ratioCJ < 0.15)
        return 0.0;
    const double meanRatio = 0.7;
    const double sigRatio = 0.368;
    const double SIGR2 = 2 * sigRatio * sigRatio;
    double ratioLength = ratioCJ - meanRatio;
    double ratioLengthScore = exp(- ratioLength*ratioLength / SIGR2 );
    
    /* ratio between of content words number :
     * higher: similar
     * lower: un-similar
     */
    double ratioContentALen = (double)contentWordsNum / aLen;
    double ratioContentBLen = (double)other.contentWordsNum / bLen;
    double ratioBetweenContentLen = ratioContentALen / ratioContentBLen;
    if (ratioBetweenContentLen > 1.0) ratioBetweenContentLen = 1.0 / ratioBetweenContentLen;
    //double ratioContentLength = (double)contentWordsNum / other.contentWordsNum;

    /* ratio of dictionary mapping words number
     * both should be the higher, the better
     */
    int matchedWordsNum = intersectSize(cnWords, other.cnWords);
    double ratioAMatchedLen = matchedWordsNum / aLen;    
    double ratioBMatchedLen = matchedWordsNum / bLen;

    /* ratio of dictionary mapping digits number
     */
    int matchedDigitsNum = intersectSize(cnDigits, other.cnDigits);
    int allDigitsNum = unionSize(cnDigits, other.cnDigits);
    double ratioMatchedDigits = (double)matchedDigitsNum / max(1.0, (double)allDigitsNum);
    if (allDigitsNum == 0)
        ratioMatchedDigits = 1;

    // simple matching filtering
    if (ratioAMatchedLen == 0 && ratioBMatchedLen==0)// && ratioMatchedDigits == 0)
        return 0;
    
    /* ratio of dictionary mapping special words number
     * Since the dictionary is far from complete, this feature is not used
     */
    int matchedSpecialsNum = intersectSize(cnSpecials, other.cnSpecials);       
    int allSpecialsNum = unionSize(cnSpecials, other.cnSpecials);     
    double ratioMatchedSpecials = (double)matchedSpecialsNum / max(1.0, (double)allSpecialsNum);
    if (allSpecialsNum == 0)
        ratioMatchedSpecials = 1;

    /* ratio of mapping Punctuation(biaodian) number
     */
    int matchedPunctuationNum = intersectSize(cnPunctuation, other.cnPunctuation);      
    int allPunctuationNum = unionSize(cnPunctuation, other.cnPunctuation);        
    double ratioMatchedPunctuation = (double)matchedPunctuationNum / max(1.0, (double)allPunctuationNum);
    if (allPunctuationNum == 0)
        ratioMatchedPunctuation = 1;

    /* DOM paths distance: ratio between common paths ratios
     * the higher, the more possible of same content region
     * the lower, the more un-related possibility
     */
    int matchedCommonDomPath = 0;
    int minpathsize = min(dompaths.size()-1, other.dompaths.size()-1);
    for (int i = 0; i < minpathsize; ++i)
    {
        if (dompaths[i] != other.dompaths[i])
            break;
        matchedCommonDomPath++;
    }
    double ratioAMatchedDomPath = (double)matchedCommonDomPath / dompaths.size();
    double ratioBMatchedDomPath = (double)matchedCommonDomPath / other.dompaths.size();
    double ratioBetweenDomPath = ratioAMatchedDomPath / ratioBMatchedDomPath;
    if (ratioBetweenDomPath > 1.0) ratioBetweenDomPath = 1.0 / ratioBetweenDomPath;
    
    // simple dom tree level filtering
    if (ratioBetweenDomPath < 0.751)
        return 0;
    if (ratioAMatchedDomPath * ratioBMatchedDomPath < 0.246)
        return 0;
    
    /* ratio between positions which is the rate in the same content session of the same language
     * the more closer, the better
     */
    double distance = fabs(m_position - other.m_position);
    const double sig = 5.68;
    const double SIG2 = 2 * sig * sig;
    double ratioBetweenPosition = exp(- distance*distance / SIG2 );
    
    // feature for max entropy
    vector<pair<string, float> > feature;
    
    feature.push_back(make_pair("ratioLengthScore", ratioLengthScore));//c
    
    feature.push_back(make_pair("ratioBetweenContentLen", ratioBetweenContentLen));
    
    feature.push_back(make_pair("ratioAMatchedLen", ratioAMatchedLen));
    feature.push_back(make_pair("ratioBMatchedLen", ratioBMatchedLen));
    
    feature.push_back(make_pair("ratioMatchedDigits", ratioMatchedDigits));
    //feature.push_back(make_pair("ratioMatchedSpecials", ratioMatchedSpecials));
    feature.push_back(make_pair("ratioMatchedPunctuation", ratioMatchedPunctuation));
    
    feature.push_back(make_pair("ratioBetweenDomPath", ratioBetweenDomPath));
    
    feature.push_back(make_pair("ratioBetweenPosition", ratioBetweenPosition));
    // TODO: others ...
    
	double weight = model->evaluate(feature);

    if (exportFeature)
        exportFeatureToFile(weight, other, feature);
    
    return weight;
    
}

void MtSentence::exportFeatureToFile(const double weight, const MtSentence & other, const vector<pair<string, float> > & feature)
{
    ifstream ftmp("tmpname");
    string furl;
    ftmp >> furl;
    ftmp.close();

    string fname;
    const char* curl = furl.c_str();
    for (int i = 0; i < furl.size(); ++i)
    {
        if (curl[i] == '/')
            fname.push_back('_');
        else
            fname.push_back(curl[i]);
    }
    
    
    ofstream fout(fname.c_str(), ios::app);
    if (fout.fail())
    {
        cout << "fail to open " << fname << endl;
        return;
    }
    
    //MtLogFile* fout = MtLogFile::Instance();
    
    fout << weight << "\n";//endl;
    fout << "JAPANESE" << "[<split>]";
    fout << toString() << "[<split>]";
    for (int i = 0; i < dompaths.size(); ++i)
        fout << dompaths[i] << " ";
    fout << sid << "[<split>]";
    fout << "CHINESE" << "[<split>]";
    fout << other.toString() << "[<split>]";
    for (int i = 0; i < other.dompaths.size(); ++i)
        fout << other.dompaths[i] << " ";
    fout << other.sid << "[<split>]";
    
    for (int i = 0; i < feature.size(); ++i)
    {
        fout << feature[i].first << ":" << feature[i].second << "[<split>]";
    }

    //fout << m_position << " " << other.m_position << "\n";

/*    
    map<string, int>::const_iterator it;
    for (it = cnDigits.begin(); it != cnDigits.end(); ++it)
        fout << (*it).first << ":" << (*it).second << " ";
    fout << "[<split>]";
    for (it = other.cnDigits.begin(); it != other.cnDigits.end(); ++it)
        fout << (*it).first << ":" << (*it).second << " ";
    fout << "[<split>]";
*/  

    fout << "\n";
    fout.close();

}


void MtSentence::normalize()
{
	initNonsenseCharSet();
	
    vector<string> newwords;
    
	for (int i = 0; i < words.size(); ++i) 
	{
		/** Jump over leading digits header */
		if (i == 0 && leadingWord.find(words[i]) != leadingWord.end()) 
			continue;

		/** Normalize the digit character to standard digits */
		if (bigTosmall.find(words[i]) != bigTosmall.end())
        {
			words[i] = bigTosmall[words[i]];
            // Merge next digits into same words
			int e = newwords.size() - 1;
            if (e > 0)
            {
                if (StringOperation::isDigit(newwords[e]))
                {
                    newwords[e] += words[i];
                    continue;
                }
            }
        }

		/** Open & Close bracket */
		if (openSet.find(words[i]) != openSet.end())
			words[i] = "(";
		if (closeSet.find(words[i]) != closeSet.end())
			words[i] = ")";
		
		/** Japanese quotation marks
         * Need to be recover when output this sentence */
		if ("「" == words[i])
			words[i] = "“";
		if ("」" == words[i])
			words[i] = "”";
        if ("『" == words[i])
            words[i] = "《";
        if ("』" == words[i])
            words[i] = "》";
        if ("、" == words[i])
            words[i] = "，";
        
		/** Skip the nonsense word */
		string & word = words[i];
		if (word.length() == 0)
			continue;
		if (word == " ")
			continue;
		if (word == "\t")
			continue;
		if (word == "　")
			continue;
		if (word == "\r")
			continue;

		/** Deal with 1. 2, prefix of the sentence */
		bool needClear = false;
		if (words[i] == ")") 
		{
			if (newwords.size() == 2 && newwords[0] == "(")
				needClear = true;
			if (newwords.size() == 1)
				needClear = true;
		}
		if (needClear) 
		{
			newwords.clear();
			continue;
		}

		/** Deal with U.S. */
		if (word == "." && newwords.size() > 0) 
		{
            if (SpecialCaseOfSplit::judge(newwords[newwords.size()-1])) 
			{
                newwords[newwords.size() - 1] += ".";
                
				if (newwords.size() >= 2) 
				{
					string & lastlastWord = newwords[newwords.size() - 2];
					if (lastlastWord[lastlastWord.length() - 1] == '.') 
					{
						newwords[newwords.size() - 2] += newwords[newwords.size() - 1];
						newwords.pop_back();
					}
				}
                continue;
			}
		}

		/** Deal with 's */
		string lastWord = "";
		if (newwords.size() > 1)
			lastWord = newwords[newwords.size() - 1];

		if (lastWord == "'" || lastWord == "’") 
		{
			bool needMerge = false;
			for (int i = 0; i < 7; ++i)
            {
				if (word == care[i])
					needMerge = true;
            }
			if (needMerge) 
			{
				newwords[newwords.size() - 2] += "'" + word;
                newwords.pop_back();
				continue;
			}
		}

		newwords.push_back(word);
	}
	words = newwords;

	/** Deal with 100,000, convert 100 , 000 to 100000 */
	vector<string> refineWords;
	for (int i = 0; i < words.size(); ++i) 
	{
		if (words[i] == "," && i > 0 && StringOperation::isDigit(words[i-1]) &&
			i < words.size() - 1 && StringOperation::isDigit(words[i+1]) && refineWords.size() > 0) 
		{
			refineWords[refineWords.size() - 1] += words[i + 1];
			i++;
		} else {
			refineWords.push_back(words[i]);
		}
	}
	words = refineWords;

	/** remove the start ( or tail ) character */
	if (words.size() > 0)
	{
		if (words[0] == ")" || words[0] == "]" || words[0] == "）"	|| words[0] == "］")
			words.erase(words.begin());
	}
	if (words.size() > 0)
	{
		if (words[words.size() - 1] == "(" || words[words.size() - 1] == "[" 
			|| words[words.size() - 1] == "（"  || words[words.size() - 1] == "［")
			words.pop_back();
	}

	/** Remove the leading nonsense character */
	int openclose = 0;
	for (int i = 0; i < words.size(); ++i) 
	{
		if (words[i] == "(")
			openclose++;
		if (words[i] == ")")
			openclose--;
	}
	if (openclose != 0) 
	{
		if (words.size() > 0 && words[0] == "(")
			words.erase(words.begin());
		if (words.size() > 0 && words[words.size() - 1] == ")")
			words.pop_back();
	}
	
	/** Other cases ... */
}

void MtSentence::detectLanguage()
{
    MtLogFile* m_logfile = MtLogFile::Instance();
    
    // Since English sentence needs to be splited by whitespace,
    // we process this case specially !
    int engwords = 0;
    int totalwords = 0;
    string a_sentence;
    bool prevEnglish = true;
    for (int i = 0; i < words.size(); ++i)
    {
        if (!StringOperation::isDots(words[i]) 
                && !StringOperation::isDigit(words[i]) 
                && !StringOperation::isSpecialCase(words[i]))
        {
            ++totalwords;
            if (StringOperation::isWord(words[i]))
            {
                ++engwords;
                if (!prevEnglish) a_sentence.append(" ");
                a_sentence.append(words[i]);
                a_sentence.append(" ");
                prevEnglish = true;
            }
            else
            {
                prevEnglish = false;
                a_sentence.append(words[i]);
            }
        }
    }

    // if is not english, detection methods have to used
    LanguageDetector languageDetector;
    LanguageType m_lt = UNDEF_LANGUAGE;
    try
    {
        m_lt = languageDetector.LanguageDetect(a_sentence);
    }
    catch (const exception &)
    {
        m_lt = UNDEF_LANGUAGE;
    }

    // english ratio judgement first
    if ( (double)engwords / (double)totalwords > 0.8 )
    {
        m_language = ENGLISH;
    }
    else
    {
        switch(m_lt)
        {
        case ENGLISH:
            m_language = ENGLISH;
            break;
        case CHINESE_SIMPLIFIED:
        //case CHINESE_TRANDITIONAL:
            m_language = CHINESE;
            break;
        case JAPANESE:
            m_language = JAPANESE;
            break;
        default:
            m_language = UNDEFINED;
            break;
        }
    }

    if (m_language == JAPANESE)
    {
        // first: write japanese sentence to file
        string jpsentence("jpsentence.jp");
        ofstream fout(jpsentence.c_str());
        fout << toString() << endl;
        fout.close();
        
        // second: use mecab to split sentence to words
        string jpwords("jpwords.jp");
        string cmd("/home/xuyd/mecab_tool/bin/mecab -O wakati ");
        //string cmd("/home/mecab_tool/bin/mecab -O wakati ");
        cmd.append(jpsentence);
        cmd.append(" -o ");
        cmd.append(jpwords);
        system(cmd.c_str());

        // third: read back the result of words splitting
        ifstream fin(jpwords.c_str());
        if(fin.fail()){
            cout << "fail to re-split japanese words" << endl;
            return;
        }
        string line;
        words.clear();
        while(getline(fin, line))
        {
            string word;
            istringstream sin(line);
            while (sin >> word)
            {
                words.push_back(word);
            }
        }
        
        // finally: normalize japanese again because we split words again
        normalize();
    }
}

void MtSentence::featureExtract()
{
	initDotSet();

	/** Generate cnPunctuation & contentWordsNum feature */
	contentWordsNum = 0;
	for (int i = 0; i < words.size(); ++i) 
	{
		if (m_language == ENGLISH)
		{
            // Find a Chinese Character
			if (dotChToEn.find(words[i]) != dotChToEn.end()) 
				words[i] = dotChToEn[words[i]];
		} 
		else 
		{
            // Find a English Character
			if (dotEnToCh.find(words[i]) != dotEnToCh.end())
				words[i] = dotEnToCh[words[i]];
		}

		if (!StringOperation::isDots(words[i]))
		{
			contentWordsNum++;
		}
		else 
		{
			if (m_language == ENGLISH)
				cnPunctuation[words[i]]++;
			else
				cnPunctuation[dotChToEn[words[i]]]++;
		}
	}
	
	/** Generate cnWords & cnSpecials feature */
    for (int i = 0; i < words.size(); ++i)
    {
        const string & word = words[i];
        // If this word is special words, such as U.S. entity
        if (StringOperation::isSpecialCase(word))
            cnSpecials[word]++;
            
        if (StringOperation::isDots(word))
            continue;

	    if (m_language == JAPANESE)
		{
			const set<string> & chs = dict->JP2CH(word);
			for (set<string>::const_iterator itr = chs.begin(); itr	!= chs.end(); ++itr) 
				cnWords[(*itr)]++;
		}
        else if (m_language == ENGLISH)
        {
            const set<string> & chs = dict->EN2CH(word);
            for (set<string>::const_iterator itr = chs.begin(); itr != chs.end(); ++itr)
                cnWords[(*itr)]++;
        }
		else if (m_language == CHINESE)
		{
			cnWords[word]++;
		}
    }
		
    // Japanese digits need precise algorithm
    if (m_language == JAPANESE)
    {
        cnDigits = japConvert.getNums(words);
        
        for (int i = 0; i < words.size(); ++i)
        {
            if (StringOperation::isDigit(words[i]))
                cnDigits[words[i]]++;
        }

        cnDigits.erase("0");
        cnDigits.erase("1");
    }
    else if (m_language == CHINESE)
    {
        cnDigits = chiConvert.getNums(words);
        cnDigits.erase("0");
        cnDigits.erase("1");
    }
    else if (m_language == ENGLISH)
    {
        cnDigits = engConvert.getNums(words);
        cnDigits.erase("0");
        cnDigits.erase("1");
    }
    
}

};// end of namespace
