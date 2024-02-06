#include "components/view/main_window.h"

// 이 옵션은 console의 사용 여부이다. cmake-gui에서 설정할 수 있다.
#ifdef USE_CONSOLE
// console application일 경우에는 main() 함수가 필요하다.
wxIMPLEMENT_APP_CONSOLE(main_window::MainApp);
#else
// GUI application일 경우에는 WinMain() 함수가 필요하다.
wxIMPLEMENT_APP(main_window::MainApp);
#endif

// test code //
// #include "components/action_operator.h"
// #include <iostream>
// using namespace std;
//
// int main(int argc, char* argv[]) {
//     cout << "program start" << endl;
//     action_operator::test_inlining3();
// }
