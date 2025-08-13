import chisel3._

package object riscv32Types {
  // 字长
  val word_len=32

  // 操作数类型
  val typeR=0
  val typeI=1
  val typeS=2
  val typeB=3
  val typeU=4
  val typeJ=5

  // ALU操作类型
  val typeAdd=0
  val typeShiftLeft=1
  val typeSignedLess=2
  val typeUnsignedLess=3
  val typeXor=4
  val typeShiftRight=5
  val typeOr=6
  val typeAnd=7

  // mainALU输入类型
  val typeRs2=0
  val typeImm=1

  // pcALU输入类型
  val typeRs1=0
  val typePC=1

  // 寄存器写入类型
  val typeAluResult=0
  val typeMemRead=1
  val typeNextPC=2
  val typeImmU=3
  val typePCTarget=4
  val typeCsrRead=5

  // CSR寄存器类型
  val typeCsrMepc="h341"
  val typeCsrMstatus="h300"
  val typeCsrMcause="h342"
  val typeCsrMtvec="h305"

  // CSR写入类型
  val typeCsrEcall=0
  val typeCsrWrite=1
  val typeCsrSet=2
  val typeCsrClear=3

  // PC跳转目标
  val typePCNext=0
  val typePCAluResult=1
  val typeCsrResult=2

  // 符号类型
  val typeUnsigned=0
  val typeSigned=1
}