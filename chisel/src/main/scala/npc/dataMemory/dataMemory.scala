package npc.dataMemory
import chisel3._
import chisel3.util._
import riscv32Types._

class dataMemory extends Module{
  val io=IO(new Bundle {
    val targetAddr=Input(UInt(word_len.W))
    val funct3=Input(UInt(3.W))
    val rs2=Input(UInt(word_len.W))
    val memWrite=Input(Bool())
    val memRead=Input(Bool())

    val readAddr=Output(UInt(word_len.W))
    val readType=Output(Bool())
    val readEnable=Output(Bool())
    val writeAddr=Output(UInt(word_len.W))
    val writeData=Output(UInt(word_len.W))
    val writeEnable=Output(Bool())

    val byteNum=Output(UInt(3.W))
  })

  io.readAddr:=io.targetAddr
  io.readEnable:=io.memRead
  io.writeAddr:=io.targetAddr
  io.writeEnable:=io.memWrite

  io.byteNum:=MuxCase(0.U,Seq(
    (io.funct3===0.U || io.funct3===4.U) -> 1.U,
    (io.funct3===1.U || io.funct3===5.U) -> 2.U,
    (io.funct3===2.U) -> 4.U
  ))

  io.readType:=Mux(io.funct3<3.U,typeSigned.U,typeUnsigned.U)

  io.writeData:=MuxCase(0.U,Seq(
    (io.funct3===0.U && io.memWrite) -> io.rs2(7,0),
    (io.funct3===1.U && io.memWrite) -> io.rs2(15,0),
    (io.funct3===2.U && io.memWrite) -> io.rs2
  ))
}
