import std.core;

using u64 = unsigned long long;

int main(int argc, char* argv[])
{
	auto ChronoStart = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: AoC24Day11.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	u64 part1 = 0, part2 = 0, val;
	std::unordered_map<u64, u64> pebbles, work;
	while (in >> val)
		++pebbles[val];

	std::vector powers = { 1ull };
	for (int i = 1; i < 18; ++i)
		powers.push_back(10ull * powers.back());

	for (int i = 0; i < 75; ++i)
	{
		if (i == 25)
			for (auto [_, x] : pebbles)
				part1 += x;

		for (auto [v, count] : pebbles)
		{
			if (v == 0)
				work[1ull] += count;
			else
			{
				int digits = 1;
				while (v >= powers[digits])
					++digits;
				if (digits % 2 == 1)
					work[v * 2024ull] += count;
				else
				{
					digits /= 2;
					work[v % powers[digits]] += count;
					work[v / powers[digits]] += count;
				}
			}
		}
		std::swap(work, pebbles);
		work.clear();
	}

	for (auto [_, x] : pebbles)
		part2 += x;

	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, part2);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}