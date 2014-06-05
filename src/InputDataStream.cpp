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


}
