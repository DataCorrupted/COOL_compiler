-- Tested and passed. --
-- Jianxiong Cai 14 Jun, 2018 --

class Main{
	a: A <- new A;
	b: B <- new B;

	cout: IO <- new IO;

	main(): Int{{
		b.getA();
		a.getA();
		b@A.getA();
		1;
	}};
};

class A{
	a: Int <- 1;
	cout2: IO <- new IO;
	getA(): Int {{ cout2.out_string("A.getA get called\n"); a; }};
};

class B inherits A {
	incA(): Int {{ a <- a + 1; 10; }};
	getA(): Int {{ cout2.out_string("B.getA get called\n"); a; }};
};
