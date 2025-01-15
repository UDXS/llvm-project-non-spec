#include "RISCV.h"
#include "RISCVInstrInfo.h"
#include "RISCVSubtarget.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/MC/MCSymbol.h"
using namespace llvm;

//*******************************************************************************************************************************************
// This is a LLVM backend pass that right now looks for conditional branches,
// turns them into BMOV instructions and later deletes the branch. The BMOVC has
// two GPR operands which are the original operands used in the conditional
// branch it's replacing
//*******************************************************************************************************************************************
#define RISCV_BMOV_INSERTION_PASS_NAME "RISCV bmov insertion pass"

namespace {

class RISCVBMOVInsertion : public MachineFunctionPass {
public:
  static char ID;
  int bmov_index = 0;
  RISCVBMOVInsertion() : MachineFunctionPass(ID) {
    initializeRISCVBMOVInsertionPass(*PassRegistry::getPassRegistry());
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  StringRef getPassName() const override {
    return RISCV_BMOV_INSERTION_PASS_NAME;
  }
};

char RISCVBMOVInsertion::ID = 0;

bool RISCVBMOVInsertion::runOnMachineFunction(MachineFunction &MF) {

  for (auto BB = MF.rbegin(); BB != MF.rend();) {
    outs() << "Entering MBB\n";
    auto nBB = std::next(BB);
    auto &MBB = *BB;
    for (auto I = MBB.rbegin(); I != MBB.rend();) {
      auto &MI = *I;
      outs() << MI << "\n Branch Info: " <<  MI.isConditionalBranch() << MI.isUnconditionalBranch() << MI.isCall() << "\n";
      if (MI.isConditionalBranch()) { // Is the instruction a bne/beq/blt
        auto nI = std::next(I);
        const auto &STI = MF.getSubtarget<RISCVSubtarget>();
        const RISCVInstrInfo *TII = STI.getInstrInfo();
        Register DestReg_BPR_T =
            MF.getRegInfo().createVirtualRegister(&RISCV::BPR_TRegClass);
        Register DestReg_BPR_S =
            MF.getRegInfo().createVirtualRegister(&RISCV::BPR_SRegClass);
        Register DestReg_BPR_C =
            MF.getRegInfo().createVirtualRegister(&RISCV::BPR_CRegClass);
        Register DestReg_BPR =
            MF.getRegInfo().createVirtualRegister(&RISCV::BPRRegClass);

        DebugLoc DL;
        MachineInstrBuilder MIB;
        MachineInstrBuilder SourceMIB;

        // Unify BP Register
        MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::REG_SEQUENCE), DestReg_BPR)
                  .addReg(DestReg_BPR_T)
                  .addImm(RISCV::BPR_TRegClassID)
                  .addReg(DestReg_BPR_S)
                  .addImm(RISCV::BPR_SRegClassID)
                  .addReg(DestReg_BPR_C)
                  .addImm(RISCV::BPR_CRegClassID);

        MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::BMOVT_J))
                  .addReg(DestReg_BPR_T, RegState::Define)
                  .addMBB(MI.getOperand(2).getMBB());
        MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::BMOVS_J))
                  .addReg(DestReg_BPR_S, RegState::Define);
        SourceMIB = MIB;

        unsigned int BMOVOpcode = RISCV::BMOVC_BNE;

        switch (MI.getOpcode()) {
        case RISCV::BNE:
          BMOVOpcode = RISCV::BMOVC_BNE;
          break;
        case RISCV::BEQ:
          BMOVOpcode = RISCV::BMOVC_BEQ;
          break;
        case RISCV::BLT:
          BMOVOpcode = RISCV::BMOVC_BLT;
          break;
        case RISCV::BGE:
          BMOVOpcode = RISCV::BMOVC_BGE;
          break;
        case RISCV::BLTU:
          BMOVOpcode = RISCV::BMOVC_BLTU;
          break;
        case RISCV::BGEU:
          BMOVOpcode = RISCV::BMOVC_BGEU;
          break;
        default:
          outs() << "Unknown branch opcode!";
          break;
        }

        MIB = BuildMI(MBB, MI, DL, TII->get(BMOVOpcode))
                  .addReg(DestReg_BPR_C, RegState::Define)
                  .addReg(MI.getOperand(0).getReg())
                  .addReg(MI.getOperand(1).getReg());
        MIB.getInstr()->setBMOVIndex(bmov_index);

        auto PB_MBB = MBB.splitAt(MI);
        PB_MBB->setLabelMustBeEmitted();

        outs() << (MBB.succ_size()) << "\n";
        assert(MBB.getSingleSuccessor() == PB_MBB &&
               "PlaceholderBranch expected to follow MBB.");

        MIB = BuildMI(*PB_MBB, PB_MBB->front(), DL, TII->get(RISCV::PBAL))
                  .addReg(RISCV::X0)
                  .addReg(DestReg_BPR)
                  .addReg(RISCV::X0);
        SourceMIB.addMBB(PB_MBB);

        MI.eraseFromBundle();
        I = nI;
        bmov_index++;
        // break;

      } else if (MI.isUnconditionalBranch()) {
        
        auto nI = std::next(I);
        const auto &STI = MF.getSubtarget<RISCVSubtarget>();
        const RISCVInstrInfo *TII = STI.getInstrInfo();
        Register DestReg_BPR_T =
            MF.getRegInfo().createVirtualRegister(&RISCV::BPR_TRegClass);
        Register DestReg_BPR_S =
            MF.getRegInfo().createVirtualRegister(&RISCV::BPR_SRegClass);
        Register DestReg_BPR_C =
            MF.getRegInfo().createVirtualRegister(&RISCV::BPR_CRegClass);
        Register DestReg_BPR =
            MF.getRegInfo().createVirtualRegister(&RISCV::BPRRegClass);

        DebugLoc DL;
        MachineInstrBuilder MIB;
        MachineInstrBuilder SourceMIB;

        // Unify BP Register
        MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::REG_SEQUENCE), DestReg_BPR)
                  .addReg(DestReg_BPR_T)
                  .addImm(RISCV::BPR_TRegClassID)
                  .addReg(DestReg_BPR_S)
                  .addImm(RISCV::BPR_SRegClassID)
                  .addReg(DestReg_BPR_C)
                  .addImm(RISCV::BPR_CRegClassID);

        // We don't emit a BMOVC here but its vital that BMOV Target is set
        // after Source to enter the unconditional mode.
        MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::BMOVS_J))
                  .addReg(DestReg_BPR_S, RegState::Define);
        SourceMIB = MIB;

        switch (MI.getOpcode()) {
        // We must fixup JAL into a single
        case RISCV::JAL:
          MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::BMOVT_J))
                    .addReg(DestReg_BPR_T, RegState::Define)
                    .addMBB(MI.getOperand(1).getMBB());
          break;
        case RISCV::JALR:
          MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::BMOVT_I))
                    .addReg(DestReg_BPR_T, RegState::Define)
                    .addReg(MI.getOperand(1).getReg())
                    .addImm(MI.getOperand(2).getImm());
          break;
        default:
          outs() << "Unknown branch opcode!";
          break;
        }

        auto PB_MBB = MBB.splitAt(MI);
        PB_MBB->setLabelMustBeEmitted();

        outs() << (MBB.succ_size()) << "\n";
        assert(MBB.getSingleSuccessor() == PB_MBB &&
               "PlaceholderBranch expected to follow MBB.");

        MIB = BuildMI(*PB_MBB, PB_MBB->front(), DL, TII->get(RISCV::PBAL))
                  .addReg(MI.getOperand(0).getReg())
                  .addReg(DestReg_BPR)
                  .addReg(RISCV::X0);
        MIB.getInstr()->setBMOVIndex(bmov_index);
        SourceMIB.addMBB(PB_MBB);

        MI.eraseFromBundle();
        I = nI;
        bmov_index++;
        // break;
      } else {
        I = std::next(I);
      }
    }
    // outs() << "Contents of MachineBasicBlock:\n";
    // outs() << MBB << "\n";
    BB = nBB;
  }

  return true;
}

} // end of anonymous namespace

INITIALIZE_PASS(RISCVBMOVInsertion, "RISCV-BMOV-insertion",
                RISCV_BMOV_INSERTION_PASS_NAME,
                true, // is CFG only?
                true  // is analysis?
)

namespace llvm {

FunctionPass *createRISCVBMOVInsertionPass() {
  return new RISCVBMOVInsertion();
}

} // namespace llvm