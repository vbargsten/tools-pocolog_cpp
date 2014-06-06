#include "IndexFile.hpp"
#include "Stream.hpp"
#include "Index.hpp"
#include "LogFile.hpp"
#include <string.h>
#include <iostream>

namespace pocolog_cpp
{
    
IndexFile::IndexFileHeader::IndexFileHeader() : numStreams(0)
{
    memcpy(magic, getMagic().c_str(), sizeof(magic));
}

std::string IndexFile::IndexFileHeader::getMagic()
{
    return std::string("IndexV2");
}

    
IndexFile::IndexFile(std::string indexFileName)
{
    if(!loadIndexFile(indexFileName))
        throw std::runtime_error("Error, index is corrupted");
    
}

IndexFile::IndexFile(LogFile &logFile)
{
    std::string indexFileName(logFile.getFileBaseName() + ".id2");
    if(!loadIndexFile(indexFileName))
    {
        if(!createIndexFile(indexFileName, logFile))
        {
            throw std::runtime_error("Error building Index");
        }
        if(!loadIndexFile(indexFileName))
            throw std::runtime_error("Internal Error, created index is corrupted");
    }
}

bool IndexFile::loadIndexFile(std::string indexFileName)
{
//     std::cout << "Loading Index File " << std::endl;
    std::ifstream indexFile(indexFileName.c_str(), std::fstream::in | std::fstream::binary );
    
    if(!indexFile.good())
        return false;
    
    IndexFileHeader header;
    
    indexFile.read((char *) &header, sizeof(IndexFileHeader));
    if(!indexFile.good())
        return false;

    if(header.magic != header.getMagic())
    {
        std::cout << "Migic is " << header.magic << std::endl;;
        std::cout << "Migic should be " << header.getMagic() << std::endl;;
        std::cout << "Error, index magic does not match" << std::endl;
        return false;
    }
    
    indexFile.close();
    
    for(int i = 0; i < header.numStreams; i++)
    {
        //load streams
        indices.push_back(new Index(indexFileName, i));
    }
    
    return true;
    
}

Index& IndexFile::getIndexForStream(const StreamDescription& desc)
{
    for(std::vector<Index *>::const_iterator it = indices.begin(); it != indices.end(); it++ )
    {
        if((*it)->matches(desc))
            return **it;
    }
    
    throw std::runtime_error("IndexFile does not contain valid index for Stream ");
}

bool IndexFile::createIndexFile(std::string indexFileName, LogFile& logFile)
{
    std::cout << "Creating Index File for logfile " << logFile.getFileName() << std::endl;
    std::vector<char> writeBuffer;
    writeBuffer.resize(8096 * 1024);
    std::fstream indexFile;
    indexFile.rdbuf()->pubsetbuf(writeBuffer.data(), writeBuffer.size());

    indexFile.open(indexFileName.c_str(), std::fstream::out | std::fstream::binary | std::fstream::trunc);
    
    const std::vector< StreamDescription > streams(logFile.getStreamDescriptions());
    indices.resize(streams.size());
    
//     std::cout << "Found " << streams.size() << " datastreams " << std::endl;
    
    for(std::vector< StreamDescription >::const_iterator it = streams.begin(); it != streams.end(); it++)
    {
        Index *newIndex = new Index(*it);
        size_t index = it->getIndex();
        if(index >= indices.size() )
        {
            throw std::runtime_error("Error building Index, Unexpected stream index");
        }
        indices[index] = newIndex;

    }

    while(logFile.readNextBlockHeader())
    {
        if(!logFile.readSampleHeader())
        {
            throw std::runtime_error("Error building index, log file seems corrupted");
        }
        
        indices[logFile.getSampleStreamIdx()]->addSample(logFile.getSamplePos(), logFile.getSampleTime());
    }

    IndexFileHeader header;
    header.numStreams = streams.size();
    
    assert(header.magic == header.getMagic());
    
    indexFile.write((char *) &header, sizeof(header));
    if(!indexFile.good())
        throw std::runtime_error("Error writing index header");
    
    off_t curProloguePos = sizeof(IndexFileHeader);
    off_t curDataPos = indices.size() * Index::getPrologueSize() + sizeof(IndexFileHeader);
    for(std::vector<Index* >::iterator it = indices.begin(); it != indices.end(); it++)
    {
//         std::cout << "Writing index for stream " << (*it)->getName() << " , num samples " << (*it)->getNumSamples() << std::endl;
    
        //Write index prologue
        curDataPos = (*it)->writeIndexToFile(indexFile, curProloguePos, curDataPos);
        curProloguePos += Index::getPrologueSize();
        
        if(!indexFile.good())
            throw std::runtime_error("Error writing index File");
        
        delete *it;
    }
    
    indexFile.close();
//     std::cout << "done " << std::endl;
    
    indices.clear();;
    
    return true;
}


}