#!/usr/bin/python3

import sys

mapdata = open(sys.argv[1], "r")

lines = {}
for line in sys.stdin:
	realnums= mapdata.readline().split(",")
	for i in realnums:
		lines[int(i)] = line

for _, line in sorted(lines.items(), key=lambda x: x[0]):
	print(line, end="")
