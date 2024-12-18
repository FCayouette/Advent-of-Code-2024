import std.core;

#define ALLc(x) (x).cbegin(),(x).cend()

std::pair<std::string, std::string> SplitAtFirst(std::string s, std::string token)
{
	auto pos = s.find(token);
	if (pos == std::string::npos)
		return std::make_pair(std::move(s), std::string{});
	return std::make_pair(s.substr(0, pos), s.substr(pos + token.size()));
}

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
	constexpr Coord operator + (const Coord& p) const { return Coord(x + p.x, y + p.y); }
	constexpr Coord& operator+=(const Coord& p) { x += p.x; y += p.y; return *this; }
	constexpr Coord& operator%=(const Coord& p) { x %= p.x; y %= p.y; return *this; }
	template<typename U, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	bool Within(const U& grid) const { return x >= 0 && y >= 0 && x < grid.size() && y < grid[x].size(); }
	template<typename U, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	auto& GetAt(U& grid) const { return grid[x][y]; }
	template<typename U, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	const auto& GetAt(const U& grid) const { return grid[x][y]; }
	T x = {}, y = {};
};
using Point = Coord<int>;
constexpr std::array<Point, 4> directions = { Point(1, 0), Point(0, 1), Point(-1, 0), Point(0,-1) };

Point GetPoint(std::string s)
{
	auto pos = s.find(',');
	return Point(stoi(s.substr(0, pos)), stoi(s.substr(pos+1)));
}

int main(int argc, char* argv[])
{
	auto ChronoStart = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: AoC24Day14.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	std::string line;
	std::vector<Point> pos, vel;
	const Point gridMax(101, 103);
	while (std::getline(in, line))
	{
		auto [p,v] = SplitAtFirst(line.substr(2), " v=");
		pos.push_back(GetPoint(p));
		vel.push_back(GetPoint(v));
	}

	int midX = gridMax.x / 2, midY = gridMax.y / 2, part2 = 0, steps = 0, maxSize = 7;
	std::array<int, 4> quadrantCount = {0,0,0,0};

	std::vector<std::vector<char>> grid(101, std::vector<char>(103,' ')), blank = grid;
	while (steps < 101*103)
	{
		++steps;
		for (int i = 0; i < pos.size(); ++i)
		{
			pos[i] += vel[i] + gridMax;
			pos[i] %= gridMax;
			grid[pos[i].x][pos[i].y] = 'X';
		}
		if (steps == 100)
			for (Point p : pos)
			{
				if (p.x == midX || p.y == midY)
					continue;
				++quadrantCount[(p.x > midX ? 2 : 0) + (p.y > midY ? 1 : 0)];
			}

		for (Point p : pos)
			if (p.GetAt(grid) == 'X')
			{
				int size = 1;
				p.GetAt(grid) = '#';
				std::deque<Point> toProcess;
				for (Point d : directions)
					toProcess.push_back(p+d);
				while (!toProcess.empty())
				{
					if (Point t = toProcess.front(); t.Within(grid) && t.GetAt(grid) == 'X')
					{
						++size;
						t.GetAt(grid) = '#';
						for (Point d : directions)
							toProcess.push_back(t + d);
					}
					toProcess.pop_front();
				}

				if (size > maxSize)
				{
					maxSize = size;
					part2 = steps;
				}
			}
		grid = blank;
	}

	std::cout << std::format("Part 1: {}\nPart 2: {}\n", std::accumulate(ALLc(quadrantCount), 1, std::multiplies()), part2);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}