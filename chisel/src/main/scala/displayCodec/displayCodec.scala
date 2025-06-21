// See README.md for license details.

package displayCodec

import chisel3._
import chisel3.util._

//
object displayCodecMap {
  val displayCodecMap=VecInit(Seq.fill(128)(0.U(8.W)))
  // 0-9数码管编码
  displayCodecMap(48):="b11111101".U
  displayCodecMap(49):="b01100001".U
  displayCodecMap(50):="b11011011".U
  displayCodecMap(51):="b11110011".U
  displayCodecMap(52):="b01100111".U
  displayCodecMap(53):="b10110111".U
  displayCodecMap(54):="b10111111".U
  displayCodecMap(55):="b11100001".U
  displayCodecMap(56):="b11111111".U
  displayCodecMap(57):="b11110111".U
  // a-f数码管编码
  displayCodecMap(97):="b11101111".U
  displayCodecMap(98):="b00111111".U
  displayCodecMap(99):="b10011101".U
  displayCodecMap(100):="b01111011".U
  displayCodecMap(101):="b10011111".U
  displayCodecMap(102):="b10001111".U
}

// 定义编码器,返回对应字符数码管编码
class displayEncodec() extends Module {
  val encodec = IO(new Bundle {
    val n = Input(UInt(7.W))
    val en = Input(Bool())
    val m = Output(Bits(8.W))
  })
  encodec.m := Mux(encodec.en && displayCodecMap.displayCodecMap(encodec.n)=/=0.U, displayCodecMap.displayCodecMap(encodec.n), 1.U(8.W))
}

// 定义解码器，返回log2Ceil(m)
//class simpleDecodec extends Module {
//  val decodec = IO(new Bundle {
//    val m = Input(Bits(7.W))
//    val en = Input(Bool())
//    val n = Output(UInt(8.W))
//  })
//  decodec.n := Mux(decodec.en, PriorityEncoder(decodec.m), 0.U)
//}


class displayCodecTop extends Module {
  val encodeIO = IO(new Bundle {
    val n = Input(UInt(7.W))
    val en = Input(Bool())
    val m = Output(Bits(8.W))
  })

  val displayEncodec = Module(new displayEncodec())
  displayEncodec.encodec.n := encodeIO.n
  displayEncodec.encodec.en := encodeIO.en
  encodeIO.m := ~displayEncodec.encodec.m

//  val decodeIO=IO(new Bundle {
//    val m = Input(Bits(8.W))
//    val en = Input(Bool())
//    val n = Output(UInt(8.W))
//  })
//
//  val simpleDecodec=Module(new simpleDecodec())
//  simpleDecodec.decodec.m:=decodeIO.m
//  simpleDecodec.decodec.en:=decodeIO.en
//  decodeIO.n:=simpleDecodec.decodec.n

}