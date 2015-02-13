#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main(int argc, char **argv)
{
    std::vector<char> readBuffer;
    const size_t bufferSize = 8096 * 1024;
    readBuffer.resize(bufferSize);
    
    if(argc > 1)
    {
        std::fstream file1;
        
        file1.rdbuf()->pubsetbuf(readBuffer.data(), readBuffer.size());
        file1.open("test.dat", std::ifstream::binary | std::ifstream::out);

        
        
        for(size_t i = 0 ; i < bufferSize; i++)
        {
            char t = 'a';
            file1.write(&t, sizeof(char));
        }
        
        file1.close();
    }
    else
    {
        int fd = ::open("test2.dat", O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
        if(fd < 0)
        {
            perror("f");
            std::cout << "Error opening File " << std::endl;
            return 0;
        }
        
        for(size_t i = 0 ; i < bufferSize; i++)
        {
            char t = 'a';
            readBuffer[i] = t;
        }
        
        size_t written = 0;
        
        std::cout << "Fd is " << fd <<std::endl;
        
        while(written < bufferSize)
        {
            int ret = ::write(fd, readBuffer.data() + written, bufferSize - written);
            if(ret < 0)
            {
                perror("foo");
                std::cout << "Error writing data" << std::endl;
                close(fd);
                return 0;
            }
            written += ret;
        }
        
        close(fd);
        
    }
    
    return 0;
}
