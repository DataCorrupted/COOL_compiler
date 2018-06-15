-- Tested and passed. --
-- Peter Rong 15 Jun, 2018 --
class Main{
	mid : Int <- 3;
	cout : IO <- new IO;
	x : Int <- 2; 
	main(): Int {{
		cout.out_string("Expecting 3, got: ");
		case mid of
			x : Int => self.printInt(x);
			y : Bool => self.printInt(2);
		esac;

		mid <- mid + 1;

		cout.out_string("Expecting 24, got: ");
		case mid of
			z : Int => self.printInt((z + 2) * z);
			y : Bool => self.printInt(mid);
		esac;
		0;
	}};

	printInt(a: Int): Int {{
		cout.out_int(a);
		cout.out_string("\n");
		a;
	}};
};