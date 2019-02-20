#include "llvm/Pass.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"
#include "/lib231/lib231.cpp"
#include <map>
#include <vector>

using namespace llvm;
using namespace std;

namespace {
    struct CountDynamicInstructions : public FunctionPass {
        static char ID;
        CountDynamicInstructions() : FunctionPass(ID) {}
        
        bool runOnFunction(Function &F) override {
            Module *M = F.getParent();
            LLVMContext &Context = M->getContext();
            
            Function *updateInstrInfo = dyn_cast<Function>(M->getOrInsertFunction("updateInstrInfo", Type::getVoidTy(Context),Type::getInt32Ty(Context), Type::getInt32PtrTy(Context), Type::getInt32PtrTy(Context)));
            Function *printOutInstrInfo = dyn_cast<Function>(M->getOrInsertFunction("printOutInstrInfo", Type::getVoidTy(Context), NULL));
            
            // Iterate over all basic blocks
            for (Function::iterator blk = F.begin(); blk != F.end(); blk++) {
                // errs() << "Block " << blk->size() << "\n";
                map<int, int> dcounter;
                
                // Iterate over all instructions in a basic block
                for (BasicBlock::iterator ist = blk->begin(); ist != blk->end(); ist++) {
                    dcounter[ist->getOpcode()]++;
                }
                
                // Insert updateInstrInfo at the end of each basic block
                IRBuilder<> Builder(&*blk);
                Builder.SetInsertPoint(blk->getTerminator());
                
                // define the variable for call function
                int num = dcounter.size();
                vector<Value *> args;
                vector<Constant *> keys;
                vector<Constant *> values;
                
                // get the 'keys array' & 'values array' of the current basic block
                for (map<int, int>::iterator it = dcounter.begin(), end = dcounter.end(); it != end; ++it) {
                    keys.push_back(ConstantInt::get(Type::getInt32Ty(Context), it->first));
                    values.push_back(ConstantInt::get(Type::getInt32Ty(Context), it->second));
                }
                
                //define the data structure needs for call function
                ArrayType *array_type = ArrayType::get(Type::getInt32Ty(Context), num);
                Value* idx_list[2] = {ConstantInt::get(Type::getInt32Ty(Context), 0), ConstantInt::get(Type::getInt32Ty(Context), 0)};
                
                // put the 'keys array' & 'values array' into the data structure
                GlobalVariable *keys_global = new GlobalVariable(*M, array_type, true,
                                                                 GlobalVariable::InternalLinkage, ConstantArray::get(array_type, keys), "key_global");
                GlobalVariable *values_global = new GlobalVariable(*M, ArrayType::get(Type::getInt32Ty(Context), num), true,
                                                                   GlobalVariable::InternalLinkage, ConstantArray::get(array_type, values), "value_global");
                
                // push the compeleted data structure into the variable (args), and make call
                args.push_back(ConstantInt::get(Type::getInt32Ty(Context), num));
                args.push_back(Builder.CreateInBoundsGEP(keys_global, idx_list));
                args.push_back(Builder.CreateInBoundsGEP(values_global, idx_list));
                
                Builder.CreateCall(updateInstrInfo, args);
                
                //print the results
                for (BasicBlock::iterator ist = blk->begin(), end = blk->end(); ist != end; ++ist) {
                    if ((string) ist->getOpcodeName() == "ret") {
                        Builder.SetInsertPoint(&*ist);
                        Builder.CreateCall(printOutInstrInfo);
                    }
                }
            }
            
            return false;
        }
    }; // end of struct TestPass
}  // end of anonymous namespace

char CountDynamicInstructions::ID = 0;
static RegisterPass<CountDynamicInstructions> X("cse231-cdi", "Count Dynamic Instructions",
                                                false /* Only looks at CFG */,
                                                false /* Analysis Pass */);
