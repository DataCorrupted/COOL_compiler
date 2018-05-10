# PA3: cool semantic analysis

This folder originates from Stanford CS143. Missing folders and files(mainly binary, which is not suitable for git) can be retrived from their [couse page](http://web.stanford.edu/class/cs143/).(Or [ShanghaiTech](http://sist.shanghaitech.edu.cn/faculty/songfu/course/spring2018/CS131/)'s CS131 should the course page work by the time you see this.)

## src

	semant.(cc/h)	are my code. You can read them or modify them.
	cool-tree.h* 	also my code.
	Makefile 	Use command "make" to generate a binary. You should not modify them.

## test

	(good/bad).cl 	are provided test. 
	unit/ 		I will write my own unit test when I have time.

## scripts

	compile.sh 	While compling can be tiresome, I write a script for that. It will generate
			a binary and put it in PA3/
	cmp.py <file>	will compile your code, run lexer and compare your parser's output and 
			official's(tools-bin/semant) given the input <file> you specified.
	all_test.py <path> 
			Test all *.cl files in the given path. 

The environment for these two scripts should be in _PA3/_, or they may not work. Run them by
	
	./scripts/<script> {file}

## cool-support

cool-support is provided by CS143, yet it contains some important files that you should really read and understand.

# Contribution

## Rong Yuyang

### Contact:

rongyy@shanghaitech.edu.cn, ID: 69850764

### Contribution 

Class Inheritance check

Methods and attributes' inheritance checks

Static dispatch & dispatch

Arithmetic & Compare operators

### Implemented functions:

```cpp
	ClassTable(Classes);
	void checkFeatureInheritance();
		void collectFeatures(const Class_);
	void checkEachClassType();
		void checkMethodsReturnType(const Class_);
		const bool checkMethodSignType(const Class_, const Method, SymbolTable<Symbol, Entry>&);
	void checkMainExists();

	// Check static and dynamic dispatch at the same time, as they are ultimately the same thing.
	template <class Dispatch>
	void assignDispatchType(const Class_, const Expression, SymbolTable<Symbol, Entry>&);
	// Check <, <= and =.
	template <class Compare>
	void assignCompareType(const Class_, const Expression, SymbolTable<Symbol, Entry>&);
	// Check +, -, *, /
	template <class Arithmetic>
	void assignArithmeticType(const Class_, const Expression, SymbolTable<Symbol, Entry>&);

	// Check if two type satisfy a <= b;  
	const bool le(Symbol, const Symbol) const;
	const Symbol getSharedParent(const Symbol, const Symbol) const;
	const std::deque<Symbol> getInherDQ(Symbol) const;
	template <class K, class V>
	std::map<K, bool> initCheckMap(const std::map<K, V>&) const;
	template <class K, class V>
	const bool hasKeyInMap(const K, const std::map<K, V>&) const;
```
## Cai Jianxiong

### Contact:

caijx@shanghaitech.edu.cn, ID: 69850764

### Contribution

### Implemented functions
```cpp
  // type checking: expr_in <= type_infer
  bool checkExpressionType(const Symbol type_defined_in,
                           const Symbol type_infer_in,
                           const SymbolTable<Symbol, Entry>& scope_table,
                           const Symbol class_name);
  bool checkClassExist(const Symbol);

  // return NULL on type_check failure / NULL is passed as expr_in
  void getExpressionType(const Class_ c, const Expression expr_in, SymbolTable<Symbol, Entry>& scope_table);
 
  // type_error (type mismatch)
  void semant_type_error(
    const Class_&  c, tree_node *expr_in,
    const Symbol& type_infer, const Symbol& type_defined, const Symbol& id_name);

  // type error while initializing variable
  void semant_init_type(const Class_& c, tree_node * expr_in, const Symbol& type_infer,
                        const Symbol& type_defined, const Symbol& id_name);

  // undeclared identifier
  void semant_undeclared_id(const Class_& c, tree_node * expr_in, const Symbol& id_name);
```