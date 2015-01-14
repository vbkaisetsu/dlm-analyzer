/*
 * Model.cc
 * Copyright (C) Koichi Akabe 2013 <vbkaisetsu@gmail.com>
 */

#include "Model.h"

#include <fstream>

using namespace std;

namespace DiscrimLangModel
{

Model::Model(int ngram_size)
{
	if(ngram_size > NGRAM_MAX_SIZE)
		ngram_size = NGRAM_MAX_SIZE;
	model_ngram_size = ngram_size;
	ngram_ids_size = 0;
	wids_size = 1;
	rev_wids.push_back("#");
}

vector<int> Model::get_ngrams(vector<int> &word_ids)
{
	/*
	 *  Args:
	 *    word_ids: array of words
	 *
	 *  Returns: array of ngram ids
	 */

	vector<int> features;
	unsigned int i, j;
	for(i = 1; i <= model_ngram_size; ++i)
	{
		int unigram = (i == 1 ? 1 : 0); // unigrams don't contain <s> and </s>
		for(j = unigram; j <= word_ids.size() - i - unigram; ++j)
		{
			Ngram g;
			g.size = i;
			copy(word_ids.begin() + j, word_ids.begin() + j + i, g.w);
			if(ngram_ids.find(g) == ngram_ids.end())
			{
				ngram_ids[g] = ngram_ids_size;
				rev_ngram_ids.push_back(g);
				++ngram_ids_size;
			}
			features.push_back(ngram_ids[g]);
		}
	}
	return features;
}

vector<int> Model::get_wid_sequence(vector<string> &words)
{
	vector<int> word_ids;
	vector<string>::iterator it;
	word_ids.push_back(0);
	for(it = words.begin(); it != words.end(); ++it)
	{
		if(wids.find(*it) == wids.end())
		{
			wids[*it] = wids_size;
			rev_wids.push_back(*it);
			++wids_size;
		}
		word_ids.push_back(wids[*it]);
	}
	word_ids.push_back(0);
	return word_ids;
}

// TODO: support load_model
int Model::save_model(string filename)
{
	ofstream ofs(filename);
	if(!ofs)
	{
		return 1;
	}
	
	vector<Ngram>::iterator it_ngram;
	
	// remove zero-weight features
	vector<int> new_wid, rev_new_wid;
	new_wid.resize(rev_wids.size(), -1);
	rev_new_wid.resize(rev_wids.size() + 1, -1);
	new_wid[0] = 0; // 0 is reserved for <s> and </s>
	rev_new_wid[0] = 0;
	int wid_cnt = 1;
	int ngram_cnt = 0;
	for(it_ngram = rev_ngram_ids.begin(); it_ngram != rev_ngram_ids.end(); ++it_ngram)
	{
		if(ngram_weight[ngram_ids[*it_ngram]] != 0.0)
		{
			unsigned int i;
			for(i = 0; i < (*it_ngram).size; ++i)
			{
				if(new_wid[(*it_ngram).w[i]] == -1)
				{
					new_wid[(*it_ngram).w[i]] = wid_cnt;
					rev_new_wid[wid_cnt] = (*it_ngram).w[i];
					++wid_cnt;
				}
			}
			++ngram_cnt;
		}
	}
	
	// write model data
	ofs << wid_cnt << " " << ngram_cnt << endl;
	vector<string>::iterator it_wids;
	for(int i = 0; rev_new_wid[i] != -1; ++i)
	{
		ofs << rev_wids[rev_new_wid[i]] << endl;
	}
	for(it_ngram = rev_ngram_ids.begin(); it_ngram != rev_ngram_ids.end(); ++it_ngram)
	{
		if(ngram_weight[ngram_ids[*it_ngram]] != 0.0)
		{
			unsigned int i;
			for(i = 0; i < (*it_ngram).size-1; ++i)
			{
				ofs << new_wid[(*it_ngram).w[i]] << ",";
			}
			ofs << new_wid[(*it_ngram).w[(*it_ngram).size-1]] << " " << ngram_weight[ngram_ids[*it_ngram]] << endl;
		}
	}
	ofs << systemscore_weight << endl;
	ofs << length_weight << endl;
	return 0;
}

}
