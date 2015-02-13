#include "MultiFileIndex.hpp"
#include "Index.hpp"
#include "IndexFile.hpp"
#include <map>
#include <iostream>


namespace pocolog_cpp
{
    
MultiFileIndex::MultiFileIndex(const std::vector< std::string >& fileNames)
{
    createIndex(fileNames);
    
}
   



bool MultiFileIndex::createIndex(const std::vector< std::string >& fileNames)
{
    //order all streams by time
    std::multimap<base::Time, IndexEntry> streamMap;

    globalSampleCount = 0;
    
    for(std::vector< std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); it++ )
    {
        std::cout << "Loading logfile " << *it << std::endl;
        LogFile *curLogfile = new LogFile(*it);
        
        for(std::vector<Stream *>::const_iterator jt = curLogfile->getStreams().begin(); jt != curLogfile->getStreams().end(); jt++)
        {
            if((*jt)->getSize())
            {
                globalSampleCount += (*jt)->getSize();
                
                IndexEntry entry;
                entry.stream = *jt;
                
                streamMap.insert(std::make_pair((*jt)->getFistSampleTime(), entry));
            }
        }
        
        logFiles.push_back(curLogfile);
        std::cout << "Loading logfile Done " << *it << std::endl;
    }

    index.resize(globalSampleCount);
    
    int64_t globalSampleNr = 0;
    
    int lastPercentage = 0;
    
    std::cout << "Building multi file index " << std::endl;
    
    while(!streamMap.empty())
    {
        //remove stream from map
        IndexEntry curEntry = streamMap.begin()->second;
        streamMap.erase(streamMap.begin());

        base::Time sampleTime = curEntry.stream->getFileIndex().getSampleTime(curEntry.sampleNrInStream);

        //add index sample
        index[globalSampleNr] = curEntry;

        curEntry.sampleNrInStream++;

        //check if we reached the end of the stream
        if(curEntry.sampleNrInStream < curEntry.stream->getSize())
        {
            //reenter stream
            streamMap.insert(std::make_pair(sampleTime, curEntry));
        }        

        globalSampleNr++;
        int curPercentag = (globalSampleNr * 100 / globalSampleCount);
        if(lastPercentage != curPercentag)
        {
            lastPercentage = curPercentag;
            std::cout << "\r" << lastPercentage << "% Done" << std::flush;
        }
    }
    std::cout << "\r 100% Done";
    std::cout << std::endl;

    std::cout << "Processed " << globalSampleNr << " of " << globalSampleCount << " samples " << std::endl;
    
    
    
    return true;
}

   
    
}