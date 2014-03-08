#ifndef DISCER_MODEL_MODEL_H
#define DISCER_MODEL_MODEL_H

#include "common.h"

#include <unordered_map>

namespace DiscerLangModel
{

class Model
{

private:
	int ngram_ids_size;
	int wids_size;
	unsigned int model_ngram_size;

public:
	std::unordered_map<std::string, int> wids;
	std::vector<std::string> rev_wids;
	std::unordered_map<Ngram, int, NgramHasher> ngram_ids;
	std::vector<Ngram> rev_ngram_ids;
	std::vector<double> ngram_weight;
	double length_weight;
	double systemscore_weight;

	Model(int ngram_size);
	std::vector<int> get_wid_sequence(std::vector<std::string> &words);
	std::vector<int> get_ngrams(std::vector<int> &word_ids);
	int save_model(std::string filename);

};

}

#endif
