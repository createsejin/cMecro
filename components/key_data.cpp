//
// Created by bae on 2023-12-29.
//
#include "key_patterns.h" // <unordered_set> <windows.h> <vector> <map> <string> <iostream>
#include "key_data.h" // <unordered_set> <queue> <windows.h>
#include "timer.h"
#include <format>
#include <ranges>

#include "commander.h"
using namespace std;

namespace key_data {

    // keyboard thread와 mouse thread가 공유하는 공유 객체다.
    std::wstring previousWindowTitle;
    std::mutex previousWindowTitle_mtx; // mutex
    void checkWindowTitle() {
        //const bool debug_option = KeyboardData::get_debug_keyboard() || MouseData::get_debug_mouse();
        // Get the handle of the currently active window
        if (HWND const& hwnd = GetForegroundWindow(); hwnd != nullptr) {
            // Get the title of the currently active window
            wchar_t windowTitle[256];
            GetWindowTextW(hwnd, windowTitle, sizeof(windowTitle));
            const std::wstring currentWindowTitle(windowTitle);

            // If the window title has changed, print the new title
            { std::lock_guard lock(previousWindowTitle_mtx); // mutex lock을 획득한다.
                // If the window title has changed, print the new title
                if (currentWindowTitle != previousWindowTitle) {
                    // 키보드나 마우스의 디버그 옵션이 켜지면 현재 window title을 출력한다.
                    // if (debug_option)
                    //     wcout << L"Active window title: " << currentWindowTitle << std::endl;
                    // Update the stored window title
                    previousWindowTitle = currentWindowTitle; } }
            // If the window title contains target title, set is_target_window_title to true
            is_target_window_title.store(
                currentWindowTitle.find(target_title) != std::wstring::npos);
        }
    }

    void print_debug_output(const std::string_view front_message,
        const std::unordered_set<int>& key_set, const bool debug_raw) {
        key_patterns::KeyCodeManager& manager = key_patterns::KeyCodeManager::getInstance();
        cout << front_message;
        int counter{0};
        const size_t size = key_set.size();
        for (const auto& key : key_set) {
            string key_name;
            if (debug_raw) {
                key_name = format("{:#X}", key);
            } else {
                key_name = manager.get_key_name(key);
            }
            if (counter == size - 1) cout << key_name;
            else cout << key_name << ", ";
            counter++;
        }
        cout << endl;
    }

    bool exit_method_by_filters(const DWORD vkCode) {
        if (commander::deep_debug_pattern_matcher.load()) {
            cout << "--------------exit_method_by_filters-------------------" << endl;
        }
        const KeyboardData& keyboard_data = KeyboardData::getInstance();
        key_patterns::KeyCodeManager& key_code_manager = key_patterns::KeyCodeManager::getInstance();
        // 만약 눌린 키가 특수키라면 즉시 함수를 종료한다.
        if (vkCode == static_cast<DWORD>(keyboard_data.get_blocking_key())) return true;
        // 또는 눌린 키가 unblocking_key_bit 키(이동키) 라면 즉시 함수를 종료한다.
        const auto& unblocking_key_bit =  key_code_manager.get_unblocking_key_bit();
        if (unblocking_key_bit.test(vkCode)) return true;
        // 또는 눌린 키가 current_key_filter에 포함되지 않는다면 즉시 함수를 종료한다.
        const auto& current_key_filter
            = key_patterns::PatternMatcher::getInstance().get_current_filter();
        if (commander::deep_debug_pattern_matcher.load()) {
            cout << "current_key_filter: " << endl;
            print_key_bit(current_key_filter, PrintType::COMBINE);
        }
        if ((~current_key_filter).test(vkCode)) return true;
        return false;
    }

    bool KeyboardData::debug_keyboard = false;
    bool KeyboardData::debug_raw_keyboard = false;

    KeyboardData::KeyboardData()
    {
        blocking_key = key_patterns::KeyCode::ENTER;
        blocking_key_bit.set(static_cast<unsigned>(blocking_key));
    }

    KeyboardData& KeyboardData::getInstance() {
        static KeyboardData instance;
        return instance;
    }

    auto KeyboardData::get_debug_keyboard() -> bool { return debug_keyboard; }
    void KeyboardData::set_debug_keyboard(const bool value) { debug_keyboard = value; }
    auto KeyboardData::get_debug_raw_keyboard() -> bool { return debug_raw_keyboard; }
    void KeyboardData::set_debug_raw_keyboard(const bool value) { debug_raw_keyboard = value; }

    std::bitset<223>& KeyboardData::get_key_state() { return key_state; }

    auto KeyboardData::get_key_bit_queue() -> std::queue<std::bitset<223>>& { return key_bit_queue; }

    void KeyboardData::push_to_key_bit_queue(const KBDLLHOOKSTRUCT& key_struct) {
        if (exit_method_by_filters(key_struct.vkCode)) {
            if (commander::deep_debug_pattern_matcher.load()) {
                cout << "exit push_to_key_bit_queue by fileters: ";
                cout << key_patterns::KeyCodeManager::getInstance().get_key_name(key_struct.vkCode) << endl;
            }
            return;
        }

        key_patterns::KeyCodeManager& key_code_manager = key_patterns::KeyCodeManager::getInstance();
        const auto& unblocking_key_bit =  key_code_manager.get_unblocking_key_bit();
        if (commander::deep_debug_pattern_matcher.load()) {
            cout << "---------------push key struck-----------------------" << endl;
            cout << "pushed key struck: " <<
                key_code_manager.get_key_name(key_struct.vkCode) << endl;
        }

        const KeyboardData& keyboard_data = getInstance();
        key_patterns::PatternMatcher& pattern_matcher = key_patterns::PatternMatcher::getInstance();
        // 각종 필터로 key_state를 필터링한다.
        const bitset filtered_key_state =
            key_state
                & ~keyboard_data.get_blocking_key_bit() // blocking 특수키는 포함하지 않는다.
                & ~unblocking_key_bit // 이동키는 포함하지 않는다.
                & pattern_matcher.get_current_filter(); // current 필터 이외의 키는 포함하지 않는다.
        timer::Timer::getInstance().set_current_key_time_point();
        // exit 필터를 통과했다면 key_press_timer를 시작한다.
        timer::Timer::getInstance().start_key_press_timer();
        if (key_bit_queue.empty()) {
            // 현재 filtered_key_state에서 unblocking_key_bit를 필터링해서 넣는다.
            key_bit_queue.push(filtered_key_state);
            timer::Timer::getInstance().set_previous_key_time_point();
        } else {
            // 두 키의 press 사이의 시간이 35ms 이하라면
            if (timer::Timer::getInstance().compare_press_key_time_point()) {
                /*unblocking key_bit를 제외한 filtered_key_state를 key_bit_queue의 back key_bit와 or 연산한다(섞는다).*/
                key_bit_queue.back() |= filtered_key_state;
                timer::Timer::getInstance().set_previous_key_time_point();
            } // 그게 아니라면
            else {
                key_bit_queue.push(filtered_key_state);
                timer::Timer::getInstance().set_previous_key_time_point();
            }
        }
        if (commander::deep_debug_pattern_matcher.load()) {
            auto key_bit_queue_copy = key_bit_queue;
            cout << "pushed key_bit_queue: " << endl;
            while (!key_bit_queue_copy.empty()) {
                print_key_bit(key_bit_queue_copy.front(), PrintType::COMBINE);
                key_bit_queue_copy.pop();
            }
        }
    }

    // 이 메소드를 공용 함수로 만들어서 재사용성을 높혀야한다.
    void print_key_bit(const std::bitset<223>& key_bit, const PrintType print_type) {
        key_patterns::KeyCodeManager& manager = key_patterns::KeyCodeManager::getInstance();
        string front_message;
        switch (print_type) {
            case PrintType::HOLD:
                front_message = "hold keys: ";
                break;
            case PrintType::PRESS:
                front_message = "pressed keys: ";
                break;
            case PrintType::COMBINE:
                front_message = "combine keys: ";
                break;
            case PrintType::RELEASE:
                front_message = "released keys: ";
                break;
            default:
                cout << "something wrong" << endl;
        }
        const std::bitset<223> empty_bitset{};
        if (key_bit == empty_bitset) front_message = "empty bitset";

        cout << front_message;
        string key_name;
        for (int i {0}; i < 223; i++) {
            if (key_bit.test(i)) {
                if (KeyboardData::get_debug_raw_keyboard()) {
                    key_name += format("{:#X}, ", i);
                } else {
                    key_name += manager.get_key_name(i) + ", ";
                }
            }
        }
        key_name = key_name.substr(0, key_name.size() - 2);
        cout << key_name << endl;
        cout << "A-Z bit: ";
        for (int i {65}; i <= 90; i++) {
            cout << key_bit[i];
        } cout << endl;
    }

    void KeyboardData::print_pressed_keys() {
        KeyboardData& keyboard_data = getInstance();
        print_key_bit(keyboard_data.get_key_state(), PrintType::PRESS);
    }

    auto KeyboardData::get_blocking_key() const -> key_patterns::KeyCode { return blocking_key; }
    auto KeyboardData::get_blocking_key_bit() const -> std::bitset<223> { return blocking_key_bit; }

    void KeyboardData::set_current_thread_id(const DWORD thread_id) {
        current_thread_id = thread_id;
    }
    auto KeyboardData::get_current_thread_id() const -> DWORD {
        return current_thread_id;
    }

    bool MouseData::debug_mouse = false;
    bool MouseData::debug_raw_mouse = false;

    MouseData::MouseData() {
        blocking_key = key_patterns::KeyCode::MOUSE_X2;
    }

    MouseData& MouseData::getInstance() {
        static MouseData instance;
        return instance;
    }

    auto MouseData::get_debug_mouse() -> bool { return debug_mouse; }
    void MouseData::set_debug_mouse(const bool value) { debug_mouse = value; }
    void MouseData::set_debug_raw_mouse(const bool value) { debug_raw_mouse = value; }

    void MouseData::set_current_thread_id(const DWORD thread_id) { current_thread_id = thread_id; }
    auto MouseData::get_current_thread_id() const -> DWORD { return current_thread_id; }

    std::unordered_set<int>& MouseData::get_pressedMouseButtons() { return pressedMouseButtons; }
    std::unordered_set<int>& MouseData::get_releasedMouseButtons() { return releasedMouseButtons; }

    void MouseData::debug_mouse_print(const WPARAM wParam, const std::unordered_set<int>& key_set) {
        if (debug_mouse) {
            std:string_view front_message;
            switch (wParam) {
                case WM_MOUSEWHEEL:
                case WM_LBUTTONDOWN:
                case WM_RBUTTONDOWN:
                case WM_MBUTTONDOWN:
                case WM_XBUTTONDOWN:
                    front_message = "Pressed mouseButtons: "sv;
                    break;
                case WM_LBUTTONUP:
                case WM_RBUTTONUP:
                case WM_MBUTTONUP:
                case WM_XBUTTONUP:
                    front_message = "Released mouseButtons: "sv;
                    break;
                default: break;
            }
            print_debug_output(front_message, key_set, debug_raw_mouse);
        }
    }

    auto MouseData::get_blocking_key() const -> key_patterns::KeyCode { return blocking_key; }
} // key_data