// automatically generated by the FlatBuffers compiler, do not modify

import * as flatbuffers from 'flatbuffers';

import { Id } from '../flat-buffers/id';
import { Polygon } from '../flat-buffers/polygon';
import { Projection } from '../flat-buffers/projection';
import { Rectangle } from '../flat-buffers/rectangle';


export class Box {
  bb: flatbuffers.ByteBuffer|null = null;
  bb_pos = 0;
  __init(i:number, bb:flatbuffers.ByteBuffer):Box {
  this.bb_pos = i;
  this.bb = bb;
  return this;
}

static getRootAsBox(bb:flatbuffers.ByteBuffer, obj?:Box):Box {
  return (obj || new Box()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

static getSizePrefixedRootAsBox(bb:flatbuffers.ByteBuffer, obj?:Box):Box {
  bb.setPosition(bb.position() + flatbuffers.SIZE_PREFIX_LENGTH);
  return (obj || new Box()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

theta(obj?:Id):Id|null {
  const offset = this.bb!.__offset(this.bb_pos, 4);
  return offset ? (obj || new Id()).__init(this.bb_pos + offset, this.bb!) : null;
}

phi(obj?:Id):Id|null {
  const offset = this.bb!.__offset(this.bb_pos, 6);
  return offset ? (obj || new Id()).__init(this.bb_pos + offset, this.bb!) : null;
}

alpha(obj?:Id):Id|null {
  const offset = this.bb!.__offset(this.bb_pos, 8);
  return offset ? (obj || new Id()).__init(this.bb_pos + offset, this.bb!) : null;
}

projections(index: number, obj?:Projection):Projection|null {
  const offset = this.bb!.__offset(this.bb_pos, 10);
  return offset ? (obj || new Projection()).__init(this.bb!.__indirect(this.bb!.__vector(this.bb_pos + offset) + index * 4), this.bb!) : null;
}

projectionsLength():number {
  const offset = this.bb!.__offset(this.bb_pos, 10);
  return offset ? this.bb!.__vector_len(this.bb_pos + offset) : 0;
}

projection(obj?:Polygon):Polygon|null {
  const offset = this.bb!.__offset(this.bb_pos, 12);
  return offset ? (obj || new Polygon()).__init(this.bb!.__indirect(this.bb_pos + offset), this.bb!) : null;
}

rectangles(index: number, obj?:Rectangle):Rectangle|null {
  const offset = this.bb!.__offset(this.bb_pos, 14);
  return offset ? (obj || new Rectangle()).__init(this.bb!.__indirect(this.bb!.__vector(this.bb_pos + offset) + index * 4), this.bb!) : null;
}

rectanglesLength():number {
  const offset = this.bb!.__offset(this.bb_pos, 14);
  return offset ? this.bb!.__vector_len(this.bb_pos + offset) : 0;
}

terminal():boolean {
  const offset = this.bb!.__offset(this.bb_pos, 16);
  return offset ? !!this.bb!.readInt8(this.bb_pos + offset) : false;
}

inIndex():number {
  const offset = this.bb!.__offset(this.bb_pos, 18);
  return offset ? this.bb!.readInt32(this.bb_pos + offset) : 0;
}

static startBox(builder:flatbuffers.Builder) {
  builder.startObject(8);
}

static addTheta(builder:flatbuffers.Builder, thetaOffset:flatbuffers.Offset) {
  builder.addFieldStruct(0, thetaOffset, 0);
}

static addPhi(builder:flatbuffers.Builder, phiOffset:flatbuffers.Offset) {
  builder.addFieldStruct(1, phiOffset, 0);
}

static addAlpha(builder:flatbuffers.Builder, alphaOffset:flatbuffers.Offset) {
  builder.addFieldStruct(2, alphaOffset, 0);
}

static addProjections(builder:flatbuffers.Builder, projectionsOffset:flatbuffers.Offset) {
  builder.addFieldOffset(3, projectionsOffset, 0);
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

static addProjection(builder:flatbuffers.Builder, projectionOffset:flatbuffers.Offset) {
  builder.addFieldOffset(4, projectionOffset, 0);
}

static addRectangles(builder:flatbuffers.Builder, rectanglesOffset:flatbuffers.Offset) {
  builder.addFieldOffset(5, rectanglesOffset, 0);
}

static createRectanglesVector(builder:flatbuffers.Builder, data:flatbuffers.Offset[]):flatbuffers.Offset {
  builder.startVector(4, data.length, 4);
  for (let i = data.length - 1; i >= 0; i--) {
    builder.addOffset(data[i]!);
  }
  return builder.endVector();
}

static startRectanglesVector(builder:flatbuffers.Builder, numElems:number) {
  builder.startVector(4, numElems, 4);
}

static addTerminal(builder:flatbuffers.Builder, terminal:boolean) {
  builder.addFieldInt8(6, +terminal, +false);
}

static addInIndex(builder:flatbuffers.Builder, inIndex:number) {
  builder.addFieldInt32(7, inIndex, 0);
}

static endBox(builder:flatbuffers.Builder):flatbuffers.Offset {
  const offset = builder.endObject();
  return offset;
}

}
