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

  for (auto &MBB : MF) {
    // outs() << "Entering MBB\n";
    for (auto I = MBB.rbegin(); I != MBB.rend();) {
      auto &MI = *I;
      //      outs() << MI;
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
        DebugLoc DL;
        MachineInstrBuilder MIB;
        MachineInstrBuilder SourceMIB;
        MIB =
            BuildMI(
                MBB, MI, DL,
                TII->get(
                    RISCV::BMOVT_J)) // bne s1, s0, LBB0_4 to bmovt bt0, LBB0_4
                .addReg(DestReg_BPR_T, RegState::Define)
                .addMBB(MI.getOperand(2).getMBB());
        MIB.getInstr()->setBMOVIndex(bmov_index);

        SourceMIB = BuildMI(
                  MBB, MI, DL,
                  TII->get(RISCV::BMOVS_J)) // This currently takes in the
                                            // target label, but it should take
                                            // the source label eventually.
                  .addReg(DestReg_BPR_S, RegState::Define);
        MIB.getInstr()->setBMOVIndex(bmov_index);

        unsigned int BMOVOpcode = RISCV::BMOVC_BNE; // Default, can delete later

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

        auto PB_MBB = MBB.splitAt(MBB.back());

        MIB = BuildMI(*PB_MBB, PB_MBB->end(), DL, TII->get(RISCV::PB))
                  .addReg(DestReg_BPR_S)
                  .addReg(DestReg_BPR_T)
                  .addReg(DestReg_BPR_C);
        MIB.getInstr()->setBMOVIndex(bmov_index);
        SourceMIB.addMBB(PB_MBB);

        MI.eraseFromBundle();
        I = nI;
        bmov_index++;
        // break;
      } else if (MI.isUnconditionalBranch()) {
        auto nI = std::next(I);
        const auto &STI = MF.getSubtarget<RISCVSubtarget>();
        DebugLoc DL;
        const RISCVInstrInfo *TII = STI.getInstrInfo();

        Register DestReg_BPR_T =
            MF.getRegInfo().createVirtualRegister(&RISCV::BPR_TRegClass);
        Register DestReg_BPR_S =
            MF.getRegInfo().createVirtualRegister(&RISCV::BPR_SRegClass);
        Register DestReg_BPR_C =
            MF.getRegInfo().createVirtualRegister(&RISCV::BPR_CRegClass);
        MachineInstrBuilder MIB;

        MIB =
            BuildMI(
                MBB, MI, DL,
                TII->get(
                    RISCV::BMOVT_J)) // bne s1, s0, LBB0_4 to bmovt bt0, LBB0_4
                .addReg(DestReg_BPR_T, RegState::Define)
                .addMBB(MI.getOperand(2).getMBB());
        MIB.getInstr()->setBMOVIndex(bmov_index);

        MIB = BuildMI(
                  MBB, MI, DL,
                  TII->get(RISCV::BMOVS_J)) // This currently takes in the
                                            // target label, but it should take
                                            // the source label eventually.
                  .addReg(DestReg_BPR_S, RegState::Define)
                  .addMBB(MI.getParent());
        MIB.getInstr()->setBMOVIndex(bmov_index);

        MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::BMOVUC))
                  .addReg(DestReg_BPR_C, RegState::Define);
        MIB.getInstr()->setBMOVIndex(bmov_index);

        switch (MI.getOpcode()) {
        case RISCV::JAL:
          MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::PB))
                  .addReg(MI.getOperand(0).getReg())
                  .addReg(DestReg_BPR_S)
                  .addReg(DestReg_BPR_C)
                  .addReg(DestReg_BPR_T);
          break;
        case RISCV::JALR:
                  MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::PB))
                  .addReg(MI.getOperand(0).getReg())
                  .addReg(DestReg_BPR_S)
                  .addReg(DestReg_BPR_C)
                  .addReg(DestReg_BPR_T);
          break;
        default:
          outs() << "Unknown branch opcode!";
          break;
        }
      } else {
        I = std::next(I);
      }
    }
    // outs() << "Contents of MachineBasicBlock:\n";
    // outs() << MBB << "\n";
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