class Main{
	b: D;
	a: A <- new A;
	c: A <- new A;
	main(): Int{{
		new SELF_TYPE;
		a <- b.c(new SELF_TYPE, 1);
		c <- a.c(new SELF_TYPE, 1);
		c <- a.m(1);
		c <- a.m(1, "str");
		c <- a.m(1, 1, 1, 1);
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