# Loadavg-generator

Will create a lot of threads, limited to 2 CPUs to avoid hanging the system.
Code is avoiding pthread to avoid eating too much memory.

However, kernel task_struct will consume a lot of ram nontheless.

	$ sudo ./run.sh 4000000 2000000
	Will try to run 4000000 threads and stop at 2000000 load
	Started 4000000 threads
	    0 Load: 194380 (194380)
	    5 Load: 499128 (304748)
	   10 Load: 779471 (280343)
	   15 Load: 1037365 (257894)
	   20 Load: 1274608 (237243)
	   25 Load: 1492852 (218244)
	   30 Load: 1693620 (200768)
	   35 Load: 1878311 (184691)
	   40 Load: 2048212 (169901)
	 12:43:07 up 31 days, 20:44,  6 users,  load average: 1978913.23, 653145.05, 230518.12
