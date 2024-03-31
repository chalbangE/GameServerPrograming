#if defined(DEBUG) | defined(_DEBUG) 
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") 
#endif
#pragma once

#include "stdafx.h"
#include "ChessGame.h"
#include "Chess.h"
#include "ClientCallback.h"

extern WSABUF wsabuf[1]; // �Ź� ȣ���ϴ� �ͺ��� �������� �����ؼ� ���
extern char buf[BUFSIZE];
extern SOCKET server_s;

// read_n_send ���� ���������� ������� �Լ��� ����Ǹ� �ּҰ��� �����ذ��� �ǹ̰� ����
extern WSAOVERLAPPED wsaover;

extern bool bshutdown; // ���� ���� ����

HINSTANCE g_hinst;
LPCTSTR IpszClass = L"Window Programming Lap";
LPCTSTR IpszWindowName = L"Window Programming Lap";

LRESULT CALLBACK WndProc(HWND hEnd, UINT iMessage, WPARAM wParam, LPARAM IParam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevinstance, LPSTR IpszCmdParam, int nCmdShow)
{
	// ------- ���� ���̱� -------------------
	std::wcout.imbue(std::locale("korean")); // �ѱ۷� ���� ���

	// �����쿡���� �ؾ��ϴ� ����. ������ ���� ���α׷����� ȣȯ�� ������ �ʿ���
	// �������� �濪�� ����
	WSADATA WSAData{};
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	server_s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);
	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);

	connect(server_s, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	std::cout << server_s << '\n';

	// ------ ���� �ʱ⼳�� ---------------

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

	hWnd = CreateWindow(IpszClass, L"ChessGame", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, 0, 0, 800, 800, NULL, (HMENU)NULL, hinstance, NULL);
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
	static ChessGame chessgame{ server_s };

	// �޼��� ó���ϱ�
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

		closesocket(server_s);
		WSACleanup();

		PostQuitMessage(0);
		break;
	}
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, IParam);
}

/*	while (1) {
		char buf[BUFSIZE]{};
		while (1) {
			if (_kbhit()) {        //Ű���� �Է� Ȯ�� (true / false)
				char dir = _getch();      // ����Ű �Է½� 224 --�� ������ �Ǳ⿡ �տ� �ִ� �� 224�� ����
				if (dir == -32) {    // -32�� �ԷµǸ�
					dir = _getch();  // ���� �Է°��� �Ǻ��Ͽ� �����¿� ���
					buf[0] = dir;
					buf[1] = '\0';
					break;
				}
			}
		}

		MoveCursor();

		// send
		WSABUF wsabuf[1];
		wsabuf[0].buf = buf;
		wsabuf[0].len = static_cast<int>(strlen(buf)) + 1; // ���� 0���� �������� \0
		if (wsabuf[0].len == 1)
			break;
		DWORD sent_size{};
		int sed = WSASend(server_s, wsabuf, 1, &sent_size, 0, nullptr, nullptr);
		if (sed != 0)
			print_error("WSASend", WSAGetLastError());

		// recv
		wsabuf[0].len = BUFSIZE;
		DWORD recv_size{};
		DWORD recv_flag{ 0 };
		int res = WSARecv(server_s, wsabuf, 1, &recv_size, &recv_flag, nullptr, nullptr);		
		if (res != 0)
			print_error("WSARecv", WSAGetLastError());
		Chess hos{ wsabuf[0].buf[0],  wsabuf[0].buf[1] };
		PrintBoard(hos);
	}*/