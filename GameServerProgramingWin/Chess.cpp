#include "Chess.h"

Chess::Chess()
{
}

Chess::Chess(const std::string& str, RECT rt) : drowimgsize(rt)
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

void Chess::Move(int add_x, int add_y)
{
	x = std::clamp(x + add_x, 0, 7);
	y = std::clamp(y + add_y, 0, 7);
}
