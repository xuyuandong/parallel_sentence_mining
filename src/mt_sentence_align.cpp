/*
 * mt_sentence_align.cpp
 *
 *  Created on: Apr 11, 2010
 *      Author: yuandong.xu
 */

#include "mt_sentence_align.h"
#include "mt_bigraph_match.h"
#include "mt_log_file.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

namespace mt_parallel
{

using Graph::BiGraphMatch;

const int MAXDIMENSION = BiGraphMatch::MAXN;

// similarity matrix -> weight matrix in bigraph
int smatrix[MAXDIMENSION][MAXDIMENSION];

MtSentenceAlign::MtSentenceAlign()
{
    pGraph = NULL;
    pGraph = new BiGraphMatch();
}

MtSentenceAlign::~MtSentenceAlign()
{
    if (pGraph != NULL)
        delete pGraph;
}

void MtSentenceAlign::buildWeightMatrix(vector<MtSentence>& others, vector<MtSentence>& chns)
{
    /* single mask */
    /*
    for (int i = 0; i < others.size(); ++i)
    {
        for (int j = 0; j < chns.size(); ++j)
        {
            double sim = others[i].calcuSimilarity(chns[j]);
            smatrix[i][j] = static_cast<int>(max(1.0, scale * sim));
        }
    }
    */
    /* triple mask */
    unsigned int nx = others.size();
    unsigned int ny = chns.size();
    for (int i = 0; i < nx; ++i)
    {
        for (int j = 0; j < ny; ++j)
        {
            double sim;
            if (nx > 1 && ny > 1)
            {
                if (i == 0 || j == 0) 
                {
                    double sim1 = others[i].calcuSimilarity(chns[j]);
                    double sim2 = others[i+1].calcuSimilarity(chns[j+1]);
                    sim = (1.0 * sim1 + 0.36 * sim2) / 1.36;
                }
                else if (i == nx -1 || j == ny - 1)
                {
                    double sim0 = others[i-1].calcuSimilarity(chns[j-1]);
                    double sim1 = others[i].calcuSimilarity(chns[j]);
                    sim = (0.36 * sim0 + 1.0 * sim1) / 1.36;
                }
                else
                {
                    double sim0 = others[i-1].calcuSimilarity(chns[j-1]);
                    double sim1 = others[i].calcuSimilarity(chns[j]);
                    double sim2 = others[i+1].calcuSimilarity(chns[j+1]);
                    sim = (0.36 * sim0 + 1.0 * sim1 + 0.36 * sim2) / 1.72;
                }
            }
            else
            {
                sim = others[i].calcuSimilarity(chns[j]);
            }
            smatrix[i][j] = static_cast<int>(max(1.0, scale * sim));
        }
    }
    
}

vector<pair<double, pair<MtSentence, MtSentence> > >
MtSentenceAlign::doAlignment(vector<MtSentence>& others, vector<MtSentence>& chns)
{
    vector<pair<double, pair<MtSentence, MtSentence> > > MatchedPairs;
    
    if (others.size() == 0 || chns.size() == 0)
        return MatchedPairs;
    if (others.size() > MAXDIMENSION || chns.size() > MAXDIMENSION)
        return MatchedPairs;
    
    buildWeightMatrix(others, chns);

    // match sentences using bigraph matching
    // how to merge adjacent sentences ?? right now, no way
    int nsize = max(others.size(), chns.size());
   
    pGraph->clear();
    pGraph->make(smatrix, nsize);
    pGraph->KmMatch();
    
    // retrieve results: sentence pairs and weights
    vector<int> dsts;
    vector<int> weights;
    pGraph->getMap(dsts, weights);
    
    // collect effective results
    for(int i = 0; i < nsize; ++i)
    {
        int j = dsts[i];
        if(j < others.size() && i < chns.size())
        {
            if (weights[i] > 0.15 * scale)
            {
                MatchedPairs.push_back(make_pair((double)weights[i]/scale, make_pair(others[j], chns[i])));
            }
        }
    }

    return MatchedPairs;
}

}; // end of namespace
