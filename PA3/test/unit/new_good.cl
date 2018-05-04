class Main{
	x: String;
	obj : Main;

	main() : String{
		x <- "Hello"
	};

	foo(): SELF_TYPE{
		new SELF_TYPE
	};

	bar(): Main{
		new SELF_TYPE
	};

	foo2() : Main{
		obj <- (new SELF_TYPE)
	};
};

class Bar{

};