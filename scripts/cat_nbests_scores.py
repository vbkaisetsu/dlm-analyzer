#!/usr/bin/python3

import argparse

parser = argparse.ArgumentParser()

parser.add_argument("nbest", help="nbest data generated from a MT system")
parser.add_argument("scores", help="score data for each candidate")

args = parser.parse_args()

nbestfile = open(args.nbest, "r")
scorefile = open(args.scores, "r")

for line in nbestfile:
	score = scorefile.readline().split()[0]
	spl = line.strip().split(" ||| ")
	spl[0] = spl[0].strip()
	spl[1] = " ".join(spl[1].split())
	if not spl[1]:
		continue
	spl[2] = spl[2].strip()
	spl[3] = score
	print(" ||| ".join(spl))
