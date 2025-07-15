// See README.md for license details.

package npc.ALU

import chisel3._
import chisel3.util._

// 定义串行加法器
class singleALU extends Module {
  val singleALUIO = IO(new Bundle {
    val a = Input(UInt(1.W))
    val b = Input(UInt(1.W))
    val cin = Input(UInt(1.W))
    val s = Output(UInt(1.W))
    val cout = Output(UInt(1.W))
  })
  singleALUIO.s := (singleALUIO.a + singleALUIO.b + singleALUIO.cin)(0)
  singleALUIO.cout := (singleALUIO.a +& singleALUIO.b +& singleALUIO.cin)(1)
}


class multALU(width: Int) extends Module {
  // 声明io接口
  val io = IO(new Bundle {
    val a = Input(SInt(width.W))
    val b = Input(SInt(width.W))
    val opcode = Input(UInt(4.W))
    val cin = Input(UInt(1.W))
    val out = Output(UInt(width.W))
    val carry = Output(Bool())
    val zero = Output(Bool())
    val less =Output(Bool())
    val overflow = Output(Bool())
  })

  // 声明串行序列
  val aluSeq = Seq.tabulate(width) { i =>
    Module(new singleALU)
  }

  // 如果为减法则反转b的符号(~b+1)
  val inputB = Mux(io.opcode === 0.U, io.b, -io.b)

  // 绑定各加法器关系
  for (i <- 0 until (width - 1)) {
    aluSeq(i).singleALUIO.a := io.a(i)
    aluSeq(i).singleALUIO.b := inputB(i)
    aluSeq(i + 1).singleALUIO.cin := aluSeq(i).singleALUIO.cout
  }
  aluSeq.head.singleALUIO.cin := io.cin
  aluSeq.last.singleALUIO.a := io.a(width - 1)
  aluSeq.last.singleALUIO.b := inputB(width - 1)


  // 拼合输出、判断进位、判断是否为零和是否溢出
  io.out := Cat(aluSeq.reverse.map(dut => dut.singleALUIO.s))
  io.carry := aluSeq.last.singleALUIO.cout.asBool
  io.zero := ~io.out.orR
  io.less := io.out.asSInt<0.S
  io.overflow := (io.a(width - 1) === inputB(width - 1)) && (io.out(width-1) =/= io.a(width - 1))

  // 根据操作码执行操作
  switch(io.opcode) {
    is("b0010".U){
      io.out := (io.a << io.b(4,0)).asUInt
      io.carry := 0.U
      io.overflow := 0.U
    }

    is("b0100".U){
      io.out := Mux(io.a < io.b, 1.U, 0.U)
      io.carry := 0.U
      io.overflow := 0.U
    }
    is("b0110".U){
      io.out := Mux(io.a.asUInt < io.b.asUInt, 1.U, 0.U)
      io.carry := 0.U
      io.overflow := 0.U
    }
    is("b1000".U){
      io.out := (io.a ^ io.b).asUInt
      io.carry := 0.U
      io.overflow := 0.U
    }
    is("b1010".U){
      io.out := (io.a.asUInt >> io.b(4,0)).asUInt
      io.carry := 0.U
      io.overflow := 0.U
    }
    is("b1011".U){
      io.out := (io.a >> io.b(4,0)).asUInt
      io.carry := 0.U
      io.overflow := 0.U
    }
    is("b1100".U) {
      io.out := (io.a | io.b).asUInt
      io.carry := 0.U
      io.overflow := 0.U
    }
    is("b1110".U){
      io.out := (io.a & io.b).asUInt
      io.carry := 0.U
      io.overflow := 0.U
    }
  }
}