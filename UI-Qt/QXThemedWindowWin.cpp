
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

        bool
        onNativeEvent(QWidget * widget, const QByteArray & eventType, void * message, long * result) override {
            return false;
#if (QT_VERSION == QT_VERSION_CHECK(5, 11, 1))
            MSG* msg = *reinterpret_cast<MSG**>(message);
#else
            MSG* msg = reinterpret_cast<MSG*>(message);
#endif

            HWND hwnd = msg->hwnd;

            switch (msg->message)
            {
            case WM_NCPAINT:
            {
                printf("WM_NCPAINT\n");
                HDC hdc;
                RECT rect;
                HRGN rgn = (HRGN)msg->wParam;
                if ((msg->wParam == 0) || (msg->wParam == 1))
                    hdc = GetWindowDC(hwnd);
                else
                    hdc = GetDCEx(hwnd, rgn, DCX_WINDOW | DCX_CACHE | DCX_LOCKWINDOWUPDATE | DCX_INTERSECTRGN);

                QColor color = widget->palette().window().color().toRgb();
                QColor fg = widget->palette().text().color().toRgb();
                HBRUSH tmp, bgBrush = CreateSolidBrush(RGB(color.red(), color.green(), color.blue()));
                
                tmp = (HBRUSH)SelectObject(hdc, bgBrush);

                Rectangle(hdc, 0, 0, widget->frameSize().width() + 40, widget->frameSize().height() + 40);
                std::string title = widget->windowTitle().toStdString();

                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, RGB(fg.red(), fg.green(), fg.blue()));
                TextOut(hdc, 10, 10, title.c_str(), title.length());
                
                SelectObject(hdc, tmp);
                
                DeleteObject(bgBrush);

                ReleaseDC(hwnd, hdc);
                *result = 0;
                return true;
            }
            }
            return false;
        }
    private:
    };
}

QXWindowDecorator *
QXWindowDecorator::createInstance(QObject * parent) {
    return new QXWindowDecoratorWin32(parent);
}
