#include "InputDataStream.hpp"
#include <typelib/pluginmanager.hh>
#include <typelib/registry.hh>

namespace pocolog_cpp
{

// InputDataStream::InputDataStream()
// {
// 
// }
// 
// InputDataStream::~InputDataStream()
// {
// 
// }

InputDataStream::InputDataStream(const StreamDescription& desc, Index& index): Stream(desc, index)
{
    loadTypeLib();
}

InputDataStream::~InputDataStream()
{

}


const std::string InputDataStream::getCXXType() const
{
    std::string metadata = desc.getMetadata();
    if(metadata.find("rock_cxx_type_name") == std::string::npos)
    {
        throw std::runtime_error("couldn't find log metadata! original type extraction from metadata only works with new logfiles");
    }
    
    std::string rock_cxx_type_name = metadata.substr(metadata.find(":") + 1, metadata.length());
    rock_cxx_type_name = rock_cxx_type_name.substr(0, rock_cxx_type_name.find('\n'));
    rock_cxx_type_name.erase(std::remove(rock_cxx_type_name.begin(), rock_cxx_type_name.end(), ' '), rock_cxx_type_name.end());
    return rock_cxx_type_name;
}


void InputDataStream::loadTypeLib()
{
    utilmm::config_set empty;
    m_registry = new Typelib::Registry;
    
    std::istringstream stream(desc.getTypeDescription());

    // Load the data_types registry from pocosim
    Typelib::PluginManager::load("tlb", stream, empty, *m_registry);
    
    m_type = m_registry->build(desc.getTypeName());
}

const Typelib::Type* InputDataStream::getType() const
{
    return m_type; 
}

Typelib::Value InputDataStream::getTyplibValue(void *memoryOfType, size_t memorySize, size_t sampleNr)
{
    std::vector<uint8_t> buffer;
    if(!getSampleData(buffer, sampleNr))
        throw std::runtime_error("Error, sample for stream " + desc.getName() + " could not be loaded");

    if(memorySize < m_type->getSize())
    {
        throw std::runtime_error("Error, given memory area is to small for type " + m_type->getName() + " at stream " + desc.getName());
    }

    Typelib::Value v(memoryOfType, *m_type);
    //init memory area
    Typelib::init(v);
    Typelib::load(v, buffer);
    return v;
}

}
