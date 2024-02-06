//
// Created by bae on 2023-12-29.
//
#ifndef KEYDATA_H
#define KEYDATA_H
#include "key_patterns.h"
#include <unordered_set>
#include <windows.h>
#include <memory>
#include <bitset>
#include <mutex>
#include <queue>
#include <chrono>

namespace key_data {

    // Keyboard와 Mouse의 스레드들이 서로 공유하는 공유 객체다.
    // 특수키가 눌렸는지의 여부 [0]번 비트는 Enter키, [1]번 비트는 Mouse_X2키의 눌림 여부를 나타낸다.
    inline std::bitset<2> blocking_key_state{};
    inline std::mutex blocking_key_state_mtx;
    inline std::atomic is_proceed_blocking_key_process {false};
    // target title을 저장하는 변수다.
    inline const std::wstring target_title(L"메모장");
    // 현재 window title이 target title인지의 여부를 나타낸다.
    inline std::atomic<bool> is_target_window_title{};
    // Function to check the title of the currently active window
    void checkWindowTitle();

    enum class PrintType {
        HOLD, PRESS, COMBINE, RELEASE
    };

    bool exit_method_by_filters(DWORD vkCode);

    class KeyboardData {
        // keyboard 및 mouse hooker의 debug option
        static bool debug_keyboard;
        // 16진수 raw input을 출력하는 옵션, 이 옵션은 오로지
        // debug_keyboard가 true일때만 유효하다.
        static bool debug_raw_keyboard;
        // 키보드의 모든 키의 눌림 상태를 저장하는 bitset이다.
        std::bitset<223> key_state;

        std::queue<std::bitset<223>> key_bit_queue;

        key_patterns::KeyCode blocking_key;
        std::bitset<223> blocking_key_bit{};

        DWORD current_thread_id{0};

        // 생성자를 private으로 선언하여 외부에서 객체를 생성하는 것을 막는다.
        KeyboardData();
        ~KeyboardData() = default;
    public:
        // 싱글톤 객체의 참조를 반환하는 함수다.
        static KeyboardData& getInstance();

        // 복사 생성자와 대입 연산자를 삭제한다.
        KeyboardData(const KeyboardData&) = delete;
        KeyboardData(KeyboardData&&) = delete;
        KeyboardData& operator=(const KeyboardData&) = delete;
        KeyboardData& operator=(KeyboardData&&) = delete;

        // keyboard의 debug option getter and setter
        // getter는 참조 타입을 반환한다.
        [[nodiscard]] static auto get_debug_keyboard() -> bool;
        static void set_debug_keyboard(bool value);
        [[nodiscard]] static auto get_debug_raw_keyboard() -> bool;
        static void set_debug_raw_keyboard(bool value);
        // key_state
        auto get_key_state() -> std::bitset<223>&;
        auto get_key_bit_queue() -> std::queue<std::bitset<223>>&;
        // 눌린 키의 key structure를 key_bit_queue에 push 한다.
        void push_to_key_bit_queue(const KBDLLHOOKSTRUCT& key_struct);
        // key_state를 출력한다.
        static void print_pressed_keys();
        [[nodiscard]] auto get_blocking_key() const -> key_patterns::KeyCode;
        [[nodiscard]] auto get_blocking_key_bit() const -> std::bitset<223>;
        void set_current_thread_id(DWORD thread_id);
        [[nodiscard]] auto get_current_thread_id() const -> DWORD;
    };

    void print_key_bit(const std::bitset<223>& key_bit, PrintType print_type);

    class MouseData {
        static bool debug_mouse;
        static bool debug_raw_mouse;
        DWORD current_thread_id{0};

        std::unordered_set<int> pressedMouseButtons;
        std::unordered_set<int> releasedMouseButtons;

        key_patterns::KeyCode blocking_key;

        MouseData();
        ~MouseData() = default;
    public:
        static MouseData& getInstance();
        MouseData(const MouseData&) = delete;
        MouseData(MouseData&&) = delete;
        MouseData& operator=(const MouseData&) = delete;
        MouseData& operator=(MouseData&&) = delete;

        [[nodiscard]] static auto get_debug_mouse() -> bool;
        static void set_debug_mouse(bool value);
        static void set_debug_raw_mouse(bool value);

        void set_current_thread_id(DWORD thread_id);
        [[nodiscard]] auto get_current_thread_id() const -> DWORD;

        std::unordered_set<int>& get_pressedMouseButtons();
        std::unordered_set<int>& get_releasedMouseButtons();

        static void debug_mouse_print(WPARAM wParam, const std::unordered_set<int>& key_set);

        [[nodiscard]] auto get_blocking_key() const -> key_patterns::KeyCode;
    };
} // key_data

#endif //KEYDATA_H
