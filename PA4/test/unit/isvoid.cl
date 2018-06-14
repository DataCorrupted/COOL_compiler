-- Tested and passed. --
-- Peter Rong 14 Jun, 2018 --
class Main{
	a: Int <- 1;
	b: Bool;
	a_void: A;
	a_nonvoid: A <- new A;

	cout: IO <- new IO;
	main(): Bool{{
		if isvoid a_void then
			cout.out_string("True\n")
		else
			cout.out_string("False\n")
		fi;		

		if isvoid a_nonvoid then
			cout.out_string("True\n")
		else
			cout.out_string("False\n")
		fi;		
		false;
	}};

};

class A{
	a: Int <- 1;
};