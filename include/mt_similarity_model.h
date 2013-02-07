/**
 * mt_similarity_model.h
 * Created on 2010.4.27
 *      Author yuandong.xu
 */

#ifndef MT_SIMILARITY_MODEL_H_
#define MT_SIMILARITY_MODEL_H_

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <maxent/maxentmodel.hpp>

using namespace std;
using namespace maxent;

namespace mt_parallel
{

class MtSimilarityModel
{
public:
    static MtSimilarityModel* Instance()
    {
        if (0 == p_model.get())
        {
            p_model.reset(new MtSimilarityModel());
        }
        return p_model.get();
    }

    double evaluate(vector<pair<string, float> > & feature);

protected:
    MtSimilarityModel();
    ~MtSimilarityModel();
    
    friend class auto_ptr<MtSimilarityModel>;
    static auto_ptr<MtSimilarityModel> p_model;

private:
    MaxentModel model;
    
};

}; // end of namespace

#endif
