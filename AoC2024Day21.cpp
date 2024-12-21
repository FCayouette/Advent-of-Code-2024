import std.core;

using u64 = unsigned long long;

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
	constexpr bool operator!=(const Coord& p) const { return !IsEqual(x, p.x) || !IsEqual(y, p.y); }
	
	T x = {}, y = {};
};
using Point = Coord<int>;

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
		std::cout << "Usage: AoC2024Day21.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}
	std::map<Point, char> numKeypad, dirKeypad;
	numKeypad[{ 0,0 }] ='7';
	numKeypad[{ 0,1 }] ='8';
	numKeypad[{ 0,2 }] ='9';
	numKeypad[{ 1,0 }] ='4';
	numKeypad[{ 1,1 }] ='5';
	numKeypad[{ 1,2 }] ='6';
	numKeypad[{ 2,0 }] ='1';
	numKeypad[{ 2,1 }] ='2';
	numKeypad[{ 2,2 }] ='3';
	numKeypad[{ 3,1 }] ='0';
	numKeypad[{ 3,2 }] ='A';

	dirKeypad[{0, 1}] = '^';
	dirKeypad[{0, 2}] = 'A';
	dirKeypad[{1, 0}] = '<';
	dirKeypad[{1, 1}] = 'v';
	dirKeypad[{1, 2}] = '>';

	auto MakeMinDir = [](const std::map<Point, char>& numKeypad, Point Illegal)
	{
		std::map<std::pair<char, char>, std::vector<std::vector<char>>> numShortests;
		for (auto [sp, a] : numKeypad)
			for (auto [ep, b] : numKeypad)
			{
				std::pair<char, char> target{ a, b };
				if (a != b)
				{
					if (sp.x == ep.x)
					{
						char c = ep.y > sp.y ? '>' : '<';
						std::vector<char> seq;
						for (int i = 0, maxI = std::abs(ep.y - sp.y); i < maxI; ++i)
							seq.push_back(c);
						seq.push_back('A');
						numShortests[target].push_back(std::move(seq));
					}
					else if (sp.y == ep.y)
					{
						char c = ep.x > sp.x ? 'v' : '^';
						std::vector<char> seq;
						for (int i = 0, maxI = std::abs(ep.x - sp.x); i < maxI; ++i)
							seq.push_back(c);
						seq.push_back('A');
						numShortests[target].push_back(std::move(seq));
					}
					else
					{
						std::vector<Point> corners;
						std::array<Point, 4> tests;
						tests[0] = Point(std::min(sp.x, ep.x), std::min(sp.y, ep.y));
						tests[1] = Point(std::min(sp.x, ep.x), std::max(sp.y, ep.y));
						tests[2] = Point(std::max(sp.x, ep.x), std::min(sp.y, ep.y));
						tests[3] = Point(std::max(sp.x, ep.x), std::max(sp.y, ep.y));
						for (Point p : tests)
							if (p != sp && p != ep && p != Illegal)
								corners.push_back(p);

						for (Point t : corners)
						{
							std::vector<char> seq;
							if (sp.x == t.x)
							{
								char c = t.y > sp.y ? '>' : '<';
								for (int i = 0, maxI = std::abs(t.y - sp.y); i < maxI; ++i)
									seq.push_back(c);
								c = ep.x > t.x ? 'v' : '^';
								for (int i = 0, maxI = std::abs(ep.x - sp.x); i < maxI; ++i)
									seq.push_back(c);
							}
							else
							{
								char c = t.x > sp.x ? 'v' : '^';
								for (int i = 0, maxI = std::abs(t.x - sp.x); i < maxI; ++i)
									seq.push_back(c);
								c = ep.y > t.y ? '>' : '<';
								for (int i = 0, maxI = std::abs(ep.y - t.y); i < maxI; ++i)
									seq.push_back(c);
							}
							seq.push_back('A');
							numShortests[target].push_back(std::move(seq));
						}

					}
				}
				else
					numShortests[target].push_back({'A'});
			}
		return numShortests;
	};

	std::map<std::pair<char, char>, std::vector<std::vector<char>>> numShortests = MakeMinDir(numKeypad, Point(3, 0)), keyShortests = MakeMinDir(dirKeypad, Point(0, 0));
	std::vector memo(1, std::map<std::pair<char, char>, u64>{});
	
	auto Memoize = y_combinator([&keyShortests, &memo](auto&& Memoize, std::pair<char, char> target, int depth) ->u64
	{
		const int MaxDepth = 25;
		if (memo.size() <= depth)
			memo.push_back({});
		if (auto iter = memo[depth].find(target); iter != memo[depth].cend())
			return iter->second;
		if (depth == MaxDepth)
			return memo[depth][target] = keyShortests[target].front().size();

		u64 minResult = std::numeric_limits<u64>::max();
		for (const auto& p : keyShortests[target])
		{
			u64 result = 0;
			char prev = 'A';
			for (char c : p)
				result += Memoize(std::pair{ std::exchange(prev, c), c }, depth + 1);
			minResult = std::min(minResult, result);
		}
		memo[depth][target] = minResult;
		return minResult;
	});

	u64 part1 = 0, part2 = 0;
	std::string line;

	while (in >> line)
	{
		char numRobot = 'A';
		u64 longCount = 0, shortCount = 0;
		for (char c : line)
		{
			std::pair numWork(numRobot, c);
			numRobot = c;
			u64 expandedMin = std::numeric_limits<u64>::max(), shortMin = expandedMin;
			for (const auto& p : numShortests[numWork])
			{
				u64 result = 0, shortResult = 0;
				char prev = 'A';
				for(char t : p)
				{
					std::pair test = std::pair{ std::exchange(prev, t), t };
					result += Memoize(test, 1);
					shortResult += Memoize(test, 24);
				}
				
				expandedMin = std::min(expandedMin, result);
				shortMin = std::min(shortMin, shortResult);
			}
			shortCount += shortMin;
			longCount += expandedMin;
		}
		u64 lineValue = stoull(line.substr(0, line.size() - 1));
		part1 += lineValue * shortCount;
		part2 += lineValue * longCount;
	}
	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, part2);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}