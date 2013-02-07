/**
 * mt_dynamic_programming.cpp
 * Created on 2010.4.26
 *      Author yuandong.xu
 */

#include "mt_dynamic_programming.h"
#include "mt_sentence.h"
#include "mt_log_file.h"
#include <vector>
#include <map>
#include <fstream>

using namespace std;

namespace mt_parallel
{

MtDynamicProgram::MtDynamicProgram()
{}

MtDynamicProgram::~MtDynamicProgram()
{}

vector<pair<MtSentence, MtSentence> >
MtDynamicProgram::doAlignment(vector<MtSentence> & others, vector<MtSentence> & chns)
{
    nx = others.size();
    ny = chns.size();
    
    minMatchDistance(others, chns);
    int num = tracePaths();

    vector<pair<MtSentence, MtSentence> > result;
    
    struct Align* ra;
    for (int i = 0; i < num; ++i)
    {
        ra = &ralign[i];
        if (ra->x1 == -1 || ra->y1 == -1)
            continue;
        if (ra->x2 == -1 && ra->y2 == -1)
            result.push_back(make_pair(others[ra->x1], chns[ra->y1]));
        else if (ra->x2 == -1)
        {
            result.push_back(make_pair(others[ra->x1], chns[ra->y1] + chns[ra->y1]));
        }
        else if (ra->y2 == -1)
        {
            result.push_back(make_pair(others[ra->x1] + others[ra->x2], chns[ra->y1]));
        }
        else
        {
            result.push_back(make_pair(others[ra->x1] + others[ra->x2], chns[ra->y1] + chns[ra->y2]));
        }
    }

    return result;
}

int MtDynamicProgram::minMatchDistance(vector<MtSentence> & others, vector<MtSentence> & chns)
{
    const int MAXINT = 10000000;

    int d1, d2, d3, d4, d5, d6;
    for (int i = 0; i <= nx; ++i)
    {
        for (int j = 0; j <= ny; ++j)
        {
            // substitution 1-1
            d1 = (i > 0 && j > 0) ? dist[i-1][j-1] + distance(others[i-1], chns[j-1], MtSentence::mZero, MtSentence::mZero) : MAXINT;
            // deletion 1-0
            d2 = (i > 0) ? dist[i-1][j] + distance(others[i-1], MtSentence::mZero, MtSentence::mZero, MtSentence::mZero) : MAXINT;
            // insertion 0-1
            d3 = (j > 0) ? dist[i][j-1] + distance(MtSentence::mZero, chns[j-1], MtSentence::mZero, MtSentence::mZero) : MAXINT;
            // contraction 2-1
            d4 = (i > 1 && j > 0) ? dist[i-2][j-1] + distance(others[i-2], chns[j-1], others[i-1], MtSentence::mZero) : MAXINT;
            // expansion 1-2
            d5 = (i > 0 && j > 1) ? dist[i-1][j-2] + distance(others[i-1], chns[j-2], MtSentence::mZero, chns[j-1]) : MAXINT;
            // melding 2-2
            d6 = (i > 1 && j > 1) ? dist[i-2][i-2] + distance(others[i-2], chns[j-2], others[i-1], chns[j-1]) : MAXINT;

            int dmax = d1;
            if (dmax < d2) dmax = d2;
            if (dmax < d3) dmax = d3;
            if (dmax < d4) dmax = d4;
            if (dmax < d5) dmax = d5;
            if (dmax < d6) dmax = d6;

            if (dmax == MAXINT)
            {
                dist[i][j] = 0;
            }
            else 
            {
                dist[i][j] = dmax;
                
                if (dmax == d1)
                {
                    path_x[i][j] = i - 1;
                    path_y[i][j] = j - 1;
                }
                else if (dmax == d2)
                {
                    path_x[i][j] = i - 1;
                    path_y[i][j] = j;
                }
                else if (dmax == d3)
                {
                    path_x[i][j] = i;
                    path_y[i][j] = j - 1;
                }
                else if (dmax == d4)
                {
                    path_x[i][j] = i - 2;
                    path_y[i][j] = j - 1;
                }
                else if (dmax == d5)
                {
                    path_x[i][j] = i - 1;
                    path_y[i][j] = j - 2;
                }
                else if (dmax == d6)
                {
                    path_x[i][j] = i - 2;
                    path_y[i][j] = j - 2;
                }
            }

            
        } // end of y
    } // end of x
    
    return 0;
}

int MtDynamicProgram::tracePaths()
{
    MtLogFile* flog = MtLogFile::Instance();
    
    int n = 0; // number of sentence pair
    int i, j, oi, oj, di, dj;
    for (i = nx, j = ny; i > 0 && j > 0; i = oi, j = oj)
    {
        oi = path_x[i][j];
        oj = path_y[i][j];
        di = i - oi;
        dj = j - oj;
        
        if (di == 1 && dj == 1)
        {
            /* substitution */
            ralign[n].x1 = i-1;
            ralign[n].y1 = j-1;
            ralign[n].x2 = -1;
            ralign[n].y2 = -1;
            ralign[n].d = dist[i][j] - dist[i-1][j-1];
        }
        else if (di == 1 && dj == 0)
        {
            /* deletion */
            ralign[n].x1 = i-1;
            ralign[n].y1 = -1;
            ralign[n].x2 = -1;
            ralign[n].y2 = -1;
            ralign[n].d = dist[i][j] - dist[i-1][j];
        }
        else if (di == 0 && dj == 1)
        {
            /* insertion */
            ralign[n].x1 = -1;
            ralign[n].y1 = j-1;
            ralign[n].x2 = -1;
            ralign[n].y2 = -1;
            ralign[n].d = dist[i][j] - dist[i][j-1];
        }
        else if (di == 2 && dj == 1)
        {
            /* contraction */
            ralign[n].x1 = i-2;
            ralign[n].y1 = j-1;
            ralign[n].x2 = i-1;
            ralign[n].y2 = -1;
            ralign[n].d = dist[i][j] - dist[i-2][j-1];
        }
        else if (di == 1 && dj == 2)
        {
            /* expansion */
            ralign[n].x1 = i-1;
            ralign[n].y1 = j-2;
            ralign[n].x2 = -1;
            ralign[n].y2 = j-1;
            ralign[n].d = dist[i][j] - dist[i-1][j-2];
        }
        else //di == 2 && dj == 2
        {
            /* melding */
            ralign[n].x1 = i-2;
            ralign[n].y1 = j-2;
            ralign[n].x2 = i-1;
            ralign[n].y2 = j-1;
            ralign[n].d = dist[i][j] - dist[i-2][j-2];
        }

        *flog << i << " " << j << " " <<ralign[n].d << " " << di << " " << dj << " (" << ralign[n].x1 << " " << ralign[n].y1 << ")\n";
        ++n; // number of sentence pair increases
        
    }
    
    return n;
}

int MtDynamicProgram::distance(MtSentence & x1, MtSentence & y1, MtSentence & x2, MtSentence & y2)
{
    const double penalty01 = 0.0;
    const double penalty21 = 0.5;
    const double penalty22 = 0.5;
    
    double cost = 0.0;
    
    if (MtSentence::IsZero(x2) && MtSentence::IsZero(y2))
    {
        if (MtSentence::IsZero(x1))
        {
            // insertion
            cost = match(x1, y1) + penalty01;
        }
        else if (MtSentence::IsZero(y1))
        {
            // deletion
            cost = match(x1, y1) + penalty01;
        }
        else
        {
            // substitution
            cost = match(x1, y1);
        }
    }
    else if (MtSentence::IsZero(x2))
    {
        // expansion
        MtSentence ty = y1 + y2;
        cost = match(x1, ty) + penalty21;
    }
    else if (MtSentence::IsZero(y2))
    {
        // contraction
        MtSentence tx = x1 + x2;
        cost = match(tx, y1) + penalty21;
    }
    else
    {
        // merge
        MtSentence tx = x1 + x2;
        MtSentence ty = y1 + y2;
        cost = match(tx, ty) + penalty22;
    }
    
    const double scale = 1000;
    return static_cast<int>(max(1.0, scale * cost));
}

double MtDynamicProgram::match(MtSentence & a, MtSentence & b)
{
    double sim = a.calcuSimilarity(b);
    return sim;
}
    
};
