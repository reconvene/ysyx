package npc.regGroup

import chisel3._

// 寄存器组模块
class regGroup extends Module{
  val io=IO(new Bundle {
    val rs1=Input(UInt(4.W))
    val rs2=Input(UInt(4.W))
    val rd=Input(UInt(4.W))
    val writeData=Input(UInt(32.W))
    val writeEnable=Input(Bool())

    val rs1Data=Output(UInt(32.W))
    val rs2Data=Output(UInt(32.W))
  })

  // 声明32个32位的寄存器
  val regGroup = RegInit(VecInit(Seq.fill(16)(0.U(32.W))))
  // 如果写入目标不是$0，并且写入接口为高电平，则写入
  when(io.rd=/=0.U && io.writeEnable){
    regGroup(io.rd):=io.writeData
  }

  // 绑定输出接口
  io.rs1Data:=regGroup(io.rs1)
  io.rs2Data:=regGroup(io.rs2)

//  printf("x1 = 0x%x, x2 = 0x%x, x3 = 0x%x\n", regGroup(1), regGroup(2), regGroup(3))
}
