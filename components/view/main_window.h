//
// Created by bae on 2024-01-31.
//
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include <wx/app.h>
#include "../../lib/CLI11.hpp"

namespace main_window
{

    class MainFrame final : public wxFrame {
        static void OnClose(wxCloseEvent& event);
    public:
        MainFrame();
    };

    class MainApp final : public wxApp {
        void run_commander();
        void initialize_resources() const;
    public:
        bool OnInit() override;
        int OnExit() override;
        static MainApp* GetInstance();
    };
}
#endif //MAIN_WINDOW_H
