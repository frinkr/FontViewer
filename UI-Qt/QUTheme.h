#pragma once
class QMenuBar;

class QUTheme {
public:
    static void
    applyDarkFusion();

    static void
    applyDarkFusionOnMenuBar(QMenuBar * menuBar);

    static void
    applyDarkFusionOnWindowTitleBar();
};
