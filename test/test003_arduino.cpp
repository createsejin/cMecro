//
// Created by bae on 2024-01-02.
//
#include <iostream>
#include <chrono>
#include <windows.h>
using namespace std;
namespace test_arduino {
    // 시리얼 포트를 열고 핸들을 반환하는 함수
    HANDLE openSerialPort(const LPCSTR portName, const DWORD baudRate) {
        // Convert LPCSTR to LPCWSTR
        const int len = MultiByteToWideChar(CP_ACP, 0, portName, -1, nullptr, 0);
        auto* wPortName = new wchar_t[len];
        MultiByteToWideChar(CP_ACP, 0, portName, -1, wPortName, len);

        HANDLE const& hSerial = CreateFile(wPortName, GENERIC_WRITE, 0, nullptr,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hSerial == INVALID_HANDLE_VALUE) {
            std::cerr << "Error opening serial port\n";
            return INVALID_HANDLE_VALUE;
        }

        DCB dcbSerialParams = {0};
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        if (!GetCommState(hSerial, &dcbSerialParams)) {
            std::cerr << "Error getting state of serial port\n";
            CloseHandle(hSerial);
            return INVALID_HANDLE_VALUE;
        }

        dcbSerialParams.BaudRate = baudRate;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;

        if (!SetCommState(hSerial, &dcbSerialParams)) {
            std::cerr << "Error setting state of serial port\n";
            CloseHandle(hSerial);
            return INVALID_HANDLE_VALUE;
        }

        return hSerial;
    }

    // 시리얼 포트에 데이터를 쓰는 함수
    void writeSerialPort(HANDLE const& hSerial, const LPCVOID data, const DWORD dataSize) {
        DWORD bytesWritten;
        if (!WriteFile(hSerial, data, dataSize, &bytesWritten, nullptr)) {
            std::cerr << "Error writing to serial port\n";
        }
    }

    int send_data_to_arduino(HANDLE const& hSerial, const char* data) {
        if (hSerial != INVALID_HANDLE_VALUE) {
            const DWORD dataSize = strlen(data);
            writeSerialPort(hSerial, data, dataSize);
            return 0;
        }
        return 1;
    }

    void test002() {
        cout << "start test002" << endl;
        Sleep(3000);
        cout << "start time measurment" << endl;
        // Start time measurement
        const auto start = std::chrono::high_resolution_clock::now();
        HANDLE const& hSerial = openSerialPort("\\\\.\\COM7", CBR_9600);
        if (hSerial != INVALID_HANDLE_VALUE) {
            std::cout << "Successfully opened COM7.\n";
        } else {
            std::cerr << "Failed to open COM7.\n";
        }

        for (int i{0}; i < 3; ++i) {
            send_data_to_arduino(hSerial, "KDAD");
            Sleep(100);
            send_data_to_arduino(hSerial, "KUAD");
            Sleep(100);
        }
        cout << "complete" << endl;

        // End time measurement and calculate the duration
        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> diff = end - start;
        std::cout << "Execution time: " << diff.count() << " s\n";

        if (hSerial != INVALID_HANDLE_VALUE) CloseHandle(hSerial);
    }
}