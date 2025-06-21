package PS2

import chisel3._
import chisel3.util._

// 定义PS2信号解码器
class ps2Codec extends Module {
  // 定义解码接口
  val codecIO = IO(new Bundle {
    val inputValue = Input(Bits(8.W))
    val codecDirection = Input(UInt(1.W))
    val outputValue = Output(Bits(8.W))
  })

//  printf("%b\n",codecIO.inputValue)
  // 定义各个扫描码对应的字符
  val keyMap = Map(
    "hF0".U(8.W) -> "?".head.toInt.U(8.W),
    "h45".U(8.W) -> "0".head.toInt.U(8.W), // 0
    "h16".U(8.W) -> "1".head.toInt.U(8.W), // 1
    "h1E".U(8.W) -> "2".head.toInt.U(8.W), // 2
    "h26".U(8.W) -> "3".head.toInt.U(8.W), // 3
    "h25".U(8.W) -> "4".head.toInt.U(8.W), // 4
    "h2E".U(8.W) -> "5".head.toInt.U(8.W), // 5
    "h36".U(8.W) -> "6".head.toInt.U(8.W), // 6
    "h3D".U(8.W) -> "7".head.toInt.U(8.W), // 7
    "h3E".U(8.W) -> "8".head.toInt.U(8.W), // 8
    "h46".U(8.W) -> "9".head.toInt.U(8.W), // 9
    "h1C".U(8.W) -> "a".head.toInt.U(8.W), // A
    "h32".U(8.W) -> "b".head.toInt.U(8.W), // B
    "h21".U(8.W) -> "c".head.toInt.U(8.W), // C
    "h23".U(8.W) -> "d".head.toInt.U(8.W), // D
    "h24".U(8.W) -> "e".head.toInt.U(8.W), // E
    "h2B".U(8.W) -> "f".head.toInt.U(8.W) // F
  )

  val decodeCases = keyMap.map { case (k, v) =>
    (codecIO.inputValue === k) -> v
  }.toSeq

  val encodeCases = keyMap.map { case (k, v) =>
    (codecIO.inputValue === v) -> k
  }.toSeq

  // 定义默认值
  val tmpValue = WireDefault(0.U(8.W))
  // 当方向为1时则解码，为0时则编码
  when(codecIO.codecDirection.asBool) {
    // 解码
    tmpValue := MuxCase(0.U(8.W), decodeCases)
//    printf("decodeResult:\t%d\n",tmpValue)
  }.otherwise{
    // 编码
    tmpValue:=MuxCase(0.U(8.W),encodeCases)
  }

  // 输出匹配结果
  codecIO.outputValue:=tmpValue
//  printf("outputResult:\t%d\n",tmpValue)

}

// 定义PS/2信号接收器
class ps2Receiver extends Module{
  // 定义接收端口
  val receiverIO=IO(new Bundle {
    val din=Input(Bits(1.W))
    val clkin=Input(Bits(2.W))
    val dout=Output(Bits(8.W))
  })

  val tmpOut=RegInit(0.U(8.W))
  // 定义接收计数
  var signalCount = 0
  // 判断到下降沿则执行
  when(receiverIO.clkin===2.U){
    // 接收计数+1
    signalCount +=1
    // 去掉开始位、校验位和结束位
    tmpOut:=Mux(signalCount.U%11.U > 0.U && signalCount.U%11.U < 9.U,receiverIO.din ## tmpOut(7,1), 0.U)
  }
//  printf("tmpout:\t%b\n",tmpOut)
  receiverIO.dout:=tmpOut


}

// 定义ps/2连接器
class ps2Connecter extends Module{
  // 定义连接器IO端口
  val io=IO(new Bundle {
    val ps2_clk=Input(Bits(1.W))
    val ps2_data=Input(Bits(1.W))
    val out=Output(UInt(8.W))
  })

  // 声明解码器、接收器和时钟寄存器
  val ps2Codec=Module(new ps2Codec)
  val ps2Receiver=Module(new ps2Receiver)
  val clkReg=RegInit(0.U(2.W))

  // 存储一周期的时钟，并将其送入接收器内
  clkReg:=clkReg(0) ## io.ps2_clk
  ps2Receiver.receiverIO.din:=io.ps2_data
  ps2Receiver.receiverIO.clkin:=clkReg

  ps2Codec.codecIO.codecDirection:=1.U
  ps2Codec.codecIO.inputValue:=ps2Receiver.receiverIO.dout
  io.out:=ps2Codec.codecIO.outputValue
}