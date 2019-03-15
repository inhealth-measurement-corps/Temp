#Imports for Ben and Dante's portions of the code
import os.path, time
import pymysql
import csv
import pandas
import os

#Imports for my portion of the code 
import MySQLdb
from conv_classes import Patient, Ambulation
import numpy as np
import csv

#DB login info
host = "10.162.80.9"
username = "jessie"
password = "rJvMXazyFrZCw7O4"
db = "mmambulation"


old_time = 0 #Used to see if doc has been updated by comparing to new time



while True: #(for infinite looping)
    new_time = time.ctime(os.path.getmtime("Versus Excel Sheet.xlsx")) #needs to read the time of the Versus Excel Sheet
    print ("looping")
    print (new_time)
    if new_time != old_time and old_time != 0: 

        #Bens code 
        my_cnxn = pymysql.connect(host=host, user=username, passwd=password, db=db)
        my_cursor = my_cnxn.cursor()
        df = pandas.read_excel('Versus Excel Sheet.xlsx', usecols=[0, 1, 17, 6, 15, 16, 18, 20, 23], skiprows=[1])
        #df.reindex(columns=[*df.columns.tolist(), 'col1', 'col2'], fill_value=0)
        df['Unnamed: 8'] = "NULL"
        df['Unnamed: 9'] = "NULL"
        df = df.fillna("NULL")
        df['Versus Data'] = df['Versus Data'].replace(',', '', regex=True)
        df['Unnamed: 1'] = df['Unnamed: 1'].replace(',', '', regex=True)
        df['Unnamed: 2'] = df['Unnamed: 2'].replace(',', '', regex=True)
        df['Unnamed: 3'] = df['Unnamed: 3'].replace(',', '', regex=True)
        df['Unnamed: 4'] = df['Unnamed: 4'].replace(',', '', regex=True)
        df['Unnamed: 5'] = df['Unnamed: 5'].replace(',', '', regex=True)
        df['Unnamed: 6'] = df['Unnamed: 6'].replace(',', '', regex=True)
        df.to_csv('output.csv', encoding='utf-8')
        with open('output.csv') as f:
            with open("temp.csv",'w') as f1:
                reader = csv.reader(f, quotechar = '"', delimiter=',', quoting=csv.QUOTE_ALL, skipinitialspace=True)
                for row in reader:
                    f1.write(",".join([row[8],row[1],row[5],row[2],row[3],row[4],row[6],row[7],row[9],row[10]]))
                    f1.write('\n')
        with open("temp.csv",'r') as f:
            with open("output.csv",'w') as f1:
                f.readline() # skip header line
                for line in f:
                    f1.write(line)
        os.remove("temp.csv")
        print('Loop finished')
        csv_data = csv.reader(open('output.csv'))
        for row in csv_data:
            my_cursor.execute("REPLACE INTO patient_info VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s)", row[:10])
        #my_cursor.close()




        #Dante's code
        try:
            with my_cnxn.cursor() as cursor:
                #selects the live data in order to shorten the search for the patient info      
                sql = "SELECT `patient_ID` FROM `live_details`"
                cursor.execute(sql)
                result = cursor.fetchall()
                
                #places all of the ids from the live into a list for easier search
                #also accounts for the duplicates from the list with 
                ids = []
                for i in range(1, len(result)): 
                    if result[i - 1] != result[i]: 
                        ids.append(result[i - 1])
                    if i == (len(result)-1):
                        ids.append(result[i])
                      
                #change the tuple list into an array so it can be accessed easier
                newids = []
                for id in ids:
                    newids.extend(id)
                
                #determines if there is a discharge date and if so then appends another array with new ids
                changeids = []
                for i in range(len(newids)):
                    new = newids[i]
                    ql = ("SELECT `discharge_date` FROM `patient_info` WHERE `patient_ID`= %d" %new)
                    cursor.execute(ql)
                    result1 = cursor.fetchall()
                    print(result1)
                    if result1 != ((None,),):
                        changeids.append(new)
                        print(new)
                
                
                # Create a new record based on the ids that have a discharge date
                for i in range(len(changeids)):
                    new = changeids[i]
                    cursor.execute("INSERT INTO historical_details (patient_ID, ambulation, date, time_of_day, distance, duration, speed) SELECT patient_ID, ambulation, date, time_of_day, distance, duration, speed FROM live_details WHERE patient_ID = %d;" %new)
                    cursor.execute("DELETE FROM live_details WHERE patient_ID = %d;" %new)


            # connection is not autocommit by default. So you must commit to save
            # your changes.
            
            my_cnxn.commit()
            
        finally:
            my_cnxn.close()


        #Arsen's Code

        db = MySQLdb.connect(host=host, user=username, passwd=password, db=db) #connecting to the sql database
        cur = db.cursor() #make a cursor object that lets us navigate through the database
        cur2 = db.cursor() #make a second cursor to go through the patient_info table
        cur.execute("SELECT * FROM historical_details") #get the ambulation table

        patient_map = {} # A map of patient numbers/MRNs to patient objects

        #for row in cur.fetchmany(5):
        for row in cur.fetchall():
            if patient_map.get(row[0]) != None: #the patient is already in our dict
                patient = patient_map[row[0]] #get the patient object corresponding to this MRN - stays the same
                ambulation = Ambulation(row[0], row[2], row[3], row[4], row[5], row[6], row[1])
                patient.add_ambulation(ambulation)

            else: #the patient is not in our dict yet
                cur2.execute("SELECT * FROM patient_info WHERE patient_id = %s", (row[0],))
                row2 = cur2.fetchone()
                patient =  Patient(row[0], row2[2], row2[1], row2[3], row2[4], row2[6], row2[7])
                ambulation = Ambulation(row[0], row[2], row[3], row[4], row[5], row[6], row[1])
                patient.add_ambulation(ambulation)
                patient_map[patient.mrn] = patient


            #print(type(str(ambulation.start_datetime)))


        #Need to do regression on each patient to get deltas
        for x, y in patient_map.items():
            print()
            print("Data for MRN", x)
            #y.print_days()
            #y.regression_v1()
            #y.regression_v2()
            y.regression_v3()
            #y.print_ambulations()
            y.print_data()

        '''
        for x, y in patient_map.items():
            print()
            print("Data for MRN", x)
            #y.adjust_data()
            y.regression_v1()
            y.print_list()
            #y.show_regression("speed")
        '''

        '''
        with open("Ambulation_Unit.csv", 'w') as csvfile:
            filewriter = csv.writer(csvfile, delimiter=',',
                                    quotechar='|', quoting=csv.QUOTE_MINIMAL)
            filewriter.writerow(["Patient ID", "# of Ambulations (N)", "Ambulation Frequency (N/LoS)", "Days with ambulation (number)", "Compliance 1/day (%)", "Compliance 2/day (%)", "Compliance 3/day (%)", "Total Distance (ft)", "Distance CoT","Duration CoT","Speed CoT"])
            for x, y in patient_map.items():
                freq = y.num_ambulations/y.length_of_stay
                filewriter.writerow([y.mrn, y.num_ambulations, freq, len(y.ambulations_per_day), y.compliance_1, y.compliance_2, y.compliance_3, y.total_distance, y.delta_dist, y.delta_dur, y.delta_speed])

        with open("Ambulation_Daily.csv", 'w') as csvfile:
            filewriter = csv.writer(csvfile, delimiter=',',
                                    quotechar='|', quoting=csv.QUOTE_MINIMAL)
            filewriter.writerow(["Patient ID", "Day on Unit", "# of Ambulations", "Dist Max", "Dist Mean", "Dist Min", "Dur Max", "Dur Mean", "Dur Min","Speed Max","Speed Mean", "Speed Min"])
            for x, y in patient_map.items():
                for a, b in y.date_to_day.items():
                    filewriter.writerow([y.mrn, b.day_number, b.num_ambulations, b.max_dist, b.mean_dist, b.min_dist, b.max_dur ,b.mean_dur, b.min_dur, b.max_speed, b.mean_speed, b.min_speed])
        '''



        '''
        for x, y in patient_map.items():
            freq = y.num_ambulations/y.length_of_stay
            cur.execute("INSERT INTO ambulation_unit VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)", [y.mrn, y.num_ambulations, freq, len(y.ambulations_per_day), y.compliance_1, y.compliance_2, y.compliance_3, y.total_distance, y.delta_dist, y.delta_dur, y.delta_speed])
        '''
        '''
        for x, y in patient_map.items(): #x is mrn, y is Patient object
                for a, b in y.date_to_day.items(): #a is the date, b is the Day object
                    if b.num_ambulations == 0:
                        cur.execute("INSERT INTO ambulation_daily VALUES (%s, %s, %s, null, null, null, null, null, null, null, null, null)", [y.mrn, b.day_number, b.num_ambulations])
                    else:
                        cur.execute("INSERT INTO ambulation_daily VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)", [y.mrn, b.day_number, b.num_ambulations, b.max_dist, b.mean_dist, b.min_dist, b.max_dur ,b.mean_dur, b.min_dur, b.max_speed, b.mean_speed, b.min_speed])
        '''


        cur.close()
        cur2.close()

    old_time = new_time
    time.sleep(5)


















