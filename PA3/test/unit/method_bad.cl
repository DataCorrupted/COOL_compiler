class Main inherits IO{
	main(): Int {
		"1"
	};
};

class A{
	a(i: Int, io: IO): SELF_TYPE{
		self.a(1, new D)
	};


	a(i: Int, io: D): SELF_TYPE{{
		let c: Bool <- 1 in 1;
		self.a(1, new IO);
	}};


	c(i: Int, io: IO): A{
		new SELF_TYPE
	};

	d(i: Int, io: IO): SELF_TYPE{
		new SELF_TYPE
	};

};