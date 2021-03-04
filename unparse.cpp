#include "ast.hpp"

namespace crona{

/*
doIndent is declared static, which means that it can
only be called in this file (its symbol is not exported).
*/
static void doIndent(std::ostream& out, int indent){
	for (int k = 0 ; k < indent; k++){ out << "\t"; }
}

/*
In this code, the intention is that functions are grouped
into files by purpose, rather than by class.
If you're used to having all of the functions of a class
defined in the same file, this style may be a bit disorienting,
though it is legal. Thus, we can have
ProgramNode::unparse, which is the unparse method of ProgramNodes
defined in the same file as DeclNode::unparse, the unparse method
of DeclNodes.
*/


void ProgramNode::unparse(std::ostream& out, int indent){
	/* Oh, hey it's a for-each loop in C++!
	   The loop iterates over each element in a collection
	   without that gross i++ nonsense.
	 */
	for (auto global : *myGlobals){
		/* The auto keyword tells the compiler
		   to (try to) figure out what the
		   type of a variable should be from
		   context. here, since we're iterating
		   over a list of DeclNode *s, it's
		   pretty clear that global is of
		   type DeclNode *.
		*/
		global->unparse(out, indent);
	}
}

void VarDeclNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	this->myId->unparse(out, 0);
	out << " : ";
	this->myType->unparse(out, 0);
	out << ";\n";
}

void FormalDeclNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	this->myId->unparse(out,0);
	out << " : ";
	this ->myType->unparse(out,0);
}

void FnDeclNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	this ->myType->unparse(out,0);
	out << " : ";
	this->myId->unparse(out,0);
	out << "(";

	for(auto formal : *formals){
			out << ", ";
			formal->unparse(out,0);
	}

	out << ")\n";

	for(auto stmt : *bodyVal){
		stmt->unparse(out, indent+1);
	}

	doIndent(out, indent);
	out << "}\n";
}

///////TYPENODE CLASSES////////////
///////////////////////////////////

void ArrayTypeNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	this->myType->unparse(out,0);
}

void BoolTypeNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "bool";
}

void ByteTypeNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "byte";
}

void IntTypeNode::unparse(std::ostream& out, int indent){
	doIndent(out,indent);
	out << "int";
}

void VoidTypeNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "void";
}

///////EXPNODE CLASSES//////////////
///////////////////////////////////

void AssignExpNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
}

void IDNode::unparse(std::ostream& out, int indent){
	doIndent(out,indent);
	out << this->myStrVal;
}

///////STMTNODE CLASSES/////////////
///////////////////////////////////

void AssignStmtNode::unparse(std::ostream& out, int indent){
		doIndent(out,indent);
		myAssignExp->unparse(out,0);
}

void ReadStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out,indent);
	myLVal->unparse(out,0);
}

void WriteStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myExp->unparse(out,0);
}

void PostDecStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out,indent);
	myLVal->unparse(out,0);
	out << "--\n";
}

void PostIncStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out,indent);
	myLVal->unparse(out,0);
	out << "++\n";
}

void IfStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out,indent);
	out << "if ( ";
	myCond->unparse(out,0);
	out << "){\n";
	for(auto state : *myBody)
	{
		state->unparse(out, indent+1);
	}
	doIndent(out, indent);
	out << "}";
}

void IfElseStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out,indent);
	out << "if (";
	myCond->unparse(out,0);
	out << "){\n";
	for(auto state : *myTrueBranch)
	{
		state->unparse(out, indent+1);
	}
	doIndent(out,indent);
	out << "}";
	out << "else{";
	for(auto state : *myFalseBranch)
	{
		state->unparse(out, indent+1);
	}
	doIndent(out, indent);
	out << "}";
}

void WhileStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out,indent);
	out << "while(";
	myExp->unparse(out,0);
	out << "){\n";
	for(auto state : *myBody)
	{
		state->unparse(out, indent+1);
	}
	doIndent(out, indent);
	out << "}";
}

void ReturnStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "return ";
	if(myExp != NULL)
	{
		out << " ";
		myExp->unparse(out,0);
	}
	out << ";\n";
}

void CallStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myCallExp->unparse(out,0);
	out << ";\n";
}


} // End namespace crona
