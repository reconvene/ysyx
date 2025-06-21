// See README.md for license details.

package multALU

import chisel3._
import scala.util.control.Breaks._
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
    val a = Input(UInt(width.W))
    val b = Input(UInt(width.W))
    val cin = Input(UInt(1.W))
    val out = Output(UInt((width + 1).W))
    val carry =Output(Bool())
    val zero =Output(Bool())
  })

  // 声明串行序列
  val aluSeq = Seq.tabulate(width) { i =>
    Module(new singleALU)
  }

  // 绑定各加法器关系
  for (i <- 0 until (width - 1)) {
    aluSeq(i).singleALUIO.a := io.a(i)
    aluSeq(i).singleALUIO.b := io.b(i)
    aluSeq(i + 1).singleALUIO.cin := aluSeq(i).singleALUIO.cout
  }
  aluSeq.head.singleALUIO.cin := io.cin
  aluSeq.last.singleALUIO.a := io.a(width - 1)
  aluSeq.last.singleALUIO.b := io.b(width - 1)
  
  // 拼合输出、判断进位、判断是否为零
  io.out := aluSeq.last.singleALUIO.cout ## Cat(aluSeq.reverse.map(dut => dut.singleALUIO.s))
  io.carry:=aluSeq.last.singleALUIO.cout.asBool
  io.zero := ~io.out.orR

}