#pragma once
#include "stdafx.h"

class Chess
{
	RECT imgsize{};
	RECT drowimgsize{};
	int x{}, y{}, id{};
	CImage img{};

public:
	Chess();
	Chess(const std::string& str, RECT rt, int id);
	void Drow(HDC& mdc);
	void Move(int x, int y);
};

