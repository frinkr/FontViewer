#include "FontX/FXLib.h"
#include "UI-Qt/QUMain.h"

int main(int argc, char *argv[])
{
    FXLib::init(FX_RESOURCES_DIR "/UCD");
    quMain(argc, argv);
    FXLib::finish();
}
