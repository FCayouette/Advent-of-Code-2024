import std.core;

#define ALL(x) (x).begin(),(x).end()
#define ALLc(x) (x).cbegin(),(x).cend()

template <typename T>
constexpr bool IsEqual(T a, T b)
{
	if constexpr (std::is_floating_point_v<T>)
		return (a < b ? b - a : a - b) < static_cast<T>(0.000001);
	else
		return a == b;
}

template<typename T>
struct Coord
{
	constexpr bool operator <  (const Coord& p) const { if (x < p.x) return true; else if (p.x < x) return false; else return y < p.y; }
	constexpr Coord operator + (const Coord& p) const { return Coord(x + p.x, y + p.y); }
	constexpr Coord& operator+=(const Coord& p) { x += p.x; y += p.y; return *this; }
	constexpr Coord operator - (const Coord& p) const { return Coord(x - p.x, y - p.y); }
	constexpr Coord& operator-=(const Coord& p) { x -= p.x; y -= p.y; return *this; }
	constexpr bool operator==(const Coord& p) const { return IsEqual(x, p.x) && IsEqual(y, p.y); }
	
	template<typename U, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	auto& GetAt(U& grid) const { return grid[x][y]; }
	template<typename U, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	const auto& GetAt(const U& grid) const { return grid[x][y]; }

	T x = {}, y = {};
};
using Point = Coord<int>;
constexpr std::array<Point, 4> directions = { Point(1, 0), Point(0, 1), Point(-1, 0), Point(0,-1) };
constexpr std::array<char, 4> coords = { 'v', '>', '^', '<' };

using Grid = std::vector<std::string>;
size_t GPSScore(const Grid& g, char c)
{
	size_t result = 0;
	for (size_t i = 0; i < g.size(); ++i)
		for (size_t j = 0; j < g[i].size(); ++j)
			if (g[i][j] == c)
				result += 100 *i + j;
	return result;
}

int main(int argc, char* argv[])
{
	auto ChronoStart = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: AoC24Day15.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	std::string line, instructions;
	Grid grid, wide;
	Point robot, r2;
	while (true)
	{
		std::getline(in, line);
		if (line.empty()) break;

		std::string s;
		for (char c : line)
			switch (c)
			{
				case '#': s += "##"; break;
				case 'O': s += "[]"; break;
				default : s += "..";
			}

		if (auto pos = line.find("@"); pos != std::string::npos)
		{
			robot = { (int)grid.size(), (int)pos };
			r2 = {robot.x, 2*robot.y};
		}
		grid.push_back(std::move(line));
		wide.push_back(std::move(s));
	}

	while (in >> line)
		instructions += line;

	robot.GetAt(grid) = '.';

	for (char m : instructions)
	{
		Point d = directions[std::find(ALLc(coords), m) - coords.cbegin()], p = robot + d;
		if (char c = p.GetAt(grid); c != '#')
		{
			if (c == '.')
				robot = p;
			else
			{
				Point t = p+d;
				while (t.GetAt(grid) == 'O')
					t += d;
				if (t.GetAt(grid) == '.')
				{
					t.GetAt(grid) = 'O';
					p.GetAt(grid) = '.';
					robot = p;
				}
			}
		}
	}

	robot = r2;
	for (char m : instructions)
	{
		int dir = std::find(ALLc(coords), m) - coords.cbegin();
		Point d = directions[dir], p = robot + d;
		if (char c = p.GetAt(wide); c != '#')
		{
			if (c == '.')
				robot = p;
			else if (dir % 2 == 1)
			{
				Point w = d+d, t = p + w;
				while (t.GetAt(wide) == c)
					t += w;
				if (t.GetAt(wide) == '.')
				{
					Point prev = t - d;
					while (prev != robot)
					{
						t.GetAt(wide) = prev.GetAt(wide);
						t = prev;
						prev -= d;
					}
					robot = p;
					robot.GetAt(wide) = '.';
				}
			}
			else
			{
				constexpr Point left = directions[3], right = directions[1];
				std::vector<std::vector<Point>> impact;
				std::vector<Point> impactLine;
				if (c == '[')
					impactLine = { p, p+right};
				else 
					impactLine = { p + left, p};
				impact.push_back(std::move(impactLine));
				bool blocked = false;
				while (!blocked)
				{
					std::vector<Point> next;
					for (Point t : impact.back())
					{
						Point test = t + d;
						if (char n = test.GetAt(wide); n == '#')
						{
							blocked = true;
							break;
						}
						else if (n != '.')
						{
							if (n == '[')
							{
								next.push_back(test);
								next.push_back(test + right);
							}
							else
							{
								next.push_back(test + left);
								next.push_back(test);
							}
						}
					}
					if(next.empty()) 
						break;
					std::sort(ALL(next));
					next.erase(std::unique(ALL(next)), next.end());
					impact.push_back(std::move(next));
				}
				if (!blocked)
				{
					for (auto iter = impact.crbegin(); iter != impact.crend(); ++iter)
						for (Point a : *iter)
						{
							Point next = a + d;
							next.GetAt(wide) = a.GetAt(wide);
							a.GetAt(wide) = '.';
						}
					robot = p;
				}
			}
		}
	}

	std::cout << std::format("Part 1: {}\nPart 2: {}\n", GPSScore(grid, 'O'), GPSScore(wide, '['));
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}