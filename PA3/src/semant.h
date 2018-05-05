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
  std::map<Symbol, std::map<Symbol, Symbol> > attr_map_;

public:
  ClassTable(Classes);
  void checkFeatureInheritance();
    void collectFeatures(Class_);
  void checkEachClassType();
    void checkMethodsType(Class_);
  void checkMethodInheritance();

  // return NULL on type_check failure / NULL is passed as expr_in
  Symbol getExpressionType(Class_ c, Expression expr_in, SymbolTable<Symbol, Symbol>& scope_table);

  // type checking: expr_in <= type_infer
  bool checkExpressionType(const Symbol type_defined_in,
                           const Symbol type_infer_in,
                           const SymbolTable<Symbol, Symbol>& scope_table,
                           const Symbol class_name);

  void collectMethods(Class_);

  bool le(Symbol, Symbol);
  Symbol getSharedParent(Symbol, Symbol);
  std::deque<Symbol> getInherVec(Symbol);

  int errors() { return semant_errors; }
  ostream& semant_error();
  ostream& semant_error(Class_ c);
  ostream& semant_error(Symbol filename, tree_node *t);

  template <class K, class V>
  std::map<K, bool> initCheckMap(std::map<K, V>&);

  template <class K, class V>
  bool hasKeyInMap(K, std::map<K, V>);
  std::map<Symbol, Class_>& getMap(){ return inher_map_; }
};

//bool operator <= (Symbol, Symbol){ return true; };

#endif
