class Main{

	cout : IO <- new IO;
	m : Main;

	main(): Int{{
		let a: Int <- 1 in {
			a <- a + 10;
			m <- self;
			m.print(a);
		};
		let b: B in 2;

        let countdown : Int <- 20 in
			while 0 < countdown loop {
				countdown <- countdown - 1;
				self.print(countdown);
			} pool
        ;
        1;
	}};

	print(a: Int): Int{{
		cout.out_int(a);
		cout.out_string("\n");
		a;
	}};
};
class B{
	b: Int <- 1;
};