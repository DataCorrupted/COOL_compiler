# Contribution

## Rong Yuyang

### Contact:

rongyy@shanghaitech.edu.cn, ID: 69850764

### Contributions:

Class Inheritance check

Methods and attributes' inheritance & type checks

Static dispatch & dispatch

Arithmetic & Compare operators

Some helper functions

Testing scripts and some tests

### Implemented functions:

```cpp
	// Check all classes and create a inhertance map.
	ClassTable(Classes);
	
	// Check all features are inherited correctly and put them in a map.
	void checkFeatureInheritance();
	
	// For each class and method, check if their returned tpye and declared ones match.
	void checkEachClassType();
	
	// Find Main class and main method
	void checkMainExists();

	// Collect all features in a class and organize into two maps.(attr_map_ and method_map_)
	void collectFeatures(const Class_);
	// Check if parameters and return type matches definition
	void checkMethodsReturnType(const Class_);
	// Check if method used undefined type as return type or parameter type.
	const bool checkMethodSignType(const Class_, const Method, SymbolTable<Symbol, Entry>&);

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

	// Find the closet parent for two classes
	const Symbol getSharedParent(const Symbol, const Symbol) const;
	const std::deque<Symbol> getInherDQ(Symbol) const;

	// Initiate a map indicating all class as unchecked.
	template <class K, class V>
	std::map<K, bool> initCheckMap(const std::map<K, V>&) const;
	// Tell if there is one key in a map.
	template <class K, class V>
	const bool hasKeyInMap(const K, const std::map<K, V>&) const;
```
## Cai Jianxiong

### Contact:

caijx@shanghaitech.edu.cn, ID: 67771603

### Contributions:

Expression Type check (everything but excluding dispatch & Arithmetic)

AST type infer and assignment

Related testing and debugging

### Implemented functions:
```cpp
  // type checking: expr_in <= type_infer
  bool checkExpressionType(const Symbol type_defined_in,
                           const Symbol type_infer_in,
                           const SymbolTable<Symbol, Entry>& scope_table,
                           const Symbol class_name);
  bool checkClassExist(const Symbol);

  // return NULL on type_check failure / NULL is passed as expr_in
  void getExpressionType(
    const Class_ c, const Expression expr_in, SymbolTable<Symbol, Entry>& scope_table);
 
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