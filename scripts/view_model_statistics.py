#!/usr/bin/python3

import argparse
from collections import defaultdict

parser = argparse.ArgumentParser()

parser.add_argument("model", help="model data generated from dlm-train")
parser.add_argument("top_K", help="view K-best erroneous n-grams", type=int)

args = parser.parse_args()

fp1 = open(args.model, "r")

wids = defaultdict(lambda : len(wids))
wids.update({"<s>": 0})
ngramweights = defaultdict(lambda : [])

line = fp1.readline()
spl = line.split()
widlen = int(spl[0])
ngramlen = int(spl[1])
# skip first word
fp1.readline()

for i in range(1, widlen):
	line = fp1.readline()
	wids[line.strip()] = i
for i in range(ngramlen):
	line = fp1.readline()
	spl = line.split()
	widseq = spl[0].split(",")
	if len(widseq) >= 1:
		ngramweights[len(widseq)].append((tuple(int(j) for j in widseq), float(spl[1])))

rev_wids = {v: k for k, v in wids.items()}

for i in range(1, len(ngramweights) + 1):
	sorted_ngram = sorted(ngramweights[i], key=lambda x: x[1])
	print("==== %d gram ====" % i)
	for ngram in sorted_ngram[:args.top_K]:
		print(" ".join(rev_wids[wid] for wid in ngram[0]), "\t", ngram[1])
