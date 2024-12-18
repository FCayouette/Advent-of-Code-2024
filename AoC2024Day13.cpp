import std.core;

using i64 = long long;

std::pair<std::string, std::string> SplitAtFirst(std::string s, std::string token)
{
	auto pos = s.find(token);
	if (pos == std::string::npos)
		return std::make_pair(std::move(s), std::string{});
	return std::make_pair(s.substr(0, pos), s.substr(pos + token.size()));
}

std::pair<i64, i64> GetValues(std::string s)
{
	auto [_, values] = SplitAtFirst(std::move(s), "+");
	auto [X, Y] = SplitAtFirst(std::move(values), ", Y+");
	return std::make_pair(stoll(X), stoll(Y));
}

int main(int argc, char* argv[])
{
	auto ChronoStart = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: AoC24Day13.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	i64 part1 = 0, part2 = 0;

	while (true)
	{
		std::string line;
		std::getline(in, line);
		auto [a, c] = GetValues(std::move(line));
		std::getline(in, line);
		auto [b, d] = GetValues(std::move(line));
		std::getline(in, line);
		auto [_, values] = SplitAtFirst(line, "=");
		auto [X, Y] = SplitAtFirst(std::move(values), ", Y=");
		i64 tX = stoll(X), tY = stoll(Y);

		auto Solve = [a, b, c, d](i64 tX, i64 tY) -> std::optional<i64>
		{
			if (i64 invDet = a * d - b * c, tmpA = d * tX - b * tY, tmpB = a * tY - c * tX;
				tmpA % invDet == 0 && tmpB % invDet == 0)
				return (3 * tmpA + tmpB) / invDet;
			return {};
		};
		if (auto p1 = Solve(tX, tY))
			part1 += *p1;

		if (auto p2 = Solve(tX + 10000000000000ll, tY + 10000000000000ll))
			part2 += *p2;
		
		if (!std::getline(in, line)) break;
	}

	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, part2);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}