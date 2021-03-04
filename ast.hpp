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
	: l(lineIn), c(colIn){
	}
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
	: ASTNode(1, 1), myGlobals(globalsIn){
	}
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
	: ASTNode(line, col) {
	}

	void unparse(std::ostream& out, int indent) override = 0;
};

class StmtNode : public ASTNode{

public:
	StmtNode(size_t l, size_t c) : ASTNode(l,c){}

private:

};

class ExpNode : public ASTNode{
protected:
	ExpNode(size_t line, size_t col)
	: ASTNode(line, col){
	}
};

class TypeNode : public ASTNode{
protected:
	TypeNode(size_t lineIn, size_t colIn)
	: ASTNode(lineIn, colIn){
	}
public:
	virtual void unparse(std::ostream& out, int indent) = 0;
	//TODO: consider adding an isRef to use in unparse to
	// indicate if this is a reference type
};

class LValNode : public ExpNode{
public:
		LValNode(size_t l, size_t c)
		: ExpNode(l,c){}
private:
};

class IDNode : public LValNode{
public:
	IDNode(IDToken * token)
	: LValNode(token->line(), token->col()){
	}
	void unparse(std::ostream& out, int indent);
private:
	std::string myStrVal;
};

class VarDeclNode : public DeclNode{
public:
	VarDeclNode(size_t l, size_t c, TypeNode * type, IDNode * id)
	: DeclNode(type->line(), type->col()), myType(type), myId(id){
	}
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
private:
	TypeNode* myType;
	IDNode* myId;
};

class FnDeclNode : public DeclNode{
public:
	FnDeclNode(size_t l, size_t c, TypeNode* type, IDNode* id, std::list<FormalDeclNode*>* params, std::list<StmtNode*>* body)
			:  DeclNode(type->line(), type->col()), myType(type), myId(id){
			}
private:
	TypeNode* myType;
	IDNode* myId;
};


///////STMTNODE CLASSES////////////
///////////////////////////////////

class IfElseStmtNode : public StmtNode{
public:
			IfElseStmtNode(size_t l, size_t c, ExpNode* eval, std::list<StmtNode*>* trueBranch, std::list<StmtNode*>* falseBranch)
				: StmtNode(eval->line(), eval->col()), myCond(eval), myTrueBranch(trueBranch), myFalseBranch(falseBranch) {}
private:
			ExpNode* myCond;
			std::list<StmtNode*>* myTrueBranch;
			std::list<StmtNode*>* myFalseBranch;
};

class IfStmtNode : public StmtNode{
public:
	IfStmtNode(size_t l, size_t c, ExpNode* eval, std::list<StmtNode*>* body)
			: StmtNode(eval->line(), eval->col()), myCond(eval), myBody(body) {}
private:
			ExpNode* myCond;
			std::list<StmtNode*>* myBody;
};

class PostDecStmtNode : public StmtNode{

};

class PostIncStmtNode : public StmtNode{

};

class ReadStmtNode : public StmtNode{

};

class ReturnStmtNode : public StmtNode{

};

class whileStmtNode : public StmtNode{

};

class WriteStmtNode : public StmtNode{

};

class AssignStmtNode : public StmtNode{

};

class CallStmtNode : public StmtNode{

};


///////TYPENODE CLASSES////////////
///////////////////////////////////

class ArrayTypeNode : public TypeNode{

};

class BoolTypeNode : public TypeNode{

};

class ByteTypeNode : public TypeNode{

};

class IntTypeNode : public TypeNode{
public:
	IntTypeNode(size_t lineIn, size_t colIn)
	: TypeNode(lineIn, colIn){
	}
	void unparse(std::ostream& out, int indent);
};
class VoidTypeNode : public TypeNode{

};



///////EXPNODE CLASSES//////////////
///////////////////////////////////

class AssignExpNode : public ExpNode{

};

class BinaryExpNode : public ExpNode{

};

class CallExpNode : public ExpNode{

};

class FalseNode : public ExpNode{

};

class HavocNode : public ExpNode{

};

class IntLitNode : public ExpNode{

};



class StrLitNode : public ExpNode{

};

class TrueNode : public ExpNode{

};

class UnaryExpNode : public ExpNode{

};

///////BINARYEXPNODE SUBCLASSES//////////////
////////////////////////////////////////////

class AndNode : public BinaryExpNode{

};

class DivideNode : public BinaryExpNode{

};

class EqualsNode : public BinaryExpNode{

};

class GreateEqNode : public BinaryExpNode{

};

class GreaterNode : public BinaryExpNode{

};

class LessEqNode : public BinaryExpNode{

};

class MinusNode : public BinaryExpNode{

};

class NotEqualsNode : public BinaryExpNode{

};

class OrNode : public BinaryExpNode{

};

class PlusNode : public BinaryExpNode{

};

class TimesNode : public BinaryExpNode{

};

} //End namespace crona

#endif
