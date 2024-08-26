#include <chrono>
#include <iostream>
#include <conio.h>

#include "tgraphics.h"


int main(int argc, char* argv[])
{
	auto fullSize = GetLargestConsoleWindowSize(GetStdHandle(STD_OUTPUT_HANDLE));

	TG::Graphics g{ COORD{360, 120}, argc, argv };

	typedef std::chrono::high_resolution_clock clock;
	std::chrono::steady_clock::time_point begin{};
	std::chrono::steady_clock::time_point end{};
	std::chrono::duration<float> elapsed{};
	
	while (true)
	{
		begin = clock::now();
		g.Draw(elapsed.count());
		end = clock::now();
		elapsed = (end - begin);

		if(_kbhit()) 
		{
			break;
		}
	}

	return 0;
}