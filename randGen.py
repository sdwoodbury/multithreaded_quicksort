#!/usr/bin/python

import random

with open("unsorted.txt", "w") as output:
	for i in range(0, 1000):
		output.write("%d\n" % random.randint(0, 1000))
