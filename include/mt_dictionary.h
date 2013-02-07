/**
 * mt_dictionary.h
 * Created 2010.4.19
 *      Author yuandong.xu
 */

#ifndef MT_DICTIONARY_H_
#define MT_DICTIONARY_H_

#include <map>
#include <set>
#include <string>
#include <memory>

using namespace std;

namespace mt_parallel
{
    
class MtDictionary
{
public:
    static MtDictionary* Instance()
    {
        if(0 == m_dict.get())
        {
            m_dict.reset(new MtDictionary());
            //m_dict = auto_ptr<MtDictionary>(new MtDictionary());
        }
        return m_dict.get();
    }

    inline const set<string> & JP2CH(const string & word){
        return jpToch[word];
    }

    inline const set<string> & CH2JP(const string & word){
        return chTojp[word];
    }

    inline const set<string> & EN2CH(const string & word){
        return enToch[word];
    }

protected:
    MtDictionary();
    ~MtDictionary();
    
    friend class auto_ptr<MtDictionary>;
    static auto_ptr<MtDictionary> m_dict;
    
private:
    // dictionary map
    map<string, set<string> > enToch;
    map<string, set<string> > chToen;
    map<string, set<string> > jpToch;
    map<string, set<string> > chTojp;
    // exception words 
    set<string> enCommonWords;
    set<string> enStopWords;
        
};

}; // end of namespace

#endif
