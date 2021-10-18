# Loadavg-generator

Will create a lot of threads, limited to 2 CPUs to avoid hanging the system.
Code is avoiding pthread to avoid eating too much memory.

However, kernel task_struct will consume a lot of ram nontheless.

	$ sudo \time ./run.sh 4000000 2000000
	Will try to run 4000000 threads and stop at 2000000 load
	Started 4000000 threads
	    0 Load: 2 (2)
	    5 Load: 320315 (320313)
	   10 Load: 614977 (294662)
	   15 Load: 886044 (271067)
	   20 Load: 1135404 (249360)
	   25 Load: 1364796 (229392)
	   30 Load: 1575819 (211023)
	   35 Load: 1769944 (194125)
	   40 Load: 1948523 (178579)
	   45 Load: 2112802 (164279)
	104.27user 71.73system 1:44.09elapsed 169%CPU (0avgtext+0avgdata 16000296maxresident)k
	0inputs+0outputs (0major+4003112minor)pagefaults 0swaps
	 10:47:18 up 31 days, 18:48,  6 users,  load average: 221381.63, 66572.37, 22953.94

