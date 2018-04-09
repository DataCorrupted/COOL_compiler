class Feature {
	a: A;
	b: B <- 23;
	c: C <- {
		a <- 23;
		d: D <- a + b;
		while d*2 < 1000 loop {
			d <- d * 2;
			e: E <- isvoid d;
		} pool;
		d;
	}

	main() : SELF_TYPE a;
	another_main( a: String, b: Main) {
		c.main(34, d);
	};
};