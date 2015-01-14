#ifndef DISCER_MODEL_COMMON_H
#define DISCER_MODEL_COMMON_H

#include <vector>
#include <tuple>

#include <gflags/gflags.h>

#define NGRAM_MAX_SIZE 5

typedef std::tuple<std::vector<int>, double, double, double> scored_candidate;
typedef std::vector<scored_candidate> nbest;

namespace DiscrimLangModel
{
	struct Ngram
	{
		/*
		 *   size: number of words
		 *   w: array of word ids
		 */
		unsigned int size;
		unsigned int w[NGRAM_MAX_SIZE];
		bool operator == (const Ngram &g) const
		{
			unsigned int i;
			for(i = 0; i < size; ++i)
			{
				if(w[i] != g.w[i])
					return false;
			}
			return true;
		}
	};

	struct NgramHasher
	{
		size_t operator()(const Ngram &g) const
		{
			unsigned int i;
			std::hash<unsigned int> hasher;
			size_t seed = 0;
			for(i = 0; i < g.size; ++i)
			{
				seed ^= hasher(g.w[i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			}
			return seed;
		}
	};
}

#endif
