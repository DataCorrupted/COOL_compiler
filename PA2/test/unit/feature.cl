class Feature {
	a: A;
	b: B <- 23;
	c: C <- {
		m;
		a <- 23;
		d <- a + b;
		while d*2 < 1000 loop {
			d <- d * 2;
			e <- isvoid d;
		} pool;
		d;
	};

	main() : SELF_TYPE {a};
	another_main( a: String, b: Main) : Int {
		c.main(34, d)
	};
};