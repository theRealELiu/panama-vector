/*
 * Copyright (c) 2007, 2017, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#include "precompiled.hpp"
#include "memory/allocation.inline.hpp"
#include "opto/connode.hpp"
#include "opto/subnode.hpp"
#include "opto/vectornode.hpp"
#include "utilities/powerOfTwo.hpp"

//------------------------------VectorNode--------------------------------------

// Return the vector operator for the specified scalar operation
// and vector length.
int VectorNode::opcode(int sopc, BasicType bt) {
  switch (sopc) {
  case Op_AddI:
    switch (bt) {
    case T_BOOLEAN:
    case T_BYTE:      return Op_AddVB;
    case T_CHAR:
    case T_SHORT:     return Op_AddVS;
    case T_INT:       return Op_AddVI;
    default:          ShouldNotReachHere(); return 0;
    }
  case Op_AddL:
    assert(bt == T_LONG, "must be");
    return Op_AddVL;
  case Op_AddF:
    assert(bt == T_FLOAT, "must be");
    return Op_AddVF;
  case Op_AddD:
    assert(bt == T_DOUBLE, "must be");
    return Op_AddVD;
  case Op_SubI:
    switch (bt) {
    case T_BOOLEAN:
    case T_BYTE:   return Op_SubVB;
    case T_CHAR:
    case T_SHORT:  return Op_SubVS;
    case T_INT:    return Op_SubVI;
    default:       ShouldNotReachHere(); return 0;
    }
  case Op_SubL:
    assert(bt == T_LONG, "must be");
    return Op_SubVL;
  case Op_SubF:
    assert(bt == T_FLOAT, "must be");
    return Op_SubVF;
  case Op_SubD:
    assert(bt == T_DOUBLE, "must be");
    return Op_SubVD;
  case Op_MulI:
    switch (bt) {
    case T_BOOLEAN:return 0;
    case T_BYTE:   return Op_MulVB;
    case T_CHAR:
    case T_SHORT:  return Op_MulVS;
    case T_INT:    return Op_MulVI;
    default:       ShouldNotReachHere(); return 0;
    }
  case Op_MulL:
    assert(bt == T_LONG, "must be");
    return Op_MulVL;
  case Op_MulF:
    assert(bt == T_FLOAT, "must be");
    return Op_MulVF;
  case Op_MulD:
    assert(bt == T_DOUBLE, "must be");
    return Op_MulVD;
  case Op_FmaD:
    assert(bt == T_DOUBLE, "must be");
    return Op_FmaVD;
  case Op_FmaF:
    assert(bt == T_FLOAT, "must be");
    return Op_FmaVF;
  case Op_CMoveF:
    assert(bt == T_FLOAT, "must be");
    return Op_CMoveVF;
  case Op_CMoveD:
    assert(bt == T_DOUBLE, "must be");
    return Op_CMoveVD;
  case Op_DivF:
    assert(bt == T_FLOAT, "must be");
    return Op_DivVF;
  case Op_DivD:
    assert(bt == T_DOUBLE, "must be");
    return Op_DivVD;
  case Op_AbsI:
    switch (bt) {
    case T_BOOLEAN:
    case T_CHAR:  return 0; // abs does not make sense for unsigned
    case T_BYTE:  return Op_AbsVB;
    case T_SHORT: return Op_AbsVS;
    case T_INT:   return Op_AbsVI;
    default: ShouldNotReachHere(); return 0;
    }
  case Op_AbsL:
    assert(bt == T_LONG, "must be");
    return Op_AbsVL;
  case Op_MinI:
    switch (bt) {
    case T_BOOLEAN:
    case T_CHAR:   return 0;
    case T_BYTE:
    case T_SHORT:
    case T_INT:    return Op_MinV;
    default:       ShouldNotReachHere(); return 0;
    }
  case Op_MinL:
    assert(bt == T_LONG, "must be");
    return Op_MinV;
  case Op_MinF:
    assert(bt == T_FLOAT, "must be");
    return Op_MinV;
  case Op_MinD:
    assert(bt == T_DOUBLE, "must be");
    return Op_MinV;
  case Op_MaxI:
    switch (bt) {
    case T_BOOLEAN:
    case T_CHAR:   return 0;
    case T_BYTE:
    case T_SHORT:
    case T_INT:    return Op_MaxV;
    default:       ShouldNotReachHere(); return 0;
    }
  case Op_MaxL:
    assert(bt == T_LONG, "must be");
    return Op_MaxV;
  case Op_MaxF:
    assert(bt == T_FLOAT, "must be");
    return Op_MaxV;
  case Op_MaxD:
    assert(bt == T_DOUBLE, "must be");
    return Op_MaxV;
  case Op_AbsF:
    assert(bt == T_FLOAT, "must be");
    return Op_AbsVF;
  case Op_AbsD:
    assert(bt == T_DOUBLE, "must be");
    return Op_AbsVD;
  case Op_NegI:
    assert(bt == T_INT, "must be");
    return Op_NegVI;
  case Op_NegF:
    assert(bt == T_FLOAT, "must be");
    return Op_NegVF;
  case Op_NegD:
    assert(bt == T_DOUBLE, "must be");
    return Op_NegVD;
  case Op_RoundDoubleMode:
    assert(bt == T_DOUBLE, "must be");
    return Op_RoundDoubleModeV;
  case Op_SqrtF:
    assert(bt == T_FLOAT, "must be");
    return Op_SqrtVF;
  case Op_SqrtD:
    assert(bt == T_DOUBLE, "must be");
    return Op_SqrtVD;
  case Op_PopCountI:
    if (bt == T_INT) {
      return Op_PopCountVI;
    }
    // Unimplemented for subword types since bit count changes
    // depending on size of lane (and sign bit).
    return 0;
  case Op_LShiftI:
    switch (bt) {
    case T_BOOLEAN:
    case T_BYTE:   return Op_LShiftVB;
    case T_CHAR:
    case T_SHORT:  return Op_LShiftVS;
    case T_INT:    return Op_LShiftVI;
      default:       ShouldNotReachHere(); return 0;
    }
  case Op_LShiftL:
    assert(bt == T_LONG, "must be");
    return Op_LShiftVL;
  case Op_RShiftI:
    switch (bt) {
    case T_BOOLEAN:return Op_URShiftVB; // boolean is unsigned value
    case T_CHAR:   return Op_URShiftVS; // char is unsigned value
    case T_BYTE:   return Op_RShiftVB;
    case T_SHORT:  return Op_RShiftVS;
    case T_INT:    return Op_RShiftVI;
    default:       ShouldNotReachHere(); return 0;
    }
  case Op_RShiftL:
    assert(bt == T_LONG, "must be");
    return Op_RShiftVL;
  case Op_URShiftB:
    assert(bt == T_BYTE, "must be");
    return Op_URShiftVB;
  case Op_URShiftS:
    assert(bt == T_SHORT, "must be");
    return Op_URShiftVS;
  case Op_URShiftI:
    switch (bt) {
    case T_BOOLEAN:return Op_URShiftVB;
    case T_CHAR:   return Op_URShiftVS;
    case T_BYTE:
    case T_SHORT:  return 0; // Vector logical right shift for signed short
                             // values produces incorrect Java result for
                             // negative data because java code should convert
                             // a short value into int value with sign
                             // extension before a shift.
    case T_INT:    return Op_URShiftVI;
    default:       ShouldNotReachHere(); return 0;
    }
  case Op_URShiftL:
    assert(bt == T_LONG, "must be");
    return Op_URShiftVL;
  case Op_AndI:
  case Op_AndL:
    return Op_AndV;
  case Op_OrI:
  case Op_OrL:
    return Op_OrV;
  case Op_XorI:
  case Op_XorL:
    return Op_XorV;

  case Op_LoadB:
  case Op_LoadUB:
  case Op_LoadUS:
  case Op_LoadS:
  case Op_LoadI:
  case Op_LoadL:
  case Op_LoadF:
  case Op_LoadD:
    return Op_LoadVector;

  case Op_StoreB:
  case Op_StoreC:
  case Op_StoreI:
  case Op_StoreL:
  case Op_StoreF:
  case Op_StoreD:
    return Op_StoreVector;
  case Op_MulAddS2I:
    return Op_MulAddVS2VI;

  default:
    return 0; // Unimplemented
  }
}

int VectorNode::replicate_opcode(BasicType bt) {
  switch(bt) {
    case T_BOOLEAN:
    case T_BYTE:
      return Op_ReplicateB;
    case T_SHORT:
    case T_CHAR:
      return Op_ReplicateS;
    case T_INT:
      return Op_ReplicateI;
    case T_LONG:
      return Op_ReplicateL;
    case T_FLOAT:
      return Op_ReplicateF;
    case T_DOUBLE:
      return Op_ReplicateD;
    default:
      break;
  }

  return 0;
}

// Also used to check if the code generator
// supports the vector operation.
bool VectorNode::implemented(int opc, uint vlen, BasicType bt) {
  if (is_java_primitive(bt) &&
      (vlen > 1) && is_power_of_2(vlen) &&
      Matcher::vector_size_supported(bt, vlen)) {
    int vopc = VectorNode::opcode(opc, bt);
    return vopc > 0 && Matcher::match_rule_supported_vector(vopc, vlen, bt);
  }
  return false;
}

bool VectorNode::is_type_transition_short_to_int(Node* n) {
  switch (n->Opcode()) {
  case Op_MulAddS2I:
    return true;
  }
  return false;
}

bool VectorNode::is_type_transition_to_int(Node* n) {
  return is_type_transition_short_to_int(n);
}

bool VectorNode::is_muladds2i(Node* n) {
  if (n->Opcode() == Op_MulAddS2I) {
    return true;
  }
  return false;
}

bool VectorNode::is_roundopD(Node *n) {
  if (n->Opcode() == Op_RoundDoubleMode) {
    return true;
  }
  return false;
}

bool VectorNode::is_shift(Node* n) {
  switch (n->Opcode()) {
  case Op_LShiftI:
  case Op_LShiftL:
  case Op_RShiftI:
  case Op_RShiftL:
  case Op_URShiftI:
  case Op_URShiftL:
    return true;
  default:
    return false;
  }
}

bool VectorNode::is_vshift(Node* n) {
  switch (n->Opcode()) {
  case Op_LShiftVB:
  case Op_LShiftVS:
  case Op_LShiftVI:
  case Op_LShiftVL:

  case Op_RShiftVB:
  case Op_RShiftVS:
  case Op_RShiftVI:
  case Op_RShiftVL:

  case Op_URShiftVB:
  case Op_URShiftVS:
  case Op_URShiftVI:
  case Op_URShiftVL:
    return true;
  default:
    return false;
  }
}

bool VectorNode::is_vshift_cnt(Node* n) {
  switch (n->Opcode()) {
  case Op_LShiftCntV:
  case Op_RShiftCntV:
    return true;
  default:
    return false;
  }
}

// Check if input is loop invariant vector.
bool VectorNode::is_invariant_vector(Node* n) {
  // Only Replicate vector nodes are loop invariant for now.
  switch (n->Opcode()) {
  case Op_ReplicateB:
  case Op_ReplicateS:
  case Op_ReplicateI:
  case Op_ReplicateL:
  case Op_ReplicateF:
  case Op_ReplicateD:
    return true;
  default:
    return false;
  }
}

// [Start, end) half-open range defining which operands are vectors
void VectorNode::vector_operands(Node* n, uint* start, uint* end) {
  switch (n->Opcode()) {
  case Op_LoadB:   case Op_LoadUB:
  case Op_LoadS:   case Op_LoadUS:
  case Op_LoadI:   case Op_LoadL:
  case Op_LoadF:   case Op_LoadD:
  case Op_LoadP:   case Op_LoadN:
    *start = 0;
    *end   = 0; // no vector operands
    break;
  case Op_StoreB:  case Op_StoreC:
  case Op_StoreI:  case Op_StoreL:
  case Op_StoreF:  case Op_StoreD:
  case Op_StoreP:  case Op_StoreN:
    *start = MemNode::ValueIn;
    *end   = MemNode::ValueIn + 1; // 1 vector operand
    break;
  case Op_LShiftI:  case Op_LShiftL:
  case Op_RShiftI:  case Op_RShiftL:
  case Op_URShiftI: case Op_URShiftL:
    *start = 1;
    *end   = 2; // 1 vector operand
    break;
  case Op_AddI: case Op_AddL: case Op_AddF: case Op_AddD:
  case Op_SubI: case Op_SubL: case Op_SubF: case Op_SubD:
  case Op_MulI: case Op_MulL: case Op_MulF: case Op_MulD:
  case Op_DivF: case Op_DivD:
  case Op_AndI: case Op_AndL:
  case Op_OrI:  case Op_OrL:
  case Op_XorI: case Op_XorL:
  case Op_MulAddS2I:
    *start = 1;
    *end   = 3; // 2 vector operands
    break;
  case Op_CMoveI:  case Op_CMoveL:  case Op_CMoveF:  case Op_CMoveD:
    *start = 2;
    *end   = n->req();
    break;
  case Op_FmaD:
  case Op_FmaF:
    *start = 1;
    *end   = 4; // 3 vector operands
    break;
  default:
    *start = 1;
    *end   = n->req(); // default is all operands
  }
}

// Make a vector node for binary operation
VectorNode* VectorNode::make(int vopc, Node* n1, Node* n2, const TypeVect* vt) {
  // This method should not be called for unimplemented vectors.
  guarantee(vopc > 0, "vopc must be > 0");
  switch (vopc) {
  case Op_AddVB: return new AddVBNode(n1, n2, vt);
  case Op_AddVS: return new AddVSNode(n1, n2, vt);
  case Op_AddVI: return new AddVINode(n1, n2, vt);
  case Op_AddVL: return new AddVLNode(n1, n2, vt);
  case Op_AddVF: return new AddVFNode(n1, n2, vt);
  case Op_AddVD: return new AddVDNode(n1, n2, vt);

  case Op_SubVB: return new SubVBNode(n1, n2, vt);
  case Op_SubVS: return new SubVSNode(n1, n2, vt);
  case Op_SubVI: return new SubVINode(n1, n2, vt);
  case Op_SubVL: return new SubVLNode(n1, n2, vt);
  case Op_SubVF: return new SubVFNode(n1, n2, vt);
  case Op_SubVD: return new SubVDNode(n1, n2, vt);

  case Op_MulVB: return new MulVBNode(n1, n2, vt);
  case Op_MulVS: return new MulVSNode(n1, n2, vt);
  case Op_MulVI: return new MulVINode(n1, n2, vt);
  case Op_MulVL: return new MulVLNode(n1, n2, vt);
  case Op_MulVF: return new MulVFNode(n1, n2, vt);
  case Op_MulVD: return new MulVDNode(n1, n2, vt);

  case Op_DivVF: return new DivVFNode(n1, n2, vt);
  case Op_DivVD: return new DivVDNode(n1, n2, vt);

  case Op_MinV: return new MinVNode(n1, n2, vt);
  case Op_MaxV: return new MaxVNode(n1, n2, vt);

  case Op_AbsVF: return new AbsVFNode(n1, vt);
  case Op_AbsVD: return new AbsVDNode(n1, vt);
  case Op_AbsVB: return new AbsVBNode(n1, vt);
  case Op_AbsVS: return new AbsVSNode(n1, vt);
  case Op_AbsVI: return new AbsVINode(n1, vt);
  case Op_AbsVL: return new AbsVLNode(n1, vt);

  case Op_NegVI: return new NegVINode(n1, vt);
  case Op_NegVF: return new NegVFNode(n1, vt);
  case Op_NegVD: return new NegVDNode(n1, vt);

  case Op_SqrtVF: return new SqrtVFNode(n1, vt);
  case Op_SqrtVD: return new SqrtVDNode(n1, vt);

  case Op_PopCountVI: return new PopCountVINode(n1, vt);

  case Op_LShiftVB: return new LShiftVBNode(n1, n2, vt);
  case Op_LShiftVS: return new LShiftVSNode(n1, n2, vt);
  case Op_LShiftVI: return new LShiftVINode(n1, n2, vt);
  case Op_LShiftVL: return new LShiftVLNode(n1, n2, vt);

  case Op_RShiftVB: return new RShiftVBNode(n1, n2, vt);
  case Op_RShiftVS: return new RShiftVSNode(n1, n2, vt);
  case Op_RShiftVI: return new RShiftVINode(n1, n2, vt);
  case Op_RShiftVL: return new RShiftVLNode(n1, n2, vt);

  case Op_URShiftVB: return new URShiftVBNode(n1, n2, vt);
  case Op_URShiftVS: return new URShiftVSNode(n1, n2, vt);
  case Op_URShiftVI: return new URShiftVINode(n1, n2, vt);
  case Op_URShiftVL: return new URShiftVLNode(n1, n2, vt);

  case Op_AndV: return new AndVNode(n1, n2, vt);
  case Op_OrV:  return new OrVNode (n1, n2, vt);
  case Op_XorV: return new XorVNode(n1, n2, vt);

  case Op_RoundDoubleModeV: return new RoundDoubleModeVNode(n1, n2, vt);

  case Op_MulAddVS2VI: return new MulAddVS2VINode(n1, n2, vt);
  default:
    fatal("Missed vector creation for '%s'", NodeClassNames[vopc]);
    return NULL;
  }
}

// Return the vector version of a scalar binary operation node.
VectorNode* VectorNode::make(int opc, Node* n1, Node* n2, uint vlen, BasicType bt) {
  const TypeVect* vt = TypeVect::make(bt, vlen);
  int vopc = VectorNode::opcode(opc, bt);
  // This method should not be called for unimplemented vectors.
  guarantee(vopc > 0, "Vector for '%s' is not implemented", NodeClassNames[opc]);
  return make(vopc, n1, n2, vt);
}

// Make a vector node for ternary operation
VectorNode* VectorNode::make(int vopc, Node* n1, Node* n2, Node* n3, const TypeVect* vt) {
  // This method should not be called for unimplemented vectors.
  guarantee(vopc > 0, "vopc must be > 0");
  switch (vopc) {
  case Op_FmaVD: return new FmaVDNode(n1, n2, n3, vt);
  case Op_FmaVF: return new FmaVFNode(n1, n2, n3, vt);
  default:
    fatal("Missed vector creation for '%s'", NodeClassNames[vopc]);
    return NULL;
  }
}

// Return the vector version of a scalar ternary operation node.
VectorNode* VectorNode::make(int opc, Node* n1, Node* n2, Node* n3, uint vlen, BasicType bt) {
  const TypeVect* vt = TypeVect::make(bt, vlen);
  int vopc = VectorNode::opcode(opc, bt);
  // This method should not be called for unimplemented vectors.
  guarantee(vopc > 0, "Vector for '%s' is not implemented", NodeClassNames[opc]);
  return make(vopc, n1, n2, n3, vt);
}

// Scalar promotion
VectorNode* VectorNode::scalar2vector(Node* s, uint vlen, const Type* opd_t) {
  BasicType bt = opd_t->array_element_basic_type();
  const TypeVect* vt = opd_t->singleton() ? TypeVect::make(opd_t, vlen)
                                          : TypeVect::make(bt, vlen);
  switch (bt) {
  case T_BOOLEAN:
  case T_BYTE:
    return new ReplicateBNode(s, vt);
  case T_CHAR:
  case T_SHORT:
    return new ReplicateSNode(s, vt);
  case T_INT:
    return new ReplicateINode(s, vt);
  case T_LONG:
    return new ReplicateLNode(s, vt);
  case T_FLOAT:
    return new ReplicateFNode(s, vt);
  case T_DOUBLE:
    return new ReplicateDNode(s, vt);
  default:
    fatal("Type '%s' is not supported for vectors", type2name(bt));
    return NULL;
  }
}

VectorNode* VectorNode::shift_count(int opc, Node* cnt, uint vlen, BasicType bt) {
  // Match shift count type with shift vector type.
  const TypeVect* vt = TypeVect::make(bt, vlen);
  switch (opc) {
  case Op_LShiftI:
  case Op_LShiftL:
    return new LShiftCntVNode(cnt, vt);
  case Op_RShiftI:
  case Op_RShiftL:
  case Op_URShiftB:
  case Op_URShiftS:
  case Op_URShiftI:
  case Op_URShiftL:
    return new RShiftCntVNode(cnt, vt);
  default:
    fatal("Missed vector creation for '%s'", NodeClassNames[opc]);
    return NULL;
  }
}

bool VectorNode::is_vector_shift(int opc) {
  assert(opc > _last_machine_leaf && opc < _last_opcode, "invalid opcode");
  switch (opc) {
  case Op_LShiftVB:
  case Op_LShiftVS:
  case Op_LShiftVI:
  case Op_LShiftVL:
  case Op_RShiftVB:
  case Op_RShiftVS:
  case Op_RShiftVI:
  case Op_RShiftVL:
  case Op_URShiftVB:
  case Op_URShiftVS:
  case Op_URShiftVI:
  case Op_URShiftVL:
    return true;
  default:
    return false;
  }
}

bool VectorNode::is_vector_shift_count(int opc) {
  assert(opc > _last_machine_leaf && opc < _last_opcode, "invalid opcode");
  switch (opc) {
  case Op_RShiftCntV:
  case Op_LShiftCntV:
    return true;
  default:
    return false;
  }
}

static bool is_con_M1(Node* n) {
  if (n->is_Con()) {
    const Type* t = n->bottom_type();
    if (t->isa_int() && t->is_int()->get_con() == -1) {
      return true;
    }
    if (t->isa_long() && t->is_long()->get_con() == -1) {
      return true;
    }
  }
  return false;
}

bool VectorNode::is_all_ones_vector(Node* n) {
  switch (n->Opcode()) {
  case Op_ReplicateB:
  case Op_ReplicateS:
  case Op_ReplicateI:
  case Op_ReplicateL:
    return is_con_M1(n->in(1));
  default:
    return false;
  }
}

bool VectorNode::is_vector_bitwise_not_pattern(Node* n) {
  if (n->Opcode() == Op_XorV) {
    return is_all_ones_vector(n->in(1)) ||
           is_all_ones_vector(n->in(2));
  }
  return false;
}

// Return initial Pack node. Additional operands added with add_opd() calls.
PackNode* PackNode::make(Node* s, uint vlen, BasicType bt) {
  const TypeVect* vt = TypeVect::make(bt, vlen);
  switch (bt) {
  case T_BOOLEAN:
  case T_BYTE:
    return new PackBNode(s, vt);
  case T_CHAR:
  case T_SHORT:
    return new PackSNode(s, vt);
  case T_INT:
    return new PackINode(s, vt);
  case T_LONG:
    return new PackLNode(s, vt);
  case T_FLOAT:
    return new PackFNode(s, vt);
  case T_DOUBLE:
    return new PackDNode(s, vt);
  default:
    fatal("Type '%s' is not supported for vectors", type2name(bt));
    return NULL;
  }
}

// Create a binary tree form for Packs. [lo, hi) (half-open) range
PackNode* PackNode::binary_tree_pack(int lo, int hi) {
  int ct = hi - lo;
  assert(is_power_of_2(ct), "power of 2");
  if (ct == 2) {
    PackNode* pk = PackNode::make(in(lo), 2, vect_type()->element_basic_type());
    pk->add_opd(in(lo+1));
    return pk;
  } else {
    int mid = lo + ct/2;
    PackNode* n1 = binary_tree_pack(lo,  mid);
    PackNode* n2 = binary_tree_pack(mid, hi );

    BasicType bt = n1->vect_type()->element_basic_type();
    assert(bt == n2->vect_type()->element_basic_type(), "should be the same");
    switch (bt) {
    case T_BOOLEAN:
    case T_BYTE:
      return new PackSNode(n1, n2, TypeVect::make(T_SHORT, 2));
    case T_CHAR:
    case T_SHORT:
      return new PackINode(n1, n2, TypeVect::make(T_INT, 2));
    case T_INT:
      return new PackLNode(n1, n2, TypeVect::make(T_LONG, 2));
    case T_LONG:
      return new Pack2LNode(n1, n2, TypeVect::make(T_LONG, 2));
    case T_FLOAT:
      return new PackDNode(n1, n2, TypeVect::make(T_DOUBLE, 2));
    case T_DOUBLE:
      return new Pack2DNode(n1, n2, TypeVect::make(T_DOUBLE, 2));
    default:
      fatal("Type '%s' is not supported for vectors", type2name(bt));
      return NULL;
    }
  }
}

// Return the vector version of a scalar load node.
LoadVectorNode* LoadVectorNode::make(int opc, Node* ctl, Node* mem,
                                     Node* adr, const TypePtr* atyp,
                                     uint vlen, BasicType bt,
                                     ControlDependency control_dependency) {
  const TypeVect* vt = TypeVect::make(bt, vlen);
  return new LoadVectorNode(ctl, mem, adr, atyp, vt, control_dependency);
}

// Return the vector version of a scalar store node.
StoreVectorNode* StoreVectorNode::make(int opc, Node* ctl, Node* mem,
                                       Node* adr, const TypePtr* atyp, Node* val,
                                       uint vlen) {
  return new StoreVectorNode(ctl, mem, adr, atyp, val);
}

int ExtractNode::opcode(BasicType bt) {
  switch (bt) {
    case T_BOOLEAN:
      return Op_ExtractUB;
    case T_BYTE:
      return Op_ExtractB;
    case T_CHAR:
      return Op_ExtractC;
    case T_SHORT:
      return Op_ExtractS;
    case T_INT:
      return Op_ExtractI;
    case T_LONG:
      return Op_ExtractL;
    case T_FLOAT:
      return Op_ExtractF;
    case T_DOUBLE:
      return Op_ExtractD;
    default:
      fatal("Type '%s' is not supported for vectors", type2name(bt));
      return 0;
  }
}

// Extract a scalar element of vector.
Node* ExtractNode::make(Node* v, uint position, BasicType bt) {
  assert((int)position < Matcher::max_vector_size(bt), "pos in range");
  ConINode* pos = ConINode::make((int)position);
  switch (bt) {
  case T_BOOLEAN:
    return new ExtractUBNode(v, pos);
  case T_BYTE:
    return new ExtractBNode(v, pos);
  case T_CHAR:
    return new ExtractCNode(v, pos);
  case T_SHORT:
    return new ExtractSNode(v, pos);
  case T_INT:
    return new ExtractINode(v, pos);
  case T_LONG:
    return new ExtractLNode(v, pos);
  case T_FLOAT:
    return new ExtractFNode(v, pos);
  case T_DOUBLE:
    return new ExtractDNode(v, pos);
  default:
    fatal("Type '%s' is not supported for vectors", type2name(bt));
    return NULL;
  }
}

int ReductionNode::opcode(int opc, BasicType bt) {
  int vopc = opc;
  switch (opc) {
    case Op_AddI:
      switch (bt) {
        case T_BOOLEAN:
        case T_CHAR: return 0;
        case T_BYTE:
        case T_SHORT:
        case T_INT:
          vopc = Op_AddReductionVI;
          break;
        default:          ShouldNotReachHere(); return 0;
      }
      break;
    case Op_AddL:
      assert(bt == T_LONG, "must be");
      vopc = Op_AddReductionVL;
      break;
    case Op_AddF:
      assert(bt == T_FLOAT, "must be");
      vopc = Op_AddReductionVF;
      break;
    case Op_AddD:
      assert(bt == T_DOUBLE, "must be");
      vopc = Op_AddReductionVD;
      break;
    case Op_MulI:
      switch (bt) {
        case T_BOOLEAN:
        case T_CHAR: return 0;
        case T_BYTE:
        case T_SHORT:
        case T_INT:
          vopc = Op_MulReductionVI;
          break;
        default:          ShouldNotReachHere(); return 0;
      }
      break;
    case Op_MulL:
      assert(bt == T_LONG, "must be");
      vopc = Op_MulReductionVL;
      break;
    case Op_MulF:
      assert(bt == T_FLOAT, "must be");
      vopc = Op_MulReductionVF;
      break;
    case Op_MulD:
      assert(bt == T_DOUBLE, "must be");
      vopc = Op_MulReductionVD;
      break;
    case Op_MinI:
      switch (bt) {
        case T_BOOLEAN:
        case T_CHAR: return 0;
        case T_BYTE:
        case T_SHORT:
        case T_INT:
          vopc = Op_MinReductionV;
          break;
        default: ShouldNotReachHere(); return 0;
      }
      break;
    case Op_MinL:
      assert(bt == T_LONG, "must be");
      vopc = Op_MinReductionV;
      break;
    case Op_MinF:
      assert(bt == T_FLOAT, "must be");
      vopc = Op_MinReductionV;
      break;
    case Op_MinD:
      assert(bt == T_DOUBLE, "must be");
      vopc = Op_MinReductionV;
      break;
    case Op_MaxI:
      switch (bt) {
        case T_BOOLEAN:
        case T_CHAR: return 0;
        case T_BYTE:
        case T_SHORT:
        case T_INT:
          vopc = Op_MaxReductionV;
          break;
        default: ShouldNotReachHere(); return 0;
      }
      break;
    case Op_MaxL:
      assert(bt == T_LONG, "must be");
      vopc = Op_MaxReductionV;
      break;
    case Op_MaxF:
      assert(bt == T_FLOAT, "must be");
      vopc = Op_MaxReductionV;
      break;
    case Op_MaxD:
      assert(bt == T_DOUBLE, "must be");
      vopc = Op_MaxReductionV;
      break;
    case Op_AndI:
      switch (bt) {
      case T_BOOLEAN:
      case T_CHAR: return 0;
      case T_BYTE:
      case T_SHORT:
      case T_INT:
        vopc = Op_AndReductionV;
        break;
      default: ShouldNotReachHere(); return 0;
      }
      break;
    case Op_AndL:
      assert(bt == T_LONG, "must be");
      vopc = Op_AndReductionV;
      break;
    case Op_OrI:
      switch(bt) {
      case T_BOOLEAN:
      case T_CHAR: return 0;
      case T_BYTE:
      case T_SHORT:
      case T_INT:
        vopc = Op_OrReductionV;
        break;
      default: ShouldNotReachHere(); return 0;
      }
      break;
    case Op_OrL:
      assert(bt == T_LONG, "must be");
      vopc = Op_OrReductionV;
      break;
    case Op_XorI:
      switch(bt) {
      case T_BOOLEAN:
      case T_CHAR: return 0;
      case T_BYTE:
      case T_SHORT:
      case T_INT:
        vopc = Op_XorReductionV;
        break;
      default: ShouldNotReachHere(); return 0;
      }
      break;
    case Op_XorL:
      assert(bt == T_LONG, "must be");
      vopc = Op_XorReductionV;
      break;
    default:
      break;
  }
  return vopc;
}

// Return the appropriate reduction node.
ReductionNode* ReductionNode::make(int opc, Node *ctrl, Node* n1, Node* n2, BasicType bt) {

  int vopc = opcode(opc, bt);

  // This method should not be called for unimplemented vectors.
  guarantee(vopc != opc, "Vector for '%s' is not implemented", NodeClassNames[opc]);

  switch (vopc) {
  case Op_AddReductionVI: return new AddReductionVINode(ctrl, n1, n2);
  case Op_AddReductionVL: return new AddReductionVLNode(ctrl, n1, n2);
  case Op_AddReductionVF: return new AddReductionVFNode(ctrl, n1, n2);
  case Op_AddReductionVD: return new AddReductionVDNode(ctrl, n1, n2);
  case Op_MulReductionVI: return new MulReductionVINode(ctrl, n1, n2);
  case Op_MulReductionVL: return new MulReductionVLNode(ctrl, n1, n2);
  case Op_MulReductionVF: return new MulReductionVFNode(ctrl, n1, n2);
  case Op_MulReductionVD: return new MulReductionVDNode(ctrl, n1, n2);
  case Op_MinReductionV:  return new MinReductionVNode(ctrl, n1, n2);
  case Op_MaxReductionV:  return new MaxReductionVNode(ctrl, n1, n2);
  case Op_AndReductionV:  return new AndReductionVNode(ctrl, n1, n2);
  case Op_OrReductionV:   return new OrReductionVNode(ctrl, n1, n2);
  case Op_XorReductionV:  return new XorReductionVNode(ctrl, n1, n2);
  default:
    fatal("Missed vector creation for '%s'", NodeClassNames[vopc]);
    return NULL;
  }
}

Node* VectorStoreMaskNode::make(PhaseGVN& gvn, Node* in, BasicType in_type, uint num_elem) {
  assert(in->bottom_type()->isa_vect(), "sanity");
  const TypeVect* vt = TypeVect::make(T_BOOLEAN, num_elem);
  int elem_size = type2aelembytes(in_type);
  return new VectorStoreMaskNode(in, gvn.intcon(elem_size), vt);
}

VectorCastNode* VectorCastNode::make(int vopc, Node* n1, BasicType bt, uint vlen) {
  const TypeVect* vt = TypeVect::make(bt, vlen);
  switch (vopc) {
    case Op_VectorCastB2X: return new VectorCastB2XNode(n1, vt);
    case Op_VectorCastS2X: return new VectorCastS2XNode(n1, vt);
    case Op_VectorCastI2X: return new VectorCastI2XNode(n1, vt);
    case Op_VectorCastL2X: return new VectorCastL2XNode(n1, vt);
    case Op_VectorCastF2X: return new VectorCastF2XNode(n1, vt);
    case Op_VectorCastD2X: return new VectorCastD2XNode(n1, vt);
    default: fatal("unknown node: %s", NodeClassNames[vopc]);
  }
  return NULL;
}

int VectorCastNode::opcode(BasicType bt) {
  switch (bt) {
    case T_BYTE:   return Op_VectorCastB2X;
    case T_SHORT:  return Op_VectorCastS2X;
    case T_INT:    return Op_VectorCastI2X;
    case T_LONG:   return Op_VectorCastL2X;
    case T_FLOAT:  return Op_VectorCastF2X;
    case T_DOUBLE: return Op_VectorCastD2X;
    default: Unimplemented();
  }
  return 0;
}

Node* ReductionNode::make_reduction_input(PhaseGVN& gvn, int opc, BasicType bt) {
  int vopc = opcode(opc, bt);
  guarantee(vopc != opc, "Vector reduction for '%s' is not implemented", NodeClassNames[opc]);

  switch (vopc) {
    case Op_AndReductionV:
      switch (bt) {
        case T_BYTE:
        case T_SHORT:
        case T_INT:
          return gvn.makecon(TypeInt::MINUS_1);
        case T_LONG:
          return gvn.makecon(TypeLong::MINUS_1);
        default:
          fatal("Missed vector creation for '%s' as the basic type is not correct.", NodeClassNames[vopc]);
          return NULL;
      }
      break;
    case Op_AddReductionVI: // fallthrough
    case Op_AddReductionVL: // fallthrough
    case Op_AddReductionVF: // fallthrough
    case Op_AddReductionVD:
    case Op_OrReductionV:
    case Op_XorReductionV:
      return gvn.zerocon(bt);
    case Op_MulReductionVI:
      return gvn.makecon(TypeInt::ONE);
    case Op_MulReductionVL:
      return gvn.makecon(TypeLong::ONE);
    case Op_MulReductionVF:
      return gvn.makecon(TypeF::ONE);
    case Op_MulReductionVD:
      return gvn.makecon(TypeD::ONE);
    case Op_MinReductionV:
      switch (bt) {
        case T_BYTE:
        case T_SHORT:
        case T_INT:
          return gvn.makecon(TypeInt::MAX);
        case T_LONG:
          return gvn.makecon(TypeLong::MAX);
        case T_FLOAT:
          return gvn.makecon(TypeF::POS_INF);
        case T_DOUBLE:
          return gvn.makecon(TypeD::POS_INF);
          default: Unimplemented(); return NULL;
      }
      break;
    case Op_MaxReductionV:
      switch (bt) {
        case T_BYTE:
        case T_SHORT:
        case T_INT:
          return gvn.makecon(TypeInt::MIN);
        case T_LONG:
          return gvn.makecon(TypeLong::MIN);
        case T_FLOAT:
          return gvn.makecon(TypeF::NEG_INF);
        case T_DOUBLE:
          return gvn.makecon(TypeD::NEG_INF);
          default: Unimplemented(); return NULL;
      }
      break;
    default:
      fatal("Missed vector creation for '%s'", NodeClassNames[vopc]);
      return NULL;
  }
}

bool ReductionNode::implemented(int opc, uint vlen, BasicType bt) {
  if (is_java_primitive(bt) &&
      (vlen > 1) && is_power_of_2(vlen) &&
      Matcher::vector_size_supported(bt, vlen)) {
    int vopc = ReductionNode::opcode(opc, bt);
    return vopc != opc && Matcher::match_rule_supported(vopc);
  }
  return false;
}

MacroLogicVNode* MacroLogicVNode::make(PhaseGVN& gvn, Node* in1, Node* in2, Node* in3,
                                       uint truth_table, const TypeVect* vt) {
  assert(truth_table <= 0xFF, "invalid");
  assert(in1->bottom_type()->is_vect()->length_in_bytes() == vt->length_in_bytes(), "mismatch");
  assert(in2->bottom_type()->is_vect()->length_in_bytes() == vt->length_in_bytes(), "mismatch");
  assert(in3->bottom_type()->is_vect()->length_in_bytes() == vt->length_in_bytes(), "mismatch");
  Node* fn = gvn.intcon(truth_table);
  return new MacroLogicVNode(in1, in2, in3, fn, vt);
}

#ifndef PRODUCT
void VectorMaskCmpNode::dump_spec(outputStream *st) const {
  st->print(" %d #", _predicate); _type->dump_on(st);
}
#endif // PRODUCT

Node* VectorReinterpretNode::Identity(PhaseGVN *phase) {
  Node* n = in(1);
  if (n->Opcode() == Op_VectorReinterpret) {
    if (Type::cmp(bottom_type(), n->in(1)->bottom_type()) == 0) {
      return n->in(1);
    }
  }
  return this;
}

Node* VectorInsertNode::make(Node* vec, Node* new_val, int position) {
  assert(position < (int)vec->bottom_type()->is_vect()->length(), "pos in range");
  ConINode* pos = ConINode::make(position);
  return new VectorInsertNode(vec, new_val, pos, vec->bottom_type()->is_vect());
}

Node* VectorUnboxNode::Identity(PhaseGVN *phase) {
  Node* n = obj()->uncast();
  if (EnableVectorReboxing && n->Opcode() == Op_VectorBox) {
    if (Type::cmp(bottom_type(), n->in(VectorBoxNode::Value)->bottom_type()) == 0) {
      return n->in(VectorBoxNode::Value);
    }
  }
  return this;
}

const TypeFunc* VectorBoxNode::vec_box_type(const TypeInstPtr* box_type) {
  const Type** fields = TypeTuple::fields(0);
  const TypeTuple *domain = TypeTuple::make(TypeFunc::Parms, fields);

  fields = TypeTuple::fields(1);
  fields[TypeFunc::Parms+0] = box_type;
  const TypeTuple *range = TypeTuple::make(TypeFunc::Parms+1, fields);

  return TypeFunc::make(domain, range);
}

#ifndef PRODUCT
void VectorBoxAllocateNode::dump_spec(outputStream *st) const {
  CallStaticJavaNode::dump_spec(st);
}
#endif // !PRODUCT
