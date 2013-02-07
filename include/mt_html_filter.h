/*
 * mt_single_html_filter.h
 *
 *  Created on: 2010-4-7
 *      Author: yuandong.xu
 */

#ifndef MT_SINGLE_HTML_FILTER_H_
#define MT_SINGLE_HTML_FILTER_H_

#include "mt_sentence.h"
#include "htmlparser/feature.h"
#include <set>
#include <string>
#include <vector>
using namespace std;
using namespace galaxy::utility::mthtmlparser;

namespace mt_parallel
{

class MtSingleHtmlFilter
{
public:
	MtSingleHtmlFilter();
	~MtSingleHtmlFilter();

    // initialize the filter
    void clear();

	// parsing html, extract DOM features
	void doHtmlParsing(string & htmlstr, const string & source);

    // filtering out useful common paths and filter out useful text later
    int doTextSelect();
    
	// split texts into sentences
	int splitToSentence();

	// Mapping sentences into groups according to it's language
	int doTextMapping(vector<MtSentence> & others, vector<MtSentence> & chns);

    void setTargetLanguage(bool jp = true);
    
private:
    bool jp;
	Feature m_htmlfeature;
    set<vector<int> > m_compaths;
	vector<MtSentence> m_content;
	
	static const int MaxHeapSize = 40;
    static const int MinHeapSize = 25;
};


};

#endif /* MT_SINGLE_HTML_FILTER_H_ */
