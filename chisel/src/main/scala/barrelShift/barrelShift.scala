package barrelShift

import selector._
import chisel3._
import chisel3.util._

class barrelShift(width: Int) extends Module {
  // 定义桶形移位器IO接口
  val io = IO(new Bundle {
    // 0右1左
    val moveDirection = Input(UInt(1.W))
    val moveLogic = Input(UInt(1.W))
    val moveNum = Input(UInt(log2Ceil(width).W))
    val din = Input(Bits(width.W))
    val dout = Output(Bits(width.W))
  })
  // 只处理2的级数的宽度
  if (width > 0 && (width & (width - 1)) == 0) {
    // 定义逻辑选择器
    val logicSelector = Module(new mux21c)
    logicSelector.muxIO.numInput(1) := io.din(width - 1)
    logicSelector.muxIO.numInput(0) := 0.U
    logicSelector.muxIO.s := io.moveLogic
    // 定义总选择器模块
    val grossMuxModule = Seq.tabulate(log2Ceil(width)) { i =>
      Seq.tabulate(width) { j =>
        Module(new mux41c)
      }
    }
    // 连线
    for (i <- 0 until (log2Ceil(width))) {
      // 声明移位数
      var q = 1 << i
      for (j <- 0 until (width)) {

        // 连接移动方向和是否移动
        grossMuxModule(i)(j).muxIO.s := io.moveDirection ## io.moveNum(i)
        // 第一层选择器端口连到原数据,后俩层则连到上一层的输出
        if (i == 0) {
          grossMuxModule(i)(j).muxIO.numInput(0) := io.din(j)
          grossMuxModule(i)(j).muxIO.numInput(2) := io.din(j)

          // 将头部元素的左边定义到逻辑选择器，其它则定义到上一批选择器的输出值
          if(j+q>width-1){
            grossMuxModule(i)(j).muxIO.numInput(1) := logicSelector.muxIO.y
          }else{
            grossMuxModule(i)(j).muxIO.numInput(1) := io.din(j + q)
          }
          // 将尾部元素的右边定义到0，其它则定义到下一批选择器的输出值
          if(j-q<0){
            grossMuxModule(i)(j).muxIO.numInput(3) := 0.U
          }else{
            grossMuxModule(i)(j).muxIO.numInput(3) := io.din(j - q)
          }

        } else {
          grossMuxModule(i)(j).muxIO.numInput(0) := grossMuxModule(i - 1)(j).muxIO.y
          grossMuxModule(i)(j).muxIO.numInput(2) := grossMuxModule(i - 1)(j).muxIO.y

          // 将头部元素的左边定义到逻辑选择器，其它则定义到上一批选择器的输出值
          if(j+q>width-1){
            grossMuxModule(i)(j).muxIO.numInput(1) := logicSelector.muxIO.y
          }else{
            grossMuxModule(i)(j).muxIO.numInput(1) := grossMuxModule(i - 1)(j + q).muxIO.y
          }

          // 将尾部元素的右边定义到0，其它则定义到下一批选择器的输出值
          if(j-q<0){
            grossMuxModule(i)(j).muxIO.numInput(3) := 0.U
          }else{
            grossMuxModule(i)(j).muxIO.numInput(3) := grossMuxModule(i - 1)(j - q).muxIO.y
          }
        }

      }
    }

    // 将计算结果连接到dout
    io.dout:=Cat(grossMuxModule(log2Ceil(width)-1).reverse.map(_.muxIO.y))
  } else {
    io.dout := io.din
  }
}
