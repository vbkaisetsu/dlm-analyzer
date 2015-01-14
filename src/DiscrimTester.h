#ifndef DISCER_MODEL_TESTER_H
#define DISCER_MODEL_TESTER_H

#include "common.h"
#include "Model.h"

namespace DiscrimLangModel
{

class DiscrimTester
{

private:
	Model *model;
	std::vector<nbest> testdata;

public:
	DiscrimTester(Model *m);
	double calcurate_score(scored_candidate &cand);
	double calcurate_bad_average(scored_candidate &cand);
	void load_data(std::string filename);
	double eval();
	double eval_no_model();
	std::vector<double> get_onebest_bad_averages();
};

}

#endif
