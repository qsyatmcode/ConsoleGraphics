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

	TG::Graphics g{ COORD{180, 60} };

	typedef std::chrono::high_resolution_clock clock;
	std::chrono::steady_clock::time_point begin{};
	std::chrono::steady_clock::time_point end{};

	std::chrono::duration<float> elapsed{};

	//g.DrawLine(COORD{ 10, 10 }, COORD{ 12, 0 });

	TG::Triangle t0 = { { TG::Vector3{50, 5}, TG::Vector3{25, 50}, TG::Vector3{75, 50} }};
	//TG::Triangle t1 = { {TG::Vector3{180, 50},  TG::Vector3{150, 1},   TG::Vector3{70, 180} }};
	//TG::Triangle t2 = { {TG::Vector3{180, 150}, TG::Vector3{120, 160}, TG::Vector3{130, 180} }};

	//g.DrawTriangle(t0);
	//g.DrawTriangle(t1);
	//g.DrawTriangle(t2);

	//g.DrawTriangle(TG::Triangle{
	//	{
	//		TG::Vector3{20, 20},
	//		TG::Vector3{5, 40},
	//		TG::Vector3{25, 40},
	//	}
	//});
	
	
	///printw("000");
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