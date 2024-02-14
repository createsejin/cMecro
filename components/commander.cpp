//
// Created by creat on 2023-12-27.
//
#include "sql_executive.h"
#include "commander.h"
#include "action_operator.h"
#include "keyboard_hooker.h"
#include "mouse_hooker.h"
#include "key_data.h"
#include "timer.h"

#include <sstream>

using namespace std;

namespace commander {

    bool start_up {true};
    bool use_mouse_hooker {true};
    bool use_gui {true};

    void set_debug_option(const std::string_view arg, const std::string_view option_name,
            bool& debug_option, bool& valid_option) {
        if (arg.find(option_name) == 0) {
            if (const std::string_view value = arg.substr(option_name.length());
                value == "true" || value == "false") {
                debug_option = value == "true";
                valid_option = true;
            } else {
                std::cerr << "Invalid value for " << option_name << ". Expected 'true' or 'false', got '"
                          << value << "'. Using default value." << std::endl;
                valid_option = true;
            }
        }
    }

    void set_atomic_debug_option(const std::string_view arg, const std::string_view option_name,
            std::atomic<bool>& debug_option, bool& valid_option) {
        if (arg.find(option_name) == 0) {
            if (const std::string_view value = arg.substr(option_name.length());
                value == "true" || value == "false") {
                debug_option.store(value == "true");
                valid_option = true;
            } else {
                std::cerr << "Invalid value for " << option_name << ". Expected 'true' or 'false', got '"
                          << value << "'. Using default value." << std::endl;
                valid_option = true;
            }
        }
    }

    void set_debug_option_from_command_line(const int argc, char* argv[]) {
        // Default debug values
        bool debug_keyboard {false};
        bool debug_mouse {false};
        // 16진수 raw input을 출력하는 옵션인데, 이 옵션은 오로지
        // debug_keyboard나 debug_mouse가 true일때만 작동한다.
        bool debug_raw_keyboard {false};
        bool debug_raw_mouse {false};

        // Parse command line arguments
        // useage:
        // .\gta_c2.exe --keyboard_hooker_debug=true --mouse_hooker_debug=false --debug_raw_keyboard=true
        // 첫번째 option 인자부터 순회하기 시작한다. 만약 option 인자가 없으면 아래 반복문이 false로 되어 실행되지 않는다.
        for (int i {1}; i < argc; ++i) {
            const std::string_view arg = argv[i];
            bool valid_option {false};
            set_debug_option(arg, "--keyboard_hooker_debug=", debug_keyboard, valid_option);
            set_debug_option(arg, "--mouse_hooker_debug=", debug_mouse, valid_option);
            set_debug_option(arg, "--debug_raw_keyboard=", debug_raw_keyboard, valid_option);
            set_debug_option(arg, "--debug_raw_mouse=", debug_raw_mouse, valid_option);
            set_debug_option(arg, "--use_mouse_hooker=", use_mouse_hooker, valid_option);
            set_atomic_debug_option(arg, "--deep_debug_pattern_matcher=",
                deep_debug_pattern_matcher, valid_option);
            set_atomic_debug_option(arg, "--command_mode=", command_mode, valid_option);
            //set_debug_option(arg, "--use_console=", use_console, valid_option);
            set_debug_option(arg, "--use_gui=", use_gui, valid_option);
            if (!valid_option) {
                std::cerr << "Unknown command line argument: " << arg << std::endl;
            }
        }

        // Set the debug values
        key_data::KeyboardData::set_debug_keyboard(debug_keyboard);
        if (key_data::KeyboardData::get_debug_keyboard()) {
            key_data::KeyboardData::set_debug_raw_keyboard(debug_raw_keyboard);
        } else if (!debug_keyboard && debug_raw_keyboard) {
            std::cout << "Keyboard hooker debug option is false. "
                         "Raw keyboard debug option is ignored." << std::endl;
        }
        key_data::MouseData::set_debug_mouse(debug_mouse);
        if (key_data::MouseData::get_debug_mouse()) {
            key_data::MouseData::set_debug_raw_mouse(debug_raw_mouse);
        } else if (!debug_mouse && debug_raw_mouse) {
            std::cout << "Mouse hooker debug option is false. "
                         "Raw mouse debug option is ignored." << std::endl;
        }
    }

    void joinThreads(std::thread& eventLoopThread_keyboard, std::thread& eventLoopThread_mouse) {
        if (eventLoopThread_keyboard.joinable()) {
            eventLoopThread_keyboard.join();
        }
        if (eventLoopThread_mouse.joinable()) {
            eventLoopThread_mouse.join();
        }
    }

    auto get_args_from_input(const std::string_view prompt) -> std::vector<std::string> {
        std::string input;
        std::getline(std::cin, input);
        const auto pos = input.find(prompt);
        if (pos != std::string::npos) {
            input.erase(pos, 5);
        }
        ranges::transform(input.begin(), input.end(), input.begin(),
            [](const unsigned char c) {
            return std::tolower(c);
        });
        std::istringstream iss(input);
        std::vector<std::string> args;
        std::string arg;
        while (std::getline(iss, arg, ' ')) {
            args.push_back(arg);
        }
        return args;
    }

    void command_operator() {
        const std::string prompt {"cmd> "};
        std::cout << prompt; // command mode prompt

        const auto args = std::move(get_args_from_input(prompt));
        std::string first_command;
        if (!args.empty()) first_command = args[0];
        bool valid_command {false};

        if (first_command == "exit") {
            exit_program.store(true);
            exit_from_cmd.store(true);
            action_operator::exit_program_action();
            /* 프로그램이 종료되는 시점
            CLion에서 stop 버튼을 눌러 종료할때(핸들링하기 가장 어려움)
            Ctrl+C로 종료할때(console mode가 On일때만 가능) -> signalHandler() call
            일반적인 종료는 다음과 같다. -> 공통 호출: MainFrame::OnClose()
            커맨드 모드 진입 후 exit를 입력했을때
            커맨드 모드 진입 후 GUI X버튼을 누르고 커맨드 라인에 엔터를 눌렀을때
            그리고 user 입장에서는 다음과 같이 두 가지로 종료할 수 있다. (Console mode Off)
            그냥 GUI에서 X버튼으로 눌러 닫을때
            F1+F4로 종료할때 */
        }
        else if (first_command == "testdb003") {
            sql_executive::sql_manager->testdb003();
            valid_command = true;
        }
        else if (first_command == "insert") {
            if (args[1] == "key_code") {
                sql_executive::sql_manager->insert_key_code();
                valid_command = true;
            }
            else if (args[1] == "csv") {
                if (args[2] == "menus") {

                    valid_command = true;
                }
            }
        }
        if (!valid_command && !exit_program.load()) {
            cout << "Unknown command: ";
            for (const auto& arg : args) {
                cout << arg << " ";
            }
            cout << endl;
        }
    }

    void commander(const int argc, char* argv[]) {
        // keyboard_hooker와 mouse_hooker의 이벤트 루프를 실행할 스레드변수의 선언
        thread eventLoopThread_keyboard;
        thread eventLoopThread_mouse;
        bool start_command_mode {true};
        while(true) { // command loop
            if (start_up) {
                // Program을 시작하고 처음에만 실행되는 코드다.
                cout << "Program start" << endl;
                start_up = false;

                if (!command_mode.load()) {
                    /* 마우스와 키보드의 이벤트를 감지하는 이벤트 루프를 각각의 스레드에서 실행한다.
                    * 처음에 선언된 스레드 변수의 참조를 받고, 함수 내부에서 임시 스레드로 keyboard_hooker와 mouse_hooker의
                    * 이벤트 루프를 실행한 후, 임시 스레드를 미리 선언된 스레드 변수에 move시킨다.
                    * 이렇게 하는 이유는 스레드 변수를 모든 하위 블록에서 사용을 가능케 하기 위함이다.
                    */
                    std::thread eventLoop_tempThread_keyboard(keyboard_hooker::runEventLoop_keyboard);
                    if (use_mouse_hooker) {
                        std::thread eventLoop_tempThread_mouse(mouse_hooker::runEventLoop_mouse);
                        eventLoopThread_mouse = std::move(eventLoop_tempThread_mouse);
                    }
                    eventLoopThread_keyboard = std::move(eventLoop_tempThread_keyboard);
                } else {
                    into_command_mode.store(true);
                }
            }
            // Wait for the event loop threads to finish
            if (eventLoopThread_keyboard.joinable()) {
                eventLoopThread_keyboard.join();
                cout << "keyboard hooker event loop breaked." << endl;
            }
            if (use_mouse_hooker && eventLoopThread_mouse.joinable()) {
                eventLoopThread_mouse.join();
                cout << "mouse hooker event loop breaked." << endl;
            }
            // 프로그램 종료가 확정되면 commander loop를 break하고 프로그램을 종료한다.
            if (exit_program.load()) break;
            // command mode 진입시
            if (into_command_mode.load() && start_command_mode) {
                cout << "command mode" << endl;
                start_command_mode = false;
            }
            command_operator();
        }
    }
}