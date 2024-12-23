import std.core;

std::pair<std::string, std::string> SplitAtFirst(std::string s, std::string token)
{
	auto pos = s.find(token);
	if (pos == std::string::npos)
		return std::make_pair(std::move(s), std::string{});
	return std::make_pair(s.substr(0, pos), s.substr(pos + token.size()));
}

int main(int argc, char* argv[])
{
	auto ChronoStart = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: AoC2024Day23.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	int part1 = 0;
	std::string line, part2;
	std::map<std::string, std::set<std::string>> connections;

	while (in >> line)
	{
		auto [a, b] = SplitAtFirst(std::move(line), "-");
		connections[a].insert(b);
		connections[b].insert(std::move(a));
	}
	auto TryAdd = [&connections](std::vector<std::string>& lan, const std::string& s)
	{
		bool result = true;
		for (int i = 0; result && i < lan.size(); ++i)
			result = connections[lan[i]].contains(s);
		if (result)
			lan.push_back(s);
		return result;
	};

	for (const auto& [a, aLinks] : connections)
		for (const auto& [b, bLinks] : connections)
		{
			if (b <= a || !aLinks.contains(b)) continue;
			for (const std::string& c : bLinks)
				if (b < c && aLinks.contains(c) && (a[0] == 't' || b[0] == 't' || c[0] == 't'))
					++part1;

			int lanSize = 2;
			std::vector<std::string> lan = { a, b };
			std::string lanString = a + "," + b;
			for (const auto& [c, _] : connections)
				if (c > b && TryAdd(lan, c))
					lanString += "," + c;
				
			if (lanString.size() > part2.size())
				part2 = lanString;
		}

	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, part2);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}