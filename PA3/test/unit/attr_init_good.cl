class Main{
	a: Int <- b;
	b: Int <- 1;
	main(): Int{
		1
	};
};

class Base {
	base: Int <- {
		1 < 1;
		(1 + 1 + 1) * 3;
	};
};

class A inherits Base {
	a: String <- {
		"String";
	};
};

class B inherits Base {
	b: Main <- new Main;
};