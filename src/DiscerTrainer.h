#ifndef DISCER_MODEL_TRAINER_H
#define DISCER_MODEL_TRAINER_H

#include "common.h"
#include "Model.h"

namespace DiscerLangModel
{

class DiscerTrainer
{

private:
	Model *model;
	std::vector<int> target_best_id;
	std::vector<double> target_best_score;
	std::vector<int> ngram_last_update;

public:
	std::vector<nbest> traindata;
	std::random_device seed_gen;
	std::default_random_engine random_engine;
	std::uniform_real_distribution<> zero_one_distribution;

	DiscerTrainer(Model *m);
	void regularizeL1(int id, int iter, double eta);
	void regularizeL2(int id, int iter, double eta);
	double calcurate_score(scored_candidate &cand, int iter, double eta);
	void update_weights(scored_candidate &target, scored_candidate &cand, int iter, double eta);
	void load_data(std::string filename);
	void train(unsigned int iter, double merginlevel, double eta);
};

}

#endif
