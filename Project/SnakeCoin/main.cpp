#include "Block.h"
#include <vector>

int main(int argc, char const* argv[])
{
	Block genesis = Block::CreateGenesis();

	std::vector<Block> blockchains;
	

	for (int i=0; i<2000; ++i)
	{
		blockchains.push_back(Block::Create(genesis, nullptr));
	}

    return 0;
}
