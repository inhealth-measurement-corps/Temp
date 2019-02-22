import pymysql
import csv

# DB INFO
# TODO: Figure out formatting for multiple badges for single patient_info
# TODO: Push stuff from excel to database: Starting from 342 = 113, there is a one to one coorespondance (343 = 114, etc).
# Currently does not import patients that have multiple badges. Badge information will be imported as 0s. My suggested solution is to format these into multiple lines, and for any additional badges mark the patient ID with a letter. For example, Patient 500 has badges 1 and 2. Format this as first row having Patient 500 with badge 1, second row as Patient 500b having badge 2. This would also require the SQL table's format for patient ID to be changed from integer to string.

host = "10.162.80.9"
username = "jessie"
password = "rJvMXazyFrZCw7O4"
db = "mmambulation"

#host = "localhost"
#username = "root"
#password = "password"
#db = "new_schema"

my_cnxn = pymysql.connect(host=host, user=username, passwd=password, db=db)
my_cursor = my_cnxn.cursor()

#def insert_records(table, yourcsv, cursor, cnxn):
#    with open(yourcsv) as csvfile:
#        csvFile = csv.reader(csvfile, delimiter=',')
#        header = next(csvFile)
#        headers = map((lambda x: x.strip()), header)
#        insert = 'INSERT INTO {} ('.format(table) + ', '.join(headers) + ') VALUES '
#        for row in csvFile:
#            values = map((lambda x: "'"+x.strip()+"'"), row)
#            b_cursor.execute(insert +'('+ ', '.join(values) +');' )
#            b_cnxn.commit() #must commit, no auto commit

			
while True: #(for infinite looping)
	csv_data = csv.reader(open('output.csv'))
	for row in csv_data:
	#temp = row[:10]
	#temp = temp + temp
		my_cursor.execute("REPLACE INTO patient_info_test VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s)", row[:10])
#my_cursor.close()