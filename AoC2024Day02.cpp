import std.core;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: AoC24Day02.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	int part1 = 0, part2 = 0, temp;
	std::string line;
	while (std::getline(in, line))
	{
		std::stringstream ss(line);
		std::vector<int> nums;
		while (ss >> temp)
			nums.push_back(temp);

		bool increasing = nums[1] > nums[0], ok = true;
		for (int i = 1; i < nums.size(); ++i)
			if ((increasing && nums[i] <= nums[i - 1]) || (!increasing && nums[i] >= nums[i - 1]) || std::abs(nums[i] - nums[i - 1]) > 3)
			{
				ok = false;
				break;
			}
		if (ok)
		{
			++part1;
			++part2;
		}
		else
		{
			std::vector<int> incorrect;
			for (int i = 0; i < nums.size(); ++i)
				incorrect.push_back(i);
			for (int x : incorrect)
			{
				std::vector<int> toTest;
				for (int i = 0; i < nums.size(); ++i)
					if (x != i)
						toTest.push_back(i);
			
				bool increasing = nums[toTest[1]] > nums[toTest[0]], failed = false;
				for (int i = 1; !failed && i < toTest.size(); ++i)
					if ((increasing && nums[toTest[i]] <= nums[toTest[i - 1]]) || (!increasing && nums[toTest[i]] >= nums[toTest[i - 1]]) || std::abs(nums[toTest[i]] - nums[toTest[i - 1]]) > 3)
						failed = true;
				if (!failed)
				{
					ok = true;
					break;
				}
			}
			if (ok) 
				++part2;
		}
	}
	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, part2);
}