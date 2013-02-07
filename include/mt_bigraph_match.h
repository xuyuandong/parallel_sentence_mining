/*
 * mt_bigraph_match.h
 *
 *  Created on: 2010-4-6
 *      Author: yuandong.xu
 */

#ifndef MT_BIGRAPH_MATCH_H_
#define MT_BIGRAPH_MATCH_H_

#include <vector>
using std::vector;

namespace mt_parallel
{

namespace Graph{
    
class BiGraphMatch{
public:
	static const int MAXN = 1000;
	static const int INF = 1<<28;

public:
    BiGraphMatch();
    ~BiGraphMatch();

	void clear();
    int make(const int (*matrix)[MAXN], const int nsize);
	int KmMatch();
    void getMap(vector<int> & xdst, vector<int> & xysim);

private:
	bool DFS(int);
    
private:
	bool xckd[MAXN];
	bool yckd[MAXN];
	int m_xMatch[MAXN]; //y[i] matchs m_xMatch[i]
	int lx[MAXN];
	int ly[MAXN];
	int slack[MAXN];
	int nCount;
	int weight[MAXN][MAXN];

};

}; //end of namespace Graph

}; //end of namespace Graph

#endif /* BIGRAPH_MATCH_H_ */
