pub mod b_rep;
pub mod b_rep_adaptor;
pub mod b_rep_algo_api;
pub mod b_rep_bnd_lib;
pub mod b_rep_builder_api;
pub mod b_rep_feat;
pub mod b_rep_fillet_api;
pub mod b_rep_g_prop;
pub mod b_rep_int_curve_surface;
pub mod b_rep_lib;
pub mod b_rep_mesh;
pub mod b_rep_offset_api;
pub mod b_rep_prim_api;
pub mod b_rep_tools;
pub mod bin_tools;
pub mod bnd;
pub mod bop_algo;
pub mod g_prop;
pub mod gc;
pub mod gc_pnts;
pub mod geom;
pub mod geom2d;
pub mod geom_abs;
pub mod geom_api;
pub mod gp;
pub mod if_select;
pub mod iges_control;
pub mod law;
pub mod message;
pub mod poly;
pub mod shape_analysis;
pub mod shape_upgrade;
pub mod standard;
pub mod step_control;
pub mod stl_api;
pub mod t_col_gp;
pub mod top_abs;
pub mod top_exp;
pub mod top_loc;
pub mod top_tools;
pub mod topo_ds;

// daedalus-fixes-v0.3.0 P5: back-compat ffi:: flat namespace.
//
// Pre-#199 the cxx-bridge lived in a single `pub mod ffi { ... }` block, so
// downstreams (notably daedalus's 313 ffi::Foo call sites) addressed every
// binding as `opencascade_sys::ffi::Foo`. Upstream HEAD's split organizes the
// bindings into 41 per-feature modules at the crate root. This back-compat
// shim re-exports every module's contents flat under `ffi::*` so existing
// downstreams keep building without a sweep.
pub mod ffi {
    pub use crate::b_rep::*;
    pub use crate::b_rep_adaptor::*;
    pub use crate::b_rep_algo_api::*;
    pub use crate::b_rep_bnd_lib::*;
    pub use crate::b_rep_builder_api::*;
    pub use crate::b_rep_feat::*;
    pub use crate::b_rep_fillet_api::*;
    pub use crate::b_rep_g_prop::*;
    pub use crate::b_rep_int_curve_surface::*;
    pub use crate::b_rep_lib::*;
    pub use crate::b_rep_mesh::*;
    pub use crate::b_rep_offset_api::*;
    pub use crate::b_rep_prim_api::*;
    pub use crate::b_rep_tools::*;
    pub use crate::bin_tools::*;
    pub use crate::bnd::*;
    pub use crate::bop_algo::*;
    pub use crate::g_prop::*;
    pub use crate::gc::*;
    pub use crate::gc_pnts::*;
    pub use crate::geom::*;
    pub use crate::geom2d::*;
    pub use crate::geom_abs::*;
    pub use crate::geom_api::*;
    pub use crate::gp::*;
    pub use crate::if_select::*;
    pub use crate::iges_control::*;
    pub use crate::law::*;
    pub use crate::message::*;
    pub use crate::poly::*;
    pub use crate::shape_analysis::*;
    pub use crate::shape_upgrade::*;
    pub use crate::standard::*;
    pub use crate::step_control::*;
    pub use crate::stl_api::*;
    pub use crate::t_col_gp::*;
    pub use crate::top_abs::*;
    pub use crate::top_exp::*;
    pub use crate::top_loc::*;
    pub use crate::top_tools::*;
    pub use crate::topo_ds::*;
}
