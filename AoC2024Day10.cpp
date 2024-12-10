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
constexpr std::array<Point, 4> directions = { Point(1, 0), Point(0, 1), Point(-1, 0), Point(0,-1) };

template <typename T>
class y_combinator {
	T lambda;
public:
	constexpr y_combinator(T&& t) : lambda(std::forward<T>(t)) {}
	template <typename...Args>
	constexpr decltype(auto) operator()(Args&&...args) const { return lambda(std::move(*this), std::forward<Args>(args)...); }
};

int main(int argc, char* argv[])
{
	auto ChronoStart = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: AoC24Day10.exe inputFilename\n";
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
	std::vector<std::vector<int>> grid;
	
	while (in >> line)
	{
		std::vector<int> v;
		for (char c : line)
			v.push_back(c-'0');
		grid.push_back(std::move(v));
	}

	auto PathCount = y_combinator([&grid](auto && PathCount, Point p, int step) -> int
	{
		if (step++ >= 9)
			return 1;
		int result = 0;
		for (Point d : directions)
			if (Point np = p + d; np.Within(grid) && np.GetAt(grid) == step)
				result += PathCount(np, step);

		return result;
	});

	for (int i = 0; i < grid.size(); ++i)
		for (int j = 0; j < grid.front().size(); ++j)
			if (grid[i][j] == 0)
			{
				std::set<Point> cur, next;
				cur.insert({i, j});
				int iter = 1;
				while (iter < 10 && !cur.empty())
				{
					for (Point p : cur)
						for (Point d : directions)
							if (Point np = p + d; np.Within(grid) && np.GetAt(grid) == iter)
								next.insert(np);
					++iter;
					std::swap(cur, next);
					next.clear();
				}

				part1 += cur.size();
				part2 += PathCount(Point{i, j}, 0);
			}

	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, part2);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}