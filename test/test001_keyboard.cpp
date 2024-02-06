//
// Created by creat on 2023-12-27.
//
#include "test001_keyboard.h"
#include "../components/key_patterns.h"
#include <windows.h>
#include <thread>
#include <chrono>
#include <iostream>
using namespace std;

namespace keyboard_test {
    using enum key_patterns::KeyCode;

    UINT PressKeyScan(const WORD scanCode, const bool isExtendedKey)
    {
        INPUT input[1] = {0};
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = NULL;
        input[0].ki.wScan = scanCode;
        input[0].ki.dwFlags = KEYEVENTF_SCANCODE;
        if (isExtendedKey) {
            input[0].ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
        }

        const UINT ret = SendInput(1, input, sizeof(INPUT));

        return ret;
    }

    UINT ReleaseKeyScan(const WORD scanCode, const bool isExtendedKey)
    {
        INPUT input[1] = {0};
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = NULL;
        input[0].ki.wScan = scanCode;
        input[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
        if (isExtendedKey) {
            input[0].ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
        }

        const UINT ret = SendInput(1, input, sizeof(INPUT));

        return ret;
    }

    void key_press_test2(const WORD scanCode) {
        PressKeyScan(scanCode, true);
        this_thread::sleep_for(chrono::milliseconds(300));
        ReleaseKeyScan(scanCode, true);
        this_thread::sleep_for(chrono::milliseconds(300));
    }

    void key_press_test_threaded(const WORD scanCode) {
        std::thread([scanCode]() {
            PressKeyScan(scanCode, true);
            std::this_thread::sleep_for(std::chrono::milliseconds(70));
            ReleaseKeyScan(scanCode, true);
            std::this_thread::sleep_for(std::chrono::milliseconds(70));
        }).detach();
    }

    auto PrintScanCode() -> bool {
        for (int vkCode = 0; vkCode <= 255; ++vkCode) {
            if (GetAsyncKeyState(vkCode) & 0x8000) {
                const UINT scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);
                std::cout << format("vk code = {0:#X}, scan code = {1:#X}",
                    vkCode, scanCode) << std::endl;
                if (vkCode == VK_ESCAPE) {
                    return true;
                }
            }
        }
        return false;
    }

    void test_002() {
        cout << "Test 002 start" << endl;
        this_thread::sleep_for(chrono::seconds(3));

        for (int i{0}; i < 3; ++i) {
            key_press_test_threaded(0x50);
        }
        cout << "test end" << endl;
    }

    void test_003() {
        while (true) {
            std::this_thread::sleep_for(chrono::microseconds(400));
            if (PrintScanCode()) break;
        }
        // UINT scanCode = MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC);
        // std::cout << "The scan code for DOWN key is: " << scanCode << std::endl;
    }

    void test_run_004() {
        WORD scanCode = 0x32;
        bool isExtendedKey = false; // M 키는 확장 키가 아니다.
        PressKeyScan(scanCode, isExtendedKey);
        std::this_thread::sleep_for(std::chrono::milliseconds(70));
        ReleaseKeyScan(scanCode, isExtendedKey);
        std::this_thread::sleep_for(std::chrono::milliseconds(150));

        scanCode = 0x50;
        isExtendedKey = true;
        for (int i{0}; i < 3; ++i) {
            PressKeyScan(scanCode, isExtendedKey);
            std::this_thread::sleep_for(std::chrono::milliseconds(70));
            ReleaseKeyScan(scanCode, isExtendedKey);
            std::this_thread::sleep_for(std::chrono::milliseconds(70));
        }
    }

    void test_004() {
        cout << "Test_004 start" << endl;
        this_thread::sleep_for(chrono::seconds(3));

        cout << "key_press_test_threaded start" << endl;
        thread t1(test_run_004);
        t1.join();
        cout << "key_press_test_threaded end" << endl;
    }

}

