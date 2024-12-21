import std.core;

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

int main(int argc, char* argv[])
{
	auto ChronoStart = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: AoC2024Day20.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	int part1 = 0, part2 = 0;
	std::string line;
	std::vector<std::string> grid;
	Point Start, End;

	while (in >> line)
	{
		if (auto pos = line.find('S'); pos != std::string::npos)
		{
			Start = { (int)grid.size(), (int)pos };
			line[pos] = '.';
		}
		if (auto pos = line.find('E'); pos != std::string::npos)
		{
			End = { (int)grid.size(), (int)pos };
			line[pos] = '.';
		}
		grid.push_back(std::move(line));
	}
	std::vector baseTime = std::vector(grid.size(), std::vector(grid.front().size(), 0));

	auto BaseSolve = [&grid, Start, End, &baseTime]()
	{
		int steps = 0;
		std::set<Point> cur = { Start }, next, guard = cur;
		Start.GetAt(baseTime) = 0;
		while (!cur.empty())
		{
			++steps;
			for (Point p : cur)
				for (Point d : directions)
				{
					Point np = p + d;
					if (char c = np.GetAt(grid); c != '#' && guard.insert(np).second)
					{
						np.GetAt(baseTime) = steps;
						next.insert(np);
					}
				}
			std::swap(cur, next);
			next.clear();
		}
	};

	BaseSolve();

	for (int i = 1; i < grid.size() - 1; ++i)
		for (int j = 1; j < grid.front().size() - 1; ++j)
			if (Point p = { i, j }; grid[i][j] == '.')
			{
				int pTime = p.GetAt(baseTime) + 2;
				for (Point d : directions)
					if (Point two = p + d + d; two.Within(grid) && two.GetAt(grid) == '.')
						if (int tTime = two.GetAt(baseTime); tTime - pTime >= 100)
							++part1;
			}

	
	for (int i = 1; i < grid.size() - 1; ++i)
		for (int j = 1; j < grid.front().size() - 1; ++j)
			if (Point p = { i, j }; grid[i][j] == '.')
			{
				std::set<Point> cur = {p}, next, guard = cur;
				for (int steps = 1, pTime = p.GetAt(baseTime) + 1; steps <= 20; ++steps, ++pTime)
				{
					for (Point t : cur)
						for (Point d : directions)
							if (Point np = t + d; np.Within(grid) && guard.insert(np).second)
							{
								if (int npTime = np.GetAt(baseTime); npTime - pTime >= 100)
									++part2;
								next.insert(np);
							}
					std::swap(cur, next);
					next.clear();
				}
			}

	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, part2);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}