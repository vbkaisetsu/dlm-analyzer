#!/usr/bin/python3

import argparse
from collections import defaultdict
import random

parser = argparse.ArgumentParser()

parser.add_argument("model", help="model data generated from dlm-train")
parser.add_argument("translations", help="one-best translations")
parser.add_argument("top_K", help="use K-best erroneous n-grams", type=int)
parser.add_argument("-l", "--limit", help="maximum locations for each n-gram", type=int, default=10)

args = parser.parse_args()

fp1 = open(args.model, "r") # model
fp2 = open(args.translations, "r") # system output

# =============================================
#  load model
# =============================================

wids = defaultdict(lambda : len(wids))
wids.update({"&lt;s&gt;": 0})
ngramweights = []

# first line
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
		ngramweights.append((tuple(int(j) for j in widseq), float(spl[1])))

ngramweights.sort(key=lambda x: x[1])

# =============================================
#  load nbest
# =============================================

systemouts = []

for i, line in enumerate(fp2):
	systemouts.append((i, [wids[w] for w in line.split()]))

rev_wids = {v: k for k, v in wids.items()}

k = -1
for ngram, score in ngramweights:
	k += 1
	if k == args.top_K:
		break
	random.shuffle(systemouts)
	i = 0
	ngram_str = " " + " ".join(str(wid) for wid in ngram) + " "
	for sid, systemout in systemouts:
		systemout_str = " 0 " + " ".join(str(wid) for wid in systemout) + " 0 "
		if ngram_str in systemout_str:
			splsent = systemout_str.split(ngram_str)
			pos = random.randint(0, len(splsent)-2)
			left_str = ngram_str.join(splsent[:pos+1])
			right_str = ngram_str.join(splsent[pos+1:])
			finalsent = " ".join(rev_wids[int(wid)] for wid in left_str.split()) + " <span>" + " ".join(rev_wids[int(wid)] for wid in ngram_str.split()) + "</span> " + " ".join(rev_wids[int(wid)] for wid in right_str.split())
			print(k, "|||", sid, "|||", finalsent)
			i += 1
		if i == args.limit:
			break
	if i == 0:
		k -= 1
