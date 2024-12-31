import std.core;

#define ALL(x) (x).begin(),(x).end()
#define ALLc(x) (x).cbegin(),(x).cend()

int main(int argc, char* argv[])
{
	auto ChronoStart = std::chrono::high_resolution_clock::now();
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

	int part2 = 0, a, b;
	std::vector<int> left, right;
	while (in >> a >> b)
	{
		left.push_back(a);
		right.push_back(b);
	}

	std::sort(ALL(left));
	std::sort(ALL(right));

	auto l = left.cbegin(), r = right.cbegin();
	while (l != left.cend() && r != right.cend())
	{
		int countL = 1, countR = 0;
		while (l + countL != left.cend() && *(l+countL) == *l)
			++countL;
		while (r != right.cend() && *r < *l)
			++r;
		while (r != right.cend() && *r == *l)
			++countR, ++r;
		part2 += *l * countL * countR;
		l += countL;
	}
		
	std::cout << std::format("Part 1: {}\nPart 2: {}\n", std::inner_product(ALLc(left), right.cbegin(), 0, std::plus(), [](int a, int b) { return std::abs(a - b); }), part2);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}