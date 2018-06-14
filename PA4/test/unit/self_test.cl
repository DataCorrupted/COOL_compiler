-- Tested and passed. --
-- jianxiong Cai 14 Jun, 2018 --
class Main{
	a: Int <- 1;
	i: Int <- 0;
	cout : IO <- new IO;

	s: Main <- self;

	foo():Object{
		cout.out_string("Calling Main.foo")
	};

	main(): Object{{
		s.foo();
	}};
};
