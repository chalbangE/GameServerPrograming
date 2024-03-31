#pragma once

#include "stdafx.h"
#include "ChessGame.h"
#include "Chess.h"


extern WSABUF wsabuf[1]; // �Ź� ȣ���ϴ� �ͺ��� �������� �����ؼ� ���
extern char buf[BUFSIZE];
extern SOCKET server_s;

// read_n_send ���� ���������� ������� �Լ��� ����Ǹ� �ּҰ��� �����ذ��� �ǹ̰� ����
extern WSAOVERLAPPED wsaover;

extern bool bshutdown; // ���� ���� ����

static void CALLBACK recv_callback(DWORD err, DWORD recv_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag)
{
	int p_size{ 0 };
	while (recv_size > p_size) {
		int m_size{ buf[p_size] };
		std::cout << "Player [" << static_cast<int>(buf[p_size + 1]) << "] : ";

		// ���� wsarecv ������ �ϴ� ���� ���⼭ ó������
		for (unsigned int i = 0; i < recv_size; ++i) {
			std::cout << buf[i + p_size + 2];
		}
		std::cout << std::endl;
		p_size += m_size;
	}
}

static void CALLBACK send_callback(DWORD err, DWORD sent_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag)
{
	// ���� wsasend ������ �ϴ� ���� ���⼭ ó������
	ZeroMemory(pwsaover, sizeof(pwsaover));   // recv���� ����ϱ� ���� �ʱ�ȭ
	wsabuf[0].len = BUFSIZE;
	DWORD recv_flag = 0;
	WSARecv(server_s, wsabuf, 1, nullptr, &recv_flag, pwsaover, recv_callback);
}