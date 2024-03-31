#pragma once

#include "ChessGame.h"
#include "Chess.h"
#include "ClientCallback.h"

extern WSABUF wsabuf[1]; // 매번 호출하는 것보다 전역으로 생성해서 사용
extern char buf[BUFSIZE];

// read_n_send 안의 지역변수로 했을경우 함수가 종료되면 주소값을 보내준것이 의미가 없음
extern WSAOVERLAPPED wsaover;

extern bool bshutdown; // 종료 조건 변수

ChessGame::ChessGame()
{
	// backgroud.Draw(mdc, 0, 0, 800, 800, 0, 0, 800, 800); // 기본 시작화면
	backgroud.Load(TEXT("IMG/Background.png"));

	w = backgroud.GetWidth();
	h = backgroud.GetHeight();
}

ChessGame::ChessGame(SOCKET& server) : sev_s{ server }
{
	std::cout << sev_s << "tid\n";

	// backgroud.Draw(mdc, 0, 0, 800, 800, 0, 0, 800, 800); // 기본 시작화면
	backgroud.Load(TEXT("IMG/Background.png"));

	w = backgroud.GetWidth();
	h = backgroud.GetHeight();
}

void ChessGame::Drow(HDC& mdc)
{
	backgroud.Draw(mdc, 0, 0, w, h, 0, 0, w, h);

	for (Chess* ch : chesses) {
		ch->Drow(mdc);
	}
}

void ChessGame::AddChess()
{
	const std::string str{ "IMG/Momonga" + std::to_string(chesses.size() + 1) + ".png"};
	RECT rt{ 0, 0, 100, 100 };
	Chess* ch = new Chess{ str, rt, int(chesses.size() + 1) };

	chesses.push_back(ch);
}

void ChessGame::MinusChess(int my_id)
{
	// chesses[my_id] = nullptr;
}

void ChessGame::Move(int my_id, int add_x, int add_y)
{
	chesses[my_id]->Move(add_x, add_y);
}

void ChessGame::Move()
{
	wsabuf[0].len = BUFSIZE;
	DWORD recv_size{};
	DWORD recv_flag{ 0 };
	int res = WSARecv(sev_s, wsabuf, 1, &recv_size, &recv_flag, nullptr, nullptr);
	if (res != 0)
		print_error("WSARecv", WSAGetLastError());

	std::cout << "x : " << wsabuf[0].buf[0] + 'a' << "y : " << wsabuf[0].buf[1] + 'a' << "\n";
	Move(0, wsabuf[0].buf[0], wsabuf[0].buf[1]);
}

void ChessGame::PressKey(WPARAM wParam)
{
	buf[0] = wParam;
	buf[1] = '\0';
	wsabuf[0].buf = buf;
	wsabuf[0].len = static_cast<int>(strlen(buf)) + 1; // 끝에 0까지 보내야함 \0
	if (wsabuf[0].len == 1) {
		bshutdown = true;
		return;
	}
	DWORD sent_size{};
	int sed = WSASend(sev_s, wsabuf, 1, &sent_size, 0, nullptr, nullptr);
	if (sed != 0)
		print_error("WSASend", WSAGetLastError());

	Move();
		
	//switch (wParam)
	//{
	//case VK_RIGHT: {
	//	Move(0, 1, 0);
	//	break;
	//}
	//case VK_LEFT: {
	//	Move(0, -1, 0);
	//	break;
	//}
	//case VK_UP: {
	//	Move(0, 0, -1);
	//	break;
	//}
	//case VK_DOWN: {
	//	Move(0, 0, 1);
	//	break;
	//}
	//case VK_ESCAPE: {
	//	exit(829);
	//}
	//default: {
	//	break;
	//}
	//}
}

