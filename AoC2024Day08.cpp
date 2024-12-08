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
	constexpr Coord operator - (const Coord& p) const { return Coord(x - p.x, y - p.y); }
	constexpr bool operator != (const Coord& p) const { return !IsEqual(x, p.x) || !IsEqual(y, p.y); }

	T x = {}, y = {};
};
using Point = Coord<int>;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: AoC24Day08.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	int X = 0, Y;
	std::string line;
	std::map<char, std::vector<Point>> antennas;

	while (in >> line)
	{
		for (int i = 0; i < line.size(); ++i)
			if (char c = line[i]; c != '.')
				antennas[c].push_back({X, i});
		Y = line.size();
		++X;
	}

	auto Within = [X, Y](Point p) { return p.x >= 0 && p.y >= 0 && p.x < X && p.y < Y; };

	std::set<Point> antinodes, resonant;
	for (auto& [_, points] : antennas)
		for (const Point a : points)
			for (const Point b : points )
				if (a != b)
				{
					Point delta = b-a, anti = b + delta;
					resonant.insert(b);
					if (Within(anti))
					{
						antinodes.insert(anti);
						do 
						{
							resonant.insert(anti);
							anti += delta;
						} while (Within(anti));
					}
				}

	std::cout << std::format("Part 1: {}\nPart 2: {}\n", antinodes.size(), resonant.size());
}