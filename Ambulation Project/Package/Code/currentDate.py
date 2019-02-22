import datetime
import csv

#while True:                    #If you want it to run infinitely put everything under this loop

now = datetime.datetime.now()   #current date/time
now_string = str(now)    		#string form


r = csv.reader(open('IData.csv'))
lines = list(r)

for row in r:
	lines[row][2] = now_string[:19]	#formatted

writer = csv.writer(open('IData.csv', 'w'))
writer.writerows(lines)