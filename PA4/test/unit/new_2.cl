class Int2{
	a2: Int;
	cout : IO <- new IO;

	foo(): Object{
		cout.out_string("Int2.foo is called\n")
	};

};

class Main{
	a: Int2;
	s: Main;

	cout : IO <- new IO;

	init_var(): Int2{{
		s <- new SELF_TYPE;
		a <- new Int2;
	}};


	bar(x: Int) : Object{{
		cout.out_string("Int2.bar is called with x=");
		cout.out_int(x);
		cout.out_string("\n");
	}};

	main(): Int{{
		init_var();
		s.bar(100);
		0;
	}};
};
