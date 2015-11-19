#include "Index.hpp"
#include "LogFile.hpp"
#include "IndexFile.hpp"
#include <stdint.h>
#include <unistd.h>
#include <iostream>

namespace pocolog_cpp
{
    
Index::Index(std::string indexFileName, size_t streamIdx): curSampleNr(-1)
{
    indexFile.open(indexFileName.c_str(), std::ifstream::binary | std::ifstream::in);
    
    indexFile.seekg(streamIdx * sizeof(IndexPrologue) + sizeof(IndexFile::IndexFileHeader));
    
    indexFile.read((char *) & prologue, sizeof(IndexPrologue));
    
    firstSampleTime = base::Time::fromMicroseconds(prologue.firstSampleTime);
    lastSampleTime = base::Time::fromMicroseconds(prologue.lastSampleTime);
}

Index::Index(const pocolog_cpp::StreamDescription& desc, off_t posOfStreamDesc) : firstAdd(true), curSampleNr(-1)
{
    prologue.streamIdx = desc.getIndex();
    prologue.nameCrc = 0;
    prologue.numSamples = 0;
    prologue.streamDescPos = posOfStreamDesc;
    name = desc.getName();
}

bool Index::matches(const StreamDescription& odesc) const
{
    return prologue.streamIdx == odesc.getIndex();
//    return (desc.getName() == odesc.getName() && desc.getTypeName() == odesc.getTypeName());
}


void Index::addSample(off_t filePosition, const base::Time& sampleTime)
{
    if(firstAdd)
    {
        prologue.firstSampleTime = sampleTime.microseconds;
    }
    
    IndexInfo info;
    info.samplePosInLogFile = filePosition;
    info.sampleTime = sampleTime.microseconds;
    buildBuffer.push_back(info);
    
    prologue.lastSampleTime = sampleTime.microseconds;
    prologue.numSamples++;
}

off_t Index::getPrologueSize()
{
    return sizeof(IndexPrologue);
}

off_t Index::writeIndexToFile(std::fstream& indexFile, off_t prologPos, off_t indexDataPos)
{
    indexFile.seekp(prologPos, std::fstream::beg);
    if(!indexFile.good())
        throw std::runtime_error("Error writing index file");
    
    prologue.dataPos = indexDataPos;
    
    if(prologue.numSamples)
    {
        prologue.firstSampleTime = buildBuffer[0].sampleTime;
        prologue.lastSampleTime = buildBuffer[prologue.numSamples -1].sampleTime;
    }
    
//     std::cout << "Found " << prologue.numSamples << " in stream " << name<< std::endl; 
    
    indexFile.write((char *) &prologue, sizeof(IndexPrologue));
    if(!indexFile.good())
        throw std::runtime_error("Error writing index file");
    
    indexFile.seekp(indexDataPos, std::fstream::beg);
    if(!indexFile.good())
        throw std::runtime_error("Error writing index file");

//     std::cout << "Wrinting " << buildBuffer.size() * sizeof(IndexInfo) / 1024 << " KBytes to index File " << std::endl;
    
    indexFile.write((char *) buildBuffer.data(), buildBuffer.size() * sizeof(IndexInfo));
    if(!indexFile.good())
        throw std::runtime_error("Error writing index file");
    
//     std::cout << "Done new pos " << indexFile.tellp() << std::endl; 
    
    return indexFile.tellp();
}

void Index::loadIndex(size_t sampleNr)
{
    if(sampleNr >= prologue.numSamples)
        throw std::runtime_error("Error sample out of index requested");
        
    if(sampleNr != curSampleNr)
    {
        std::streampos pos(prologue.dataPos + sampleNr * sizeof(IndexInfo));
//         std::cout << "Seeking to " << pos << " start of index Data " << prologue.dataPos << " pos in data " << sampleNr * sizeof(IndexInfo) << std::endl;
        indexFile.seekg(std::streampos(prologue.dataPos + sampleNr * sizeof(IndexInfo)));
        if(!indexFile.good())
            throw std::runtime_error("Internal Error, index file is corrupted");
        indexFile.read((char *) &curIndexInfo, sizeof(IndexInfo));
        if(!indexFile.good())
            throw std::runtime_error("Internal Error, index file is corrupted");
        
        curSampleNr = sampleNr;
    }
}


std::streampos Index::getSamplePos(size_t sampleNr)
{

    loadIndex(sampleNr);
    
    return std::streampos(curIndexInfo.samplePosInLogFile);
    
}

base::Time Index::getSampleTime(size_t sampleNr)
{
    loadIndex(sampleNr);

    return base::Time::fromMicroseconds(curIndexInfo.sampleTime);
}

Index::~Index()
{
    indexFile.close();
}
}