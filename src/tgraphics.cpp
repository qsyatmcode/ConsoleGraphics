#include "tgraphics.h"

#include <chrono>

namespace TG
{
	constexpr float PI = 3.141592f;

	const Vector3& MatVecM(const Vector3& vec, const Matrix4& mat)
	{
		float x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + mat.m[3][0];
		float y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + mat.m[3][1];
		float z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + mat.m[3][2];

		if(float w = vec.x * mat.m[0][3] + vec.y * mat.m[1][3] + vec.z * mat.m[2][3] + mat.m[3][3]; w != 0.0f)
		{
			x /= w;
			y /= w;
			z /= w;
			return { x, y, z };
		}
	}

	const Vector3& CrossProduct(const Vector3& a, const Vector3& b)
	{
		return {
			a.y * b.z - a.z * b.y,
			a.z* b.x - a.x * b.z,
			a.x* b.y - a.y * b.x
		};
	}

	const Vector3& Normalize(const Vector3& vec)
	{
		float l = vec.Length();
		if (l <= 0)
			return {0, 0, 0};

		return {
			vec.x / l,
			vec.y / l,
			vec.z / l
		};
	}

	float DotProduct(const Vector3& a, const Vector3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z ;
	}

	void Graphics::SetCursorPosition(COORD pos)
	{
		std::wcout << L"\x1b[" << pos.Y << L";" << pos.X << L"H";
	}

	void Graphics::Clear()
	{
		std::wcout << L"\x1b[1;1H\x1b[2J";
	}

	void Graphics::Draw(float elapsedTime)
	{
		static float zOffset = 3.0f;
		static float rotAngle{ 0.0f };
		rotAngle += 1.0f * elapsedTime;
		static const float fov{ 80.0f };
		static const float fovRad{ 1.0f / tanf( fov * 0.5f / 180.0f * PI ) };
		static const float aspectRatio{ static_cast<float>(m_ScreenHeight) / static_cast<float>(m_ScreenWidth) };
		static const float zNear{ 0.1f };
		static const float zFar{ 1000.0f };

		// rotation matrix for 3d space - https://w.wiki/AjaZ
		Matrix4 rotXMat{
			{
				{1, 0, 0, 0},
				{0, cosf(rotAngle * 0.5f), -sinf(rotAngle * 0.5f), 0},
				{0, sinf(rotAngle * 0.5f), cosf(rotAngle * 0.5f), 0},
				{0, 0, 0, 1}
			}
		};

		Matrix4 rotZMat{
			{
				{cosf(rotAngle), -sinf(rotAngle), 0, 0},
				{sinf(rotAngle), cosf(rotAngle), 0, 0},
				{0, 0, 1, 0},
				{0, 0, 0, 1},
			}
		};

		static Matrix4 projMatrix{
			{
				{ aspectRatio * fovRad, 0, 0, 0 },
				{	0, fovRad, 0, 0 },
				{ 0, 0, zFar / (zFar - zNear),  1 },
				{ 0, 0, (-zFar * zNear) / (zFar - zNear),0 },
			}
		};

		static Mesh cube{
			{
				// SOUTH
				{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

				// EAST                                                      
				{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
				{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

				// NORTH                                                     
				{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
				{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

				// WEST                                                      
				{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

				// TOP                                                       
				{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
				{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

				// BOTTOM                                                    
				{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
				{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
			}
		};

		//SetCursorPosition(COORD{ 0,0 });
		wprintf(L"%f", elapsedTime);
		for(auto tri : cube.tris) // draw cube mesh
		{
			Vector3 rotatedZ[3];
			rotatedZ[0] = MatVecM(tri.verts[0], rotZMat);
			rotatedZ[1] = MatVecM(tri.verts[1], rotZMat);
			rotatedZ[2] = MatVecM(tri.verts[2], rotZMat);

			Vector3 rotatedXZ[3];
			rotatedXZ[0] = MatVecM(rotatedZ[0], rotXMat);
			rotatedXZ[1] = MatVecM(rotatedZ[1], rotXMat);
			rotatedXZ[2] = MatVecM(rotatedZ[2], rotXMat);
			
			rotatedXZ[0].z += zOffset;
			rotatedXZ[1].z += zOffset;
			rotatedXZ[2].z += zOffset;

			Vector3 a, b; // make lines for cross product
			a.x = rotatedXZ[1].x - rotatedXZ[0].x;
			a.y = rotatedXZ[1].y - rotatedXZ[0].y;
			a.z = rotatedXZ[1].z - rotatedXZ[0].z;
			b.x = rotatedXZ[2].x - rotatedXZ[0].x;
			b.y = rotatedXZ[2].y - rotatedXZ[0].y;
			b.z = rotatedXZ[2].z - rotatedXZ[0].z;

			Vector3 cp = CrossProduct(a, b);
			Vector3 normCp = Normalize(cp);

			if(normCp.z > 0.0f)
			{
				continue;
			}

			Vector3 proj[3];
			proj[0] = MatVecM(rotatedXZ[0], projMatrix);
			proj[1] = MatVecM(rotatedXZ[1], projMatrix);
			proj[2] = MatVecM(rotatedXZ[2], projMatrix);

			//SetCursorPosition({ static_cast<short>((proj[0].x + 1.0f) * (0.5f * m_ScreenWidth)), static_cast<short>((proj[0].y + 1.0f) * (0.5f * m_ScreenHeight))});
			//wprintf(L"%c", full_block_char);
			//SetCursorPosition({ static_cast<short>((proj[1].x + 1.0f) * (0.5f * m_ScreenWidth)), static_cast<short>((proj[1].y + 1.0f) * (0.5f * m_ScreenHeight)) });
			//wprintf(L"%c", full_block_char);
			//SetCursorPosition({ static_cast<short>((proj[2].x + 1.0f) * (0.5f * m_ScreenWidth)), static_cast<short>((proj[2].y + 1.0f) * (0.5f * m_ScreenHeight)) });
			//wprintf(L"%c", full_block_char);

			DrawLine(COORD{ static_cast<short>((proj[0].x + 1.0f) * (0.5f * m_ScreenWidth)), static_cast<short>((proj[0].y + 1.0f) * (0.5f * m_ScreenHeight)) },
				COORD{ static_cast<short>((proj[1].x + 1.0f) * (0.5f * m_ScreenWidth)), static_cast<short>((proj[1].y + 1.0f) * (0.5f * m_ScreenHeight)) });
			DrawLine(COORD{ static_cast<short>((proj[1].x + 1.0f) * (0.5f * m_ScreenWidth)), static_cast<short>((proj[1].y + 1.0f) * (0.5f * m_ScreenHeight)) },
				COORD{ static_cast<short>((proj[2].x + 1.0f) * (0.5f * m_ScreenWidth)), static_cast<short>((proj[2].y + 1.0f) * (0.5f * m_ScreenHeight)) });
			DrawLine(COORD{ static_cast<short>((proj[2].x + 1.0f) * (0.5f * m_ScreenWidth)), static_cast<short>((proj[2].y + 1.0f) * (0.5f * m_ScreenHeight)) },
				COORD{ static_cast<short>((proj[0].x + 1.0f) * (0.5f * m_ScreenWidth)), static_cast<short>((proj[0].y + 1.0f) * (0.5f * m_ScreenHeight)) });

			//SetCursorPosition({ static_cast<short>((proj.x + 1.0f) * (0.5f * m_ScreenWidth) ), static_cast<short>((proj.y + 1.0f) * (0.5f * m_ScreenHeight)) });
			//wprintf(L"%c", full_block_char);

			//for(auto vert : tri.verts )
			//{
			//	Vector3 rotatedZ = MatVecM(vert, rotZMat);
			//	Vector3 rotatedXZ = MatVecM(rotatedZ, rotXMat);
			//
			//	rotatedXZ.z += zOffset;
			//
			//	Vector3 proj = MatVecM(rotatedXZ, projMatrix); // rotate
			//	//Vector3 proj = MatVecM(vert, projMatrix);
			//	SetCursorPosition({ static_cast<short>((proj.x + 1.0f) * (0.5f * m_ScreenWidth) ), static_cast<short>((proj.y + 1.0f) * (0.5f * m_ScreenHeight)) });
			//	wprintf(L"%c", full_block_char);
			//}
		}

		//DrawLine(COORD{ 10, 10 }, COORD{ 10, 0 });
		//DrawLine(COORD{ 10, 10 }, COORD{ 17, 5 });
		//DrawLine(COORD{ 10, 10 }, COORD{ 20, 10 });
		//DrawLine(COORD{ 10, 10 }, COORD{ 17, 15 });
		//DrawLine(COORD{ 10, 10 }, COORD{ 10, 20 });
		//DrawLine(COORD{ 10, 10 }, COORD{ 3, 15 });
		//DrawLine(COORD{ 10, 10 }, COORD{ 0, 10 });
		//DrawLine(COORD{ 10, 10 }, COORD{ 3, 5 });

		Clear();
	}

	void Graphics::DrawLine(COORD startPoint, COORD endPoint) // Bresenham's line algorithm
	{
		enum class YDirection { UP, DOWN } ydir{YDirection::UP};
		enum class XDirection{ RIGHT, LEFT } xdir{XDirection::RIGHT};

		bool invert {false};
		float error{0.0f};
		short dx = endPoint.X - startPoint.X;
		if (dx < 0) {
			dx = -dx;
			xdir = XDirection::LEFT;
		}
		short dy = endPoint.Y - startPoint.Y;
		if (dy < 0) {
			dy = -dy;
			ydir = YDirection::DOWN;
		}
		// dx > dy => line's up => one pixel per column
		// dx < dy => line's down => one pixel per row
		if (dx < dy)
			invert = true;
		const float m = static_cast<float>(dy) / static_cast<float>(dx); // slope.  how much change in Y, for 1 change in X

		if (dx <= dy) { // invert
			const float m = static_cast<float>(dx) / static_cast<float>(dy); // slope.  how much change in Y, for 1 change in X
			while (startPoint.Y != endPoint.Y)
			{
				SetCursorPosition(startPoint);
				wprintf(L"%c", full_block_char);
				if (ydir == YDirection::UP)
					startPoint.Y++; // move Y down
				else
					startPoint.Y--; // move X up
				error += std::abs(m);
				if (error > 0.5f && error < 500.0f)
				{
					if (xdir == XDirection::RIGHT)
						startPoint.X++; // move X right
					else
						startPoint.X--; // move X left
					error -= 1.0f;
					//error = 0.0f;
				}
			}
		}
		else { // not invert
			while (startPoint.X != endPoint.X)
			{
				SetCursorPosition(startPoint);
				wprintf(L"%c", full_block_char);
				if (xdir == XDirection::RIGHT)
					startPoint.X++; // move X right
				else
					startPoint.X--; // move X left
				error += std::abs(m);
				if (error > 0.5f && error < 500.0f)
				{
					if (ydir == YDirection::UP)
						startPoint.Y++; // move Y down
					else
						startPoint.Y--; // move Y up
					error -= 1.0f;
					//error = 0.0f;
				}
			}
		}
	}

	std::pair<unsigned, unsigned> Graphics::GetWindowBoundsSize() const
	{
		RECT rect{0, 0, 0, 0};
		GetClientRect(m_ConsoleWindow, &rect); // gets client area coords
		const int clientRectAreaX{rect.right - rect.left};
		const int clientRectAreaY{rect.bottom - rect.top};

		GetWindowRect(m_ConsoleWindow, &rect); // get window bounds area coords
		const int windowBoundsSizeX{rect.right - rect.left - clientRectAreaX};
		const int windowBoundsSizeY{rect.bottom - rect.top - clientRectAreaY};

		return std::pair{windowBoundsSizeX, windowBoundsSizeY};
	}

	void Graphics::SetConsoleBuffSize(short cols, short rows) const
	{
		if (cols < m_NewConsoleScreenSize.first || rows < m_NewConsoleScreenSize.second)
		{
			std::cerr <<
				"Screen buffer size changing error: new buffer size must be greater or equal to console window size\n";
			return;
		}
		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (!SetConsoleScreenBufferSize(consoleHandle, COORD{cols, rows}))
		{
			std::cerr << "Screen buffer size changing error: " << GetLastError() << std::endl;
		}
	}

	void Graphics::ResetConsoleBuffSize() const
	{
		SetConsoleBuffSize(m_DefaultCsbi.dwSize.X, m_DefaultCsbi.dwSize.Y);
	}

	void Graphics::SetConsoleScreenSize(short cols, short rows)
	{
		if (!SetWindowPos(m_ConsoleWindow,
		                  HWND_TOP,
		                  0, 0, //rect.left, rect.top,
		                  cols * m_NewCfi.dwFontSize.X + m_WindowBoundsSize.first,
		                  rows * m_NewCfi.dwFontSize.Y + m_WindowBoundsSize.second,
		                  SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER))
		{
			std::cerr << "Console screen size settings up failed: " << GetLastError() << std::endl;
		}
		else
		{
			m_NewConsoleScreenSize = std::pair<unsigned, unsigned>{cols, rows};
		}
	}

	/// return: Cols (first), Rows (second)
	std::pair<unsigned, unsigned> Graphics::GetConsoleScreenSize() const
	{
		RECT rect{0, 0, 0, 0};
		std::pair<unsigned, unsigned> result{};

		GetClientRect(m_ConsoleWindow, &rect);

		unsigned width = rect.right - rect.left;
		unsigned height = rect.bottom - rect.top;

		result.first = width / m_DefaultCfi.dwFontSize.X;
		result.second = height / m_DefaultCfi.dwFontSize.Y;

		return result;
	}

	void Graphics::ResetConsoleScreenSize() const
	{
		if (!SetWindowPos(m_ConsoleWindow,
		                  HWND_TOP,
		                  0, 0, //rect.left, rect.top,
		                  m_DefaultConsoleScreenSize.first * m_DefaultCfi.dwFontSize.X + m_WindowBoundsSize.first,
		                  m_DefaultConsoleScreenSize.second * m_DefaultCfi.dwFontSize.Y + m_WindowBoundsSize.second,
		                  SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER))
		{
			std::cerr << "Console screen size reset is failed: " << GetLastError() << std::endl;
		}
	}

	void Graphics::EnableVirtualTerminalProcessing() const
	{
		DWORD defaultOutMode {0};
		if (!GetConsoleMode(m_ConsoleOutHandle, &defaultOutMode))
			throw std::exception("Error in EnableVirtualTerminalProcessing (getting OUT CM): " + GetLastError());

		DWORD defaultInMode {0};
		if (!GetConsoleMode(m_ConsoleOutHandle, &defaultInMode))
			throw std::exception("Error in EnableVirtualTerminalProcessing (getting IN CM): " + GetLastError());

		DWORD newOutMode = defaultOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
		DWORD newInMode = defaultInMode | ENABLE_VIRTUAL_TERMINAL_INPUT;

		if(!SetConsoleMode(m_ConsoleOutHandle, newOutMode))
		{
			newOutMode = defaultOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;

			if (!SetConsoleMode(m_ConsoleOutHandle, newOutMode))
				throw std::exception("Error in EnableVirtualTerminalProcessing (setting OUT CM): " + GetLastError());
		}

		if (!SetConsoleMode(m_ConsoleInHandle, newInMode))
			throw std::exception("Error in EnableVirtualTerminalProcessing (setting IN CM): " + GetLastError());
	}
}
