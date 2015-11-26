#include "MultiFileIndex.hpp"
#include "Index.hpp"
#include "IndexFile.hpp"
#include <map>
#include <iostream>
#include "InputDataStream.hpp"

namespace pocolog_cpp
{
    
MultiFileIndex::MultiFileIndex(const std::vector< std::string >& fileNames)
{
    createIndex(fileNames);
}

bool MultiFileIndex::createIndex(const std::vector< LogFile* >& logfiles)
{
    //order all streams by time
    std::multimap<base::Time, IndexEntry> streamMap;

    globalSampleCount = 0;
    
    size_t globalStreamIdx = 0;
    
    for(LogFile *curLogfile : logfiles)
    {
        for(Stream *stream : curLogfile->getStreams())
        {
            globalSampleCount += stream->getSize();
            
            IndexEntry entry;
            entry.stream = stream;
            entry.globalStreamIdx = globalStreamIdx;
            
            streamToGlobalIdx.insert(std::make_pair(stream, globalStreamIdx));
            
            globalStreamIdx++;
            streamMap.insert(std::make_pair(stream->getFistSampleTime(), entry));

            InputDataStream *dataStream = dynamic_cast<InputDataStream *>(entry.stream);
            if(dataStream)
            {
                combinedRegistry.merge(dataStream->getStreamRegistry());
            }
            streams.push_back(stream);
        }
        std::cout << "Loading logfile Done " << curLogfile->getFileName() << std::endl;
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
        
        //ignore empty stream here, no data to play back
        if(!curEntry.stream->getSize())
            continue;

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

size_t MultiFileIndex::getGlobalStreamIdx(Stream* stream) const
{
    auto it = streamToGlobalIdx.find(stream);
    if(it != streamToGlobalIdx.end())
        return it->second;
    
    throw std::runtime_error("Error, got unknown stream");
}


bool MultiFileIndex::createIndex(const std::vector< std::string >& fileNames)
{
    for(std::vector< std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); it++ )
    {
        std::cout << "Loading logfile " << *it << std::endl;
        LogFile *curLogfile = new LogFile(*it);
        logFiles.push_back(curLogfile);
        std::cout << "Loading logfile Done " << *it << std::endl;
    }

    return createIndex(logFiles);
}

   
    
}