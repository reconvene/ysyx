// See README.md for license details.

package singleALU

import chisel3._
import chisel3.util._

// 定义编码器,返回2的n次方
class singleALU extends Module{
  val singleALUIO=IO(new Bundle {
    val a=Input(UInt(1.W))
    val b=Input(UInt(1.W))
    val cin=Input(UInt(1.W))
    val s=Output(UInt(1.W))
    val cout=Output(UInt(1.W))
  })
  singleALUIO.s:=(singleALUIO.a+singleALUIO.b+singleALUIO.cin)(0)
  singleALUIO.cout:= (singleALUIO.a+&singleALUIO.b+&singleALUIO.cin)(1)
}


class singleALUTop extends Module {
  val io=IO(new Bundle {
    val a=Input(UInt(1.W))
    val b=Input(UInt(1.W))
    val cin=Input(UInt(1.W))
    val s=Output(UInt(1.W))
    val cout=Output(UInt(1.W))
  })

  val singleALU=Module(new singleALU())

  singleALU.singleALUIO.a:=io.a
  singleALU.singleALUIO.b:=io.b
  singleALU.singleALUIO.cin:=io.cin
  io.s:=singleALU.singleALUIO.s
  io.cout:=singleALU.singleALUIO.cout
}