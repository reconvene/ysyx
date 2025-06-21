// See README.md for license details.

package streamingLight

import chisel3._
import chisel3.simulator.EphemeralSimulator._
import streamingLight.streamingLightTop
import org.scalatest.freespec.AnyFreeSpec
import org.scalatest.matchers.must.Matchers

class streamingLightSpec extends AnyFreeSpec with Matchers{
  "streamingLight should light every light orderly" in {
    simulate(new streamingLightTop(8)){ dut =>

      // 手动复位，持续3个周期
      dut.reset.poke(true)
      dut.clock.step(3)
      dut.reset.poke(false)
      dut.clock.step(1)

      // 8个时间周期，每次输出ledLineStatus值
      for (i <- 0 until 8) {
        println(dut.io.ledLineStatusOutput.peek().litValue)
        dut.clock.step(1)
      }
    }
  }
}