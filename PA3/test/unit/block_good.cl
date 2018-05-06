class Main{
	mid : Int;
	x : Bool;
	c: Bool;

	main(): Bool {
	    {
	        mid <- 19;
	        x <- true;
	    }
	};

	foo(): Bool {
	    {
	    	c <- true;
	    	{
	        	mid <- 19;
	        	x <- true;
	    	};
	    	c <- false;
	    }
	};
};