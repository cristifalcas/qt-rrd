#include <QApplication>

#include "statsgraph.h"
#include "commons.h"

int main(int argc, char* argv[])
{

    qInstallMsgHandler(myMessageOutput);

    QApplication app(argc, argv);

    statsGraph myapp;
    myapp.draw();
    return myapp.exec();
    //TODO
    // more threads
    // cache the files in ram
    //only ask about overwrite on export
    //add more buttons, reload info, save, new
    //implement gnu tools also. They must be faster.
    //check the rrd lib for linux
}

/*

Database exists?
    - yes: update configuration with db values
    - no: keep defaults
- move from exists to not exists
    - write last or defaults?
*/
