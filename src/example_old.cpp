#include <iostream>
#include <fstream>
#include "Read.hpp"

using namespace std;
using namespace pocolog_cpp;

int main(int argc, char** argv)
{
    ifstream logfile(argv[1]);

    pocolog_cpp::Input input;
    input.init(logfile);

    cerr << argv[1] << " has " << input.size() << " streams" << endl;
    for (size_t i = 0; i < input.size(); ++i)
    {
        DataStream& stream = dynamic_cast<DataStream&>(input[i]);
        cerr << "  stream " << i << " is " << stream.getName() << "[" << stream.getTypeName() << "]" << endl;
    }
}

