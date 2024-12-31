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

enum OP : char
{
	AND = 0,
	OR = 1,
	XOR = 2
};

struct Stage
{
	std::string left, right;
	OP op;
	std::string ToString() const { return std::format("{} {} {}", left,op== 0 ? "AND" : op == 1 ? "OR " : "XOR", right); }
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

	u64 part1 = 0, X = 0, Y = 0, addition;
	std::string line, part2;
	std::map<std::string, bool> knowns, previousKnowns, numbers;
	std::map<std::string, Stage> circuit;
	std::map<std::string, std::vector<std::string>> usedBy;
	
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
	numbers = previousKnowns = knowns;
	addition = X+Y;
	
	std::set<std::string> orOp, andOp, fullAdd;
	std::vector<std::string> halfAdd(knowns.size()/2), halfCarry(knowns.size()/2);
	while (std::getline(in, line))
	{
		auto [operation, target] = SplitAtFirst(line, " -> ");
		auto tokens = SplitAt(operation, " ");
		OP op = (tokens[1][0] == 'A' ? OP::AND : (tokens[1][0] == 'O' ? OP::OR : OP::XOR));
		bool isBase = tokens[0][0] == 'x' || tokens[0][0] == 'y';
		Stage s{tokens[0], tokens[2], op};
		circuit[target]  = s;
		if (op == OP::OR)
			orOp.insert(target);
		else if (isBase)
		{
			if (op == OP::AND)
				halfCarry[stoi(tokens[0].substr(1))] = target;
			else halfAdd[stoi(tokens[0].substr(1))] = target;
		}
		else
			(op == OP::AND ? andOp : fullAdd).insert(target);
		
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

	auto CycleCheck = y_combinator([&circuit](auto && CycleCheck, const std::string & s, std::vector<std::string>&chain) ->bool
	{
		if (std::find(chain.cbegin(), chain.cend(), s) != chain.cend())
			return true;
		auto iter = circuit.find(s);
		if (iter == circuit.cend())
			return false;
		chain.push_back(s);
		const Stage& st = iter->second;
		if (CycleCheck(st.left, chain) || CycleCheck(st.right, chain))
			return true;
		chain.pop_back();
		return false;
	});

	auto zIter = circuit.find("z00");
	while (zIter != circuit.cend())
	{
		if (Evaluate(zIter->first))
			part1 |= (1ull << stoi(zIter->first.substr(1)));
		++zIter;
	}
	u64 mask = 0, iter = 0, curBit = 1;
	std::vector<std::string> swaps;

	zIter = circuit.find("z00");
	knowns = numbers;
	auto GetStepDiffence = [&knowns, &previousKnowns]()
	{
		std::vector<std::string> step;
		for (const auto& k : knowns)
			if (!previousKnowns.contains(k.first))
				step.push_back(k.first);
		return step;
	};

	while (zIter != circuit.cend() && swaps.size() < 8)
	{
		previousKnowns = knowns;
		bool bitValue = Evaluate(zIter->first);
		std::vector<std::string> step = GetStepDiffence();
		//for (const std::string& s: step)
		//	std::cout << std::format("{} -> {}\n", circuit[s].ToString(), s);
		int expected = (!iter ? 1 : iter == 1 ? 3 : 5); // incorrect for last
		if ((addition & curBit) != (bitValue << iter) || step.size() != expected)
		{
			Stage zStage = circuit[zIter->first];
			OP op = zStage.op;
			if (op != OP::XOR)
			{
				std::string a = halfAdd[iter];
				for (const std::string& f : fullAdd)
					if (const Stage& st = circuit[f]; st.left == a || st.right == a)
					{
						swaps.push_back(zIter->first);
						swaps.push_back(f);
						std::swap(circuit[f], circuit[zIter->first]);
					}
			}
			else
			{
				std::vector<std::string> toSwap;
				if (char c = zStage.left[0]; c == 'x' || c == 'y')
				{
					// Incorrect XOR operation
					std::string hc = halfCarry[iter-1], o = *std::find_if(orOp.cbegin(), orOp.cend(), [&](const std::string& s){return circuit[s].left == hc || circuit[s].right == hc;});
					std::string x = *std::find_if(fullAdd.cbegin(), fullAdd.cend(), [&](const std::string& s) {return circuit[s].left == o || circuit[s].right== o;});
					toSwap.push_back(zIter->first);
					toSwap.push_back(x);
				}
				else if (const auto& ha = halfAdd[iter]; ha != zStage.left && ha != zStage.right)
				{
					// disconnected half add operation
					toSwap.push_back(ha);
					std::string orSide = (orOp.contains(zStage.left) ? zStage.left : zStage.right);
					if (const auto& hc = halfCarry[iter-1]; hc != circuit[orSide].left && hc != circuit[orSide].right)
						toSwap.push_back(hc);
					else
					{
						std::string xorSide = zStage.left == orSide ? zStage.right : zStage.left;
						if (ha != xorSide)
							toSwap.push_back(xorSide);
						else
						{
							std::cout << "!";
						}
					}
				}
				else if ((ha == zStage.left && circuit[zStage.right].op != OP::OR) ||(ha == zStage.right && circuit[zStage.left].op != OP::OR))
				{
					// Disconnected or operation
					toSwap.push_back(ha != zStage.left ? zStage.left : zStage.right);
					std::string hc = halfCarry[iter - 1];
					toSwap.push_back(*std::find_if(orOp.cbegin(), orOp.cend(), [&](const std::string& s) {return circuit[s].left == hc || circuit[s].right == hc; }));
					
				}
				else 
					std::cout << "!";
				swaps.insert(swaps.end(), toSwap.cbegin(), toSwap.cend());
				std::swap(circuit[toSwap[0]], circuit[toSwap[1]]);
			}
			knowns = previousKnowns;
			Evaluate(zIter->first); // set circuit to correct values
		}
		++zIter;
		++iter;
		curBit <<= 1;
	}

	std::sort(swaps.begin(), swaps.end());
	for (const std::string& s: swaps)
		part2 += s + ",";
	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, part2.substr(0, part2.size()-1));
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}