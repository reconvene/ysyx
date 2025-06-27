// See README.md for license details.

package multALU

import chisel3._
import chisel3.simulator.EphemeralSimulator._
import org.scalatest.freespec.AnyFreeSpec
import org.scalatest.matchers.must.Matchers
import scala.util.Random



class multALUSpec extends AnyFreeSpec with Matchers {
  val width:Int=4
  "multALU should print the right number" in {
    // 加法检测
    simulate(new multALU((width))) { dut =>
      for (i <- -8 until (8)) {
        for (j <- -8 until (8)) {
          // 推入数字
          dut.io.a.poke(i.S)
          dut.io.b.poke(j.S)
          dut.io.cin.poke(0.U)
          // 加法
          dut.io.opcode.poke(0.U)
          dut.clock.step(1)
/*          // 检查输入
          println(s"a:${dut.io.a.peek().litValue.toInt}")
          println(s"aBinary:${dut.io.a.peek().litValue.toInt.toBinaryString}")
          println(s"b:${dut.io.b.peek().litValue.toInt}")
          println(s"bBinary:${dut.io.b.peek().litValue.toInt.toBinaryString}")
          // 打印二进制以及是否溢出
          println(s"当前值:${dut.io.out.peek().litValue.toInt.toBinaryString}")
          println(s"overflow:${dut.io.overflow.peek().litValue}")*/
          // 在输入合理的情况下，如果值不等于目标值且没有溢出则为计算错误
          assert(dut.io.out.peek().asSInt.litValue.toInt==(i+j) || dut.io.overflow.peek().litToBoolean)
        }
      }
      println("Addition test passed")

      // 减法检测
      for (i <- -8 until (8)) {
        for (j <- -7 until (8)) {
          // 推入数字
          dut.io.a.poke(i.S)
          dut.io.b.poke(j.S)
          dut.io.cin.poke(0.U)
          // 减法
          dut.io.opcode.poke(1.U)
          dut.clock.step(1)
/*          // 检查输入
          println(s"a:${dut.io.a.peek().litValue.toInt}")
          println(s"aBinary:${dut.io.a.peek().litValue.toInt.toBinaryString}")
          println(s"b:${dut.io.b.peek().litValue.toInt}")
          println(s"bBinary:${dut.io.b.peek().litValue.toInt.toBinaryString}")
          // 打印二进制以及是否溢出
          println(s"当前值:${dut.io.out.peek().litValue.toInt.toBinaryString}")
          println(s"overflow:${dut.io.overflow.peek().litValue}")*/
          // 在输入合理的情况下，如果值不等于目标值且没有溢出则为计算错误
          assert(dut.io.out.peek().asSInt.litValue.toInt==(i-j) || dut.io.overflow.peek().litToBoolean)
        }
      }
      println("Subtraction test passed")

      def multALUTester(inputA:Int,inputB:Int,inputCode:Int): Int = {
        dut.io.a.poke(inputA.S)
        dut.io.b.poke(inputB.S)
        dut.io.cin.poke(0.U)
        dut.io.opcode.poke(inputCode.U)
        dut.clock.step(1)
        return dut.io.out.peek().asSInt.litValue.toInt
      }

      //逻辑运算检测
      val randomGenerator=new Random()
      val randomA=randomGenerator.nextInt(16) -8
      val randomB=randomGenerator.nextInt(16) -8
      assert(multALUTester(randomA,0,2)== ~randomA)
      assert(multALUTester(randomA,randomB,3) == (randomA & randomB))
      assert(multALUTester(randomA,randomB,4) == (randomA | randomB))
      assert(multALUTester(randomA,randomB,5) == (randomA ^ randomB))
      assert(multALUTester(randomA,randomB,6) == (if(randomA < randomB) 1 else 0))
      assert(multALUTester(randomA,randomB,7) == (if(randomA == randomB) 1 else 0))
      println("Logic test passed")
    }
  }
}