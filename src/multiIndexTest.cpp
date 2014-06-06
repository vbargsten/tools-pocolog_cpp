#include "MultiFileIndex.hpp"
#include <iostream>
#include "Stream.hpp"

int main(int argc, char** argv)
{
    std::vector<std::string> filenames;
    
    for(int i = 1; i < argc; i++)
    {
        filenames.push_back(argv[i]);
    }
    
    pocolog_cpp::MultiFileIndex multiIndex(filenames);
    
    std::cout << "Replaying all samples" << std::endl;
    base::Time start(base::Time::now());
    
    size_t allSamples = multiIndex.getSize();
    int lastPercentage = 0;
    
    for(size_t i = 0; i < allSamples; i++)
    {
        pocolog_cpp::Stream *stream = multiIndex.getSampleStream(i);
        std::vector<uint8_t> data;
        stream->getSampleData(data, multiIndex.getPosInStream(i));
        
        int curPercentag = (i * 100 / allSamples);
        if(lastPercentage != curPercentag)
        {
            lastPercentage = curPercentag;
            std::cout << "\r" << lastPercentage << "% Done" << std::flush;
        }
        
    }

    base::Time end(base::Time::now());

    base::Time firstSampleTime = multiIndex.getSampleStream(0)->getFistSampleTime();
    base::Time lastSampleTime = multiIndex.getSampleStream(allSamples - 1)->getLastSampleTime();
    
    std::cout << "First sample time " << firstSampleTime.toString() << " last sample time " << lastSampleTime.toString() << std::endl;
    
    std::cout << "Took " << end-start << " realtime " << lastSampleTime - firstSampleTime << std::endl;
    
    return 0;
    
}