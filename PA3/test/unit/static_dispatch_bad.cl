class GrandParent {
	method(i: Int, s: String): Int{
		1
	};
};

class Father inherits GrandParent{
	method(i: Int, s: String): Int{
		2
	};
};

class Mother inherits GrandParent{
	method(i: Int, s: String): Int{
		2
	};
};

class Child1 inherits Father{
	age: Int <- 3;
};

class Child2 inherits Mother{
	age: Int <- 4;
};

class Child3 inherits Mother{
	age: Int <- 3;
	method(i: Int, s: String): Int {
		5
	};
};


class Main{
	g: GrandParent <- new GrandParent;
	c1: Child1 <- new Child1;
	b: Bool;
	main(): Int{{ 
		g.method(1, "S");
		b <- c1@Mother.method(1, 2);
		c1@Father.m("#", 1, 1);
		c1@GrandParent.method(1, "3");
		1; 
	}};
};