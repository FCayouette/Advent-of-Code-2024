import std.core;

using u64 = unsigned long long;

std::pair<std::string, std::string> SplitAtFirst(std::string s, std::string token)
{
	auto pos = s.find(token);
	if (pos == std::string::npos)
		return std::make_pair(std::move(s), std::string{});
	return std::make_pair(s.substr(0, pos), s.substr(pos + token.size()));
}

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

enum OP
{
	AND = 0,
	OR = 1,
	XOR = 2
};

struct Stage
{
	std::string left, right;
	OP op;
};

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
		std::cout << "Usage: AoC2024Day24.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	u64 part1 = 0, X = 0, Y = 0;
	std::string line, part2;
	std::map<std::string, bool> knowns, previousKnowns;
	std::map<std::string, Stage> circuit;
	
	while (true)
	{
		std::getline(in, line);
		if (line.empty()) break;

		auto [n,v] = SplitAtFirst(line, ": ");
		knowns[n] = (v[0] == '1');
		if ((v[0] == '1'))
		{
			if (n[0] == 'x')
				X |= (1ull << stoi(n.substr(1)));
			else
				Y |= (1ull << stoi(n.substr(1)));
		}
	}
	previousKnowns = knowns;

	while (std::getline(in, line))
	{
		auto [operation, target] = SplitAtFirst(line, " -> ");
		auto tokens = SplitAt(operation, " ");
		Stage s{tokens[0], tokens[2], (tokens[1][0] == 'A' ? OP::AND : (tokens[1][0] == 'O' ? OP::OR : OP::XOR))};
		circuit[target]  = s;
	}

	auto Evaluate = y_combinator([&knowns, &circuit](auto&& Evaluate, const std::string& s) -> bool
	{
		if (knowns.contains(s)) return knowns[s];
		const Stage& stage = circuit[s];
		bool l = Evaluate(stage.left), r = Evaluate(stage.right), result;
		if (stage.op == OP::AND)
			result = l && r;
		else if (stage.op == OP::OR)
			result = l || r;
		else result = l ^ r;
		knowns[s] = result;
		return result;
	});

	auto zIter = circuit.find("z00");
	u64 mask = 0, iter = 0;
	while (zIter != circuit.cend())
	{
		previousKnowns = knowns;
		if (Evaluate(zIter->first))
			part1 |= (1ull << stoi(zIter->first.substr(1)));

		for (const auto& k: knowns)
			if (!previousKnowns.contains(k.first))
			{
				const Stage st = circuit[k.first];
				std::cout << std::format("{} {} {} -> {}\n", st.left, !st.op ? "AND" : (st.op == OP::OR ? "OR" : "XOR"), st.right, k.first);
			}
		mask = (mask << 1) | 1ull;
		std::cout << std::format("{}: X+Y = {}  {}\n", iter++, (X+Y) & mask, part1);
		++zIter;
	}

	std::vector<std::string> pairs= {"z09", "hnd", "z16", "tdv", "z23", "bks", "tjp", "nrn"};

	std::sort(pairs.begin(), pairs.end());
	for (const std::string& s: pairs)
		part2 += s + ",";
	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, part2.substr(0, part2.size()-1));
	//bks,hnd,z09,z23
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}