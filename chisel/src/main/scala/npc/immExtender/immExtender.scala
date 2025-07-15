package npc.immExtender
import chisel3._
import chisel3.util._
import riscv32Types._

class immExtender extends Module {
  def immI(inst:UInt):SInt=inst(31,20).asSInt
  def immS(inst:UInt):SInt=Cat(inst(31,25),inst(11,7)).asSInt
  def immB(inst:UInt):SInt=Cat(inst(31),inst(7),inst(30,25),inst(11,8),0.U(1.W)).asSInt
  def immU(inst:UInt):SInt=Cat(inst(31,12),0.U(12.W)).asSInt
  def immJ(inst:UInt):SInt=Cat(inst(31),inst(19,12),inst(20),inst(30,21),0.U(1.W)).asSInt

  val io=IO(new Bundle {
    val inst=Input(UInt(word_len.W))
    val immType=Input(UInt(3.W))

    val immSrc=Output(SInt(word_len.W))
  })

  io.immSrc:=MuxCase(0.S,Seq(
    (io.immType === typeI.U) -> immI(io.inst),
    (io.immType === typeS.U) -> immS(io.inst),
    (io.immType === typeB.U) -> immB(io.inst),
    (io.immType === typeU.U) -> immU(io.inst),
    (io.immType === typeJ.U) -> immJ(io.inst)
  ))
}
