#include "tgraphics.h"

#include <algorithm>
#include <chrono>
#include <string>

namespace TG
{
	constexpr float PI = 3.141592f;

	Vector3 operator*(const Matrix4& mat, const Vector3& vec)
	{
		float x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + mat.m[3][0];
		float y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + mat.m[3][1];
		float z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + mat.m[3][2];

		if (float w = vec.x * mat.m[0][3] + vec.y * mat.m[1][3] + vec.z * mat.m[2][3] + mat.m[3][3]; w != 0.0f)
		{
			x /= w;
			y /= w;
			z /= w;
			return { x, y, z };
		}

		return { 0, 0, 0 };
	}

	Vector3 operator*(const Vector3& vec, const Matrix4& mat) { return mat * vec; }

	//const Vector3& MatVecM(const Vector3& vec, const Matrix4& mat)
	//{
	//	float x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + mat.m[3][0];
	//	float y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + mat.m[3][1];
	//	float z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + mat.m[3][2];
	//
	//	if(float w = vec.x * mat.m[0][3] + vec.y * mat.m[1][3] + vec.z * mat.m[2][3] + mat.m[3][3]; w != 0.0f)
	//	{
	//		x /= w;
	//		y /= w;
	//		z /= w;
	//		return { x, y, z };
	//	}
	//}

	const Vector3& CrossProduct(const Vector3& a, const Vector3& b)
	{
		return {
			a.y * b.z - a.z * b.y,
			a.z* b.x - a.x * b.z,
			a.x* b.y - a.y * b.x
		};
	}

	float DotProduct(const Vector3& a, const Vector3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z ;
	}

	void Graphics::SetCursorPosition(COORD pos)
	{
		move(pos.Y, pos.X);
		//std::wcout << L"\x1b[" << pos.Y << L";" << pos.X << L"H";
	}

	void Graphics::Clear()
	{
		clear();
		//std::wcout << L"\x1b[1;1H\x1b[2J";
	}

	void Graphics::Draw(float elapsedTime)
	{
		static float zOffset = 20.0f;
		static float rotAngle{ 0.0f };
		rotAngle += 1.0f * elapsedTime;
		static const float fov{ 80.0f };
		static const float fovRad{ 1.0f / tanf( fov * 0.5f / 180.0f * PI ) };
		static const float aspectRatio{ static_cast<float>(m_ScreenHeight) / static_cast<float>(m_ScreenWidth) };
		static const float zNear{ 0.1f };
		static const float zFar{ 1000.0f };

		constexpr static Vector3 lightDirection{0, 0, -1};

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

		SetCursorPosition(COORD{ 0,0 });
		printw("%f", elapsedTime);

		std::vector<Triangle> triToRaster{};

		for(auto tri : Model.Tris) // draw Model mesh
		{
			Vector3 rotatedZ[3];
			//rotatedZ[0] = MatVecM(tri.verts[0], rotZMat);
			//rotatedZ[1] = MatVecM(tri.verts[1], rotZMat);
			//rotatedZ[2] = MatVecM(tri.verts[2], rotZMat);
			rotatedZ[0] = tri.verts[0] * rotZMat;
			rotatedZ[1] = tri.verts[1] * rotZMat;
			rotatedZ[2] = tri.verts[2] * rotZMat;

			Vector3 rotatedXZ[3];
			//rotatedXZ[0] = MatVecM(rotatedZ[0], rotXMat);
			//rotatedXZ[1] = MatVecM(rotatedZ[1], rotXMat);
			//rotatedXZ[2] = MatVecM(rotatedZ[2], rotXMat);
			rotatedXZ[0] = rotatedZ[0] * rotXMat;
			rotatedXZ[1] = rotatedZ[1] * rotXMat;
			rotatedXZ[2] = rotatedZ[2] * rotXMat;
			
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
			Vector3 normCp = cp.Normalize();

			if(DotProduct(normCp, rotatedXZ[0]) > 0.0f) // see if less than 90 degrees
			{
				continue;
			}

			Vector3 proj[3];
			//proj[0] = MatVecM(rotatedXZ[0], projMatrix);
			//proj[1] = MatVecM(rotatedXZ[1], projMatrix);
			//proj[2] = MatVecM(rotatedXZ[2], projMatrix);
			proj[0] = rotatedXZ[0] * projMatrix;
			proj[1] = rotatedXZ[1] * projMatrix;
			proj[2] = rotatedXZ[2] * projMatrix;

			proj[0].x = (proj[0].x + 0.6f) * (0.5f * m_ScreenWidth);
			proj[0].y = (proj[0].y + 1.0f) * (0.5f * m_ScreenHeight);
			proj[1].x = (proj[1].x + 0.6f) * (0.5f * m_ScreenWidth);
			proj[1].y = (proj[1].y + 1.0f) * (0.5f * m_ScreenHeight);
			proj[2].x = (proj[2].x + 0.6f) * (0.5f * m_ScreenWidth);
			proj[2].y = (proj[2].y + 1.0f) * (0.5f * m_ScreenHeight);

			Triangle toRaster{
				{
					{proj[0].x, proj[0].y, proj[0].z},
					{proj[1].x, proj[1].y, proj[1].z},
					{proj[2].x, proj[2].y, proj[2].z},
				}
			};

			toRaster.filler = PixelIllumination(lightDirection, normCp);

			triToRaster.push_back(toRaster);
		}

		std::sort(triToRaster.begin(), triToRaster.end(), [](Triangle& t1, Triangle& t2)
		{
				float z1 = (t1.verts[0].z + t1.verts[1].z + t1.verts[2].z) / 3.0f;
				float z2 = (t2.verts[0].z + t2.verts[1].z + t2.verts[2].z) / 3.0f;
				return z1 > z2;
		});

		for(auto tri : triToRaster)
		{
			DrawTriangle(tri);
		}

		refresh();
		Clear();
	}

	void Graphics::DrawLine(COORD startPoint, COORD endPoint, const char fillChar[]) // Bresenham's line algorithm
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
				printw("%s", fillChar);

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
				}
			}
		}
		else { // not invert
			while (startPoint.X != endPoint.X)
			{
				SetCursorPosition(startPoint);
				printw("%s", fillChar);

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
				}
			}
		}
	}

	const Vector3& interpolate(const Point2& p1, const Point2& p2, float factor) {
		return Vector3{(p1.x + (p2.x - p1.x) * factor )};
	}

	void Graphics::DrawTriangle(Triangle tri)
	{
		auto v3 = Point2{ static_cast<int>(tri.verts[2].x), static_cast<int>(tri.verts[2].y) };
		auto v2 = Point2{ static_cast<int>(tri.verts[1].x), static_cast<int>(tri.verts[1].y) };
		auto v1 = Point2{ static_cast<int>(tri.verts[0].x), static_cast<int>(tri.verts[0].y) };


		if (v1.y > v2.y) std::swap(v1, v2);
		if (v1.y > v3.y) std::swap(v1, v3);
		if (v2.y > v3.y) std::swap(v2, v3);


		// Вычисление общей высоты треугольника
		float total_height = v3.y - v1.y;

		if (v2.y != v1.y) {
			// Растеризация нижней половины треугольника
			for (int y = v1.y; y <= v2.y; y++) {
				float segment_height = v2.y - v1.y + 1;
				if (total_height == 0 || segment_height == 0)
					continue;
				float alpha = (total_height != 0) ? (y - v1.y) / total_height : 0;
				float beta = (segment_height != 0) ? (y - v1.y) / segment_height : 0;
				Vector3 A = interpolate(v1, v3, alpha);
				Vector3 B = interpolate(v1, v2, beta);

				if (A.x > B.x) std::swap(A, B);

				for (int x =  A.x; x <= B.x; x++) {
					SetCursorPosition(COORD{ static_cast<short>(x), static_cast<short>(y) });
					printw("%s", tri.filler);
				}
			}
		}
		if (v2.y == v3.y)
			return;
		// Растеризация верхней половины треугольника
		for (int y = v2.y; y <= v3.y; y++) {
			float segment_height = v3.y - v2.y + 1;
			if (total_height == 0 || segment_height == 0)
				continue;
			float alpha = (total_height != 0) ? (y - v1.y) / total_height : 0;
			float beta = (segment_height != 0) ? (y - v2.y) / segment_height : 0;
			Vector3 A = interpolate(v1, v3, alpha);
			Vector3 B = interpolate(v2, v3, beta);
			if (A.x > B.x) std::swap(A, B);
			for (int x = A.x; x <= B.x; x++) {
				SetCursorPosition(COORD{ static_cast<short>(x), static_cast<short>(y) });
				printw("%s", tri.filler);
			}
		}
	}

	const char* Graphics::PixelIllumination(const Vector3& lightDir, const Vector3& normal)
	{
		float dp = DotProduct(normal, lightDir);
		if(dp > 0.75f)
		{
			return "█";
		}else if (dp > 0.5f)
		{
			return "▓";
		}else if (dp > 0.25f)
		{
			return "▒";
		}else if(dp > 0.0f) 
		{
			return "░";
		}

		return " ";
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
}
