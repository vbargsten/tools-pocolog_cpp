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
   
struct IndexEntry
{
    IndexEntry() : logFileIdx(0), streamIdxInLogFile(0), sampleNrInStream(0), sampleTime(0) {};
    uint16_t logFileIdx;
    uint16_t streamIdxInLogFile;
    uint64_t sampleNrInStream;
    uint64_t sampleTime;
};

struct BuildEntry {
    Stream *stream;
    IndexEntry entry;
};
   
bool MultiFileIndex::createIndex(const std::vector< std::string >& fileNames)
{
    std::vector<LogFile *> logFiles;
    
    std::vector<Index *> indices;
    std::vector<Stream *> streams;

    //order all streams by time
    std::multimap<base::Time, BuildEntry> streamMap;

    int64_t combinedSize = 0;
    
    uint16_t logFileIdx = 0;
    uint16_t streamIdxInLogFile = 0;
    
    for(std::vector< std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); it++ )
    {
        LogFile *curLogfile = new LogFile(*it);
        
        streamIdxInLogFile = 0;
        
        for(std::vector<Stream *>::const_iterator it = curLogfile->getStreams().begin(); it != curLogfile->getStreams().end(); it++)
        {
            indices.push_back(&((*it)->getFileIndex()));
            streams.push_back(*it);
            
            combinedSize += (*it)->getSize();
            
            BuildEntry entry;
            entry.stream = *it;
            entry.entry.logFileIdx = logFileIdx;
            entry.entry.streamIdxInLogFile = streamIdxInLogFile;
            
            streamMap.insert(std::make_pair((*it)->getFistSampleTime(), entry));
            streamIdxInLogFile++;
        }
        
        logFiles.push_back(curLogfile);
        logFileIdx++; 
    }

    int64_t globalSampleNr = 0;
    
    int lastPercentage = 0;
    
    std::cout << "Building multi file index " << std::endl;
    
    while(!streamMap.empty())
    {
        BuildEntry curEntry = streamMap.begin()->second;
        streamMap.erase(streamMap.begin());
        
        //check if we reached the end of the stream
        if(curEntry.entry.sampleNrInStream + 1 < curEntry.stream->getSize())
        {
            curEntry.entry.sampleNrInStream++;
            
            globalSampleNr++;
            int curPercentag = (globalSampleNr * 100 / combinedSize);
            if(lastPercentage != curPercentag)
            {
                lastPercentage = curPercentag;
                std::cout << "\r" << lastPercentage << "% Done" << std::flush;
            }
            
            //TODO write to index
            
            //reenter stream
            streamMap.insert(std::make_pair(curEntry.stream->getFileIndex().getSampleTime(curEntry.entry.sampleNrInStream), curEntry));
        }        
    }
    std::cout << "\r 100% Done";
    std::cout << std::endl;

    std::cout << "Processed " << globalSampleNr << " samples " << std::endl;
    
    
    
    return true;
}

   
    
}