-- Tested and passed. --
-- Peter Rong 14 Jun, 2018 --
class Main{

	a: Int;
	cout : IO <- new IO;

	main(): Int{{
		a <- 1;
		if (a < 0) then		cout.out_int(1)
		else				cout.out_int(0)
		fi;

		if (a < 2) then		cout.out_int(1)
		else				cout.out_int(0)
		fi;

		if true then 		cout.out_string("true")
		else				cout.out_string("false")
		fi;

		1;
	}};
};