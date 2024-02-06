//
// Created by creat on 2023-12-27.
//
#include "commander.h"
#include "keyboard_hooker.h"
#include "mouse_hooker.h"
#include "key_data.h"

using namespace std;

namespace commander {

    bool start_up {true};
    bool use_mouse_hooker {true};

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
            //set_debug_option(arg, "--use_console=", use_console, valid_option);
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

    void commander(const int argc, char* argv[]) {
        // keyboard_hooker와 mouse_hooker의 이벤트 루프를 실행할 스레드변수의 선언
        thread eventLoopThread_keyboard;
        thread eventLoopThread_mouse;

        while(true) { // command loop
            if (start_up) {
                // Program을 시작하고 처음에만 실행되는 코드다.
                cout << "Program start" << endl;
                start_up = false;

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
            if (exit_program.load()) {
                cout << "exit command loop" << endl;
                return;
            }
            // 특정 키 조합에 의한 operation에 의해 스레드를 종료시키고 커맨드 모드로 진입한다.
            // code here //
            if(into_command_mode.load()) {
                cout << "command mode" << endl;
                into_command_mode.store(false);
            }
            std::cout << "cmd> ";
            std::string input;
            std::getline(std::cin, input); // blocking
            const auto pos = input.find("cmd>");
            if (pos != std::string::npos) {
                input.erase(pos, 5);
            }
            try {
                cli.parse(input, false);
            } catch (const CLI::ParseError& e) {
                std::cout << "Parse error: " << e.what() << std::endl;
            }
        }
    }
}