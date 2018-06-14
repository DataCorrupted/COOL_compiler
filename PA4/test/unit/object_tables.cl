-- Tested and passed. --
-- Peter Rong 14 Jun, 2018 --
class A{
	base0: Int;
	base1: Int;

	method0(): Int {
		1
	};
	method1(): Int{
		2
	};
};

class B inherits A{
	a: A;
	another_base: Int;

	-- Overload --
	method0(): Int{
		2
	};
	another_method(): Int{
		2
	};
};

class C inherits B{
	final_base: Int;
	b: B;
	c: C;
	fianl_method(): Int{
		3
	};
};

class D inherits B{
	copy_base: Int;

	method0(): Int{
		3
	};
	another_method(): Int{
		3
	};
	copy_method(): Int{
		4
	};
};

class Main{
	main(): Int{
		6
	};
};