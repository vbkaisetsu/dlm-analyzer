/*
 * main.cc
 * Copyright (C) Koichi Akabe 2013 <vbkaisetsu@gmail.com>
 * 
 * This package is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This package is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DiscerTrainer.h"
#include "DiscerTester.h"

#include <iostream>

// setup gflags
DEFINE_string(traindata, "/dev/stdin", "file name of training data");
DEFINE_string(testdata, "", "file name of test data");
DEFINE_string(modeldata, "", "file name of model data");
DEFINE_double(eta, 0.0, "regularization eta");
DEFINE_int32(iter, 100, "number of iteration");
DEFINE_double(mergin, 0.0, "mergin level");
DEFINE_int32(ngramsize, 3, "ngram size (MAX: 5)");
DEFINE_bool(badaverages, false, "show trained scores");
//DEFINE_bool(baseline, false, "baseline !!! eta=0, mergin=0, iter=1 !!!");

int main(int argc, char **argv)
{
	google::ParseCommandLineFlags(&argc, &argv, true);

	if(FLAGS_modeldata == "" && FLAGS_testdata == "")
	{
		std::cerr << "Please specify model file or test file." << std::endl;
	}

	DiscerLangModel::Model model(FLAGS_ngramsize);
	DiscerLangModel::DiscerTrainer trainer(&model);
	std::cerr << "Loading ..." << std::endl;
	trainer.load_data(FLAGS_traindata);
	std::cerr << "Training ..." << std::endl;
	trainer.train(FLAGS_iter, FLAGS_mergin, FLAGS_eta);
	std::cerr << "Training finished." << std::endl;
	if(FLAGS_modeldata != "")
	{
		std::cerr << "Saving model data ..." << std::endl;
		model.save_model(FLAGS_modeldata);
	}
	if(FLAGS_testdata != "")
	{
		DiscerLangModel::DiscerTester tester(&model);
		std::cerr << "Loading test data ..." << std::endl;
		tester.load_data(FLAGS_testdata);
		std::cerr << "Calcurating score ..." << std::endl << std::endl;
		if(FLAGS_badaverages)
		{
			std::vector<double> scores = tester.get_onebest_bad_averages();
			std::vector<double>::iterator it;
			int i = 0;
			for(it = scores.begin(); it != scores.end(); ++it)
			{
				std::cout << i << " " << *it << std::endl;
				++i;
			}
		}
		else
		{
			double before = tester.eval_no_model();
			double after = tester.eval();
			std::cout << "eta: " << FLAGS_eta << " ; " << before << " -> " << after << std::endl;
		}
		std::cerr << std::endl;
	}

	std::cerr << "Finished." << std::endl;
	return 0;
}
