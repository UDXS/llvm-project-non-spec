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
    for (auto I = MBB.begin(); I != MBB.end();) {
      auto &MI = *I;
      if (MI.isConditionalBranch()) { // Is the instruction a bne/beq/blt
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
        MIB =
            BuildMI(
                MBB, MI, DL,
                TII->get(
                    RISCV::BMOVT_J)) // bne s1, s0, LBB0_4 to bmovt bt0, LBB0_4
                .addReg(DestReg_BPR_T, RegState::Define)
                .addMBB(MI.getOperand(2).getMBB());
        MIB.getInstr()->setBMOVIndex(bmov_index);

        MIB =
            BuildMI(MBB, MI, DL,
                    TII->get(
                        RISCV::BMOVS_J)) // This currently takes in the target
                                         // label, but it should take the source
                                         // label eventually.
                .addReg(DestReg_BPR_S, RegState::Define)
                .addMBB(MI.getOperand(2).getMBB());
        MIB.getInstr()->setBMOVIndex(bmov_index);

        if (MI.getOpcode() == RISCV::BNE) {
          /*
            BNE xA, xB, imm
            =>
            SUB xC, xA, xB ; Will be 0 if equal
            SLT xD, x0, xC ; In equal case: 0 is not less than 0, so result 0.
            Else, result 1. ; The above forms an inequality check. BMOVC_NE BPxC

            ; NE tag purely for IR annotation
            ;(We should probably  move this to a flag in RISCVInstrInfo)
          */

          Register Comparator =
              MF.getRegInfo().createVirtualRegister(&RISCV::GPRRegClass);
          Register ResultBit =
              MF.getRegInfo().createVirtualRegister(&RISCV::GPRRegClass);

          MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::SUB))
                    .addReg(Comparator, RegState::Define)
                    .addReg(MI.getOperand(0).getReg())
                    .addReg(MI.getOperand(1).getReg());
          MIB.getInstr()->setBMOVIndex(bmov_index);

          MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::SLTU))
                    .addReg(ResultBit, RegState::Define)
                    .addReg(RISCV::X0)
                    .addReg(Comparator);
          MIB.getInstr()->setBMOVIndex(bmov_index);

          MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::BMOVC_NE))
                    .addReg(DestReg_BPR_C, RegState::Define)
                    .addReg(ResultBit)
                    .addReg(MI.getOperand(0).getReg())
                    .addReg(MI.getOperand(1).getReg());
          // Last two operands are only for analyzeBranch.
          MIB.getInstr()->setBMOVIndex(bmov_index);

        } else if (MI.getOpcode() == RISCV::BEQ) {

          /*
          BEQ xA, xB, imm
          =>
          SUB xC, xA, xB ; Will be 0 if equal
          SLTIU xD, xC, 0x1 ; In equal case: xc = 0 is less than 1, so result 1.
          Else, result 0. ; the above forms an equality check. BMOVC_EQ BPxC
        */

          Register Comparator =
              MF.getRegInfo().createVirtualRegister(&RISCV::GPRRegClass);
          Register ResultBit =
              MF.getRegInfo().createVirtualRegister(&RISCV::GPRRegClass);

          MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::SUB))
                    .addReg(Comparator, RegState::Define)
                    .addReg(MI.getOperand(0).getReg())
                    .addReg(MI.getOperand(1).getReg());
          MIB.getInstr()->setBMOVIndex(bmov_index);

          MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::SLTIU))
                    .addReg(ResultBit, RegState::Define)
                    .addReg(Comparator)
                    .addImm(0x1);
          MIB.getInstr()->setBMOVIndex(bmov_index);

          MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::BMOVC_EQ))
                    .addReg(DestReg_BPR_C, RegState::Define)
                    .addReg(ResultBit)
                    .addReg(MI.getOperand(0).getReg())
                    .addReg(MI.getOperand(1).getReg());
          MIB.getInstr()->setBMOVIndex(bmov_index);

        } else if (MI.getOpcode() == RISCV::BLT) {

          /*
          BEQ xA, xB, imm
          =>
          SUB xC, xA, xB ; Will be 0 if equal
          SLtI xD, xC, 0x0 ; In less-than case: xC is less than 0 (signed), so
          result 1. Else, result 0. ; the above forms the less-than check.
          BMOVC_LT BPxC
         */

          Register Comparator =
              MF.getRegInfo().createVirtualRegister(&RISCV::GPRRegClass);
          Register ResultBit =
              MF.getRegInfo().createVirtualRegister(&RISCV::GPRRegClass);

          MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::SUB))
                    .addReg(Comparator, RegState::Define)
                    .addReg(MI.getOperand(0).getReg())
                    .addReg(MI.getOperand(1).getReg());
          MIB.getInstr()->setBMOVIndex(bmov_index);

          MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::SLTI))
                    .addReg(ResultBit, RegState::Define)
                    .addReg(Comparator)
                    .addImm(0x0);
          MIB.getInstr()->setBMOVIndex(bmov_index);

          MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::BMOVC_LT))
                    .addReg(DestReg_BPR_C, RegState::Define)
                    .addReg(ResultBit)
                    .addReg(MI.getOperand(0).getReg())
                    .addReg(MI.getOperand(1).getReg());
          MIB.getInstr()->setBMOVIndex(bmov_index);
        }

        MIB = BuildMI(MBB, MI, DL, TII->get(RISCV::PB))
                  .addReg(DestReg_BPR_C)
                  .addReg(DestReg_BPR_S)
                  .addReg(DestReg_BPR_T);
        MIB.getInstr()->setBMOVIndex(bmov_index);

        auto nI = std::next(I);
        MI.eraseFromBundle();
        I = nI;
        bmov_index++;
        // break;
      }
      /*else if (MI.isUnconditionalBranch()) {
        std::cout << "Unconditional branch " << MI.getOpcode() << std::endl;
        const auto &STI = MF.getSubtarget<RISCVSubtarget>();
        const RISCVInstrInfo *TII = STI.getInstrInfo();
        Register DestReg_BPR_T =
      MF.getRegInfo().createVirtualRegister(&RISCV::BPR_TRegClass); Register
      DestReg_BPR_S =
      MF.getRegInfo().createVirtualRegister(&RISCV::BPR_SRegClass); Register
      DestReg_BPR_C =
      MF.getRegInfo().createVirtualRegister(&RISCV::BPR_CRegClass); DebugLoc DL;
        MachineInstrBuilder MIB;

        MIB = BuildMI(MBB, MI ,DL, TII->get(RISCV::BMOVT_J)) // bne s1, s0,
      LBB0_4 to bmovt bt0, LBB0_4 .addReg(DestReg_BPR_T, RegState::Define)
            .addMBB(MI.getOperand(0).getMBB());
        MIB.getInstr()->setBMOVIndex(bmov_index);

        MIB = BuildMI(MBB, MI ,DL, TII->get(RISCV::BMOVS_J)) // This currently
      takes in the target label, but it should take the source label eventually.
            .addReg(DestReg_BPR_S, RegState::Define)
            .addMBB(MI.getOperand(0).getMBB());
        MIB.getInstr()->setBMOVIndex(bmov_index);

        MIB = BuildMI(MBB, MI ,DL, TII->get(RISCV::BMOVUC))
              .addReg(DestReg_BPR_C, RegState::Define)
              .addReg(RISCV::X0)
              .addReg(RISCV::X0);
        MIB.getInstr()->setBMOVIndex(bmov_index);

        auto nI = std::next(I);
        MI.eraseFromBundle();
        I = nI;
        bmov_index++;
      }*/
      else {
        I++;
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