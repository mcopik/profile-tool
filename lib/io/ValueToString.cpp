
#include "io/ValueToString.hpp"
#include "io/SCEVAnalyzer.hpp"
#include "util/util.hpp"

#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"
#include "llvm/Analysis/ScalarEvolution.h"

#include <ostream>

llvm::Optional<std::string> ValueToString::toString(const Constant * val)
{
  assert(val);
  if(const ConstantInt * integer = dyn_cast<ConstantInt>(val)) {
    return std::to_string(integer->getSExtValue());
  } else if(const GlobalVariable * var = dyn_cast<GlobalVariable>(val)) {
    return var->getName().str();
  } else if(const UndefValue * var = dyn_cast<UndefValue>(val)) {
    return llvm::Optional<std::string>();
  }
  else {
    //FIXME:
    return llvm::Optional<std::string>();
    //assert(!"Unknown type!");
  }
}

llvm::Optional<std::string> ValueToString::toString(const Argument * arg)
{
  assert(arg);
  const Function * f = arg->getParent();
  if(f->hasName()) {
    // remove leading underscores - some symbolic solvers (e.g. Matlab's) go crazy because of that
    std::string name = f->getName().str();
    auto idx = name.find_first_not_of("_");
    name = name.substr(idx, name.length() - idx + 1);
    return cppsprintf("%s_%d", name, arg->getArgNo());
  } else {
    return cppsprintf("unknown_function(%s)", arg->getArgNo());
  }
}

llvm::Optional<std::string> ValueToString::toString(Value * value)
{
    assert(value);
    //dbgs() << "Print: " << *value << "\n";
    const SCEV * scev = SE.isSCEVable(value->getType()) ?  SE.getSCEV(value) : nullptr;
    //if(scev)
     //   dbgs() << "Print SCEV: " << *scev << "\n";
    if(const Constant * val = dyn_cast<Constant>(value)) {
        return toString(val);
    } else if(const Argument * arg = dyn_cast<Argument>(value)) {
        return toString(arg);
    } else if(scev && scevPrinter.couldBeIV(scev)) {
        return scevPrinter.toString(scev, false);
    } else if(Instruction * instr = dyn_cast<Instruction>(value)) {
        return toString(instr);
    }
    else {
        if(value->hasName()) {
            return value->getName().str();
        } else {
            dbgs() << "Unknown name: " << *value << " " << value->getValueID() << "\n";
            // report lack of name
            assert(!"Name unknown!");
        }
    }
}

llvm::Optional<std::string> ValueToString::toString(Instruction * instr, bool exitsOnSuccess)
{
    dbgs() << instr << '\n';
    if(instr->isCast()) {
        //TODO: implement casts - for the momement ignore
        return toString(instr->getOperand(0));
    } else if(const BinaryOperator * op = dyn_cast<BinaryOperator>(instr)) {//const SCEV * scev = SE.getSCEV(value)) {
        //return scevPrinter.toString(scev);
        return toString(op);
    } else if(const ICmpInst * cmp_instr = dyn_cast<ICmpInst>(instr)) {
        return toString(cmp_instr, exitsOnSuccess);
    } else if(const LoadInst * load_instr = dyn_cast<LoadInst>(instr)) {
        //TODO: get debug information
        //std::string type;
        //raw_string_ostream os(type);
        if(const GetElementPtrInst * get_inst = dyn_cast<GetElementPtrInst>(load_instr->getOperand(0))) {
            return toString(get_inst);
        } else if(ConstantExpr * const_expr = dyn_cast<ConstantExpr>(load_instr->getOperand(0))) {
            //dbgs() << *const_expr << " " << const_expr->isGEPWithNoNotionalOverIndexing() << "\n";
            //FIXME:
            //assert(const_expr->isGEPWithNoNotionalOverIndexing());
            if(!const_expr->isGEPWithNoNotionalOverIndexing())
                return llvm::Optional<std::string>();
            Value * x = const_expr->getOperand(0);

    //        dbgs() << dyn_cast<GlobalVariable>(x)->getName() << " " << isa<ConstantArray>(x) << " " << *const_expr->getOperand(1) << "\n";
            //return toString(dyn_cast<GetElementPtrInst>(const_expr->getAsInstruction()));
          return cppsprintf("%s_%d", toString(x), dyn_cast<ConstantInt>(const_expr->getOperand(2))->getUniqueInteger().getSExtValue());
        }
        return load_instr->getOperand(0)->getName().str();//->print(os);
        //return os.str();
    } else if(const PHINode * phi = dyn_cast<PHINode>(instr)) {
        std::string str;
        llvm::raw_string_ostream ss(str);
        phi->print(ss);
        log << "Detected unsolvable PHI: " << ss.str();
        const DebugLoc & loc = phi->getDebugLoc();
        auto * scope = phi->getFunction()->getSubprogram();
        if(loc) {
            log << "; file: " << loc.get()->getFilename().str() << " line: " << loc.getLine() << "\n";
        } else if (scope) {
            log << "; function: " << scope->getName().str() << " file: " << scope->getFilename().str() << " line: " << scope->getLine() << "\n";
        } else {
            log << ";\n";
        }
        return llvm::Optional<std::string>();
    } else if(const CallInst * invoke = dyn_cast<CallInst>(instr)) {
        //dbgs() << "Function: " << invoke->getOperand(0) << "\n";
        // TODO: process simple function calls
        return llvm::Optional<std::string>();
    }
    //FIXME:
    return llvm::Optional<std::string>();
    //assert(!"Unknown instr type!");
}

llvm::Optional<std::string> ValueToString::toString(const ICmpInst * integer_comparison, bool exitOnSuccess)
{
    //dbgs() << *integer_comparison << "\n";
    std::string val;
    // Get negation!
    switch (integer_comparison->getPredicate()) {
        case CmpInst::ICMP_EQ:
            val += exitOnSuccess ? " ~= " : " == ";
            break;
        case CmpInst::ICMP_NE:
            val += exitOnSuccess ? " == " : " ~= ";
            break;
        case CmpInst::ICMP_UGT:
        case CmpInst::ICMP_SGT:
            val += exitOnSuccess ? " <= " : " > ";
            break;
        case CmpInst::ICMP_UGE:
        case CmpInst::ICMP_SGE:
            val += exitOnSuccess ? " < " : " >= ";
            break;
        case CmpInst::ICMP_ULT:
        case CmpInst::ICMP_SLT:
            val += exitOnSuccess ? " >= " : " < ";
            break;
        case CmpInst::ICMP_ULE:
        case CmpInst::ICMP_SLE:
            val += exitOnSuccess ? " > " : " <= ";
            break;
    }
    return cppsprintf("%s%s%s", toString(integer_comparison->getOperand(0)), val, toString(integer_comparison->getOperand(1)));
}

llvm::Optional<std::string> ValueToString::toString(const BinaryOperator * op)
{
    llvm::Optional<std::string> op1 = toString(op->getOperand(0));
    llvm::Optional<std::string> op2 = toString(op->getOperand(1));
    std::string op_str;
    switch(op->getOpcode())
    {
        case BinaryOperator::FMul:
        case BinaryOperator::Mul:
            op_str = " * ";
            break;
        case BinaryOperator::FAdd:
        case BinaryOperator::Add:
            op_str = " + ";
            break;
        case BinaryOperator::FSub:
        case BinaryOperator::Sub:
            op_str = " - ";
            break;
        case BinaryOperator::FDiv:
            op_str = " / ";
            break;
        // TODO: verify it is indeed multiplication by two?
        case BinaryOperator::Shl:
            op_str = " * ";
            op2 = cppsprintf("( 2^(%s) )", op2);
            break;
        default:
            //FIXME:
            //assert(!"Unknown binary operator!");
            return llvm::Optional<std::string>();
    }
    return cppsprintf("%s%s%s", op1, op_str, op2);
}

llvm::Optional<std::string> ValueToString::toString(const GetElementPtrInst * get)
{
    //dbgs() << get->getNumOperands() << "\n";
    //assert(false);
    //FIXME:
    return llvm::Optional<std::string>();
}