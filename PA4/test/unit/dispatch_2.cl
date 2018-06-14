-- Tested and passed. --
-- Jianxiong Cai 14 Jun, 2018 --

class Main{
	a: A <- new A;
	b: B <- new B;

	cout: IO <- new IO;

	main(): Int{{
		b.getA();
		a.getA();
		b.getA();
		1;
	}};
};

class A{
	a: Int <- 1;
	getA(): Int { a };
};

class B inherits A {
	incA(): Int {{ a <- a + 1; 10; }};
};