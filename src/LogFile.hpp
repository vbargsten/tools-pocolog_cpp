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
    std::streampos firstBlockHeaderPos;
    std::streampos nextBlockHeaderPos;
    std::streampos curBlockHeaderPos;
    std::streampos curSampleHeaderPos;
    FileStream logFile;
    
    std::vector<Stream *> streams;
    std::vector<StreamDescription> descriptions;

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

    const BlockHeader &getCurBlockHeader() const;
    
    bool readNextBlockHeader(struct BlockHeader &curBlockHeade);
    bool readNextBlockHeader();
    bool readSampleHeader();
    bool checkSampleComplete();
    
    bool readCurBlock(std::vector<uint8_t> &blockData);
    
    
    std::streampos getSamplePos() const;
    const base::Time getSampleTime() const;
    size_t getSampleStreamIdx() const;
    
    bool eof() const;
    
    Stream &getStream(const std::string streamName) const;
    
};
}
#endif // LOGFILE_H
