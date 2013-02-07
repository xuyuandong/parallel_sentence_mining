/*
 * mt_sentence_align.h
 *
 *  Created on: 2010-4-7
 *      Author: yuandong.xu
 */

#ifndef MT_SENTENCE_ALIGN_H_
#define MT_SENTENCE_ALIGN_H_

#include "mt_sentence.h"
#include "mt_bigraph_match.h"

#include <vector>
#include <map>
using std::vector;
using std::pair;

namespace mt_parallel
{

using Graph::BiGraphMatch;
     
class MtSentenceAlign
{
public:
	MtSentenceAlign();
	~MtSentenceAlign();

	vector<pair< double, pair<MtSentence, MtSentence> > >
		doAlignment(vector<MtSentence> & others, vector<MtSentence> & chns);
    
public:
    static const int MAXDIMENSION = BiGraphMatch::MAXN;
    static const int scale = 1000;
    
private:
    void buildWeightMatrix(vector<MtSentence> & others, vector<MtSentence> & chns);

private:
    // similarity matrix -> weight matrix in bigraph
    int smatrix[MAXDIMENSION][MAXDIMENSION];
    // using bigraph to match sentences
    BiGraphMatch* pGraph;
    
};

};

#endif /* MT_SENTENCE_ALIGN_H_ */
