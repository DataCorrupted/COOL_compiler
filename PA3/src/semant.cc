

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"

#include <typeinfo>
#include <iostream>

#include <string>
#include <vector>
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
std::map<K, bool> ClassTable::initCheckMap(std::map<K, V>& tmpl_map){
    std::map<K, bool> checked;
    for(typename std::map<K, V>::iterator iter = tmpl_map.begin(); 
      iter != tmpl_map.end(); 
      ++iter){
        checked.insert(std::pair<K, bool>(iter->first, false));
    }	
    return checked;
}

template <class K, class V>
bool ClassTable::hasKeyInMap(K key, std::map<K, V> map_){
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

std::string getMethodSignature(Method m){
	std::string signature;
	// Add method name.
	signature.append(m->getName()->get_string());
	
	// Add formals.
	signature.append("(");
	Formals formals = m->getFormals();
	for (int i=0; i<formals->len(); i++){
		signature.append(formals->nth(i)->getType()->get_string());
		if (i != formals->len() - 1){
			signature.append(", ");
		}
	}
	signature.append("): ");

	// Add return type.
	signature.append(m->getType()->get_string());
	signature.append(";");
	return signature;
}
void ClassTable::collectFeatures(Class_ c){

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
				semant_error(c->getFilename(), f) 
					<< "Class " << c->getName()
					<< " has attribute: " << a->getName()
					<< " which has already been defined in it's parent class."
				;
			} else if (hasKeyInMap(a->getName(), attr_map_[c->getName()])) {
				semant_error(c->getFilename(), f) 
					<< "Class " << c->getName()
					<< " has attribute: " << a->getName()
					<< " which duplicats with the one defined in "
					<< c->getFilename() << ":" << a->get_line_number()
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
						if (m_formals->nth(j)->getType() != parent_formals->nth(j)->getType()){
							legal_inhertance = false;		
							break;			
						}
					}
				}
				if (!legal_inhertance){
					semant_error(c->getFilename(), f) 
						<< "Class " << c->getName()
						<< " has method: " << getMethodSignature(m)
						<< "\n\twhich disagrees with it's parents' method: "
						<< getMethodSignature(parent_method) << "\n"
					;
				}

			// In itself's method table, this is a duplicated method.
			} else if (hasKeyInMap(m->getName(), method_map_[c->getName()]))  {
				semant_error(c->getFilename(), f)
					<< "Class " << c->getName()
					<< " has duplicated methods. We will ignore the later definition, " 
					<< "which has the signature: " << 	getMethodSignature(m) << "\n";

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


void ClassTable::checkMethodsType(Class_ c){
	// It's not necessary anymore whether father or son goes first.
	// But we still did it anyway.
	if (c->getName() != Object && !checked_[c->getName()]){
		checkMethodsType(inher_map_.find(c->getParent())->second);
	}

	Symbol class_name = c->getName();
	// Add every attribute of this class to symbol table.
	SymbolTable<Symbol, Symbol> tbl;
	tbl.enterscope();
	for (std::map<Symbol, Symbol>::iterator iter = attr_map_[class_name].begin();
	  iter != attr_map_[class_name].end();
	  ++iter){
		tbl.addid(iter->first, &(iter->second));
	}
/*
	// Check for each method.
	for (std::map<Symbol, Method>::iterator iter = method_map_[class_name].begin();
	  iter != method_map_[class_name].end();
	  ++iter){
	  	Method m = iter->second;

	  	// Each expression will be assigned a type inside getExpressionType().
		Symbol returned_type = getExpressionType(m->getExpr(), tbl);

		// Check failed.
		if (returned_type != no_expr && returned_type != m->getType()){
			
			semant_error(c->get_filename(), m)
				<< "Method " << c->getName() << "." << m->getName()
				<< "expected return type: " << m->getType() << ", "
				<< "got return type: " << returned_type << "."
			;
		}
	}*/
}
void ClassTable::checkEachClassType(){
	checked_ = initCheckMap(inher_map_);
	for (std::map<Symbol, Class_>::iterator iter = inher_map_.begin();
	  iter != inher_map_.end();
	  ++iter){
		if (checked_[iter->first]){
			continue;
		} else {
			checkMethodsType(iter->second);
		}
	}
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
void program_class::semant()
{
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    ClassTable *classtable = new ClassTable(classes);
    // Error encountered when constructing class table. 
    // Inhertance relation is not clear. Abort.
    if (classtable->errors()) {
        cerr << "Compilation halted due to static semantic errors." << endl;
        delete classtable;
        exit(1);
    }

    /* Check for method inhertance correctness. */
    classtable->checkFeatureInheritance();
    if (classtable->errors()) {
        cerr << "Compilation halted due to static semantic errors." << endl;
        delete classtable;
        exit(1);
    }

    classtable->checkEachClassType();

    // Memory SAFETY!!
    delete classtable;
}
