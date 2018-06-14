class Main{
	mid : Int <- 1;
	cout : IO <- new IO;
	main(): Int {{
	    case mid of
	    	x : Int => cout.out_int(1);
	    	y : Bool => cout.out_int(2);
	    esac;
	    1;
	}};
};