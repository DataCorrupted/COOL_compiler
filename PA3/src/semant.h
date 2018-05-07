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
    void collectFeatures(const Class_);
  void checkEachClassType();
    void checkMethodsReturnType(const Class_);
    const bool checkMethodSignType(const Class_, const Method);
  void checkMainExists();

  // type checking: expr_in <= type_infer
  bool checkExpressionType(const Symbol type_defined_in,
                           const Symbol type_infer_in,
                           const SymbolTable<Symbol, Symbol>& scope_table,
                           const Symbol class_name);

  // return NULL on type_check failure / NULL is passed as expr_in
  void getExpressionType(const Class_ c, const Expression expr_in, SymbolTable<Symbol, Symbol>& scope_table);
  
  // Check static and dynamic dispatch at the same time, as they are ultimately the same thing.
  template <class Dispatch>
  void assignDispatchType(const Class_, const Expression, SymbolTable<Symbol, Symbol>&);

  // Check <, <= and =.
  template <class Compare>
  void assignCompareType(const Class_, const Expression, SymbolTable<Symbol, Symbol>&);

  // Check +, -, *, /
  template <class Arithmetic>
  void assignArithmeticType(const Class_, const Expression, SymbolTable<Symbol, Symbol>&);

  // Check if two type satisfy a <= b;  
  const bool le(Symbol, const Symbol) const;
  const Symbol getSharedParent(const Symbol, const Symbol) const;
  const std::deque<Symbol> getInherVec(Symbol) const;

  int errors() { return semant_errors; }
  ostream& semant_error();
  ostream& semant_error(Class_ c);
  ostream& semant_error(Symbol filename, tree_node *t);

  // type_error
  void semant_type_error(Class_  c, tree_node *expr_in, Symbol type_infer, Symbol type_defined, Symbol id_name);

  
  template <class K, class V>
  std::map<K, bool> initCheckMap(const std::map<K, V>&) const;

  template <class K, class V>
  const bool hasKeyInMap(const K, const std::map<K, V>&) const;
};

#endif
