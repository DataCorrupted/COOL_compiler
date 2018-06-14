-- Tested and passed. --
-- Peter Rong 14 Jun, 2018 --
class Main{
	b: Bool <- true;
	cout : IO <- new IO;

	main(): Bool{{
		self.print(b);

		b <- not b;
		self.print(b);
		
		b <- not b;
		self.print(b);
	}};

	print(b: Bool): Bool{{
		if b 
			then cout.out_string("True\n")
			else cout.out_string("False\n")
		fi;
		b;
	}};
};