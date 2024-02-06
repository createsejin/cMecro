//
// Created by bae on 2024-01-02.
//

#include "test002_key_pattern.h"

#include <utility>
#include "../components/key_patterns.h"
using namespace std;

namespace key_patterns_test
{
    using enum key_patterns::KeyCode;
    using enum PatternType;

    CombineKeys_old::CombineKeys_old(const std::initializer_list<key_patterns::KeyCode>& keys) :
    keys(keys), key_size(keys.size()) { this->key_state.resize(key_size * 2); }

    size_t CombineKeys_old::get_key_size() const { return key_size; }
    std::vector<bool>& CombineKeys_old::get_key_state() { return key_state; }

    KeyPattern_old::KeyPattern_old(const std::string_view action_name, const PatternType type,
                           std::vector<CombineKeys_old> key_pattern_vec, const bool is_need_blocking) :
        action_name(action_name), pattern_type(type),
        key_pattern_vec(std::move(key_pattern_vec)), is_need_blocking(is_need_blocking) { }

    using Cb = CombineKeys_old;
    void test000() {
        auto pattern1 = KeyPattern_old("test_action1"sv,NORMAL, {Cb{F, G}, Cb{T}});
    }
}