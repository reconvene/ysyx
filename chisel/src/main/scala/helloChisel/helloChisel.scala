// See README.md for license details.

package helloChisel

import chisel3._

// 定义led1接口
class helloLED1 extends Module {
  val ledIO=IO(new Bundle {
    val ledInput=Input(Bool())
    val ledOutput=Output(Bool())
  })
  ledIO.ledOutput:=ledIO.ledInput
}

// 定义led2接口
class helloLED2 extends Module {
  val ledIO=IO(new Bundle {
    val ledInput=Input(Bool())
    val ledOutput=Output(Bool())
  })
  ledIO.ledOutput:=ledIO.ledInput
}

class helloChiselTop extends Module {
  val io=IO(new Bundle {
    val led1Output=Output(Bool())
    val led2Output=Output(Bool())
    val counterValue= Output(UInt(16.W))
  })

  val helloLED1=Module(new helloLED1())
  val helloLED2=Module(new helloLED2())
  helloLED2.ledIO.ledInput:=helloLED1.ledIO.ledOutput
  io.led1Output:=helloLED1.ledIO.ledOutput
  io.led2Output:=helloLED2.ledIO.ledOutput

  val countResetValue = 10000.U / 2.U
  val currentCountValue = RegInit(0.U(16.W))
  val ledWire = RegInit(false.B)
  io.counterValue:=currentCountValue
  helloLED1.ledIO.ledInput:=ledWire

  when(currentCountValue === countResetValue) {
    ledWire := ~ledWire
    currentCountValue := 0.U
  }.otherwise {
    currentCountValue := currentCountValue + 1.U
  }


}