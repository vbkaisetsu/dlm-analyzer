#!/usr/bin/python3

import argparse

parser = argparse.ArgumentParser()

parser.add_argument("order_map", help="map data to revert ordering")
parser.add_argument("mark_data", help="annotator's mark data")

args = parser.parse_args()

order_map = open(args.order_map, "r")
mark_data = open(args.mark_data, "r")

lines = {}
for line in mark_data:
	realnums= order_map.readline().split(",")
	for i in realnums:
		lines[int(i)] = line
mark_data.close()

for _, line in sorted(lines.items(), key=lambda x: x[0]):
	print(line, end="")

