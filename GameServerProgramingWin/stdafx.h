#pragma once
#include <random>
#include <time.h>
#include <iostream>
#include <atlImage.h>
#include <string>
#include <vector>
#include <algorithm>
#include <conio.h>
#include <array>

#include <Windows.h>
#include <tchar.h>
#include <WS2tcpip.h>
#pragma comment (lib, "WS2_32.LIB") // 32 쓰는 이유는 16비트 쓰던 구시절의 잔재

constexpr short SERVER_PORT{ 2551 };
constexpr int BUFSIZE{ 256 };
constexpr char SERVER_ADDR[] = "127.0.0.1";

#define UP 38
#define DOWN 40
#define LEFT 37
#define RIGHT 39

static WSABUF wsabuf[1]{}; // 매번 호출하는 것보다 전역으로 생성해서 사용
static char buf[BUFSIZE]{};
static SOCKET server_s{};

// read_n_send 안의 지역변수로 했을경우 함수가 종료되면 주소값을 보내준것이 의미가 없음
static WSAOVERLAPPED wsaover{};

static bool bshutdown{ false }; // 종료 조건 변수


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