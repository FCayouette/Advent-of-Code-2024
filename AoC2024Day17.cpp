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

int main(int argc, char* argv[])
{
	auto ChronoStart = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: AoC24DayXX.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	std::string line;
	std::array<u64, 3> reg;
	std::vector<u64> program;

	for (int i = 0; i < 3; ++i)
	{
		std::getline(in, line);
		auto [_, v] = SplitAtFirst(std::move(line), ": ");
		reg[i] = stoull(v);
	}
	std::getline(in, line);
	in >> line >> line;
	for (const std::string& s: SplitAt(line, ","))
		program.push_back(stoi(s));

	auto Run = [&program, &reg](u64 A) -> std::string
	{
		std::string output;
		std::array<u64, 3> registers = reg;
		auto GetCombo = [&registers](u64 op)
		{
			if (op >= 0 && op <= 3)
				return op;
			return registers[op - 4];
		};

		registers[0] = A;
		int pc = 0;
		while (pc < program.size())
			switch (u64 inst = program[pc], operand = program[pc + 1]; inst)
			{
			case 0:
				registers[0] /= (1 << GetCombo(operand));
				pc += 2;
				break;
			case 1:
				registers[1] ^= operand;
				pc += 2;
				break;
			case 2:
				registers[1] = GetCombo(operand) % 8ull;
				pc += 2;
				break;
			case 3:
				if (registers[0])
					pc = operand;
				else
					pc += 2;
				break;
			case 4:
				registers[1] ^= registers[2];
				pc += 2;
				break;
			case 5:
				if (char c = ('0' + (char)(GetCombo(operand) % 8ull)); output.empty())
					output = c;
				else
				{
					output += ',';
					output += c;
				}
				pc += 2;
				break;
			case 6:
				registers[1] = registers[0] / (1 << GetCombo(operand));
				pc += 2;
				break;
			case 7:
				registers[2] = registers[0] / (1 << GetCombo(operand));
				pc += 2;
				break;
			}
		
		return output;
	};

	u64 part2 = 0, digits = 1;
	std::string toGet = line.substr(line.size() - (2 * digits - 1));
	while (true)
	{
		if (std::string s = Run(part2); s == line)
			break;
		else if (s == toGet)
		{
			digits++;
			toGet = line.substr(line.size() - (2*digits-1));
			part2 *= 8ull;
		}
		else
			++part2;
	}
	std::cout << std::format("Part 1: {}\nPart 2: {}\n", Run(reg[0]), part2);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}