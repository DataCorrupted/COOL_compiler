class Main{
	mid : Int;
	y : Bool;

	main(): Int {
		{
			mid <- (let x : Int in (x + 12));
			mid <- (let mid : Int in (x + 12));
			mid <- (let x : Int <- x in (x + 12));
			x + 2;
		}
	};

	foo(): Int{
		{
			id <- (let x : Int <- x in (x < 12));
			mid <- (let x : Int <- y in (x < 12));
			mid <- (let x : Int <- y in (x + 12));
		}
	};
};

