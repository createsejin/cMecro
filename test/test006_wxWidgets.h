//
// Created by creat on 2024-01-31.
//

#ifndef TEST006_WXWIDGETS_H
#define TEST006_WXWIDGETS_H
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include <wx/app.h>

namespace test_wxWidgets
{
    class MainFrame final : public wxFrame {
    public:
        MainFrame();
    };

    class MyApp final : public wxApp {
    public:
        bool OnInit() override;
    };
    //void test000();
}

#endif //TEST006_WXWIDGETS_H
