class Main{
	b: Bool;
	a: A <- new A;
	main(): Int{{
		new SELF_TYPE;
		b <- a.c(new SELF_TYPE, 1);
		1;
	}};
};

class Base{
	m(i: Int, b: Base): Base{
		new Base
	};
};

class A inherits Base{
	m(i: Int, b: Base): Base{
		new A
	};
};