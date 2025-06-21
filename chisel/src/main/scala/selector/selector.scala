// See README.md for license details.

package selector

import chisel3._
import chisel3.util._

// 定义二路选择器组件
class mux21c extends Module {
  val muxIO=IO(new Bundle {
    val numInput=Input(Vec(2, UInt(1.W)))
    val s=Input(UInt(1.W))
    val y=Output(UInt(1.W))
  })
  muxIO.y:=muxIO.numInput(muxIO.s)
}

// 定义四路选择器组件
class mux41c extends Module{
  val muxIO=IO(new Bundle {
    val numInput=Input(Vec(4, UInt(1.W)))
    val s=Input(UInt(2.W))
    val y=Output(UInt(1.W))
  })
  muxIO.y:=muxIO.numInput(muxIO.s)
}

class selectorTop extends Module {
  val io=IO(new Bundle {
    val a=Input(UInt(1.W))
    val b=Input(UInt(1.W))
    val c=Input(UInt(1.W))
    val d=Input(UInt(1.W))
    val s=Input(UInt(2.W))
    val mux21s=Output(UInt(2.W))
    val y1=Output(UInt(1.W))
    val y2=Output(UInt(1.W))
  })

  val mux21c=Module(new mux21c())
  val mux41c=Module(new mux41c())

  mux21c.muxIO.numInput(0):=io.a
  mux21c.muxIO.numInput(1):=io.b
  mux21c.muxIO.s:=Mux(io.s>1.U,1.U,io.s)
  io.y1:=mux21c.muxIO.y
  io.mux21s:=mux21c.muxIO.s

  mux41c.muxIO.numInput(0):=io.a
  mux41c.muxIO.numInput(1):=io.b
  mux41c.muxIO.numInput(2):=io.c
  mux41c.muxIO.numInput(3):=io.d
  mux41c.muxIO.s:=Mux(io.s>3.U,3.U,io.s)
  io.y2:=mux41c.muxIO.y


}