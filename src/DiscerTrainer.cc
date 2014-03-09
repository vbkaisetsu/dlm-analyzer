/*
 * DiscerTrainer.cc
 * Copyright (C) Koichi Akabe 2013 <vbkaisetsu@gmail.com>
 */

#include "DiscerTrainer.h"
#include "Utils.h"

#include <fstream>
#include <cfloat>
#include <boost/algorithm/string.hpp>

using namespace std;

namespace DiscerLangModel
{

DiscerTrainer::DiscerTrainer(Model *m)
{
	model = m;
}

double pow_int(double x, unsigned int n)
{
	double result, tmp;
	int i, j;
	result = 1.0;
	for(j = 0; j < sizeof(n) * 8; ++j)
	{
		if(n >> j & 0x01)
		{
			tmp = x;
			for(i = 0; i < j; ++i)
				tmp = tmp * tmp;
			result *= tmp;
		}
	}
	return result;
}

void DiscerTrainer::regularizeL1(int id, int iter, double eta)
{
	double c = eta * (iter - ngram_last_update[id]);
	if(model->ngram_weight[id] >= c)
		model->ngram_weight[id] -= c;
	else if(-model->ngram_weight[id] >= c)
		model->ngram_weight[id] += c;
	else
		model->ngram_weight[id] = 0;
	ngram_last_update[id] = iter;
}

void DiscerTrainer::regularizeL2(int id, int iter, double eta)
{
	// w_{n+1} <- w_n - w_n * eta
	// w_{n+m} <- w_n * (1 - eta) ^ m
	model->ngram_weight[id] *= pow_int((1.0 - eta), iter - ngram_last_update[id]);
	ngram_last_update[id] = iter;
}

double DiscerTrainer::calcurate_score(scored_candidate &cand, int iter, double eta)
{
	double score = 0.0;
	vector<int>::iterator it_ngram_id;
	for(it_ngram_id = get<0>(cand).begin(); it_ngram_id != get<0>(cand).end(); ++it_ngram_id)
	{
		regularizeL1(*it_ngram_id, iter, eta);
		score += model->ngram_weight[*it_ngram_id];
	}
	score += get<1>(cand) * model->length_weight + get<2>(cand) * model->systemscore_weight;
	return score;
}

void DiscerTrainer::update_weights(scored_candidate &target, scored_candidate &cand, int iter, double eta)
{
	vector<int>::iterator it_ngram_id;
	for(it_ngram_id = get<0>(target).begin(); it_ngram_id != get<0>(target).end(); ++it_ngram_id)
	{
		regularizeL1(*it_ngram_id, iter, eta);
		model->ngram_weight[*it_ngram_id] += 1.0;
	}
	for(it_ngram_id = get<0>(cand).begin(); it_ngram_id != get<0>(cand).end(); ++it_ngram_id)
	{
		regularizeL1(*it_ngram_id, iter, eta);
		model->ngram_weight[*it_ngram_id] -= 1.0;
	}
	model->systemscore_weight += get<2>(target) - get<2>(cand);
	model->length_weight += get<1>(target) - get<1>(cand);
}

void DiscerTrainer::load_data(string filename)
{
	/*
	 *  Args:
	 *    filename: input file name
	 * 
	 *  The structure of N-bests:
	 * 
	 *   root
	 *   |- nbest
	 *   |  + ( ngram ids, length, system score, target score )
	 *   |  + ( ... )
	 *   |  + ...
	 *   |
	 *   |- nbest
	 *   |- ...
	 */

	ifstream ifs(filename);
	string line;
	
	unordered_map<int, int> nbest_ids;
	int nbest_id = 0;
	
	while(ifs && getline(ifs, line))
	{
		vector<string> splitline;
		split_by_string(splitline, line, " ||| ");
		if(splitline.size() != 4)
			continue;

		unsigned int act_nbest_id = atoi(splitline[0].c_str());
		if(nbest_ids.find(act_nbest_id) == nbest_ids.end())
		{
			nbest_ids[act_nbest_id] = nbest_id;
			++nbest_id;
			nbest v;
			traindata.push_back(v);
			target_best_score.push_back(-DBL_MAX);
			target_best_id.push_back(0);
		}

		vector<string> words;
		boost::split(words, splitline[1], boost::is_space());

		// replace words with word ids
		vector<int> word_ids = model->get_wid_sequence(words);

		double systemscore, targetscore;
		systemscore = atof(splitline[2].c_str());
		targetscore = atof(splitline[3].c_str());

		vector<int> ngrams = model->get_ngrams(word_ids);

		// update best score
		if(target_best_score[nbest_ids[act_nbest_id]] < targetscore)
		{
			target_best_score[nbest_ids[act_nbest_id]] = targetscore;
			target_best_id[nbest_ids[act_nbest_id]] = traindata[nbest_ids[act_nbest_id]].size();
		}

		traindata[nbest_ids[act_nbest_id]].push_back(make_tuple(ngrams, (double)word_ids.size(), systemscore, targetscore));
	}
}

void DiscerTrainer::train(unsigned int iter, double merginlevel, double eta)
{
	/*
	 * train weights using structured perceptron
	 * 
	 * features:
	 *   n-grams
	 *   number of words
	 *   system score
	 */
	// initialize
	model->ngram_weight.resize(model->ngram_ids.size(), 0.0);
	ngram_last_update.resize(model->ngram_ids.size(), 0);
	model->length_weight = 0.0;
	model->systemscore_weight = 1.0;
	unsigned int i, j, k, iter_total = 0;
	for(i = 0; i < iter; ++i)
	{
		for(j = 0; j < traindata.size(); ++j)
		{
			double system_best_score = -DBL_MAX, system_best_score2 = -DBL_MAX;
			int system_best_id = 0, system_best_id2 = 0;

			// get system output
			for(k = 0; k < traindata[j].size(); ++k)
			{
				double score;
				score = calcurate_score(traindata[j][k], iter_total, eta);
				if(score > system_best_score)
				{
					system_best_id2 = system_best_id;
					system_best_id = k;
					system_best_score2 = system_best_score;
					system_best_score = score;
				}
				else if(score > system_best_score2)
				{
					system_best_id2 = k;
					system_best_score2 = score;
				}
			}

			if(target_best_score[j] == get<3>(traindata[j][system_best_id]))
			{
				// system output == target output
				// check mergin
				double mergin = merginlevel * (target_best_score[j] - get<3>(traindata[j][system_best_id2]));
				if(system_best_score - system_best_score2 < mergin)
				{
					update_weights(traindata[j][target_best_id[j]], traindata[j][system_best_id2], iter_total, eta);
				}
			}
			else
			{
				// system output != target output
				update_weights(traindata[j][target_best_id[j]], traindata[j][system_best_id], iter_total, eta);
			}
			
			// regularize
			if(model->length_weight >= eta)
				model->length_weight -= eta;
			else if(-model->length_weight >= eta)
				model->length_weight += eta;
			else
				model->length_weight = 0;
			if(model->systemscore_weight >= eta)
				model->systemscore_weight -= eta;
			else if(-model->systemscore_weight >= eta)
				model->systemscore_weight += eta;
			else
				model->systemscore_weight = 0;
			
			++iter_total;
		}
	}
	// regularize all weights
	for(i = 0; i < model->ngram_weight.size(); i++)
	{
		regularizeL1(i, iter_total, eta);
	}
}

}
