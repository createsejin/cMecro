//
// Created by bae on 2024-01-02.
//

#ifndef TEST002_KEY_PATTERN_H
#define TEST002_KEY_PATTERN_H
#include "../components/key_patterns.h"
namespace key_patterns_test {
    /* PatternType 규칙 :
     * 각 시퀀스 index가 일치하는 패턴끼리는 중복될 수 없다.
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
     * 2. HOLD나 TOGGLE 타입의 첫번째 CombineKeys는 1개에서 3개 까지의 결합키를 가질 수 있다.
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
    enum class PatternType {
        NORMAL, HOLD, TOGGLE
    };

    class CombineKeys_old {
        // combine key들의 집합이다.
        const std::vector<key_patterns::KeyCode> keys;
        const size_t key_size;
        std::vector<bool> key_state{};
    public:
        CombineKeys_old(const std::initializer_list<key_patterns::KeyCode>& keys);
        [[nodiscard]] auto get_key_size() const -> size_t;
        std::vector<bool>& get_key_state();
    };

    class KeyPattern_old {
        const std::string action_name;
        /* PatternType이 HOLD나 TOGGLE일 경우에는 반드시 첫번째 CombineKeys가 HOLD나 TOGGLE이어야 한다.
         * 또한 TOGGLE일 경우에는 반드시 하나의 CombineKeys만을 가져야한다.
         * 따라서 이를 검사하여 조건에 맞지 않으면 예외를 던진다던지 해야한다. */
        const PatternType pattern_type;
        const std::vector<CombineKeys_old> key_pattern_vec;
        const bool is_need_blocking;

        bool is_hold{};
        bool is_toggle{};
    public:
        KeyPattern_old(std::string_view action_name, PatternType type,
            std::vector<CombineKeys_old> key_pattern_vec, bool is_need_blocking = false);
    };

    void test000();
}
#endif //TEST002_KEY_PATTERN_H
