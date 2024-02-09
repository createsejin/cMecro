//
// Created by bae on 2024-01-19.
//

#include "timer.h"

#include <iostream>

#include "commander.h"
#include "key_data.h"
#include "key_patterns.h"
using namespace std;

namespace timer
{
    Timer::Timer()
        // thread 변수를 초기화 한다.
        // 여기에서 &TimeMeasurement::timeMeasurementLoop는 TimeMeasurement의 멤버 메소드를 가리키는 주소다.
        // this는 현재 객체의 포인터이다.
    {
        if (commander::command_mode.load()) return;
        key_release_timer_thread = std::move(std::thread{&Timer::key_release_timerLoop, this});
        key_press_timer_thread = std::move(std::thread{&Timer::key_press_timerLoop, this});
    }

    Timer::~Timer() {
        // 객체가 소멸될때 스레드가 joinable하면 join한다.
        if (key_release_timer_thread.joinable()) {
            key_release_timer_thread.join();
        }
        if (key_press_timer_thread.joinable()) {
            key_press_timer_thread.join();
        }
    }

    Timer& Timer::getInstance() {
        static Timer instance;
        return instance;
    }

    void Timer::key_release_timerLoop() {
        while (true) {
            // condition_variable의 unique_lock
            std::unique_lock cv_lock(key_release_timer_cv_mtx);
            key_release_timer_cv.wait(cv_lock, [] {
                // timer_active가 true가 되거나 커맨드 모드로 들어갈 때까지 기다린다.
                return key_release_timer_ative.load() || commander::into_command_mode.load();
            });

            // 만약 커맨드 모드로 들어가면
            if (commander::into_command_mode.load()) {
                cv_lock.unlock(); // lock을 해제하고
                key_release_timer_cv.notify_one(); // condition variable에 notify하고
                unique_lock cout_lock(cout_mtx);
                cout << "key_release_timer is breaked." << endl;
                cout_lock.unlock();
                break; // 루프에 break를 건다.
            }

            // doing something here --------------------------------------//
            // timer thread의 현재 time point를 얻는다.
            std::chrono::steady_clock::time_point current_time_point = std::chrono::steady_clock::now();
            // last_release_time_point와 current_time_point의 duration을 계산한다.
            const auto current_release_duration = std::chrono::duration_cast<std::chrono::milliseconds>
                (current_time_point - last_key_release_time_point);
            // last_release로부터 일정 시간이 지나면 timeout을 true로 설정한고 confirm을 호출한다.
            if (current_release_duration >= relase_duration) {
                // time out이 되면
                if (commander::deep_debug_pattern_matcher.load())
                    cout << "-------------last key release timeout------------------" << endl;
                last_key_release_timeout.store(true);
                key_release_timer_ative.store(false);
                // current를 재평가한다.
                if (!key_patterns::PatternMatcher::getInstance().confirm_current_key_patterns_vec()) {
                    // 아무것도 매치되지 않았을때에도 current를 초기화한다.
                    key_patterns::PatternMatcher::getInstance().initialize_current_resources();
                    // key_bit_queue를 루핑하며 출력하고 비운다.
                }
                //calculate_duration();
            }
            // doing something end ----------------------------------------//

            // thread sleep duration
            std::this_thread::sleep_for(key_release_thread_duration);

            // 만약 timer_ative가 false가 되면
            if (!key_release_timer_ative.load()) {
                // lock이 해제되고
                cv_lock.unlock();
                // condition variable에 notify : 이걸 하는 이유는 다시 루핑될때 wait에서 콜백 함수를 재평가하기 위함이다.
                // 만약 이후에도 pattern_key_active가 false라면 어차피 콜백이 false이기 때문에
                // 다시 대기 상태에 들어가게 된다.
                //std::cout << "timer is stop" << std::endl;
                key_release_timer_cv.notify_one();
            } // 이후에 다시 루핑되고 처음으로 돌아가서 wait 한다.
        }
    }

    void Timer::key_press_timerLoop() {
        while (true) {
            // condition_variable의 unique_lock
            std::unique_lock cv_lock(key_press_timer_cv_mtx);
            key_press_timer_cv.wait(cv_lock, [] {
                // timer_active가 true가 되거나 커맨드 모드로 들어갈 때까지 기다린다.
                return key_press_timer_ative.load() || commander::into_command_mode.load();
            });
            // 만약 커맨드 모드로 들어가면
            if (commander::into_command_mode.load()) {
                cv_lock.unlock(); // lock을 해제하고
                key_press_timer_cv.notify_one(); // condition variable에 notify하고
                unique_lock cout_lock(cout_mtx);
                cout << "key_press_timer is breaked." << endl;
                cout_lock.unlock();
                break; // 루프에 break를 건다.
            }

            // doing something here --------------------------------------//
            // timer thread의 현재 time point를 얻는다.
            std::chrono::steady_clock::time_point current_time_point = std::chrono::steady_clock::now();
            // key press 이후 일정 hold_key_press_duration이 지나면
            const auto current_press_duration = std::chrono::duration_cast<std::chrono::milliseconds>
                (current_time_point - current_key_press_time_point);
            if (current_press_duration >= hold_key_press_duration
                && !commander::into_command_mode.load()) {
                if (commander::deep_debug_pattern_matcher.load())
                    cout << "--------------hold key press timeout---------------------" << endl;
                single_hold_key_press_timeout.store(true);
                key_press_timer_ative.store(false);
                key_patterns::PatternMatcher::getInstance().pattern_matching();
            }
            // doing something end ----------------------------------------//
            // thread sleep duration
            std::this_thread::sleep_for(key_press_thread_duration);

            if (!key_press_timer_ative.load()) {
                cv_lock.unlock();
                key_press_timer_cv.notify_one();
            }
        }
    }

    void Timer::start_key_release_timer() {
        key_release_timer_ative.store(true);
        key_release_timer_cv.notify_one();
    }
    void Timer::stop_key_release_timer() {
        key_release_timer_ative.store(false);
        key_release_timer_cv.notify_one();
    }

    void Timer::start_key_press_timer() {
        key_press_timer_ative.store(true);
        key_press_timer_cv.notify_one();
    }
    void Timer::stop_key_press_timer() {
        key_press_timer_ative.store(false);
        key_press_timer_cv.notify_one();
    }

    void Timer::restart_key_release_timer_thread() {
        key_release_timer_thread = std::thread(&Timer::key_release_timerLoop, this);
    }

    void Timer::restart_key_press_timer_thread() {
        key_press_timer_thread = std::thread(&Timer::key_press_timerLoop, this);
    }

    void Timer::join_key_release_timer_thread() {
        if (key_release_timer_thread.joinable()) {
            key_release_timer_thread.join();
        }
    }

    void Timer::join_key_press_timer_thread() {
        if (key_press_timer_thread.joinable()) {
            key_press_timer_thread.join();
        }
    }

    void Timer::set_last_release_time_point() {
        last_key_release_time_point = std::chrono::steady_clock::now();
        if (duration_check) {
            last_key_release_time_point_vec.push_back(last_key_release_time_point);
            if (key_data::KeyboardData::get_debug_keyboard()) {
                key_data::KeyboardData::print_pressed_keys();
                std::cout << "last key release time = " <<
                    std::chrono::duration_cast<std::chrono::milliseconds>
                    (last_key_release_time_point.time_since_epoch()) << std::endl;
            }
        }
    }

    void Timer::set_previous_key_time_point() {
        previous_key_press_time_point = current_key_press_time_point;

        if (check_previous_key_time_point)
            std::cout << "previous key press time = " <<
                std::chrono::duration_cast<std::chrono::milliseconds>
                (previous_key_press_time_point.time_since_epoch()) << std::endl;
    }

    void Timer::set_current_key_time_point() {
        current_key_press_time_point = std::chrono::steady_clock::now();
    }

    auto Timer::compare_press_key_time_point() const -> bool {
        return current_key_press_time_point - previous_key_press_time_point <= simultaneous_key_duration;
    }

    void Timer::calculate_duration() {
        if (duration_check && current_key_press_time_point_vec.size() > 1
                && !last_key_release_time_point_vec.empty()) {
            const auto current_time = current_key_press_time_point_vec[1];
            const auto last_time = last_key_release_time_point_vec[0];
            const auto duration {std::chrono::duration_cast<std::chrono::milliseconds>(last_time - current_time)};
            std::cout << "duration = " << duration << std::endl;
            current_key_press_time_point_vec.clear();
            last_key_release_time_point_vec.clear();
        }
    }

    auto Timer::get_last_key_release_timeout() const -> bool { return last_key_release_timeout.load(); }

    void Timer::reset_last_key_release_timeout() { last_key_release_timeout.store(false); }

    void Timer::reset_single_hold_key_press_timeout() { single_hold_key_press_timeout.store(false); }

    auto Timer::get_single_hold_key_press_timeout() const -> bool { return single_hold_key_press_timeout.load(); }

    void Timer::set_duration_check(const bool value) { duration_check = value; }

    void Timer::set_check_previous_key_time_point(const bool check) { check_previous_key_time_point = check; }
}
