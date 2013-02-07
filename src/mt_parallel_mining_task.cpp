/*
 * mt_parallel_mining_task.cpp
 * Created on 2010.5.5
 *      Author: yuandong.xu
 */

#include <vector>
#include <fstream>

#include "mt_parallel_mining_task.h"
#include "mt_log_file.h"

using namespace std;

namespace mt_parallel
{

static const int MAX_FALSE_COUNT = 200 ;

MtParallelMiningTask::MtParallelMiningTask()
{
    m_extractor = new MtCorpusExtractor();
}

MtParallelMiningTask::~MtParallelMiningTask()
{
    if (m_extractor)
        delete m_extractor;
}

void MtParallelMiningTask::Run194G(const char* fname)
{
    MtLogFile* m_logfile = MtLogFile::Instance();
    
    (*m_logfile) << "Task begin ...\n ";// << endl;
    
    int count = 0;
    int falseCount = 0;
    
    ifstream pgIn;
    ofstream pgOut;
   
    std::string url;
    try
    {
        pgIn.open(fname);
        if (pgIn.fail())
        {
            (*m_logfile) << "fail to open " << fname <<"\n";// endl;
            throw;
        }
   
        bool isURL = false;
        bool isPOST = false;
        bool isCONTENT = false;
        bool isLENGTH = false;
        
        std::string page;
        std::string line;
        while (getline(pgIn, line))
        {
            if (falseCount >= MAX_FALSE_COUNT)
            {
                // if too many continous false line has been read, throw exception
                (*m_logfile) << "false count reached max number, throw\n";// << endl;
                throw falseCount;
            }
            
            // check whether read pangu file valid
            if (line.empty())
            {
                ++falseCount;
                continue;
            }
            else
            {
                falseCount =  0;
            }

            if (0 == line.find("URL:"))
            {
                isURL = true;
                isLENGTH = false;

                if (url.size() > 0 && page.size() > 0)
                {            
                    ++count;
                    
                    //ofstream fout("tmpname");
                    //fout << url;
                    //fout.close();

                    if (!m_extractor->loadHtmlFromString(page, url))
                    {
                        (*m_logfile) << "fail to load html\n";// << endl;
                        throw;
                    }

                    page.clear();
                    
                    int num = m_extractor->doExtract();
                    if (num < 0)
                    {
                        (*m_logfile) << "fail to extract html\n";// << endl;
                        throw;
                    }
                    (*m_logfile) << "ok: " << url << "\n";//endl;
                }
                
                url.assign(line, 4, line.size() - 4);
            }
            else if (0 == line.find("POST") && isURL)
            {
                isURL = false;
                isPOST = true;
            }
            else if (0 == line.find("CONTENT") && isPOST)
            {
                isPOST = false;
                isCONTENT = true;
            }
            else if (0 == line.find("LENGTH") && isCONTENT)
            {
                isCONTENT = false;
                isLENGTH = true;
            }
            else if (isLENGTH)
            {
                size_t pos = line.find_first_of("\r\n");
                while (std::string::npos != pos)
                {
                    line[pos] = ' ';
                    pos = line.find_first_of("\r\n", pos + 1);
                }
                page.append(line);
            }
            
        }

        if (url.size() > 0 && page.size() > 0)
        {
            ++count;
            
            //ofstream fout("tmpname");
            //fout << url;
            //fout.close();
            
            if (!m_extractor->loadHtmlFromString(page, url))
            {
                (*m_logfile) << "fail to load html\n";// << endl;
                throw;
            }

            url.clear();
            page.clear();
            
            int num = m_extractor->doExtract();
            if (num < 0)
            {
                (*m_logfile) << "fail to extract html\n";// << endl;
                throw;
            }
            (*m_logfile) << "ok: " << url << "\n";//endl;
        }
        
    }
    // we caught the exceptions and exit normally to prevent the whole job fail
    // because of individual instance fail
    catch (exception & e) // we do not want this task to reload
    {
        (*m_logfile) << "exception in: " << url <<"\n";//<<endl;
        (*m_logfile) << "reason: " << e.what() << "\n";//endl;
    }

    pgIn.close();
    
    (*m_logfile) << "Page Count:\t" << count << "\n";//endl;
    (*m_logfile) << "Task done\n";
    
}


void MtParallelMiningTask::RunGalaxy(const char* fname)
{
    MtLogFile* m_logfile = MtLogFile::Instance();
    
    (*m_logfile) << "Task begin ... \n" ;//<< endl;

    int count = 0;
    int falseCount = 0;
    
    ifstream pgIn;
    string url;
   
    try
    {
        pgIn.open(fname);
        if (pgIn.fail())
        {
            (*m_logfile) << "fail to open " << fname << "\n";
            throw;
        }
   
        string page;
        string line;
        while (getline(pgIn, line))
        {
            if (falseCount >= MAX_FALSE_COUNT)
            {
                // if too many continous false line has been read, throw exception
                (*m_logfile) << "false count reached max number, throw\n";// << endl;
                throw falseCount;
            }
            
            // check whether read pangu file valid
            if (line.empty())
            {
                ++falseCount;
                continue;
            }
            else
            {
                falseCount =  0;
            }

            if ( line.find("URL::") == 0)
            {
                url.assign(line, 5, line.size() - 5);
                ofstream fo("tmpname");
                fo << url;
                fo.close();
                page.clear();
            }
            else
            {
                if (line.find("-----@@@@@-----") == 0)
                {
                    ++count;
                    
                    if (!m_extractor->loadHtmlFromString(page, url))
                    {
                        (*m_logfile) << "fail to load html\n";// << endl;
                        throw;
                    }
                    page.clear();
                    
                    int num = m_extractor->doExtract();
                    if (num < 0)
                    {
                        (*m_logfile) << "fail to extract html\n";// << endl;
                        throw;
                    }
                    (*m_logfile) << "ok: " << url << "\n";//endl;
                }
                else
                {
                    page.append(line);
                }
            }
        }
        
    }
    // we caught the exceptions and exit normally to prevent the whole job fail
    // because of individual instance fail
    catch (exception & e) // we do not want this task to reload
    {
        (*m_logfile) << "exception in: " << url << "\n";
        (*m_logfile) << "reason: " << e.what() << "\n";//endl;
    }

    pgIn.close();
    
    (*m_logfile) << "Page Count:\t" << count << "\n";//endl;
    (*m_logfile) << "Task done\n";
}




} // end of namespace 
