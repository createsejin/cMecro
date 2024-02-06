//
// Created by bae on 2024-01-19.
//

#ifndef TIMER_H
#define TIMER_H
#include <condition_variable>
#include <thread>
#include <vector>

namespace timer
{
    class Timer {
        bool duration_check {false};
        bool check_previous_key_time_point {false};
        // 타이머를 재우고 깨우는 condition variable
        std::condition_variable key_release_timer_cv;
        std::mutex key_release_timer_cv_mtx;
        std::thread key_release_timer_thread;
        std::condition_variable key_press_timer_cv;
        std::mutex key_press_timer_cv_mtx;
        std::thread key_press_timer_thread;
        std::mutex cout_mtx; // 출력 스트림을 위한 mutex
        // timer가 동작 여부를 설정해준다. 이걸 설정하고 반드시 timer_cv.notify_one()을 호출해야한다.
        inline static std::atomic<bool> key_release_timer_ative {false};
        inline static std::atomic<bool> key_press_timer_ative {false};
        // timer loop 한 사이클의 sleep duration이다.
        const std::chrono::milliseconds key_release_thread_duration{30};
        const std::chrono::milliseconds key_press_thread_duration{30};
        // 마지막 키가 release된 시점 기준으로 timeout이 됐을때 true로 설정된다.
        std::atomic<bool> last_key_release_timeout {false};
        // 홀드키가 press된 시점 기준으로 timeout이 됐을때 true로 설정된다.
        std::atomic<bool> single_hold_key_press_timeout {false};
        // 이걸 이용해서 key_bit_queue에 동시키, 또는 순서키가 적절하게 삽입된다.
        std::chrono::steady_clock::time_point previous_key_press_time_point{};
        // 이 변수는 last_key_press time point를 확인할때에도 사용된다.
        std::chrono::steady_clock::time_point current_key_press_time_point{};
        // hold key의 입력 duration을 설정해준다.
        const std::chrono::milliseconds hold_key_press_duration{300};
        // 키가 release된 시점을 기준으로 타임아웃을 재기 위한 time_point 변수이다.
        // 키가 release될 때마다 갱신된다.
        std::chrono::steady_clock::time_point last_key_release_time_point{};
        // release duration의 timeout 기준 시간이다.
        const std::chrono::milliseconds relase_duration{237};
        // debug를 위한 변수 저장 vector
        std::vector<std::chrono::steady_clock::time_point> current_key_press_time_point_vec{};
        std::vector<std::chrono::steady_clock::time_point> last_key_release_time_point_vec{};
        // 동시키의 입력 duration을 설정해준다. 위 두 변수들의 compare 메소드에서 사용된다.
        const std::chrono::milliseconds simultaneous_key_duration{32};

        Timer();
        ~Timer();
        // timer thread의 메인 루프이다.
        void key_release_timerLoop();
        void key_press_timerLoop();
    public:
        // timer의 singleton 객체 참조를 반환한다.
        static Timer& getInstance();
        // singleton 객체를 위한 각종 복사 생성자들을 막아놓음.
        Timer(const Timer&) = delete;
        Timer(Timer&&) = delete;
        Timer& operator=(const Timer&) = delete;
        Timer& operator=(Timer&&) = delete;

        void start_key_release_timer(); // 반드시 이 함수를 실행시켜야 타이머가 깨어난다.
        void stop_key_release_timer(); // 타이머를 멈추고 재우게 한다.

        void start_key_press_timer();
        void stop_key_press_timer();

        void restart_key_release_timer_thread(); // 타이머 스레드를 재시작 시킨다.
        void restart_key_press_timer_thread();
        // 타이머 스레드를 종료시킨다.
        void join_key_release_timer_thread();
        void join_key_press_timer_thread();
        // key release를 설정하는 메소드. 매개변수가 있는 메소드는 key_struck의 time으로 설정해준다.
        // 그게 아니라면 이 메소드에서의 현재 now()를 통해 time_point를 설정해준다.
        void set_last_release_time_point();
        //void set_last_key_press_time_point();
        // key_bit_queue push를 위한 time_point 설정 메소드
        void set_previous_key_time_point();
        void set_current_key_time_point();
        // 이것 역시 key_bit_queue push때 사용된다.
        auto compare_press_key_time_point() const -> bool;
        void calculate_duration();
        // 키가 release된 시점을 기준으로 타임아웃을 재기 위한 time_point 변수 상태를 리턴한다.
        [[nodiscard]] auto get_last_key_release_timeout() const -> bool;
        void reset_last_key_release_timeout();
        [[nodiscard]] auto get_single_hold_key_press_timeout() const -> bool;
        void reset_single_hold_key_press_timeout();
        // debug 변수 설정 메소드. 얘네들 없으면 always false 경고 뱉어서 그냥 만듦
        void set_duration_check(bool value);
        void set_check_previous_key_time_point(bool check);
    };
}
#endif //TIMER_H
