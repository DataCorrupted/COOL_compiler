class Main{
	mid : A;

	main(): Int {
	    case mid of
	    	x : Int => (1+1+1*9);
	    	y : Bool => (1+2);
	    	z : A => 1;
	    esac
	};
};

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

	print(a: Int){{
		cout.out_int(a);
		cout.out_string("\n");
		a;
	}};
};

Class A{
	a: Int <- 1;
};