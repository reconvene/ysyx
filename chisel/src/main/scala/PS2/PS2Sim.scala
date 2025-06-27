package PS2

import chisel3._
import chisel3.util._
import PS2.ps2Codec

// ps/2键盘仿真模组
class ps2Sim extends Module {
  val simIO=IO(new Bundle {
    val inputChar=Input(UInt(8.W))
    val ps2_dout=Output(UInt(1.W))
    val ps2_clk=Output(UInt(1.W))
  })


  // 初始化时钟、临时数据位、计数器、数据索引和解码器
  val initialClk=RegInit(1.U(1.W))
  val clkGroup=RegInit(0.U(22.W))
//  val tmpData=RegInit(0.U(1.W))
  val initialCnt=RegInit(0.U(5.W))
  val tmpDataGroup=RegInit(0.U(22.W))

//  val dataIndex=RegInit(0.U(4.W))

  val ps2Encoder=Module(new ps2Codec)
  ps2Encoder.codecIO.codecDirection:=0.U
  ps2Encoder.codecIO.inputValue:=simIO.inputChar
  val outputNum=ps2Encoder.codecIO.outputValue
//  printf("encodeResult:\t%d\n",outputNum)


  // 拼接待处理字符串
  val finalData=1.U ## ~outputNum.xorR ## outputNum ## 0.U

  when(simIO.inputChar=/=0.U){
    when(initialCnt>20.U){
      simIO.ps2_dout:=finalData(10)
      tmpDataGroup:= tmpDataGroup(20,0) ## finalData(10)
      initialCnt:=0.U
      initialClk:= 1.U
    }.otherwise{
      simIO.ps2_dout:=finalData((initialCnt-1.U)/2.U)
      tmpDataGroup:= tmpDataGroup(20,0) ## finalData((initialCnt%22.U)/2.U)
      initialClk:= ~initialClk
      initialCnt:=initialCnt+1.U
    }
  }.otherwise{
    simIO.ps2_dout:=0.U
  }

  clkGroup:= clkGroup(20,0) ## initialClk


//  printf("dataIndex:\t%d\n",(initialCnt-1.U)/2.U)
//  printf("initialCnt:\t%d\n",initialCnt)
//  printf("finalData:%b\n",finalData)
//  printf("tmpDataGroup:\t%b\n",tmpDataGroup)
//  printf("clkGroup:\t%b\n",clkGroup)
//  printf("currentClk:%b\n",initialClk)
  // 连接输出端口
  simIO.ps2_clk:=initialClk

}

// 定义仿真顶部模块
class ps2Top extends Module{
  // 定义io接口
  val io=IO(new Bundle {
    val inputChar=Input(UInt(8.W))
    val outputChar=Output(Bits(8.W))
  })

  // 定义ps/2键盘发射器和ps/2键盘连接器
  val ps2Sim=Module(new ps2Sim)
  val ps2Connceter=Module(new ps2Connecter)
  val outReg=RegInit(0.U(8.W))

  // 连接对应端口
  ps2Sim.simIO.inputChar:=io.inputChar
  ps2Connceter.io.ps2_clk:=ps2Sim.simIO.ps2_clk
  ps2Connceter.io.ps2_data:=ps2Sim.simIO.ps2_dout
//  printf("finalResult:\t%d\n",ps2Connceter.io.out)
  outReg:=ps2Connceter.io.out
  io.outputChar:=outReg

}
