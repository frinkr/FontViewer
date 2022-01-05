
#include <windows.h>
#include <WinUser.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <objidl.h> // Fixes error C2504: 'IUnknown' : base class undefined
#include <gdiplus.h>
#include <GdiPlusColor.h>
#pragma comment (lib,"Dwmapi.lib") // Adds missing library, fixes error LNK2019: unresolved external symbol __imp__DwmExtendFrameIntoClientArea
#pragma comment (lib,"user32.lib")

#include "QXThemedWindow.h"

namespace {
    class QXWindowDecoratorWin32 : public QXWindowDecorator {
    public:
        using QXWindowDecorator::QXWindowDecorator;

        void
        onWidgetInit(QWidget * widget) override {
            return;
        }

        void
        onWidgetShow(QWidget * widget, QShowEvent * event) override {
            
        }

    private:
    };
}

QXWindowDecorator *
QXWindowDecorator::createInstance(QObject * parent) {
    return new QXWindowDecoratorWin32(parent);
}
