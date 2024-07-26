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
	TG::Graphics g{ COORD{80, 40} };

	typedef std::chrono::high_resolution_clock clock;
	std::chrono::steady_clock::time_point begin{};
	std::chrono::steady_clock::time_point end{};

	std::chrono::duration<float> elapsed{};

	//g.DrawLine(COORD{ 10, 10 }, COORD{ 12, 0 });
	//g.DrawLine(COORD{ 10, 10 }, COORD{ 17, 5 });
	//g.DrawLine(COORD{ 10, 10 }, COORD{ 20, 10 });
	//g.DrawLine(COORD{ 10, 10 }, COORD{ 17, 15 });
	//g.DrawLine(COORD{ 10, 10 }, COORD{ 10, 20 });
	//g.DrawLine(COORD{ 10, 10 }, COORD{ 3, 15 });
	//g.DrawLine(COORD{ 10, 10 }, COORD{ 0, 10 });
	//g.DrawLine(COORD{ 10, 10 }, COORD{ 3, 5 });

	while (true)
	{
		begin = clock::now();
		g.Draw(elapsed.count());
		end = clock::now();
		elapsed = (end - begin);
	}

	std::cin.get();
}