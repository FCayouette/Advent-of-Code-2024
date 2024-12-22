import std.core;

using u64 = unsigned long long;

int main(int argc, char* argv[])
{
	auto ChronoStart = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: AoC2024Day22.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	u64 part1 = 0, part2 = 0;
	std::string line;
	using Sequence = std::array<int, 4>;
	std::map<Sequence, u64> scores;

	while (in >> line)
	{
		u64 s = stoull(line);
		Sequence seq;
		std::map<Sequence, u64> monkey;
		for (int i = 0, cur = s % 10, prev; i < 2000; ++i)
		{
			s ^= s << 6;
			s &= 0xffffff;
			s ^= s >> 5;
			s &= 0xffffff;
			s ^= s << 11;
			s &= 0xffffff;
			prev = std::exchange(cur, s%10);
			for (int j = 0; j < 3; ++j)
				seq[j] = seq[j+1];
			seq[3] = cur - prev;
			if (i > 3 && monkey.find(seq) == monkey.cend())
				monkey[seq] = cur;
		}
		for (const auto& [a, c] : monkey)
			scores[a] += c;
		part1 += s;
	}
	for (const auto& [_, c] : scores)
		part2 = std::max(part2, c);

	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, part2);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}