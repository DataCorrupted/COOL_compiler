#include <assert.h>
#include <stdio.h>
#include "emit.h"
#include "cool-tree.h"
#include "symtab.h"
#include <map>
#include <vector>

enum Basicness     {Basic, NotBasic};
#define TRUE 1
#define FALSE 0

class CgenClassTable;
typedef CgenClassTable *CgenClassTableP;

class CgenNode;
typedef CgenNode *CgenNodeP;

class CgenClassTable : public SymbolTable<Symbol,CgenNode> {
private:
	 List<CgenNode> *nds;
	 ostream& str;
	 int stringclasstag;
	 int intclasstag;
	 int boolclasstag;
	 int tag_cnt_ = -1;

// The following methods emit code for
// constants and global declarations.

	void code_global_data();
	void code_global_text();
	void code_bools(int);
	void code_select_gc();
	void code_constants();

	void codeClassNameTab() const;
	void codeClassObjTab() const;
	void codeDispatchTable() const;
	void codeProtoTypeObj() const;
	void codeObjectInit() const ;
	void codeClassMethod() const ;

// The following creates an inheritance graph from
// a list of classes.  The graph is implemented as
// a tree of `CgenNode', and class names are placed
// in the base class symbol table.

	 void install_basic_classes();
	 void install_class(CgenNodeP nd);
	 void install_classes(Classes cs);
	 void build_inheritance_tree();
	 void set_relations(CgenNodeP nd);
public:
	CgenClassTable(Classes, ostream& str);
	void setTagForAllObjects();
	const int newTag() {
		tag_cnt_++;
		return tag_cnt_;
	}
	void code();
	const CgenNodeP root();
	const CgenNodeP getNodeWithTag(const unsigned int);
};


class CgenNode : public class__class {
private: 
	 CgenNodeP parentnd;                        // Parent of class
	 List<CgenNode> *children;                  // Children of class
	 Basicness basic_status;                    // `Basic' if class is basic
												// `NotBasic' otherwise
	 std::map<Symbol, Method> method_map_;
	 std::vector<Attribute> attr_vec_;

	 int tag_ = -1;

public:
	CgenNode(Class_ c,
			Basicness bstatus,
			CgenClassTableP class_table);

	void collectFeatures();
	void codeDispatchTable(ostream&) const;
	// Generate a proto tpye code for this object.
	void codeProtoTypeObj(ostream&) const;
	void codeObjectInit(ostream&) const;
	void codeClassMethod(ostream&) const;
	const std::map<Symbol, Method>& getMethodMap() const { return method_map_; }
	const std::vector<Attribute>& getAttrVec() const { return attr_vec_; }

	void add_child(CgenNodeP child);
	List<CgenNode> *getChildren() { return children; }
	void set_parentnd(CgenNodeP p);
	CgenNodeP get_parentnd() { return parentnd; }
	int basic() { return (basic_status == Basic); }
	void setTag(const int tag) {tag_ = tag;}
	const int getTag() const { return tag_; }
};

class BoolConst 
{
 private: 
	int val;
 public:
	BoolConst(int);
	void code_def(ostream&, int boolclasstag);
	void code_ref(ostream&) const;
};

class ObjectLocation {
private:
	char* reg_;
	int offset_;
public:
	ObjectLocation(char* c, int o): reg_(c), offset_(o) {;};
	const char* getReg() const { return reg_; };
	const int getOffset() const { return offset_; };
};
