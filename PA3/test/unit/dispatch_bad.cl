class Main{
	main(): Int{{
		let a:A <- new A in {
			a.m(new SELF_TYPE, 1);
		};
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