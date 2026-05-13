# opencascade-rs (daedalus-fixes-v0.2.0)

This branch is a vendored snapshot of opencascade-sys 0.2.0 and occt-sys 0.2.0 from crates.io with the following fixes for Windows + CMake 4 + VS 2026:

- `occt-sys/build.rs`: add `CMAKE_POLICY_VERSION_MINIMUM=3.5` and force Release profile.
- `opencascade-sys/include/wrapper.hpp` (renamed from `.hxx`): remove redundant `Handle_Poly_Triangulation` typedef, add explicit `handle_try_deref` overload, retype `BRep_Tool_Triangulation` and `compute_normals` parameters.
- `opencascade-sys/src/lib.rs`: switch `include!` from `wrapper.hxx` to `wrapper.hpp`.

Used by [daedalus](https://github.com/brodiesmith/daedalus) via `[patch.crates-io]`.

**Do not PR upstream.** This is a project-local fork.
