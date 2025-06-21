// See README.md for license details.

package displayCodec

import chisel3._
import chisel3.simulator.EphemeralSimulator._
import codec.codecTop
import displayCodec.displayCodecTop
import org.scalatest.freespec.AnyFreeSpec
import org.scalatest.matchers.must.Matchers

import scala.math._

class displayCodecSpec extends AnyFreeSpec with Matchers{
  "displayCodec should print the right number" in {
    simulate(new displayCodecTop){ dut =>

      for(i<-48 until(53)){
        dut.encodeIO.en.poke(true.B)
        dut.encodeIO.n.poke(i.U)
        dut.clock.step(1)
        println(dut.encodeIO.m.peek().litValue)
      }

    }
  }
}