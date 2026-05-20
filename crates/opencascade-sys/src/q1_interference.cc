// Q1 interference-binding orchestrator.
//
// Wraps BOPAlgo_CheckerSI to detect clashes between two arbitrary shapes:
// build a TopoDS_Compound of {shape_a, shape_b}, run the self-intersection
// checker on the compound, then walk the resulting BOPDS_DS picking the first
// inter-shape interference pair (filtering out intra-shape pairs). Out-params
// support_a_out / support_b_out are populated with the support sub-shapes;
// err_msg_out captures CheckerSI errors or Standard_Failure exception messages.
//
// `ClashFfi` is a cxx-rs shared struct declared in `opencascade-sys/src/lib.rs`
// (#[cxx::bridge] pub mod ffi). cxx-rs emits its full C++ definition in
// `lib.rs.h`, which we include here so the struct is complete at the point
// where we construct it.
//
// `support_a_kind` and `support_b_kind` carry OCCT's BRepExtrema_SupportType
// enumerator values as u32. We don't bind that enum into the cxx bridge:
// OCCT declares it as an unscoped C enum, and cxx-rs would otherwise emit a
// scoped `enum class` of the same name, which MSVC rejects as a conflicting
// redeclaration.

#include "opencascade-sys/include/wrapper.hpp"
#include "opencascade-sys/src/lib.rs.h"

#include <BOPAlgo_CheckerSI.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_VectorOfInterfEE.hxx>
#include <BOPDS_VectorOfInterfEF.hxx>
#include <BOPDS_VectorOfInterfFF.hxx>
#include <BOPDS_VectorOfInterfVE.hxx>
#include <BOPDS_VectorOfInterfVF.hxx>
#include <BOPDS_VectorOfInterfVV.hxx>
#include <BRep_Builder.hxx>
#include <BRepExtrema_SupportType.hxx>
#include <Standard_Failure.hxx>

#include <exception>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>

#include <cstdint>
#include <memory>
#include <sstream>
#include <string>

namespace {

void q1_build_origin_map(const TopoDS_Shape &s, TopTools_MapOfShape &out) {
  for (TopExp_Explorer e(s, TopAbs_VERTEX); e.More(); e.Next()) out.Add(e.Current());
  for (TopExp_Explorer e(s, TopAbs_EDGE); e.More(); e.Next()) out.Add(e.Current());
  for (TopExp_Explorer e(s, TopAbs_FACE); e.More(); e.Next()) out.Add(e.Current());
}

// Encodes OCCT's BRepExtrema_SupportType (0=IsVertex, 1=IsOnEdge, 2=IsInFace)
// as the matching u32. Non-vertex/edge/face inputs (compound / solid / shell /
// wire) fall back to IsInFace, matching the most-permissive surface support.
std::uint32_t q1_kind_of(const TopoDS_Shape &s) {
  switch (s.ShapeType()) {
    case TopAbs_VERTEX:
      return static_cast<std::uint32_t>(BRepExtrema_IsVertex);
    case TopAbs_EDGE:
      return static_cast<std::uint32_t>(BRepExtrema_IsOnEdge);
    case TopAbs_FACE:
      return static_cast<std::uint32_t>(BRepExtrema_IsInFace);
    default:
      return static_cast<std::uint32_t>(BRepExtrema_IsInFace);
  }
}

}  // anon namespace

ClashFfi check_interference(const TopoDS_Shape &shape_a, const TopoDS_Shape &shape_b,
                            std::unique_ptr<TopoDS_Shape> &support_a_out,
                            std::unique_ptr<TopoDS_Shape> &support_b_out, std::string &err_msg_out) {
  ClashFfi result{false, false,
                  static_cast<std::uint32_t>(BRepExtrema_IsVertex),
                  static_cast<std::uint32_t>(BRepExtrema_IsVertex)};
  try {
    // Wrap both inputs in a single compound and hand it to BOPAlgo_CheckerSI;
    // the populated BOPDS_DS then carries every interference pair found.
    TopoDS_Compound compound;
    BRep_Builder builder;
    builder.MakeCompound(compound);
    builder.Add(compound, shape_a);
    builder.Add(compound, shape_b);

    BOPAlgo_CheckerSI checker;
    TopTools_ListOfShape args;
    args.Append(compound);
    checker.SetArguments(args);
    checker.Perform();

    if (checker.HasErrors()) {
      std::stringstream ss;
      checker.DumpErrors(ss);
      err_msg_out = ss.str();
      result.has_errors = true;
      return result;
    }

    // Per-input origin maps let us distinguish A-sub-shapes from B-sub-shapes
    // when we walk the CheckerSI's interference pairs.
    TopTools_MapOfShape origin_a;
    TopTools_MapOfShape origin_b;
    q1_build_origin_map(shape_a, origin_a);
    q1_build_origin_map(shape_b, origin_b);

    // PDS() is non-const on BOPAlgo_PaveFiller (it returns BOPDS_DS*); the
    // Interf* accessors are also non-const, so we keep `ds` non-const here.
    BOPDS_DS *ds_ptr = checker.PDS();
    if (ds_ptr == nullptr) return result;
    BOPDS_DS &ds = *ds_ptr;

#define Q1_TRY_VECTOR(GETTER)                                                                                      \
  do {                                                                                                              \
    auto &vec = ds.GETTER();                                                                                        \
    for (Standard_Integer i = 0; i < vec.Size(); ++i) {                                                             \
      const auto &interf = vec(i);                                                                                  \
      Standard_Integer i1 = interf.Index1();                                                                        \
      Standard_Integer i2 = interf.Index2();                                                                        \
      const TopoDS_Shape &s1 = ds.Shape(i1);                                                                        \
      const TopoDS_Shape &s2 = ds.Shape(i2);                                                                        \
      bool s1_in_a = origin_a.Contains(s1);                                                                         \
      bool s2_in_b = origin_b.Contains(s2);                                                                         \
      bool s1_in_b = origin_b.Contains(s1);                                                                         \
      bool s2_in_a = origin_a.Contains(s2);                                                                         \
      if (s1_in_a && s2_in_b) {                                                                                     \
        support_a_out = std::make_unique<TopoDS_Shape>(s1);                                                         \
        support_b_out = std::make_unique<TopoDS_Shape>(s2);                                                         \
        result.interferes = true;                                                                                   \
        result.support_a_kind = q1_kind_of(s1);                                                                     \
        result.support_b_kind = q1_kind_of(s2);                                                                     \
        return result;                                                                                              \
      } else if (s1_in_b && s2_in_a) {                                                                              \
        support_a_out = std::make_unique<TopoDS_Shape>(s2);                                                         \
        support_b_out = std::make_unique<TopoDS_Shape>(s1);                                                         \
        result.interferes = true;                                                                                   \
        result.support_a_kind = q1_kind_of(s2);                                                                     \
        result.support_b_kind = q1_kind_of(s1);                                                                     \
        return result;                                                                                              \
      }                                                                                                             \
    }                                                                                                               \
  } while (0)

    Q1_TRY_VECTOR(InterfVV);
    Q1_TRY_VECTOR(InterfVE);
    Q1_TRY_VECTOR(InterfEE);
    Q1_TRY_VECTOR(InterfVF);
    Q1_TRY_VECTOR(InterfEF);
    Q1_TRY_VECTOR(InterfFF);
#undef Q1_TRY_VECTOR

    // No inter-shape interference (only intra-shape, or none at all).
    return result;
  } catch (const Standard_Failure &f) {
    err_msg_out =
        f.GetMessageString() ? std::string(f.GetMessageString()) : std::string("(no message)");
    result.has_errors = true;
    return result;
  } catch (const std::exception &e) {
    err_msg_out = e.what() ? std::string(e.what()) : std::string("(no message)");
    result.has_errors = true;
    return result;
  } catch (...) {
    err_msg_out = "(unknown exception)";
    result.has_errors = true;
    return result;
  }
}
