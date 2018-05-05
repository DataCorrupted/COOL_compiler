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

class Bar inherits Main{
    y_1 : Main;
    y_2 : SELF_TYPE;

    yoo_1() : Main {
        y_1 <- new SELF_TYPE
    };

    yoo_2() : SELF_TYPE{
        y_2 <- new SELF_TYPE
    };

    yoo_3() : SELF_TYPE{
        {
            y_2 <- new Bar;
            new SELF_TYPE;
        }
    };
};