


#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"

using namespace llvm;
using namespace std;

namespace {
    struct CountStaticInstructions : public FunctionPass {
        static char ID;
        CountStaticInstructions() : FunctionPass(ID) {}
        
        bool runOnFunction(Function &F) override {
            map< string, int> scount;
            for (inst_iterator p=inst_begin(F); p!=inst_end(F); p++){
                scount[p->getOpcodeName()]++;
            }
            
            for (map<string, int>::iterator q=scount.begin(); q!=scount.end(); q++){
                errs() << q->first << '\t' << q->second <<'\n';
            }
            
            return false;
            
        }
    }; // end of struct TestPass
}  // end of anonymous namespace





char CountStaticInstructions::ID = 0;
static RegisterPass<CountStaticInstructions> X("cse231-csi", "Collecting Static Instruction Counts",
                                false /* Only looks at CFG */,
                                false /* Analysis Pass */);
