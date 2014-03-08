#include "src/DiscerTrainer.h"
#include "src/DiscerTester.h"

#include <boost/algorithm/string.hpp>

#define TEST_EQ(x, y) if((x) == (y)) std::cout << "EXPECT_EQ("#x", "#y") ... OK" << std::endl; else {std::cerr << "EXPECT_EQ("#x", "#y") ... FAILED" << std::endl; return 1;}

using namespace std;

DiscerLangModel::Model m(3);
DiscerLangModel::DiscerTrainer trainer(&m);
DiscerLangModel::DiscerTester tester(&m);

int main(int argc, char **argv)
{
	trainer.load_data("test_trainset.txt");
	
	vector<string> words;
	vector<int> wids;
	
	boost::split(words, "i saw a girl with the telescope .", boost::is_space());
	wids = m.get_wid_sequence(words);
	vector<int> feats{0, 1, 2, 3, 4, 24, 5, 6, 7, 8, 9, 10, 11, 25, 26, 14, 15, 16, 17, 18, 19, 27, 28, 29, 23};
	TEST_EQ(m.get_ngrams(wids), feats);
	
	trainer.train(1, 0, 0);
	
	tester.load_data("test_testset.txt");
	
	TEST_EQ(tester.eval_no_model(), 1);
	TEST_EQ(tester.eval(), 2);
	return 0;
}
