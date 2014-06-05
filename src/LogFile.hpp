#ifndef LOGFILE_H
#define LOGFILE_H
#include <string>
#include <vector>
#include "Stream.hpp"
#include "Format.hpp"
#include "FileStream.hpp"

namespace pocolog_cpp
{
class LogFile
{
    std::vector<char > readBuffer;
    std::string filename;
    std::streampos nextBlockHeaderPos;
    std::streampos curBlockHeaderPos;
    std::streampos curSampleHeaderPos;
    FileStream logFile;
    
    std::vector<StreamDescription> descriptions;
    std::vector<Stream *> streams;
    
    bool gotBlockHeader;
    struct BlockHeader curBlockHeader;
    bool gotSampleHeader;
    struct SampleHeaderData curSampleHeader;
    
public:
    LogFile(const std::string &fileName);
    
    std::string getFileName() const;
    std::string getFileBaseName() const;
    
    const std::vector<Stream *> &getStreams() const;
    const std::vector<StreamDescription> &getStreamDescriptions() const; 
    
    bool readNextBlockHeader();
    bool readSampleHeader();
    
    std::streampos getSamplePos() const;
    const base::Time getSampleTime() const;
    size_t getSampleStreamIdx() const;
    
    Stream &getStream(const std::string streamName) const;
    
};
}
#endif // LOGFILE_H
