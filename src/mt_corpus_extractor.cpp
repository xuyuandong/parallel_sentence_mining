/*
 * mt_corpus_extractor.cpp
 *
 *  Created on: 2010-4-9
 *      Author: yuandong.xu
 */

#include "mt_corpus_extractor.h"
#include "mt_log_file.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

namespace mt_parallel {

MtCorpusExtractor::MtCorpusExtractor() 
{
    m_shfilter = new MtSingleHtmlFilter();
    m_salign = new MtSentenceAlign();
    // you can choose DP algorithm to do alignment
    //m_salign = new MtDynamicProgram();
}

MtCorpusExtractor::~MtCorpusExtractor() 
{
    if(m_shfilter!=NULL)
        delete m_shfilter;
    if(m_salign!=NULL)
        delete m_salign;
}

int MtCorpusExtractor::loadHtmlFromString(const string & str, const string & url)
{
    m_urlstr = url;
	m_htmlstr = str;
  
	return 1;
}

int MtCorpusExtractor::loadHtmlFromUrl(const char* url) 
{
	string command = "wget -q -O mt_corpus.html.tmp ";
	command.append(url);
	cout << command << endl;
	system(command.c_str());

	int rc = loadHtmlFromDisk("mt_corpus.html.tmp");
	if(!rc){
		return 0;
	}
    m_urlstr = string(url);

	return 1;
}

int MtCorpusExtractor::loadHtmlFromDisk(const char* fname) 
{
	ifstream fin;
	fin.open(fname);
	if (fin.fail()) {
		return 0;
	}

	m_htmlstr.clear();
	string stmp;
	while (getline(fin, stmp)) 
    {
		m_htmlstr.append(stmp + "\t");
	}
    m_urlstr = string(fname);

	fin.close();
	return 1;
}

int MtCorpusExtractor::doExtract() 
{
    MtLogFile* flog  = MtLogFile::Instance();
    
    //m_shfilter->setTargetLanguage(false);
    m_shfilter->clear();
    
    /*parse html, split to sentences, cluster to parallel paragraphs*/
    try {
	    /** parse html structure, filter out noise tags */
		//*flog << "proceeding html parsing\n";// << endl;
        m_shfilter->doHtmlParsing(m_htmlstr, m_urlstr);
        //*flog << "finish html parsing\n";// << endl;

        /** filter out useful content text */
        //*flog << "proceeding text filtering\n";// << endl;
        m_shfilter->doTextSelect();
        //*flog << "finish text filtering\n";// << endl;
        
		/** split the content string into sentences */
		//*flog << "proceeding sentences spliting\n";// << endl;
        m_shfilter->splitToSentence();
        //*flog << "finish sentences spliting\n";// << endl;
        
	} catch (const exception &) {
		*flog << "fail to extract text from html\n";// << endl;
		return -1;
	}

	/** add two kinds of language sentences into each vector */
	vector<MtSentence> others;
	vector<MtSentence> chns;
    m_shfilter->doTextMapping(others, chns);

    /** print out two groups of language sentence */
    /*
    *flog << "Japanese: " << others.size() << "\n";
    for (int i = 0; i < others.size(); ++i)
        *flog << i << " " << others[i].toString() << "\n";
    *flog << "Chinese: " << chns.size() << "\n";
    for (int i = 0; i < chns.size(); ++i)
        *flog << i << " " << chns[i].toString() << "\n";
    */
    
    vector<pair<double, pair<MtSentence, MtSentence> > > result;
    try
    {
        //*flog << "proceeding sentence alignment\n";// << endl;
        result = m_salign->doAlignment(others, chns);
        //*flog << "finish sentence alignment\n";// << endl;
    } catch (const exception & e) {
        *flog << "fail to align sentences\n";// << endl;
        return -1;
    }
    
    /** print out sentence pairs : result of alignment */
    if (result.size() > 0)
    {
        ofstream fout("result_0.2.txt", ios::app);
        if (fout.fail())
        {
            *flog << "fail to open result_0.2.txt\n";
            return -1;
        }
        fout << m_urlstr << endl;
        for (int i = 0; i < result.size(); ++i)
        {
            double weight = result[i].first;
            MtSentence & a = result[i].second.first;
            MtSentence & b = result[i].second.second;
            fout << weight << "[<split>]";
            fout << a.toString() << "[<split>]";
            fout << b.toString() << "[<split>]";
            fout << endl;
        }
        fout.close();
    }
    
	return result.size();
}

}
; // end of namespace
