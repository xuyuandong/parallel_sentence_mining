/**
 * mt_similarity_model.cpp
 * Created on 2010.4.27
 *      Author yuandong.xu
 */

#include "mt_similarity_model.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <maxent/maxentmodel.hpp>

using namespace std;
using namespace maxent;
namespace mt_parallel
{

auto_ptr<MtSimilarityModel> MtSimilarityModel::p_model;
    
MtSimilarityModel::MtSimilarityModel()
{
    fstream _file;
    _file.open("xModel", ios::in);
    if (!_file)
    {
        cout << "Fail to load Maxent model" << endl;
    }

    model.load("xModel");
}

MtSimilarityModel::~MtSimilarityModel()
{
}

double MtSimilarityModel::evaluate(vector<pair<string, float> > & feature)
{
    return model.eval(feature, "1");
}

};
