

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"

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
    for (int i = classes->first(); classes->more(i); i = classes->next(i)){

        Class_ curr = classes->nth(i);
        
        // You can't define SELF_TYPE as a class.
        if (curr->getName() == SELF_TYPE){
            semant_error(curr) << "Redefinition of basic class SELF_TYPE.\n";

        // You can't define classes that have been declared too.
        } else if (hasKeyInMap(curr->getName(), inher_map_)){
            semant_error(curr) 
            	<< "Redefinition of class " << curr->getName()->get_string()
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
                	<< "Class " << curr->getName()->get_string() << " has undefined parent " 
                	<< "class " << parent_name->get_string() << " for inhertance.\n";
                break;

            // and is not one of the following...
            } else if (parent_name == SELF_TYPE || parent_name == Int 
                    || parent_name == Bool || parent_name == Str) {
                semant_error(curr) << "Class" << curr->getName()->get_string() 
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
            	error_stream << dep[i]->getName()->get_string() << " -> ";
            	if (i == dep.size() -1){
            		error_stream << iter->second->getName()->get_string() << ".\n";
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

void ClassTable::collectMethods(Class_ c){

	// We always check parent first if it's not checked. 
	// We don't have worry about circular dependency,
	// as it has been taken care of.
	if (!checked_[c->getParent()]){
		collectMethods(inher_map_.find(c->getParent())->second);
	}

	// Use of copy constructor, 
	// every method belongs to my parent(s) belongs to me.
	method_map_[c->getName()] 
		= std::map<Symbol, method_class>(method_map_[c->getParent()]);

	Features features = c->getFeatures();
	for (int i = features->first(); features->more(i); features->next(i)){
		
		Feature f = features->nth(i);

		// We don't have anything to do with attributes now.
		if (f->isAttribute()){ 
			continue;
		}

		

	}
	// Label this class as been checked.
	checked_[c->getName()] = true;
}

void ClassTable::checkMethodInheritance(){
	checked_ = initCheckMap(inher_map_);
	for (std::map<Symbol, Class_>::iterator iter = inher_map_.begin();
	  iter != inher_map_.end();
	  ++iter){
		if (checked_[iter->first]){
			continue;
		} else {
			collectMethods(iter->second);
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

    //classtable->checkMethodInheritance();

    /* some semantic analysis code may go here */

    // Memory SAFETY!!
    delete classtable;
}
