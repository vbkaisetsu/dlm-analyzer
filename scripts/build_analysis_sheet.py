#!/usr/bin/python3

import argparse
from collections import defaultdict

parser = argparse.ArgumentParser()

parser.add_argument("orig", help="original sentences")
parser.add_argument("trg_refs", help="translation references")
parser.add_argument("order_map", help="path to the map data to revert ordering")
parser.add_argument("seed", help="seed file for building the evaluation sheet")

args = parser.parse_args()

org_file = open(args.orig, "r")
trg_file = open(args.trg_refs, "r")
map_file = open(args.order_map, "w")
seed_file = open(args.seed, "r")

order_map = defaultdict(lambda : [])

org_ref = []
trg_ref = []

for line in org_file:
	org_ref.append(line.strip())
org_file.close()

for line in trg_file:
	trg_ref.append(line.strip())
trg_file.clsoe()

for i, line in enumerate(seed_file):
	spl = line.split(" ||| ")
	order_map[(int(spl[1]), spl[2].strip())].append(i)
seed_file.close()

print("""
<!DOCTYPE html>
<html>
<head>
	<meta charset="utf-8" />
	<title>Error analysis</title>
	<style>

table
{
	width: auto;
	border-spacing: 0;
	font-size:10pt;
	padding: 8px;
}

table tr td
{
	padding: 8px 15px;
	text-align: left;
}

table tr td:first-child
{
	text-align: right;
	width: 100px;
}

table tr td:first-child
{
	border-left: 1px solid #aaa;
}

table tr td:last-child
{
	border-right: 1px solid #aaa;
}

table tr td
{
	box-shadow: 2px 2px 1px rgba(0,0,0,0.1);
}

table tr {
	background: #fff;
}

table tr:nth-child(2n+1)
{
	background: #f5f5f5;
}

table tr:first-child td
{
	border-top:1px solid #ccc;
}

table tr:last-child td
{
	border-bottom:1px solid #aaa;
	box-shadow: 2px 2px 1px rgba(0,0,0,0.1);
}

table tr:first-child td:first-child
{
	border-radius: 5px 0 0 0 ;
}

table tr:first-child td:last-child
{
	border-radius: 0 5px 0 0 ;
}

table tr:last-child td:first-child
{
	border-radius: 0 0 0 5px;
}

table tr:last-child td:last-child
{
	border-radius: 0 0 5px 0;
}

span
{
	color: #f00;
}

	</style>
</head>
<body>
<h1>Error Analysis</h1>
""")

for i, ((sid, s), r) in enumerate(sorted(order_map.items(), key=lambda x: x[0][0])):
	print("""
	<table>
		<tr>
			<td>Sentence ID:</td>
			<td>%d(%d)</td>
		</tr>
		<tr>
			<td>Origial:</td>
			<td>%s</td>
		</tr>
		<tr>
			<td>Target Ref:</td>
			<td>%s</td>
		</tr>
		<tr>
			<td>Target MT:</td>
			<td>%s</td>
		</tr>
	</table>""" %(sid, i, org_ref[sid], trg_ref[sid], s))
	print(",".join(str(i) for i in r), file=map_file)

print("""
</body>
</html>
""")

map_file.close()
