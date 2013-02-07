/*
 * mt_bigraph_match.cpp
 *
 *  Created on: 2010-4-6
 *      Author: yuandong.xu
 */

#include "mt_bigraph_match.h"
#include <cstring>
#include <vector>
#include <iostream>

using namespace std;

namespace mt_parallel{

namespace Graph{

BiGraphMatch::BiGraphMatch()
{
    memset(weight, 0, sizeof(weight));
}

BiGraphMatch::~BiGraphMatch()
{
}

void BiGraphMatch::clear()
{
    memset(weight, 0, sizeof(weight));
}

int BiGraphMatch::make(const int (*matrix)[MAXN], const int nsize)
{
	// copy edge & vertex
    memset(weight, 0, sizeof(weight));
    
    for (int i = 0; i < nsize; ++i)
    {
        for (int j = 0; j < nsize; ++j)
        {
            weight[i][j] = matrix[i][j];
        }
    }
    
    nCount = nsize;
    
	return 0;
}

void BiGraphMatch::getMap(vector<int> & xdst, vector<int> & xysim)
{
    // copy results
    for (int i = 0; i < nCount; ++i)
    {
        int j = m_xMatch[i];
        xdst.push_back(j);
        xysim.push_back(weight[j][i]);
    }
}

int BiGraphMatch::KmMatch()
{
	// initialize
	for(int i = 0; i < nCount; ++i)
	{
		ly[i] = 0;
		lx[i] = -INF;
		for(int j = 0; j < nCount; ++j)
		{
			if(lx[i] < weight[i][j])
				lx[i] = weight[i][j];
		}
	}
	memset(m_xMatch, -1, sizeof(m_xMatch));

	// proceed matching
	for(int m = 0; m < nCount; ++m)
	{
		for(int i = 0; i < nCount; ++i)
			slack[i] = INF;

		while(true)
		{
            // clear visiting vertex labels before finding paths        
		    memset(xckd, false, sizeof(xckd));
    		memset(yckd, false, sizeof(yckd));
            
            if(DFS(m))  break;
            
            // update equal-set and slack
			int dx = INF;
			for(int i = 0; i < nCount; ++i)
			{
				if(!yckd[i] && slack[i]<dx )
					dx = slack[i];
			}

			for(int i = 0; i < nCount; ++i)
			{
				if(xckd[i])
					lx[i] -= dx;
				if(yckd[i])
					ly[i] += dx;
				slack[i] -= dx;
			}

		}// end of while

	}// end of for

	// return max weight
	int maxWeight = 0;
	for(int i = 0; i < nCount; ++i)
		maxWeight += weight[m_xMatch[i]][i];

	return maxWeight;
}

bool BiGraphMatch::DFS(int t)
{
	xckd[t] = true;
	for(int i = 0; i < nCount; ++i)//y
	{
		if(!yckd[i])
        {
		    int wt = lx[t] + ly[i] - weight[t][i];
            if(wt == 0)
            {
			    yckd[i] = true;
    			if(m_xMatch[i]==-1 || DFS(m_xMatch[i]))
	    		{
		    		m_xMatch[i] = t;
			    	return true;
			    }
		    }
    		else if(slack[i]>wt)
	    	{
		    	slack[i] = wt;
		    }
        }
	}
	return false;
}


}; // end of namespace Graph

}; // end of namespace mt_parallel

