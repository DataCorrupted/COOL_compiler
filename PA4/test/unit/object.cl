class A{
	base0: Int;
	base1: Int;

	method0(): Int {
		1
	};
};

class B inherits A{
	another_base: Int;

	another_method(): Int{
		2
	};
};

class C inherits B{
	final_base: Int;

	fianl_method(): Int{
		3
	};
};

class D inherits B{
	copy_base: Int;

	copy_method(): Int{
		4
	};
};

class Main{
	main(): Int{
		6
	};
};