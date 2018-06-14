-- Tested and passed. --
-- jianxiong Cai 14 Jun, 2018 --
class Main{
	a: Int <- 1;
	i: Int <- 30;
	cout : IO <- new IO;
	main(): Object{{
		while not ((i<-(i-1)) = 0) loop
		{
			cout.out_int(i);
			cout.out_string(" ");
		}
		pool;
	}};
};