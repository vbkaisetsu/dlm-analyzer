#!/usr/bin/python3

from collections import defaultdict
import sys

old_method = []
new_method = []

sheetdata = open(sys.argv[1], "r")
markdata = open(sys.argv[2], "r")

keys = ["KKK", "***", "RRR", "DDD", "III", "XXX", "YYY", "CCC", "PPP", "FFF", "+++", "WWW"]

cnt = {}
cnt_sum = {}

for k in keys:
	cnt[k] = defaultdict(lambda : 0)
	cnt_sum[k] = 0

cnt_all = defaultdict(lambda : 0)

for markline in markdata:
	mark = markline.split()[0]
	ngramid = int(sheetdata.readline().split(" ||| ")[0])
	cnt[mark][ngramid] += 1
	cnt_all[ngramid] += 1

all_sum = 0

print("RANK KKK *** RRR DDD III XXX CCC PPP FFF +++ WWW TOTAL")

for i, v in sorted(cnt_all.items(), key=lambda x: x[0]):
	for k in keys:
		cnt_sum[k] += cnt[k][i]
	all_sum += v
	print(i + 1, cnt_sum["KKK"]/all_sum, cnt_sum["***"]/all_sum, cnt_sum["RRR"]/all_sum, cnt_sum["DDD"]/all_sum, cnt_sum["III"]/all_sum, (cnt_sum["XXX"]+cnt_sum["YYY"])/all_sum, 
	      cnt_sum["CCC"]/all_sum, cnt_sum["PPP"]/all_sum, cnt_sum["FFF"]/all_sum, cnt_sum["+++"]/all_sum, cnt_sum["WWW"]/all_sum, all_sum)
"""
for i, v in sorted(cnt_all.items(), key=lambda x: x[0]):
	print(i + 1, cnt["KKK"][i], cnt["***"][i], cnt["RRR"][i], cnt["DDD"][i], cnt["III"][i], (cnt["XXX"][i]+cnt["YYY"][i]), 
	      cnt["CCC"][i], cnt["PPP"][i], cnt["FFF"][i], cnt["+++"][i], cnt["WWW"][i], v)
"""
