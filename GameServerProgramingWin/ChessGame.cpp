#pragma once

#include "stdafx.h"
#include "ChessGame.h"
#include "Chess.h"
#include "define.h"
#include <cassert>

void CALLBACK recv_callback(DWORD err, DWORD recv_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag);
void CALLBACK send_callback(DWORD err, DWORD sent_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag);

extern WSABUF recv_wsabuf[1]; 
extern WSABUF send_wsabuf[1];
extern char buf[BUFSIZE];

// read_n_send ���� ���������� ������� �Լ��� ����Ǹ� �ּҰ��� �����ذ��� �ǹ̰� ����
extern WSAOVERLAPPED wsaover;

extern bool bshutdown; // ���� ���� ����

extern ChessGame chessgame;

static void print_error(const char* msg, int err_no);

ChessGame::ChessGame()
{
	// backgroud.Draw(mdc, 0, 0, 800, 800, 0, 0, 800, 800); // �⺻ ����ȭ��
	backgroud.Load(TEXT("IMG/Background.png"));

	w = backgroud.GetWidth();
	h = backgroud.GetHeight();
}

ChessGame::ChessGame(SOCKET& server) : sev_s{ server }
{ }

void ChessGame::init(SOCKET& server)
{
	sev_s = server;
	recv_wsabuf[0].buf = reinterpret_cast<char*>(&pos_p);
	recv_wsabuf[0].len = sizeof(POS_PACKET);
	send_wsabuf[0].buf = reinterpret_cast<char*>(&key_p);
	send_wsabuf[0].len = sizeof(KEY_PACKET);

	if (backgroud.IsNull()) {
		backgroud.Load(TEXT("IMG/Background.png"));

		w = backgroud.GetWidth();
		h = backgroud.GetHeight();
	}
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
	ZeroMemory(&wsaover, sizeof(wsaover));   // recv���� ����ϱ� ���� �ʱ�ȭ
	memset(&wsaover, 0, sizeof(wsaover));
	int sed = WSASend(sev_s, send_wsabuf, 1, nullptr, 0, &wsaover, send_callback);
	if (0 != sed) {
		int err_no = WSAGetLastError();
		// ���� ��ģ i/o �۾��� �����ϰ� �ֽ��ϴ�. ��� ������ �� ������
		if (WSA_IO_PENDING != err_no)
			print_error("AddChess - WSASend", WSAGetLastError());
	}

	while (1) {
		do_recv();

		std::cout << "pos_p.id : " << pos_p.id << std::endl;

		if (pos_p.id != -1) {
			const std::string str{ "IMG/Momonga" + std::to_string(pos_p.id + 1) + ".png" };
			RECT rt{ 0, 0, 100, 100 };
			Chess* ch = new Chess{ str, rt, pos_p.id };
			chesses.push_back(ch);
			chesses[pos_p.id]->Move(pos_p.x, pos_p.y);
		}
		else
			break;

	} 

	id = chesses.size() - 1;
	std::cout << "����� ü�� ���� : " << chesses.size() << std::endl;
}

void ChessGame::MinusChess(int my_id)
{
	// chesses[my_id] = nullptr;
}

void ChessGame::Move()
{
	DWORD recv_flag{ 0 };

	do_recv();

	std::cout << "Move - x : " << pos_p.x << "   y : " << pos_p.y << "\n";
}

void ChessGame::PressKey(WPARAM wParam)
{
	key_p.type = KEY_TYPE;
	key_p.key = wParam;
	DWORD sent_size{};

	int sed = WSASend(sev_s, send_wsabuf, 1, &sent_size, 0, &wsaover, send_callback);
	if (0 != sed) {
		int err_no = WSAGetLastError();
		// ���� ��ģ i/o �۾��� �����ϰ� �ֽ��ϴ�. ��� ������ �� ������
		if (WSA_IO_PENDING != err_no)
			print_error("PressKey - WSASend", WSAGetLastError());
	}

	Move();
}

void ChessGame::do_recv()
{
	DWORD recv_flag{ 0 };
	int res = WSARecv(sev_s, recv_wsabuf, 1, nullptr, &recv_flag, &wsaover, recv_callback);
	if (0 != res) {
		int err_no = WSAGetLastError();
		// ���� ��ģ i/o �۾��� �����ϰ� �ֽ��ϴ�. ��� ������ �� ������
		if (WSA_IO_PENDING != err_no)
			print_error("WSARecv", WSAGetLastError());
	}
}

static void print_error(const char* msg, int err_no)
{
	WCHAR* msg_buf{};
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&msg_buf), 0, NULL);
	std::cout << msg;
	std::wcout << L"\t���� : " << msg_buf;
	while (true);
	//#ifdef _DEBUG
	//assert(false);
	//#endif _DEBUG
	LocalFree(msg_buf);
}