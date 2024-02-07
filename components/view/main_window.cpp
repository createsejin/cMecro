//
// Created by bae on 2024-01-31.
//
#include "main_window.h"
#include "../commander.h"
#include "../key_patterns.h"
#include "../action_operator.h"

#include <wx/splitter.h>
#include <locale>
#include <iostream>
#include <csignal>
using namespace main_window;
using namespace std;

void MainFrame::OnClose(wxCloseEvent& event) {
    commander::exit_program.store(true);
    if (!commander::into_command_mode.load())
        action_operator::break_key_pattern_threads();
    else cout << "\nMain window closing request received. Press Enter to exit." << endl;
    event.Skip();
}

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "Main Window", wxDefaultPosition, wxSize(800, 600))
{
    this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose);
    auto* splitter = new wxSplitterWindow(this, wxID_ANY);
    auto* topPanel = new wxPanel(splitter, wxID_ANY);
    auto* console = new wxTextCtrl(splitter, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);

    console->SetBackgroundColour(*wxBLACK);
    console->SetForegroundColour(*wxWHITE);

    splitter->SplitHorizontally(topPanel, console);
    splitter->SetSashPosition(370);
}

std::vector<char*> convertArgs(const wxCmdLineArgsArray& argv) {
    std::vector<char*> arg_c_vec;
        // wxCmdLineArgsArray로부터 arguments를 얻는다.
    for (const auto& args = argv.GetArguments();
        const auto & arg : args) {
        // 얻은 arg를 string으로 만든다.
        auto arg_str = std::string(arg.c_str().AsChar());
        // char*를 arg_str.length() + 1 만큼 할당한다.
        auto* arg_c = new char[arg_str.length() + 1]; // dynamic allocation
        // arg_str를 char*형으로 만든뒤 그것을 동적 할당한 arg_c에 복사한다.
        std::strcpy(arg_c, arg_str.c_str());
        // 복사된 arg_c를 arg_c_vec에 push_back한다.
        arg_c_vec.push_back(arg_c);
    } // char*를 담은 vector를 반환한다.
    return arg_c_vec;
}

void MainApp::run_commander() {
    // Convert wxCmdLineArgsArray to char**
    auto arg_c_vec { convertArgs(argv) };
    // run commander
    // commander_future_mutex lock을 획득하고
    unique_lock lock(commander::commander_future_mutex);
    // commander_future를 비동기로 실행한다.
    commander::commander_future = std::async(std::launch::async,
        // 콜백 함수 commander::commander를 실행하고, 매개변수를 넘긴다.
        // 이때 arg_c_vec.data()를 통해 char* 의 포인터인 char**를 매개변수로 넘겨준다.
        commander::commander, argc, arg_c_vec.data()); // using argv_c
    lock.unlock(); // commander_future_mutex lock을 해제한다.
    for (const auto* arg : arg_c_vec) {
        // arg_c_vec에 담긴 동적 할당된 char*를 메모리 해제한다.
        delete[] arg; // deallocation
    }
}

void signalHandler(const int signum) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    action_operator::break_key_pattern_threads();
    MainApp::GetInstance()->ExitMainLoop();
    std::cout << "resources released.\n";
    // Terminate program
    exit(signum);
}

void MainApp::initialize_resources() const {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // set locale to UTF-8, and set wcout to use it
    std::locale::global(std::locale("en_US.utf8"));
    std::wcout.imbue(std::locale());
    // set debug option
    commander::set_debug_option_from_command_line(argc, argv);
    // test initialize key patterns
    key_patterns::test003();
}

bool MainApp::OnInit() { // 이제 이게 main() 함수 역할, 리소스 초기화 역할을 한다.
    cout << "program main entry point" << endl;
    // initialize resources
    initialize_resources();
    // run commander
    run_commander();
    // main frame show
    auto* frame = new MainFrame();
    frame->Show(true);
    return true;
}

int MainApp::OnExit() {
    auto* frame = dynamic_cast<MainFrame*>(GetTopWindow());
    if (frame) {
        frame->Close(true);
    }
    return 0;
}

MainApp* MainApp::GetInstance() {
    return dynamic_cast<MainApp*>(wxApp::GetInstance());
}
