#include <ostream>
#include "3ac.hpp"
#include "err.hpp"

namespace lake{

void IRProgram::allocGlobals(){
	std::map<SemSymbol*, SymOpd*>::iterator globItr = globals.begin();
	while(globItr != globals.end()){
		globItr->second->setMemoryLoc("(gbl_" + globItr->first->getName() + ")");
		++globItr;
	}
	HashMap<AuxOpd*, std::string>::iterator strItr = strings.begin();
	while(strItr != strings.end()){
		strItr->first->setMemoryLoc("(" + strItr->first->getName() + ")");
		++strItr;
	}
	//may need to iterate over strings and give them memLocs too
}

void IRProgram::datagenX64(std::ostream& out){
	std::map<SemSymbol*, SymOpd*>::iterator globItr = globals.begin();
	while(globItr != globals.end()){
		std::string name = globItr->first->getName();
		out << "gbl_" + name << ":\n\t.quad " << 0 << "\n";
		++globItr;
	}
	HashMap<AuxOpd*, std::string>::iterator strItr = strings.begin();
	while(strItr != strings.end()){
		std::string name = strItr->first->getName();
		out <<  name << ":\n\t.asciz " << strItr->second << "\n";
		++strItr;
	}
	out << ".align 8\n";
	allocGlobals();
	for(auto procItr : procs){
		procItr->allocLocals();
		procItr->toX64(out);
	}
	//do some sort of exit/epilogue for main
	
	// std::list<Procedure *>::iterator listProcItr = procs.begin();
	// while(listProcItr != procs.end()){
	// 	(*listProcItr)->allocLocals();
	// 	++listProcItr;
	// }
	
}

void IRProgram::toX64(std::ostream& out){
	out << ".data\n";
	this->datagenX64(out);
}

void Procedure::allocLocals(){
	int loc = 16;
	std::map<SemSymbol* , SymOpd*>::iterator localItr = locals.begin();
	while(localItr != locals.end()){
		localItr->second->setMemoryLoc("-" + to_string(loc) + "(%rbp)");
		loc += 8;
		++localItr;
	}
}

void Procedure::toX64(std::ostream& out){
	//Allocate all locals(given by drew)
	//allocLocals();

	out << "fun_" << myName << ":" << "\n";

	enter->codegenX64(out);
	for (auto quad : bodyQuads){
		quad->codegenLabels(out);
		quad->codegenX64(out);
	}
	leave->codegenLabels(out);
	leave->codegenX64(out);
}

void Quad::codegenLabels(std::ostream& out){
	if (labels.empty()){ return; }

	size_t numLabels = labels.size();
	size_t labelIdx = 0;
	for ( Label * label : labels){
		out << label->toString() << ": \n";
		if (labelIdx != numLabels - 1){ out << "\n"; }
		labelIdx++;
	}
}

void BinOpQuad::codegenX64(std::ostream& out){
	switch(op){
		case ADD:
				src1->genLoad(out, "%rax");
				src2->genStore(out, "%rbx");
				out << "addq %rbx, %rax\n";
				break;
		case SUB:
				src1->genLoad(out, "%rax");
				src2->genLoad(out, "%rbx");
				out << "subq %rbx, %rax\n";
				break;
		case DIV:
				out << "movq $0, %rdx\n";
				src1->genLoad(out, "%rax");
				src2->genLoad(out, "%rbx");
				out << "idivq %rbx\n";
				dst->genStore(out, "%rax");
				//return;
				break;
		case MULT:
				src1->genLoad(out, "%rax");
				src2->genLoad(out, "%rbx");
				out << "imulq %rbx\n";
				dst->genStore(out, "%rax");
				//return;
				break;
		case OR:
				src1->genLoad(out, "%rax");
				src2->genLoad(out, "%rbx");
				out << "orq %rbx, %rax\n";
				break;
		case AND:
				src1->genLoad(out, "%rax");
				src2->genLoad(out, "%rbx");
				out << "andq %rbx, %rax\n";
				break;
		case EQ:
				src1->genLoad(out, "%rax");
				src2->genLoad(out, "%rbx");
				out << "cmpq %rbx, %rax\n";
				out << "sete %r15\n";
				break;
		case NEQ:
				src1->genLoad(out, "%rax");
				src2->genLoad(out, "%rbx");
				out << "cmpq %rbx, %rax\n";
				out << "setne %r15\n";
				break;
		case LT:
				src1->genLoad(out, "%rax");
				src2->genLoad(out, "%rbx");
				out << "cmpq %rbx, %rax\n";
				out << "setl %r15\n";
				break;
		case GT:
				src1->genLoad(out, "%rax");
				src2->genLoad(out, "%rbx");
				out << "cmpq %rbx, %rax\n";
				out << "setg %r15\n";
				break;
		case LTE:
				src1->genLoad(out, "%rax");
				src2->genLoad(out, "%rbx");
				out << "cmpq %rbx, %rax\n";
				out << "setle %r15\n";
				break;
		case GTE:
				src1->genLoad(out, "%rax");
				src2->genLoad(out, "%rbx");
				out << "cmpq %rbx, %rax\n";
				out << "setge %r15\n";
				break;
	}
}

void UnaryOpQuad::codegenX64(std::ostream& out){
	if(op == NEG){
		src->genLoad(out, "%rbx");
		out << "negq %rbx\n";
		dst->genStore(out, "%rbx");
	}
	else if(op == NOT){
		src->genLoad(out, "%rbx");
		out << "notq %rbx\n";
		dst->genStore(out, "%rbx");
	}
}

void AssignQuad::codegenX64(std::ostream& out){
	src->genLoad(out, "%rax");
	dst->genStore(out, "%rax");
}

void LocQuad::codegenX64(std::ostream& out){
	//nope
}

void JmpQuad::codegenX64(std::ostream& out){
	out << "jmp " << tgt->toString() << "\n";
}

void JmpIfQuad::codegenX64(std::ostream& out){
	
	out << "jmpif" << tgt->toString() << "\n";
}

void NopQuad::codegenX64(std::ostream& out){
	out << "nop" << "\n";
}

void SyscallQuad::codegenX64(std::ostream& out){
	if(mySyscall == READ){
		if(myArg->getType() == NUMERIC){
			//myArg->genLoad(out, "%rdi");
			out << "callq getInt\n";
			myArg->genStore(out, "%rax");
		}
		else{
			throw new InternalError("Cannot read strings.");
		}
	}
	else if(mySyscall == WRITE){
		if(myArg->getType() == NUMERIC){
			myArg->genLoad(out, "%rdi");
			out << "callq printInt\n";
		}
		else{
			myArg->genLoad(out, "%rdi");
			out << "callq printString\n";
		}
	}
	else if(mySyscall == EXIT){
		out << "movq $60, %rax\n";
		out << "movq $0, %rdi\n";
		out << "syscall\n";
	}
}

void CallQuad::codegenX64(std::ostream& out){
	out << "callq " << callee->getName() << "\n";
}

void EnterQuad::codegenX64(std::ostream& out){
	out << "subq $8, %rsp\n";
	out << "movq %rbp, (%rsp)\n";
	out << "movq %rsp, %rbp\n";
	out << "addq $16, %rbp\n";
	out << "subq $" << myProc->numLocals() * 8 << ", %rsp\n";
}

void LeaveQuad::codegenX64(std::ostream& out){
	out << "addq $" << myProc->numLocals() * 8 << ", %rsp\n";
	out << "movq (%rsp), %rbp\n";
	out << "addq $8, %rsp\n";
	out << "retq\n";
}

void SetInQuad::codegenX64(std::ostream& out){
	out << "subq $8, %rsp\n";
	opd->genLoad(out, "%rsp");
}

void GetInQuad::codegenX64(std::ostream& out){
	//We don't actually need to do anything here
}

void SetOutQuad::codegenX64(std::ostream& out){
	opd->genLoad(out, "%rax");
}

void GetOutQuad::codegenX64(std::ostream& out){
	opd->genStore(out, "%rax");
}

void SymOpd::genLoad(std::ostream & out, std::string regStr){
	out << "movq " << myLoc << ", " << regStr << "\n";
}

void SymOpd::genStore(std::ostream& out, std::string regStr){
	out << "movq " << regStr << ", " << myLoc << "\n";
}

void AuxOpd::genLoad(std::ostream & out, std::string regStr){
	out << "movq " << myLoc << ", " << regStr << "\n";
}

void AuxOpd::genStore(std::ostream& out, std::string regStr){
	out << "movq " << regStr << ", " << myLoc << "\n";
}

void LitOpd::genLoad(std::ostream & out, std::string regStr){
	// throw new InternalError("Cannot use literal as l-val");
	out << "movq $" << val << ", " << regStr << "\n";
}

void LitOpd::genStore(std::ostream& out, std::string regStr){
	throw new InternalError("Cannot use literal as l-val");
}

}
