package npc.regGroup

import chisel3._
import chisel3.util._
import riscv32Types._
import npc.bundleType._

// CSR寄存器组模块
class csrGroup extends Module{
  val io=IO(new Bundle {
    val csr=Input(UInt(12.W))
    val opcode=Input(UInt(3.W))
    val ecallIF=Input(Bool())
    val mretIF=Input(Bool())
    val src1=Input(UInt(word_len.W))
    val currentPC=Input(UInt(word_len.W))
    val writeEnable=Input(Bool())

    val csrData=Output(UInt(word_len.W))
  })

  // 声明4个32位的寄存器
  // mepc、mcause、mstatus、mtvec
  val regGroup = RegInit(VecInit(Seq.fill(4)(0.U(32.W))))

  // 映射编号
  val targetCsr = MuxCase(0.U,Seq(
    (io.csr===typeCsrMepc.U || io.mretIF) -> 0.U,
    (io.csr===typeCsrMcause.U) -> 1.U,
    (io.csr===typeCsrMstatus.U) -> 2.U,
    (io.csr===typeCsrMtvec.U || io.ecallIF) -> 3.U,
  ))

  // 输出读取数据
  io.csrData:=regGroup(targetCsr)

  // 写入寄存器(ecall情况特殊处理)
  when(io.ecallIF){
    regGroup(0.U):=io.currentPC
    regGroup(1.U):="hb".U
    regGroup(2.U):="h1800".U

  }.elsewhen(io.writeEnable) {
    // 未处理情况则不变
    regGroup(targetCsr):=MuxCase(regGroup(targetCsr), Seq(
      (io.opcode===1.U) -> io.src1,
      (io.opcode===2.U) -> (regGroup(targetCsr) | io.src1),
      (io.opcode===3.U) -> (regGroup(targetCsr) & (~io.src1).asUInt)
    ))
  }
  //  printf("x1 = 0x%x, x2 = 0x%x, x3 = 0x%x\n", regGroup(1), regGroup(2), regGroup(3))
}
