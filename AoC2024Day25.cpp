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
		if (line.empty()) break;
		bool isLock = line.find('#') != std::string::npos;
		std::vector<std::string> pattern = {std::move(line)};
		for (int i = 0; i < 6; ++i)
		{
			in >> line;
			pattern.push_back(std::move(line));
		}
		
		if (Pin p; isLock)
		{
			for (int x = 0; x < 5; ++x)
				for (int i = 1; i < 7; ++i)
					if (pattern[i][x] == '.')
					{
						p[x] = i-1;
						break;
					}
			locks.push_back(std::move(p));
		}
		else
		{
			for (int x = 0; x < 5; ++x)
				for (int i = 5; i >= 0; --i)
					if (pattern[i][x] == '.')
					{
						p[x] = 5-i;
						break;
					}
			keys.push_back(std::move(p));
		}
	}

	int answer = 0;
	for (const auto& l : locks)
		for (const auto& k : keys)
		{
			bool ok = true;
			for (int i = 0; ok && i < 5; ++i)
				if (l[i]+k[i]>5)
					ok = false;
			answer += ok;
		}

	std::cout << std::format("Answer: {}\n", answer);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}