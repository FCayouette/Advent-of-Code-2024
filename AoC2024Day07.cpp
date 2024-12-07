import std.core;

#define ALL(x) (x).begin(),(x).end()
#define ALLc(x) (x).cbegin(),(x).cend()
#define ALLr(x) (x).rbegin(),(x).rend()
#define ALLcr(x) (x).crbegin(),(x).crend()

using u64 = unsigned long long;
using i64 = long long;

std::pair<std::string, std::string> SplitAtFirst(std::string s, std::string token)
{
	auto pos = s.find(token);
	if (pos == std::string::npos)
		return std::make_pair(std::move(s), std::string{});
	return std::make_pair(s.substr(0, pos), s.substr(pos + token.size()));
}

enum Op : char
{
	ADD = 0,
	MUL = 1,
	CONC = 2,
};

int main(int argc, char* argv[])
{
	auto ChronoStart = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: AoC24Day07.exe inputFilename\n";
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

	while (std::getline(in, line))
	{
		auto [total, values] = SplitAtFirst(line, ": ");
		std::vector<u64> vals, previous;
		std::stringstream ss(values);
		u64 v, target;
		while (ss >> v)
			vals.push_back(v);
		std::vector<Op> operations;
		for (int i = 0; i < vals.size()-1; ++i)
			operations.push_back(ADD);

		target = stoull(total);
		
		bool found = false;
		previous = {vals.front()};
		while (true)
		{
			for (int i = 1; i < vals.size(); ++i)
				if (previous.size() <= i)
				{
					u64 r = previous.back();
					if (operations[i-1] == ADD)
						r += vals[i];
					else
						r *= vals[i];
					previous.push_back(r);
				}
			
			if (u64 t = previous.back(); t == target)
			{
				part1 += t;
				part2 += t;
				found = true;
				break;
			}
			int i = operations.size()-1;
			while (i >= 0)
			{
				previous.pop_back();
				if (operations[i] == MUL)
					operations[i--] = ADD;
				else 
				{
					operations[i] = MUL;
					break;
				};
			}
			if (i < 0) break;
		}
		if (!found)
		{
			bool ok = true;
			
			operations.back() = CONC;
			while (ok)
			{
				for (int i = std::max(1ull, previous.size()); i < vals.size(); ++i)
				{
					u64 r = previous.back();
					if (operations[i-1] == ADD)
						r += vals[i];
					else if (operations[i-1] == MUL)
						r *= vals[i];
					else
					{
						u64 v = vals[i];
						while (v > 0)
						{
							r *= 10ull;
							v /= 10ull;
						}
						r += vals[i];
					}
					previous.push_back(r);
				}
				if (u64 t = previous.back(); t == target)
				{
					part2 += t;
					break;
				}
				int i = operations.size()-1;
				while (i >= 0)
				{
					previous.pop_back();
					if (operations[i] == CONC)
						operations[i--] = ADD;
					else
					{
						operations[i] = (operations[i] == ADD ? MUL : CONC);
						break;
					};
				}
				if (i < 0) 
					ok = false;
			}
		}
	}


	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, part2);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}