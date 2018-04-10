class L{
	main(argv: Int, argc: String) : Int{{

		-- This is a conflict. Below are ways of explain them.
		-- (let a:A in 3) + (let b:B <- 3 in 5);
		-- let a:A in (3 + let b:B <- 3 in 5);
		let a:A in 3 + let b:B <- 3 in 5;

		let a: A <- 1 in {1;};

		let a: A <- 1, b: B <- 243, c: C <- 0 in {
			c <- a + b ;
		};
		let a: A in {
			a <- a + 1;
		};

		let a: A in { c <- a + b ;};

		{ let a:A in {3;}; "This is a long test about let."; let block: Block in {2;};};

	}};
};
