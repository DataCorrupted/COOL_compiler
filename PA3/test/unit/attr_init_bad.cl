
class A{
	a: Int;
	m(): Int{ 1};
};

class Main{
	c: String <- {
		1 < 2;
		let a: Int <- 1 in {
			a <- a + 1;
			a;
		};
	};
	d: Int;
	main(): Int{
		1
	};
};

class B inherits A{
	a: Int <- 1	;
	m(): String{ 1};
};