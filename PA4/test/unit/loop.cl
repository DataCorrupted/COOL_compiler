-- Tested and passed. --
-- jianxiong Cai 14 Jun, 2018 --
class Main{
	a: Int <- 1;
	i: Int <- 0;
	cout : IO <- new IO;
	main(): Object{{
		while i < (20+10) loop
		{
			cout.out_int(i);
			cout.out_string(" ");
			i <- i + 1;
		}
		pool;
	}};
};