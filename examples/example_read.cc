#include <iostream>
#include <fstream>
#include "loginput.hh"
#include "laser_readings.h"

using namespace std;
using namespace Logging;

int main(int argc, char** argv)
{
    ifstream logfile(argv[1]);

    Logging::Input input;
    input.init(logfile);

    cerr << argv[1] << " has " << input.size() << " streams" << endl;
    for (size_t i = 0; i < input.size(); ++i)
    {
        DataStream& stream = dynamic_cast<DataStream&>(input[i]);
        cerr << "  stream " << i << " is " << stream.getName() << "[" << stream.getTypeName() << "]" << endl;

        DataInputIterator it = stream.begin();
        DataInputIterator const end = stream.end();
        while (it != end)
        {
            DFKI::LaserReadings readings = it.getData<DFKI::LaserReadings>();
            cout << readings.stamp << endl;

            ++it;
        }

    }
}

