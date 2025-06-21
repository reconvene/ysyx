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
  val initialClk=RegInit(0.U(1.W))
  val tmpData=RegInit(0.U(1.W))
  val initialCnt=RegInit(1.U(5.W))
  val tmpDataGroup=RegInit(0.U(22.W))
  val dataIndex=RegInit(0.U(4.W))
  val ps2Encoder=Module(new ps2Codec)
  ps2Encoder.codecIO.codecDirection:=0.U
  ps2Encoder.codecIO.inputValue:=simIO.inputChar
  val outputNum=ps2Encoder.codecIO.outputValue
//  printf("encodeResult:\t%d\n",outputNum)


  // 拼接待处理字符串
  val finalData=0.U ## outputNum ## 2.U


  // 计数<22则反转时钟、根据数据数据索引获取临时数据并且增加计数器
  when(initialCnt<22.U){
    initialClk:= ~initialClk
    tmpData:=finalData(dataIndex)
    tmpDataGroup:= tmpDataGroup(20,0) ## tmpData
    // dataIndex每2进1
    when(initialCnt%2.U===0.U && initialCnt =/= 0.U){
      dataIndex:=dataIndex+1.U
    }
    initialCnt:=initialCnt+1.U

    // 超过22则进行以下重置操作并发送最后的数据和时钟
  }.otherwise{
    initialClk:= ~initialClk
    tmpData:=finalData(dataIndex)
    tmpDataGroup:= tmpDataGroup(20,0) ## tmpData
    initialCnt:=1.U
    initialClk:=0.U
    dataIndex:=0.U
  }

//  printf("dataIndex:\t%d\n",dataIndex)
//  printf("initialCnt:\t%d\n",initialCnt)

//  printf("finalData:%b\n",finalData)
//  printf("tmpDataGroup:\t%b\n",tmpDataGroup)
  // 连接输出端口
  simIO.ps2_clk:=initialClk
  simIO.ps2_dout:=tmpData

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
//  printf("finalResult:\t%d\n",io.outputChar)
  outReg:=ps2Connceter.io.out
  io.outputChar:=outReg


}
