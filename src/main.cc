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

#include "DiscrimTrainer.h"
#include "DiscrimTester.h"

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

using namespace std;

int main(int argc, char **argv)
{
	google::ParseCommandLineFlags(&argc, &argv, true);

	if(FLAGS_modeldata == "" && FLAGS_testdata == "")
	{
		cerr << "Please specify model file or test file." << endl;
	}

	DiscrimLangModel::Model model(FLAGS_ngramsize);
	DiscrimLangModel::DiscrimTrainer trainer(&model);
	cerr << "Loading ..." << endl;
	trainer.load_data(FLAGS_traindata);
	cerr << "Training ..." << endl;
	trainer.train(FLAGS_iter, FLAGS_mergin, FLAGS_eta);
	cerr << "Training finished." << endl;
	if(FLAGS_modeldata != "")
	{
		cerr << "Saving model data ..." << endl;
		model.save_model(FLAGS_modeldata);
	}
	if(FLAGS_testdata != "")
	{
		DiscrimLangModel::DiscrimTester tester(&model);
		cerr << "Loading test data ..." << endl;
		tester.load_data(FLAGS_testdata);
		cerr << "Calcurating score ..." << endl << endl;
		if(FLAGS_badaverages)
		{
			vector<double> scores = tester.get_onebest_bad_averages();
			vector<double>::iterator it;
			int i = 0;
			for(it = scores.begin(); it != scores.end(); ++it)
			{
				cout << i << " " << *it << endl;
				++i;
			}
		}
		else
		{
			double before = tester.eval_no_model();
			double after = tester.eval();
			cout << "eta: " << FLAGS_eta << " ; " << before << " -> " << after << endl;
		}
		cerr << endl;
	}

	cerr << "Finished." << endl;
	return 0;
}
