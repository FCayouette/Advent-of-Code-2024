import std.core;

#define ALL(x) (x).begin(),(x).end()
#define ALLc(x) (x).cbegin(),(x).cend()

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: AoC24Day01.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	int part1 = 0, part2 = 0, a, b;
	std::vector<int> left, right;
	while (in >> a >> b)
	{
		left.push_back(a);
		right.push_back(b);
	}

	std::sort(ALL(left));
	std::sort(ALL(right));

	for (int i = 0; i < left.size(); ++i)
	{
		part1 += std::abs(left[i] - right[i]);
		part2 += std::count(ALLc(right), left[i]) * left[i];
	}
		
	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, part2);
}