#include "application.h"
#include "bsp.h"

#include <iostream>
using std::cerr;
using std::endl;
using std::stoi;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <path> <hdr>" << endl;
        return EXIT_FAILURE;
    }

    Application a("BSP Viewer");
    BSP bsp;
    bsp.LoadBSPFile(argv[1], stoi(argv[2]) != 0);
    return a.exec();
}
