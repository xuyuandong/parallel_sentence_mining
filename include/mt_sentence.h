/*
 * mt_sentence.h
 *
 *  Created on: 2010-4-7
 *      Author: yuandong.xu
 */

#ifndef MT_SENTENCE_H_
#define MT_SENTENCE_H_

#include "mt_dictionary.h"
#include "mt_similarity_model.h"
#include <string>
#include <vector>

using namespace std;

namespace mt_parallel
{

enum LANGUAGE
{
	UNDEFINED = -1,
	ENGLISH = 0,
	CHINESE = 1,
	JAPANESE = 3
};

inline static string getLanguageName(LANGUAGE lang)
{
	if (lang == UNDEFINED)
	    return "UNDEF";
	if (lang == ENGLISH)
	    return "ENGLISH";
	if (lang == CHINESE)
	    return "CHINESE";
	if (lang == JAPANESE)
	    return "JAPANESE";
}


class MtSentence
{
public:
    static MtSentence mZero;
    static bool IsZero(MtSentence & mts) 
    {
        return &mts == &mZero; 
    }
    
public:
	LANGUAGE m_language;

    /** index of this sentence in its segment page */
    int sid;

    /** Position in the same content session of the same language */
	double m_position;

    /** The number of meaningful words*/
    int contentWordsNum;

	/** The word list of this sentense */
	vector<string> words;
    /** The dom path list of each word */
    vector<int> dompaths;

    /** Dictionary mapping words list */
    map<string, int> cnWords;

    /** Dictionary mapping digital words list */
    map<string, int> cnDigits;

    /** Dictionary mapping special words list */
    map<string, int> cnSpecials;

    /** Punctuation (biaodian) */
    map<string, int> cnPunctuation;

    /** MtDictionary for mapping words */
    static MtDictionary* dict;

    /** Maximun Entropy Model for Computing Similarity given features */
    static MtSimilarityModel* model;

public:
	MtSentence();
    MtSentence(const vector<string> & wordlist, const vector<int> & dompathlist);
	~MtSentence();

    // how many words in the sentence
	inline size_t length() const {
        return words.size();
    }

	// return a string of the sentence's content
	inline string toString(bool sep = false) const 
    {
        string a_sentence;
        string word;
        for (int i = 0; i < words.size(); ++i)
        {
            // Recover some special charactor for japanese
            word = words[i];
            if (m_language == JAPANESE)
            {

                if ("“" == words[i])    word = "「";
                if ("”" == words[i])    word = "」";
                if ("《" == words[i])   word = "『";
                if ("》" == words[i])   word = "』";
            }
            if (sep || m_language == ENGLISH)
            {
                word += " ";
            }
            a_sentence.append(word);
        }
        return a_sentence;
    }

	// update self by linking another sentence
	MtSentence& operator+=(const MtSentence& rhs);

	// generate a new sentence by linking two sentences
	friend MtSentence operator+(const MtSentence & lhs, const MtSentence & rhs);

	// calculate similarity score with another sentence
	double calcuSimilarity(const MtSentence & other, bool exportFeature = false);

    // export features to file for labeling and checking
    void exportFeatureToFile(const double weight, const MtSentence & other, const vector<pair<string, float> > & feature);

private:
    // filter Un-standard character to standard representation
    void normalize();
    
    // detect language
    void detectLanguage();

    // feature extraction (content, digits ...) for compare similarity
    void featureExtract();

};


}; // end of namespace

#endif /* MT_SENTENCE_H_ */
