// automatically generated by the FlatBuffers compiler, do not modify

import * as flatbuffers from 'flatbuffers';

import { Box } from '../flat-buffers/box';
import { Polyhedron } from '../flat-buffers/polyhedron';


export class Debug {
  bb: flatbuffers.ByteBuffer|null = null;
  bb_pos = 0;
  __init(i:number, bb:flatbuffers.ByteBuffer):Debug {
  this.bb_pos = i;
  this.bb = bb;
  return this;
}

static getRootAsDebug(bb:flatbuffers.ByteBuffer, obj?:Debug):Debug {
  return (obj || new Debug()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

static getSizePrefixedRootAsDebug(bb:flatbuffers.ByteBuffer, obj?:Debug):Debug {
  bb.setPosition(bb.position() + flatbuffers.SIZE_PREFIX_LENGTH);
  return (obj || new Debug()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

hole(obj?:Polyhedron):Polyhedron|null {
  const offset = this.bb!.__offset(this.bb_pos, 4);
  return offset ? (obj || new Polyhedron()).__init(this.bb!.__indirect(this.bb_pos + offset), this.bb!) : null;
}

plug(obj?:Polyhedron):Polyhedron|null {
  const offset = this.bb!.__offset(this.bb_pos, 6);
  return offset ? (obj || new Polyhedron()).__init(this.bb!.__indirect(this.bb_pos + offset), this.bb!) : null;
}

boxes(index: number, obj?:Box):Box|null {
  const offset = this.bb!.__offset(this.bb_pos, 8);
  return offset ? (obj || new Box()).__init(this.bb!.__indirect(this.bb!.__vector(this.bb_pos + offset) + index * 4), this.bb!) : null;
}

boxesLength():number {
  const offset = this.bb!.__offset(this.bb_pos, 8);
  return offset ? this.bb!.__vector_len(this.bb_pos + offset) : 0;
}

static startDebug(builder:flatbuffers.Builder) {
  builder.startObject(3);
}

static addHole(builder:flatbuffers.Builder, holeOffset:flatbuffers.Offset) {
  builder.addFieldOffset(0, holeOffset, 0);
}

static addPlug(builder:flatbuffers.Builder, plugOffset:flatbuffers.Offset) {
  builder.addFieldOffset(1, plugOffset, 0);
}

static addBoxes(builder:flatbuffers.Builder, boxesOffset:flatbuffers.Offset) {
  builder.addFieldOffset(2, boxesOffset, 0);
}

static createBoxesVector(builder:flatbuffers.Builder, data:flatbuffers.Offset[]):flatbuffers.Offset {
  builder.startVector(4, data.length, 4);
  for (let i = data.length - 1; i >= 0; i--) {
    builder.addOffset(data[i]!);
  }
  return builder.endVector();
}

static startBoxesVector(builder:flatbuffers.Builder, numElems:number) {
  builder.startVector(4, numElems, 4);
}

static endDebug(builder:flatbuffers.Builder):flatbuffers.Offset {
  const offset = builder.endObject();
  return offset;
}

static finishDebugBuffer(builder:flatbuffers.Builder, offset:flatbuffers.Offset) {
  builder.finish(offset);
}

static finishSizePrefixedDebugBuffer(builder:flatbuffers.Builder, offset:flatbuffers.Offset) {
  builder.finish(offset, undefined, true);
}

}
