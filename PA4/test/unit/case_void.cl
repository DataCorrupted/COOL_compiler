-- Tested and passed. --
-- Peter Rong 14 Jun, 2018 --
class Main{
	mid : A;
	cout : IO <- new IO;

	main(): Int {{
		case mid of
			x : A => cout.out_int(1);
			y : Bool => cout.out_int(2);
		esac;
		0;
	}};

	print(a: Int): Int {{
		cout.out_int(a);
		cout.out_string("\n");
		a;
	}};
};

Class A{
	a: Int <- 1;
};