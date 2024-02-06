//
// Created by creat on 2023-12-27.
//
#include "keyboard_hooker.h" // <queue>
#include "key_patterns.h" // <unordered_set> <windows.h> <vector> <map> <string> <iostream>
#include "key_data.h" // <unordered_set> <queue> <windows.h>
#include <format>
#include <numeric>

#include "timer.h"
using namespace std;
using namespace key_data;
using enum key_patterns::KeyCode;

namespace keyboard_hooker {
    void update_blocking_key_state(const WPARAM wParam, const KBDLLHOOKSTRUCT* p) {
        // 공유 객체에 대한 lock을 얻는다. lock은 함수가 종료되면 자동으로 해제된다.
        std::lock_guard lock(blocking_key_state_mtx);
        const KeyboardData& keyboard_data = KeyboardData::getInstance();
        if (p->vkCode == static_cast<DWORD>(keyboard_data.get_blocking_key())
            && p->flags & LLKHF_EXTENDED) {
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                blocking_key_state.set(0);
                // 만약 특수키가 하나라도 눌려지면 is_proceed_blocking_key_process를 true로 설정한다.
                if (blocking_key_state.any()) is_proceed_blocking_key_process.store(true);
                // 특수키가 눌리거나 떼어졌을때 current_key_patterns_vec를 어떤것으로 쓸지 결정한다.
                key_patterns::PatternMatcher::getInstance().decide_current_key_patterns_vec();
            }
            else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
            { blocking_key_state.reset(0); }
        }
    }
    bool check_blocking_key_state(const KBDLLHOOKSTRUCT* p) {
        key_patterns::KeyCodeManager& key_code_manager = key_patterns::KeyCodeManager::getInstance();
        if (std::lock_guard lock(blocking_key_state_mtx);
            blocking_key_state.any() && !key_code_manager.get_unblocking_key_bit().test(p->vkCode)) {
            return true;
        }
        return false;
    }

    std::chrono::steady_clock::time_point c_press;
    std::chrono::steady_clock::time_point c_release;
    std::vector<std::chrono::milliseconds> durations_vec;
    void get_average_of_duration(const std::chrono::milliseconds& duration) {
        durations_vec.push_back(duration);
        const double average {
            static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>
                (std::accumulate(durations_vec.begin(), durations_vec.end(),
                    std::chrono::milliseconds{0})).count())
                / static_cast<double>(durations_vec.size())
        };
        cout << "average = " << average << "ms" << endl;
    }
    void handleKeyPress(const WPARAM wParam, const KBDLLHOOKSTRUCT* p) {
        // 객체 인스턴스를 얻는다.
        KeyboardData& keyboard_data = KeyboardData::getInstance();
        // 만약 눌린 키의 key_state가 1이 아니라면
        if (!keyboard_data.get_key_state().test(p->vkCode)) {// 이것은 키의 중복 입력을 방지한다.
            // 눌린 키의 key_state를 1로 설정하고
            keyboard_data.get_key_state().set(p->vkCode);
            update_blocking_key_state(wParam, p);
            // 눌린 키의 key_struck을 key_bit_queue에 push 한다.
            keyboard_data.push_to_key_bit_queue(*p);
        }
    }

    void check_special_key_relase(const DWORD vkCode) {
        const KeyboardData& keyboard_data = KeyboardData::getInstance();
        if (vkCode == static_cast<DWORD>(keyboard_data.get_blocking_key())) {
            //if (keyboard_data.get_key_bit_queue().empty()) return;
            timer::Timer::getInstance().set_last_release_time_point();
            timer::Timer::getInstance().start_key_release_timer();
        }
    }

    void handleKeyRelease(const WPARAM wParam, const KBDLLHOOKSTRUCT* p) {
        KeyboardData& keyboard_data = KeyboardData::getInstance();
        if (keyboard_data.get_key_state().test(p->vkCode)) {
            update_blocking_key_state(wParam, p);
            keyboard_data.get_key_state().reset(p->vkCode);
            // 단일 홀드키가 눌려지고 키가 하나라도 떼어지면 key_press_timer를 즉시 멈춘다.
            timer::Timer::getInstance().stop_key_press_timer();
            // 만약 특수키가 떼어지면 release 타이머를 동작시킨다.
            check_special_key_relase(p->vkCode);
            // 키보드가 릴리즈 될때 pattern_matcher를 호출한다.
            // 단, 지정한 이동키나 특수키가 눌려져 있을때, 혹은 현재 필터키가 아닌 경우에는 무효로 처리된다.
            // 또한 만약 현재 필터키가 눌려져있다면 그때에도 무효다. 반드시 현재 필터키가 모두 떼어져야
            // pattern_matcher가 실행된다.
            key_patterns::PatternMatcher::getInstance().pattern_matching(*p);
        }
    }

    LRESULT CALLBACK LowLevelKeyboardProc(const int nCode,const WPARAM wParam,const LPARAM lParam) {
        checkWindowTitle();
        if (nCode == HC_ACTION && is_target_window_title) {
            // 키보드 이벤트가 발생하면 lParam에 키보드 이벤트에 대한 정보가 담겨진다.
            const auto* p = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                handleKeyPress(wParam, p);
                // key blocking
                if (check_blocking_key_state(p)) return 1;
            } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
                handleKeyRelease(wParam, p);
                if (check_blocking_key_state(p)) return 1;
            }
        }
        // 정상적으로 키 이벤트를 입력으로 넘겨준다.
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    void runEventLoop_keyboard() {
        const HHOOK& keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc,
            GetModuleHandle(nullptr), 0);
        if (keyboardHook == nullptr) {
            cerr << "Failed to install keyboard hook. \u25A1" << endl;
            return;
        }
        KeyboardData::getInstance().set_current_thread_id(GetCurrentThreadId());

        // Message loop to keep the program running
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        UnhookWindowsHookEx(keyboardHook);
    }

    void test_code000() {

    }
}