import std.core;

int main(int argc, char* argv[])
{
	auto ChronoStart = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: AoC2024Day25.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	std::string line;
	using Pin = std::array<int, 5>;

	std::vector<Pin> locks, keys;
	while (in >> line)
	{
		bool isLock = line.find('#') != std::string::npos;
		Pin current = (isLock ? Pin{0, 0, 0, 0, 0} : Pin{-1,-1,-1,-1,-1});
		for (int i = 0; i < 6; ++i)
		{
			in >> line;
			for (int j = 0; j < 5; ++j)
				current[j] += line[j] == '#';
		}
		(isLock ? locks : keys).push_back(std::move(current));
	}

	int answer = 0;
	for (const auto& l : locks)
		for (const auto& k : keys)
			answer += std::inner_product(l.cbegin(), l.cend(), k.cbegin(), true, std::bit_and(), [](int a, int b) { return a + b < 6;});

	std::cout << std::format("Answer: {}\n", answer);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}