import std.core;

std::vector<std::string> SplitAt(std::string s, std::string token)
{
	std::vector<std::string> results;
	auto pos = s.find(token), tSize = token.size();
	while (pos != std::string::npos)
	{
		results.push_back(s.substr(0, pos));
		s = s.substr(std::min(pos + tSize, s.length()));
		pos = s.find(token);
	}
	if (!s.empty())
		results.push_back(std::move(s));
	return results;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: AoC24Day05.exe inputFilename\n";
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
	std::vector<std::pair<int, int>> invalids;

	while (std::getline(in, line))
	{
		if (line.empty())
			break;
		auto t = SplitAt(line, "|");
		invalids.emplace_back(stoi(t[1]), stoi(t[0]));
	}

	std::sort(invalids.begin(), invalids.end());

	auto ValidPair = [&invalids](int a, int b)
	{
		return !std::binary_search(invalids.cbegin(), invalids.cend(), std::make_pair(a,b));
	};

	while (std::getline(in, line))
	{
		std::vector<int> v;
		for (const auto& s : SplitAt(line, ","))
			v.push_back(stoi(s));

		bool valid = true;
		for (int i = 0; valid && i < v.size()-1; ++i)
			for (int j = i + 1; valid && j < v.size(); ++j)
				valid = ValidPair(v[i], v[j]);

		if (valid)
			part1 += v[v.size() /2];
		else
		{
			for (int i = 0; i <= v.size()/2; ++i)
				for (int j = i + 1; j < v.size(); ++j)
						if (!ValidPair(v[i], v[j]))
							std::swap(v[i], v[j]);
				
			part2 += v[v.size()/2];
		}
	}
	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, part2);
}