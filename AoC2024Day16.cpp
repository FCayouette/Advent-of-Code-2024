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
	constexpr bool operator==(const Coord& p) const { return IsEqual(x, p.x) && IsEqual(y, p.y); }
	
	template<typename U, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	auto& GetAt(U& grid) const { return grid[x][y]; }
	template<typename U, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	const auto& GetAt(const U& grid) const { return grid[x][y]; }

	T x = {}, y = {};
};
using Point = Coord<int>;
constexpr std::array<Point, 4> directions = { Point(1, 0), Point(0, 1), Point(-1, 0), Point(0,-1) };

struct Reindeer
{
	Point pos;
	int dir = 1;
	constexpr bool operator < (const Reindeer& r) const { if (pos < r.pos) return true; else if (r.pos < pos) return false; else return dir < r.dir; }
};

int main(int argc, char* argv[])
{
	auto ChronoStart = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: AoC24Day16.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	
	std::string line;
	std::vector<std::string> grid;
	while (in >> line)
		grid.push_back(std::move(line));
	
	const Point Start = {(int)grid.size() - 2, 1}, End = {1, (int)grid[1].size()-2};
	Start.GetAt(grid) = '.';
	End.GetAt(grid) = '.';

	std::map<Reindeer, int> minCosts, cur, next;
	auto InsertNext = [&next](Reindeer r, int c)
	{
		if (auto iter = next.find(r); iter == next.cend() || iter->second > c)
			next[r] = c;
	};

	Reindeer startDeer = {Start, 1}, testDeer1 = {End, 1}, testDeer2 = {End, 2};
	cur[startDeer] = 0;
	while (!cur.empty())
	{
		for (auto [r, c] : cur)
		{
			if (auto iter = minCosts.find(r); iter != minCosts.cend() && iter->second < c)
				continue;
			minCosts[r] = c;
			if (Point n = r.pos + directions[r.dir]; n.GetAt(grid) != '#')
				InsertNext({n, r.dir}, c+1);
			if (Point t = r.pos + directions[(r.dir + 3)%4]; t.GetAt(grid) != '#')
				InsertNext({r.pos, (r.dir + 3) % 4 }, c + 1000);
			if (Point t = r.pos + directions[(r.dir + 1) %4]; t.GetAt(grid) != '#')
				InsertNext({r.pos, (r.dir + 1) % 4 }, c + 1000);
		}

		std::swap(cur, next);
		next.clear();
	}

	int part1 = std::numeric_limits<int>::max(), dir = -3;
	for (int i = 0; i < 4; ++i)
		if (auto iter = minCosts.find({ End, i }); iter != minCosts.cend())
			if(part1 > iter->second)
			{
				dir = i;
				part1 = iter->second;
			}

	std::set<Point> paths;
	std::set<Reindeer> sCur = {Reindeer{End, dir}}, sNext;

	while (!sCur.empty())
	{
		for (auto r : sCur)
		{
			paths.insert(r.pos);
			int cost = minCosts[r];
			Reindeer back = {r.pos + directions[(r.dir + 2)%4], r.dir};
			if (auto iter = minCosts.find(back); iter != minCosts.cend() && iter->second == cost - 1)
				sNext.insert(back);
			Reindeer a = {r.pos, (r.dir + 1) % 4};
			if (auto iter = minCosts.find(a); iter != minCosts.cend() && iter->second == cost - 1000)
				sNext.insert(a);
			Reindeer b = { r.pos, (r.dir + 3) % 4 };
			if (auto iter = minCosts.find(b); iter != minCosts.cend() && iter->second == cost - 1000)
				sNext.insert(b);
		}
		
		std::swap(sCur, sNext);
		sNext.clear();
	}

	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, paths.size());
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}