

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"

#include <typeinfo>
#include <iostream>

#include <string>
#include <typeinfo>
#include <vector>
#include <sstream>

extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
	arg,
	arg2,
	Bool,
	concat,
	cool_abort,
	copy,
	Int,
	in_int,
	in_string,
	IO,
	length,
	Main,
	main_meth,
	No_class,
	No_type,
	Object,
	out_int,
	out_string,
	prim_slot,
	self,
	SELF_TYPE,
	Str,
	str_field,
	substr,
	type_name,
	val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
	arg         = idtable.add_string("arg");
	arg2        = idtable.add_string("arg2");
	Bool        = idtable.add_string("Bool");
	concat      = idtable.add_string("concat");
	cool_abort  = idtable.add_string("abort");
	copy        = idtable.add_string("copy");
	Int         = idtable.add_string("Int");
	in_int      = idtable.add_string("in_int");
	in_string   = idtable.add_string("in_string");
	IO          = idtable.add_string("IO");
	length      = idtable.add_string("length");
	Main        = idtable.add_string("Main");
	main_meth   = idtable.add_string("main");
	//   _no_class is a symbol that can't be the name of any 
	//   user-defined class.
	No_class    = idtable.add_string("_no_class");
	No_type     = idtable.add_string("_no_type");
	Object      = idtable.add_string("Object");
	out_int     = idtable.add_string("out_int");
	out_string  = idtable.add_string("out_string");
	prim_slot   = idtable.add_string("_prim_slot");
	self        = idtable.add_string("self");
	SELF_TYPE   = idtable.add_string("SELF_TYPE");
	Str         = idtable.add_string("String");
	str_field   = idtable.add_string("_str_field");
	substr      = idtable.add_string("substr");
	type_name   = idtable.add_string("type_name");
	val         = idtable.add_string("_val");
}


template <class K, class V>
std::map<K, bool> ClassTable::initCheckMap(const std::map<K, V>& tmpl_map) const{
	std::map<K, bool> checked;
	for(typename std::map<K, V>::const_iterator iter = tmpl_map.begin(); 
	  iter != tmpl_map.end(); 
	  ++iter){
		checked.insert(std::pair<K, bool>(iter->first, false));
	}	
	return checked;
}

template <class K, class V>
const bool ClassTable::hasKeyInMap(const K key, const std::map<K, V>& map_) const{
	return map_.find(key) != map_.end();
}
ClassTable::ClassTable(Classes classes) : semant_errors(0) , error_stream(cerr) {

	/* Let's construct the basic classes first. */
	install_basic_classes();

	/* Check the usage and the exsitence of each Symbol first.  */
	for (int i = 0; i < classes->len(); i++){

		Class_ curr = classes->nth(i);
		
		// You can't define SELF_TYPE as a class.
		if (curr->getName() == SELF_TYPE){
			semant_error(curr) << "Redefinition of basic class SELF_TYPE.\n";

		// You can't define classes that have been declared too.
		} else if (hasKeyInMap(curr->getName(), inher_map_)){
			semant_error(curr) 
				<< "Redefinition of class " << curr->getName()
				<< ". Ignoring the later definition.\n";

		// A fresh new class name, you are good to go.
		} else {
			inher_map_.insert(std::pair<Symbol, Class_>(curr->getName(), curr));
		}
	}

	/* Check for inhertance correctness.                    */
	/* To guarantee speed, we only check one class once.    */
	/* We use a map to record if one class has been checked.*/
	/* Extra space made sure that the worst case time drop  */
	/* from O(N2) to O(N) 									*/
	
	// Construct a map with all flags saying not checked yet.
	// checked is a member of this class.
	checked_ = initCheckMap(inher_map_);
	for (std::map<Symbol, Class_>::iterator iter = inher_map_.begin(); 
	  iter != inher_map_.end(); 
	  ++iter){

		// This class is checked before, we move on.
		if (checked_[iter->first]) { continue; }

		// Take current class.
		Class_ curr = iter->second;

		// Label it as checked.
		checked_[curr->getName()] = true;
		Symbol parent_name = curr->getParent();

		// Vector to record dependency relation.
		std::vector<Class_> dep;
		dep.push_back(curr);

		// Either this class follows Object, or it's circular inherted.
		while (curr->getName() != Object 
			&& parent_name != iter->second->getName()){

			// First of all... the parent must exists,
			if (!hasKeyInMap(parent_name, inher_map_)){
				semant_error(curr) 
					<< "Class " << curr->getName() << " has undefined parent " 
					<< "class " << parent_name << " for inhertance.\n";
				break;

			// and is not one of the following...
			} else if (parent_name == SELF_TYPE || parent_name == Int 
					|| parent_name == Bool || parent_name == Str) {
				semant_error(curr) << "Class" << curr->getName() 
					<< " inherts from one of the following: SELF_TYPE, Int, Bool, Str,"
					<< " which is illegal.\n";
				break;

			// then find the grand-parent.
			} else {
				curr = inher_map_[parent_name];
				checked_[curr->getName()] = true;
				dep.push_back(curr);
				parent_name = curr->getParent();
			}
		}
		// After all the searching for my parent, I found myself?
		if (parent_name ==  iter->second->getName()){
			semant_error(iter->second) << "Circular inhertance found: ";
			for(unsigned int i=0; i<dep.size(); i++){
				error_stream << dep[i]->getName() << " -> ";
				if (i == dep.size() -1){
					error_stream << iter->second->getName() << ".\n";
				}
			}
		}
	}

	if (!hasKeyInMap(Main, inher_map_)){
		semant_error() << "Class Main not found.\n";
	}
}

void ClassTable::install_basic_classes() {

	// The tree package uses these globals to annotate the classes built below.
   // curr_lineno  = 0;
	Symbol filename = stringtable.add_string("<basic class>");
	
	// The following demonstrates how to create dummy parse trees to
	// refer to basic Cool classes.  There's no need for method
	// bodies -- these are already built into the runtime system.
	
	// IMPORTANT: The results of the following expressions are
	// stored in local variables.  You will want to do something
	// with those variables at the end of this method to make this
	// code meaningful.

	// 
	// The Object class has no parent class. Its methods are
	//        abort() : Object    aborts the program
	//        type_name() : Str   returns a string representation of class name
	//        copy() : SELF_TYPE  returns a copy of the object
	//
	// There is no need for method bodies in the basic classes---these
	// are already built in to the runtime system.

	Class_ Object_class =
	class_(Object, 
		   No_class,
		   append_Features(
				   append_Features(
						   single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
						   single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
				   single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
		   filename);

	// 
	// The IO class inherits from Object. Its methods are
	//        out_string(Str) : SELF_TYPE       writes a string to the output
	//        out_int(Int) : SELF_TYPE            "    an int    "  "     "
	//        in_string() : Str                 reads a string from the input
	//        in_int() : Int                      "   an int     "  "     "
	//
	Class_ IO_class = 
	class_(IO, 
		   Object,
		   append_Features(
				   append_Features(
						   append_Features(
								   single_Features(method(out_string, single_Formals(formal(arg, Str)),
											  SELF_TYPE, no_expr())),
								   single_Features(method(out_int, single_Formals(formal(arg, Int)),
											  SELF_TYPE, no_expr()))),
						   single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
				   single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
		   filename);  

	//
	// The Int class has no methods and only a single attribute, the
	// "val" for the integer. 
	//
	Class_ Int_class =
	class_(Int, 
		   Object,
		   single_Features(attr(val, prim_slot, no_expr())),
		   filename);

	//
	// Bool also has only the "val" slot.
	//
	Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);

	//
	// The class Str has a number of slots and operations:
	//       val                                  the length of the string
	//       str_field                            the string itself
	//       length() : Int                       returns length of the string
	//       concat(arg: Str) : Str               performs string concatenation
	//       substr(arg: Int, arg2: Int): Str     substring selection
	//       
	Class_ Str_class =
	class_(Str, 
		   Object,
		   append_Features(
				   append_Features(
						   append_Features(
								   append_Features(
										   single_Features(attr(val, Int, no_expr())),
										   single_Features(attr(str_field, prim_slot, no_expr()))),
								   single_Features(method(length, nil_Formals(), Int, no_expr()))),
						   single_Features(method(concat, 
									  single_Formals(formal(arg, Str)),
									  Str, 
									  no_expr()))),
				   single_Features(method(substr, 
							  append_Formals(single_Formals(formal(arg, Int)), 
									 single_Formals(formal(arg2, Int))),
							  Str, 
							  no_expr()))),
		   filename);

	// Add premitive classes.
	inher_map_.insert(std::pair<Symbol, Class_>(Object, Object_class));
	inher_map_.insert(std::pair<Symbol, Class_>(IO, IO_class));
	inher_map_.insert(std::pair<Symbol, Class_>(Int, Int_class));
	inher_map_.insert(std::pair<Symbol, Class_>(Bool, Bool_class));
	inher_map_.insert(std::pair<Symbol, Class_>(Str, Str_class));
}

void ClassTable::checkFeatureInheritance(){
	checked_ = initCheckMap(inher_map_);
	for (std::map<Symbol, Class_>::iterator iter = inher_map_.begin();
	  iter != inher_map_.end();
	  ++iter){
		if (checked_[iter->first]){
			continue;
		} else {
			collectFeatures(iter->second);
		}
	}
}

void ClassTable::collectFeatures(const Class_ c){

	// We always check parent first if it's not checked. 
	// We don't have worry about circular dependency,
	// as it has been taken care of.
	// We don't ask for Object's parent.
	if (c->getName() != Object && !checked_[c->getParent()]){
		collectFeatures(inher_map_.find(c->getParent())->second);
	}

	// Use of copy constructor, 
	// every method belongs to my parent(s) belongs to me.
	method_map_[c->getName()] 
		= std::map<Symbol, Method>(method_map_[c->getParent()]);
	attr_map_[c->getName()]
		= std::map<Symbol, Symbol>(attr_map_[c->getParent()]);

	// Add self as predefined.
	attr_map_[c->getName()].insert(std::pair<Symbol, Symbol>(self, SELF_TYPE));

	Features features = c->getFeatures();
	for (int i = 0; i < features->len(); i++){
		
	  Feature f = features->nth(i);

		// We don't have anything to do with attributes now.
		// Acturally, you can use:
		// 		(typeid(*f) != typeid(method_class))
		// but I think that would make the code too hard to read.
		// Also, I find out too late...
		if (f->isAttribute()){ 
			Attribute a  = (Attribute) f;
			if (hasKeyInMap(a->getName(), attr_map_[c->getParent()])){
				semant_error(c->get_filename(), f) 
					<< "Class " << c->getName()
					<< " has attribute: " << a->getName()
					<< " which has already been defined in it's parent class."
				;
			} else if (hasKeyInMap(a->getName(), attr_map_[c->getName()])) {
				semant_error(c->get_filename(), f) 
					<< "Class " << c->getName()
					<< " has attribute: " << a->getName()
					<< " which duplicats with the one defined in "
					<< c->get_filename() << ":" << a->get_line_number()
				;			
			} else {
				attr_map_[c->getName()]
					.insert(
						std::pair<Symbol, Symbol>(a->getName(), a->getType())
					);
			}
		} else {
			Method m = (Method) f;

			// In it's parent's method table, this is a inherited method.
			if (hasKeyInMap(m->getName(), method_map_[c->getParent()])) {
				Method parent_method = method_map_[c->getName()][f->getName()];
				Formals parent_formals = parent_method->getFormals();

				Formals m_formals = m->getFormals();

				bool legal_inhertance = true;
				// Different number of formals or different return type.
				if (m_formals->len() !=  parent_formals->len() 
				  || m->getType() != parent_method->getType()){
					legal_inhertance = false;

				// Or we have to check if all formals are the same.
				} else {
					for (int j = 0; j < m_formals->len(); j++){
						if (m_formals->nth(j)->getType() 
						  != parent_formals->nth(j)->getType()){
							legal_inhertance = false;		
							break;			
						}
					}
				}
				if (!legal_inhertance){
					semant_error(c->get_filename(), f) 
						<< "Method " << c->getName() << "." << m->getMethodSignature()
						<< " has different sinature with it's parents' method: "
						<< c->getParent() << "." << parent_method->getMethodSignature() 
						<< "\n"
					;
				}

			// In itself's method table, this is a duplicated method.
			} else if (hasKeyInMap(m->getName(), method_map_[c->getName()]))  {
				semant_error(c->get_filename(), f)
					<< "Method " << c->getName() << "." << m->getMethodSignature()
					<< " has been defined in line " 
					<< method_map_[c->getName()][f->getName()]->get_line_number()
					<< ". We will ignore this definition\n";

			// We got a new method!
			} else {
				method_map_[c->getName()]
					.insert(
						std::pair<Symbol, Method>(f->getName(), m)
					);
			}

		}
	}
	// Label this class as been checked.
	checked_[c->getName()] = true;
}

void ClassTable::checkEachClassType(){
	checked_ = initCheckMap(inher_map_);

	// Deliberately skip the following 5 classes.
	checked_[Object] = true;
	checked_[Int] = true;		checked_[IO] = true;
	checked_[Bool] = true;		checked_[Str] = true;
	for (std::map<Symbol, Class_>::iterator iter = inher_map_.begin();
	  iter != inher_map_.end();
	  ++iter){
		if (checked_[iter->first]){
			continue;
		} else {
			checkMethodsReturnType(iter->second);
		}
	}
}

void ClassTable::checkMethodsReturnType(const Class_ c){
	Symbol class_name = c->getName();
	
	// It's not necessary anymore whether father or son goes first.
	// But we still did it anyway.
	if (class_name != Object && !checked_[c->getParent()]){
		checkMethodsReturnType(inher_map_[c->getParent()]);
	}

	// Add every attribute of this class to symbol table.
	SymbolTable<Symbol, Symbol> tbl;
	tbl.enterscope();
	for (std::map<Symbol, Symbol>::iterator iter = attr_map_[class_name].begin();
	  iter != attr_map_[class_name].end();
	  ++iter){
		tbl.addid(iter->first, &(iter->second));
	}

	// Check for each method.
	for (std::map<Symbol, Method>::iterator iter = method_map_[class_name].begin();
	  iter != method_map_[class_name].end();
	  ++iter){
		Method m = iter->second;
		std::map<Symbol, Method>& parent_methods 
			= method_map_[inher_map_[c->getParent()]->getName()];

		// That this method is inherited from the parent,
		// furthermore, they are the same.
		// Then we skip this method as it has been checked previously.
		if (hasKeyInMap(iter->first, parent_methods)
		  && parent_methods[iter->first] == m) {
			continue;
		}

		// This method contains unknown type in it's signature.
		if (!isMethodSignTypeValid(c, m)){
			continue;
		}

		// Each expression will be assigned a type inside getExpressionType().
		tbl.enterscope();
		getExpressionType(c, m->getExpr(), tbl);
		tbl.exitscope();

		Symbol returned_type = m->getExpr()->get_type();

		// This should not happen.
		if (returned_type == NULL){	continue; }

		Symbol expr_cast = (returned_type == SELF_TYPE) ? c->getName() : returned_type;
		// No such returned type
		if (!hasKeyInMap(expr_cast, inher_map_) || 
		// returned type not le than defined type
		  ((m->getType() != SELF_TYPE && !le(expr_cast, m->getType())) ||
		// returned type is not SELF_TYPE when it should be.
		  (m->getType() == SELF_TYPE && returned_type != SELF_TYPE))){
			semant_error(c->get_filename(), m)
				<< "Method " << c->getName() << "." << m->getMethodSignature()
				<< " expected return type: " << m->getType() << ", "
				<< "got return type: " << returned_type << ".\n";
			;
		}
	}
	// Label this class as been checked.
	checked_[class_name] = true;
}

const bool ClassTable::isMethodSignTypeValid(const Class_ c, const Method m){
	bool is_sign_correct = true;
	std::stringstream undefined_type;
	// Method can be SELF_TYPE
	if (m->getType() != SELF_TYPE && !hasKeyInMap(m->getType(), inher_map_)){
		if (!is_sign_correct) {	undefined_type << ", ";	}
		undefined_type << m->getType();
		is_sign_correct = false;
	}
	Formals f = m->getFormals();
	for (int i=0; i<f->len(); i++){
		Symbol type_name = f->nth(i)->getType();
		if (!hasKeyInMap(type_name, inher_map_)){
			if (!is_sign_correct) {	undefined_type << ", ";	}
			undefined_type << m->getType();
			is_sign_correct = false;
		}
	}
	if (!is_sign_correct){
		semant_error(c->get_filename(), m)
			<< "Method " << c->getName() << "." << m->getMethodSignature()
			<< " got undefined type: " << undefined_type.str() << "\n";
	}
	return is_sign_correct;
}

/*
 * If type_infer <= type_defined, return true
 * otherwise, return false
 */
bool ClassTable::checkExpressionType(const Symbol type_defined_in,
									 const Symbol type_infer_in,
									 const SymbolTable<Symbol, Symbol>& scope_table,
									 const Symbol class_name) {
	Symbol type_defined = type_defined_in;
	Symbol type_infer = type_infer_in;

	// if both are SELF_TYPE
	if (type_defined == SELF_TYPE){
		return type_infer == SELF_TYPE;
	}

	// if one is SELF_TYPE
	if (type_infer == SELF_TYPE){
		type_infer = class_name;
	}

	// call le for type checking
	// type_infer <= type_defined
	return le(type_infer, type_defined);
}

void ClassTable::getExpressionType(
  const Class_ c, const Expression expr_in, SymbolTable<Symbol, Symbol>& scope_table){

	// If the input expression is NULL (expression does not exist)
	assert(expr_in != NULL);

	// early return
	if (expr_in->get_type() != NULL){ return ; }

	// infer type and perform type checking when (expr_in->get_type != NULL)
	// if expression type is no_expr, return NULL(no_expr)
	else if (typeid(*expr_in) == typeid(no_expr_class)){
		expr_in->set_type(No_type);
	}

	// deal with all consts
	// only infer type, no type checking is performed here
	// infer type for int_const
	else if (typeid(*expr_in) == typeid(int_const_class)){
		expr_in->set_type(Int);
	}
	else if (typeid(*expr_in) == typeid(bool_const_class)){
		expr_in->set_type(Bool);
	}
	else if (typeid(*expr_in) == typeid(string_const_class)){
		expr_in->set_type(Str);
	}
	else if (typeid(*expr_in) == typeid(object_class)){
		object_class * expr_obj = (object_class *) expr_in;
		Symbol * object_type_ptr = scope_table.lookup(expr_obj->get_name());
		if (object_type_ptr == NULL){
		    // if object not defined before
            semant_error(c->get_filename(),expr_in) << "Undeclared identifier "<<expr_obj->get_name()
                                                    <<"." << std::endl;
		    expr_in->set_type(Object);
		}
		else {
            expr_in->set_type(*object_type_ptr);
        }
	}

	// isvoid
	else if (typeid(*expr_in) == typeid(isvoid_class)){
		isvoid_class * expr_isvoid = (isvoid_class *) expr_in;
		getExpressionType(c,expr_isvoid->get_expr(),scope_table);
		expr_in->set_type(Bool);
	}


	// new_class
	else if (typeid(*expr_in) == typeid(new__class)){
		new__class * expr_new = (new__class *) expr_in;
		Symbol type_name = expr_new->get_type_name();

		if (type_name != SELF_TYPE && !hasKeyInMap(type_name,inher_map_)){
			semant_error(c->get_filename(),expr_in) 
				<< "'new' used with undefined class "
				<< type_name << ".\n" 
			;
		}
		expr_new->set_type(type_name);
	}

	// check expression type and infer type
	// assign class
	else if (typeid(*expr_in) == typeid(assign_class)){
		assign_class * expr_assign = (assign_class *) expr_in;
		// get the type from the sub-expression
		// std::cout << "ASSIGN: get expr: " ;
		// expr_assign->get_expr()->dump(std::cout,0);
		getExpressionType(c, expr_assign->get_expr(),scope_table);
		Symbol type_infer = expr_assign->get_expr()->get_type();

		// type checking
		// std::cout << "type_inferred: " << type_infer << std::endl;
		Symbol type_defined = *scope_table.lookup(expr_assign->get_name());

		// std::cout << "type_defined: " << type_defined  << std::endl;
		if (!checkExpressionType(type_defined,type_infer,scope_table,c->getName())){
			semant_type_error(c,expr_in,type_infer,type_defined,expr_assign->get_name());
		}

		expr_assign->set_type(type_infer);
	}

	// condition
	else if (typeid(*expr_in) == typeid(cond_class)){
		cond_class * expr_cond = (cond_class *) expr_in;
		getExpressionType(c,expr_cond->get_pred(),scope_table);
		if (expr_cond->get_pred()->get_type() != Bool){
			// TODO
			semant_error(c) << "Cond::pred is invalid" << std::endl;
		}

		getExpressionType(c,expr_cond->get_then_exp(),scope_table);
		getExpressionType(c,expr_cond->get_else_exp(), scope_table);
		Symbol type_then = expr_cond->get_then_exp()->get_type();
		Symbol type_else = expr_cond->get_else_exp()->get_type();

		Symbol type_lup = getSharedParent(type_then,type_else);
		expr_cond->set_type(type_lup);
	}

	// loop
	else if (typeid(*expr_in) == typeid(loop_class)){
		loop_class * expr_loop = (loop_class *) expr_in;

		getExpressionType(c,expr_loop->get_pred(),scope_table);
		getExpressionType(c,expr_loop->get_body(),scope_table);

		Symbol type_pred = expr_loop->get_pred()->get_type();
		Symbol type_body = expr_loop->get_pred()->get_type();


		if ( type_pred != Bool){
			// TODO
			semant_error(c) << "loop::pred is invalid" << std::endl;
		}

		expr_in->set_type(Object);
	}

	// block
	else if (typeid(*expr_in) == typeid(block_class)){
		block_class * expr_block = (block_class *) expr_in;
		Expressions expr_body = expr_block->get_body();
		// get the Symbol of last body as the Symbol for this block
		// perform type checking along the way
		Symbol last_symbol;
		for (int i = 0; i < expr_body->len(); i++){
			Expression expr_tmp = expr_body->nth(i);
			getExpressionType(c, expr_tmp, scope_table);
			last_symbol = expr_tmp->get_type();
		}
		expr_block->set_type(last_symbol);
	}
	// not
	else if (typeid(*expr_in) == typeid(comp_class)){
		comp_class * expr_comp = (comp_class *) expr_in;
		getExpressionType(c,expr_comp->get_expr(),scope_table);
		if (expr_comp->get_expr()->get_type() != Bool){
			// TODO
			semant_error(c) << "comp:expr is invalid" << std::endl;
		}
		expr_comp->set_type(Bool);

	// =
	} else if (typeid(*expr_in) == typeid(eq_class)){
		assignCompareType<eq_class*>(c, expr_in, scope_table);

	// <
	} else if (typeid(*expr_in) == typeid(lt_class)){
		assignCompareType<lt_class*>(c, expr_in, scope_table);

	// <=
	} else if (typeid(*expr_in) == typeid(leq_class)){
		assignCompareType<leq_class*>(c, expr_in, scope_table);

	// neg
	} else if (typeid(*expr_in) == typeid(neg_class)){
		neg_class * expr_neg = (neg_class *) expr_in;

		getExpressionType(c,expr_neg->get_expr(),scope_table);
		Symbol type_1 = expr_neg->get_expr()->get_type();

		if (type_1 != Int){
			semant_error(c->get_filename(),expr_in) << "Argument of '~' has type "<< type_1
													<<" instead of Int." << std::endl;
		}
		expr_neg->set_type(type_1);

	// +
	} else if (typeid(*expr_in) == typeid(plus_class)) {
		assignArithmeticType<plus_class*>(c, expr_in, scope_table);

	// -
	} else if (typeid(*expr_in) == typeid(sub_class)) {
		assignArithmeticType<sub_class*>(c, expr_in, scope_table);

	// *
	} else if (typeid(*expr_in) == typeid(mul_class)) {
		assignArithmeticType<mul_class*>(c, expr_in, scope_table);

	// /
	} else if (typeid(*expr_in) == typeid(divide_class)) {
		assignArithmeticType<divide_class*>(c, expr_in, scope_table);
	
	// static dispatch
	} else if (typeid(*expr_in) == typeid(static_dispatch_class) ) {
		assignDispatchType<static_dispatch_class*>(c, expr_in, scope_table);

	// dynamic dispatch
	} else if (typeid(*expr_in) == typeid(dispatch_class) /* dynamic dispathc */) {
		assignDispatchType<dispatch_class*>(c, expr_in, scope_table);

	} else if (typeid(*expr_in) == typeid(let_class) /* let */) {
		let_class * expr_let = (let_class *) expr_in;

		scope_table.enterscope();

		Symbol id = expr_let->get_id();
	    Symbol type_defined = expr_let->get_type_decl();
	    if (type_defined != SELF_TYPE && !hasKeyInMap(type_defined,inher_map_) ){
	    	//TODO: type not defined (class not decleared)
	    }

	    // deal with init (assign and creat new value)
	    Expression init = expr_let->get_init();
	    getExpressionType(c,init,scope_table);
	    if (init->get_type() != No_type){
            checkExpressionType(type_defined,init->get_type(),scope_table,c->getName());
	    }


        scope_table.addid(id,&type_defined);

	    std::cerr << "Here" << std::endl;

	    // deal with expr
		Expression expr_body = expr_let->get_body();
		getExpressionType(c,expr_body,scope_table);
		checkExpressionType(type_defined,expr_body->get_type(),scope_table,c->getName());
		expr_let->set_type(expr_body->get_type());

        // std::cerr << "Here2" << std::endl;

	    scope_table.exitscope();


	}
	// case (branch)
	else if (typeid(*expr_in) == typeid(typcase_class)){
		typcase_class * expr_typecase = (typcase_class *) expr_in;
		getExpressionType(c,expr_typecase->get_expr(),scope_table);

		// get the cases (deal with all branches)
        // TODO: what if no branch
        Cases expr_cases = expr_typecase->get_cases();
        Symbol common_parent;
        for (int i = 0; i < expr_cases->len(); i++){
            Case_class * case_ptr = expr_cases->nth(i);
            branch_class * branch_ptr = (branch_class * ) case_ptr;

            scope_table.enterscope();

            Symbol id = branch_ptr->get_name();

            Symbol type_defined = branch_ptr->get_type_decl();
            if ((type_defined != SELF_TYPE) && (!hasKeyInMap(type_defined,inher_map_))){
                // TODO: error for type_defined not exist
            }

            Expression branch_body = branch_ptr->get_expr();
            getExpressionType(c,branch_body,scope_table);

            // find the common parent of all exprs
            if (i == 0)     common_parent = branch_body->get_type();
            else            common_parent = getSharedParent(common_parent,branch_body->get_type());

            scope_table.exitscope();
        }
        expr_typecase->set_type(common_parent);
	}
	else {
		// raise error if still no match
		throw 6;
	}
}

template <class Dispatch>
void ClassTable::assignDispatchType(
  const Class_ c, const Expression e, SymbolTable<Symbol, Symbol>& tbl){
  	
  	Dispatch d = dynamic_cast<Dispatch>(e);


  	/* Evaluate each and every expression. */
	getExpressionType(c, d->getExpr(), tbl);
	Expressions expr_list = d->getExprList();
	for (int i=0; i<expr_list->len(); i++){
		getExpressionType(c, expr_list->nth(i), tbl);
	} 

	/* Determine dispatch type. */
	Symbol dispatch_type;
	// Static dispatch.
	if (typeid(Dispatch) == typeid(static_dispatch_class*)){
		dispatch_type = d->getDispatchType();
	// Dynamic dispatch with SELF_TYPE, using current class's type. 
	} else if (d->getExpr()->get_type() == SELF_TYPE) {
			dispatch_type = c->getName();
	// Dynamic dispatch using expr's type.
	} else {
		dispatch_type = d->getExpr()->get_type();
	}

	/* Check if the type exists and if the method exists. */
	if (!hasKeyInMap(dispatch_type, inher_map_) || 
		!hasKeyInMap(d->getName(), method_map_[dispatch_type])){
		semant_error(c->get_filename(), e)
			<< "Dispatch to undefined class/method " 
			<< dispatch_type << "." << d->getName() << "\n";
		;
		e->set_type(Object);
		return;
	} else if (!le(d->getExpr()->get_type(), dispatch_type)){
		semant_error(c->get_filename(), e)
			<< "Expression type " << d->getExpr()->get_type()
			<< " does not conform to declared static dispatch type " << dispatch_type << ".\n"
		;
		e->set_type(Object);
		return;
	}
	/* Check parameters. */
	Method m = method_map_[dispatch_type][d->getName()];
	Formals formal_list = m->getFormals();
	// Length should be the same.
	if (formal_list->len() != expr_list->len()){
		semant_error(c->get_filename(), e)
			<< "Method m called with wrong number of arguments."
			<< "Expected " << formal_list->len() << " parameters, "
			<< "got " << expr_list->len() << "\n";
		;
	}
	// And each parameter should have a type le method's definition.
	int len = std::min(formal_list->len(), expr_list->len());
	for (int i=0; i<len; i++){
		Symbol expr_type = expr_list->nth(i)->get_type();
		Symbol form_type = formal_list->nth(i)->getType();
		// Can't have SELF_TYPE as parameter type.
		if (expr_type == SELF_TYPE || !le(expr_type, form_type)){
			semant_error(c->get_filename(), e)
				<< "In call of method " << c->getName() << "." << m->getMethodSignature()
				<< " parameter " << i << " expected type " << form_type
				<< ", got type " << expr_type << "\n";
			;
		}
	}

	/* Determine return type. */
	Symbol returned_type = m->getType();
	if (returned_type == SELF_TYPE){
		returned_type = d->getExpr()->get_type();
	}
	e->set_type(returned_type);
}

template <class Compare>
void ClassTable::assignCompareType(
  const Class_ c, const Expression e, SymbolTable<Symbol, Symbol>& tbl){
	Compare a = dynamic_cast<Compare>(e);
	getExpressionType(c, a->getExprOne(), tbl);
	getExpressionType(c, a->getExprTwo(), tbl);

	Symbol type_1 = a->getExprOne()->get_type();
	Symbol type_2 = a->getExprTwo()->get_type();

	bool is_legal;
	// For eq, they either have same static type, or don't have it at all
	if (typeid(Compare) == typeid(eq_class*)){
		is_legal =  
		// Have same static type
		  ((type_2 == type_1) && (type_1 == Int || type_1 == Bool || type_1 == Str)) ||
		// Or can be freely compared, but non of them can be of Bool, Int or Str
		  (type_1 != Int && type_1 != Bool && type_1 != Str && type_2 != Int && type_2 != Bool && type_2 != Str);

	// For leq and le, they all need to be Int.
	} else {
		is_legal = ((type_1 == Int) && (type_2 == Int));
	}

	if (!is_legal) {
		if (typeid(Compare) == typeid(eq_class*)){
			semant_error(c->get_filename(), e)
				<< "Illegal comparison with a basic type.\n";
			;
		} else {
			semant_error(c->get_filename(), e)
				<< "non-Int arguments: " 
				<< type_1 << " " << a->getOperator() 
				<< " " << type_2 << "\n";
			;
		}
	}
	e->set_type(Bool);
}

template <class Arithmetic>
void ClassTable::assignArithmeticType(
  const Class_ c, const Expression e, SymbolTable<Symbol, Symbol>& tbl){
	Arithmetic a = dynamic_cast<Arithmetic>(e);

	getExpressionType(c, a->getExprOne(), tbl);
	getExpressionType(c, a->getExprTwo(), tbl);

	Symbol type_1 = a->getExprOne()->get_type();
	Symbol type_2 = a->getExprTwo()->get_type();

	if (type_1 != Int || type_2 != Int){
		semant_error(c->get_filename(), e)
			<< "non-Int arguments: " 
			<< type_1 << " " << a->getOperator() 
			<< " " << type_2 << "\n";
		;
	}
	e->set_type(Int);
}

const bool ClassTable::le(Symbol a, const Symbol b) const {
	// One of the class is missing.
	if (!hasKeyInMap(a, inher_map_) || !hasKeyInMap(b, inher_map_)){
		return false;
	}

	// Everyone is le Object.
	if (b == Object) { return true; }
	// Find a's parent until it gets to b.
	// Or a ends up in Object.
	while (a != b && a != Object){
		a = inher_map_.find(a)->second->getParent();
	}
	return a == b;
}

const Symbol ClassTable::getSharedParent(const Symbol a, const Symbol b) const {
	std::deque<Symbol> a_path = getInherVec(a);
	std::deque<Symbol> b_path = getInherVec(b);

	unsigned int i = 0;
	while (
	  i+1 < a_path.size()-1 && 
	  i+1 < b_path.size()-1 && 
	  a_path[i+1] == b_path[i+1]){
		i++;
	}
	return a_path[i];
}

const std::deque<Symbol> ClassTable::getInherVec(Symbol curr) const {
	std::deque<Symbol> inher_vec;
	inher_vec.push_front(curr);
	while (curr != Object){
		curr = inher_map_.find(curr)->second->getParent();
		inher_vec.push_front(curr);
	}
	return inher_vec;
}

// a helper function for type checking semant error
void ClassTable::semant_type_error(Class_  c, tree_node *expr_in , Symbol type_infer, Symbol type_defined,
									   Symbol id_name) {
	semant_error(c->get_filename(),expr_in) << "Type " << type_infer
											<< " of assigned expression does not conform to declared type "
			  << type_defined <<" of identifier "<<id_name<<"." << std::endl;
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
	return semant_error(c->get_filename(),c);
}    

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
	error_stream << filename << ":" << t->get_line_number() << ": ";
	return semant_error();
}

ostream& ClassTable::semant_error()                  
{                                                 
	semant_errors++;                            
	return error_stream;
} 



/*   This is the entry point to the semantic checker.

	 Your checker should do the following two things:

	 1) Check that the program is semantically correct
	 2) Decorate the abstract syntax tree with type information
		by setting the `type' field in each Expression node.
		(see `tree.h')

	 You are free to first do 1), make sure you catch all semantic
	 errors. Part 2) can be done in a second stage, when you want
	 to build mycoolc.
 */
# define IF_ERROR_THEN_DELETE_TABLE_AND_EXIT \
	if (classtable->errors()) { \
		cerr << "Compilation halted due to static semantic errors." << endl; \
		delete classtable; \
		exit(1); \
	}

void program_class::semant() {
	initialize_constants();

	/* ClassTable constructor may do some semantic analysis */
	ClassTable *classtable = new ClassTable(classes);
	// Error encountered when constructing class table. 
	// Inhertance relation is not clear. Abort.
	IF_ERROR_THEN_DELETE_TABLE_AND_EXIT

	/* Check for method inhertance correctness. */
	classtable->checkFeatureInheritance();
	IF_ERROR_THEN_DELETE_TABLE_AND_EXIT

	classtable->checkEachClassType();
	IF_ERROR_THEN_DELETE_TABLE_AND_EXIT

	// Memory SAFETY!!
	delete classtable;
}

