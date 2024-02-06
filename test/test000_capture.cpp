//
// Created by creat on 2023-12-25.
//
#include "test000_capture.h"
#include <format>
#include <windows.h>
#include <gdiplus.h>
#include <iostream>
#include <vector>
#include <tuple>
using namespace std;

// 특정 영역의 화면을 캡쳐하고 HBITMAP 객체를 반환하는 함수
// 여기서 특정 영역은 RECT 객체로 지정한다. 지정 함수는 set_view_port 함수를 사용한다.
HBITMAP CaptureScreen(HWND const& hWnd, RECT const& captureRect) {
    HDC const &hScreen = GetDC(hWnd);
    HDC const &hDC = CreateCompatibleDC(hScreen);
    HBITMAP const &hBitmap = CreateCompatibleBitmap(
        hScreen, captureRect.right - captureRect.left, captureRect.bottom - captureRect.top);
    SelectObject(hDC, hBitmap);

    // 캡처할 영역 설정
    BitBlt(hDC, 0, 0,
        captureRect.right - captureRect.left, captureRect.bottom - captureRect.top,
        hScreen, captureRect.left, captureRect.top, SRCCOPY);

    // clean up
    DeleteDC(hDC);
    ReleaseDC(nullptr, hScreen);

    // hBitmap에는 캡처된 이미지 데이터가 들어 있음.
    return hBitmap;
}

// HBITMAP 객체를 PNG 파일로 저장하는 함수
// 이 함수를 사용하기 위해서는 반드시 Gdiplus::GdiplusStartupInput 객체를 생성하고
// 사용 후에는 Gdiplus::GdiplusShutdown 함수를 호출하여 memory clean up을 해야 한다.
void SaveHBITMAPToPNG(HBITMAP const& hBitmap, const wstring& filename) {
    Gdiplus::Bitmap bitmap(hBitmap, nullptr);
    CLSID pngClsid;
    if (const HRESULT hr = CLSIDFromString(L"{557CF406-1A04-11D3-9A73-0000F81EF32E}", &pngClsid); SUCCEEDED(hr)) {
        bitmap.Save(filename.c_str(), &pngClsid, nullptr);
    } else {
        // Handle error
        wcerr << L"Failed to get CLSID for PNG format. Error: " << hr << endl;
    }
}

// PNG 파일을 Gdiplus::Bitmap 객체로 변환하는 함수
// 이 함수를 사용하기 위해서는 반드시 Gdiplus::GdiplusStartupInput 객체를 생성하고
// 사용 후에는 Gdiplus::GdiplusShutdown 함수를 호출하여 memory clean up을 해야 한다.
Gdiplus::Bitmap* LoadPNGToBitmap(const wstring& filename) {
    return new Gdiplus::Bitmap(filename.c_str());
}

/* 특정 target color의 좌표를 찾아주는 유틸리티 함수다.
사용법은 캡쳐된 이미지를 그림판으로 열어서 특정 색상(예를들면 빨간색)으로 마킹한 후
해당 이미지를 LoadPNGToBitmap 함수를 사용하여 Gdiplus::Bitmap 객체로 변환한 후
FindColorInBitmap 함수를 사용하여 특정 색상의 좌표를 찾는다. */
vector<pair<int, int>> FindColorInBitmap(Gdiplus::Bitmap& bitmap, const Gdiplus::Color targetColor) {
    // 찾으려는 픽셀 색상의 좌표를 저장할 벡터
    vector<pair<int, int>> coordinates;

    for (int y = 0; y < bitmap.GetHeight(); ++y) {
        for (int x = 0; x < bitmap.GetWidth(); ++x) {
            // 임의의 pixelColor 객체를 선언한다.
            Gdiplus::Color pixelColor;
            // 현재 탐색중인 x, y 좌표상의 픽셀 색상을 가져온다.
            bitmap.GetPixel(x, y, &pixelColor);

            // 만약 현재 탐색중인 픽셀의 색상이 targetColor와 같다면, 해당 좌표를 coordinates에 추가한다.
            if (pixelColor.GetValue() == targetColor.GetValue()) {
                coordinates.emplace_back(x, y);
            }
        }
    }

    return coordinates;
}

// 생성된 HBITMAP 객체에서 특정 좌표의 RGB 값을 추출하는 함수
tuple<int, int, int> GetPixelColorFromHBITMAP(HBITMAP const& hBitmap, const int x, const int y) {
    BITMAP bitmap;
    GetObject(hBitmap, sizeof(BITMAP), &bitmap);

    // 비트맵의 픽셀 데이터에 접근
    const auto* pixels = static_cast<COLORREF *>(bitmap.bmBits);

    // 특정 픽셀의 색상 데이터에 접근
    const COLORREF color = pixels[y * bitmap.bmWidth + x];
    // 아마도 COLORREF는 1차원 배열의 형태로 데이터가 저장되어 있는 것 같다.

    // 색상 데이터에서 RGB 값을 추출
    int red = GetRValue(color);
    int green = GetGValue(color);
    int blue = GetBValue(color);

    return make_tuple(red, green, blue);
}

// 캡처할 영역을 설정한 RECT 객체를 반환하는 함수
RECT set_view_port(const int left, const int top, const int right, const int bottom) {
    RECT rect;
    rect.left = left;
    rect.top = top;
    rect.right = right;
    rect.bottom = bottom;
    return rect;
}

// BitBlt로 화면 캡쳐 및 파일 저장 테스트
namespace test_capture {
    void test000() {
        // 파일 저장을 위한 것이다. 인스턴스를 위해서는 필요가 없다.
        const Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

        // 캡처할 영역 설정 : 메뉴 아이템을 캡쳐하는 뷰포인트를 설정한다.
        const RECT captureRect = set_view_port(505, 162, 665, 549);

        // bitblt으로 메뉴 화면 일부를 캡쳐한다.
        HBITMAP const& hbitmap = CaptureScreen(GetDesktopWindow(), captureRect);
        // test 이미지를 저장한다.
        SaveHBITMAPToPNG(hbitmap, LR"|(img\test.png)|");
        cout << "test.png saved" << endl;

        // gdiplus api clean up
        Gdiplus::GdiplusShutdown(gdiplusToken);
    }

    /*void test001() { // pixel RGB picking test
        // 캡처할 영역 설정 : 메뉴 아이템을 캡쳐하는 뷰포인트를 설정한다.
        const RECT captureRect = set_view_port(505, 162, 665, 549);

        // bitblt으로 메뉴 화면 일부를 캡쳐한다.
        HBITMAP const& hbitmap = CaptureScreen(GetDesktopWindow(), captureRect);


    }*/

    /* Red color로 된 마커를 이용해 캡쳐한 bitmap의 이미지에서 좌표를 찾고 다시 원본파일에서 해당
    좌표를 통해 RGB 값을 추출하고 적절한 자료구조에 저장한다. 이것은 추후 함수화 시킬 필요가 있다. */
    void test002() {
        // 파일 저장을 위한 것이다. 인스턴스를 위해서는 필요가 없다.
        // 추후 캡쳐 이미지를 저장하여 사용할 경우 이러한 토큰을 한 번만 생성하도록 코드를 짜야할 것 같다.
        const Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

        // LoadPNGToBitmap 함수를 사용하여 Gdiplus::Bitmap 객체를 생성
        const unique_ptr<Gdiplus::Bitmap> bitmap(LoadPNGToBitmap(LR"|(img\test - 복사본.png)|"));

        // FindColorInBitmap 함수를 사용하여 특정 target 색상의 좌표를 찾는다. 여기서는 (255, 0, 0) 색상을 찾는다.
        const Gdiplus::Color targetColor(255, 0, 0);
        // ReSharper disable once CppTooWideScopeInitStatement
        const vector<pair<int, int>> coordinates = FindColorInBitmap(*bitmap, targetColor);

        // 찾은 좌표를 출력 test 한다.
        cout << format("Found {} target colors", coordinates.size()) << endl;
        for (const auto& [x, y] : coordinates) {
            cout << "Found target color at (" << x << ", " << y << ")" << endl;
        }

        // gdiplus api clean up
        Gdiplus::GdiplusShutdown(gdiplusToken);
    }
}
