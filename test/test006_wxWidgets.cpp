//
// Created by creat on 2024-01-31.
//
#include "test006_wxWidgets.h"
#include <wx/splitter.h>
using namespace test_wxWidgets;

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "Main Window", wxDefaultPosition, wxSize(800, 600))
{
    auto* splitter = new wxSplitterWindow(this, wxID_ANY);
    auto* topPanel = new wxPanel(splitter, wxID_ANY);
    auto* console = new wxTextCtrl(splitter, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);

    console->SetBackgroundColour(*wxBLACK);
    console->SetForegroundColour(*wxWHITE);

    splitter->SplitHorizontally(topPanel, console);
}

bool MyApp::OnInit() {
    auto* frame = new MainFrame();
    frame->Show(true);
    return true;
}

/*void test_wxWidgets::test000() {
    const auto* app = new MyApp();
    wxEntryStart(nullptr, nullptr);
    wxTheApp->CallOnInit();
    wxTheApp->OnRun();
    delete app;
    wxEntryCleanup();
}*/


