// See README.md for license details.

package streamingLight

import chisel3._
import chisel3.util._

// 定义led组件
class normalLED extends Module {
  val ledIO=IO(new Bundle {
    val ledInput=Input(Bool())
    val ledOutput=Output(Bool())
  })
  ledIO.ledOutput:=ledIO.ledInput
}

class streamingLightTop(ledNum: Int) extends Module {
  // 定义led输出状态组
  val io=IO(new Bundle {
    val ledOutputGroup=Output(UInt(ledNum.W))
    val ledLineStatusOutput=Output(UInt(ledNum.W))
  })

  // 定义led灯信号控制组和led对象
  val ledLineStatus=RegInit(1.U(ledNum.W))
  val leds=Seq.tabulate(ledNum){i=>
      Module(new normalLED())
  }
  io.ledLineStatusOutput:=ledLineStatus


  // 遍历led对象将其与信号控制组绑定
  for(i<-0 until ledNum){
    leds(i).ledIO.ledInput:=ledLineStatus(i)
  }
  // 将led输出状态组与led输出绑定
  io.ledOutputGroup:=Cat(leds.reverse.map(_.ledIO.ledOutput))

  // 每次时间进1，则将信号控制组的1往左移一位
  ledLineStatus := ledLineStatus.rotateLeft(1)

}