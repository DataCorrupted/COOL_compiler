class Int3 inherits Int2{

	bar2(x: Bool, y:Int): Int{
		1
	};

	foo(x: Bool, y:Int): Int{
		2
	};

	ba3(x: Bool, y:Int): Int{
		1
	};
};

class Int2{
	a2: Int;

	foo(x: Bool, y:Int): Int{
		1
	};
};

class Main{
	a: Int3 <- new Int3;

	bar(x: Bool, y:Int): Int{
		1
	};

	main(): Int{{
		a@Int2.foo(true,1);
		0;
	}};
};