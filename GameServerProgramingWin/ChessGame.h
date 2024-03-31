#pragma once
#include "stdafx.h"
class Chess;

class ChessGame
{
	int w{}, h{};
	CImage backgroud{};
	std::vector<Chess*> chesses;
	SOCKET sev_s;

public: 
	ChessGame();
	ChessGame(SOCKET& server);
	void Drow(HDC&);
	void AddChess();
	void MinusChess(int my_id);
	void Move();
	void Move(int my_id, int add_x, int add_y);
	void PressKey(WPARAM wParam);
};

