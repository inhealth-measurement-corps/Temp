import os.path, time

old_time = 0

while True: #(for infinite looping)
	new_time = time.ctime(os.path.getmtime("test.txt"))
	print ("looping")
	print (new_time)
	if new_time != old_time and old_time != 0:
		print("insert code here")
	old_time = new_time
	time.sleep(5)