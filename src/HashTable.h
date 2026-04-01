#pragma once

#include <cstddef>
#include <cstring>
#include <list>
#include <utility>
#include <vector>

namespace TehImage
{
	class HashTable
	{
	private:
		const size_t TABLE_SIZE;
		std::vector<std::list<std::pair<unsigned int, unsigned int>>> table;

	public:
		HashTable(size_t size);

		unsigned int hashFunction(char key[3]);
		void insert(char key[3], unsigned int value);
		void remove(char key[3]);
		unsigned int get(char key[3]);
		bool contains(char key[3]);
	};
}
