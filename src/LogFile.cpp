#include "LogFile.hpp"
#include "StreamDescription.hpp"
#include "IndexFile.hpp"
#include "InputDataStream.hpp"
#include <iostream>
         
namespace pocolog_cpp
{

LogFile::LogFile(const std::string& fileName) : filename(fileName)
{

    logFile.open(fileName.c_str(), std::ifstream::binary | std::ifstream::in);
    
    readBuffer.resize(8096 * 1024);
//     logFile.rdbuf()->pubsetbuf(readBuffer.data(), readBuffer.size());
    
    if (! logFile.good())
        throw std::runtime_error("Error, empty File");

    // Load the prologue
    Prologue prologue;
    logFile.read(reinterpret_cast<char*>(&prologue), sizeof(prologue));
    if (! logFile.good() || std::string(prologue.magic, 7) != std::string(FORMAT_MAGIC))
        throw std::runtime_error("Error, Bad Magic Block, not a Pocolog file ?");;

    nextBlockHeaderPos = logFile.tellg();
    std::streampos lastBlockHeaderPos = nextBlockHeaderPos;
    do 
    {
        lastBlockHeaderPos = nextBlockHeaderPos;
        if(!readNextBlockHeader())
        {
            throw std::runtime_error("Error, could not read block header");
        }
        if(curBlockHeader.type != StreamBlockType)
        {
            break;
        }

        descriptions.push_back(StreamDescription(fileName, logFile, curBlockHeader.stream_idx, curBlockHeader.data_size));
    } while (curBlockHeader.type == StreamBlockType);
    
    nextBlockHeaderPos = lastBlockHeaderPos;
    
    gotBlockHeader = false;

    std::cout << "Loaded " << descriptions.size() << " stream descriptions " << std::endl;
    
    //load Index
    IndexFile indexFile(*this);
    
    
    for(std::vector<StreamDescription>::const_iterator it = descriptions.begin(); it != descriptions.end();it++)
    {
        switch(it->getType())
        {
            case DataStreamType:
                std::cout << "Creating InputDataStream " << it->getName() << std::endl;
                streams.push_back(new InputDataStream(*it, indexFile.getIndexForStream(*it)));
                
                break;
            default:
                std::cout << "Ignoring stream " << it->getName() << std::endl;
                break;
        }
    }

    
    
}

const std::vector< Stream* >& LogFile::getStreams() const
{
    return streams;
}

Stream& LogFile::getStream(const std::string streamName) const
{
    for(std::vector<Stream *>::const_iterator it = streams.begin(); it != streams.end(); it++)
    {
        std::cout << "Stream " << (*it)->getName() << std::endl;
        if(streamName == (*it)->getName())
            return **it;
    }
    throw std::runtime_error("Error stream " + streamName + " not Found");
}

const std::vector< StreamDescription >& LogFile::getStreamDescriptions() const
{
    return descriptions;
}

std::string LogFile::getFileName() const
{
    return filename;
}

std::string LogFile::getFileBaseName() const
{
    std::string baseName(filename);
    //easy implemenatiton, filename should end as '.log'
    baseName.resize(filename.size() - 4);
    
    return baseName;
}

bool LogFile::readNextBlockHeader()
{
    logFile.seekg(nextBlockHeaderPos);
    if(logFile.eof())
        return false;
    
    curBlockHeaderPos = nextBlockHeaderPos;
    
    logFile.read((char *) &curBlockHeader, sizeof(BlockHeader));
    if(!logFile.good())
    {
        std::cout << "Reading Block Header failedSample Pos is " << curBlockHeaderPos << std::endl;

        return false;
    }

    nextBlockHeaderPos += sizeof(BlockHeader) + curBlockHeader.data_size;
    curSampleHeaderPos = curBlockHeaderPos;
    curSampleHeaderPos += sizeof(BlockHeader);
    
    gotBlockHeader = true;
    
    return true;
}

bool LogFile::readSampleHeader()
{
    if(!gotBlockHeader)
    {
        throw std::runtime_error("Internal Error: Called readSampleHeader without reading Block header first");
    }
    
    logFile.seekg(curSampleHeaderPos);
    
    if(logFile.eof() || logFile.fail())
    {
        std::cout << "Sample Pos is " << curSampleHeaderPos << " block header pos " << curBlockHeaderPos << std::endl;
        throw std::runtime_error("Error, log file corrupted, could not read Sample Header");
    }
    
    logFile.read((char *) &curSampleHeader, sizeof(SampleHeaderData));
    if(!logFile.good())
    {
        throw std::runtime_error("Error, log file corrupted, could not read Sample Header");
    }
    
    gotSampleHeader = true;

    return true;  
}

std::streampos LogFile::getSamplePos() const
{
    if(!gotBlockHeader)
    {
        throw std::runtime_error("Internal Error: Called getSamplePos without reading Block header first");
    }
    std::streampos ret(curSampleHeaderPos);
    ret += sizeof(SampleHeaderData);
    return ret; 
}

size_t LogFile::getSampleStreamIdx() const
{
    if(!gotBlockHeader)
    {
        throw std::runtime_error("Internal Error: Called getSampleStreamIdx without reading Block header first");
    }
    return curBlockHeader.stream_idx;
}

const base::Time LogFile::getSampleTime() const
{
    if(!gotSampleHeader)
    {
        throw std::runtime_error("Internal Error: Called getSampleTime without reading Sample header first");
    }
    return base::Time::fromMicroseconds(curSampleHeader.realtime);
}



    
}

    

