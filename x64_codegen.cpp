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
		strItr->first->setMemoryLoc("(str_" + strItr->first->getName() + ")");
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
		out << "str_" << name << ":\n\t.asciz " << strItr->second << "\n";
		++strItr;
	}
	out << ".align 8\n";
	for(auto procItr : procs){
		procItr->allocLocals();
	}
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
		out << label->toString() << ": ";
		if (labelIdx != numLabels - 1){ out << "\n"; }
		labelIdx++;
	}
}

void BinOpQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void UnaryOpQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void AssignQuad::codegenX64(std::ostream& out){
	src->genLoad(out, "%rax");
	dst->genStore(out, "%rax");
}

void LocQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void JmpQuad::codegenX64(std::ostream& out){
	out << "jmp " << tgt->toString() << "\n";
}

void JmpIfQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void NopQuad::codegenX64(std::ostream& out){
	out << "nop" << "\n";
}

void SyscallQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void CallQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void EnterQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void LeaveQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void SetInQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void GetInQuad::codegenX64(std::ostream& out){
	//We don't actually need to do anything here
}

void SetOutQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void GetOutQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void SymOpd::genLoad(std::ostream & out, std::string regStr){
	TODO(Implement me)
}

void SymOpd::genStore(std::ostream& out, std::string regStr){
	TODO(Implement me)
}

void AuxOpd::genLoad(std::ostream & out, std::string regStr){
	TODO(Implement me)
}

void AuxOpd::genStore(std::ostream& out, std::string regStr){
	TODO(Implement me)
}

void LitOpd::genLoad(std::ostream & out, std::string regStr){
	TODO(Implement me)
}

void LitOpd::genStore(std::ostream& out, std::string regStr){
	throw new InternalError("Cannot use literal as l-val");
}

}
