#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>  
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include "list.h"

#include <map>
#include <deque>

#define TRUE 1
#define FALSE 0

class ClassTable;
typedef ClassTable *ClassTableP;

// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.

class ClassTable {
private:
	int semant_errors;
	void install_basic_classes();
	ostream& error_stream;

	std::map<Symbol, Class_> inher_map_;

	std::map<Symbol, bool> checked_;

	// method_map[class_name][method_name] = method.
	std::map<Symbol, std::map<Symbol, Method> > method_map_;
	std::map<Symbol, std::map<Symbol, Attribute> > attr_map_;
public:
	// Check all classes and create a inhertance map.
	ClassTable(Classes);
	// Check all features are inherited correctly and put them in a map.
	void checkFeatureInheritance();
	// For each class and method, check if their returned tpye and declared ones match.
	void checkEachClassType();
	// Find Main class and main method
	void checkMainExists();

	const int errors() const { return semant_errors; }

private:

	// Collect all features in a class and organize into two maps.(attr_map_ and method_map_)
	void collectFeatures(const Class_);
	// Check if parameters and return type matches definition
	void checkMethodsReturnType(const Class_);
	// Check if method used undefined type as return type or parameter type.
	const bool checkMethodSignType(const Class_, const Method, SymbolTable<Symbol, Entry>&);


	// type checking: expr_in <= type_infer
	bool checkExpressionType(const Symbol type_defined_in,
													 const Symbol type_infer_in,
													 const SymbolTable<Symbol, Entry>& scope_table,
													 const Symbol class_name);
	bool checkClassExist(const Symbol);


	// return NULL on type_check failure / NULL is passed as expr_in
	void getExpressionType(const Class_ c, const Expression expr_in, SymbolTable<Symbol, Entry>& scope_table);
	
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

	ostream& semant_error();
	ostream& semant_error(Class_ c);
	ostream& semant_error(Symbol filename, tree_node *t);

	// type_error (type mismatch)
	void semant_type_error(
		const Class_&  c, tree_node *expr_in,
		const Symbol& type_infer, const Symbol& type_defined, const Symbol& id_name);

	// type error while initializing variable
	void semant_init_type(const Class_& c, tree_node * expr_in, const Symbol& type_infer,
												const Symbol& type_defined, const Symbol& id_name);

	// undeclared identifier
	void semant_undeclared_id(const Class_& c, tree_node * expr_in, const Symbol& id_name);

	// Initiate a map indicating all class as unchecked.
	template <class K, class V>
	std::map<K, bool> initCheckMap(const std::map<K, V>&) const;
	// Tell if there is one key in a map.
	template <class K, class V>
	const bool hasKeyInMap(const K, const std::map<K, V>&) const;
};

#endif
