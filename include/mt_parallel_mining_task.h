/*
 * mt_parallel_mining_task.h 
 * Created on 2010.5.5
 *      Author: yuandong.xu
 */

#ifndef MT_PARALLEL_MINING_TASK_H_
#define MT_PARALLEL_MINING_TASK_H_

#include "mt_corpus_extractor.h"
#include <string>

using namespace std;

namespace mt_parallel
{

class MtParallelMiningTask
{
public:

    MtParallelMiningTask();
    ~MtParallelMiningTask();

    void RunGalaxy(const char* fname);
    void Run194G(const char* fname);

private:
    
    MtCorpusExtractor* m_extractor;
};


}; // end of namespace

#endif




