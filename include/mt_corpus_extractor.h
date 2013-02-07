/*
 * mt_corpus_extractor.h
 *
 *  Created on: 2010-4-9
 *      Author: yuandong.xu
 */

#ifndef MT_CORPUS_EXTRACTOR_H_
#define MT_CORPUS_EXTRACTOR_H_

#include "mt_html_filter.h"
#include "mt_sentence_align.h"
#include "mt_dynamic_programming.h"

#include <string>
using std::string;

namespace mt_parallel
{

class MtCorpusExtractor
{
public:
	MtCorpusExtractor();
	~MtCorpusExtractor();

	/** load an html file from www, given a url string */
	int loadHtmlFromUrl(const char* url);

	/** load an html file from disk, given a file name */
	int loadHtmlFromDisk(const char* fname);

	/** load an html file from string, given a string */
	int loadHtmlFromString(const string & str, const string & url = "http://");

	/**parse html structure, extract parallel texts
	 * return 0 when fails.
	 */
	int doExtract();

private:
	MtSingleHtmlFilter* m_shfilter;
	MtSentenceAlign* m_salign;
    // you can choose DP algorithm to do alignment
//  MtDynamicProgram* m_salign;

    std::string m_urlstr;
	std::string m_htmlstr;

private:

};


}; // end of namespace


#endif /* MT_CORPUS_EXTRACTOR_H_ */
