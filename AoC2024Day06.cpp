import std.core;

template<typename T>
struct Coord
{
	constexpr bool operator <  (const Coord& p) const { if (x < p.x) return true; else if (p.x < x) return false; else return y < p.y; }
	constexpr Coord operator + (const Coord& p) const { return Coord(x + p.x, y + p.y); }

	template<typename U, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	bool Within(const U& grid) const { return x >= 0 && y >= 0 && x < grid.size() && y < grid[x].size(); }
	template<typename U, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	const auto& GetAt(const U& grid) const { return grid[x][y]; }

	T x = {}, y = {};
};
using Point = Coord<int>;
constexpr std::array<Point, 4> directions = { Point(-1, 0), Point(0, 1), Point(1, 0), Point(0, -1), };

using Guard = std::pair<Point, int>;

int main(int argc, char* argv[])
{
	auto ChronoStart = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: AoC24Day06.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	std::string line;
	std::vector<std::string> map;
	Point guard;
	int dir = 0, part2 = 0;
	while (in >> line)
	{
		if (auto pos = line.find('^'); pos != std::string::npos)
			guard = {(int)map.size(), (int)pos};
		map.push_back(std::move(line));
	}
	Guard testStart = {guard, 0};

	std::set<Point> visited = {guard};
	while (true)
	{
		Point np = guard + directions[dir];
		if (!np.Within(map))
			break;
		if (np.GetAt(map) == '#')
			dir = (dir+1)%4;
		else
		{
			guard = np;
			visited.insert(guard);
		}
	}

	for (int x = 0; x < map.size(); ++x)
		for (int y = 0; y < map.front().size(); ++y)
			if (map[x][y] == '.' && visited.find({x, y}) != visited.cend())
			{
				map[x][y] = '#';

				std::set<Guard> check = {testStart};
				guard = testStart.first;
				dir = 0;
				while (true)
				{
					Point np = guard + directions[dir];
					if (!np.Within(map))
						break;
					if (np.GetAt(map) == '#')
						dir = (dir + 1) % 4;
					else
						guard = np;
					if (!check.insert({np, dir}).second)
					{
						++part2;
						break;
					}
				}
				map[x][y]= '.';
			}
	std::cout << std::format("Part 1: {}\nPart 2: {}\n", visited.size(), part2);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}