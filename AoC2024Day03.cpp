import std.core;

using u64 = unsigned long long;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: AoC24Day03.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	u64 part1 = 0, part2 = 0;
	std::string part, line;
	
	while (std::getline(in, part))
		line += part;

	auto pos = line.find("mul("), dont = line.find("don't()");

	auto ProcessMul = [&line, &pos]()
	{
		pos += 4;
		u64 a = 0, b = 0, result = 0;
		while (std::isdigit(line[pos]))
			a = a * 10 + line[pos++] - '0';

		if (line[pos] == ',')
		{
			++pos;
			while (std::isdigit(line[pos]))
				b = b * 10 + line[pos++] - '0';
			if (line[pos] == ')')
				result = a * b;
		}
		pos = line.find("mul(", pos);
		return result;
	};

	while (pos != std::string::npos)
		part1 += ProcessMul();
	
	pos = line.find("mul(");
	while (pos != std::string::npos)
	{
		if (pos >= dont)
		{
			auto p = line.find("do()", dont);
			pos = line.find("mul(", p);
			dont = line.find("don't()", p);
		}
		else
			part2 += ProcessMul();
	}

	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, part2);
}