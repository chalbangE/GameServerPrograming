#if defined(DEBUG) | defined(_DEBUG) 
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") 
#endif

#include "stdafx.h"
#include "ChessGame.h"
#include "Chess.h"

#include <Windows.h>
#include <tchar.h>

HINSTANCE g_hinst;
LPCTSTR IpszClass = L"Window Programming Lap";
LPCTSTR IpszWindowName = L"Window Programming Lap";

LRESULT CALLBACK WndProc(HWND hEnd, UINT iMessage, WPARAM wParam, LPARAM IParam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevinstance, LPSTR IpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hinst = hinstance;

	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hinstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = IpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);

	hWnd = CreateWindow(IpszClass, L"ChessGame", WS_OVERLAPPEDWINDOW, 0, 0, 800, 800, NULL, (HMENU)NULL, hinstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM IParam)
{
	srand(time(NULL));

	PAINTSTRUCT ps;
	HDC hdc{}; HDC mdc{};
	HBITMAP HBitmap, OldBitmap;
	RECT window{ 0, 0, 800, 800 };
	static ChessGame chessgame;

	// 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE: {
		AdjustWindowRect(&window, WS_OVERLAPPEDWINDOW, false);         
		MoveWindow(hWnd, 150, 70, window.right - window.left, window.bottom - window.top, false);

		chessgame.AddChess();

		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	case WM_SIZE:
	case WM_MOVE: {
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	case WM_PAINT: {
		hdc = BeginPaint(hWnd, &ps);
		mdc = CreateCompatibleDC(hdc);
		HBitmap = CreateCompatibleBitmap(hdc, window.right, window.bottom);
		OldBitmap = (HBITMAP)SelectObject(mdc, (HBITMAP)HBitmap);
		FillRect(mdc, &window, WHITE_BRUSH);

		chessgame.Drow(mdc);

		BitBlt(hdc, 0, 0, window.right, window.bottom, mdc, 0, 0, SRCCOPY);

		DeleteDC(mdc);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_TIMER: {
		break;
	}
	case WM_KEYDOWN: {
		chessgame.PressKey(wParam);

		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, IParam);
}