import std.core;

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
	constexpr bool operator <  (const Coord& p) const { if (x < p.x) return true; else if (p.x < x) return false; else return y < p.y; }
	constexpr Coord operator + (const Coord& p) const { return Coord(x + p.x, y + p.y); }
	constexpr bool operator==(const Coord& p) const { return IsEqual(x, p.x) && IsEqual(y, p.y); }

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
		std::cout << "Usage: AoC24Day18.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	int part1 = 0, maxX = 71, maxY = 71, insert = 1024;
	std::vector grid = std::vector(maxX, std::vector<char>(maxY, '.'));
	const Point Target{maxX-1, maxY-1};
	std::vector<Point> bytes;
	std::string line;

	while (in >> line)
	{
		auto [x, y] = SplitAtFirst(std::move(line), ",");
		bytes.emplace_back(stoi(x), stoi(y));
	}

	for (int i = 0; i < 1024; ++i)
		bytes[i].GetAt(grid) = '#';

	while (true)
	{
		bool goOn = true;
		std::set<Point> cur = { Point{} }, next, guard;
		while (goOn && !cur.empty())
		{
			if (insert == 1024)
				++part1;
			for (Point p : cur)
				for (Point d : directions)
					if (Point np = p + d; np.Within(grid) && np.GetAt(grid) == '.' && guard.insert(np).second)
					{
						if (np == Target)
							goOn = false;
						next.insert(np);
					}
			std::swap(cur, next);
			next.clear();
		}
		if (!goOn)
			bytes[insert++].GetAt(grid) = '#';
		else
		{
			--insert;
			std::cout << std::format("Part 2: {},{}\n", bytes[insert].x, bytes[insert].y);
			break;
		}
	}
	std::cout << std::format("Part 1: {}\n", part1);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}