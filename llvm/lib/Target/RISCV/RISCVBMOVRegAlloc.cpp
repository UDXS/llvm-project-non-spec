#include "RISCV.h"
#include "RISCVInstrInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "RISCVSubtarget.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/CodeGen/AsmPrinter.h"
using namespace llvm;


//************************************************************************************************************************************************
// This is a LLVM backend pass that can be used to fix register allocation problems by setting the physical register index. (NOT IN USE CURRENTLY)
//************************************************************************************************************************************************
#define RISCV_BMOV_REGALLOC_PASS_NAME                                   \
  "RISCV bmov regalloc pass"

namespace {

class RISCVBMOVRegAlloc : public MachineFunctionPass {
public:
  static char ID;

  RISCVBMOVRegAlloc() : MachineFunctionPass(ID) {
    initializeRISCVBMOVRegAllocPass(*PassRegistry::getPassRegistry());
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  StringRef getPassName() const override {
    return RISCV_BMOV_REGALLOC_PASS_NAME;
  }
};

char RISCVBMOVRegAlloc::ID = 0;

bool RISCVBMOVRegAlloc::runOnMachineFunction(MachineFunction &MF) {

  for(auto &MBB : MF) {
    //outs() << "Entering MBB\n";
    for (auto &MI : MBB) {
      if (MI.getOpcode() == RISCV::BMOVT_J) {  
        MI.getOperand(0).setReg(Register(RISCV::BT0 + MI.getBMOVIndex()));
        std::cout << "HERE and " << MI.getBMOVIndex() << std::endl;
      }
      else if (MI.getOpcode() == RISCV::BMOVS_J) {  
        MI.getOperand(0).setReg(Register(RISCV::BS0 + MI.getBMOVIndex()));
      }
      else if (MI.getOpcode() == RISCV::BMOVC_BNE || MI.getOpcode() == RISCV::BMOVC_BEQ || MI.getOpcode() == RISCV::BMOVC_BLT || MI.getOpcode() == RISCV::BMOVC_BGE || MI.getOpcode() == RISCV::BMOVC_BLTU|| MI.getOpcode() == RISCV::BMOVC_BGEU) {
        MI.getOperand(0).setReg(Register(RISCV::BC0 + MI.getBMOVIndex()));
      }
      else if (MI.getOpcode() == RISCV::PB) {  
        MI.getOperand(0).setReg(Register(RISCV::BC0 + MI.getBMOVIndex()));
        MI.getOperand(1).setReg(Register(RISCV::BS0 + MI.getBMOVIndex()));
        MI.getOperand(2).setReg(Register(RISCV::BT0 + MI.getBMOVIndex()));
      }
    }
  }

  return true;
}

} // end of anonymous namespace

INITIALIZE_PASS(RISCVBMOVRegAlloc, "RISCV-BMOV-regalloc",
                RISCV_BMOV_REGALLOC_PASS_NAME,
                true, // is CFG only?
                true  // is analysis?
)

namespace llvm {

FunctionPass *createRISCVBMOVRegAllocPass() {
  return new RISCVBMOVRegAlloc();
}

} // namespace llvm