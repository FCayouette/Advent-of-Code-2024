import std.core;

using u64 = unsigned long long;

struct Block
{
	u64 start, size, id;
	u64 GetCheckSum() const
	{
		u64 result = 0;
		for (u64 x = start, i = 0; i < size; ++i, ++x)
			result += x * id;
		return result;
	}
	bool operator<(const Block& b) const { return start < b.start; }
};

int main(int argc, char* argv[])
{
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

	u64 part1 = 0, part2 = 0;
	std::string line;

	in >> line;
	bool file = true;
	u64 pos = 0, fileId = 0;
	std::deque<Block> files, empty, fBackup;
	std::vector<Block> compacted;
	for (char c : line)
	{
		if (u64 v = c - '0'; v > 0)
		{
			Block b;
			b.start = pos;
			b.size = v;
			pos += v;
			if (file)
			{
				b.id = fileId++;
				files.push_back(b);
			}
			else
				empty.push_back(b);
		}		
		file = !file;
	}
	fBackup = files;
	std::set<Block> spaces= { empty.cbegin(), empty.cend() };
	
	pos = 0;
	while (!empty.empty() && !files.empty())
	{
		if (files.front().start < empty.front().start)
		{
			compacted.push_back(files.front());
			compacted.back().start = pos;
			pos += files.front().size;
			files.pop_front();
		}
		else
		{
			u64 eSize = empty.front().size, fSize = files.back().size;
			if (eSize >= fSize)
			{
				compacted.push_back(files.back());
				compacted.back().start = pos;
				pos += files.back().size;
				if (eSize != fSize)
					empty.front().size -= fSize;
				else 
					empty.pop_front();
				files.pop_back();
			}
			else
			{
				Block b = files.back();
				b.start = pos;
				b.size = eSize;
				pos += eSize;
				files.back().size -= eSize;
				compacted.push_back(b);
				empty.pop_front();
			}
		}
	}

	for (const Block& b : compacted)
		part1 += b.GetCheckSum();

	std::swap(files, fBackup);

	for (int i = files.size() - 1; i >= 1; --i)
	{
		Block &b = files[i];
		auto iterCheck = spaces.end();
		for (const Block& e : spaces)
		{
			if (b.start < e.start)
				break;
			if (e.size >= b.size)
			{
				Block newSpace = b;
				auto s = const_cast<Block*>(&e);
				b.start = e.start;
				s->size -= b.size;
				s->start += b.size;
				iterCheck = spaces.insert(newSpace).first;
				break;
			}
		}
		if (iterCheck != spaces.end())
		{
			auto prev = iterCheck;
			std::array toErase = { spaces.end(), spaces.end() };
			--prev;
			for (int i = 0; i < 2 && iterCheck != spaces.cend(); ++iterCheck)
				if (prev->start + prev->size == iterCheck->start && iterCheck->size)
				{
					const_cast<Block*>(&*prev)->size += iterCheck->size;
					toErase[i] = iterCheck;
				}
				else
					++prev;
			for (int i = 0; i < 2; ++i)
				if (toErase[i] != spaces.end())
					spaces.erase(toErase[i]);
		}
	}

	for (const Block & b : files)
		part2 += b.GetCheckSum();
	std::cout << std::format("Part 1: {}\nPart 2: {}\n", part1, part2);
}