//
// Created by creat on 2023-12-27.
//
#include "view/main_window.h"
#include "action_operator.h"
#include "actions.h"
#include "commander.h"
#include "timer.h"
#include "key_data.h"
#include <iostream>
#include <functional>
#include <map>
#include <ranges>

using namespace std;

namespace action_operator {
    std::chrono::milliseconds keyboard_duration {15};

    inline void break_keyboard_and_mouse_eventloop() {
        // 현재 스레드에 WM_QUIT 메시지를 보내 keyboard_hooker event loop를 종료
        const auto keyboard_thread_id = key_data::KeyboardData::getInstance().get_current_thread_id();
        const auto mouse_thread_id = key_data::MouseData::getInstance().get_current_thread_id();
        if (keyboard_thread_id != 0)
            PostThreadMessage(keyboard_thread_id, WM_QUIT, 0, 0);
        if (mouse_thread_id != 0)
            PostThreadMessage(mouse_thread_id, WM_QUIT, 0, 0);
    }

    inline void command_mode_action() {
        commander::into_command_mode.store(true);
        timer::Timer::getInstance().start_key_release_timer();
        timer::Timer::getInstance().start_key_press_timer();
        timer::Timer::getInstance().join_key_release_timer_thread();
        timer::Timer::getInstance().join_key_press_timer_thread();
        break_keyboard_and_mouse_eventloop();
    }

    inline void break_key_pattern_threads() {
        command_mode_action();
        if (lock_guard lock(commander::commander_future_mutex);
            commander::commander_future.valid()) {
            commander::commander_future.wait();
        }
    }

    inline void exit_program_action() {
        main_window::MainApp::GetInstance()->ExitMainLoop();
        std::cout << "Program exit" << std::endl;
    }

    std::vector<function<void()>> single_actions {
        [] { cout << "empty action" << endl; }, // 0: Empty action
        exit_program_action, // 1: Exit action
        command_mode_action, // 2: into command mode action
        [] { // 3: up key action
            cout << "up key press" << endl;
            std::this_thread::sleep_for(keyboard_duration);
            cout << "up key release" << endl;
            std::this_thread::sleep_for(keyboard_duration);
        }
    };

    using enum Actions;
    static map<Actions, function<void()>> action_function_map {
        {XC, []() { cout << "XC action" << endl; }},
        {XC_BLOCK_TOGGLE, []() { cout << "XC_BLOCK_TOGGLE action" << endl; }},
        {XC_HOLD, []() { cout << "XC_HOLD action" << endl; }},
        {XC_V, []() { cout << "XC_V action" << endl; }},
        {XC_V_BLOCK, []() { cout << "XC_V_BLOCK action" << endl; }},
        // duplicate action
        {XC_V_HOLD, []() { cout << "XC_V_HOLD action" << endl; }},
        {XC_V_B_HOLD, []() { cout << "XC_V_B_HOLD action" << endl; }},
        {XC_V_B, []() { cout << "XC_V_B action" << endl; }},
        {XC_B_HOLD, []() { cout << "XC_B_HOLD action" << endl; }},
        //{X, []() { cout << "X action" << endl; }},
        {X_TOGGLE, []() { cout << "X_TOGGLE action" << endl; }},
        {X_BLOCK_TOGGLE, []() { cout << "X_BLOCK_TOGGLE action" << endl; }},
        {X_V_BLOCK, []() { cout << "X_V_BLOCK action" << endl; }},
        {X_V_HOLD, []() { cout << "X_V_HOLD action" << endl; }},
        {V_X, []() { cout << "V_X action" << endl; }},
        // duplicate action: map은 중복된 요소를 덮어쓰므로 컴파일 에러가 나지 않는다.
        // 나중에 SQL로부터 만들때 중복되는지를 SQL에서 체크하던지, 아니면 여기에 넣을때 체크하면 된다.
        //{XC_V_HOLD, []() { cout << "XC_V_HOLD action" << endl; }},
        // V missing
        {V, []() { cout << "V action" << endl; }},
        {V_HOLD, []() { cout << "V_HOLD action" << endl; }},
        {V_BLOCK_HOLD, []() { cout << "V_BLOCK_HOLD action" << endl; }},

        {PROGRAM_EXIT, exit_program_action},
        {COMMAND_MODE, command_mode_action},
    };

    ActionOperator::ActionOperator() = default;
    ActionOperator::~ActionOperator() = default;

    ActionOperator& ActionOperator::getInstance() {
        static ActionOperator action_operator;
        return action_operator;
    }

    // key_patterns 관점에서 function_map
    void ActionOperator::find_missing_function_map_actions_in_key_patterns() {
        key_patterns::PatternManager& pattern_manager = key_patterns::PatternManager::getInstance();
        auto& key_patterns = pattern_manager.get_key_patterns();
        ranges::for_each(key_patterns.begin(), key_patterns.end(),
            [](key_patterns::KeyPattern& key_pattern) {
                if (!action_function_map.contains(key_pattern.get_action())) {
                    std::cout << "missing action of action_function_map in key_patterns: "
                        << key_pattern.get_action_name() << std::endl;
                }
        });
    }

    void ActionOperator::find_missing_key_patterns_actions_in_function_map() {
        key_patterns::PatternManager& pattern_manager = key_patterns::PatternManager::getInstance();
        const auto& key_patterns = pattern_manager.get_key_patterns();

        std::unordered_set<Actions> key_patterns_action_set;
        for (const auto& key_pattern : key_patterns) {
            key_patterns_action_set.insert(key_pattern.get_action());
        }

        for (const auto& action : action_function_map | views::keys) {
            if (!key_patterns_action_set.contains(action)) {
                cout << "missing key_pattern in action_function_map: "
                    << static_cast<unsigned>(action) << endl;
            }
        }
    }

    void ActionOperator::find_missing_function_map_actions_in_Actions() {
        for (unsigned i {0}; i < static_cast<unsigned>(MAX); ++i) {
            if (!action_function_map.contains(static_cast<Actions>(i))) {
                cout << "missing action of action_function_map in Actions: "
                    << i << endl;
            }
        }
    }

    void ActionOperator::find_missing_key_patterns_actions_in_Actions() {
        key_patterns::PatternManager& pattern_manager = key_patterns::PatternManager::getInstance();
        const auto& key_patterns = pattern_manager.get_key_patterns();

        // key_patterns의 모든 action을 set에 저장
        std::unordered_set<Actions> actions_set;
        for (const auto& key_pattern : key_patterns) {
            actions_set.insert(key_pattern.get_action());
        }

        // Actions 열거체를 순회하면서 각 action이 set에 존재하는지 확인
        for (int i = 0; i < static_cast<int>(MAX); ++i) {
            auto action = static_cast<Actions>(i);
            if (!actions_set.contains(action)) {
                std::cout << "missing action in key_patterns in Actions: " << i << std::endl;
            }
        }
    }

    void ActionOperator::verify_action_function_map() {
        // key_patterns 관점에서 function_map
        find_missing_function_map_actions_in_key_patterns();
        // function_map 관점에서 key_patterns
        find_missing_key_patterns_actions_in_function_map();
        // Actions 관점에서 function_map
        find_missing_function_map_actions_in_Actions();
        // Actions 관점에서 key_patterns
        find_missing_key_patterns_actions_in_Actions();
    }

    void ActionOperator::toggling() {
        const auto& key_pattern = execute_key_pattern_opt.value();
        if (key_pattern.get_pattern_type() == key_patterns::PatternType::TOGGLE) {
            const auto& toggle_key_patterns = key_patterns::PatternManager::getInstance().get_toggle_key_patterns();
            // 여기서 it iterator는 toggle_key_patterns에 담긴 key_pattern 객체의 포인터의 포인터다.
            for (const auto& key_pattern_ptr : toggle_key_patterns) {
                if (*key_pattern_ptr == key_pattern) {
                    key_pattern_ptr->flip_toggle();
                    break;
                }
            }
        }
    }

    void ActionOperator::frame_of_action() {
        if (!execute_key_pattern_opt.has_value()) return;
        // check and execute toggle action
        toggling();
        if (commander::deep_debug_pattern_matcher.load()) {
            cout << "-------------------execute action----------------------" << endl;
            cout << "execute action: " << execute_key_pattern_opt.value().get_action_name() << endl;
        }
        action_function_map[execute_key_pattern_opt.value().get_action()]();
        cout << "executed action: " << execute_key_pattern_opt.value().get_action_name() << " \u25A0" << endl;
        execute_key_pattern_opt.reset();
    }

    void ActionOperator::operate() {
        if (!execute_key_pattern_opt.has_value()) return;
        //auto& key_pattern = execute_key_pattern_opt.value();
        //key_patterns::PatternManager::print_key_pattern(key_pattern);
        // operate에 의한 블로킹 혹은 SendInput에 대한 interruption, 실행 시간에 의한 블로킹을 방지하기 위해
        // 별도의 스레드에서 명령을 실행한다.
        // 명령이 실행되는 동안에는 다른 패턴키는 무시되도록 해야한다.
        std::thread action_thread(&ActionOperator::frame_of_action);
        // action_thread를 메인 스레드로부터 분리한다.
        action_thread.detach();
    }

    void ActionOperator::set_execute_key_pattern_opt(key_patterns::KeyPattern& key_pattern) {
        execute_key_pattern_opt = key_pattern;
    }
    auto ActionOperator::get_execute_key_pattern_opt() -> std::optional<key_patterns::KeyPattern>& {
        return execute_key_pattern_opt;
    }
}
