#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "231DFA.h"
#include <deque>
#include <map>
#include <utility>
#include <vector>
#include <set>
using namespace llvm;
using namespace std;

class ReachingData: public Info{

private:
	set<unsigned> infoList;
public:
	ReachingData(){};
	ReachingData(set<unsigned> inputList){
		this->infoList = inputList;
	}


	set<unsigned> & getInfoList()
    {
        return this->infoList;
    }

    void seInfoList(set<unsigned> information)
    {
        this->infoList = information;
    }


    static bool equal(ReachingData * info1, ReachingData * info2){
    	return (info1->getInfoList() == info2->getInfoList());
    }

    static ReachingData* join(ReachingData * info1, ReachingData * info2, ReachingData * result){
    	set<unsigned> tmp1 = info1->getInfoList();
    	set<unsigned> tmp2 = info2->getInfoList();
    	tmp1.insert(tmp2.begin(),tmp.end());
    	result.setInfoList(tmp1);
    	return result;
    }
};


class ReachingDefinationAnalysis : public DataFlowAnalysis<ReachingData, true>{ //modify the template class by passing parameters

private:
	typedef std::pair<unsigned, unsigned> Edge;
	std::map<Edge, ReachingData *> EdgeToInfo;

public:
	ReachingDefinationAnalysis(ReachingData & bottom, ReachingData & initialState):
		 // Bottom(bottom), InitialState(initialState),EntryInstr(nullptr) {}
	DataFlowAnalysis(bottom, initialState){}
	/*
     * The flow function.
     *   Instruction I: the IR instruction to be processed.
     *   std::vector<unsigned> & IncomingEdges: the vector of the indices of the source instructions of the incoming edges.
     *   std::vector<unsigned> & IncomingEdges: the vector of indices of the source instructions of the outgoing edges.
     *   std::vector<Info *> & Infos: the vector of the newly computed information for each outgoing eages.
     */

	// implement the virtual function of parent class here as a child class function. (Though it's actually used in the parent class function, not in child class function)
	void flowfunction(Instruction * I,
					  std::vector<unsigned> & IncomingEdges,
					  std::vector<unsigned> & OutgoingEdges,
					  std::vector<Info *> & Infos){

		string opname = I->getOpcodeName();
		ReachingData* outInfo = new ReachingData();
		unsigned ind = InstrToIndex[I];

		for(unsigned i=0; i<IncomingEdges.size();i++){
			Edge inEdge=make_pair(IncomingEdges[i],ind);
			ReachingData::join(outInfo,inEdge,outInfo);
		}

		if(I->isBinaryOp()||opname=="alloca"||opname=="load"||opname=="select"||opname=="icmp"||opname=="fcmp"||opname=="getelementptr"){
			outInfo.infoList.push_back(index);
		}

		else if(opname=="phi"){
			Instruction * firstNonPhi = I->getParent()->getFirstNonPHI();
			unsigned phiInd = InstrToIndex[firstNonPhi];
			for(unsigned i=ind; i<phiInd; i++){
				outInfo.infoList.push_back(i)
			}
		}


		for(unsigned i=0; i<OutgoingEdges.size(); i++){
			Infos.push_back(outInfo);
		}
	}
};



namespace {
    struct CountStaticInstructions : public FunctionPass {
        static char ID;
        CountStaticInstructions() : FunctionPass(ID) {}
        
        bool runOnFunction(Function &F) override {
            ReachingData & bottom;
            ReachingData & initialState;
            ReachingDefinationAnalysis * myAnalysis = new ReachingDefinationAnalysis(bottom, initialState);
            myAnalysis->runWorklistAlgorithm( &F);  //public function in parent class
            myAnalysis->print(); // public functon in parent class
            return false;
            
        }
    }; // end of struct TestPass
}  // end of anonymous namespace





char CountStaticInstructions::ID = 0;
static RegisterPass<ReachingDefinitionAnalysis> X("cse231-reaching", "reaching definition analysis",false ,false);



