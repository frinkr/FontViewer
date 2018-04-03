#include <iostream>
#include <zlib.h>
#include "UI-Qt/QUMain.h"
#include "FontX/FXLib.h"

int main(int argc, char *argv[])
{
    FXLib::init(FX_RESOURCES_DIR "/UCD");
    
    quMain(argc, argv);
    
    FXLib::finish();
}
