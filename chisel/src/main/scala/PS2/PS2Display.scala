package PS2

import chisel3._
import chisel3.util._
import displayCodec._
import streamingLight._

// 定义PS/2数码管显示模块
class ps2Display extends Module {
  val io=IO(new Bundle {
    val ps2_clk=Input(UInt(1.W))
    val ps2_data=Input(UInt(1.W))
    val en=Input(Bool())

    // 定义PS/2解码输出接口和数码管控制接口
    val ps2_out=Output(UInt(8.W))
    val seg0_out=Output(UInt(8.W))
    val seg1_out=Output(UInt(8.W))
    val seg2_out=Output(UInt(8.W))
    val seg3_out=Output(UInt(8.W))
    val seg4_out=Output(UInt(8.W))
    val seg5_out=Output(UInt(8.W))
  })

  // 定义16进制转字符映射表
  val hexToCharMap=VecInit(Seq.fill(16)(0.U(8.W)))
  hexToCharMap(0):="0".head.toInt.U(8.W)
  hexToCharMap(1):="1".head.toInt.U(8.W)
  hexToCharMap(2):="2".head.toInt.U(8.W)
  hexToCharMap(3):="3".head.toInt.U(8.W)
  hexToCharMap(4):="4".head.toInt.U(8.W)
  hexToCharMap(5):="5".head.toInt.U(8.W)
  hexToCharMap(6):="6".head.toInt.U(8.W)
  hexToCharMap(7):="7".head.toInt.U(8.W)
  hexToCharMap(8):="8".head.toInt.U(8.W)
  hexToCharMap(9):="9".head.toInt.U(8.W)
  hexToCharMap(10):="a".head.toInt.U(8.W)
  hexToCharMap(11):="b".head.toInt.U(8.W)
  hexToCharMap(12):="c".head.toInt.U(8.W)
  hexToCharMap(13):="d".head.toInt.U(8.W)
  hexToCharMap(14):="e".head.toInt.U(8.W)
  hexToCharMap(15):="f".head.toInt.U(8.W)

  // 声明PS/2连接器和8个数码管控制器
  val ps2Connecter=Module(new ps2Connecter)
  val segGroup=Seq.tabulate(6){i=>
    val singleSeg=Module(new displayCodecTop())
    singleSeg.encodeIO.en:=io.en
    singleSeg
  }

  // 定义输出字符计数器
  val charCount=RegInit(0.U(8.W))


  // 连接PS/2时钟线和数据线
  ps2Connecter.io.ps2_clk:=io.ps2_clk
  ps2Connecter.io.ps2_data:=io.ps2_data

  // 定义当前显示字符和扫描码
  val currentChar=RegInit(0.U(8.W))
  val currentMakeCode=RegInit(0.U(8.W))
  // 定义显示锁
  val displayLock=RegInit(0.U(1.W))
  // 定义断码重置计数器
  val breakCodeStatus=RegInit(0.U(8.W))


  when(breakCodeStatus=/=0.U){
    breakCodeStatus:=breakCodeStatus-1.U
    // 如果遇到断码
  }.elsewhen(ps2Connecter.io.out===63.U){
      // 熄灭数码管
      displayLock:=0.U
      // 重设断码重置计数器
      breakCodeStatus:=255.U
    }
    // 当PS/2有输出且不为断码
  .elsewhen(ps2Connecter.io.out=/=0.U && ps2Connecter.io.out=/=63.U){
    // 存储当前字符和当前通码
    currentChar:=ps2Connecter.io.out
    currentMakeCode:=ps2Connecter.io.originOut

    // 如果数码管为熄灭状态且上一个字符与当前输出不相同
    // 则将上一个字符变成当前输出、字符输入计数+1并显示数码管
    when((~displayLock).asBool){
      charCount:=charCount+1.U
      displayLock:=1.U
    }
  }

  // 定义连接数码管输出接口
  segGroup(0).encodeIO.n:=hexToCharMap(currentMakeCode(3,0))
  segGroup(0).encodeIO.en:=displayLock.asBool
  segGroup(1).encodeIO.n:=hexToCharMap(currentMakeCode(7,4))
  segGroup(1).encodeIO.en:=displayLock.asBool
  segGroup(2).encodeIO.n:=hexToCharMap(currentChar(3,0))
  segGroup(2).encodeIO.en:=displayLock.asBool
  segGroup(3).encodeIO.n:=hexToCharMap(currentChar(7,4))
  segGroup(3).encodeIO.en:=displayLock.asBool
  segGroup(4).encodeIO.n:=hexToCharMap(charCount(3,0))
  segGroup(5).encodeIO.n:=hexToCharMap(charCount(7,4))

  // 连接输出接口
  io.ps2_out:=ps2Connecter.io.out
  io.seg0_out:=segGroup(0).encodeIO.m
  io.seg1_out:=segGroup(1).encodeIO.m
  io.seg2_out:=segGroup(2).encodeIO.m
  io.seg3_out:=segGroup(3).encodeIO.m
  io.seg4_out:=segGroup(4).encodeIO.m
  io.seg5_out:=segGroup(5).encodeIO.m


}
