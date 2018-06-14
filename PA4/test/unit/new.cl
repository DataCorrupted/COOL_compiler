class Int2{
	a2: Int;
	cout : IO <- new IO;

	foo(): Object{
		cout.out_string("Int2.foo is called\n")
	};
};

class Main{
	a: Int2;

	bar(x: Bool, y:Int): Int{
		1
	};

	init_var(): Int2{
		a <- new Int2
	};

	main(): Int{{
		init_var();
		a.foo();
		0;
	}};
};