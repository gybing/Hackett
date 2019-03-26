#pragma once
#include "time.h"
#include <iostream>
#include <string>
#include <functional>

class BlockData {
public:
	virtual std::string hashCode() const = 0;
};

class Block final {
public:
    __int64 index;
    __int64 timestamp;
    std::string previoushash;
    std::string hash;

    BlockData* data;

public:
    static Block Create(const Block& lastblock, BlockData* data)
    {
        return Block(
            lastblock.index + 1,
            _time64(nullptr),
            data,
            lastblock.hash);
    }

	static Block CreateGenesis()
	{
		return Block(
			0,
			_time64(nullptr),
			nullptr,
			"0");
	}

public:
    ~Block() {}

private:
    Block(__int64 index, __int64 timestamp, BlockData* data, const std::string& previoushash)
		: index(index)
		, timestamp(timestamp)
		, data(data)
		, previoushash(previoushash)
	{
		hash = hashCode();
	}

	std::string Block::hashCode() const
	{
		std::string result;

		const std::size_t sz64 = 64 + 1;

		char mem[sz64];
		_i64toa_s(index, mem, sz64, 10);
		result += mem;

		_i64toa_s(timestamp, mem, sz64, 10);
		result += mem;

		if (data)
		{
			result += data->hashCode();
		}

		result += previoushash;
		return result;
	}

};
