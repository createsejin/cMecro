//
// Created by creat on 2023-12-27.
//
#ifndef COMMANDER_H
#define COMMANDER_H
#include "../lib/CLI11.hpp"
#include <future>

namespace commander {
    // pattern matcher와 관련된 코드들의 deep debug option
    inline std::future<void> commander_future;
    inline std::mutex commander_future_mutex;
    inline std::atomic deep_debug_pattern_matcher{false};
    inline std::atomic into_command_mode{false};
    inline std::atomic exit_program{false};
    extern std::unique_ptr<CLI::App> commander_app_ptr;

    void set_debug_option_from_command_line(int argc, char* argv[]);
    auto get_args_from_input(std::string_view prompt) -> std::vector<std::string>;
    void commander(int argc, char* argv[]);
}
#endif //COMMANDER_H
