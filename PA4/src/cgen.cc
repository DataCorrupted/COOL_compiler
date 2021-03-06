
//**************************************************************
//
// Code generator SKELETON
//
// Read the comments carefully. Make sure to
//    initialize the base class tags in
//       `CgenClassTable::CgenClassTable'
//
//    Add the label for the dispatch tables to
//       `IntEntry::code_def'
//       `StringEntry::code_def'
//       `BoolConst::code_def'
//
//    Add code to emit everyting else that is needed
//       in `CgenClassTable::code'
//
//
// The files as provided will produce code to begin the code
// segments, declare globals, and emit constants.  You must
// fill in the rest.
//
//**************************************************************

#include "cgen.h"
#include "cgen_gc.h"
#include <stack>
#include <typeinfo>
#include <map>
extern void emit_string_constant(ostream& str, char *s);
extern int cgen_debug;


//
// Three symbols from the semantic analyzer (semant.cc) are used.
// If e : No_type, then no code is generated for e.
// Special code is generated for new SELF_TYPE.
// The name "self" also generates code different from other references.
//
//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
Symbol 
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

static char *gc_init_names[] =
	{ "_NoGC_Init", "_GenGC_Init", "_ScnGC_Init" };
static char *gc_collect_names[] =
	{ "_NoGC_Collect", "_GenGC_Collect", "_ScnGC_Collect" };


//  BoolConst is a class that implements code generation for operations
//  on the two booleans, which are given global names here.
BoolConst falsebool(FALSE);
BoolConst truebool(TRUE);

//*********************************************************
//
// Define global constants used in C++
//
//*********************************************************
int local_var_cnt = 1;
SymbolTable<Symbol, ObjectLocation> env;
CgenNode* cur_class;
std::map<Symbol, CgenNodeP> class_map;

// Get inheritance link.
const std::deque<Symbol> getInherDQ(Symbol curr) {
	std::deque<Symbol> inher_vec;
	inher_vec.push_front(curr);
	while (curr != Object){
		curr = class_map[curr]->get_parentnd()->getName();
		inher_vec.push_front(curr);
	}
	return inher_vec;
}
// Find shared parent for two class.
const Symbol getSharedParent(const Symbol a, const Symbol b) {
	std::deque<Symbol> a_path = getInherDQ(a);
	std::deque<Symbol> b_path = getInherDQ(b);

	unsigned int i = 0;
	while (
	  i+1 < a_path.size() && 
	  i+1 < b_path.size() && 
	  a_path[i+1] == b_path[i+1]){
		i++;
	}
	return a_path[i];
}
// Find shared parent for a vector of classes.
const Symbol getSharedParent(const std::vector<Symbol> vec){
	Symbol sum = vec[0];
	for (int i=1; i<vec.size(); i++){
		sum = getSharedParent(sum, vec[i]);
		// Reached the end. No need for further purching.
		if (sum == Object){
			return sum;
		}
	}
	return sum;
}


//*********************************************************
//
// Define method for code generation
//
// This is the method called by the compiler driver
// `cgtest.cc'. cgen takes an `ostream' to which the assembly will be
// emmitted, and it passes this and the class list of the
// code generator tree to the constructor for `CgenClassTable'.
// That constructor performs all of the work of the code
// generator.
//
//*********************************************************

void program_class::cgen(ostream &os) {
	initialize_constants();
	CgenClassTable *codegen_classtable = new CgenClassTable(classes,os);
}


//////////////////////////////////////////////////////////////////////////////
//
//  emit_* procedures
//
//  emit_X  writes code for operation "X" to the output stream.
//  There is an emit_X for each opcode X, as well as emit_ functions
//  for generating names according to the naming conventions (see emit.h)
//  and calls to support functions defined in the trap handler.
//
//  Register names and addresses are passed as strings.  See `emit.h'
//  for symbolic names you can use to refer to the strings.
//
//////////////////////////////////////////////////////////////////////////////

static void emit_load(char *dest_reg, int offset, char *source_reg, ostream& s)
{
	s << LW << dest_reg << " " << offset * WORD_SIZE << "(" << source_reg << ")" 
		<< endl;
}

static void emit_store(char *source_reg, int offset, char *dest_reg, ostream& s)
{
	s << SW << source_reg << " " << offset * WORD_SIZE << "(" << dest_reg << ")"
			<< endl;
}

static void emit_load_imm(char *dest_reg, int val, ostream& s)
{ s << LI << dest_reg << " " << val << endl; }

static void emit_load_address(char *dest_reg, char *address, ostream& s)
{ s << LA << dest_reg << " " << address << endl; }

static void emit_partial_load_address(char *dest_reg, ostream& s)
{ s << LA << dest_reg << " "; }

static void emit_load_bool(char *dest, const BoolConst& b, ostream& s)
{
	emit_partial_load_address(dest,s);
	b.code_ref(s);
	s << endl;
}

static void emit_load_string(char *dest, StringEntry *str, ostream& s)
{
	emit_partial_load_address(dest,s);
	str->code_ref(s);
	s << endl;
}

static void emit_load_int(char *dest, IntEntry *i, ostream& s)
{
	emit_partial_load_address(dest,s);
	i->code_ref(s);
	s << endl;
}

static void emit_move(char *dest_reg, char *source_reg, ostream& s)
{ s << MOVE << dest_reg << " " << source_reg << endl; }

static void emit_neg(char *dest, char *src1, ostream& s)
{ s << NEG << dest << " " << src1 << endl; }

static void emit_add(char *dest, char *src1, char *src2, ostream& s)
{ s << ADD << dest << " " << src1 << " " << src2 << endl; }

static void emit_addu(char *dest, char *src1, char *src2, ostream& s)
{ s << ADDU << dest << " " << src1 << " " << src2 << endl; }

static void emit_addiu(char *dest, char *src1, int imm, ostream& s)
{ s << ADDIU << dest << " " << src1 << " " << imm << endl; }

static void emit_div(char *dest, char *src1, char *src2, ostream& s)
{ s << DIV << dest << " " << src1 << " " << src2 << endl; }

static void emit_mul(char *dest, char *src1, char *src2, ostream& s)
{ s << MUL << dest << " " << src1 << " " << src2 << endl; }

static void emit_sub(char *dest, char *src1, char *src2, ostream& s)
{ s << SUB << dest << " " << src1 << " " << src2 << endl; }

static void emit_sll(char *dest, char *src1, int num, ostream& s)
{ s << SLL << dest << " " << src1 << " " << num << endl; }

static void emit_jalr(char *dest, ostream& s)
{ s << JALR << "\t" << dest << endl; }

static void emit_jal(char *address,ostream &s)
{ s << JAL << address << endl; }

static void emit_return(ostream& s)
{ s << RET << endl; }

static void emit_gc_assign(ostream& s)
{ s << JAL << "_GenGC_Assign" << endl; }

static void emit_disptable_ref(Symbol sym, ostream& s)
{  s << sym << DISPTAB_SUFFIX; }

static void emit_init_ref(Symbol sym, ostream& s)
{ s << sym << CLASSINIT_SUFFIX; }

static void emit_label_ref(int l, ostream &s)
{ s << "label" << l; }

static void emit_protobj_ref(Symbol sym, ostream& s)
{ s << sym << PROTOBJ_SUFFIX; }

static void emit_method_ref(Symbol classname, Symbol methodname, ostream& s)
{ s << classname << METHOD_SEP << methodname; }

static void emit_label_def(int l, ostream &s)
{
	emit_label_ref(l,s);
	s << ":" << endl;
}

static void emit_beqz(char *source, int label, ostream &s)
{
	s << BEQZ << source << " ";
	emit_label_ref(label,s);
	s << endl;
}

static void emit_beq(char *src1, char *src2, int label, ostream &s)
{
	s << BEQ << src1 << " " << src2 << " ";
	emit_label_ref(label,s);
	s << endl;
}

static void emit_bne(char *src1, char *src2, int label, ostream &s)
{
	s << BNE << src1 << " " << src2 << " ";
	emit_label_ref(label,s);
	s << endl;
}

static void emit_bleq(char *src1, char *src2, int label, ostream &s)
{
	s << BLEQ << src1 << " " << src2 << " ";
	emit_label_ref(label,s);
	s << endl;
}

static void emit_blt(char *src1, char *src2, int label, ostream &s)
{
	s << BLT << src1 << " " << src2 << " ";
	emit_label_ref(label,s);
	s << endl;
}

static void emit_blti(char *src1, int imm, int label, ostream &s)
{
	s << BLT << src1 << " " << imm << " ";
	emit_label_ref(label,s);
	s << endl;
}

static void emit_bgti(char *src1, int imm, int label, ostream &s)
{
	s << BGT << src1 << " " << imm << " ";
	emit_label_ref(label,s);
	s << endl;
}

static void emit_branch(int l, ostream& s)
{
	s << BRANCH;
	emit_label_ref(l,s);
	s << endl;
}

static void emit_copy(ostream& s){
	emit_jal("Object.copy", s);
}

//
// Push a register on the stack. The stack grows towards smaller addresses.
//
static void emit_push(char *reg, ostream& str) {
	emit_store(reg,0,SP,str);
	emit_addiu(SP,SP,-4,str);
}
static void emit_pop(ostream& str){
	emit_addiu(SP, SP, 4, str);
}
static void emit_pop(char* reg, ostream& str){
	emit_load(reg, 1, SP, str);
	emit_addiu(SP, SP, 4, str);
}
//
// Fetch the integer value in an Int object.
// Emits code to fetch the integer value of the Integer object pointed
// to by register source into the register dest
//
static void emit_fetch_int(char *dest, char *source, ostream& s)
{ emit_load(dest, DEFAULT_OBJFIELDS, source, s); }

//
// Emits code to store the integer value contained in register source
// into the Integer object pointed to by dest.
//
static void emit_store_int(char *source, char *dest, ostream& s)
{ emit_store(source, DEFAULT_OBJFIELDS, dest, s); }


static void emit_test_collector(ostream &s)
{
	emit_push(ACC, s);
	emit_move(ACC, SP, s); // stack end
	emit_move(A1, ZERO, s); // allocate nothing
	s << JAL << gc_collect_names[cgen_Memmgr] << endl;
	emit_addiu(SP,SP,4,s);
	emit_load(ACC,0,SP,s);
}

static void emit_gc_check(char *source, ostream &s)
{
	if (source != (char*)A1) emit_move(A1, source, s);
	s << JAL << "_gc_check" << endl;
}

static void emitCalleeStart(ostream& str) {
	emit_addiu(SP, SP, -3 * WORD_SIZE, str);
	emit_store(FP, 3, SP, str);
	emit_store(SELF, 2, SP, str);
	emit_store(RA, 1, SP, str);
	emit_addiu(FP, SP, WORD_SIZE, str);
	emit_move(SELF, ACC, str);
}
static void emitCalleeReturn(const int arg_cnt, ostream& str) {
	emit_load(FP, 3, SP, str);
	emit_load(SELF, 2, SP, str);
	emit_load(RA, 1, SP, str);
	emit_addiu(SP, SP, (3 + arg_cnt) * WORD_SIZE, str);
	emit_return(str);
}


///////////////////////////////////////////////////////////////////////////////
//
// coding strings, ints, and booleans
//
// Cool has three kinds of constants: strings, ints, and booleans.
// This section defines code generation for each type.
//
// All string constants are listed in the global "stringtable" and have
// type StringEntry.  StringEntry methods are defined both for String
// constant definitions and references.
//
// All integer constants are listed in the global "inttable" and have
// type IntEntry.  IntEntry methods are defined for Int
// constant definitions and references.
//
// Since there are only two Bool values, there is no need for a table.
// The two booleans are represented by instances of the class BoolConst,
// which defines the definition and reference methods for Bools.
//
///////////////////////////////////////////////////////////////////////////////

//
// Strings
//
void StringEntry::code_ref(ostream& s)
{
	s << STRCONST_PREFIX << index;
}

//
// Emit code for a constant String.
// You should fill in the code naming the dispatch table.
//

void StringEntry::code_def(ostream& s, int stringclasstag)
{
	IntEntryP lensym = inttable.add_int(len);

	// Add -1 eye catcher
	s << WORD << "-1" << endl;

	code_ref(s);  s  << LABEL                                             // label
			<< WORD << stringclasstag << endl                                 // tag
			<< WORD << (DEFAULT_OBJFIELDS + STRING_SLOTS + (len+4)/4) << endl // size
			<< WORD;

			emit_disptable_ref(Str, s);

			s << endl;                                              // dispatch table
			s << WORD;  lensym->code_ref(s);  s << endl;            // string length
	emit_string_constant(s,str);                                // ascii string
	s << ALIGN;                                                 // align to word
}

//
// StrTable::code_string
// Generate a string object definition for every string constant in the 
// stringtable.
//
void StrTable::code_string_table(ostream& s, int stringclasstag)
{  
	for (List<StringEntry> *l = tbl; l; l = l->tl())
		l->hd()->code_def(s,stringclasstag);
}

//
// Ints
//
void IntEntry::code_ref(ostream &s)
{
	s << INTCONST_PREFIX << index;
}

//
// Emit code for a constant Integer.
// You should fill in the code naming the dispatch table.
//

void IntEntry::code_def(ostream &s, int intclasstag)
{
	// Add -1 eye catcher
	s << WORD << "-1" << endl;

	code_ref(s);  s << LABEL                                // label
			<< WORD << intclasstag << endl                      // class tag
			<< WORD << (DEFAULT_OBJFIELDS + INT_SLOTS) << endl  // object size
			<< WORD; 

			emit_disptable_ref(Int, s);

			s << endl;                                          // dispatch table
			s << WORD << str << endl;                           // integer value
}


//
// IntTable::code_string_table
// Generate an Int object definition for every Int constant in the
// inttable.
//
void IntTable::code_string_table(ostream &s, int intclasstag)
{
	for (List<IntEntry> *l = tbl; l; l = l->tl())
		l->hd()->code_def(s,intclasstag);
}


//
// Bools
//
BoolConst::BoolConst(int i) : val(i) { assert(i == 0 || i == 1); }

void BoolConst::code_ref(ostream& s) const
{
	s << BOOLCONST_PREFIX << val;
}
	
//
// Emit code for a constant Bool.
// You should fill in the code naming the dispatch table.
//

void BoolConst::code_def(ostream& s, int boolclasstag)
{
	// Add -1 eye catcher
	s << WORD << "-1" << endl;

	code_ref(s);  s << LABEL                                  // label
			<< WORD << boolclasstag << endl                       // class tag
			<< WORD << (DEFAULT_OBJFIELDS + BOOL_SLOTS) << endl   // object size
			<< WORD;

			emit_disptable_ref(Bool, s);

			s << endl;                                            // dispatch table
			s << WORD << val << endl;                             // value (0 or 1)
}

//////////////////////////////////////////////////////////////////////////////
//
//  CgenClassTable methods
//
//////////////////////////////////////////////////////////////////////////////

//***************************************************
//
//  Emit code to start the .data segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_data()
{
	Symbol main    = idtable.lookup_string(MAINNAME);
	Symbol string  = idtable.lookup_string(STRINGNAME);
	Symbol integer = idtable.lookup_string(INTNAME);
	Symbol boolc   = idtable.lookup_string(BOOLNAME);

	str << "\t.data\n" << ALIGN;
	//
	// The following global names must be defined first.
	//
	str << GLOBAL << CLASSNAMETAB << endl;
	str << GLOBAL; emit_protobj_ref(main,str);    str << endl;
	str << GLOBAL; emit_protobj_ref(integer,str); str << endl;
	str << GLOBAL; emit_protobj_ref(string,str);  str << endl;
	str << GLOBAL; falsebool.code_ref(str);  str << endl;
	str << GLOBAL; truebool.code_ref(str);   str << endl;
	str << GLOBAL << INTTAG << endl;
	str << GLOBAL << BOOLTAG << endl;
	str << GLOBAL << STRINGTAG << endl;

	//
	// We also need to know the tag of the Int, String, and Bool classes
	// during code generation.
	//
	str << INTTAG << LABEL
			<< WORD << intclasstag << endl;
	str << BOOLTAG << LABEL 
			<< WORD << boolclasstag << endl;
	str << STRINGTAG << LABEL 
			<< WORD << stringclasstag << endl;    
}


//***************************************************
//
//  Emit code to start the .text segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_text()
{
	str << GLOBAL << HEAP_START << endl
			<< HEAP_START << LABEL 
			<< WORD << 0 << endl
			<< "\t.text" << endl
			<< GLOBAL;
	emit_init_ref(idtable.add_string("Main"), str);
	str << endl << GLOBAL;
	emit_init_ref(idtable.add_string("Int"),str);
	str << endl << GLOBAL;
	emit_init_ref(idtable.add_string("String"),str);
	str << endl << GLOBAL;
	emit_init_ref(idtable.add_string("Bool"),str);
	str << endl << GLOBAL;
	emit_method_ref(idtable.add_string("Main"), idtable.add_string("main"), str);
	str << endl;
}

void CgenClassTable::code_bools(int boolclasstag)
{
	falsebool.code_def(str,boolclasstag);
	truebool.code_def(str,boolclasstag);
}

void CgenClassTable::code_select_gc()
{
	//
	// Generate GC choice constants (pointers to GC functions)
	//
	str << GLOBAL << "_MemMgr_INITIALIZER" << endl;
	str << "_MemMgr_INITIALIZER:" << endl;
	str << WORD << gc_init_names[cgen_Memmgr] << endl;
	str << GLOBAL << "_MemMgr_COLLECTOR" << endl;
	str << "_MemMgr_COLLECTOR:" << endl;
	str << WORD << gc_collect_names[cgen_Memmgr] << endl;
	str << GLOBAL << "_MemMgr_TEST" << endl;
	str << "_MemMgr_TEST:" << endl;
	str << WORD << (cgen_Memmgr_Test == GC_TEST) << endl;
}


//********************************************************
//
// Emit code to reserve space for and initialize all of
// the constants.  Class names should have been added to
// the string table (in the supplied code, is is done
// during the construction of the inheritance graph), and
// code for emitting string constants as a side effect adds
// the string's length to the integer table.  The constants
// are emmitted by running through the stringtable and inttable
// and producing code for each entry.
//
//********************************************************

void CgenClassTable::code_constants()
{
	//
	// Add constants that are required by the code generator.
	//
	stringtable.add_string("");
	inttable.add_string("0");

	stringtable.code_string_table(str,stringclasstag);
	inttable.code_string_table(str,intclasstag);
	code_bools(boolclasstag);
}


CgenClassTable::CgenClassTable(Classes classes, ostream& s) : nds(NULL) , str(s)
{
	enterscope();
	if (cgen_debug) cout << "# Building CgenClassTable" << endl;
	install_basic_classes();
	install_classes(classes);
	build_inheritance_tree();

	setTagForAllObjects();

	stringclasstag = probe(Str)->getTag();
	intclasstag =    probe(Int)->getTag();
	boolclasstag =   probe(Bool)->getTag();


	for (List<CgenNode> *l = nds; l; l = l->tl()){
		CgenNodeP n = l->hd();
		class_map[n->getName()] = n;
	}

	code();
	exitscope();
}

void CgenClassTable::setTagForAllObjects(){
	CgenNodeP cur_node;

	std::stack<CgenNodeP> node_stk;
	node_stk.push(root());
	while (!node_stk.empty()){
		cur_node = node_stk.top();
		node_stk.pop();

		cur_node->collectFeatures();

		cur_node->setTag(newTag());

		for(List<CgenNode> *l = cur_node->getChildren(); l; l = l->tl()) {
			node_stk.push(l->hd());
		}
	}
}
void CgenClassTable::install_basic_classes()
{

// The tree package uses these globals to annotate the classes built below.
	//curr_lineno  = 0;
	Symbol filename = stringtable.add_string("<basic class>");

//
// A few special class names are installed in the lookup table but not
// the class list.  Thus, these classes exist, but are not part of the
// inheritance hierarchy.
// No_class serves as the parent of Object and the other special classes.
// SELF_TYPE is the self class; it cannot be redefined or inherited.
// prim_slot is a class known to the code generator.
//
	addid(No_class,
		new CgenNode(class_(No_class,No_class,nil_Features(),filename),
					Basic,this));
	addid(SELF_TYPE,
		new CgenNode(class_(SELF_TYPE,No_class,nil_Features(),filename),
					Basic,this));
	addid(prim_slot,
		new CgenNode(class_(prim_slot,No_class,nil_Features(),filename),
					Basic,this));

// 
// The Object class has no parent class. Its methods are
//        cool_abort() : Object    aborts the program
//        type_name() : Str        returns a string representation of class name
//        copy() : SELF_TYPE       returns a copy of the object
//
// There is no need for method bodies in the basic classes---these
// are already built in to the runtime system.
//
	install_class(
	 new CgenNode(
		class_(Object, No_class,
		append_Features(
			append_Features(
			single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
			single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
		filename),
	Basic,this));

// 
// The IO class inherits from Object. Its methods are
//        out_string(Str) : SELF_TYPE          writes a string to the output
//        out_int(Int) : SELF_TYPE               "    an int    "  "     "
//        in_string() : Str                    reads a string from the input
//        in_int() : Int                         "   an int     "  "     "
//
	install_class(
		new CgenNode( class_(IO, 
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
		filename),	    
	Basic,this));

//
// The Int class has no methods and only a single attribute, the
// "val" for the integer. 
//
	install_class(
		new CgenNode( class_(Int, 
			Object,
			single_Features(attr(val, prim_slot, no_expr())),
		filename),
	Basic,this));

//
// Bool also has only the "val" slot.
//
	install_class(
		new CgenNode(
			class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename),
	Basic,this));

//
// The class Str has a number of slots and operations:
//       val                                  ???
//       str_field                            the string itself
//       length() : Int                       length of the string
//       concat(arg: Str) : Str               string concatenation
//       substr(arg: Int, arg2: Int): Str     substring
//       
	install_class(
		new CgenNode(
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
						no_expr()))
					),
				single_Features(method(substr, 
					append_Formals(single_Formals(formal(arg, Int)), 
					single_Formals(formal(arg2, Int))),
					Str, 
					no_expr()))),
		filename),
	Basic,this));

}

// CgenClassTable::install_class
// CgenClassTable::install_classes
//
// install_classes enters a list of classes in the symbol table.
//
void CgenClassTable::install_class(CgenNodeP nd)
{
	Symbol name = nd->get_name();

	if (probe(name))
		{
			return;
		}

	// The class name is legal, so add it to the list of classes
	// and the symbol table.
	nds = new List<CgenNode>(nd,nds);
	addid(name,nd);
}

void CgenClassTable::install_classes(Classes cs)
{
	for(int i = cs->first(); cs->more(i); i = cs->next(i))
		install_class(new CgenNode(cs->nth(i),NotBasic,this));
}

//
// CgenClassTable::build_inheritance_tree
//
void CgenClassTable::build_inheritance_tree()
{
	for(List<CgenNode> *l = nds; l; l = l->tl())
			set_relations(l->hd());
}

//
// CgenClassTable::set_relations
//
// Takes a CgenNode and locates its, and its parent's, inheritance nodes
// via the class table.  Parent and child pointers are added as appropriate.
//
void CgenClassTable::set_relations(CgenNodeP nd)
{
	CgenNode *parent_node = probe(nd->get_parent());
	nd->set_parentnd(parent_node);
	parent_node->add_child(nd);
}

void CgenNode::add_child(CgenNodeP n)
{
	children = new List<CgenNode>(n,children);
}

void CgenNode::set_parentnd(CgenNodeP p)
{
	assert(parentnd == NULL);
	assert(p != NULL);
	parentnd = p;
}

void CgenClassTable::code()
{
	if (cgen_debug) cout << "# coding global data" << endl;
	code_global_data();

	if (cgen_debug) cout << "# choosing gc" << endl;
	code_select_gc();

	if (cgen_debug) cout << "# coding constants" << endl;
	code_constants();

	if (cgen_debug) { cout << "# coding class_nameTab" << endl; }
	codeClassNameTab();

	if (cgen_debug) { cout <<"# coding class_objTab" << endl; }
	codeClassObjTab();

	if (cgen_debug) { cout << "# coding dispatch tables" << endl; }
	codeDispatchTable();

	if (cgen_debug) { cout << "# coding prototype objects" << endl; }
	codeProtoTypeObj();

	if (cgen_debug) cout << "# coding global text" << endl;
	code_global_text();

	if (cgen_debug) { cout << "# coding object init " << endl; }
	codeObjectInit();

	if (cgen_debug) { cout << "# coding each method" << endl; }
	codeClassMethod();


//                 Add your code to emit
//                   - object initializer
//                   - the class methods
//                   - etc...

}

void CgenClassTable::codeClassNameTab() const{
	str << CLASSNAMETAB << LABEL;
	for (int i=0; i<getCurrTotalTag(); i++){
		for(List<CgenNode> *l = nds; l; l = l->tl()){
			CgenNodeP cur_node = l->hd();
			if (cur_node->getTag() == i){
				StringEntry* se = 
					stringtable.lookup_string(cur_node->get_name()->get_string());
				str << WORD; se->code_ref(str); str << endl;
				break;
			}
		}
	}
}

void CgenClassTable::codeClassObjTab() const{
	str << CLASSOBJTAB << LABEL;
	CgenNode* cur_node;
	for (int i=0; i<getCurrTotalTag(); i++){
		for (List<CgenNode> *l = nds; l; l = l->tl()){
			CgenNodeP cur_node = l->hd();
			if (cur_node->getTag() == i){
				str << WORD; 
					emit_protobj_ref(cur_node->get_name(), str); 
				str << endl;
				
				str << WORD; 
					emit_init_ref(cur_node->get_name(), str); 
				str << endl;
				break;
			}
		}		
	}
}

void CgenClassTable::codeDispatchTable() const{
	for (int i=0; i<getCurrTotalTag(); i++){
		for (List<CgenNode> *l = nds; l; l = l->tl()){
			CgenNodeP cur_node = l->hd();
			if (cur_node->getTag() == i){
				cur_node->codeDispatchTable(str);
				break;
			}
		}		
	}
}

void CgenClassTable::codeProtoTypeObj() const{
	for (int i=0; i<getCurrTotalTag(); i++){
		for (List<CgenNode> *l = nds; l; l = l->tl()){
			CgenNodeP cur_node = l->hd();
			if (cur_node->getTag() == i){
				cur_node->codeProtoTypeObj(str);
				break;
			}
		}		
	}
}

void CgenClassTable::codeObjectInit() const {
	for (int i=0; i<getCurrTotalTag(); i++){
		for (List<CgenNode> *l = nds; l; l = l->tl()){
			CgenNodeP cur_node = l->hd();
			if (cur_node->getTag() == i){
				cur_node->codeObjectInit(str);
				break;
			}
		}		
	}
}
void CgenClassTable::codeClassMethod() const {
	for (int i=0; i<getCurrTotalTag(); i++){
		for (List<CgenNode> *l = nds; l; l = l->tl()){
			CgenNodeP cur_node = l->hd();
			if (cur_node->getTag() == i){
				cur_node->codeClassMethod(str);
				break;
			}
		}		
	}
}

const CgenNodeP CgenClassTable::root() { return probe(Object); };
const CgenNodeP CgenClassTable::getNodeWithTag(const unsigned int tag){
	CgenNodeP cur_node;
	for(List<CgenNode> *l = nds; l; l = l->tl()){
		cur_node = l->hd();
		if (cur_node->getTag() == tag){
			return cur_node;
		}
	}
	return NULL;
}
///////////////////////////////////////////////////////////////////////
//
// CgenNode methods
//
///////////////////////////////////////////////////////////////////////

void CgenNode::codeDispatchTable(ostream& str) const {
	emit_disptable_ref(name, str); str << LABEL;
	// For each method
	for (int i=0; i<method_vec_.size(); i++){
		Method m = method_vec_[i];
		str << WORD; 
		emit_method_ref(m->getNative(), m->getName(), str); 
		str << endl;	
	}
}
void CgenNode::codeProtoTypeObj(ostream& str) const {
	// Word -1: GC word.
	str << WORD << -1 << endl;
	emit_protobj_ref(name, str); str << LABEL;

	// Word 0: Tag.
	str << WORD << tag_ << endl;

	// Word 1: Size.
	str << WORD << DEFAULT_OBJFIELDS + attr_vec_.size() << endl;

	// Word 2: Dispatch Table
	str << WORD; emit_disptable_ref(name, str); str << endl;

	// Word 3-n: Attribute
	for (int i=0; i<attr_vec_.size(); i++){
		str << WORD;
		Attribute a = attr_vec_[i];
		// Three primitive type has the following default value:
		// Int: 0
		if (a->type_decl == Int){
			IntEntry* int_entry = inttable.lookup_string("0");
			int_entry->code_ref(str);
		
		// String: ""
		} else if (a->type_decl == Bool){
			BoolConst bool_const = BoolConst(false);
			bool_const.code_ref(str);
		
		// Bool: false
		} else if (a->type_decl == Str){
			StringEntry* str_entry = stringtable.lookup_string("");
			str_entry->code_ref(str);

		// Last case: Set it to void.		
		} else {
			str << 0;
		}
		str << endl;
	}
}

void CgenNode::codeObjectInit(ostream& str) const{
	emit_init_ref(name, str); 	str << LABEL;
	// Callee startup.
	emitCalleeStart(str);

	// Init parent first. Only Object has no parent.
	if (name != Object){
		str << JAL; emit_init_ref(parentnd->get_name(), str); str << endl;
	}

	// Init each attribute defined in this class,
	// as inherited attribute has been defined.
	for (int i=0; i<attr_vec_.size(); i++){
		Attribute a = attr_vec_[i];
		// This attribute is not native defined but inherited.
		// It should've been inited in a parent class,
		// so we don't care about it.
		if (a->getNative() != name){ continue; }

		// If there is no init expression, there is no need
		// to add anything, just use the default is enought.
		if (typeid(*(a->getInit())) != typeid(no_expr_class)) {
			a->getInit()->code(str);
			emit_store(ACC, i+3, SELF, str);
		}
	}

	// Return inited object.
	emit_move(ACC, SELF, str);

	// Callee quit procedure. There is no arg for init procedure.
	emitCalleeReturn(0, str);
	
}
void CgenNode::codeClassMethod(ostream& str){
	// We don't generate code for primitive type.
	if (name == Object || 
	  name == Str || name == Int || name == Bool ||
	  name == IO){
		return;
	}
	cur_class = this;
	env.enterscope();

	// Add all attributes to current scope.
	for (int i=0; i<attr_vec_.size(); i++){
		env.addid(
			attr_vec_[i]->getName(),  						// Key
			new ObjectLocation(
				SELF, DEFAULT_OBJFIELDS + i, attr_vec_[i]->getType()) 	// Value
		);
	}
	// self have SELF_TYPE
	env.addid(
		self,
		new ObjectLocation(SELF, 0, SELF_TYPE)
	);

	// Code each method one by one. Skip those inherited ones.
	for (int i=0; i<method_vec_.size(); i++){
		Method m = method_vec_[i];
		// Code method only when this method is defined by myself,
		// or it would've been coded by it's parent.
		if (m->getNative() == name){
			m->codeMethod(str);
		}
	}

	env.exitscope();
}

void method_class::codeMethod(ostream& str) const{
	// Mark a label.
	emit_method_ref(getNative(), name, str); 	str << LABEL;

	// Prepare to enter this function and create a new scope.
	emitCalleeStart(str);
	env.enterscope();

	// Add all formals.
	for (int i=formals->first(); formals->more(i); i=formals->next(i)){
		Formal formal = formals->nth(i);
		env.addid(
			formal->getName(), 										// Key
			new ObjectLocation(
				FP,  3 + formals->len() - i - 1, formal->getType())	// Value
		);
	}

	// Evaluate the expression inside.
	expr->code(str);
	// Exit scope and reutrn this function.
	env.exitscope();
	emitCalleeReturn(formals->len(), str);
}
CgenNode::CgenNode(Class_ nd, Basicness bstatus, CgenClassTableP ct) :
	 class__class((const class__class &) *nd),
	 parentnd(NULL),
	 children(NULL),
	 basic_status(bstatus)
{ 
	 stringtable.add_string(name->get_string());          // Add class name to string table
}

void CgenNode::collectFeatures(){
	// Object has no parent. Leave it empty.
	// Or we "stole" the map from the parent.
	if (parentnd){
		method_vec_ = std::vector<Method>(parentnd->getMethodVec());
		attr_vec_ = std::vector<Attribute>(parentnd->getAttrVec());
	}

	Features features = this->getFeatures();

	for (int i=0; i<features->len(); i++){
		Feature f = features->nth(i);
		f->setNative(name);

		// Add to attribute table.
		if (f->isAttribute()){
			Attribute a  = dynamic_cast<Attribute>(f);
			attr_vec_.push_back(a);

		// Add to method table.
		} else {
			Method m = dynamic_cast<Method>(f);
			// Record which object does this method belong to.
			// This will not change, unless it's override by 
			// inherted method.
			bool inherited = false;
			for (int i=0; i<method_vec_.size(); i++){
				if (method_vec_[i]->getName() == m->getName()){
					method_vec_[i] = m;
					inherited = true;
					// No need for further probing.
					break;
				} 
			}
			if (!inherited){
				method_vec_.push_back(m);
			}
		}
	}
}
//******************************************************************
//
//   Fill in the following methods to produce code for the
//   appropriate expression.  You may add or remove parameters
//   as you wish, but if you do, remember to change the parameters
//   of the declarations in `cool-tree.h'  Sample code for
//   constant integers, strings, and booleans are provided.
//
//*****************************************************************

// Counting the LAST USED label. 
// Add 1 to create new label.
// Initially it's -1. The first usable label is 0.
int label_cnt = -1;
// Labels are counted in terms of int. Each decimal number represents
// a new lebel. To create a new one, simply add it and return it.
int newLabel(){
	label_cnt ++;
	return label_cnt;
}

#define DEF_LABEL(label) emit_label_def(label, s)

//
// * call for functions that is defined outside of this class;
// * use of global variables everywhere;
// * yet const correctness is nowhere to be found;
// * memory leaks like Chernobyl;
// * use of unsafe C functions when you have std;
// * forget about naming rules too when you have lowercased class name
//
// Let's make it polite: 
//
//      such code should be posted on pornhub instead of github. :)
//
void assign_class::code(ostream &s) {
	// Eval expression
	expr->code(s);
	// Get the location(in terms of register).
	ObjectLocation* obj_loc = env.lookup(name);
	// Copy it to that location.
	emit_store(ACC, obj_loc->getOffset(), obj_loc->getReg(), s);
    if (cgen_debug)     s << "# [Done] encoding assign_class" << endl;
}

// An helper function for dispatch
// type_name is of use only when it's non-static dispatch
void dispatch_common(Expression& expr, Symbol * type_name, Symbol& name, Expressions& actual,
					 char * filename, int line_num, bool is_static_dispatch, ostream &s){
	if (cgen_debug)  s << "# dispatch_common called" << endl;


	if (cgen_debug)  s << "# creating new label" << endl;
	// create new label
	int label_exec = newLabel();

	// deal with all args and push to stack (a1-aN)
	for (int i = 0; i < actual->len(); i++){
		Expression arg = actual->nth(i);
		// eval the arg and push to stack
		arg->code(s);
		emit_push(ACC,s);
	}

	// the first parameter is the object itself
	if (cgen_debug)  s << "# generating code for object_id: " << expr->get_type() << endl;
	if (expr->get_type() == SELF_TYPE) {
		// move s0 to a0 (as the first parameter)
		emit_move(ACC, SELF, s);
	}
	else{
		// eval the attr name, the attr get loaded to $a0 automatically
		expr->code(s);
	}

	if (cgen_debug)  s << "# abort on error" << endl;
	// Abort
	// if a0 is $0, abort and call dispatch_abort
	emit_bne(ACC,ZERO,label_exec,s);
	// load filename
	StringEntry * fname = stringtable.lookup_string(filename);
	emit_partial_load_address(ACC,s);
	fname->code_ref(s);
	s << endl;
	// T1 is the line number
	emit_load_imm(T1,line_num,s);
	emit_jal("_dispatch_abort",s);

	if (cgen_debug)  s << "# exec dispatched table" << endl;
	// Valid: preceed to exec
	emit_label_def(label_exec,s);

	// get the dispatch table of object at $a0
	if (is_static_dispatch){
		emit_partial_load_address(T1,s);
		emit_disptable_ref(*type_name,s);
		s << endl;
	}
	else {
		emit_load(T1, 2, ACC, s);
	}

	std::vector<Method> method_vec;
	// get the dispatch table for object
	if (is_static_dispatch){
		method_vec = class_map[*type_name]->getMethodVec();
	}
	else {
		Symbol object_name = expr->get_type();
		if (object_name == SELF_TYPE) {
			method_vec = cur_class->getMethodVec();
		} else {
			method_vec = class_map[object_name]->getMethodVec();
		}
	}

	// get the function offset
	int func_offset = 0;
	// search for the target function in the dispatch table
	for (func_offset=0; func_offset < method_vec.size(); func_offset++){
		Method m = method_vec[func_offset];
		
		if (m->getName() == name){ break; }
	}
	emit_load(T1, func_offset, T1, s);

	// jalr
	emit_jalr(T1,s);
}

void static_dispatch_class::code(ostream &s) {
    if (cgen_debug)     s << "# encoding static dispatch" << endl;

	char * filename = cur_class->get_filename()->get_string();
	dispatch_common(expr, &type_name,name,actual,filename,this->get_line_number(),true,s);
}

void dispatch_class::code(ostream &s) {
	if (cgen_debug) s << "# dispatch called" << endl;

	char * filename = cur_class->get_filename()->get_string();
	dispatch_common(expr,NULL,name,actual,filename,this->get_line_number(),false,s);

}

void cond_class::code(ostream &s) {
	int label_else = newLabel();
	int label_endif = newLabel();

	pred->code(s);
	emit_load_bool(T1, falsebool, s);
	emit_beq(ACC, T1, label_else, s);
		then_exp->code(s);
		emit_branch(label_endif, s);
	DEF_LABEL(label_else);
		else_exp->code(s);
	DEF_LABEL(label_endif);
}

void loop_class::code(ostream &s) {
    if (cgen_debug)     s << "# encoding loop_class" << endl;
	int label_beginloop = newLabel();
	int label_endloop = newLabel();

	// Loop begin.
	DEF_LABEL(label_beginloop);

	// Whether condition is true.
	pred->code(s);
	emit_load_bool(T1, falsebool, s);
	emit_beq(ACC, T1, label_endloop, s);

	// Loop
	body->code(s);

	// Jump back. One iter finished.
	emit_branch(label_beginloop, s);
	DEF_LABEL(label_endloop);
}

void typcase_class::code(ostream &s) {
    if (cgen_debug)     s << "# encoding typecase_class" << endl;
	int label_notvoid = newLabel();
	int label_endcase = newLabel();

	// Get shared parent.

	expr->code(s);
	// In case of a void match.
	emit_bne(ACC, ZERO, label_notvoid, s);
	// Get filename and line number, abort.
		StringEntry *str_entry = 
			stringtable.lookup_string(cur_class->get_filename()->get_string());
		emit_load_string(ACC, str_entry, s);
		emit_load_imm(T1, get_line_number(), s);
		emit_jal("_case_abort2", s);
	// Not a void match, go on.
	DEF_LABEL(label_notvoid);
	
	emit_push(ACC, s);
	

	int label_next = newLabel();
	int label_curr = -1;
	for (int i=cases->first(); cases->more(i); i = cases->next(i)){
		Case c = cases->nth(i);

		env.enterscope();
		env.addid(
			c->getName(),							// Key
			new ObjectLocation(
				FP, -local_var_cnt, c->getType()) 	// Value
		);
		local_var_cnt ++;

		// Prepare label.
		label_curr = label_next;
		label_next = newLabel();
		DEF_LABEL(label_curr);
		// Prepare condition.
		emit_load(ACC, 1, SP, s);
		// Current class tag.
		emit_load(T1, TAG_OFFSET, ACC, s);
		// Branch's tag.
		emit_load_imm(T2, class_map[c->getType()]->getTag(), s);
		// Not a match.
		emit_bne(T1, T2, label_next, s);
		// This is a match.
		c->getExpr()->code(s);
		emit_branch(label_endcase, s);

		local_var_cnt --;
		env.exitscope();
	}
    DEF_LABEL(label_next);
	// No match found.
	emit_load(ACC, 1, SP, s);
	emit_jal("_case_abort", s);

	DEF_LABEL(label_endcase);
	emit_pop(s);
}

void block_class::code(ostream &s) {
    if (cgen_debug)     s << "# encoding block" << endl;
	for (int i = body->first(); body->more(i); i = body->next(i)){
		body->nth(i)->code(s);
	}
}

void let_class::code(ostream &s) {
    if (cgen_debug)     s << "# encoding let" << endl;
	// Init the new object.
	if (typeid(*init) == typeid(no_expr_class)) {
		// Int: 0
		if (type_decl == Int){
			IntEntry* int_entry = inttable.lookup_string("0");
			emit_load_int(ACC, int_entry, s);
		
		// String: ""
		} else if (type_decl == Bool){
			BoolConst bool_const = BoolConst(false);
			emit_load_bool(ACC, bool_const, s);
		
		// Bool: false
		} else if (type_decl == Str){
			StringEntry* str_entry = stringtable.lookup_string("");
			emit_load_string(ACC, str_entry, s);

		} else {
			init->code(s);
		}
	} else {
		init->code(s);
	}

	// Enter scope and add a new var.
	env.enterscope();
	env.addid(
		identifier,											// Key
		new ObjectLocation(FP, -local_var_cnt, type_decl) 	// Value
	);
	local_var_cnt ++;

	emit_push(ACC, s);
	body->code(s);
	emit_pop(s);

	// Exit.
	local_var_cnt --;
	env.exitscope();
}

// Notice that arith are based on Int type and thus need to
// use fetch int.
void arith_common(Expression e1, Expression e2, ostream& s){
    if (cgen_debug)     s << "# arith_common is called" << endl;
	// Eval e1. (the result is stored in a0)
	e1->code(s);
	// push a0 to stack
	emit_push(ACC, s);
	local_var_cnt ++;
	// eval e2
	e2->code(s);
	emit_copy(s);
	// Take e2.
	emit_fetch_int(T2, ACC, s);
	// Take e1
	emit_pop(T1, s);
	emit_fetch_int(T1, T1, s);
	local_var_cnt--;
}

void plus_class::code(ostream &s) {
	arith_common(e1, e2, s);
	// Doing the op.
	emit_addu(T1, T1, T2, s);
	emit_store_int(T1, ACC, s);
}

void sub_class::code(ostream &s) {
	arith_common(e1, e2, s);
	// Doing the op.
	emit_sub(T1, T1, T2, s);
	emit_store_int(T1, ACC, s);
}

void mul_class::code(ostream &s) {
	arith_common(e1, e2, s);
	// Doing the op.
	emit_mul(T1, T1, T2, s);
	emit_store_int(T1, ACC, s);
}

void divide_class::code(ostream &s) {
	arith_common(e1, e2, s);
	// Doing the op.
	emit_div(T1, T1, T2, s);
	emit_store_int(T1, ACC, s);
}

// Negate.
void neg_class::code(ostream &s) {
	// eval e1: get int object
	e1->code(s);
	emit_copy(s);
	// load value of int object
	emit_load(T1, 3, ACC, s);
	emit_neg(T1, T1, s);
	// store value back
	emit_store_int(T1, ACC, s);
}

// Not
void comp_class::code(ostream &s) {
	int label_endcomp = newLabel();
	e1->code(s);
	emit_load(T1, 3, ACC, s);
	emit_load_bool(ACC, truebool, s);
	emit_beqz(T1, label_endcomp, s);
	emit_load_bool(ACC, falsebool, s);
	DEF_LABEL(label_endcomp);
}

void lessThanCommon(Expression e1, Expression e2, ostream& s, const bool eq){
    if (cgen_debug)     s << "# lessThanCommon is called" << endl;
	int label_isles = newLabel() ;
	int label_endif = newLabel();
	// eval e1, e2 and store to t1 t2
	arith_common(e1, e2, s);
	if (eq){
		emit_bleq(T1, T2, label_isles, s);
	} else {
		emit_blt(T1, T2, label_isles, s);
	}
	// Else branch.
	emit_load_bool(ACC, falsebool, s);
	emit_branch(label_endif, s);
	// Then branch. T1 <(=) T2.
	DEF_LABEL(label_isles);
	emit_load_bool(ACC, truebool, s);
	// Endif.
	DEF_LABEL(label_endif);
}

void lt_class::code(ostream &s) {
	lessThanCommon(e1, e2, s, false);
}

void leq_class::code(ostream &s) {
	lessThanCommon(e1, e2, s, true);
}

void eq_class::code(ostream &s) {
    if (cgen_debug)     s << "# encoding eq" << endl;
	int label_ptreq = newLabel();

	e1->code(s);
	emit_push(ACC, s);
	local_var_cnt ++;
	e2->code(s);
	local_var_cnt --;

	emit_pop(T1, s);
	emit_move(T2, ACC, s);
	
	// See this usage of function equality_test in 
	// trap.s line 458.
	emit_load_bool(ACC, truebool, s);
	emit_load_bool(A1, falsebool, s);

	// Of the same ptr/value, don't bother to compare.
	emit_beq(T1, T2, label_ptreq, s);
		// Call eq test.
		emit_jal("equality_test", s);
	DEF_LABEL(label_ptreq);
	// End of branch.
}

void int_const_class::code(ostream& s)
{
    if (cgen_debug)     s << "# encoding int_const" << endl;
	//
	// Need to be sure we have an IntEntry *, not an arbitrary Symbol
	//
	emit_load_int(ACC,inttable.lookup_string(token->get_string()),s);
}

void string_const_class::code(ostream& s)
{
    if (cgen_debug)     s << "# encoding string_const" << endl;
	emit_load_string(ACC,stringtable.lookup_string(token->get_string()),s);
}

void bool_const_class::code(ostream& s)
{
    if (cgen_debug)     s << "# encoding bool_const" << endl;
	emit_load_bool(ACC, BoolConst(val), s);
}

void new__class::code(ostream &s) {
    if (cgen_debug)     s << "# encoding new_class" << endl;
	if (type_name == SELF_TYPE){
		emit_load(T1, 0, SELF, s);
		// *8 == <<3
		emit_sll(T1, T1, 3, s);
		emit_load_address(T2, CLASSOBJTAB, s);
		emit_addu(T2, T1, T2, s);
		// Use stack to temporarily save table address.
		emit_push(T2, s);
		// Address for prototype in acc.
		emit_load(ACC, 0, T2, s);
		emit_copy(s);
		// Load table address.
		emit_pop(T2, s);
		// Load init address.
		emit_load(T2, 1, T2, s);
		// Init.
		emit_jalr(T2, s);
	} else {
		// Get object proto type.
		emit_partial_load_address(ACC, s);
		emit_protobj_ref(type_name, s);
		s << endl;
		s << "# new" << type_name->get_string() << endl;
		
		// Copy it to heap.
		emit_copy(s);

		s << JAL; emit_init_ref(type_name, s); s << endl;
	}
}

void isvoid_class::code(ostream &s) {
    if (cgen_debug)     s << "# encoding isvoid" << endl;
	int label_isvod = newLabel();
	int label_endif = newLabel();

	e1->code(s);
	emit_beq(ACC, ZERO, label_isvod, s);
		emit_load_bool(ACC, falsebool, s);
		emit_branch(label_endif, s);
	
	DEF_LABEL(label_isvod);
		emit_load_bool(ACC, truebool, s);

	DEF_LABEL(label_endif);
}

void no_expr_class::code(ostream &s) {
    if (cgen_debug)     s << "# encoding no_expr" << endl;
	emit_move(ACC, ZERO, s);
}

void object_class::code(ostream &s) {
    if (cgen_debug)     s << "# encoding object" << endl;
	
	if (name == self){
		emit_move(ACC, SELF, s);
	} else {
		// search for the location (offset to the object)
		ObjectLocation * loc = env.lookup(name);
		// move the attr to a0
		emit_load(ACC,loc->getOffset(), loc->getReg(), s);
	}
}