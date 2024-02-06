//
// Created by creat on 2023-12-28.
//
#include "key_patterns.h"
#include "key_data.h"
#include "timer.h"
#include "commander.h"
#include "actions.h"
#include "action_operator.h"
#include <format>

using namespace std;
namespace key_patterns {

    using enum KeyCode;
    using enum PatternType;
    using namespace action_operator;

    // 열거형 KeyboardButtons를 int set으로 변환하는 함수다.
    unordered_set<int> make_int_keyset(const vector<KeyCode>& keys) {
        unordered_set<int> keyset;
        for (const auto& key : keys) {
            keyset.insert(static_cast<int>(key));
        }
        return keyset;
    }

    KeyCodeManager::KeyCodeManager() {
        std::map<KeyCode, std::string> keyCode_name_map_ = {
            {A, "A"}, {B, "B"}, {C, "C"}, {D, "D"}, {E, "E"},
            {F, "F"}, {G, "G"}, {H, "H"}, {I, "I"}, {J, "J"},
            {K, "K"}, {L, "L"}, {M, "M"}, {N, "N"}, {O, "O"},
            {P, "P"}, {Q, "Q"}, {R, "R"}, {S, "S"}, {T, "T"},
            {U, "U"}, {V, "V"}, {W, "W"}, {X, "X"}, {Y, "Y"},
            {Z, "Z"},
            {N1, "1"}, {N2, "2"}, {N3, "3"}, {N4, "4"},
            {N5, "5"}, {N6, "6"}, {N7, "7"}, {N8, "8"}, {N9, "9"},
            {N0, "0"},
            {NP1, "NP1"}, {NP2, "NP2"}, {NP3, "NP3"},
            {NP4, "NP4"}, {NP5, "NP5"}, {NP6, "NP6"},
            {NP7, "NP7"}, {NP8, "NP8"}, {NP9, "NP9"},
            {NP0, "NP0"}, {NP_DECIMAL, "Decimal"},
            {NUMBER_LOCK, "NumberLock"}, {DIVIDE, "NP/"}, {MULTIPLY, "NP*"},
            {SUBTRACT, "NP-"}, {ADD, "NP+"},
            {F1, "F1"}, {F2, "F2"}, {F3, "F3"}, {F4, "F4"},
            {F5, "F5"}, {F6, "F6"}, {F7, "F7"}, {F8, "F8"},
            {F9, "F9"}, {F10, "F10"}, {F11, "F11"}, {F12, "F12"},
            {LCONTROL, "LCtrl"}, {RCONTROL, "RCtrl"},
            {LSHIFT, "LShift"}, {RSHIFT, "RShift"},
            {LALT, "LAlt"}, {RALT, "RAlt"},
            {TAB, "Tab"}, {CAPSLOCK, "CapsLock"}, {ESC, "Esc"},
            {ENTER, "Enter"}, {BACKSPACE, "Backspace"}, {SPACE, "Space"},
            {UP, "Up"}, {DOWN, "Down"}, {LEFT, "Left"}, {RIGHT, "Right"},
            {INSERT_KEY, "Insert"}, {DELETE_KEY, "Delete"}, {HOME_KEY, "Home"},
            {END_KEY, "End"}, {PAGE_UP, "PageUp"}, {PAGE_DOWN, "PageDown"},
            {SCROLLLOCK, "ScrollLock"},
            {PRINTSCREEN, "PrintScreen"}, {PAUSE, "Pause"},
            {LSQUARE_BRACKET, "["}, {RSQUARE_BRACKET, "]"},
            {SEMICOLON, ";"}, {QUOTE, "'"}, {BACKTICK, "`"},
            {COMMA, ","}, {PERIOD, "."}, {SLASH, "/"},
            {BACKSLASH, R"(\)"}, {MINUS, "-"},
            {EQUALS, "="}, {WINDOWS, "Win"}, {MENU, "Menu"},
            // mouse key
            {MOUSE_LEFT, "MouseLeft"}, {MOUSE_RIGHT, "MouseRight"},
            {MOUSE_MIDDLE, "MouseMiddle"}, {MOUSE_X1, "MouseX1"}, {MOUSE_X2, "MouseX2"},
            {MOUSE_WHEEL_UP, "MouseWheelUp"}, {MOUSE_WHEEL_DOWN, "MouseWheelDown"}
        };
        keyCode_name_map = std::move(keyCode_name_map_);

        for (// 이것도 나중에 파일화 해야함. 아니면 SQL 쓰던가.
            const std::unordered_set unblocking_keys { make_int_keyset({
                // 이동키
                W, A, S, D,
                // 이동 조작 및 무기 발사 키
                //CAPSLOCK, SPACE,
                // 오른쪽 Enter가 먹히기 위한 KeyCode
                //ENTER,
                // 항공기 조종 키 및 무기 발사 키
                NP8, NP4, NP5, NP6, //NP0,
            }) };
            const auto& key_code : unblocking_keys) {

            unblocking_keys_bit.set(static_cast<unsigned>(key_code));
        }

        // 지정한 특수키(REnter, XButton2)를 눌렀을때 블로킹 되는 마우스 키들의 집합이다. -> decrpated
        /*inline std::unordered_set blocking_mouse { make_int_keyset({
        MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE,
        MOUSE_WHEEL_UP, MOUSE_WHEEL_DOWN }) };*/
    }
    KeyCodeManager::~KeyCodeManager() = default;

    KeyCodeManager& KeyCodeManager::getInstance() {
        static KeyCodeManager instance;
        return instance;
    }
    std::unordered_set<int> KeyCodeManager::make_int_keyset(const std::initializer_list<KeyCode>& keys) {
        std::unordered_set<int> keyset;
        for (const auto& key : keys) {
            keyset.insert(static_cast<int>(key));
        }
        return keyset;
    }
    std::string KeyCodeManager::get_key_name(const KeyCode key_code) {
        return keyCode_name_map[key_code];
    }
    std::string KeyCodeManager::get_key_name(int vk_code) {
        return keyCode_name_map[static_cast<KeyCode>(vk_code)];
    }

    std::string KeyCodeManager::get_key_name(DWORD vkCode) {
        return keyCode_name_map[static_cast<KeyCode>(vkCode)];
    }

    std::bitset<223>& KeyCodeManager::get_unblocking_key_bit() {
        return unblocking_keys_bit;
    }

    std::map<KeyCode, std::string>& KeyCodeManager::get_keyCode_name_map() { return keyCode_name_map; }

    CombineKey::CombineKey(const std::initializer_list<KeyCode>& keys) {
        for (const auto& key : keys) {
            key_bit.set(static_cast<int>(key));
        }
    }

    std::bitset<223>& CombineKey::get_key_bit() { return key_bit; }

    int CombineKey::get_first_set_bit() const {
        for (int i {0}; i < 223; i++) {
            if (key_bit.test(i)) return i;
        }
        return -1;
    }

    bool CombineKey::operator==(const CombineKey& other) const {
        return key_bit == other.key_bit;
    }

    KeyPattern::KeyPattern(const std::string_view action_name, const Actions action,
                           const PatternType type, const std::initializer_list<CombineKey>& key_pattern_list,
                           const bool is_need_blocking) :
        action_name(action_name), action(action), pattern_type(type), is_need_blocking(is_need_blocking),
        combine_vec(key_pattern_list), combine_vec_size(key_pattern_list.size()),
        combine_key_queue_size(combine_vec_size)
    {
        // 만약 pattern_type이 HOLD이고, combine_vec의 크기가 1보다 크다면
        if (pattern_type == HOLD && combine_vec_size > 1) {
            // 첫번째 comb bit인 hold_key_bit를
            const bitset hold_key_bit {combine_vec[0].get_key_bit()};
            for (int i {1}; i < combine_vec_size; ++i) {
                // 다른 bit들과 섞는다.
                combine_vec[i].get_key_bit() |= hold_key_bit;
            }
        }
        // combine_key_queue에 combine_vec의 요소들을 push한다.
        for (const auto& combine_key : combine_vec) {
            combine_key_queue.push(combine_key);
        }
    }

    std::size_t KeyPattern::get_combine_vec_size() const { return combine_vec_size; }
    auto KeyPattern::get_combine_vec() -> std::vector<CombineKey>& { return combine_vec; }

    auto KeyPattern::get_combine_key_queue() -> std::queue<CombineKey>& { return combine_key_queue; }

    auto KeyPattern::get_action_name() -> std::string& { return action_name; }
    auto KeyPattern::get_pattern_type() const -> PatternType { return pattern_type; }
    auto KeyPattern::get_action() const -> Actions { return action; }
    auto KeyPattern::get_is_need_blocking() const -> bool { return is_need_blocking; }

    auto KeyPattern::get_toggle() const -> bool {
        if (pattern_type == TOGGLE) {
            return toggle;
        }
        cerr << "This pattern is not toggle pattern. \u25A1" << endl;
        return false;
    }
    void KeyPattern::flip_toggle() {
        if (pattern_type == TOGGLE) {
            toggle = !toggle;
            cout << "toggled: " << std::boolalpha << toggle << endl;
            cout << std::noboolalpha;
            return;
        }
        cerr << "This pattern is not toggle pattern. \u25A1" << endl;
    }

    bool KeyPattern::operator==(const KeyPattern& other) const {
        return action == other.action
            && action_name == other.action_name
            && pattern_type == other.pattern_type
            && is_need_blocking == other.is_need_blocking
            && combine_vec == other.combine_vec
            && combine_vec_size == other.combine_vec_size;
    }

    PatternManager::PatternManager() = default;
    PatternManager::~PatternManager() = default;

    PatternManager& PatternManager::getInstance() { static PatternManager instance; return instance; }

    void PatternManager::push_back_key_pattern(const KeyPattern& key_pattern) {
        key_patterns.push_back(key_pattern);
    }

    // 패턴들을 적절하게 정렬해주는 멤버 메소드다.
    void PatternManager::sort_key_patterns() {
        // vector의 sort 함수를 사용해서 key_patterns vector를 정렬한다.
        ranges::sort(key_patterns,
            // Key_patterns vector에 든 KeyPattern 객체들을 비교하는 콜백 람다함수다.
         [](KeyPattern& a, KeyPattern& b) {
             // 가장 먼저 blocking 이 필요한지에 따라 정렬된다.
             if (a.get_is_need_blocking() != b.get_is_need_blocking())
                 return a.get_is_need_blocking() > b.get_is_need_blocking();
             // 만약 두 객체의 패턴 벡터의 크기가 서로 다른 경우,
             if (a.get_combine_vec_size() != b.get_combine_vec_size())
                 // 패턴 벡터의 크기가 큰 객체가 먼저 오도록 한다.
                 return a.get_combine_vec_size() > b.get_combine_vec_size();
             // 패턴 타입에 따라 정렬한다.
             if (a.get_pattern_type() != b.get_pattern_type())
                 return a.get_pattern_type() < b.get_pattern_type();
             // 두 객체의 패턴 벡터의 크기가 같거나 패턴 타입이 같을 경우,
             // 패턴 벡터의 첫번째 요소인 CombineKey 객체의 첫번째 set bit를 비교해서
             // 첫 set bit의 인덱스 크기가 작은 객체가 먼저 오도록 한다.
             return a.get_combine_key_queue().front().get_first_set_bit() <
                 b.get_combine_key_queue().front().get_first_set_bit();
         });
    }

    void PatternManager::print_key_pattern(KeyPattern& key_pattern) {
        cout << format("action name: {}", key_pattern.get_action_name()) << endl;
        string pattern_type;
        switch (key_pattern.get_pattern_type()) {
        case NORMAL:
            pattern_type = "NORMAL";
            break;
        case HOLD:
            pattern_type = "HOLD";
            break;
        case TOGGLE:
            pattern_type = "TOGGLE";
            break;
        default:
            pattern_type = "UNKNOWN";
            break;
        }
        cout << format("pattern type: {0:<6}, pattern size: {1}, need_blocking: {2:d}",
            pattern_type, key_pattern.get_combine_vec_size(),
            key_pattern.get_is_need_blocking()) << endl;
        auto combine_key_queue_copy = key_pattern.get_combine_key_queue();
        while (!combine_key_queue_copy.empty()) {
            print_key_bit(combine_key_queue_copy.front().get_key_bit(), key_data::PrintType::COMBINE);
            combine_key_queue_copy.pop();
        }
        if (key_pattern.get_combine_key_queue().empty()) {
            cout << "current key_pattern's combine_key_queue is empty" << endl;
        }
        if (commander::deep_debug_pattern_matcher.load()) {
            cout << "-----------------------------------------------------" << endl;
        }
    }

    void PatternManager::print_key_patterns_vec(std::vector<KeyPattern>& key_patterns_vec) {
        if (!commander::deep_debug_pattern_matcher.load()) return;
        for (auto& key_pattern : key_patterns_vec) {
            print_key_pattern(key_pattern);
        }
        start_line = true;
    }

    void PatternManager::print_key_filter(const std::bitset<223>& key_filter_bitset) {
        if (!commander::deep_debug_pattern_matcher.load()) return;
        cout << "key filter: " << endl;
        print_key_bit(key_filter_bitset, key_data::PrintType::COMBINE);
    }

    void PatternManager::print_current_key_patterns_and_filters() {
        cout << "------------------key_patterns--------------------" << endl;
        print_key_patterns_vec(key_patterns);

        // cout << "------------------key_patterns_block--------------------" << endl;
        // print_key_patterns_vec(key_patterns_block);
        // cout << "------------------first_key_filter_block--------------------" << endl;
        // print_key_filter(first_key_filter_block);
        //
        // cout << "------------------key_patterns_non_block--------------------" << endl;
        // print_key_patterns_vec(key_patterns_non_block);
        // cout << "------------------first_key_filter_non_block--------------------" << endl;
        // print_key_filter(first_key_filter_non_block);
        //
        // cout << "------------------single_hold_key_patterns_block--------------------" << endl;
        // print_key_patterns_vec(single_hold_key_patterns_block);
        // cout << "------------------single_hold_key_filter_block--------------------" << endl;
        // print_key_filter(single_hold_key_filter_block);
        // cout << "------------------single_hold_key_patterns_non_block--------------------" << endl;
        // print_key_patterns_vec(single_hold_key_patterns_non_block);
        // cout << "------------------single_hold_key_filter_non_block--------------------" << endl;
        // print_key_filter(single_hold_key_filter_non_block);

        // cout << "------------------toggle_key_patterns--------------------" << endl;
        // for (const auto& key_pattern : toggle_key_patterns) {
        //     print_key_pattern(*key_pattern);
        // }

        /*PatternMatcher& pattern_matcher = PatternMatcher::getInstance();
        cout << "------------------current_key_patterns_vec--------------------" << endl;
        print_key_patterns_vec(pattern_matcher.get_current_patterns_vec());
        cout << "------------------current_key_filter--------------------" << endl;
        print_key_filter(pattern_matcher.get_current_filter());
        cout << "---------------------------------------------------------------" << endl;*/
    }

    void PatternManager::initialize_pattern_manager() {
        // 모든 pattern이 pattern_manager에 push 된 후, 처음 초기화때만 아래 블록이 실행된다.
        for (auto& key_pattern : key_patterns) {
            // Hold, Blocking 여부에 따라 각각 적절하게 나눈다.
            if (key_pattern.get_is_need_blocking()) {
                if (key_pattern.get_pattern_type() == HOLD) {
                    // 홀드키인 경우에만 해당 key_pattern의 모든 combine_key의 key_bit를 filter에 or연산한다.
                    for (auto& combine_key : key_pattern.get_combine_vec()) {
                        first_key_filter_block |= combine_key.get_key_bit();
                        if(key_pattern.get_combine_vec_size() == 1) { // 단일 홀드키의 경우.
                            single_hold_key_patterns_block.push_back(key_pattern);
                        }
                    }
                } else {
                    first_key_filter_block |= key_pattern.get_combine_vec().begin()->get_key_bit();
                }
                if (key_pattern.get_pattern_type() == HOLD && key_pattern.get_combine_vec_size() == 1) continue;
                key_patterns_block.push_back(key_pattern);
            } else {
                if (key_pattern.get_pattern_type() == HOLD) {
                    for (auto& combine_key : key_pattern.get_combine_vec()) {
                        first_key_filter_non_block |= combine_key.get_key_bit();
                        if(key_pattern.get_combine_vec_size() == 1) { // 단일 홀드키의 경우.
                            single_hold_key_patterns_non_block.push_back(key_pattern);
                        }
                    }
                } else {
                    first_key_filter_non_block |= key_pattern.get_combine_vec().begin()->get_key_bit();
                }
                if (key_pattern.get_pattern_type() == HOLD && key_pattern.get_combine_vec_size() == 1) continue;
                key_patterns_non_block.push_back(key_pattern);
            }
            // toggle key bool을 위해 toggle 패턴만 따로 저장해둔다.
            if (key_pattern.get_pattern_type() == TOGGLE) {
                toggle_key_patterns.push_back(&key_pattern);
            }
        }
        // 위 루핑이 모두 끝난 후, single hold key가 담긴 vector를 순회하며
        for (auto& key_pattern : single_hold_key_patterns_block) {
            // single hold key filter bitset을 non-block과 block에 대하여 나누어 만들고 객체에 저장한다.
            single_hold_key_filter_block |= key_pattern.get_combine_key_queue().front().get_key_bit();
        }
        for (auto& key_pattern : single_hold_key_patterns_non_block) {
            single_hold_key_filter_non_block |= key_pattern.get_combine_key_queue().front().get_key_bit();
        }

        // 처음에는 non_block_key_patterns_vec를 current_key_patterns_vec로 사용한다.
        // 이게 defualt 값임. non-block이 기본값
        // 이것은 어차피 나중에 특수키를 누르면 다시 바뀐다.
        PatternMatcher::getInstance().get_current_patterns_vec() = key_patterns_non_block;
        // current가 초기화 됐으므로, current filter도 초기화한다.
        PatternMatcher::getInstance().get_current_filter() = first_key_filter_non_block;
    }

    /*void PatternManager::verify_key_patterns() {
    }*/

    auto PatternManager::get_key_patterns() -> std::vector<KeyPattern>& { return key_patterns; }

    auto PatternManager::get_key_patterns_block() -> std::vector<KeyPattern>& { return key_patterns_block; }

    auto PatternManager::get_key_patterns_block_mtx() -> std::mutex& { return key_patterns_block_mtx; }

    auto PatternManager::get_key_patterns_non_block() -> std::vector<KeyPattern>& {
        return key_patterns_non_block;
    }
    auto PatternManager::get_first_key_filter_non_block() -> std::bitset<223>& {
        return first_key_filter_non_block;
    }

    auto PatternManager::get_first_key_filter_block_mtx() -> std::mutex& { return first_key_filter_block_mtx; }

    auto PatternManager::get_first_key_filter_block() -> std::bitset<223>& { return first_key_filter_block; }

    auto PatternManager::get_single_hold_key_filter_non_block() -> std::bitset<223>& { return single_hold_key_filter_non_block; }
    auto PatternManager::get_single_hold_key_filter_block() -> std::bitset<223>& { return single_hold_key_filter_block; }

    auto PatternManager::get_single_hold_key_patterns_non_block() -> std::vector<KeyPattern>& {
        return single_hold_key_patterns_non_block;
    }
    auto PatternManager::get_single_hold_key_patterns_block() -> std::vector<KeyPattern>& {
        return single_hold_key_patterns_block;
    }
    auto PatternManager::get_toggle_key_patterns() -> std::vector<KeyPattern*>& { return toggle_key_patterns; }

    PatternMatcher::PatternMatcher() = default;
    PatternMatcher::~PatternMatcher() = default;
    PatternMatcher& PatternMatcher::getInstance() {
        static PatternMatcher instance;
        return instance;
    }

    void PatternMatcher::execute_action() {
        timer::Timer::getInstance().stop_key_release_timer();
        // 그 즉시 그 패턴의 action을 실행한다.
        ActionOperator::operate();
        // 패턴이 실행됐으면 current_key_patterns_vec를 초기화한다.
        initialize_current_resources();
        // key_bit_queue를 루핑하며 비운다.
        // key_bit_queue가 비워지는건 이렇게 action 실행이 확정되거나, timer의 time out이 된 경우이다.
    }

    bool PatternMatcher::confirm_current_key_patterns_vec(
            const std::optional<std::vector<KeyPattern>>& key_patterns_opt) {
        std::vector<KeyPattern> key_patterns;
        if (commander::into_command_mode.load()) return false;
        if (key_patterns_opt.has_value()) {
            key_patterns = key_patterns_opt.value();
        } else {
            key_patterns = current_key_patterns_vec;
        }
        if (commander::deep_debug_pattern_matcher.load()) {
            cout << "----------confirm_current_key_patterns_vec--------------" << endl;
            //PatternManager::print_key_patterns_vec(key_patterns);
        }
        if (key_patterns.empty()) return false;
        if (key_patterns.size() == 1 &&
                // 그 패턴 객체의 combine_key_queue가 비어있다면
                key_patterns.begin()->get_combine_key_queue().empty()) {
            if (commander::deep_debug_pattern_matcher.load())
                cout << "confirm one matched" << endl;
            ActionOperator::set_execute_key_pattern_opt(*key_patterns.begin());
            execute_action();
            return true; // 함수를 종료한다.
        }
        // 만약 timer가 타임아웃되고, current_key_patterns_vec의 크기가 2 이상이라면
        if (timer::Timer::getInstance().get_last_key_release_timeout()
                && key_patterns.size() >= 2) {
            if (commander::deep_debug_pattern_matcher.load())
                cout << "key_release timeout confirm" << endl;
            // current를 순회하며
            for (auto& key_pattern : key_patterns) {
                auto& current_combine_key_queue = key_pattern.get_combine_key_queue();
                // combine_key_queue가 비어있는 패턴의
                if (current_combine_key_queue.empty()) {
                    ActionOperator::set_execute_key_pattern_opt(key_pattern);
                    execute_action();
                    return true; // 함수를 종료한다.
                }
            }
        }
        return false;
    }

    void PatternMatcher::pattern_matching(const KBDLLHOOKSTRUCT& key_struct) {
        // 지정된 키 이외의 키들이 입력될때 pattern_matcher를 종료한다.
        if (key_data::exit_method_by_filters(key_struct.vkCode)) {
            if (commander::deep_debug_pattern_matcher.load()) {
                cout << "exit pattern_matching by filters: ";
                cout << KeyCodeManager::getInstance().get_key_name(key_struct.vkCode) << endl;
            }
            return;
        }
        const auto& key_state = key_data::KeyboardData::getInstance().get_key_state();
        // 현재 필터의 키 중 하나라도 눌려있으면 pattern_matcher를 실행하지 않는다.
        if ((key_state & get_current_filter()).any()) return;

        KeyCodeManager& key_code_manager = KeyCodeManager::getInstance();
        if (commander::deep_debug_pattern_matcher.load()) {
            cout << "-------------call release key pattern matcher----------------" << endl;
            cout << "release key: " << key_code_manager.get_key_name(key_struct.vkCode) << endl;
        }

        auto& key_bit_queue = key_data::KeyboardData::getInstance().get_key_bit_queue();
        // 만약 key_bit_queue가 비었다면 함수를 종료한다.
        if (key_bit_queue.empty()) {
            if (commander::deep_debug_pattern_matcher.load()) {
                cout << "key_bit_queue is empty. exit pattern_matcher." << endl;
            }
            return;
        }

        while (!key_bit_queue.empty()) {
            // 루핑을 한번 돌 때마다 next를 clear해준다.
            next_key_patterns_vec.clear();
            // 패턴 매칭을 진행하고 매칭된 패턴 객체를 next에 넣는다.
            const auto& current_key_bit = key_bit_queue.front();
            if (commander::deep_debug_pattern_matcher.load()) {
                cout << "------------------------------------------------------" << endl;
                cout << "current_key_bit : " << endl;
                print_key_bit(current_key_bit, key_data::PrintType::COMBINE);
            }

            for (auto& key_pattern : current_key_patterns_vec) {
                auto& current_combine_key_queue = key_pattern.get_combine_key_queue();
                if (!current_combine_key_queue.empty()) {
                    auto& current_pattern_bit = current_combine_key_queue.front().get_key_bit();
                    if ((current_key_bit ^ current_pattern_bit).none()) {
                        if (commander::deep_debug_pattern_matcher.load()) {
                            cout << "---------------pattern matching----------------" << endl;
                            cout << "current pattern action :" << key_pattern.get_action_name() << endl;
                            print_key_bit(current_pattern_bit, key_data::PrintType::COMBINE);
                        }
                        current_combine_key_queue.pop();
                        next_key_patterns_vec.push_back(key_pattern);
                    }
                }
            }
            // 만약 매치된게 하나라도 있다면
            if (!next_key_patterns_vec.empty()) {
                // next에 있는걸 current로 옮기고 필터를 refresh 한다.
                current_key_patterns_vec = next_key_patterns_vec;
                // current가 갱신되었으므로 current_filter 역시 갱신한다.
                refresh_current_key_filter();
            }
            // 매치된게 있든 없든 pop 시킴
            key_bit_queue.pop();
        }
        // 루프가 종료되면 current를 검사한다.
        if (confirm_current_key_patterns_vec(current_key_patterns_vec)) return;

        // if (confirm_current_key_patterns_vec()) 조건식에 의해 함수가 종료되지 않으면 타이머를 작동시킨다
        // 반드시 key_bit_queue가 비어있지 않을때에만 타이머가 작동해야한다.
        timer::Timer::getInstance().set_last_release_time_point();
        timer::Timer::getInstance().start_key_release_timer();
    }

    void PatternMatcher::pattern_matching() {
        if (commander::deep_debug_pattern_matcher.load()) {
            cout << "-------------call single hold key pattern matcher -----------------" << endl;
            cout << "by key press timeout" << endl;
        }

        auto& key_bit_queue = key_data::KeyboardData::getInstance().get_key_bit_queue();
        // 만약 key_bit_queue가 비었다면 함수를 종료한다.
        if (key_bit_queue.empty()) {
            if (commander::deep_debug_pattern_matcher.load()) {
                cout << "key_bit_queue is empty. exit pattern_matcher." << endl;
            }
            return;
        }
        // key_bit_queue의 사이즈가 1이 아니라면 함수를 종료한다.
        if (key_bit_queue.size() != 1) return;

        PatternManager& pattern_manager = PatternManager::getInstance();
        auto single_hold_key_patterns_vec =
            key_data::is_proceed_blocking_key_process.load() ?
            pattern_manager.get_single_hold_key_patterns_block() :
            pattern_manager.get_single_hold_key_patterns_non_block();

        while (!key_bit_queue.empty()) {
            // 루핑을 한번 돌 때마다 next를 clear해준다.
            current_single_hold_key_pattern.clear();
            // 패턴 매칭을 진행하고 매칭된 패턴 객체를 next에 넣는다.
            const auto& current_key_bit = key_bit_queue.front();
            if (commander::deep_debug_pattern_matcher.load()) {
                cout << "------------------------------------------------------" << endl;
                cout << "current_key_bit : " << endl;
                print_key_bit(current_key_bit, key_data::PrintType::COMBINE);
            }
            key_data::KeyboardData& keyboard_data = key_data::KeyboardData::getInstance();
            for (auto& key_pattern : single_hold_key_patterns_vec) {
                auto& current_combine_key_queue = key_pattern.get_combine_key_queue();
                if (!current_combine_key_queue.empty()) {
                    auto& current_pattern_bit = current_combine_key_queue.front().get_key_bit();
                    // 일단 현재 key_bit_queue의 front와 싱글 홀드키 패턴과 일치하는 걸 찾는다.
                    if ((current_key_bit ^ current_pattern_bit).none()) {
                        // 현재 패턴의 홀드키가 모두 눌린 상태인지 확인
                        if ((keyboard_data.get_key_state() & current_pattern_bit ^ current_pattern_bit).none()) {
                            if (commander::deep_debug_pattern_matcher.load()) {
                                cout << "---------------pattern matching----------------" << endl;
                                cout << "current pattern action :" << key_pattern.get_action_name() << endl;
                                print_key_bit(current_pattern_bit, key_data::PrintType::COMBINE);
                            }
                            current_combine_key_queue.pop();
                            current_single_hold_key_pattern.push_back(key_pattern);
                            break;
                        }
                    }
                }
            }
            key_bit_queue.pop();
        }
        // 패턴이 매치되든 아니든 key_press 타이머를 종료한다.
        timer::Timer::getInstance().stop_key_press_timer();
        if (confirm_current_key_patterns_vec(current_single_hold_key_pattern)) return;
        initialize_current_resources();
    }

    auto PatternMatcher::get_current_filter() -> std::bitset<223>& {
        return current_key_filter;
    }

    auto PatternMatcher::get_current_patterns_vec() -> std::vector<KeyPattern>& {
        return current_key_patterns_vec;
    }

    void PatternMatcher::initialize_current_resources() {
        PatternManager& pattern_manager = PatternManager::getInstance();
        current_key_filter = pattern_manager.get_first_key_filter_non_block();
        current_key_patterns_vec = pattern_manager.get_key_patterns_non_block();

        auto& key_bit_queue = key_data::KeyboardData::getInstance().get_key_bit_queue();
        while (!key_bit_queue.empty()) {
            key_bit_queue.pop();
        }
        // 작업이 끝나면 is_proceed_blocking_key_process를 false로 초기화한다.
        key_data::is_proceed_blocking_key_process.store(false);
        timer::Timer::getInstance().reset_last_key_release_timeout();
        timer::Timer::getInstance().reset_single_hold_key_press_timeout();

        if (commander::deep_debug_pattern_matcher.load())
            cout << "initialized current resources" << endl;
    }

    void PatternMatcher::decide_current_key_patterns_vec() {
        // 키보드와 마우스 스레드에서 공유되는 자원의 lock을 획득한다.
        lock_guard lock1(current_key_patterns_vec_mtx);
        lock_guard lock2(current_key_filter_mtx);
        /*
         * 특수키를 누르면 current_key_patterns_vec를 block_key_patterns로 바꾸고,
         * current_key_filter를 first_key_filter_block으로 바꾼다.
         * 만약 특수키를 뗐는데 아무것도 매치되지 않았다면 타임아웃에 의해
         * initialize_current_resources가 실행되어
         * current_key_patterns_vec를 non_block_key_patterns로 바꾸고,
         * current_key_filter를 first_key_filter_non_block으로 바꾼다.
         * 또한 is_proceed_blocking_key_process가 false로 초기화된다.
         */
        if (PatternManager& pattern_manager = PatternManager::getInstance();
            key_data::is_proceed_blocking_key_process.load()) {
            lock_guard lock3(pattern_manager.get_key_patterns_block_mtx());
            lock_guard lock4(pattern_manager.get_first_key_filter_block_mtx());
            current_key_patterns_vec = pattern_manager.get_key_patterns_block();
            current_key_filter = pattern_manager.get_first_key_filter_block();
        }
    }

    void PatternMatcher::refresh_current_key_filter() {
        if (commander::deep_debug_pattern_matcher.load())
            cout << "--------------refresh filter--------------------" << endl;
        // 현재 current_key_patterns_vec에 대한 패턴 필터 bitset을 만든다.
        const std::bitset<223> empty_bitset{};
        current_key_filter = empty_bitset;
        // 만약 패턴키가 눌리고, 특수키가 떼어진 상태에서도 이미 특수키의 눌림에 의해
        // current_key_patterns_vec가 block_key_patterns로 바뀌었다면
        // 특수키가 떼어질때 이 함수가 불려지고 아래에서 필터를 만들때
        // 바뀌어진 current_key_patterns_vec로 만들어지므로 제대로 작동할 것이다.
        for (auto& key_pattern : current_key_patterns_vec) {
            lock_guard lock(key_data::blocking_key_state_mtx);
            auto& current_combine_key_queue = key_pattern.get_combine_key_queue();
            if (!current_combine_key_queue.empty()) {
                current_key_filter |= current_combine_key_queue.front().get_key_bit();
            }
        }
        // filter debug
        if (key_data::KeyboardData::get_debug_keyboard() && commander::deep_debug_pattern_matcher.load()) {
            cout << "current_key_filter : " << endl;
            print_key_bit(current_key_filter, key_data::PrintType::COMBINE);
        }
    }
    void test_excute_action() {
        const CombineKey xc {X, C};
        const KeyPattern xc_pattern("XC_pattern",
            Actions::XC, NORMAL, {xc});
    }

    void test001() {
        KeyCodeManager& manager = KeyCodeManager::getInstance();
        cout << "keyCode_name_map.size() = " << manager.get_keyCode_name_map().size() << endl;
        // bitset의 overflow를 막기 위해 키코드가 222이므로 223으로 크기를 설정했다.
        // 이렇게 하면 -1 연산이 필요가 없이 바로 그 즉시 비트를 올바르게 설정할 수 있다.
        bitset<223> key_bit{};
        for (const auto& [key, value] : manager.get_keyCode_name_map()) {
            if (static_cast<int>(key) < 0 || static_cast<int>(key) > 500)
            {
                cout << "mouse code = " << format("{:03}", static_cast<int>(key))
                << ", name = " << value << endl;
            } else {
                cout << "key code = " << format("{:03}", static_cast<int>(key))
                << ", name = " << value << endl;
            }
            /* C++에서 range overflow는 가장 조심해야하는 적이다. */
            if (static_cast<int>(key) < key_bit.size())
                key_bit.set(static_cast<int>(key));
        }
        //cout << key_bit.to_string() << endl;
    }

    void A_Z_bits(const bitset<223>& key_bit) {
        cout << "A-Z bit: ";
        for (int i {65}; i <= 90; i++) {
            cout << key_bit[i];
        } cout << endl;
    }

    void test003() {
        PatternManager& pattern_manager = PatternManager::getInstance();

        const CombineKey exit {F1, F4};
        const CombineKey c1 {F2, F4};
        const CombineKey xc {X, C};
        const CombineKey x {X};
        const CombineKey v {V};
        const CombineKey b {B};

        // const 객체라 원본 객체는 변경이 안됨.
        // 아마 토글키에 대한 bool 변수는 pattern_manager에서 해당 key_pattern을 복제해서 따로 관리해야할듯.
        const KeyPattern exit_program("Exit Program",
            Actions::PROGRAM_EXIT, NORMAL, {exit});
        pattern_manager.push_back_key_pattern(exit_program);
        const KeyPattern command_mode("Command Mode",
            Actions::COMMAND_MODE, NORMAL, {c1});
        pattern_manager.push_back_key_pattern(command_mode);

        const KeyPattern xc_pattern("XC_pattern",
            Actions::XC, NORMAL, {xc});
        pattern_manager.push_back_key_pattern(xc_pattern);
        const KeyPattern xc_block_pattern("XC_block_pattern",
            Actions::XC_BLOCK_TOGGLE, TOGGLE, {xc}, true);
        pattern_manager.push_back_key_pattern(xc_block_pattern);

        const KeyPattern xc_hold_pattern("XC_hold_pattern",
            Actions::XC_HOLD, HOLD, {xc});
        pattern_manager.push_back_key_pattern(xc_hold_pattern);

        // const KeyPattern x_pattern("X_pattern",
        //     Actions::X, NORMAL, {x});
        // pattern_manager.push_back_key_pattern(x_pattern);

        const KeyPattern x_toggle_pattern("X_toggle_pattern",
            Actions::X_TOGGLE, TOGGLE, {x});
        pattern_manager.push_back_key_pattern(x_toggle_pattern);

        const KeyPattern x_block_pattern("X_block_toggle_pattern",
            Actions::X_BLOCK_TOGGLE, TOGGLE, {x}, true);
        pattern_manager.push_back_key_pattern(x_block_pattern);

        const KeyPattern x_v_block_pattern("X_V_block_pattern",
            Actions::X_V_BLOCK, NORMAL, {x, v}, true);
        pattern_manager.push_back_key_pattern(x_v_block_pattern);

        const KeyPattern x_v_hold_pattern("X_V_hold_pattern",
            Actions::X_V_HOLD, HOLD, {x, v});
        pattern_manager.push_back_key_pattern(x_v_hold_pattern);

        const KeyPattern xc_v_pattern("XC_V_pattern",
            Actions::XC_V, NORMAL, {xc, v});
        pattern_manager.push_back_key_pattern(xc_v_pattern);
        const KeyPattern xc_v_block_pattern("XC_V_block_pattern",
            Actions::XC_V_BLOCK, NORMAL, {xc, v}, true);
        pattern_manager.push_back_key_pattern(xc_v_block_pattern);

        const KeyPattern xc_v_hold_pattern("XC_V_hold_pattern",
            Actions::XC_V_HOLD, HOLD, {xc, v});
        pattern_manager.push_back_key_pattern(xc_v_hold_pattern);
        const KeyPattern xc_b_hold_pattern("XC_B_hold_pattern",
            Actions::XC_B_HOLD, HOLD, {xc, b});
        pattern_manager.push_back_key_pattern(xc_b_hold_pattern);

        const KeyPattern xc_v_b_hold_pattern("XC_V_B_hold_pattern",
            Actions::XC_V_B_HOLD, HOLD, {xc, v, b});
        pattern_manager.push_back_key_pattern(xc_v_b_hold_pattern);

        const KeyPattern xc_v_b_pattern("XC_V_B_pattern",
            Actions::XC_V_B, NORMAL, {xc, v, b});
        pattern_manager.push_back_key_pattern(xc_v_b_pattern);

        const KeyPattern v_pattern("V_pattern",
            Actions::V, NORMAL, {v});
        pattern_manager.push_back_key_pattern(v_pattern);

        const KeyPattern v_x_pattern("V_X_pattern",
            Actions::V_X, NORMAL, {v, x});
        pattern_manager.push_back_key_pattern(v_x_pattern);

        const KeyPattern v_hold_pattern("V_hold_pattern",
            Actions::V_HOLD, HOLD, {v});
        pattern_manager.push_back_key_pattern(v_hold_pattern);
        const KeyPattern v_block_hold_pattern("V_block_hold_pattern",
            Actions::V_BLOCK_HOLD, HOLD, {v}, true);
        pattern_manager.push_back_key_pattern(v_block_hold_pattern);

        pattern_manager.sort_key_patterns();
        pattern_manager.initialize_pattern_manager();
        pattern_manager.print_current_key_patterns_and_filters();
        ActionOperator::verify_action_function_map();
    }
}
