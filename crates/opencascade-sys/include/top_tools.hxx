#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopoDS_Face.hxx>
#include <bindings_common.hxx>

// daedalus-fixes-v0.3.0 P8: emit the MSVC-only non-template overload
// `handle_try_deref(const Handle_TopTools_HSequenceOfShape &)`. No-op
// on POSIX (template in bindings_common.hxx is sufficient there).
DAEDALUS_HANDLE_TRY_DEREF_OVERLOAD(TopTools_HSequenceOfShape)

inline std::unique_ptr<Handle_TopTools_HSequenceOfShape> new_Handle_TopTools_HSequenceOfShape() {
  // daedalus-fixes-v0.3.0 P8: construct Handle_X directly (not via
  // opencascade::handle<X>). On MSVC Handle_X is a derived CLASS,
  // upstream's previous form `new opencascade::handle<X>(...)` produces a
  // base-class pointer that can't initialize unique_ptr<Handle_X>. Direct
  // construction works on both compilers (typedef on POSIX, class on MSVC).
  auto sequence = new TopTools_HSequenceOfShape();
  return std::unique_ptr<Handle_TopTools_HSequenceOfShape>(new Handle_TopTools_HSequenceOfShape(sequence));
}

inline void TopTools_HSequenceOfShape_append(Handle_TopTools_HSequenceOfShape &handle, const TopoDS_Shape &shape) {
  handle->Append(shape);
}

inline Standard_Integer TopTools_HSequenceOfShape_length(const Handle_TopTools_HSequenceOfShape &handle) {
  return handle->Length();
}

inline const TopoDS_Shape &TopTools_HSequenceOfShape_value(const Handle_TopTools_HSequenceOfShape &handle,
                                                           Standard_Integer index) {
  return handle->Value(index);
}
