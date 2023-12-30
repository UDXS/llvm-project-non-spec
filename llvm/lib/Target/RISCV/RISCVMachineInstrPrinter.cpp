#include "RISCV.h"
#include "RISCVInstrInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "RISCVSubtarget.h"
using namespace llvm;

#define RISCV_MACHINEINSTR_PRINTER_PASS_NAME                                   \
  "Dummy RISCV machineinstr printer pass"

namespace {

class RISCVMachineInstrPrinter : public MachineFunctionPass {
public:
  static char ID;

  RISCVMachineInstrPrinter() : MachineFunctionPass(ID) {
    initializeRISCVMachineInstrPrinterPass(*PassRegistry::getPassRegistry());
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  StringRef getPassName() const override {
    return RISCV_MACHINEINSTR_PRINTER_PASS_NAME;
  }
};

char RISCVMachineInstrPrinter::ID = 0;

bool RISCVMachineInstrPrinter::runOnMachineFunction(MachineFunction &MF) {

  
      /*for (auto &MBB : MF) {
        outs() << "Contents of MachineBasicBlock:\n";
        outs() << MBB << "\n";
        const BasicBlock *BB = MBB.getBasicBlock();
        outs() << "Contents of BasicBlock corresponding to MachineBasicBlock:\n";
        outs() << BB << "\n";
      }*/
  for(auto &MBB : MF) {
    for (auto &MI : MBB) {
      if (MI.isConditionalBranch()) {
        const auto &STI = MF.getSubtarget<RISCVSubtarget>();
        const RISCVInstrInfo *TII = STI.getInstrInfo();
        Register DestReg_T = MF.getRegInfo().createVirtualRegister(&RISCV::BPR_TRegClass);
        Register DestReg_S = MF.getRegInfo().createVirtualRegister(&RISCV::BPR_SRegClass);
        Register DestReg_C1 = MF.getRegInfo().createVirtualRegister(&RISCV::GPRRegClass);
        Register DestReg_C11 = MF.getRegInfo().createVirtualRegister(&RISCV::GPRRegClass);
        Register DestReg_C2 = MF.getRegInfo().createVirtualRegister(&RISCV::BPR_CRegClass);
        //Register SrcReg = DestReg;

        DebugLoc DL;
        BuildMI(MBB, MI ,DL, TII->get(RISCV::BMOVT))
            .addReg(DestReg_T, RegState::Define)
            //.addReg(SrcReg)
            .addMBB(MI.getOperand(2).getMBB());

        BuildMI(MBB, MI ,DL, TII->get(RISCV::BMOVS))
            .addReg(DestReg_S, RegState::Define)
            //.addReg(SrcReg)
            .addMBB(MI.getOperand(2).getMBB());
        
        if (MI.getOpcode() == RISCV::BNE) {
          BuildMI(MBB, MI ,DL, TII->get(RISCV::SUB))
              .addReg(DestReg_C1, RegState::Define)
              .addReg(MI.getOperand(0).getReg())
              .addReg(MI.getOperand(1).getReg()); 

          BuildMI(MBB, MI ,DL, TII->get(RISCV::SLTIU))
              .addReg(DestReg_C11, RegState::Define)
              .addReg(RISCV::X0)
              .addReg(DestReg_C1);

          BuildMI(MBB, MI ,DL, TII->get(RISCV::BMOVC))
              .addReg(DestReg_C2, RegState::Define)
              .addReg(DestReg_C11)
              .addReg(RISCV::X0);

          BuildMI(MBB, MI ,DL, TII->get(RISCV::PB))
              .addReg(DestReg_T, RegState::Define)
              .addReg(DestReg_S)
              .addReg(DestReg_C2)
              .addReg(DestReg_T, RegState::Implicit);            
        }

        MI.eraseFromBundle();       
        return true;
      }
    }
  }

  return false;
}

} // end of anonymous namespace

INITIALIZE_PASS(RISCVMachineInstrPrinter, "RISCV-machineinstr-printer",
                RISCV_MACHINEINSTR_PRINTER_PASS_NAME,
                true, // is CFG only?
                true  // is analysis?
)

namespace llvm {

FunctionPass *createRISCVMachineInstrPrinterPass() {
  return new RISCVMachineInstrPrinter();
}

} // namespace llvm