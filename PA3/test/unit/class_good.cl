class Base {
	base : Int;
};

class Mid0 inherits Base {
	mid : Int;
};

class Mid1 inherits Base {
	mid : Int;
};

class Util inherits Mid1 {
	util : Int;
};

class Print inherits IO {
	print : Int;
};

class Bottom00 inherits Mid0 {
	b00 : String;
};

class Bottom01 inherits Mid0 {
	b01 : String;
};

class Bottom10 inherits Mid1{
	b10 : Int;
};	
class Bottom11 inherits Mid1{
	b11 : Int;
};	

class Main inherits IO {
	main(): Int {
		1
	};
};