#pragma once

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <io.h> // for _setmode()
#include <fcntl.h>
#include <vector>
#include <utility> // for std::pair

#include "curses.h"

//#define NEW_OBJ // Load new quad model or old poly
#undef  NEW_OBJ

namespace TG
{
	// █
	constexpr wchar_t full_block_char{ L'\u2588' };
	// ▓
	constexpr wchar_t dark_shade{ L'\u2593' };
	// ▒
	constexpr wchar_t medium_shade{ L'\u2592' };
	// ░
	constexpr wchar_t light_shade{ L'\u2591' };

	struct Point2
	{
		int x{}, y{};

		constexpr Point2() = default;

		const Point2& operator-(const Point2& rhs) const
		{
			return Point2{ x - rhs.y, y - rhs.y};
		}
		const Point2& operator+(const Point2& rhs) const
		{
			return Point2{ x + rhs.y, y + rhs.y};
		}

		const Point2& operator*(int rhs) const
		{
			return Point2{ x * rhs, y * rhs};
		}
	};

	struct Matrix4; // for friend operator+
	struct Vector3
	{
		float x{}, y{}, z{};

		constexpr Vector3() = default;

		float Length() const
		{
			return sqrtf(x * x + y * y + z * z);
		}

		const Vector3& Normalize() const
		{
			float l = Length();
			return {
			x / l,
			y / l,
			z / l
			};
		}

		Vector3 operator-(const Vector3& rhs) const
		{
			return Vector3{ x - rhs.y, y - rhs.y, z - rhs.z };
		}
		Vector3 operator+(const Vector3& rhs) const
		{
			return Vector3{ x + rhs.y, y + rhs.y, z + rhs.z };
		}

		Vector3 operator*(float rhs) const
		{
			return Vector3{ x * rhs, y * rhs, z * rhs };
		}

		friend Vector3 operator*(const Matrix4& mat, const Vector3& vec);
		friend Vector3 operator*(const Vector3& vec, const Matrix4& mat);
	};
	struct Matrix4
	{
		float m[4][4]{};

		friend Vector3 operator*(const Matrix4& mat, const Vector3& vec);
		friend Vector3 operator*(const Vector3& vec, const Matrix4& mat);
	};

	struct Triangle
	{
		Vector3 verts[3]{};

		const char* filler{ "?" };
	};

	class Mesh
	{
	public:
		enum ReadMode
		{
			RM_OLD,
			RM_NEW,
			RM_ERROR
		} ModelReadMode;

		std::vector<Triangle> Tris{};

		constexpr Mesh() = default;

		explicit Mesh(const char* fileName, const char* mode = "old")
		{
			if(strcmp(mode, "old") == 0)
			{
				ModelReadMode = RM_OLD;
			}else if(strcmp(mode, "new") == 0)
			{
				ModelReadMode = RM_NEW;
			}else
			{
				ModelReadMode = RM_ERROR;
			}

			std::vector<Vector3> verts{};

			std::ifstream file(fileName);
			if (!file)
			{
				throw std::exception("File is not open");
			}

			for (std::string line; std::getline(file, line);)
			{
				std::stringstream s;
				s << line;

				//line.replace()

				char junk{ };
				if (line[0] == 'v' && line[1] == ' ') // Vert
				{
					Vector3 v{};
					s >> junk >> v.x >> v.y >> v.z;
					verts.push_back(v);
				}
				if (line[0] == 'f') // face
				{
//#ifdef NEW_OBJ
					if (ModelReadMode == RM_NEW) {
						int ti[4]{}; // vert indexes (quad)
						int index{ 0 };
						std::string token{};
						s >> token; // skip 'f'
						while (s >> token)
						{
							size_t slashPos = token.find('/');
							if (slashPos != std::string::npos) {
								std::string numberStr = token.substr(0, slashPos);
								ti[index] = std::stoi(numberStr);
								index++;
							}
						}
						Tris.emplace_back(Triangle{ verts[ti[0] - 1], verts[ti[2] - 1], verts[ti[3] - 1] }); // first triangle
						Tris.emplace_back(Triangle{ verts[ti[0] - 1], verts[ti[1] - 1], verts[ti[2] - 1] }); // second triangle
					}
					else if (ModelReadMode == RM_OLD) {
						//#else
						int ti[3]{};
						char junk{};
						s >> junk >> ti[0] >> ti[1] >> ti[2];
						Tris.emplace_back(Triangle{ verts[ti[0] - 1], verts[ti[1] - 1] , verts[ti[2] - 1] });
					}
						//#endif
				}
			}
		}
	};

	const Vector3& CrossProduct(const Vector3& a, const Vector3& b);
	float DotProduct(const Vector3& a, const Vector3& b);

	class Graphics
	{
	public:
		void SetCursorPosition(COORD pos);
		void Clear();
		void Draw(float elapsedTime);
		void DrawLine(COORD startPoint, COORD endPoint, const char fillChar[]);
		void DrawTriangle(Triangle tri);
		const char* PixelIllumination(const Vector3& lightDir, const Vector3& normal);

		Mesh Model;

		explicit Graphics(COORD screenSize, int argc, char* argv[])
		{
			SetConsoleCP(CP_UTF8);
			SetConsoleOutputCP(CP_UTF8);
			_setmode(_fileno(stdout), _O_U16TEXT);

			if(m_ConsoleOutHandle == INVALID_HANDLE_VALUE)
				throw std::exception("Invalid OUT handle value: " + GetLastError()); // todo: It will need to be improved
			if (m_ConsoleInHandle == INVALID_HANDLE_VALUE)
				throw std::exception("Invalid IN handle value: " + GetLastError()); // todo: It will need to be improved

			m_DefaultCfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
			m_DefaultCsbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
			GetCurrentConsoleFontEx(m_ConsoleOutHandle, FALSE, &m_DefaultCfi); // save user's font info
			GetConsoleScreenBufferInfoEx(m_ConsoleOutHandle, &m_DefaultCsbi); // save buffer info
			SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &m_NewCfi);
			GetConsoleCursorInfo(m_ConsoleOutHandle, &m_DefaultCci);
			SetConsoleCursorInfo(m_ConsoleOutHandle, &m_NewCci);
			m_ConsoleWindow = GetConsoleWindow();
			m_WindowBoundsSize = GetWindowBoundsSize();
			m_DefaultConsoleScreenSize = GetConsoleScreenSize();


			SetConsoleScreenSize(screenSize.X, screenSize.Y);
			SetConsoleBuffSize(screenSize.X, screenSize.Y);
			m_ScreenHeight = screenSize.Y;
			m_ScreenWidth = screenSize.X;

			initscr(); // ncurses
			curs_set(0);
			cbreak();
			noecho();
			nodelay(stdscr, TRUE); // for non-blocking input with getch()
			scrollok(stdscr, TRUE);

			int row, col;
			getmaxyx(stdscr, row, col);

			if(argc > 1)
			{
				if(argc > 2) // with user's read mode
				{
					Model = Mesh{ argv[1], argv[2]};
				}else // default read mode (old)
				{
					Model = Mesh{ argv[1] };
				}
			}else
			{
				std::cout << "not enough arguments";
				Shutdown();
				throw std::exception("not enough arguments");
			}
			
		}

		Graphics(const Graphics& other) = delete;

		~Graphics()
		{
			Shutdown();
		}

	private:
		short m_ScreenHeight{};
		short m_ScreenWidth{};
		UINT m_OldConsoleCp{ GetConsoleCP() };
		UINT m_OldConsoleOutputCp{ GetConsoleOutputCP() };
		HANDLE m_ConsoleOutHandle{GetStdHandle(STD_OUTPUT_HANDLE)};
		HANDLE m_ConsoleInHandle{GetStdHandle(STD_INPUT_HANDLE)};
		CONSOLE_FONT_INFOEX m_DefaultCfi{};
		CONSOLE_SCREEN_BUFFER_INFOEX m_DefaultCsbi{};
		CONSOLE_FONT_INFOEX m_NewCfi{CreateCFI()};
		CONSOLE_CURSOR_INFO m_DefaultCci{};
		CONSOLE_CURSOR_INFO m_NewCci{1, false}; // hardcoded cursor info
		HWND m_ConsoleWindow{};
		std::pair<unsigned, unsigned> m_DefaultConsoleScreenSize{};
		std::pair<unsigned, unsigned> m_NewConsoleScreenSize{};
		// TODO: make this field a std::optional type of (because SetConsoleScreenSize function may be failed)
		std::pair<unsigned, unsigned> m_WindowBoundsSize{};

	private:

		std::pair<unsigned, unsigned> GetWindowBoundsSize() const;

		void SetConsoleBuffSize(short cols, short rows) const;
		void ResetConsoleBuffSize() const;

		std::pair<unsigned, unsigned> GetConsoleScreenSize() const;
		void SetConsoleScreenSize(short cols, short rows);
		void ResetConsoleScreenSize() const;

		void Shutdown()
		{
			endwin(); // curses

			SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &m_DefaultCfi);
			SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &m_DefaultCci);

			ResetConsoleScreenSize();
			ResetConsoleBuffSize();

			SetConsoleCP(m_OldConsoleCp);
			SetConsoleOutputCP(m_OldConsoleOutputCp);
		}

		CONSOLE_FONT_INFOEX CreateCFI(COORD fontSize = {8, 8}, const WCHAR* faceName = L"Raster",
		                              UINT fFamily = FF_DONTCARE, UINT fWeight = FW_NORMAL)
		{
			CONSOLE_FONT_INFOEX cfi{};
			cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
			cfi.nFont = 0;
			cfi.dwFontSize.X = fontSize.X;
			cfi.dwFontSize.Y = fontSize.Y;
			cfi.FontFamily = fFamily;
			cfi.FontWeight = fWeight;
			wcscpy_s(cfi.FaceName, faceName);
			return cfi;
		}
	};
}
