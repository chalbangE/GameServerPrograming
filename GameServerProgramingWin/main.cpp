#if defined(DEBUG) | defined(_DEBUG) 
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") 
#endif
#pragma once

#include "stdafx.h"
#include "ChessGame.h"
#include "Chess.h"
#include "define.h"

extern WSABUF wsabuf[1]; // 매번 호출하는 것보다 전역으로 생성해서 사용
extern char buf[BUFSIZE];
extern SOCKET server_s;

// read_n_send 안의 지역변수로 했을경우 함수가 종료되면 주소값을 보내준것이 의미가 없음
extern WSAOVERLAPPED wsaover;

ChessGame chessgame{};

extern bool bshutdown; // 종료 조건 변수
static void print_error(const char* msg, int err_no);

HINSTANCE g_hinst;
LPCTSTR IpszClass = L"Window Programming Lap";
LPCTSTR IpszWindowName = L"Window Programming Lap";

LRESULT CALLBACK WndProc(HWND hEnd, UINT iMessage, WPARAM wParam, LPARAM IParam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevinstance, LPSTR IpszCmdParam, int nCmdShow)
{
	// ------- 서버 붙이기 -------------------
	std::wcout.imbue(std::locale("korean")); // 한글로 오류 출력

	// 윈도우에서만 해야하는 삽질. 옛날에 만든 프로그램과의 호환성 때문에 필요함
	// 빌게이츠 흑역사 ㅋㅋ
	WSADATA WSAData{};
	int err = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (0 != err) {
		print_error("WSAStartup", WSAGetLastError());
	}

	server_s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr; 
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);

	connect(server_s, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));

	// ------ 윈프 초기설정 ---------------

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


	// 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE: {
		chessgame.init(server_s);

		AdjustWindowRect(&window, WS_OVERLAPPEDWINDOW, false);         
		MoveWindow(hWnd, 150, 70, window.right - window.left, window.bottom - window.top, false);

		chessgame.AddChess();

		SetTimer(hWnd, 0, 30, NULL);

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
		SleepEx(1, TRUE);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	case WM_KEYDOWN: {
		chessgame.PressKey(wParam);

		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	case WM_DESTROY: {
		chessgame.MinusChess();

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


void CALLBACK recv_callback(DWORD err, DWORD recv_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag)
{
	switch (chessgame.pos_p.type)
	{
	case NEW_LOGIN_TYPE: {
		if (chessgame.pos_p.id != -1 && chessgame.id != chessgame.pos_p.id) {
			const std::string str{ "IMG/Momonga" + std::to_string(chessgame.pos_p.id + 1) + ".png" };
			RECT rt{ 0, 0, 100, 100 };
			Chess* ch = new Chess{ str, rt, chessgame.pos_p.id };
			chessgame.chesses.try_emplace(chessgame.pos_p.id, ch);
			chessgame.chesses[chessgame.pos_p.id]->Move(chessgame.pos_p.x, chessgame.pos_p.y);
		}
		std::cout << "현재 체스는 몇 개? : " << chessgame.chesses.size() << std::endl;

		break;
	}
	case NEW_LOGOUT_TYPE: {
		chessgame.chesses[chessgame.pos_p.id] = nullptr;
		break;
	}
	case POS_TYPE: {
		if (chessgame.pos_p.id != -1) {
			chessgame.chesses[chessgame.pos_p.id]->Move(chessgame.pos_p.x, chessgame.pos_p.y);
		}
		break;
	}
	default:
		break;
	}

	chessgame.do_recv();
}

void CALLBACK send_callback(DWORD err, DWORD sent_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag)
{ }

static void print_error(const char* msg, int err_no)
{
	WCHAR* msg_buf{};
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&msg_buf), 0, NULL);
	std::cout << msg;
	std::wcout << L"\t에러 : " << msg_buf;
	while (true);
	LocalFree(msg_buf);
}