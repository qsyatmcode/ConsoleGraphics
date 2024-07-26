#pragma once

#include <Windows.h>
#include <iostream>
#include <io.h> // for _setmode()
#include <fcntl.h>
#include <vector>
#include <utility> // for std::pair

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

	struct Vector3
	{
		float x{}, y{}, z{};

		float Length() const
		{
			return sqrtf(x * x + y * y + z * z);
		}
	};
	struct Matrix4
	{
		float m[4][4];
	};

	struct Triangle
	{
		Vector3 verts[3];
	};

	struct Mesh
	{
		std::vector<Triangle> tris{};
	};

	const Vector3& MatVecM(const Vector3& vec, const Matrix4& mat);
	const Vector3& CrossProduct(const Vector3& a, const Vector3& b);
	const Vector3& Normalize(const Vector3& vec);
	float DotProduct(const Vector3& a, const Vector3& b);

	struct Graphics
	{
		void SetCursorPosition(COORD pos);
		void Clear();
		void Draw(float elapsedTime);
		void DrawLine(COORD startPoint, COORD endPoint);

		Graphics(COORD screenSize)
		{
			SetConsoleCP(CP_UTF8);
			SetConsoleOutputCP(CP_UTF8);
			_setmode(_fileno(stdout), _O_U16TEXT);

			if(m_ConsoleOutHandle == INVALID_HANDLE_VALUE)
				throw std::exception("Invalid OUT handle value: " + GetLastError()); // todo: It will need to be improved
			if (m_ConsoleInHandle == INVALID_HANDLE_VALUE)
				throw std::exception("Invalid IN handle value: " + GetLastError()); // todo: It will need to be improved

			EnableVirtualTerminalProcessing();

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
		}

		~Graphics()
		{
			SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &m_DefaultCfi);
			SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &m_DefaultCci);

			ResetConsoleScreenSize();
			ResetConsoleBuffSize();

			SetConsoleCP(m_OldConsoleCp);
			SetConsoleOutputCP(m_OldConsoleOutputCp);
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

		void EnableVirtualTerminalProcessing() const;

		CONSOLE_FONT_INFOEX CreateCFI(COORD fontSize = {12, 16}, const WCHAR* faceName = L"Consolas",
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
