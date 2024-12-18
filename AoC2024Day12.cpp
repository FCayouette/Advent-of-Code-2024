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
	constexpr bool operator==(const Coord& p) const { return IsEqual(x, p.x) && IsEqual(y, p.y); }
	
	template<typename U, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	bool Within(const U& grid) const { return x >= 0 && y >= 0 && x < grid.size() && y < grid[x].size(); }
	template<typename U, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	const auto& GetAt(const U& grid) const { return grid[x][y]; }

	T x = {}, y = {};
};
using Point = Coord<int>;
constexpr std::array<Point, 4> directions = { Point(0, 1), Point(1, 0), Point(0, -1), Point(-1, 0) };

int main(int argc, char* argv[])
{
	auto ChronoStart = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: AoC24Day12.exe inputFilename\n";
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

	while (in >> line)
		grid.push_back(std::move(line));

	std::set<Point> guard;
	
	for (int x = 0; x < grid.size(); ++x)
		for (int y = 0; y < grid.front().size(); ++y)
			if (Point p{ x, y }; guard.find(p) == guard.cend())
			{
				std::set<Point> patch = {p}, cur = patch, sides;
				char C = grid[x][y];
				while(!cur.empty())
				{
					Point c = *cur.cbegin();
					for (Point d : directions)
						if (Point np = c + d; np.Within(grid) && np.GetAt(grid) == C && patch.insert(np).second)
							cur.insert(np);
					cur.erase(c);
				}
				int per = 0;
				for(Point t : patch)
					for (Point d: directions)
						if (Point np = t + d; !np.Within(grid) || np.GetAt(grid) != C)
						{
							++per;
							sides.insert(np);
						}
				
				part1 += patch.size() * per;
				int s = 0;
				bool outside = true;
				while (!sides.empty())
				{
					int dir = 0;
					Point start = *sides.cbegin(), cur = start;
					std::set<Point> curPer = {cur};
					do 
					{
						if (outside)
						{
							if (Point n = cur + directions[dir]; sides.find(n) != sides.cend())
							{
								if (Point rightHand = n + directions[(dir + 1) % 4]; patch.find(rightHand) != patch.cend())
									cur = n;
								else
								{
									++s;
									dir = (dir + 1) % 4;
									cur = rightHand;
								}
								curPer.insert(cur);
							}
							else
							{
								++s;
								if (patch.find(n) != patch.cend())
									dir = (dir + 3) % 4;
								else
								{
									dir = (dir + 1) % 4;
									cur = n + directions[dir];
									curPer.insert(cur);
								}
							}
						}
						else
						{
							if (Point n = cur + directions[dir]; sides.find(n) != sides.cend())
							{
								if (Point inTurn = n + directions[(dir + 3) % 4]; sides.find(inTurn) != sides.cend())
								{
									++s;
									dir = (dir + 3) % 4;
									cur = inTurn;
								}
								else
									cur = n;
								curPer.insert(cur);
							}
							else
							{
								dir = (dir + 1) % 4;
								++s;
							}
						}
					} while (cur != start || dir != 0);
					outside = false;
					for (Point x : curPer)
						sides.erase(x);
				}
				part2 += s * patch.size();
				guard.insert(patch.cbegin(), patch.cend());
			}


	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, part2);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}