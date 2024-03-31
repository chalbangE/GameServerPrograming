#pragma once

#include "stdafx.h"
#include "ChessGame.h"
#include "Chess.h"


extern WSABUF wsabuf[1]; // 매번 호출하는 것보다 전역으로 생성해서 사용
extern char buf[BUFSIZE];
extern SOCKET server_s;

// read_n_send 안의 지역변수로 했을경우 함수가 종료되면 주소값을 보내준것이 의미가 없음
extern WSAOVERLAPPED wsaover;

extern bool bshutdown; // 종료 조건 변수

static void CALLBACK recv_callback(DWORD err, DWORD recv_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag)
{
	int p_size{ 0 };
	while (recv_size > p_size) {
		int m_size{ buf[p_size] };
		std::cout << "Player [" << static_cast<int>(buf[p_size + 1]) << "] : ";

		// 원래 wsarecv 끝나고 하는 것을 여기서 처리해줌
		for (unsigned int i = 0; i < recv_size; ++i) {
			std::cout << buf[i + p_size + 2];
		}
		std::cout << std::endl;
		p_size += m_size;
	}
}

static void CALLBACK send_callback(DWORD err, DWORD sent_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag)
{
	// 원래 wsasend 끝나고 하는 것을 여기서 처리해줌
	ZeroMemory(pwsaover, sizeof(pwsaover));   // recv에서 사용하기 전에 초기화
	wsabuf[0].len = BUFSIZE;
	DWORD recv_flag = 0;
	WSARecv(server_s, wsabuf, 1, nullptr, &recv_flag, pwsaover, recv_callback);
}