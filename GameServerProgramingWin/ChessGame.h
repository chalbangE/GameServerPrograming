#pragma once
#include "stdafx.h"

class Chess;

class ChessGame
{
public: 
	int w{}, h{}, id{};
	CImage backgroud{};
	std::vector<Chess*> chesses;
	SOCKET sev_s{};

	KEY_PACKET key_p{ 0 };
	POS_PACKET pos_p{ 3 };

	ChessGame();
	ChessGame(SOCKET& server);
	void init(SOCKET& server);
	void Drow(HDC&);
	void AddChess();
	void MinusChess(int my_id);
	void Move();
	void PressKey(WPARAM wParam);
	void do_recv();
};

