-- Tested and passed. --
-- Peter Rong 14 Jun, 2018 --
class Main{
	cout : IO <- new IO;
	s: Main;

	bar(x: Int) : Object{{
		cout.out_string("Main.bar is called with x=");
		cout.out_int(x);
		cout.out_string("\n");
	}};

	main(): Int{{
		s <- new SELF_TYPE;
		s.bar(100);
		0;
	}};
};
