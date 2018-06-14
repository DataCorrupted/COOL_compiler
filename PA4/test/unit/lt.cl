-- Tested and passed. --
-- Peter Rong 14 Jun, 2018 --
class Main{
	a: Int <- 1;
	b: Int <- 2;
	c: Bool;
	stdout: IO <- new IO;
	main(): Bool{{
		if (a = b) then
			stdout.out_int(1)
		else
			stdout.out_int(2)
		fi;
		if (a < b)
		then
			c <- true
		else
			c <- false
		fi;
	}};
};