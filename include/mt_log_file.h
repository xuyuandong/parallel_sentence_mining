/*
 * mt_log_file.h
 * Created on 2010.5.7
 *      Author yuandong.xu
 */

#ifndef MT_LOG_FILE_H_
#define MT_LOG_FILE_H_

#include <fstream>
#include <memory>
#include <string>
#include <ctime>

using namespace std;

namespace mt_parallel
{

class MtLogFile 
{
public:
    static MtLogFile* Instance()
    {
        if (0 == m_plog.get())
        {
            m_plog.reset(new MtLogFile());
        }
        return m_plog.get();
    }

    template<typename T>
    inline MtLogFile & operator<< (const T & val)
    {
        flog << val;
        flog.flush();
        return *this;
    }

private:
    MtLogFile()
    {
        time_t tNowTime;
        time(&tNowTime);
        
        tm* tLocalTime = localtime(&tNowTime);
        const char* timeStr = asctime(tLocalTime);
        
        string tstr;
        for (int i = 0; i < 16; ++i)
        {
            if (timeStr[i] == ' ')
                tstr.push_back('-');
            else
                tstr.push_back(timeStr[i]);
        }
        
        string fname = "mtInfo-";
        fname.append(tstr);
        fname.append(".log");
        
        flog.open(fname.c_str(), ios::app);
    }
    
    ~MtLogFile()
    {
        flog.close();
    }

    friend class auto_ptr<MtLogFile>;
    static auto_ptr<MtLogFile> m_plog;

public:
    ofstream flog;
    
};


    
};// end of namespace

#endif
