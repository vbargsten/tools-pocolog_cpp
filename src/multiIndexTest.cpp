#include "MultiFileIndex.hpp"

int main(int argc, char** argv)
{
    std::vector<std::string> filenames;
    
    for(int i = 1; i < argc; i++)
    {
        filenames.push_back(argv[i]);
    }
    
    pocolog_cpp::MultiFileIndex multiIndex(filenames);
    
    return 0;
    
}