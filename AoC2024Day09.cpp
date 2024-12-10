import std.core;

using u64 = unsigned long long;

struct Block
{
	u64 start, size, id;
	u64 GetCheckSum() const
	{
		u64 end = start + size;
		return (end * (end - 1ull) - start * (start - 1ull)) * id / 2ll;
	}
	bool operator<(const Block& b) const { return start < b.start; }
};

int main(int argc, char* argv[])
{
	auto ChronoStart = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: AoC24Day09.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

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
			Block b {pos, v, 0ull};
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

	std::swap(files, fBackup);

	for (auto f = files.rbegin(), fEnd = std::prev(files.rend()); f != fEnd; ++f)
	{
		Block &b = *f;
		for (auto iter = spaces.begin(); iter != spaces.end() && iter->start < b.start; ++iter)
		{
			if (iter->size >= b.size)
			{
				f->start = iter->start;
				if (iter->size == b.size)
					spaces.erase(iter);
				else
				{
					auto s = const_cast<Block*>(&*iter);
					s->size -= b.size;
					s->start += b.size;
				}
				break;
			}
		}
	}

	auto Acc = [](u64 a, const Block& b) { return a + b.GetCheckSum(); };
	std::cout << std::format("Part 1: {}\nPart 2: {}\n", std::accumulate(compacted.cbegin(), compacted.cend(), 0ull, Acc), std::accumulate(files.cbegin(), files.cend(), 0ull, Acc));
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}