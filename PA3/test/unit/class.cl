class C inherits A {
	a : Int;
	b : Bool;
	init(x : Int, y : Bool) : C {
           {
		a <- x;
		b <- y;
		self;
           }
	};
};

class A inherits C{
	a : Int;
};

class D inherits E{
	b : Int;
};

class Int{
	a : Int;

};

class D inherits A{
	b : Int;
};

class SELF_TYPE {
	self_type : Int;
};
 