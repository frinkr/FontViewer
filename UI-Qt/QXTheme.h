#pragma once
class QMenuBar;

class QXTheme {
public:
    static void
    applyDarkFusion();

    static void
    applyDarkFusionOnMenuBar(QMenuBar * menuBar);

    static void
    applyDarkFusionOnWindowTitleBar();
};
