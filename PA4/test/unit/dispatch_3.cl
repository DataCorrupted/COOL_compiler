class Main{
	a: Int <- 1;
	b: Main;
	cout : IO <- new IO;

	foo(x: Int, y:Int): Int{{
		cout.out_int(x);
		cout.out_int(y);
		0;	
	}};

	bar(x: Int, y:Int): Int{{
		cout.out_string("Main.foo get called");
		0;
	}};

	main(): Int{{
		b <- new Main;
		self.foo(1,2);
		1;
	}};
};