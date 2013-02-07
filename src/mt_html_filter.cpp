/*
 * mt_html_filter.cpp
 *
 *  Created on: Apr 11, 2010
 *      Author: yuandong.xu
 */
#include "mt_html_filter.h"
#include "mt_sentence.h"
#include "mt_code_converter.h"
#include "mt_special_charset.h"
#include "mt_log_file.h"
#include "htmlparser/html_parser.h"
#include "htmlparser/extractor/text_extractor.h"
#include "htmlparser/extractor/link_extractor.h"
#include "htmlparser/extractor/image_extractor.h"
#include "htmlparser/extractor/url_normalizer.h"
#include "htmlparser/extractor/br_extractor.h"
#include "htmlparser/extractor/page_segmenter.h"
#include "htmlparser/extractor/meta_extractor.h"
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>

using namespace std;
using namespace galaxy::utility::mthtmlparser;

namespace mt_parallel {

/** for pre-sentence spliting ... */
static set<string> sepCharSet;
// initialize seperate charset
static void initSepCharSet()
{
    static bool flag = true;
    if(flag)
    {
        // English seperate charset
        sepCharSet.insert("\t");
        sepCharSet.insert(".");
        sepCharSet.insert("?");
        sepCharSet.insert("!");
//        sepCharSet.insert(":");
        // Chinese and Japanese seperate charset
        sepCharSet.insert("。");
        sepCharSet.insert("？");
        sepCharSet.insert("！");
        // set flag
        flag = false;
    }
}

// generate html structure path tags
static void tagPathGen(Node* node, int order)
{
	if (node == NULL)
		return;
	if (node->GetParent() != NULL)
		node->tagPath = (node->GetParent()->tagPath);
	(node->tagPath).push_back(order);

	int count = 0;
	if (node->GetChildNum() > 0)
	{
		Node* child = node->GetLeftChild();
		while (child != NULL)
		{
			tagPathGen(child, count++);
			child = child->GetSibling();
		}
	}
}

double calPathDistance(const vector<int> & a, const vector<int> & b)
{   
    int aLen = a.size();    
    int bLen = b.size();    
    int n = min(aLen, bLen);    

    int count = 0;  
    for (int i = 0; i < n; ++i) 
    {       
        if(a[i] == b[i])            
            count++;        
        else            
            break;  
    }   
    return (double)(count*count)/(double)max(1, aLen*bLen);
}

MtSingleHtmlFilter::MtSingleHtmlFilter()
{
    jp = true;
}

MtSingleHtmlFilter::~MtSingleHtmlFilter()
{
}

void MtSingleHtmlFilter::setTargetLanguage(bool jp)
{
    this->jp = jp;
}

void MtSingleHtmlFilter::clear()
{
    m_htmlfeature.Clear();
    m_content.clear();
    m_compaths.clear();
}

void MtSingleHtmlFilter::doHtmlParsing(string & htmlstr, const string& source)
{
    MtCodeConverter ec;
    htmlstr = ec.convertToUTF8(htmlstr);
    
    ExtractorMediator extractors;
    TextExtractor te;
    BrExtractor be;
    PageSegmenter ps;
    extractors.SetNodeExtractor("text", &te);
    extractors.SetNodeExtractor("br", &be);
    //extractors.AddPageExtractor(&ps);
    
    HtmlParser parser(&extractors);
    parser.Parse(htmlstr);
    Node * root = parser.GetRoot();
    tagPathGen(root, 0);

    m_htmlfeature.Clear();
    m_htmlfeature.pageUrl = source;
    extractors.ExtractHtmlTree(root, m_htmlfeature);

    ps.ExtractPageFeature(m_htmlfeature);
    parser.Reset();
    root = NULL;
}

int MtSingleHtmlFilter::doTextSelect()
{
    // a vector store length of each page segment text
    vector<int> pagesize;
    for (int i=0; i < m_htmlfeature.domPageContent.size(); ++i)
        pagesize.push_back(m_htmlfeature.domPageContent[i].size());
    if (pagesize.size() == 0)
        return 0;
    
    // calculate heap size
    int nPages = pagesize.size()/5;
    if (nPages > MaxHeapSize )         
        nPages = MaxHeapSize;     
    if (nPages < MinHeapSize)          
        nPages = MinHeapSize;     
    if (nPages > pagesize.size())           
        nPages = pagesize.size();      

    // for get the first K longest page segments
    priority_queue<int, vector<int>, greater<int> > pqheap;

    for (int i = 0; i < nPages; ++i)
    {
        pqheap.push(pagesize[i]);
    }
    for (int i = nPages; i < pagesize.size(); ++i)
    {
        if(pagesize[i] > pqheap.top())
        {
            pqheap.pop();
            pqheap.push(pagesize[i]);
        }
    }
    
    // average page size of the first K longest page segments
    int avrpagesize = 0;
    for (int i = 0; i < nPages; ++i)
    {
        avrpagesize += pqheap.top();
        pqheap.pop();
    }
    avrpagesize /= max(1, nPages);
    
    // record & label feature page paths
    vector<vector<int> > labelpaths;
    for (int i = 0; i < pagesize.size(); ++i)
    {
        if (pagesize[i] >= avrpagesize )
        {
            labelpaths.push_back(m_htmlfeature.domTreePath[i]);
        }
        if (labelpaths.size() > MaxHeapSize - 1)
            break;
    }

    //calculate similarity matrix of label paths
    const int MaxNodeNum = MaxHeapSize;
    double edge[MaxNodeNum][MaxNodeNum];
    memset(edge, 0, sizeof(edge));
    int nsize = labelpaths.size();
    for (int i = 0; i < nsize; ++i)
    {
        for (int j = 0; j < nsize; ++j)
        {
            edge[i][j] = calPathDistance(labelpaths[i], labelpaths[j]);
        }
    }

    // label paths simialrity & frequency container
    set<double> distance; 
    set<double>::iterator iter; 
    for (int i = 0; i < nsize; ++i)
    {
        for (int j = 0; j < nsize; ++j)
        {
            if (edge[i][j] < 0.24 || edge[i][j] > 0.9)
                continue;
            distance.insert(edge[i][j]);
        }
    }
    
    // cluster group label
    bool clusterlabel[MaxNodeNum];
    memset(clusterlabel, true, sizeof(clusterlabel));

    vector<int> candidatePathId;
    vector<int> tmpcompath;

    // calculate common paths
    m_compaths.clear();
    set<double>::reverse_iterator riter;
    for (riter = distance.rbegin(); riter != distance.rend(); ++riter)
    {
        double similarity = (*riter);
        candidatePathId.clear();
        for (int i = 0; i < nsize; ++i)
        {
            if (clusterlabel[i])
            {
                for (int j = 0; j < nsize; ++j)
                {
                    if (edge[i][j] == similarity)
                    {
                        if (clusterlabel[i])
                        {
                            clusterlabel[i] = false;
                            candidatePathId.push_back(i);
                        }
                        if (clusterlabel[j])
                        {
                            clusterlabel[j] = false;
                            candidatePathId.push_back(j);
                        }
                    }
                } // end for j
            } // end if
        } // end for i

        // special case: if only one is found
        if (1 == candidatePathId.size())
        {
            int a = candidatePathId[0];
            m_compaths.insert(labelpaths[a]);
            continue;
        }

        // calculate common paths in the local cluster of candidate paths
        for (int i = 0; i < candidatePathId.size(); ++i)
        {
            for (int j = i+1; j < candidatePathId.size(); ++j)
            {
                int a = candidatePathId[i];
                int b = candidatePathId[j];
                if (edge[a][b] == similarity)
                {
                    // calculate common path between a & b
                    tmpcompath.clear();
                    int n = min(labelpaths[a].size(), labelpaths[b].size());
                    for (int k = 0; k < n; ++k)
                    {
                        if(labelpaths[a][k] != labelpaths[b][k])
                            break;
                        tmpcompath.push_back(labelpaths[a][k]);
                    }
                    if (m_compaths.find(tmpcompath) == m_compaths.end())
                    {
                        m_compaths.insert(tmpcompath);
                    }
                }
            } // end for j
        } // end for i
        
    } // end for iteration
    
    return m_compaths.size();
}


int MtSingleHtmlFilter::splitToSentence()
{
	//Feature.domTreePath : vector<vector<int>>
    //Feature.domPageContent : vector<string>
    //Feature.domSegmentedContent : vector<vector<string>>
    
    if (m_compaths.size() == 0)
        return 0;
    
    // filter out content text
    vector<vector<int> > ctextpath;
    vector<vector<string> > ctextword;
    
    for (int i = 0; i < m_htmlfeature.domTreePath.size(); ++i)
    {
        bool useful = false;
        set<vector<int> >::const_iterator iter;
        for(iter = m_compaths.begin(); iter != m_compaths.end(); ++iter)
        {
            const vector<int> & pcompath = (*iter);
            if(m_htmlfeature.domTreePath[i].size() >= pcompath.size())
            {
                int count = 0;
                for (int k = 0; k < pcompath.size(); ++k)
                {
                    if(pcompath[k] != m_htmlfeature.domTreePath[i][k]) 
                        break;
                    count++;
                }
                if (count == pcompath.size())
                {
                    useful = true;
                    break;
                }
            }
        } // end for each comparsion
        
        if (useful)
        {
            if (m_htmlfeature.domSegmentedContent[i].size() > 2)
            {
                ctextword.push_back(m_htmlfeature.domSegmentedContent[i]);
                ctextpath.push_back(m_htmlfeature.domTreePath[i]);
            }
        }
    } // end for each dom tree path
    
    //-------------------------------------------------------------------//

    // initialize seperate charset
    initSepCharSet();
    
    // start to split sentences
    m_content.clear();

    vector<string> sentence;
    int sid = 0;
    for (int i = 0; i < ctextword.size(); ++i)
    {
        sid = 0;
        for (int j = 0; j < ctextword[i].size(); ++j)
        {
            bool sepflag = false;
            if (sepCharSet.find(ctextword[i][j]) != sepCharSet.end())
            {
                sepflag = true;
            }
   
            //special case judgement 
            if (ctextword[i][j] == ".")
            {
                //case: "www.hello-inc.com" or "e.g. Hello"
                if (j > 0 && SpecialCaseOfSplit::judge(ctextword[i][j-1]))
                    sepflag = false;
                //case: "wait..."
                if (j < ctextword[i].size() - 1 && ctextword[i][j+1] == ".")
                    sepflag = false;
            }

            if (sepflag) 
            {
                if (sentence.size()>0)
                {
                    sentence.push_back(ctextword[i][j]);

                    //quote case
                    if (j < ctextword[i].size() - 1)
                    {
                        const string & quote = ctextword[i][j+1];
                        if (quote == "\"" || quote == "”" || quote == "」")
                        {
                            sentence.push_back(ctextword[i][j+1]);
                            ++j;
                        }
                    }
                    
                    //domTreePath info: ctextpath[i]
                    MtSentence mtsen(sentence, ctextpath[i]);
                    mtsen.sid = sid;
                    m_content.push_back(mtsen);
                    sid = sid + 1;
                } 
                sentence.clear();    
            } 
            else 
            {
                sentence.push_back(ctextword[i][j]);    
            }
        }
        if (sentence.size()>0)
        {
            MtSentence mtsen(sentence, ctextpath[i]);
            mtsen.sid = sid;
            m_content.push_back(mtsen);
            sentence.clear();
        }
    }
   
    
	return m_content.size();
}


int MtSingleHtmlFilter::doTextMapping(vector<MtSentence>& others,
		vector<MtSentence>& chns)
{
    const LANGUAGE TARGET = jp ? JAPANESE : ENGLISH;
    
	for (int i = 0; i < m_content.size(); ++i)
	{
        // filter special key phrase
        if (SpecialCaseOfSplit::hasKeyPhrase(m_content[i].toString()))
            continue;

        // group by language
		if (m_content[i].m_language == CHINESE)
        {
			if (m_content[i].length() > 1)
				chns.push_back(m_content[i]);
		}
        else if (m_content[i].m_language == TARGET)
        {
			if (m_content[i].length() > 1)
				others.push_back(m_content[i]);
        }
	}

    // record relative position in the same language region
    int nsize;
    nsize = others.size();
    for (int i = 0; i < nsize; ++i)
    {
        others[i].m_position = i;
    }
    nsize = chns.size();
    for (int i = 0; i < nsize; ++i)
    {
        chns[i].m_position = i;
    }

	return 1;
}



}; // end of namespace
