-- Tested and passed. --
-- Peter Rong 14 Jun, 2018 --
class Main{
	a: Int;
	b: Int;


	cout: IO <- new IO;

	main(): Bool{{
		cout.out_string("Testing Int eq...\n");
		cout.out_string("a = b: "); 				self.printBool(a = b);			
		cout.out_string("1 = 2: "); 				self.printBool(1 = 2);		
		cout.out_string("\n");

		cout.out_string("Testing String eq...\n");
		cout.out_string("\"sdf\" = \";lk\": "); 	self.printBool("sdf" = ";lk");	
		cout.out_string("\"abc\" = \"abc\": "); 	self.printBool("abc" = "abc");	
		cout.out_string("\n");

		cout.out_string("Testing Bool eq...\n");
		cout.out_string("true = false: "); 			self.printBool(true = false);
		cout.out_string("false = false: "); 		self.printBool(false = false);
		cout.out_string("\n");

		cout.out_string("Testing general object eq...\n");
		let c: C <- new C in
		let d: C <- new C in{
			cout.out_string("c = c: ");				self.printBool(c = c);
			cout.out_string("c = d: ");				self.printBool(c = d);
			cout.out_string("\n");
			c.incA();
			cout.out_string("c = c: ");				self.printBool(c = c);
			cout.out_string("c = d: ");				self.printBool(c = d);			
		};
	}};

	printBool(b: Bool): Bool{{
		if b 
			then cout.out_string("True\n")
			else cout.out_string("False\n")
		fi;
		b;
	}};
};

class C {
	a: Int <- 1;
	incA(): Int {{ a <- a + 1; a; }};
};