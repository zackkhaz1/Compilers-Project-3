#ifndef CRONAC_AST_HPP
#define CRONAC_AST_HPP

#include <ostream>
#include <list>
#include "tokens.hpp"

// **********************************************************************
// ASTnode class (base class for all other kinds of nodes)
// **********************************************************************

namespace crona{

/* You may find it useful to forward declare AST subclasses
   here so that you can use a class before it's full definition
*/
class DeclListNode;
class DeclNode;
class TypeNode;
class IDNode;
class StmtNode;

class ASTNode{
public:
	ASTNode(size_t lineIn, size_t colIn)
	: l(lineIn), c(colIn) {}

	virtual void unparse(std::ostream& out, int indent) = 0;
	size_t line(){ return l; }
	size_t col() { return c; }

	/**
	* Return a string specifying the position this node begins.
	* For some node a position doesn't really make sense (i.e.
	* ProgramNode) but for the rest it's the position in the
	* input file that represents that node
	**/
	std::string pos(){
		return "[" + std::to_string(line()) + ","
		+ std::to_string(col()) + "]";
	}

private:
	size_t l; /// The line at which the node starts in the input file
	size_t c; /// The column at which the node starts in the input file
};

/**
* \class ProgramNode
* Class that contains the entire abstract syntax tree for a program.
* Note the list of declarations encompasses all global declarations
* which includes (obviously) all global variables and struct declarations
* and (perhaps less obviously), all function declarations
**/
class ProgramNode : public ASTNode{
public:
	ProgramNode(std::list<DeclNode *> * globalsIn)
	: ASTNode(1, 1), myGlobals(globalsIn) {}

	void unparse(std::ostream& out, int indent) override;
private:
	std::list<DeclNode * > * myGlobals;
};


/** \class DeclNode
* Superclass for declarations (i.e. nodes that can be used to
* declare a struct, function, variable, etc).  This base class will
**/
class DeclNode : public ASTNode{
public:
	DeclNode(size_t line, size_t col)
	: ASTNode(line, col) {}

	void unparse(std::ostream& out, int indent) override = 0;
};

class StmtNode : public ASTNode{
public:
	StmtNode(size_t line, size_t col)
	: ASTNode(line, col) {}

	virtual void unparse(std::ostream& out, int indent) override = 0;
};

class ExpNode : public ASTNode{
protected:
	ExpNode(size_t line, size_t col)
	: ASTNode(line, col) {}
public:
	virtual void unparse(std::ostream& out, int indent) override = 0;
};

class TypeNode : public ASTNode{
protected:
	TypeNode(size_t lineIn, size_t colIn)
	: ASTNode(lineIn, colIn) {}

public:
	virtual void unparse(std::ostream& out, int indent) = 0;
	//TODO: consider adding an isRef to use in unparse to
	// indicate if this is a reference type
};

class LValNode : public ExpNode{
public:
	LValNode(size_t line, size_t col)
	: ExpNode(line, col) {}
	void unparse(std::ostream& out, int indent) override = 0;
private:

};

class IDNode : public LValNode{
public:
	IDNode(IDToken * token)
	: LValNode(token->line(), token->col()) {}

	void unparse(std::ostream& out, int indent);
private:
	std::string myStrVal;
};

class VarDeclNode : public DeclNode{
public:
	VarDeclNode(size_t l, size_t c, TypeNode * type, IDNode * id)
	: DeclNode(type->line(), type->col()), myType(type), myId(id) {}

	void unparse(std::ostream& out, int indent);
private:
	TypeNode * myType;
	IDNode * myId;
};

class FormalDeclNode : public VarDeclNode{
public:
	FormalDeclNode(size_t l, size_t c, TypeNode* type, IDNode* id)
	: VarDeclNode(type->line(), type->col(), type, id), myType(type), myId(id){
}
	void unparse(std::ostream& out, int indent);
private:
	TypeNode* myType;
	IDNode* myId;
};

class FnDeclNode : public DeclNode{
public:
	FnDeclNode(size_t l, size_t c, TypeNode* type, IDNode* id, std::list<FormalDeclNode*>* params, std::list<StmtNode*>* body)
	:  DeclNode(type->line(), type->col()), myType(type), myId(id), formals(params), bodyVal(body) {}
	void unparse(std::ostream& out, int indent) override;
private:
	TypeNode* myType;
	IDNode* myId;
	std::list<FormalDeclNode*>* formals;
	std::list<StmtNode*>* bodyVal;
};

///////TYPENODE CLASSES////////////
///////////////////////////////////

class ArrayTypeNode : public TypeNode{
public:
	ArrayTypeNode(size_t l, size_t c, TypeNode* type)
	: TypeNode(type->line(), type->col()), myType(type) {}
	void unparse(std::ostream& out, int indent) override;
private:
	TypeNode* myType;
};

class BoolTypeNode : public TypeNode{
public:
	BoolTypeNode(size_t l, size_t c)
	: TypeNode(l,c){}
	void unparse(std::ostream& out, int indent) override;
private:
};

class ByteTypeNode : public TypeNode{
public:
	ByteTypeNode(size_t l, size_t c)
	: TypeNode(l,c){}
	void unparse(std::ostream& out, int indent) override;
private:
};

class IntTypeNode : public TypeNode{
public:
	IntTypeNode(size_t lineIn, size_t colIn)
	: TypeNode(lineIn, colIn) {}
	void unparse(std::ostream& out, int indent);
private:
};

class VoidTypeNode : public TypeNode{
public:
	VoidTypeNode(size_t l, size_t c)
	: TypeNode(l,c){}
	void unparse(std::ostream& out, int indent) override;
private:
};

///////EXPNODE CLASSES//////////////
///////////////////////////////////

class AssignExpNode : public ExpNode{
public:
	AssignExpNode(size_t l, size_t c, LValNode* dst, ExpNode* source)
	: ExpNode(l,c), dest(dst), src(source) { }
	void unparse(std::ostream& out, int indent) override;
private:
	LValNode* dest;
	ExpNode* src;
};

class BinaryExpNode : public ExpNode{
public:
	BinaryExpNode(size_t l, size_t c, ExpNode* left, ExpNode* right)
	: ExpNode(l,c), lhs(left), rhs(right) {}

protected:
	ExpNode* lhs;
	ExpNode* rhs;
};

class CallExpNode : public ExpNode{
public:
	CallExpNode(size_t l, size_t c, IDNode* id, std::list<ExpNode*>* listOfExp)
	: ExpNode(l,c), myIDNode(id), myListOfExp(listOfExp) {}
	void unparse(std::ostream& out, int indent) override;
private:
	IDNode* myIDNode;
	std::list<ExpNode*>* myListOfExp;
};

class FalseNode : public ExpNode{
public:
	FalseNode(size_t l, size_t c)
	: ExpNode(l,c) { }
	void unparse(std::ostream& out, int indent) override;
private:
};

class HavocNode : public ExpNode{
public:
	HavocNode(size_t l, size_t c)
	: ExpNode(l,c) {}
	void unparse(std::ostream& out, int indent) override;
private:
};

class IntLitNode : public ExpNode{
public:
	IntLitNode(size_t l, size_t c, const int src)
	: ExpNode(l,c), val(src) {}
	void unparse(std::ostream& out, int indent) override;
private:
	int val;
};

class StrLitNode : public ExpNode{
public:
	StrLitNode(size_t l, size_t c, const std::string src)
	: ExpNode(l,c), val(src) {}
	void unparse(std::ostream& out, int indent) override;
private:
	std::string val;
};

class TrueNode : public ExpNode{
public:
	TrueNode(size_t l, size_t c)
	: ExpNode(l,c) { }
	void unparse(std::ostream& out, int indent) override;
private:
};

class UnaryExpNode : public ExpNode{
public:
	UnaryExpNode(size_t l, size_t c, ExpNode* src)
	: ExpNode(l,c), val(src) {}

protected:
	ExpNode* val;

};

///////BINARYEXPNODE SUBCLASSES//////////////
////////////////////////////////////////////

class AndNode : public BinaryExpNode{
public:
	AndNode(size_t l, size_t c, ExpNode* left, ExpNode* right)
	: BinaryExpNode(l,c,left,right){}
	void unparse(std::ostream& out, int indent) override;
private:
};

class DivideNode : public BinaryExpNode{
public:
	DivideNode(size_t l, size_t c, ExpNode* left, ExpNode* right)
	: BinaryExpNode(l,c,left,right){}
	void unparse(std::ostream& out, int indent) override;
private:
};

class EqualsNode : public BinaryExpNode{
public:
	EqualsNode(size_t l, size_t c, ExpNode* left, ExpNode* right)
	: BinaryExpNode(l,c,left,right){}
	void unparse(std::ostream& out, int indent) override;
private:
};

class GreaterEqNode : public BinaryExpNode{
public:
	GreaterEqNode(size_t l, size_t c, ExpNode* left, ExpNode* right)
	: BinaryExpNode(l,c,left,right){}
	void unparse(std::ostream& out, int indent) override;
private:
};

class GreaterNode : public BinaryExpNode{
public:
	GreaterNode(size_t l, size_t c, ExpNode* left, ExpNode* right)
	: BinaryExpNode(l,c,left,right){}
	void unparse(std::ostream& out, int indent) override;
private:
};

class LessEqNode : public BinaryExpNode{
public:
	LessEqNode(size_t l, size_t c, ExpNode* left, ExpNode* right)
	: BinaryExpNode(l,c,left,right){}
	void unparse(std::ostream& out, int indent) override;
private:
};

class LessNode : public BinaryExpNode{
public:
	LessNode(size_t l, size_t c, ExpNode* left, ExpNode* right)
	: BinaryExpNode(l,c,left,right){}
	void unparse(std::ostream& out, int indent) override;
private:
};

class MinusNode : public BinaryExpNode{
public:
	MinusNode(size_t l, size_t c, ExpNode* left, ExpNode* right)
	: BinaryExpNode(l,c,left,right){}
	void unparse(std::ostream& out, int indent) override;
private:
};

class NotEqualsNode : public BinaryExpNode{
public:
	NotEqualsNode(size_t l, size_t c, ExpNode* left, ExpNode* right)
	: BinaryExpNode(l,c,left,right){}
	void unparse(std::ostream& out, int indent) override;
private:
};

class OrNode : public BinaryExpNode{
public:
	OrNode(size_t l, size_t c, ExpNode* left, ExpNode* right)
	: BinaryExpNode(l,c,left,right){}
	void unparse(std::ostream& out, int indent) override;
private:
};

class PlusNode : public BinaryExpNode{
public:
	PlusNode(size_t l, size_t c, ExpNode* left, ExpNode* right)
	: BinaryExpNode(l,c,left,right){}
	void unparse(std::ostream& out, int indent) override;
private:
};

class TimesNode : public BinaryExpNode{
public:
	TimesNode(size_t l, size_t c, ExpNode* left, ExpNode* right)
	: BinaryExpNode(l,c,left,right){}
	void unparse(std::ostream& out, int indent) override;
private:
};

class NegNode : public UnaryExpNode{
public:
	NegNode(size_t l, size_t c, ExpNode* src)
	: UnaryExpNode(l,c,src) { }
	void unparse(std::ostream& out, int indent) override;
private:
};

class NotNode : public UnaryExpNode{
public:
	NotNode(size_t l, size_t c, ExpNode* src)
	: UnaryExpNode(l,c,src) { }
	void unparse(std::ostream& out, int indent) override;
private:
};

///////STMTNODE CLASSES////////////
///////////////////////////////////

class AssignStmtNode : public StmtNode{
public:
	AssignStmtNode(size_t line, size_t col, AssignExpNode* assignExp)
	: StmtNode(line, col), myAssignExp(assignExp) {}
	void unparse(std::ostream& out, int indent) override;
private:
	AssignExpNode* myAssignExp;
};

class ReadStmtNode : public StmtNode{
public:
	ReadStmtNode(size_t line, size_t col, LValNode* lval)
	: StmtNode(line, col), myLVal(lval) {}
	void unparse(std::ostream& out, int indent) override;
private:
	LValNode* myLVal;
};

class WriteStmtNode : public StmtNode{
public:
	WriteStmtNode(size_t line, size_t col, ExpNode* exp)
	: StmtNode(line, col), myExp(exp) {}
	void unparse(std::ostream& out, int indent) override;
private:
	ExpNode* myExp;
};

class PostDecStmtNode : public StmtNode{
public:
	PostDecStmtNode(size_t line, size_t col, LValNode* lval)
	: StmtNode(line, col), myLVal(lval) {}
	void unparse(std::ostream& out, int indent) override;
private:
	LValNode* myLVal;
};

class PostIncStmtNode : public StmtNode{
public:
	PostIncStmtNode(size_t line, size_t col, LValNode* lval)
	: StmtNode(line, col), myLVal(lval) {}
	void unparse(std::ostream& out, int indent) override;
private:
	LValNode* myLVal;
};

class IfStmtNode : public StmtNode{
public:
	IfStmtNode(size_t line, size_t col, ExpNode* evalCond, std::list<StmtNode*>* body)
	: StmtNode(evalCond->line(), evalCond->col()), myCond(evalCond), myBody(body) {}
	void unparse(std::ostream& out, int indent) override;
private:
	ExpNode* myCond;
	std::list<StmtNode*>* myBody;
};

class IfElseStmtNode : public StmtNode{
public:
	IfElseStmtNode(size_t line, size_t col, ExpNode* evalCond, std::list<StmtNode*>* trueBranch, std::list<StmtNode*>* falseBranch)
	: StmtNode(evalCond->line(), evalCond->col()), myCond(evalCond), myTrueBranch(trueBranch), myFalseBranch(falseBranch) {}
	void unparse(std::ostream& out, int indent) override;
private:
	ExpNode* myCond;
	std::list<StmtNode*>* myTrueBranch;
	std::list<StmtNode*>* myFalseBranch;
};

class WhileStmtNode : public StmtNode{
public:
	WhileStmtNode(size_t line, size_t col, ExpNode* exp, std::list<StmtNode*>* body)
	: StmtNode(line, col), myExp(exp), myBody(body) {}
	void unparse(std::ostream& out, int indent) override;
private:
	ExpNode* myExp;
	std::list<StmtNode*>* myBody;
};

class ReturnStmtNode : public StmtNode{
public:
	ReturnStmtNode(size_t line, size_t col, ExpNode* exp)
	: StmtNode(line, col), myExp(exp) {}
	void unparse(std::ostream& out, int indent) override;
private:
	ExpNode* myExp;
};

class CallStmtNode : public StmtNode{
public:
	CallStmtNode(size_t line, size_t col, CallExpNode* callExp)
	: StmtNode(line, col), myCallExp(callExp) {}
	void unparse(std::ostream& out, int indent) override;
private:
	CallExpNode* myCallExp;
};

///////LValNode SUBCLASSES//////////////
////////////////////////////////////////
class IndexNode : public LValNode{
public:
	IndexNode(size_t l, size_t c, IDNode* baseSrc, ExpNode* offsetSrc)
	:LValNode(l,c), base(baseSrc), offset(offsetSrc){}
	void unparse(std::ostream& out, int indent) override;
private:
	IDNode* base;
	ExpNode* offset;
};


} //End namespace crona

#endif
