#!/usr/bin/python3

import sys
from collections import defaultdict
import math

maxN = 3

def main():
	wids = defaultdict(lambda : len(wids) + 1)
	ngrams_oracle = defaultdict(lambda : 0)
	ngrams_output = defaultdict(lambda : 0)
	ngrams = defaultdict(lambda : 0)
	
	oraclefile = open(sys.argv[1], "r")
	outputfile = open(sys.argv[2], "r")

	for line in oraclefile:
		words = [0] + [wids[w] for w in line.split()] + [0]
		for n in range(maxN):
			one = 1 if n == 0 else 0
			for i in range(one, len(words) - n - one):
				ngram = tuple(words[i:i+n+1])
				ngrams_oracle[ngram] += 1
				ngrams[ngram] += 1
				
	for line in outputfile:
		words = [0] + [wids[w] for w in line.split()] + [0]
		for n in range(maxN):
			one = 1 if n == 0 else 0
			for i in range(one, len(words) - n - one):
				ngram = tuple(words[i:i+n+1])
				ngrams_output[ngram] += 1
				ngrams[ngram] += 1

	oraclefile.close()
	outputfile.close()


	counts = defaultdict(lambda : defaultdict(lambda : 0))
	probs = defaultdict(lambda : 0)
	total = defaultdict(lambda : 0)

	for ngram in ngrams:
		probs[0] += ngrams_oracle[ngram]
		probs[1] += ngrams_output[ngram]
		
		for j in range(ngrams_oracle[ngram]):
			counts[0][j] += 1
		for j in range(ngrams_output[ngram]):
			counts[1][j] += 1
		for i in range(ngrams_oracle[ngram] + ngrams_output[ngram]):
			total[i] += 1

	probsum = probs[0] + probs[1]
	for i in probs:
		probs[i] /= probsum

	alpha = 0.05
	diff = 1
	cutoff = 0.0000001

	while diff < -cutoff or cutoff < diff:

		der1 = 0
		der2 = 0
		lik = 0

		for j in counts:
			talpha = probs[j] * alpha
			for i in counts[j]:
				val = probs[j] / (talpha + i)
				der1 += counts[j][i] * val
				der2 -= counts[j][i] * val * val
				lik += counts[j][i] * math.log(talpha + i)
		for i in total:
			val = 1 / (alpha + i)
			der1 -= total[i] * val
			der2 += total[i] * val * val
			lik -= total[i] * math.log(alpha + i)
		
		diff = -der1 / der2
		print("newt(%f) -> %f (der1=%f, der2=%f, diff=%f)" % (alpha, lik, der1, der2, diff), file=sys.stderr)
		alpha = alpha + diff if alpha + diff > 0 else alpha / 2

	# output model
	print(len(wids) + 1, len(ngrams))
	print("#")
	for w in sorted(wids.items(), key=lambda x: x[1]):
		print(w[0])
	for ngram in ngrams:
		p = (ngrams_oracle[ngram] + alpha * probs[0]) / (ngrams[ngram] + alpha)
		print(",".join(str(i) for i in ngram), p)

if __name__ == "__main__":
	main()
