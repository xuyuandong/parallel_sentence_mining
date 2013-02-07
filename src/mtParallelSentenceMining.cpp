//============================================================================
// Name        : mtParallelSentenceMining.cpp
// Author      : x
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <exception>
#include "mt_corpus_extractor.h"
#include "mt_parallel_mining_task.h"

using namespace std;
using namespace mt_parallel;

void testOnUrl()
{
	MtCorpusExtractor* cextractor = new MtCorpusExtractor();
    
    ifstream fin;
    try
    {
        fin.open("originalURL");
        if (fin.fail())
        {
            cout << "fail to open url list file: originalURL" << endl;
            throw;
        }
        
        string urlstr;
        while (getline(fin, urlstr))
        {
            cout << "extract " << urlstr << " ..." << endl;
            ofstream ftmp("tmpname");
            ftmp << urlstr;
            ftmp.close();

	        if (!cextractor->loadHtmlFromUrl(urlstr.c_str()))  
	        {
		        cout << "fail to load html from url" << endl;
        		throw ;
	        }

        	int num = cextractor->doExtract();
            if(num < 0)
	        {
		        cout << "fail to extract sentence" << endl;
    		    throw;
	        }
            cout << "ok" << endl;
        }
        cout << "done" << endl;
    }
    catch (exception & e)
    {
        cout << "exception: " << e.what() << endl;
    }
    
    fin.close();
    delete cextractor;
}


int main(int argc, char* argv[]) 
{
    if (argc == 1)
    {
        cout << "test on url..." << endl;
        testOnUrl();
    }
    else if (argc == 2)
    {
        cout << "usage: program  data_file  option[galaxy | 194G]" << endl;
        return 0;
    }
    else 
    {
        MtParallelMiningTask task;
        if (strcmp(argv[2], "galaxy") == 0)
        {
            cout << "run on galaxy data" << endl;
            task.RunGalaxy(argv[1]);
        }
        else
        {
            cout << "run on 194G data" << endl;
            task.Run194G(argv[1]);
        }
    }
        
    return 0;
}
