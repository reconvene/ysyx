package npc.decoder
import chisel3._
import chisel3.util._
import riscv32Types._
import npc.bundleType._

class decoder extends Module{
  val io=IO(new Bundle {
    val opcode=Input(UInt(7.W))
    val funct3=Input(UInt(3.W))
    val funct7=Input(UInt(7.W))
    val zero=Input(UInt())
    val compareResult=Input(UInt())
    val systemDecoderResult=Input(new systemInstCtrl)

    val regWrite=Output(Bool())
    val regWriteType=Output(UInt(3.W))
    val memWrite=Output(Bool())
    val memRead=Output(Bool())
    val immType=Output(UInt(3.W))
    val pcNextType=Output(UInt(2.W))
    val pcAluSrc=Output(Bool())
    val aluSrc=Output(Bool())
    val aluOp=Output(UInt(4.W))
    val systemInstCtrl=Output(new systemInstCtrl)
  })

  // 初始化输出接口
  io.regWrite := false.B
  io.regWriteType:=typeAluResult.U
  io.memWrite := false.B
  io.memRead := false.B
  io.immType  := typeR.U // 不生成
  io.pcNextType := typePCNext.U
  io.pcAluSrc := true.B
  io.aluSrc   := typeRs2.U
  io.aluOp    := typeAdd.U(4.W)
  io.systemInstCtrl := 0.U.asTypeOf(new systemInstCtrl)

  // 定义opcode类型
  val opcodeR="b0110011".U
  val opcodeI="b0010011".U
  val opcodeLoad="b0000011".U
  val opcodeStore="b0100011".U
  val opcodeBranch="b1100011".U
  val opcodeJal="b1101111".U
  val opcodeJalr="b1100111".U
  val opcodeLui="b0110111".U
  val opcodeAuipc="b0010111".U
  val opcodeSystem="b1110011".U

  switch(io.opcode) {
    is(opcodeR) {
      io.regWrite := true.B
      io.regWriteType := typeAluResult.U
      io.aluSrc   := typeRs2.U
      io.aluOp    := io.funct3 ## io.funct7(5)
    }

    is(opcodeI) {
      io.regWrite := true.B
      io.regWriteType := typeAluResult.U
      io.immType  := typeI.U
      io.aluSrc   := typeImm.U
      io.aluOp    := MuxCase(io.funct3 ## 0.U,Seq(
        (io.funct3===typeShiftLeft.U) -> io.funct3 ## io.funct7(5),
        (io.funct3===typeShiftRight.U) -> io.funct3 ## io.funct7(5)
      ))
    }

    is(opcodeLoad) {
      io.regWrite := true.B
      io.memRead:=true.B
      io.regWriteType:=typeMemRead.U
      io.immType  := typeI.U
      io.aluSrc:=typeImm.U
      io.aluOp:=typeAdd.U
    }

    is(opcodeStore) {
      io.memWrite := true.B
      io.immType  := typeS.U
      io.aluSrc:=typeImm.U
      io.aluOp:=typeAdd.U
    }

    is(opcodeBranch) {
      io.aluSrc:=typeRs2.U
      io.aluOp:=Mux(io.funct3<6.U, typeSignedLess.U ## 0.U, typeUnsignedLess.U ## 0.U)
      io.immType:=typeB.U
      io.pcNextType:=MuxCase(0.U,Seq(
        (io.funct3===0.U) -> io.zero,
        (io.funct3===1.U) -> ~io.zero,
        (io.funct3===4.U) -> io.compareResult,
        (io.funct3===5.U) -> ~io.compareResult,
        (io.funct3===6.U) -> io.compareResult,
        (io.funct3===7.U) -> ~io.compareResult
      ))
      io.pcAluSrc:=true.B
    }

    is(opcodeJal) {
      io.regWrite := true.B
      io.regWriteType:=typeNextPC.U
      io.pcNextType  := typePCAluResult.U
      io.pcAluSrc := true.B
      io.immType  := typeJ.U
    }

    is(opcodeJalr) {
      io.regWrite := true.B
      io.regWriteType:=typeNextPC.U
      io.pcNextType := typePCAluResult.U
      io.pcAluSrc := false.B
      io.immType  := typeI.U
    }

    is(opcodeLui) {
      io.regWrite := true.B
      io.regWriteType := typeImmU.U
      io.immType  := typeU.U
    }

    is(opcodeAuipc) {
      io.regWrite := true.B
      io.regWriteType := typePCTarget.U
      io.immType  := typeU.U
      io.pcAluSrc := true.B
    }


    is(opcodeSystem){
      io.regWrite:=io.systemDecoderResult.csrRead
      io.regWriteType:=Mux(io.systemDecoderResult.csrRead, typeCsrRead.U, typeAluResult.U)
      io.immType  := typeI.U

      // 指令为ecall/mret时,下一个pc为CSR寄存器输出结果
      io.pcNextType:=Mux(io.systemDecoderResult.ecallIF || io.systemDecoderResult.mretIF, typeCsrResult.U, typePCNext.U)
      io.systemInstCtrl:=io.systemDecoderResult
    }
  }

}
