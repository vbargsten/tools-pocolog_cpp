#ifndef STREAMDESCRIPTION_H
#define STREAMDESCRIPTION_H

#include <string>
#include "Format.hpp"
#include "FileStream.hpp"

namespace pocolog_cpp
{

class StreamDescription
{
    std::string m_fileName;
    StreamType    m_type;
    size_t        m_index;
    std::string m_streamName;
    std::string m_typeName;
    std::string m_typeDescription;
    std::string m_metadata;

    
    std::string readString(FileStream &fileStream);
public:
    StreamDescription(const std::string& fileName, pocolog_cpp::FileStream& fileStream, size_t stream_idx, size_t blockSize);
    StreamDescription();
    ~StreamDescription();
    
    size_t getIndex() const
    { 
        return m_index; 
    }

    StreamType getType() const
    {
        return m_type;
    }

    const std::string &getName() const
    {
        return m_streamName;
    }

    const std::string &getTypeName() const
    {
        return m_typeName;
    }

    const std::string &getTypeDescription() const
    {
        return m_typeDescription;
    }

    const std::string &getMetadata() const
    {
        return m_metadata;
    }
    
    const std::string &getFileName() const
    {
        return m_fileName;
    }

    
    
};
}
#endif // STREAMDESCRIPTION_H
