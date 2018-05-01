class Main inherits IO{
	main(): Int {
		1
	};
};

class Base {
	operator(argc: Int): Int{
		1
	};
	something(): String {
		"2"
	};
};

class Mid inherits Base {
	operator(arg: String): Int{ 1 };
	something(): String{
		"1"
	};
	something(): Int {
		1
	};
	somethingelse(): Int {
		1
	};
	somethingelse(): String{
		"1"
	};
};