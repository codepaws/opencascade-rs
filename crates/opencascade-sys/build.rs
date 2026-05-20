use occt_sys::{occt_include_path, occt_lib_path};

fn main() {
    let target = std::env::var("TARGET").expect("No TARGET environment variable defined");
    let is_windows = target.to_lowercase().contains("windows");
    let is_windows_gnu = target.to_lowercase().contains("windows-gnu");

    println!("cargo:rustc-link-search=native={}", occt_lib_path().to_str().unwrap());
    println!("cargo:rustc-link-lib=static=TKMath");
    println!("cargo:rustc-link-lib=static=TKernel");
    println!("cargo:rustc-link-lib=static=TKFeat");
    println!("cargo:rustc-link-lib=static=TKGeomBase");
    println!("cargo:rustc-link-lib=static=TKG2d");
    println!("cargo:rustc-link-lib=static=TKG3d");
    println!("cargo:rustc-link-lib=static=TKTopAlgo");
    println!("cargo:rustc-link-lib=static=TKGeomAlgo");
    println!("cargo:rustc-link-lib=static=TKBRep");
    println!("cargo:rustc-link-lib=static=TKPrim");
    println!("cargo:rustc-link-lib=static=TKSTEP");
    println!("cargo:rustc-link-lib=static=TKSTEPAttr");
    println!("cargo:rustc-link-lib=static=TKSTEPBase");
    println!("cargo:rustc-link-lib=static=TKSTEP209");
    println!("cargo:rustc-link-lib=static=TKSTL");
    println!("cargo:rustc-link-lib=static=TKMesh");
    println!("cargo:rustc-link-lib=static=TKShHealing");
    println!("cargo:rustc-link-lib=static=TKFillet");
    println!("cargo:rustc-link-lib=static=TKBool");
    println!("cargo:rustc-link-lib=static=TKBO");
    println!("cargo:rustc-link-lib=static=TKOffset");
    println!("cargo:rustc-link-lib=static=TKXSBase");

    if is_windows {
        println!("cargo:rustc-link-lib=dylib=user32");
    }

    let mut build = cxx_build::bridge("src/lib.rs");

    if is_windows_gnu {
        build.define("OCC_CONVERT_SIGNALS", "TRUE");
    }

    build
        .cpp(true)
        // C++17: cxx-rs 1.x emits `inline` variables (the `unsafe_bitcopy`
        // shared-struct constant) which require c++17 on MSVC. OCCT
        // 7.x itself targets c++14+ and compiles cleanly under c++17.
        .flag_if_supported("-std=c++17")
        .flag_if_supported("/std:c++17")
        .define("_USE_MATH_DEFINES", "TRUE")
        .include(occt_include_path())
        .include("include")
        // Q1 interference-binding: BOPAlgo_CheckerSI orchestrator. Lives in
        // a separate .cc (not in wrapper.hpp) because its body needs the
        // full definition of the cxx-rs ClashFfi shared struct, which is
        // emitted in lib.rs.h *after* wrapper.hpp is included.
        .file("src/q1_interference.cc")
        .compile("wrapper");

    println!("cargo:rustc-link-lib=static=wrapper");

    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=include/wrapper.hxx");
    println!("cargo:rerun-if-changed=include/wrapper.hpp");
    println!("cargo:rerun-if-changed=src/q1_interference.cc");
}
