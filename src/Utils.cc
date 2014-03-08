/*
 * Utils.cc
 * Copyright (C) Koichi Akabe 2013 <vbkaisetsu@gmail.com>
 */

#include "Utils.h"

#include <iostream>

using namespace std;

void split_by_string(vector<std::string> &result, string orig, string delimiter)
{
	/*
	 *  Args:
	 *    result: split strings
	 *    orig: an original string
	 *    delimiter: a delimiter string
	 */

	result.clear();
	const char *ostr = orig.c_str();
	const char *dstr = delimiter.c_str();
	unsigned int osize = orig.size();
	unsigned int dsize = delimiter.size();
	unsigned int st = 0;
	if(dsize < osize)
	{
		unsigned int i, j;
		for(i = 0; ostr[i + dsize] != 0; ++i)
		{
			for(j = 0; j < dsize; ++j)
			{
				if(ostr[i + j] != dstr[j])
					break;
			}
			if(j == dsize)
			{
				string substr(orig, st, i - st);
				result.push_back(substr);
				i += j - 1;
				st = i + 1;
				if(st >= osize - dsize)
					break;
			}
		}
	}
	string endstr(orig, st);
	result.push_back(endstr);
}
