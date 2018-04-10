class A {
	main ( ) : A {
		{
			234;
			true;
			"String";
			main;
			(main);
			not main;
			23 = 43;
			12 <= 21;
			43 < 11;
			~234;

			23+23;
			43-34;
			12/34;
			45*43;

			isvoid main;
			new A;

			case {23; 43; "String";} of 
				a: A => {a <= b;};
				b: B => isvoid main;
				c: C => 23 + help;
				d: D => "Testing case.";
			esac;
			while 345 loop a <- a + 1 pool;
			while {a = a;} loop {
				a <- a + 1;
				case b = 1 of
					a: A => {234;};
				esac;
			} pool;
			if 1 then {1;} else {2; l <- 1;} fi;
			a.func();
			a.func(34, b, b<0);
			a@A.func({b<0;}, 12, 4);

			func(12, 342, 3);
		}
	};
};

