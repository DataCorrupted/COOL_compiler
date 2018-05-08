class Main{
	mid : Int;

	main(): Int {
		{
			mid <- (let mid : Int, x : Int, x : Bool in (mid + 12));
			mid <- (let mid : Int <-97 in (mid + 12));
			mid <- (let x : Int <- 78 in (x + 12));
			mid <- (let x : Int <- 78, y: Bool <- true in (x + 12));
		}
	};

	foo(): Object {
		{
			mid <- (let x : Int in (x + 12));
		}
	};

	abort(): Object {
		{
			mid <- (let mid : Int in (mid + 12));
		}
	};
};