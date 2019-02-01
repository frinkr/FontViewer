#pragma once
class QMenuBar;
class QToolBar;

class QXTheme {
public:
    static void
    applyDarkFusion();

    static void
    applyDarkFusionOnMenuBar(QMenuBar * menuBar);

    static void
    applyDarkFusionOnToolBar(QToolBar * toolBar);

    static void
    applyDarkFusionOnWindowTitleBar();
};
