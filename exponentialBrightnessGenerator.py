compensation = 0.05
levels = 50

base = ((1-compensation)*(levels - 1))**(1 / float(levels - 1))

outputFile = open("expBrightness.txt", "w+")

length = 0
for i in range(0, levels):
	val = int(base**(i) + compensation * float(i))
	if val >= 1000:
		length = length + 5
	elif val >= 100:
		length = length + 4
	elif val >= 10:
		length = length + 3
	else:
		length = length + 2
	outputFile.write(str(val))
	if not i == levels - 1:
		outputFile.write(",")
	if length >= 400 and not length == 0:
		outputFile.write("\n")
		length = 0

outputFile.close()
exit()
