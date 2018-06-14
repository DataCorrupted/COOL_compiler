class Main{
	a: A <- new A;
	b: B <- new B;

	cout: IO <- new IO;

	main(): Int{{
		b.incA();
		a <- b;

		cout.out_int(b.getA());
		cout.out_int(a.getA());
		cout.out_int(b.getA());
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