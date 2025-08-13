package npc.decoder
import chisel3._
import chisel3.util._
import riscv32Types._
import npc.bundleType._

// SYSTEM类型指令decoder
class systemDecoder extends Module{
  val io=IO(new Bundle {
    val funct3=Input(UInt(3.W))
    val rs1=Input(UInt(4.W))
    val rd=Input(UInt(4.W))
    val imm=Input(SInt(word_len.W))

    val csrCrtl=Output(new systemInstCtrl)
  })

  // 重置输出信号组
  io.csrCrtl:=0.U.asTypeOf(new systemInstCtrl)

  // 判断是否为ebreak/ecall/mret
  io.csrCrtl.ebreakIF := (io.funct3===0.U) && (io.imm===1.S)
  io.csrCrtl.ecallIF := (io.funct3===0.U) && (io.imm===0.S)
  io.csrCrtl.mretIF := (io.funct3===0.U) && (io.imm.asUInt==="h302".U)


  // CSR寄存器操作码
  io.csrCrtl.csrOpcode := io.funct3

  // 只有在CSRRW指令且rd为0或者funct3为0的情况下不读
  io.csrCrtl.csrRead:=Mux((io.funct3 === 1.U && io.rd===0.U) || io.funct3===0.U, 0.U, 1.U)
  // 只有在CSRRS/CSRRC指令且rs1为0或funct3为0的情况下不写
  io.csrCrtl.csrWrite:=Mux(((io.funct3 === 2.U || io.funct3===3.U) && io.rs1===0.U) || io.funct3===0.U, 0.U, 1.U)
}
