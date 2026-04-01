#include "HashTable.h"
#include <stdexcept>

namespace TehImage
{
	HashTable::HashTable(size_t size)
		:TABLE_SIZE(size),
		 table(TABLE_SIZE)
	{ }
	
	unsigned int HashTable::hashFunction(char key[3])
	{
		unsigned int hash = 0;
		std::memcpy(&hash, key, 3);
		return hash % TABLE_SIZE;
	}

	void HashTable::insert(char key[3], unsigned int value)
	{
		unsigned int index = hashFunction(key);
		unsigned int keyMem = 0;
		std::memcpy(&keyMem, key, 3);

		for(auto& pair : table[index])
		{
			if(pair.first == keyMem)
			{
				pair.second = value;
				return;
			}
		}

		table[index].push_back({keyMem, value});
	}

	void HashTable::remove(char key[3])
	{
		
		unsigned int index = hashFunction(key);
		unsigned int keyMem = 0;
		std::memcpy(&keyMem, key, 3);

		auto& chain = table[index];
        for (auto it = chain.begin(); it != chain.end(); ++it) {
            if (it->first == keyMem) {
                chain.erase(it);
                return;
            }
		}
	}

	unsigned int HashTable::get(char key[3])
	{
		unsigned int index = hashFunction(key);
		unsigned int keyMem = 0;
		std::memcpy(&keyMem, key, 3);

		for(auto& pair : table[index])
		{
			if(pair.first == keyMem)
			{
				return pair.second;
			}
		}

		throw std::out_of_range("Key not present");
	}

	bool HashTable::contains(char key[3])
	{
		unsigned int index = hashFunction(key);
		unsigned int keyMem = 0;
		std::memcpy(&keyMem, key, 3);

		for(auto& pair : table[index])
		{
			if(pair.first == keyMem)
			{
				return true;
			}
		}

		return false;
	}
}
