//
// Created by creat on 2023-12-27.
//
#include "mouse_hooker.h"
#include "key_patterns.h"
#include "key_data.h"
#include <format>
using namespace std;
using namespace key_data;
using enum key_patterns::KeyCode;

namespace mouse_hooker {
    void update_blocking_key_state(const int transed_wParam, const bool is_press = false)
    { std::lock_guard lock(blocking_key_state_mtx);
        const MouseData& mouse_data = MouseData::getInstance();
        if (transed_wParam == static_cast<int>(mouse_data.get_blocking_key())) {
            if (is_press) {
                blocking_key_state.set(1);
                // 만약 특수키가 하나라도 눌려지면 is_proceed_blocking_key_process를 true로 설정한다.
                if (blocking_key_state.any()) is_proceed_blocking_key_process.store(true);
                // 특수키가 눌리거나 떼어졌을때 current_key_patterns_vec를 어떤것으로 쓸지 결정한다.
                key_patterns::PatternMatcher::getInstance().decide_current_key_patterns_vec();
            } else {
                blocking_key_state.reset(1);
            }
        }
    }
    void handleMousePress(const WPARAM wParam, const int transed_wParam) {
        MouseData& mouse_data = MouseData::getInstance();
        // button이 press 되면 releasedMouseButtons를 clear 한다.
        mouse_data.get_releasedMouseButtons().clear();

        // If the button is not in the pressedMouseButtons set, add it
        if (!mouse_data.get_pressedMouseButtons().contains(transed_wParam)) {
            mouse_data.get_pressedMouseButtons().insert(transed_wParam);
            update_blocking_key_state(transed_wParam, true);
            MouseData::debug_mouse_print(wParam, mouse_data.get_pressedMouseButtons());
        }
        // 만약 wheel이 움직이면 앞서 적잘한 operation을 수행한 뒤,
        // pressedMouseButtons set에서 해당 값을 erase하여 set을 clean up 한다.
        if (wParam == WM_MOUSEWHEEL && mouse_data.get_pressedMouseButtons()
            .contains(transed_wParam))
            mouse_data.get_pressedMouseButtons().erase(transed_wParam);
    }

    void handleMouseRelease(const WPARAM wParam, int& transed_wParam, const int xbutton_plus) {
        MouseData& mouse_data = MouseData::getInstance();
        transed_wParam = static_cast<int>(wParam) + xbutton_plus -1;
        if (!mouse_data.get_releasedMouseButtons().contains(transed_wParam)) {
            mouse_data.get_releasedMouseButtons().insert(transed_wParam);
            update_blocking_key_state(transed_wParam);
            MouseData::debug_mouse_print(wParam, mouse_data.get_pressedMouseButtons());
        }
        if (mouse_data.get_pressedMouseButtons().contains(transed_wParam)) {
            mouse_data.get_pressedMouseButtons().erase(transed_wParam);
        }
    }
    // 이 함수는 마우스가 움직일때에도 계속 호출되므로 최적화를 위해
    // 마우스가 움직일 때에는 함수 호출이 무효로 처리되도록 했다.
    LRESULT CALLBACK LowLevelMouseProc(const int nCode, const WPARAM wParam, const LPARAM lParam) {
        if (wParam != WM_MOUSEMOVE) // 마우스가 움직일때마다 이 함수가 호출되므로 최적화를 위해
            checkWindowTitle(); // 마우스가 움직일때에는 window check를 실행하지 않는다.
        else return CallNextHookEx(nullptr, nCode, wParam, lParam);
        if (nCode == HC_ACTION && is_target_window_title) {
            // lParam의 포인터를 통해 mouseData에 접근한다.
            const auto* p {reinterpret_cast<MSLLHOOKSTRUCT *>(lParam)};
            // 비트 연산을 통해 mouseData의 상위 16비트의 정수 변환값을 얻는다.
            const auto mouse_delta {static_cast<short>(p->mouseData >> 16)};
            // xbutton이 눌리거나 떼어졌을 때의 변환된 wParam값인 transed_wParam값에 더해지는 값이다.
            const int xbutton_plus {wParam == WM_XBUTTONDOWN || wParam == WM_XBUTTONUP ? mouse_delta : 0};
            // wParam값과 xbutton_plus값을 계산한 뒤 transed_wParam에 저장한다. 이 값은 이제
            // pressedMouseButtons set에 insert되거나 erase되는 값이 된다.
            int transed_wParam {static_cast<int>(wParam) + xbutton_plus};

            // wParam값에 따른 case 문이다. wheel을 제외하고는 마우스 버튼이 눌리거나 떼어졌을때의 동작을 나타낸다.
            switch (wParam) {
                // 마우스 휠은 움직인 수 만큼 이 함수가 호출된다.
                case WM_MOUSEWHEEL:
                    /* mouse wheel이 위로 움직이면 mouse_delta는 120이고, 아래로 움직이면 -120이다.
                     * 이 값을 다시 적절한 변환값(0, +1)으로 바꾼 뒤에
                     * pressedMouseButtons에 insert한다.*/
                    if (mouse_delta == 120) transed_wParam = WM_MOUSEWHEEL;
                    else if (mouse_delta == -120) transed_wParam = WM_MOUSEWHEEL + 1;
                case WM_LBUTTONDOWN:
                case WM_RBUTTONDOWN:
                case WM_MBUTTONDOWN:
                case WM_XBUTTONDOWN:
                    handleMousePress(wParam, transed_wParam);
                    break;
                case WM_LBUTTONUP:
                case WM_RBUTTONUP:
                case WM_MBUTTONUP:
                case WM_XBUTTONUP:
                    handleMouseRelease(wParam, transed_wParam, xbutton_plus);
                    break;
                default: break;
            }
        }
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    void runEventLoop_mouse() {
        HHOOK const& mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc,
            GetModuleHandle(nullptr), 0);
        if (mouseHook == nullptr) {
            cerr << "Failed to install mouse hook." << endl;
            return;
        }
        MouseData::getInstance().set_current_thread_id(GetCurrentThreadId());
        // if (MouseData::get_debug_mouse())
        //     cout << "current mouse thread id: " << mouse_data.get_current_thread_id() << endl;

        // Message loop to keep the program running
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        UnhookWindowsHookEx(mouseHook);
    }
}