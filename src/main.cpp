#include <chrono>
#include <iostream>

#include "tgraphics.h"

void Run();

int main()
{

	Run();

	return 0;
}

void Run()
{
	auto fullSize = GetLargestConsoleWindowSize(GetStdHandle(STD_OUTPUT_HANDLE));

	TG::Graphics g{ COORD{180, 50} };

	typedef std::chrono::high_resolution_clock clock;
	std::chrono::steady_clock::time_point begin{};
	std::chrono::steady_clock::time_point end{};

	std::chrono::duration<float> elapsed{};

	//g.DrawLine(COORD{ 10, 10 }, COORD{ 12, 0 });

	//g.DrawTriangle(TG::Triangle{
	//	{
	//		TG::Vector3{5, 5},
	//		TG::Vector3{20, 20},
	//		TG::Vector3{50, 7},
	//	}
	//});

	//refresh();

	while (true)
	{
		begin = clock::now();
		g.Draw(elapsed.count());
		end = clock::now();
		elapsed = (end - begin);
	}

	std::cin.get();
}