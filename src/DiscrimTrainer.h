#ifndef DISCER_MODEL_TRAINER_H
#define DISCER_MODEL_TRAINER_H

#include "common.h"
#include "Model.h"

namespace DiscrimLangModel
{

class DiscrimTrainer
{

private:
	Model *model;
	std::vector<int> target_best_id;
	std::vector<double> target_best_score;
	std::vector<int> ngram_last_update;

public:
	std::vector<nbest> traindata;

	DiscrimTrainer(Model *m);
	void regularizeL1(int id, int iter, double eta);
	void regularizeL2(int id, int iter, double eta);
	double calcurate_score(scored_candidate &cand, int iter, double eta);
	void update_weights(scored_candidate &target, scored_candidate &cand, int iter, double eta);
	void load_data(std::string filename);
	void train(unsigned int iter, double merginlevel, double eta);
};

}

#endif
