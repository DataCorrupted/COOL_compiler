-- Tested and passed. --
-- Peter Rong 14 Jun, 2018 --
class Main{
	cout : IO <- new IO;

	main(): Int{{
		let a: Int in {
			a <- 10;
			cout.out_string("a <- 10\n");
			cout.out_string("a: "); 	self.printInt(a);
			cout.out_string("~a: "); 	self.printInt(~a);
			
			cout.out_string("a <- ~a\n");
			cout.out_string("a: "); 	self.printInt(a);
			cout.out_string("~a: "); 	self.printInt(~a);

(*			This should cause Arithmetic overflow.
			a <- 0-2147483648;
			cout.out_string("a <- -2147483648\n");
			cout.out_string("a: "); 	self.printInt(a);
			cout.out_string("~a: "); 	self.printInt(~a);
			
			cout.out_string("a <- ~a\n");
			cout.out_string("a: "); 	self.printInt(a);
			cout.out_string("~a: "); 	self.printInt(~a);
*)

			-- Yet this works as COOL saves signed value by default. --
			-- Giving it 4294967295 is actually giving it -1. --
			a <- 4294967295;
			cout.out_string("a <- 4294967295\n");
			cout.out_string("a: "); 	self.printInt(a);
			cout.out_string("~a: "); 	self.printInt(~a);
			
			cout.out_string("a <- ~a\n");
			cout.out_string("a: "); 	self.printInt(a);
			cout.out_string("~a: "); 	self.printInt(~a);
		};
		1;
	}};

	printInt(i: Int): Int{{
		cout.out_int(i);
		cout.out_string("\n");
		i;
	}};
};