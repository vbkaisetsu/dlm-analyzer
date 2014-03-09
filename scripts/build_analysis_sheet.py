#!/usr/bin/python3

import sys
from collections import defaultdict

en_file = open(sys.argv[1], "r")
ja_file = open(sys.argv[2], "r")
map_file = open(sys.argv[3], "w")

order_map = defaultdict(lambda : [])

en_ref = []
ja_ref = []
for line in en_file:
	en_ref.append(line.strip())
for line in ja_file:
	ja_ref.append(line.strip())

for i, line in enumerate(sys.stdin):
	spl = line.split(" ||| ")
	order_map[(int(spl[1]), spl[2].strip())].append(i)

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
	</table>""" %(sid, i, en_ref[sid], ja_ref[sid], s))
	print(",".join(str(i) for i in r), file=map_file)

print("""
</body>
</html>
""")
