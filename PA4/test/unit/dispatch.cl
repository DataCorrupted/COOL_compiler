-- Tested and passed. --
-- Jianxiong Cai 14 Jun, 2018 --

class Main{
	a: Int <- 1;
	b: Main;
	cout : IO <- new IO;

	foo(x: Int, y:Int): Int{{
		cout.out_string("Main.foo get called with x=");
		cout.out_int(x);
		cout.out_string("   y=");
		cout.out_int(y);
		cout.out_string("\n");
		0;	
	}};

	bar(x: Int, y:Int): Int{{
		cout.out_string("Main.foo get called");
		0;
	}};

	main(): Int{{
		self.foo(200,1);
		1;
	}};
};