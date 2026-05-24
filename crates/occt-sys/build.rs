const LIB_DIR: &str = "lib";
const INCLUDE_DIR: &str = "include";

fn main() {
    let current_dir = std::env::current_dir().expect("Should have a 'current' directory");
    let patch_dir = current_dir.join("patch");

    let dst = cmake::Config::new("OCCT")
        .define("CMAKE_POLICY_VERSION_MINIMUM", "3.5")
        // Force Release to avoid VS 2026 PDB-install incompatibility with
        // this version of OCCT's cmake_install scripts.
        .profile("Release")
        .define("BUILD_PATCH", patch_dir)
        .define("BUILD_LIBRARY_TYPE", "Static")
        // ApplicationFramework=TRUE enables the XCAF/STEPCAF chain
        // (TDocStd, XCAFApp, XCAFDoc, TDF, TDataStd, TNaming, TFunction,
        // CDM, CDF, PCDM, AppStd, AppStdL, persistence drivers). Required
        // by daedalus P-import Phase II+III (XCAF-aware STEP read+write).
        // Costs ~30% more OCCT build time vs. FALSE. Source dirs were
        // restored in the v0.2.6-prep commit (predecessor of this rev).
        .define("BUILD_MODULE_ApplicationFramework", "TRUE")
        .define("BUILD_MODULE_Draw", "FALSE")
        .define("USE_D3D", "FALSE")
        .define("USE_DRACO", "FALSE")
        .define("USE_EIGEN", "FALSE")
        .define("USE_FFMPEG", "FALSE")
        .define("USE_FREEIMAGE", "FALSE")
        .define("USE_FREETYPE", "FALSE")
        .define("USE_GLES2", "FALSE")
        .define("USE_OPENGL", "FALSE")
        .define("USE_OPENVR", "FALSE")
        .define("USE_RAPIDJSON", "FALSE")
        .define("USE_TBB", "FALSE")
        .define("USE_TCL", "FALSE")
        .define("USE_TK", "FALSE")
        .define("USE_VTK", "FALSE")
        .define("USE_XLIB", "FALSE")
        .define("INSTALL_DIR_LIB", LIB_DIR)
        .define("INSTALL_DIR_INCLUDE", INCLUDE_DIR)
        .build();

    println!(
        "cargo:rustc-env=OCCT_LIB_PATH={}",
        dst.join(LIB_DIR).to_str().expect("path is valid Unicode")
    );
    println!(
        "cargo:rustc-env=OCCT_INCLUDE_PATH={}",
        dst.join(INCLUDE_DIR).to_str().expect("path is valid Unicode")
    );
}
