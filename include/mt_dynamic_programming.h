/** 
 * mt_dynamic_programming.h
 * Created on 2010.4.26
 *      Author yuandong.xu
 */
 
#ifndef MT_DYNAMIC_PROGRAMMING_H_
#define MT_DYNAMIC_PROGRAMMING_H_

#include "mt_sentence.h"
#include <vector>
#include <map>

using namespace std;

namespace mt_parallel
{

class MtDynamicProgram
{
public:
    static const int MAXDIMENSION = 1000;
    
    MtDynamicProgram();
    ~MtDynamicProgram();

    vector<pair<MtSentence, MtSentence> > 
        doAlignment(vector<MtSentence> & others, vector<MtSentence> & chns);

private:
    int minMatchDistance(vector<MtSentence> & others, vector<MtSentence> & chns);
    int tracePaths();
    int distance(MtSentence & x1, MtSentence & y1, MtSentence & x2, MtSentence & y2);
    double match(MtSentence & a, MtSentence & b);

private:
    struct Align
    {
        int x1;
        int x2;
        int y1;
        int y2;
        int d;
    };
    
    int dist[MAXDIMENSION][MAXDIMENSION];
    int path_x[MAXDIMENSION][MAXDIMENSION];
    int path_y[MAXDIMENSION][MAXDIMENSION];
    Align ralign[2 * MAXDIMENSION];
    int nx;
    int ny;
    
};

};

#endif
