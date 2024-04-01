#pragma once

#include "stdafx.h"
#include "Chess.h"
#include "define.h"

extern WSABUF recv_wsabuf[1];
extern WSABUF send_wsabuf[1];
extern char buf[BUFSIZE];
extern SOCKET server_s;

// read_n_send ���� ���������� ������� �Լ��� ����Ǹ� �ּҰ��� �����ذ��� �ǹ̰� ����
extern WSAOVERLAPPED wsaover;

extern bool bshutdown; // ���� ���� ����

Chess::Chess()
{
}

Chess::Chess(const std::string& str, RECT rt, int id) : drowimgsize(rt), id(id)
{
	std::wstring wstr{ L"" };

	wstr.assign(str.begin(), str.end());

	img.Load(wstr.c_str());

	imgsize.right = img.GetWidth();
	imgsize.bottom = img.GetHeight();
}

void Chess::Drow(HDC& mdc)
{
	img.Draw(mdc, x * drowimgsize.right, y * drowimgsize.bottom, drowimgsize.right, drowimgsize.bottom, 0, 0, imgsize.right, imgsize.bottom);
}

void Chess::Move(int change_x, int change_y)
{
	x = change_x;
	y = change_y;
}
