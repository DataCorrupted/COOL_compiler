-- Tested and passed. --
class Main{
	a: Int <- 2;
	b: Int;
	cout : IO <- new IO;
	main(): Int{{
		cout.out_int(a);
		b <- a + a;		cout.out_int(b);
		b <- a - a; 	cout.out_int(b);
		b <- a * a; 	cout.out_int(b);
		b <- a / a; 	cout.out_int(b);
		cout.out_int((a + 1) / 3 + 10 * a + b);
		1;
	}};
};