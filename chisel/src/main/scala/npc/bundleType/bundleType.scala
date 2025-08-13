package npc.bundleType

import chisel3._

class systemInstCtrl extends Bundle {
  val csrWrite=Bool()
  val csrOpcode=UInt(3.W)
  val csrRead=Bool()

  val ebreakIF=Bool()
  val ecallIF=Bool()
  val mretIF=Bool()
}
