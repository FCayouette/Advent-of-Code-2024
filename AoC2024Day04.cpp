import std.core;

#define ALLc(x) (x).cbegin(),(x).cend()

template<typename T>
struct Coord
{
	constexpr bool operator <  (const Coord& p) const { if (x < p.x) return true; else if (p.x < x) return false; else return y < p.y; }
	constexpr Coord operator + (const Coord& p) const { return Coord(x + p.x, y + p.y); }
	constexpr Coord operator * (T scalar) const { return Coord(x * scalar, y * scalar); }
	T x = {}, y = {};
};

using Point = Coord<int>;
constexpr std::array<Point, 8> directions = { Point(1, 0), Point(0, 1), Point(-1, 0), Point(0,-1), Point(-1,-1), Point(-1, 1), Point(1,-1), Point(1,1) };
constexpr std::array<Point, 4> cross = { Point(-1,-1), Point(-1, 1), Point(1,1), Point(1,-1) };
constexpr std::array<char,4> XMAS = {'X', 'M', 'A', 'S'};

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: AoC24Day04.exe inputFilename\n";
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
	std::vector<std::string> map;

	while (in >> line)
		map.push_back(std::move(line));

	for (int i = 0; i < map.size(); ++i)
		for (int j = 0; j < map.front().size(); ++j)
		{
			if (Point p{ i, j }; map[i][j] == 'X')
				for (Point d : directions)
				{
					bool ok = true;
					for (int x = 0; x < 4 && ok; ++x)
						if (Point np = p + (d * x); 
							np.x < 0 || np.y < 0 || np.x >= map.size() || np.y >= map.front().size() || map[np.x][np.y] != XMAS[x])
							ok = false;
					if (ok)
						++part1;
				}
			else if (map[i][j] == 'A')
			{
				std::array<char, 4> check = { 0, 0, 0, 0 };
				for (int i = 0; i < 4; ++i)
				{
					Point np = p + cross[i];
					if (np.x < 0 || np.y < 0 || np.x >= map.size() || np.y >= map.front().size())
						break;
					if (char c = map[np.x][np.y]; c == 'S' || c == 'M')
						++check[(i % 2 == 0 ? 0 : 2) + (c == 'S' ? 0 : 1)];
					else
						break;
				}
				part2 += std::find(ALLc(check), 0) == check.cend();
			}
		}
	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, part2);
}