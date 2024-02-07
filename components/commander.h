//
// Created by creat on 2023-12-27.
//
#ifndef COMMANDER_H
#define COMMANDER_H
#include "../lib/CLI11.hpp"
#include <future>

namespace commander {
    inline CLI::App cli{"command line interface"};
    // pattern matcher와 관련된 코드들의 deep debug option
    inline std::future<void> commander_future;
    inline std::mutex commander_future_mutex;
    inline std::atomic deep_debug_pattern_matcher{false};
    inline std::atomic into_command_mode{false};
    inline std::atomic exit_program{false};

    void set_debug_option_from_command_line(int argc, char* argv[]);
    inline std::condition_variable command_mode_cv;
    inline std::condition_variable exit_program_cv;
    void commander(int argc, char* argv[]);
}
#endif //COMMANDER_H
