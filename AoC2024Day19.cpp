import std.core;

using u64 = unsigned long long;

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
		std::cout << "Usage: AoC24Day19.exe inputFilename\n";
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
	std::getline(in, line);
	std::vector<std::string> patterns = SplitAt(line, ", ");
	std::unordered_map<std::string, u64> memo;
	auto Find = y_combinator([&patterns, &memo](auto&& Find, std::string s)
	{
		if (auto iter = memo.find(s); iter != memo.cend())
			return iter->second;
		u64 result = 0;
		for (const std::string& p : patterns)
			if (p.size() <= s.size()) 
			{
				if (s == p) 
					++result;
				else if (s.starts_with(p))
					result += Find(s.substr(p.size()));
			}
		memo[s] = result;
		return result;
	});

	while (in >> line)
		if (u64 total = Find(line))
		{
			++part1;
			part2 += total;
		}


	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, part2);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}