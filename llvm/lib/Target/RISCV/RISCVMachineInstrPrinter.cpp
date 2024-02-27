#include "RISCV.h"
#include "RISCVInstrInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "RISCVSubtarget.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/CodeGen/AsmPrinter.h"
using namespace llvm;

#define RISCV_MACHINEINSTR_PRINTER_PASS_NAME                                   \
  "RISCV machineinstr printer pass"

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

  //return true;
      /*for (auto &MBB : MF) {
        outs() << "Contents of MachineBasicBlock:\n";
        outs() << MBB << "\n";
        const BasicBlock *BB = MBB.getBasicBlock();
        outs() << "Contents of BasicBlock corresponding to MachineBasicBlock:\n";
        outs() << BB << "\n";
      }*/
  for(auto &MBB : MF) {
    //outs() << "Entering MBB\n";
    for (auto &MI : MBB) {
      if (MI.isConditionalBranch()) {
        const auto &STI = MF.getSubtarget<RISCVSubtarget>();
        const RISCVInstrInfo *TII = STI.getInstrInfo();
        Register DestReg_BPR_T = MF.getRegInfo().createVirtualRegister(&RISCV::BPR_TRegClass);
        Register DestReg_BPR_S = MF.getRegInfo().createVirtualRegister(&RISCV::BPR_SRegClass);
        Register DestReg_GPR_SUB = MF.getRegInfo().createVirtualRegister(&RISCV::GPRRegClass);
        Register DestReg_GPR_SET = MF.getRegInfo().createVirtualRegister(&RISCV::GPRRegClass);
        Register DestReg_BPR_C = MF.getRegInfo().createVirtualRegister(&RISCV::BPR_CRegClass);
        
        //Register SrcReg = DestReg;

        //outs() <<  "In Instr printer " << MI.getOperand(2).getBlockAddress() << "\n";
        //outs() << "DONE\n";

        DebugLoc DL;
        BuildMI(MBB, MI ,DL, TII->get(RISCV::BMOVT))
            .addReg(DestReg_BPR_T, RegState::Define)
            //.addReg(SrcReg)
            .addMBB(MI.getOperand(2).getMBB());

        BuildMI(MBB, MI ,DL, TII->get(RISCV::BMOVS))
            .addReg(DestReg_BPR_S, RegState::Define)
            //.addReg(SrcReg)
            .addMBB(MI.getOperand(2).getMBB());
        
        if (MI.getOpcode() == RISCV::BNE) {
          BuildMI(MBB, MI ,DL, TII->get(RISCV::SUB))
              .addReg(DestReg_GPR_SUB, RegState::Define)
              .addReg(MI.getOperand(0).getReg())
              .addReg(MI.getOperand(1).getReg()); 

          BuildMI(MBB, MI ,DL, TII->get(RISCV::SLTIU))
              .addReg(DestReg_GPR_SET, RegState::Define)
              .addReg(RISCV::X0)
              .addReg(DestReg_GPR_SUB);

          BuildMI(MBB, MI ,DL, TII->get(RISCV::BMOVC))
              .addReg(DestReg_BPR_C, RegState::Define)
              .addReg(DestReg_GPR_SET)
              .addImm(0x0);

          BuildMI(MBB, MI ,DL, TII->get(RISCV::PB))
              //.addReg(DestReg_BPR_T, RegState::Define)
              .addReg(DestReg_BPR_C)
              .addReg(DestReg_BPR_S)
              .addReg(DestReg_BPR_T);
          MI.eraseFromBundle();            
        }
        else if (MI.getOpcode() == RISCV::BEQ) {
          BuildMI(MBB, MI ,DL, TII->get(RISCV::SUB))
              .addReg(DestReg_GPR_SUB, RegState::Define)
              .addReg(MI.getOperand(0).getReg())
              .addReg(MI.getOperand(1).getReg()); 

          BuildMI(MBB, MI ,DL, TII->get(RISCV::SLTIU))
              .addReg(DestReg_GPR_SET, RegState::Define)
              .addReg(DestReg_GPR_SUB)
              .addImm(0x1);

          BuildMI(MBB, MI ,DL, TII->get(RISCV::BMOVC))
              .addReg(DestReg_BPR_C, RegState::Define)
              .addReg(DestReg_GPR_SET)
              .addImm(0x1);


          BuildMI(MBB, MI ,DL, TII->get(RISCV::PB))
              //.addReg(DestReg_BPR_T, RegState::Define)
              .addReg(DestReg_BPR_C)
              .addReg(DestReg_BPR_S)
              .addReg(DestReg_BPR_T);
          MI.eraseFromBundle();         
        }
        else if (MI.getOpcode() == RISCV::BLT) {
          BuildMI(MBB, MI ,DL, TII->get(RISCV::SUB))
              .addReg(DestReg_GPR_SUB, RegState::Define)
              .addReg(MI.getOperand(0).getReg())
              .addReg(MI.getOperand(1).getReg()); 

          BuildMI(MBB, MI ,DL, TII->get(RISCV::SLTI))
              .addReg(DestReg_GPR_SET, RegState::Define)
              .addReg(DestReg_GPR_SUB)
              .addImm(0x0);

          BuildMI(MBB, MI ,DL, TII->get(RISCV::BMOVC))
              .addReg(DestReg_BPR_C, RegState::Define)
              .addReg(DestReg_GPR_SET)
              .addImm(0x2);


          BuildMI(MBB, MI ,DL, TII->get(RISCV::PB))
              //.addReg(DestReg_BPR_T, RegState::Define)
              .addReg(DestReg_BPR_C)
              .addReg(DestReg_BPR_S)
              .addReg(DestReg_BPR_T);
          MI.eraseFromBundle();            
        }
        //MI.eraseFromBundle();       
        //return true;
        break;
      }
    }
    //outs() << "Contents of MachineBasicBlock:\n";
    //outs() << MBB << "\n";
  }

  return true;
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