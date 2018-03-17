class Main {
	cells : CellularAutomaton;
   
   -- This is a comment.
    main() : SELF_TYPE {
		{
			cells <- (new CellularAutomaton).init("         X         ");
			cells.print();
			(let countdown : Int <- 20 in
				while countdown > 0 loop
					{
						cells.evolve();
						cells.print();
						countdown <- countdown - 1;
					
				pool
			);  (* end let countdown *)
			self;
		}
	};
};
