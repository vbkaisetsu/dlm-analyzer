/*
 * DiscerTester.cc
 * Copyright (C) Koichi Akabe 2013 <vbkaisetsu@gmail.com>
 */

#include "DiscerTester.h"
#include "Utils.h"

#include <fstream>
#include <cfloat>
#include <boost/algorithm/string.hpp>

using namespace std;

namespace DiscerLangModel
{

DiscerTester::DiscerTester(Model *m)
{
	model = m;
}

double DiscerTester::calcurate_score(scored_candidate &cand)
{
	double score = 0.0;
	vector<int>::iterator it_ngram_id;
	for(it_ngram_id = get<0>(cand).begin(); it_ngram_id != get<0>(cand).end(); ++it_ngram_id)
	{
		if(*it_ngram_id < model->ngram_weight.size())
			score += model->ngram_weight[*it_ngram_id];
	}
	score += get<1>(cand) * model->length_weight + get<2>(cand) * model->systemscore_weight;
	return score;
}

double DiscerTester::calcurate_bad_average(scored_candidate &cand)
{
	double score = 0.0;
	vector<int>::iterator it_ngram_id;
	for(it_ngram_id = get<0>(cand).begin(); it_ngram_id != get<0>(cand).end(); ++it_ngram_id)
	{
		if(*it_ngram_id < model->ngram_weight.size())
		{
			if(model->ngram_weight[*it_ngram_id] < 0)
				score += model->ngram_weight[*it_ngram_id];
		}
	}
	score /= get<1>(cand);
	return score;
}

void DiscerTester::load_data(string filename)
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
			testdata.push_back(v);
		}

		vector<string> words;
		boost::split(words, splitline[1], boost::is_space());

		// replace words with word ids
		vector<int> word_ids = model->get_wid_sequence(words);

		double systemscore, targetscore;
		systemscore = atof(splitline[2].c_str());
		targetscore = atof(splitline[3].c_str());

		vector<int> ngrams = model->get_ngrams(word_ids);

		testdata[nbest_ids[act_nbest_id]].push_back(make_tuple(ngrams, (double)word_ids.size(), systemscore, targetscore));
	}
}

double DiscerTester::eval()
{
	vector<nbest>::iterator it_nbest;
	double eval_sum = 0.0;
	for(it_nbest = testdata.begin(); it_nbest != testdata.end(); ++it_nbest)
	{
		double best_score = -DBL_MAX;
		double best_eval = 0.0;
		nbest::iterator it_cand;
		for(it_cand = (*it_nbest).begin(); it_cand != (*it_nbest).end(); ++it_cand)
		{
			double score = calcurate_score(*it_cand);
			if(score > best_score)
			{
				best_score = score;
				best_eval = get<3>(*it_cand);
			}
		}
		eval_sum += best_eval;
	}
	return eval_sum / testdata.size();
}

double DiscerTester::eval_no_model()
{
	vector<nbest>::iterator it_nbest;
	double eval_sum = 0.0;
	for(it_nbest = testdata.begin(); it_nbest != testdata.end(); ++it_nbest)
	{
		double best_score = -DBL_MAX;
		double best_eval = 0.0;
		nbest::iterator it_cand;
		for(it_cand = (*it_nbest).begin(); it_cand != (*it_nbest).end(); ++it_cand)
		{
			double score = get<2>(*it_cand);
			if(score > best_score)
			{
				best_score = score;
				best_eval = get<3>(*it_cand);
			}
		}
		eval_sum += best_eval;
	}
	return eval_sum / testdata.size();
}

vector<double> DiscerTester::get_onebest_bad_averages()
{
	vector<double> result;
	vector<nbest>::iterator it_nbest;
	for(it_nbest = testdata.begin(); it_nbest != testdata.end(); ++it_nbest)
	{
		double best_score = -DBL_MAX;
		double best_eval = 0.0;
		nbest::iterator it_cand;
		for(it_cand = (*it_nbest).begin(); it_cand != (*it_nbest).end(); ++it_cand)
		{
			double score = get<2>(*it_cand);
			if(score > best_score)
			{
				best_score = score;
				best_eval = calcurate_bad_average(*it_cand);
			}
		}
		result.push_back(best_eval);
	}
	return result;
}

}
