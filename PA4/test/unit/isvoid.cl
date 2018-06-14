class Main{
	a: Int;
	b: Bool;
	a_void: A;
	a_nonvoid: A <- new A;

	cout: IO <- new IO;
	main(): Bool{{
		outBool(isvoid a_void);
		outBool(isvoid a_nonvoid);
		false;
	}};

	outBool(b: Bool): Int{{
		if b then
			cout.out_string("True")
		else
			cout.out_string("False")
		fi;		
		1;
	}};
};

class A{
	a: Int <- 1;
};