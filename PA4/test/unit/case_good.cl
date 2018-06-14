-- Tested and passed. --
-- Peter Rong 14 Jun, 2018 --
class Main{
	mid : Int <- 1;
	cout : IO <- new IO;
	main(): Int {{
		case mid of
			x : Int => self.print(1);
			y : Bool => self.print(2);
		esac;
		0;
	}};

	print(a: Int): Int {{
		cout.out_int(a);
		cout.out_string("\n");
		a;
	}};
};