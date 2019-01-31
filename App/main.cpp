#include "FontX/FXLib.h"
#include "UI-Qt/QXMain.h"

int main(int argc, char *argv[])
{
    FXLib::init(FX_RESOURCES_DIR "/UCD");
    qxMain(argc, argv);
    FXLib::finish();
}
