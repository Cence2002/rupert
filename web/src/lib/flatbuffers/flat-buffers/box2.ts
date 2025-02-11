// automatically generated by the FlatBuffers compiler, do not modify

import * as flatbuffers from 'flatbuffers';

import { Id } from '../flat-buffers/id';
import { Projection } from '../flat-buffers/projection';


export class Box2 {
  bb: flatbuffers.ByteBuffer|null = null;
  bb_pos = 0;
  __init(i:number, bb:flatbuffers.ByteBuffer):Box2 {
  this.bb_pos = i;
  this.bb = bb;
  return this;
}

static getRootAsBox2(bb:flatbuffers.ByteBuffer, obj?:Box2):Box2 {
  return (obj || new Box2()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

static getSizePrefixedRootAsBox2(bb:flatbuffers.ByteBuffer, obj?:Box2):Box2 {
  bb.setPosition(bb.position() + flatbuffers.SIZE_PREFIX_LENGTH);
  return (obj || new Box2()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

phi(obj?:Id):Id|null {
  const offset = this.bb!.__offset(this.bb_pos, 4);
  return offset ? (obj || new Id()).__init(this.bb_pos + offset, this.bb!) : null;
}

theta(obj?:Id):Id|null {
  const offset = this.bb!.__offset(this.bb_pos, 6);
  return offset ? (obj || new Id()).__init(this.bb_pos + offset, this.bb!) : null;
}

projections(index: number, obj?:Projection):Projection|null {
  const offset = this.bb!.__offset(this.bb_pos, 8);
  return offset ? (obj || new Projection()).__init(this.bb!.__indirect(this.bb!.__vector(this.bb_pos + offset) + index * 4), this.bb!) : null;
}

projectionsLength():number {
  const offset = this.bb!.__offset(this.bb_pos, 8);
  return offset ? this.bb!.__vector_len(this.bb_pos + offset) : 0;
}

out(index: number):number|null {
  const offset = this.bb!.__offset(this.bb_pos, 10);
  return offset ? this.bb!.readUint8(this.bb!.__vector(this.bb_pos + offset) + index) : 0;
}

outLength():number {
  const offset = this.bb!.__offset(this.bb_pos, 10);
  return offset ? this.bb!.__vector_len(this.bb_pos + offset) : 0;
}

outArray():Uint8Array|null {
  const offset = this.bb!.__offset(this.bb_pos, 10);
  return offset ? new Uint8Array(this.bb!.bytes().buffer, this.bb!.bytes().byteOffset + this.bb!.__vector(this.bb_pos + offset), this.bb!.__vector_len(this.bb_pos + offset)) : null;
}

static startBox2(builder:flatbuffers.Builder) {
  builder.startObject(4);
}

static addPhi(builder:flatbuffers.Builder, phiOffset:flatbuffers.Offset) {
  builder.addFieldStruct(0, phiOffset, 0);
}

static addTheta(builder:flatbuffers.Builder, thetaOffset:flatbuffers.Offset) {
  builder.addFieldStruct(1, thetaOffset, 0);
}

static addProjections(builder:flatbuffers.Builder, projectionsOffset:flatbuffers.Offset) {
  builder.addFieldOffset(2, projectionsOffset, 0);
}

static createProjectionsVector(builder:flatbuffers.Builder, data:flatbuffers.Offset[]):flatbuffers.Offset {
  builder.startVector(4, data.length, 4);
  for (let i = data.length - 1; i >= 0; i--) {
    builder.addOffset(data[i]!);
  }
  return builder.endVector();
}

static startProjectionsVector(builder:flatbuffers.Builder, numElems:number) {
  builder.startVector(4, numElems, 4);
}

static addOut(builder:flatbuffers.Builder, outOffset:flatbuffers.Offset) {
  builder.addFieldOffset(3, outOffset, 0);
}

static createOutVector(builder:flatbuffers.Builder, data:number[]|Uint8Array):flatbuffers.Offset {
  builder.startVector(1, data.length, 1);
  for (let i = data.length - 1; i >= 0; i--) {
    builder.addInt8(data[i]!);
  }
  return builder.endVector();
}

static startOutVector(builder:flatbuffers.Builder, numElems:number) {
  builder.startVector(1, numElems, 1);
}

static endBox2(builder:flatbuffers.Builder):flatbuffers.Offset {
  const offset = builder.endObject();
  return offset;
}

}
