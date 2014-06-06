#include "StreamDescription.hpp"
#include <fstream>
#include <vector>
#include <iostream>

namespace pocolog_cpp
{
StreamDescription::StreamDescription() : m_index(-1)
{

}

    
std::string StreamDescription::readString(pocolog_cpp::FileStream& fileStream)
{
    uint32_t stringSize;
    fileStream.read((char *) &stringSize, sizeof(uint32_t));

    std::vector<char> buffer;
    buffer.resize(stringSize);
    fileStream.read(buffer.data(), stringSize);
    return std::string(buffer.begin(), buffer.end());
}

    
StreamDescription::StreamDescription(const std::string &fileName, pocolog_cpp::FileStream& fileStream, size_t stream_idx, size_t blockSize)
{
    std::streampos startPos = fileStream.tellg();
    m_fileName = fileName;
    m_index = stream_idx;
    
//     std::cout << "Start of Stream Declaration " << startPos << std::endl;

    uint8_t stream_type;
    fileStream.read((char *) &stream_type, sizeof(uint8_t));
    
    m_type = (enum StreamType) stream_type;

    switch(stream_type) 
    {
        case ControlStreamType:
        {
        }
        break;
        
        case DataStreamType:
        {
            m_streamName = readString(fileStream);
            m_typeName = readString(fileStream);

//             std::cout << "Found Stream, Name " << m_streamName << " of type " << m_typeName << std::endl;
            
            std::streampos curPos = fileStream.tellg();
            
            if(curPos - startPos < blockSize)
            {
                m_typeDescription = readString(fileStream);
//                 std::cout << "Found stream description : " << m_typeDescription << std::endl;
            }
            if(curPos - startPos < blockSize)
            {
                m_metadata = readString(fileStream);
            }
            
            curPos = fileStream.tellg();
            if(curPos - startPos != blockSize)
            {
                std::streampos diff = curPos - startPos;
//                 std::cout << "Start of Stream Declaration " << startPos << " diff " << diff << " expected size " << blockSize << std::endl;

//                 uint32_t stringSize = blockSize - diff;
// 
//                 std::vector<char> buffer;
//                 buffer.resize(stringSize);
//                 fileStream.read(buffer.data(), stringSize);
//                 std::cout << std::string(buffer.begin(), buffer.end()) << std::endl;
                
                throw std::runtime_error("Error, stream declaration has wrong size");
            }
        }
        break;
        
        default:
            throw std::runtime_error("Error, unexpected Stream description");
            break;
    };    
}

StreamDescription::~StreamDescription()
{

}


}
