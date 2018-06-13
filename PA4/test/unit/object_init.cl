class Main{
	a: Int <- 1;
	b: Main;
	main(): Int{{
		let a: Int in 	a <- ~a;
		1;
	}};
};

class A{
	a: A;
	ai: Int <- 1;
	ab: String;
};

class B inherits A {
	b: Main<- new Main;
	bb: Bool <- false;
};