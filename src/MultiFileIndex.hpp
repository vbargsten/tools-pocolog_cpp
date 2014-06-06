#ifndef MULTIFILEINDEX_H
#define MULTIFILEINDEX_H
#include <vector>
#include <string>


namespace pocolog_cpp
{
    
class MultiFileIndex
{
public:
    MultiFileIndex(const std::vector<std::string> &fileNames);
private:
    bool createIndex(const std::vector<std::string> &fileNames);
    
};
}
#endif // MULTIFILEINDEX_H
