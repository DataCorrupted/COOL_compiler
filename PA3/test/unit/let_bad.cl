class Main{
	mid : Int;

	main(): Int {
		{
			mid <- (let x : Int in (x + 12));
			mid <- (let mid : Int in (x + 12));
			mid <- (let x : Int <- x in (x + 12));
			x + 2;
		}
	};
};