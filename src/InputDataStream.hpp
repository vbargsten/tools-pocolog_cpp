#ifndef INPUTDATASTREAM_H
#define INPUTDATASTREAM_H

#include "Stream.hpp"
#include <string>
#include <typelib/value_ops.hh>

namespace Typelib
{
    class Registry;
    class Type;
}

namespace pocolog_cpp
{

class InputDataStream : public Stream
{
    
protected:
    const Typelib::Type*       m_type;
    Typelib::Registry*   m_registry;

    void loadTypeLib();
    
public:
    InputDataStream(const StreamDescription &desc, Index &index);
    virtual ~InputDataStream();

    Typelib::Type const* getType() const;
    
    template<typename T>
    bool getSample(T& out, size_t sampleNr)
    {
        std::vector<uint8_t> buffer;
        if(!getSampleData(buffer, sampleNr))
            return false;
        
        Typelib::Value v(&out, sizeof(T), *m_type);
        Typelib::load(v, buffer);
        return true;
    }
};

}


#endif // INPUTDATASTREAM_H
