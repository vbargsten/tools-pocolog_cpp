#ifndef MULTIFILEINDEX_H
#define MULTIFILEINDEX_H
#include <vector>
#include <string>
#include <stdint.h>
#include <stdexcept>

namespace pocolog_cpp
{
class LogFile;
class Stream;
    
class MultiFileIndex
{
    struct IndexEntry
    {
        IndexEntry(): sampleNrInStream(0), stream(NULL) {};
        uint64_t sampleNrInStream;
        Stream *stream;
    } __attribute__((packed));
    
    std::vector<IndexEntry> index;
    std::vector<LogFile *> logFiles;
    size_t globalSampleCount;
//     base::Time firstSampleTime;
//     base::Time lastSampleTime;
public:
    MultiFileIndex(const std::vector<std::string> &fileNames);
    
    size_t getSize() const
    {
        return globalSampleCount;
    }
    
    Stream *getSampleStream(size_t globalSamplePos) const
    {
        if(globalSamplePos > globalSampleCount - 1)
            throw std::runtime_error("Error, Sample out of index requested");
        
        return index[globalSamplePos].stream;
    }
    
    size_t getPosInStream(size_t globalSamplePos) const
    {
        if(globalSamplePos > globalSampleCount - 1)
            throw std::runtime_error("Error, Sample out of index requested");
        
        return index[globalSamplePos].sampleNrInStream;
    }

    
private:
    bool createIndex(const std::vector<std::string> &fileNames);
};
}
#endif // MULTIFILEINDEX_H
