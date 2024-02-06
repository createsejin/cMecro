//
// Created by creat on 2023-12-28.
//

#ifndef KEY_PATTERNS_H
#define KEY_PATTERNS_H
#include "actions.h"
#include <unordered_set>
#include <windows.h>
#include <vector>
#include <map>
#include <bitset>
#include <string>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>

namespace key_patterns {
    /*모든 vk_code의 열거형을 사용하기 쉽도록 재정의했으며,
    특수 키들은 WinUser.h에서 주어지는 vkCode의 매크로를 참조하여 만들었다.*/
    /* 가상키와 실제로 매치되지 않는 키들은 따로 직접 16진수 코드를 넣어주었다: N0, NP0, MULTIPLY, RALT 등 */
    enum class KeyCode {
        A = 0x41, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        N1 = 0x31, N2, N3, N4, N5, N6, N7, N8, N9, N0 = 0x30,
        NP1 = VK_NUMPAD1, NP2, NP3, NP4, NP5, NP6, NP7, NP8, NP9, NP0 = 0x60,
        NP_DECIMAL = VK_DECIMAL,
        NUMBER_LOCK = VK_NUMLOCK,
        DIVIDE = VK_DIVIDE, MULTIPLY = 0x6A, SUBTRACT = VK_SUBTRACT, ADD = VK_ADD,
        F1 = VK_F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        LCONTROL = VK_LCONTROL, RCONTROL = 0x19,
        LSHIFT = VK_LSHIFT, RSHIFT = VK_RSHIFT,
        LALT = VK_LMENU, RALT = 0x15,
        TAB = VK_TAB, CAPSLOCK = VK_CAPITAL, ESC = VK_ESCAPE,
        ENTER = VK_RETURN, BACKSPACE = VK_BACK, SPACE = VK_SPACE,
        UP = VK_UP, DOWN = VK_DOWN, LEFT = VK_LEFT, RIGHT = VK_RIGHT,
        INSERT_KEY = VK_INSERT, DELETE_KEY = VK_DELETE, HOME_KEY = VK_HOME, END_KEY = VK_END,
        PAGE_UP = VK_PRIOR, PAGE_DOWN = VK_NEXT, SCROLLLOCK = VK_SCROLL,
        PRINTSCREEN = VK_SNAPSHOT, PAUSE = VK_PAUSE,
        LSQUARE_BRACKET = VK_OEM_4, RSQUARE_BRACKET = VK_OEM_6,
        SEMICOLON = VK_OEM_1, QUOTE = VK_OEM_7,
        BACKTICK = VK_OEM_3,
        COMMA = VK_OEM_COMMA, PERIOD = VK_OEM_PERIOD, SLASH = VK_OEM_2, BACKSLASH = VK_OEM_5,
        MINUS = VK_OEM_MINUS, EQUALS = VK_OEM_PLUS,
        WINDOWS = VK_LWIN,
        MENU = VK_APPS,

        // mouse key
        MOUSE_LEFT = 0x201, MOUSE_RIGHT = 0x204, MOUSE_MIDDLE = 0x207,
        MOUSE_X1 = 0x20D, MOUSE_X2 = 0x20C,
        MOUSE_WHEEL_UP = 0x20A, MOUSE_WHEEL_DOWN = 0x20B
    };

    class KeyCodeManager {
        std::map<KeyCode, std::string> keyCode_name_map;
        std::bitset<223> unblocking_keys_bit{};
        //KeyCode blocking_keyboard_key;
        //*deprecated*/std::bitset<223> blocking_keyboard_key_bit{};
        //KeyCode blocking_mouse_key;

        // 생성자를 private으로 선언하여 외부에서 객체를 생성하는 것을 막는다.
        KeyCodeManager();
        ~KeyCodeManager();
    public:
        // 싱글톤 객체의 참조를 반환하는 함수다.
        static KeyCodeManager& getInstance();

        // 복사 생성자와 대입 연산자를 삭제한다.
        KeyCodeManager(const KeyCodeManager&) = delete;
        KeyCodeManager(KeyCodeManager&&) = delete;
        KeyCodeManager& operator=(const KeyCodeManager&) = delete;
        KeyCodeManager& operator=(KeyCodeManager&&) = delete;

        // KeyCode list를 받아서 int형 unordered_set을 만들어준다.
        static std::unordered_set<int> make_int_keyset(const std::initializer_list<KeyCode>& keys);
        // vk_code나 KeyCode를 받아서 해당하는 키의 이름을 반환한다.
        std::string get_key_name(KeyCode key_code);
        std::string get_key_name(int vk_code);
        std::string get_key_name(DWORD vkCode);
        // 특수키(Enter, Xbutton2)가 눌렸을때 blocking되지 않는 키들의 set 참조를 반환한다.
        auto get_unblocking_key_bit() -> std::bitset<223>&;
        // KeyCode의 이름을 담고 있는 map 참조를 반환한다.
        std::map<KeyCode, std::string>& get_keyCode_name_map();
    };

    /* PatternType 규칙 :
     * Pattern Type은 Key Pattern에 적용된다. 여기에서 Key Pattern은 다음과 같은 형태이다.
     * key_pattern1 = {{S, D}, {F, G}, {T}}
     * 이 키 패턴은 Pattern Type이 NORMAL이며,
     * 각 각 2개, 2개, 1개의 키를 가지는 CombineKeys가 3개로 이루어진 Key Pattern이며
     * 각 각의 CombineKeys들은 다음과 같다.
     * comb1 = {S, D}, comb2 = {F, G}, comb3 = {T}
     * 각각의 CombineKeys들은 1개에서 3개 까지의 결합키를 가질 수 있다.
     * key_pattern1에서 각각의 comb 키들은 나열된 순서를 통해 이들의 키 입력 순서를 나타낸다. 이를 Key Sequence라 칭한다.
     * 따라서 위의 Key Pattern은 첫번째로 {S, D}를 누르고, 그 다음에 {F, G}를 누르고,
     * 그 다음에 {T}를 누르는 키 입력 순서(Key Sequence)를 나타낸다.
     *
     * HOLD나 TOGGLE 타입 규칙:
     * 1. HOLD나 TOGGLE 타입은 반드시 Key Pattern의 첫번째 CombineKeys여야 한다.
     * 2. TOGGLE 타입의 첫번째 CombineKeys는 여러개의 결합키를 가질 수 있다.
     * 3. HOLD 타입은 첫번째 hold CombineKeys를 비롯한 추가적인 Key Sequence를 가질 수 있다.
     *   예를 들면 다음과 같다: {h{S, D}, {F}, {R}}
     *   다음은 불가능하다: {h{S, D}, {S}, {R}} 즉, 홀드키 다음의 Key Sequence들은 홀드키가 점유하는 키를 가질 수 없다.
     * 4. TOGGLE 타입은 반드시 단 하나의 CombineKeys를 가지는 Key Pattern이어야 한다.
     *   예를 들면 다음과 같다: {t{S, D}}
     *   다음은 불가능하다: {t{S, D}, {F, G}}
     *   이때는 TOGGLE 타입의 Key Pattern이 두 개 이상의 CombineKeys를 가지고 있기 때문이다.
     * 5. TOGGLE 타입과 NOMAL 타입의 첫번째 CombineKeys는 서로 겹칠 수 없다.
     *   예를 들면 다음을 불가능하다: {t{S, D}}, {{S, D}, {F, G}}
     *   다음은 가능하다: {t{S, D}}, {{F, G}, {S, D}}
     */
    enum class PatternType { // 패턴의 입력 타입이다. 구체적인 규칙은 위에서 설명하였다.
        HOLD, TOGGLE, NORMAL
    };

    class CombineKey {
        // combine key들의 집합이다.
        std::bitset<223> key_bit{};
    public:
        // KeyCode list를 받아서 CombineKey 객체를 만들고 key_bit를 setting 한다.
        CombineKey(const std::initializer_list<KeyCode>& keys);
        // CombineKey 객체의 key_bit bitset의 참조를 반환한다.
        [[nodiscard]] std::bitset<223>& get_key_bit();
        // CombineKey 객체의 key_bit bitset의 가장 첫번째 비트의 인덱스를 반환한다.
        // 추후 KeyPattern 객체들의 vector를 정렬하는데에 쓰인다.
        [[nodiscard]] int get_first_set_bit() const;
        bool operator==(const CombineKey& other) const;
    };

    // 이 클래스의 객체들을 담는 vector를 나중에 sort해야하기 때문에 멤버 변수들을 const로 지정할 수 없다.
    class KeyPattern {
        // 실행할 명령의 구체적인 이름.
        std::string action_name;
        // 실행할 명령의 구체적인 열거 변수다. 이를 통해 오퍼레이터는 어떤 명령을 실행시킬지 알 수 있다.
        action_operator::Actions action;
        PatternType pattern_type; // pattern type
        bool is_need_blocking; // 특수키를 눌러서 blocking된 상태에서만 동작하는 pattern을 구분한다.
        // 여러 CombineKey 객체들이 담기는 vector. 이 vector의 각 CombineKey들은
        // 순서대로 눌러야하는 key_sequence를 나타낸다.
        std::vector<CombineKey> combine_vec;
        std::size_t combine_vec_size; // key_pattern_vec의 size, 즉 pattern size이다.
        std::queue<CombineKey> combine_key_queue;
        std::size_t combine_key_queue_size;
        bool toggle {false};
    public:
        KeyPattern( // 여러가지 입력을 받아서 KeyPattern 객체를 만드는 생성자.
            // action_name을 string으로 받는다.
            std::string_view action_name,
            // operator가 실행할 구체적인 Actions 열거 변수를 받는다.
            action_operator::Actions action,
            PatternType type, // pattern type
            // 여러개의 CombineKey 객체, 혹은 단일 개수의 CombineKey 객체 list를 받는다.
            const std::initializer_list<CombineKey>& key_pattern_list,
            // 특수키를 눌러서 blocking된 상태에서만 동작하는 pattern인지의 여부를 받는다.
            bool is_need_blocking = false);
        // KeyPattern 객체의 key_pattern_vec_size를 반환한다. 나중에 sort 우선순위에서 1순위다.
        [[nodiscard]] std::size_t get_combine_vec_size() const;
        // CombineKey 객체를 가지는 key_pattern_vec의 참조를 반환한다.
        [[nodiscard]] auto get_combine_vec() -> std::vector<CombineKey>&;
        [[nodiscard]] auto get_combine_key_queue() -> std::queue<CombineKey>&;
        // action_name string의 참조 반환
        [[nodiscard]] auto get_action_name() -> std::string&;
        // 참조를 반환하지 않는 타입의 getter는 const 메소드가 가능하다.
        // PatternType을 반환
        [[nodiscard]] auto get_pattern_type() const -> PatternType;
        // 실행할 명령의 actions 열거 타입 변수를 반환
        [[nodiscard]] auto get_action() const -> action_operator::Actions;
        // 키 블로킹이 필요한지 여부를 반환
        [[nodiscard]] auto get_is_need_blocking() const -> bool;

        [[nodiscard]] auto get_toggle() const -> bool;
        void flip_toggle();
        bool operator==(const KeyPattern& other) const;
    };

    class PatternManager {
        inline static bool start_line {true};
        // KeyPattern 객체들이 담기는 vector. 모든 패턴의 원본을 담는다.
        std::vector<KeyPattern> key_patterns{};
        // 일반적 패턴
        std::vector<KeyPattern> key_patterns_non_block{};
        // 특수키를 누를때의 패턴
        std::vector<KeyPattern> key_patterns_block{};
        std::mutex key_patterns_block_mtx;
        // hold key patterns non-block and block
        std::vector<KeyPattern> single_hold_key_patterns_non_block{};
        std::vector<KeyPattern> single_hold_key_patterns_block{};

        // 홀드키의 눌림을 감지하기 위한 키 비트셋.
        // 이 비트셋과 key_state의 비트셋을 비교하여 홀드키가 눌렸는지를 판단한다.
        // 홀드키가 하나라도 눌린 상태라면 pattern_matcher를 실행하지 않는다.
        // 홀드키가 모두 떼어졌을때 pattern_matcher를 실행한다.
        std::bitset<223> single_hold_key_filter_non_block{};
        std::bitset<223> single_hold_key_filter_block{};
        // non-block 상태에서의 모든 패턴의 첫번째 키 필터다.
        // 첫 패턴키를 입력할때 다른 키들을 필터링 하기 위해 사용된다.
        // 두번째 부터는 필터가 갱신되어 첫번째에서 눌렸던 패턴의 그 다음 패턴들이 새로운 필터가 된다.
        std::bitset<223> first_key_filter_non_block{};
        std::bitset<223> first_key_filter_block{};
        std::mutex first_key_filter_block_mtx;
        // toggle_key pattern들에 대한 포인터가 저장되는 벡터다. 이를 통해 원본인 key_patterns 벡터의
        // 원본 객체를 직접 수정할 수 있게 만든다.
        std::vector<KeyPattern*> toggle_key_patterns{};
        // for singleton pattern
        PatternManager();
        ~PatternManager();
    public:
        // for singleton pattern
        static PatternManager& getInstance();
        PatternManager(const PatternManager&) = delete;
        PatternManager(PatternManager&&) = delete;
        PatternManager& operator=(const PatternManager&) = delete;
        PatternManager& operator=(PatternManager&&) = delete;

        // key_patterns vector에 KeyPattern 객체를 push_back 한다.
        void push_back_key_pattern(const KeyPattern& key_pattern);
        // key_patterns vector를 알맞게 정렬한다.
        void sort_key_patterns();
        // 하나의 key_pattern 객체에 대한 정보를 print한다.
        static void print_key_pattern(KeyPattern& key_pattern);
        // 모든 key_pattern 객체가 담긴 key_patterns vector의 모든 객체에 대한 정보를 print한다.
        static void print_key_patterns_vec(std::vector<KeyPattern>& key_patterns_vec);
        static void print_key_filter(const std::bitset<223>& key_filter_bitset);
        void print_current_key_patterns_and_filters();
        // 패턴이 모두 들어간 후 각 vector들과 filter들을 적절하게 초기화 한다.
        void initialize_pattern_manager();
        //void verify_key_patterns();

        [[nodiscard]] auto get_key_patterns() -> std::vector<KeyPattern>&;
        [[nodiscard]] auto get_key_patterns_block() -> std::vector<KeyPattern>&;
        [[nodiscard]] auto get_key_patterns_block_mtx() -> std::mutex&;
        [[nodiscard]] auto get_key_patterns_non_block() -> std::vector<KeyPattern>&;
        [[nodiscard]] auto get_first_key_filter_block() -> std::bitset<223>&;
        [[nodiscard]] auto get_first_key_filter_block_mtx() -> std::mutex&;
        [[nodiscard]] auto get_first_key_filter_non_block() -> std::bitset<223>&;\
        [[nodiscard]] auto get_single_hold_key_filter_non_block() -> std::bitset<223>&;
        [[nodiscard]] auto get_single_hold_key_filter_block() -> std::bitset<223>&;
        [[nodiscard]] auto get_single_hold_key_patterns_non_block() -> std::vector<KeyPattern>&;
        [[nodiscard]] auto get_single_hold_key_patterns_block() -> std::vector<KeyPattern>&;
        [[nodiscard]] auto get_toggle_key_patterns() -> std::vector<KeyPattern*>&;
    };

    class PatternMatcher {
        std::vector<KeyPattern> current_key_patterns_vec{};
        std::vector<KeyPattern> next_key_patterns_vec{};
        std::mutex current_key_patterns_vec_mtx;
        std::bitset<223> current_key_filter{};
        std::mutex current_key_filter_mtx;
        std::vector<KeyPattern> current_single_hold_key_pattern{};

        PatternMatcher();
        ~PatternMatcher();

        // confirm_current_key_patterns_vec()에서 적절한 조건에 의해 명령이 확정되면 그 명령을 실행한다.
        void execute_action();
        void refresh_current_key_filter();
    public:
        static PatternMatcher& getInstance();
        PatternMatcher(const PatternMatcher&) = delete;
        PatternMatcher(PatternMatcher&&) = delete;
        PatternMatcher& operator=(const PatternMatcher&) = delete;
        PatternMatcher& operator=(PatternMatcher&&) = delete;

        // current_key_patterns_vec을 검사해서 적절한 시점에 action을 실행할지 결정한다.
        bool confirm_current_key_patterns_vec(
            const std::optional<std::vector<KeyPattern>>& key_patterns_opt = std::nullopt);
        // key_bit_queue의 첫번째 요소에 대한 pattern matching을 진행하고 매치된 패턴을 current_key_patterns_vec에 담는다.
        // 마지막 release 때 실행되는 pattern_matching
        void pattern_matching(const KBDLLHOOKSTRUCT& key_struct);
        // key_press time out 때 실행되는 pattern_matching
        void pattern_matching();
        [[nodiscard]] auto get_current_filter() -> std::bitset<223>&;
        [[nodiscard]] auto get_current_patterns_vec() -> std::vector<KeyPattern>&;
        void initialize_current_resources();
        void decide_current_key_patterns_vec();
    };

    // test code //
    auto test_excute_action() -> void;
    // vk_code를 bitset의 index로 바꾸는 테스트다.
    void test001();
    // test initialize key patterns
    void test003();
}
#endif //KEY_PATTERNS_H
