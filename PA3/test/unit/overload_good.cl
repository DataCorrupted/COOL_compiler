class Main{
	mid : Int;

	main(): Int{
		1
	};

	abort(): Object {
		{
			mid <- (let mid : Int in (mid + 12));
		}
	};

	foo(): Int{
		1
	};
};

class A inherits Main{
	foo(): Int{
		2
	};
};