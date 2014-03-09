#!/usr/bin/python3

import sys

nbestfile = open(sys.argv[1], "r")
scorefile = open(sys.argv[2], "r")

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
