//
// Created by creat on 2023-12-27.
//

#ifndef OPERATOR_H
#define OPERATOR_H
#include "key_patterns.h"
#include <optional>
#include <functional>

namespace action_operator {
    void break_key_pattern_threads();
    void exit_program_action();
    enum class PlayerState {
        NORMAL,
        CEO, EMPLOYEE,
        MC, NEWCOMER, HENCHMAN,
    };

    enum class CurrentWeapon {
        UNARMED,
        MELEE,
        PISTOL,
        SHOTGUN,
        SMG,
        RIFLE,
        HEAVY,
        SNIPER,
        THROWABLE,
        SPECIAL,
    };

    struct GTAFlags {
        PlayerState player_state{PlayerState::NORMAL};
        CurrentWeapon current_weapon{CurrentWeapon::UNARMED};
    };

    struct MenuNode { // 상호작용 메뉴들이 담길 노드 구조체이다.
        unsigned menu_id; // root id
        std::string menu_name;
        std::vector<MenuNode*> children;
        MenuNode(unsigned menu_id, std::string_view menu_name);
        void addChildren(MenuNode* child);
    };

    class Action {
        const unsigned action_id{};
        const std::string action_name;
        std::vector<std::function<void()>*> action_list;
    public:
        Action(unsigned action_id, std::string_view action_name);
        void play_action_list() const;
        void add_action(std::function<void()>* func_ptr);
    };

    class ActionOperator {
        inline static std::optional<key_patterns::KeyPattern> execute_key_pattern_opt{};
        ActionOperator();
        ~ActionOperator();
        static void toggling();
        static void frame_of_action();
    public:
        static ActionOperator& getInstance();
        ActionOperator(const ActionOperator&) = delete;
        ActionOperator(ActionOperator&&) = delete;
        ActionOperator& operator=(const ActionOperator&) = delete;
        ActionOperator& operator=(ActionOperator&&) = delete;
    private:
        static void find_missing_function_map_actions_in_key_patterns();
        static void find_missing_key_patterns_actions_in_function_map();
        static void find_missing_function_map_actions_in_Actions();
        static void find_missing_key_patterns_actions_in_Actions();
    public:
        static void verify_action_function_map();

        static void operate();
        static void set_execute_key_pattern_opt(key_patterns::KeyPattern& key_pattern);
        static auto get_execute_key_pattern_opt() -> std::optional<key_patterns::KeyPattern>&;
    };

    void test_action_list();
}
#endif //OPERATOR_H
