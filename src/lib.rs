/*
Copyright (c) 2026 MOSEK ApS. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. All advertising materials mentioning features or use of this software must
display the following acknowledgement:
This product includes software developed by the the organization.

4. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDER "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT
HOLDER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
*/

//! MOSEK Core API for Rust.
//!
//! This API works by dynamically loading the MOSEK Core API library. This means that the library does not require the
//! MOSEK Core API library to be present at runtime and will only attemt to load it when the library is initialized.
//!
//! # Example
//!
//! Before any function can be used, the library list be initialized by calling `initialize()` or
//! `initialize_with_paths()`, which will load the actual library dynamically.
//!
//! ```
//! use mosek_stable_api as mca;
//!
//! fn main() {
//!     lo1().unwrap();
//! }
//!
//! fn lo1() -> Result<(),mca::APIError> {
//!     let msk = mca::initialize()?;
//!     // All the normal lo1 data:
//!     const numvar : i32 = 4;
//!     const numcon : i64 = 3;
//!     let cj : &[i32] = &[0,   1,   2,   3];
//!     let c  : &[f64] = &[3.0, 1.0, 5.0, 1.0];
//!     let rownum : &[i32]  = &[3, 4, 2 ];
//!     let subj : &[i32]    = &[0, 1, 2,
//!                              0, 1, 2, 3,
//!                                 1,    3 ];
//!     let valj : &[f64] = &[3.0, 1.0, 2.0,
//!                           2.0, 1.0, 3.0, 1.0,
//!                                2.0,      3.0 ];
//!     let blc : &[f64]  = &[30.0, 15.0,          f64::NEG_INFINITY ];
//!     let buc : &[f64]  = &[30.0, f64::INFINITY, 25.0 ];
//!     let blx : &[f64]  = &[ 0.0,           0.0,            0.0,           0.0 ];
//!     let bux : &[f64]  = &[ f64::INFINITY, 10.0, f64::INFINITY, f64::INFINITY ];
//!     // implementation
//!     msk.task()?
//!         .with_stream_callback(
//!             mca::StreamType::LOG,
//!             |msg| print!("{0}",msg),
//!             |task : &mut mca::Task| {
//!                 task.append_vars(numvar)?;
//!                 task.append_rows(4)?;
//!                 task.put_var_bound_slice(0,blx,bux)?;
//!                 task.put_row_slice(1,rownum, subj, valj)?;
//!                 task.append_vars(numvar)?;
//!                 task.append_rows(4)?;
//!                 task.put_var_bound_slice(0,blx,bux)?;
//!                 task.put_row_slice(1,rownum,subj,valj)?;
//!                 //int64_t domain_idx;
//!                 for (i,(&bl,&bu)) in blc.iter().zip(buc.iter()).enumerate() {
//!                     if bl.is_finite() {
//!                         let domidx = task.get_domain_rplus()?;
//!                         task.append_con(domidx,&[(i+1) as i64],Some(&[bl]))?;
//!                     }
//!                     if bu.is_finite() {
//!                         let domidx = task.get_domain_rminus()?;
//!                         task.append_con(domidx,&[(i+1) as i64],Some(&[bu]))?;
//!                     }
//!                 }
//!                 task.put_obj_sense(mca::ObjSense::MAXIMIZE);
//!                 task.put_row(0, cj, c)?;
//!                 task.put_obj_row(0)?;
//!
//!                 let trmcode = task.optimize()?;
//!
//!                 task.solution_summary(mca::StreamType::MSG)?;
//!
//!                const solidx : i32 = 0;
//!                match task.get_sol_status(solidx)? {
//!                    (mca::SolSta::OPTIMAL,_) => {
//!                        let mut xx = vec![0.0; numvar as usize];
//!                        task.get_sol_xx_slice(solidx, 0, &mut xx)?;
//!                        println!("xx: {:?}\n", xx);
//!                    },
//!                    (mca::SolSta::INFEAS_CERT,_)|(_,mca::SolSta::INFEAS_CERT) => {
//!                        println!("Primal or dual infeasibility certificate found.");
//!                    },
//!                    (mca::SolSta::ILLPOSED_CERT,_)|(_,mca::SolSta::ILLPOSED_CERT) => {
//!                        println!("Primal or dual illposed certificate found.");
//!                    },
//!                    _ => {
//!                        println!("Other solution status");
//!                    }
//!                }
//!
//!                Ok(())
//!            })
//!}
//! ```
//! <script type="text/javascript" id="MathJax-script" async src="https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-chtml.js"> </script>


use std::ffi::CString;
use std::ffi::CStr;
use std::ffi::{c_void,c_char};
use std::convert::TryInto;
use std::default::Default;


#[allow(non_camel_case_types)]
type Task_t = * mut c_void;
#[allow(non_camel_case_types)]
type c_void_p = * mut c_void;
#[allow(non_camel_case_types)]
#[allow(unused)]
pub type ResCode = i32;
#[allow(non_camel_case_types)]
#[allow(unused)]
pub type TrmCode = i32;
#[allow(non_camel_case_types)]
#[allow(unused)]
pub type ReadHandle = c_void_p;
#[allow(non_camel_case_types)]
#[allow(unused)]
pub type WriteHandle = c_void_p;
#[allow(non_camel_case_types)]
#[allow(unused)]
type  MSK12_ReadFunc = extern "C" fn (h : c_void_p,dest : c_void_p,num : usize) -> usize;
#[allow(non_camel_case_types)]
#[allow(unused)]
type  MSK12_WriteFunc = extern "C" fn (h : c_void_p,src : c_void_p,num : usize) -> usize;
#[allow(non_camel_case_types)]
#[allow(unused)]
type  MSK12_StreamFunc = extern "C" fn (h : c_void_p,src : *const c_char);
#[allow(non_camel_case_types)]
#[allow(unused)]
pub type CallbackHandle = c_void_p;
#[allow(non_camel_case_types)]
#[allow(unused)]
pub type ErrorCallbackHandle = c_void_p;
#[allow(non_camel_case_types)]
#[allow(unused)]
type  MSK12_ErrorCallbackFunc = extern "C" fn (h : c_void_p,r : i32,name : *const c_char,desc : *const c_char,message : *const c_char) -> i32;
#[allow(non_camel_case_types)]
#[allow(unused)]
type  MSK12_CallbackFunc = extern "C" fn (h : c_void_p,code : i32,len_iinf : i32,iinf : *const i32,len_liinf : i32,liinf : *const i64,len_dinf : i32,dinf : *const f64) -> i32;
#[allow(non_camel_case_types)]
#[allow(unused)]
type  MSK12_IntSolCallbackFunc = extern "C" fn (handle : c_void_p,num : i32,primal_obj : f64,xx : *const f64);
#[allow(non_camel_case_types)]
#[allow(unused)]
pub type AllocHandle = c_void_p;
#[allow(non_camel_case_types)]
#[allow(unused)]
type  MSK12_AllocFunc = extern "C" fn (handle : c_void_p,num : usize) -> *mut u8;


#[allow(non_camel_case_types)]
#[derive(Debug,Copy,Clone)]
pub enum DomainType {
  NIL = 0,
  RZERO = 1,
  RPLUS = 2,
  RMINUS = 3,
  R = 4,
  QUADRATIC_CONE = 5,
  ROTATED_QUADRATIC_CONE = 6,
  PRIMAL_EXP_CONE = 7,
  DUAL_EXP_CONE = 8,
  PRIMAL_POWER_CONE = 9,
  DUAL_POWER_CONE = 10,
  PRIMAL_GEOMETRIC_MEAN_CONE = 11,
  DUAL_GEOMETRIC_MEAN_CONE = 12,
  SVEC_PSD_CONE = 13,
}
impl DomainType {
#[allow(unused)]
  fn from(i : i32) -> Result<DomainType,i32> {
    match i {
      0 => Ok(DomainType::NIL),
      1 => Ok(DomainType::RZERO),
      2 => Ok(DomainType::RPLUS),
      3 => Ok(DomainType::RMINUS),
      4 => Ok(DomainType::R),
      5 => Ok(DomainType::QUADRATIC_CONE),
      6 => Ok(DomainType::ROTATED_QUADRATIC_CONE),
      7 => Ok(DomainType::PRIMAL_EXP_CONE),
      8 => Ok(DomainType::DUAL_EXP_CONE),
      9 => Ok(DomainType::PRIMAL_POWER_CONE),
      10 => Ok(DomainType::DUAL_POWER_CONE),
      11 => Ok(DomainType::PRIMAL_GEOMETRIC_MEAN_CONE),
      12 => Ok(DomainType::DUAL_GEOMETRIC_MEAN_CONE),
      13 => Ok(DomainType::SVEC_PSD_CONE),
      ii => Err(ii)
    } // match
  } // from
} // impl DomainType
#[allow(non_camel_case_types)]
#[derive(Debug,Copy,Clone)]
pub enum Feature {
  PTON = 0,
  PTS = 1,
}
impl Feature {
#[allow(unused)]
  fn from(i : i32) -> Result<Feature,i32> {
    match i {
      0 => Ok(Feature::PTON),
      1 => Ok(Feature::PTS),
      ii => Err(ii)
    } // match
  } // from
} // impl Feature
#[allow(non_camel_case_types)]
#[derive(Debug,Copy,Clone)]
pub enum ObjSense {
  MINIMIZE = 0,
  MAXIMIZE = 1,
}
impl ObjSense {
#[allow(unused)]
  fn from(i : i32) -> Result<ObjSense,i32> {
    match i {
      0 => Ok(ObjSense::MINIMIZE),
      1 => Ok(ObjSense::MAXIMIZE),
      ii => Err(ii)
    } // match
  } // from
} // impl ObjSense
#[allow(non_camel_case_types)]
#[derive(Debug,Copy,Clone)]
pub enum SolType {
  BASIC = 0,
  INTERIOR = 1,
  INTEGER = 2,
  UNKNOWN = 3,
}
impl SolType {
#[allow(unused)]
  fn from(i : i32) -> Result<SolType,i32> {
    match i {
      0 => Ok(SolType::BASIC),
      1 => Ok(SolType::INTERIOR),
      2 => Ok(SolType::INTEGER),
      3 => Ok(SolType::UNKNOWN),
      ii => Err(ii)
    } // match
  } // from
} // impl SolType
#[allow(non_camel_case_types)]
#[derive(Debug,Copy,Clone)]
pub enum SolSta {
  UNKNOWN = 0,
  UNDEFINED = 1,
  OPTIMAL = 2,
  INTEGER_OPTIMAL = 3,
  FEASIBLE = 4,
  INFEAS_CERT = 5,
  ILLPOSED_CERT = 6,
}
impl SolSta {
#[allow(unused)]
  fn from(i : i32) -> Result<SolSta,i32> {
    match i {
      0 => Ok(SolSta::UNKNOWN),
      1 => Ok(SolSta::UNDEFINED),
      2 => Ok(SolSta::OPTIMAL),
      3 => Ok(SolSta::INTEGER_OPTIMAL),
      4 => Ok(SolSta::FEASIBLE),
      5 => Ok(SolSta::INFEAS_CERT),
      6 => Ok(SolSta::ILLPOSED_CERT),
      ii => Err(ii)
    } // match
  } // from
} // impl SolSta
#[allow(non_camel_case_types)]
#[derive(Debug,Copy,Clone)]
pub enum ProSta {
  UNKNOWN = 0,
  PRIMAL_AND_DUAL_FEASIBLE = 1,
  PRIMAL_FEASIBLE = 2,
  DUAL_FEASIBLE = 3,
  PRIMAL_INFEASIBLE = 4,
  DUAL_INFEASIBLE = 5,
  PRIMAL_AND_DUAL_INFEASIBLE = 6,
  ILLPOSED = 7,
  PRIMAL_INFEASIBLE_OR_UNBOUNDED = 8,
}
impl ProSta {
#[allow(unused)]
  fn from(i : i32) -> Result<ProSta,i32> {
    match i {
      0 => Ok(ProSta::UNKNOWN),
      1 => Ok(ProSta::PRIMAL_AND_DUAL_FEASIBLE),
      2 => Ok(ProSta::PRIMAL_FEASIBLE),
      3 => Ok(ProSta::DUAL_FEASIBLE),
      4 => Ok(ProSta::PRIMAL_INFEASIBLE),
      5 => Ok(ProSta::DUAL_INFEASIBLE),
      6 => Ok(ProSta::PRIMAL_AND_DUAL_INFEASIBLE),
      7 => Ok(ProSta::ILLPOSED),
      8 => Ok(ProSta::PRIMAL_INFEASIBLE_OR_UNBOUNDED),
      ii => Err(ii)
    } // match
  } // from
} // impl ProSta
#[allow(non_camel_case_types)]
#[derive(Debug,Copy,Clone)]
pub enum Format {
  PTF = 0,
  TASK = 1,
  JTASK = 2,
}
impl Format {
#[allow(unused)]
  fn from(i : i32) -> Result<Format,i32> {
    match i {
      0 => Ok(Format::PTF),
      1 => Ok(Format::TASK),
      2 => Ok(Format::JTASK),
      ii => Err(ii)
    } // match
  } // from
} // impl Format
#[allow(non_camel_case_types)]
#[derive(Debug,Copy,Clone)]
pub enum VariableType {
  INTEGER = 0,
  CONTINUOUS = 1,
}
impl VariableType {
#[allow(unused)]
  fn from(i : i32) -> Result<VariableType,i32> {
    match i {
      0 => Ok(VariableType::INTEGER),
      1 => Ok(VariableType::CONTINUOUS),
      ii => Err(ii)
    } // match
  } // from
} // impl VariableType
#[allow(non_camel_case_types)]
#[derive(Debug,Copy,Clone)]
pub enum Compression {
  NONE = 0,
  GZIP = 1,
  ZSTD = 2,
}
impl Compression {
#[allow(unused)]
  fn from(i : i32) -> Result<Compression,i32> {
    match i {
      0 => Ok(Compression::NONE),
      1 => Ok(Compression::GZIP),
      2 => Ok(Compression::ZSTD),
      ii => Err(ii)
    } // match
  } // from
} // impl Compression
#[allow(non_camel_case_types)]
#[derive(Debug,Copy,Clone)]
pub enum SolutionFormat {
  TASK = 0,
  JTASK = 1,
  TEXT = 2,
}
impl SolutionFormat {
#[allow(unused)]
  fn from(i : i32) -> Result<SolutionFormat,i32> {
    match i {
      0 => Ok(SolutionFormat::TASK),
      1 => Ok(SolutionFormat::JTASK),
      2 => Ok(SolutionFormat::TEXT),
      ii => Err(ii)
    } // match
  } // from
} // impl SolutionFormat
#[allow(non_camel_case_types)]
#[derive(Debug,Copy,Clone)]
pub enum StreamType {
  MSG = 0,
  WRN = 1,
  ERR = 2,
  LOG = 3,
}
impl StreamType {
#[allow(unused)]
  fn from(i : i32) -> Result<StreamType,i32> {
    match i {
      0 => Ok(StreamType::MSG),
      1 => Ok(StreamType::WRN),
      2 => Ok(StreamType::ERR),
      3 => Ok(StreamType::LOG),
      ii => Err(ii)
    } // match
  } // from
} // impl StreamType


unsafe extern "C" {
    fn MSK12_initialize_library_with_paths(paths : * const * const c_char) -> i32;
    fn MSK12_initialize_library() -> i32;
    fn MSK12_library_initialized() -> i32;

    #[allow(unused)]
    fn MSK12_get_callback_code_name(code : i32) -> * const c_char;
    #[allow(unused)]
    fn MSK12_get_resp_name(r : i32) -> * const c_char;
    #[allow(unused)]
    fn MSK12_get_resp_descr(r : i32) -> * const c_char;
    #[allow(unused)]
    fn MSK12_get_last_resp(task : Task_t) -> i32;
    #[allow(unused)]
    fn MSK12_get_last_resp_msg(task : Task_t,buf : *mut c_char,buf_len : usize) -> i32;
    #[allow(unused)]
    fn MSK12_get_last_resp_msg_len(task : Task_t) -> usize;
    #[allow(unused)]
    fn MSK12_get_trm_name(trm : i32) -> * const c_char;
    #[allow(unused)]
    fn MSK12_get_trm_descr(trm : i32) -> * const c_char;
    #[allow(unused)]
    fn MSK12_new_task_from_task(task : Task_t) -> Task_t;
    #[allow(unused)]
    fn MSK12_new_task() -> Task_t;
    #[allow(unused)]
    fn MSK12_delete_task(task : Task_t);
    #[allow(unused)]
    fn MSK12_reserve_num_var(task : Task_t,add_num : i32) -> i32;
    #[allow(unused)]
    fn MSK12_reserve_num_barvar(task : Task_t,num_barvar : i32) -> i32;
    #[allow(unused)]
    fn MSK12_reserve_num_con(task : Task_t,num_con : i32) -> i32;
    #[allow(unused)]
    fn MSK12_reserve_num_row(task : Task_t,num_row : i64) -> i32;
    #[allow(unused)]
    fn MSK12_reserve_num_nz(task : Task_t,num_nz : i64) -> i32;
    #[allow(unused)]
    fn MSK12_reserve_num_dom(task : Task_t,num_dom : i64) -> i32;
    #[allow(unused)]
    fn MSK12_reserve_num_symmat(task : Task_t,num_symmat : i64) -> i32;
    #[allow(unused)]
    fn MSK12_reserve_num_symmat_nz(task : Task_t,num_nz : i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_num_var(task : Task_t) -> i32;
    #[allow(unused)]
    fn MSK12_get_num_barvar(task : Task_t) -> i32;
    #[allow(unused)]
    fn MSK12_get_num_domain(task : Task_t) -> i64;
    #[allow(unused)]
    fn MSK12_get_num_row(task : Task_t) -> i64;
    #[allow(unused)]
    fn MSK12_get_num_symmat(task : Task_t) -> i64;
    #[allow(unused)]
    fn MSK12_get_num_con(task : Task_t) -> i64;
    #[allow(unused)]
    fn MSK12_get_num_djc(task : Task_t) -> i64;
    #[allow(unused)]
    fn MSK12_append_vars(task : Task_t,num_var : i32) -> i32;
    #[allow(unused)]
    fn MSK12_append_rows(task : Task_t,num_row : i64) -> i32;
    #[allow(unused)]
    fn MSK12_append_barvar(task : Task_t,dim : i32) -> i32;
    #[allow(unused)]
    fn MSK12_append_barvars(task : Task_t,num_barvar : i32,dims : *const i32) -> i32;
    #[allow(unused)]
    fn MSK12_append_symmat(task : Task_t,dim : i32,nnz : i64,symmat_i : *const i32,symmat_j : *const i32,symmat_val : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_append_symmats(task : Task_t,num_symmat : i64,dim : *const i32,nnz : *const i64,symmat_i : *const i32,symmat_j : *const i32,symmat_val : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_append_empty_cons(task : Task_t,num_con : i64) -> i32;
    #[allow(unused)]
    fn MSK12_append_empty_djcs(task : Task_t,num_djc : i64) -> i32;
    #[allow(unused)]
    fn MSK12_put_var_type(task : Task_t,j : i32,var_type : i32) -> i32;
    #[allow(unused)]
    fn MSK12_put_var_type_slice(task : Task_t,first_var : i32,num_var : i32,var_types : *const i32) -> i32;
    #[allow(unused)]
    fn MSK12_put_var_type_slice_value(task : Task_t,first_var : i32,num_var : i32,var_type : i32) -> i32;
    #[allow(unused)]
    fn MSK12_put_var_type_list(task : Task_t,num_var : i32,var_idxs : *const i32,var_types : *const i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_var_type(task : Task_t,var_idx : i32,var_type : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_var_type_slice(task : Task_t,first_var : i32,num_var : i32,var_types : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_put_var_bound(task : Task_t,var_idx : i32,low : f64,upr : f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_var_bound_slice(task : Task_t,first_var : i32,num_var : i32,low : *const f64,upr : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_var_bound_slice_value(task : Task_t,first_var : i32,num_var : i32,low : f64,upr : f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_var_low_bound(task : Task_t,var_idx : i32,low : f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_var_low_bound_slice(task : Task_t,first_var : i32,num_var : i32,low : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_var_low_bound_slice_value(task : Task_t,first_var : i32,num_var : i32,low : f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_var_upr_bound(task : Task_t,var_idx : i32,upr : f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_var_upr_bound_slice(task : Task_t,first_var : i32,num_var : i32,upr : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_var_upr_bound_slice_value(task : Task_t,first_var : i32,num_var : i32,upr : f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_var_bound(task : Task_t,var_idx : i32,low : *mut f64,upr : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_var_bound_slice(task : Task_t,first_var : i32,num_var : i32,low : *mut f64,upr : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_barvar_slice_num_elm(task : Task_t,first_barvar : i32,num_barvar : i32,num_elm : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_barvar_dim(task : Task_t,barvar_idx : i32,dim : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_barvar_slice_dims(task : Task_t,first_barvar : i32,num_barvar : i32,dim : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_domain(task : Task_t,dom_type : i32,dim : i64,num_alpha : i32,alpha : *mut f64,dom_idx : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_domain_empty(task : Task_t,dom_idx : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_domain_rzero(task : Task_t,dom_idx : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_domain_rplus(task : Task_t,dom_idx : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_domain_rminus(task : Task_t,dom_idx : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_domain_r(task : Task_t,dom_idx : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_domain_quadratic_cone(task : Task_t,n : i64,dom_idx : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_domain_rotated_quadratic_cone(task : Task_t,n : i64,dom_idx : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_domain_primal_exponential_cone(task : Task_t,dom_idx : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_domain_dual_exponential_cone(task : Task_t,dom_idx : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_domain_primal_power_cone(task : Task_t,n : i64,num_alpha : i64,alpha : *const f64,dom_idx : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_domain_dual_power_cone(task : Task_t,n : i64,num_alpha : i64,alpha : *const f64,dom_idx : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_domain_primal_geometric_mean_cone(task : Task_t,n : i64,dom_idx : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_domain_dual_geometric_mean_cone(task : Task_t,n : i64,dom_idx : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_domain_svecpsd_cone(task : Task_t,n : i64,dom_idx : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_domain_info(task : Task_t,dom_idx : i64,dom_type : *mut i32,size : *mut i64,num_alpha : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_domain_alpha(task : Task_t,dom_idx : i64,num_alpha : i64,alpha : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_row(task : Task_t,row_idx : i64,num_nz : i32,subj : *const i32,cof : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_row_slice(task : Task_t,first_row : i64,num_row : i64,row_num_nz : *const i32,subj : *const i32,cof : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_row_list(task : Task_t,num_row : i64,row_idxs : *const i64,row_num_nz : *const i32,subj : *const *const i32,cof : *const *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_row_g(task : Task_t,row_idx : i64,g : f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_row_slice_g(task : Task_t,first_row : i64,num_row : i64,g : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_row_list_g(task : Task_t,num_row : i64,row_idxs : *const i64,g : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_col(task : Task_t,col_idx : i32,num_nz : i64,row_idxs : *const i64,cof : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_col_slice(task : Task_t,first_col : i32,num_col : i32,col_len : *const i64,row_idxs : *const i64,cof : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_col_list(task : Task_t,num_col : i32,col_idxs : *const i32,col_lens : *const i64,row_idxs : *const *const i64,cof : *const *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_ijc(task : Task_t,row_idx : i64,var_idx : i32,cof : f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_ijc_list(task : Task_t,num_nz : i64,row_idxs : *const i64,col_idxs : *const i32,cof : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_row_num_nz(task : Task_t,row_idx : i64,num_nz : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_row_slice_num_nz(task : Task_t,first_row : i64,num_row : i64,num_nz : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_row(task : Task_t,row_idx : i64,nnz : i32,subj : *mut i32,cof : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_row_slice(task : Task_t,first_row : i64,num_row : i64,nnz : i64,row_len : *mut i32,subj : *mut i32,cof : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_col_num_nz(task : Task_t,col_idx : i32,num_nz : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_col_slice_num_nz(task : Task_t,first_col : i32,num_col : i32,num_nz : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_col(task : Task_t,col_idx : i32,nnz : i64,subi : *mut i64,cof : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_col_slice(task : Task_t,first_col : i32,num_col : i32,nnz : i64,col_len : *mut i64,subi : *mut i64,cof : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_bar_entry(task : Task_t,row_idx : i64,barvar_idx : i32,num_weight : i64,matrix_idx : *const i64,weight : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_bar_entry_list(task : Task_t,num_bar_entry : i64,row_idx : *const i64,barvar_idx : *const i32,num_weight : *const i64,matrix_idx : *const i64,weight : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_bar_row(task : Task_t,row_idx : i64,num_bar_entry : i32,barvar_idx : *const i32,num_weight : *const i64,matrix_idx : *const i64,weight : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_symmat_info(task : Task_t,symmat_idx : i64,dim : *mut i32,nnz : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_symmat(task : Task_t,symmat_idx : i64,nnz : i64,symmat_i : *mut i32,symmat_j : *mut i32,symmat_val : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_symmat_slice_info(task : Task_t,first_symmat : i64,num_symmat : i64,dim : *mut i32,nnz : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_symmat_slice(task : Task_t,first_symmat : i64,num_symmat : i64,total_nnz : i64,symmat_i : *mut i32,symmat_j : *mut i32,symmat_val : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_append_con(task : Task_t,dom_idx : i64,num_rows : i64,row_idxs : *const i64,con_offset : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_append_cons(task : Task_t,num_con : i64,dom_idxs : *const i64,num_rows : *const i64,row_idxs : *const i64,con_offset : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_con(task : Task_t,con_idx : i64,num_rows : i64,dom_idx : i64,row_idxs : *const i64,rhs_offset : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_scalar_con(task : Task_t,con_idx : i64,dom_idx : i64,row_idx : i64,rhs_offset : f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_con_slice(task : Task_t,first_con : i64,num_con : i64,num_rows : i64,dom_idx : *const i64,row_idx : *const i64,rhs_offset : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_con_slice_domains(task : Task_t,first_con : i64,num_con : i64,dom_idx : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_con_slice_num_row(task : Task_t,first_con : i64,num_con : i64,num_row : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_con_slice(task : Task_t,first_con : i64,num_con : i64,num_row : i64,row_idx : *mut i64,rhs_offset : *mut f64,dom_idx : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_append_djc(task : Task_t,num_rows : i64,num_dom : i64,num_terms : i64,dom_idx : *const i64,term_size : *const i64,row_idx : *const i64,rhs_offset : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_djc(task : Task_t,djc_idx : i64,num_rows : i64,num_dom : i64,num_terms : i64,dom_idx : *const i64,term_size : *const i64,row_idx : *const i64,rhs_offset : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_djc_slice(task : Task_t,first_djc : i64,num_djc : i64,num_rows : i64,num_dom : i64,num_terms : i64,dom_idx : *const i64,term_size : *const i64,row_idx : *const i64,rhs_offset : *const f64,djc_numterm : *const i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_djc_info(task : Task_t,djc_idx : i64,num_term : *mut i64,num_dom : *mut i64,num_row : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_djc(task : Task_t,djc_idx : i64,num_terms : i64,num_dom : i64,num_row : i64,term_size : *mut i64,dom_idx : *mut i64,row_idx : *mut i64,rhs_offset : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_djc_slice_info(task : Task_t,first_djc : i64,num_djc : i64,num_term : *mut i64,num_dom : *mut i64,num_row : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_djc_slice(task : Task_t,first_djc : i64,num_djc : i64,num_term : i64,num_dom : i64,num_row : i64,term_size : *mut i64,dom_idx : *mut i64,row_idx : *mut i64,rhs_offset : *mut f64,djc_num_term : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_put_obj_sense(task : Task_t,sense : i32);
    #[allow(unused)]
    fn MSK12_get_obj_sense(task : Task_t) -> i32;
    #[allow(unused)]
    fn MSK12_put_obj_row(task : Task_t,row_idx : i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_obj_row(task : Task_t,row_idx : *mut i64,asgn : *mut i32);
    #[allow(unused)]
    fn MSK12_optimize(task : Task_t,trm : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_solution_summary(task : Task_t,whichstream : i32) -> i32;
    #[allow(unused)]
    fn MSK12_optimize_callback(task : Task_t,trm : *mut i32,cb_handle : c_void_p,cb_func : Option<extern "C" fn (h : c_void_p,code : i32,len_iinf : i32,iinf : *const i32,len_liinf : i32,liinf : *const i64,len_dinf : i32,dinf : *const f64) -> i32>,int_cb_handle : c_void_p,int_cb_func : Option<extern "C" fn (handle : c_void_p,num : i32,primal_obj : f64,xx : *const f64)>) -> i32;
    #[allow(unused)]
    fn MSK12_put_remote_solver(task : Task_t,server : *const c_char,cert : *const c_char);
    #[allow(unused)]
    fn MSK12_put_optserver_access_token(task : Task_t,token : *const c_char);
    #[allow(unused)]
    fn MSK12_get_num_sol(task : Task_t) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_type(task : Task_t,sol_idx : i32,sol_type : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_status(task : Task_t,sol_idx : i32,primal_sol_sta : *mut i32,dual_sol_sta : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_problem_status(task : Task_t,sol_idx : i32,pro_sta : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_primal_obj(task : Task_t,sol_idx : i32,obj_val : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_dual_obj(task : Task_t,sol_idx : i32,obj_val : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_xx_slice(task : Task_t,sol_idx : i32,first_var : i32,num_var : i32,xx : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_slx_slice(task : Task_t,sol_idx : i32,first_var : i32,num_var : i32,slx : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_sux_slice(task : Task_t,sol_idx : i32,first_var : i32,num_var : i32,sux : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_barxj(task : Task_t,sol_idx : i32,barvar_idx : i32,num_var : i64,barx : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_barsj(task : Task_t,sol_idx : i32,barvar_idx : i32,num_elm : i64,bars : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_barx_slice(task : Task_t,sol_idx : i32,first_barvar : i32,num_barvar : i32,num_elm : i64,barx : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_bars_slice(task : Task_t,sol_idx : i32,first_barvar : i32,num_barvar : i32,num_elm : i64,bars : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_basic_xj(task : Task_t,sol_idx : i32,var_idx : i32,basic : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_basic_barx(task : Task_t,sol_idx : i32,barvar_idx : i32,basic : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_basic_con(task : Task_t,sol_idx : i32,con_idx : i64,basic : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_sta_var(task : Task_t,sol_idx : i32,var_idx : i32,low_binding : *mut i32,upr_binding : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_sta_barx(task : Task_t,sol_idx : i32,barvar_idx : i32,binding : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_sta_con(task : Task_t,sol_idx : i32,con_idx : i64,binding : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_basic_x_slice(task : Task_t,sol_idx : i32,first_var : i32,num_var : i32,basic : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_basic_barx_slice(task : Task_t,sol_idx : i32,first_var : i32,num_var : i32,basic : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_basic_con_slice(task : Task_t,sol_idx : i32,first_con : i64,num_con : i64,basic : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_sta_var_slice(task : Task_t,sol_idx : i32,first_var : i32,num_var : i32,low_binding : *mut i32,upr_binding : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_sta_barx_slice(task : Task_t,sol_idx : i32,first_barvar : i32,num_barvar : i32,bindnig : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_sta_con_slice(task : Task_t,sol_idx : i32,first_con : i64,num_con : i64,binding : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_xc_slice(task : Task_t,sol_idx : i32,first_con : i64,num_con : i64,num_elm : i64,xc : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_sol_y_slice(task : Task_t,sol_idx : i32,first_con : i64,num_con : i64,num_elm : i64,y : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_num_input_solutions(task : Task_t) -> i32;
    #[allow(unused)]
    fn MSK12_copy_sol_to_input(task : Task_t,sol_idx : i32) -> i32;
    #[allow(unused)]
    fn MSK12_append_sol(task : Task_t,soltype : i32) -> i32;
    #[allow(unused)]
    fn MSK12_put_sol_xx(task : Task_t,sol_idx : i32,num : i32,val : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_sol_slx(task : Task_t,sol_idx : i32,num : i32,val : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_sol_sux(task : Task_t,sol_idx : i32,num : i32,val : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_sol_basic_x(task : Task_t,sol_idx : i32,num : i32,val : *const i32) -> i32;
    #[allow(unused)]
    fn MSK12_put_sol_barx(task : Task_t,sol_idx : i32,num : i64,val : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_sol_bars(task : Task_t,sol_idx : i32,num : i64,xx : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_sol_yi(task : Task_t,sol_idx : i32,i : i64,num : i64,xx : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_put_sol_basic_c(task : Task_t,sol_idx : i32,num : i64,val : *const i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_num_iinf() -> i32;
    #[allow(unused)]
    fn MSK12_get_num_liinf() -> i32;
    #[allow(unused)]
    fn MSK12_get_num_dinf() -> i32;
    #[allow(unused)]
    fn MSK12_get_iinf(task : Task_t,par_idx : i32,value : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_liinf(task : Task_t,par_idx : i32,value : *mut i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_dinf(task : Task_t,par_idx : i32,value : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_iinf_name(par_idx : i32) -> * const c_char;
    #[allow(unused)]
    fn MSK12_get_liinf_name(par_idx : i32) -> * const c_char;
    #[allow(unused)]
    fn MSK12_get_dinf_name(par_idx : i32) -> * const c_char;
    #[allow(unused)]
    fn MSK12_get_iinf_index(par_name : *const c_char) -> i32;
    #[allow(unused)]
    fn MSK12_get_liinf_index(par_name : *const c_char) -> i32;
    #[allow(unused)]
    fn MSK12_get_dinf_index(name : *const c_char) -> i32;
    #[allow(unused)]
    fn MSK12_get_double_param(task : Task_t,par_name : *const c_char,value : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_double_param_index(par_name : *const c_char) -> i32;
    #[allow(unused)]
    fn MSK12_get_double_param_name(par_idx : i32) -> * const c_char;
    #[allow(unused)]
    fn MSK12_get_num_double_param() -> i32;
    #[allow(unused)]
    fn MSK12_get_all_double_params(task : Task_t,buflen : i32,buf : *mut f64);
    #[allow(unused)]
    fn MSK12_put_all_double_params(task : Task_t,num_par : i32,params : *const f64) -> i32;
    #[allow(unused)]
    fn MSK12_get_int_param_index(par_name : *const c_char) -> i32;
    #[allow(unused)]
    fn MSK12_get_int_param_name(par_idx : i32) -> * const c_char;
    #[allow(unused)]
    fn MSK12_get_num_int_param() -> i32;
    #[allow(unused)]
    fn MSK12_get_all_int_params(task : Task_t,buflen : i32,buf : *mut i32);
    #[allow(unused)]
    fn MSK12_put_all_int_params(task : Task_t,num_par : i32,params : *const i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_int_param(task : Task_t,par_name : *const c_char,value : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_param_str_len(task : Task_t,par_name : *const c_char) -> i32;
    #[allow(unused)]
    fn MSK12_get_param_str(task : Task_t,name : *const c_char,length : i32,buf : *mut c_char);
    #[allow(unused)]
    fn MSK12_put_double_param(task : Task_t,par_name : *const c_char,value : f64,ok : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_put_int_param(task : Task_t,par_name : *const c_char,value : i32,ok : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_put_param_str(task : Task_t,par_name : *const c_char,value : *const c_char,ok : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_task_name_len(task : Task_t) -> i32;
    #[allow(unused)]
    fn MSK12_get_obj_name_len(task : Task_t) -> i32;
    #[allow(unused)]
    fn MSK12_get_task_name(task : Task_t,capacity : i32,buf : *mut c_char);
    #[allow(unused)]
    fn MSK12_get_obj_name(task : Task_t,capacity : i32,buf : *mut c_char);
    #[allow(unused)]
    fn MSK12_put_task_name(task : Task_t,name : *const c_char) -> i32;
    #[allow(unused)]
    fn MSK12_put_obj_name(task : Task_t,name : *const c_char) -> i32;
    #[allow(unused)]
    fn MSK12_get_var_name_len(task : Task_t,var_idx : i32,name_len : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_var_name_len2(task : Task_t,var_idx : i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_barvar_name_len(task : Task_t,barvar_idx : i32,name_len : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_barvar_name_len2(task : Task_t,barvar_idx : i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_var_name(task : Task_t,var_idx : i32,capacity : i32,buf : *mut c_char) -> i32;
    #[allow(unused)]
    fn MSK12_get_barvar_name(task : Task_t,barvar_idx : i32,capacity : i32,buf : *mut c_char) -> i32;
    #[allow(unused)]
    fn MSK12_put_var_name(task : Task_t,var_idx : i32,name : *const c_char) -> i32;
    #[allow(unused)]
    fn MSK12_put_barvar_name(task : Task_t,barvar_idx : i32,name : *const c_char) -> i32;
    #[allow(unused)]
    fn MSK12_get_con_name_len(task : Task_t,con_idx : i64,len : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_djc_name_len(task : Task_t,djc_idx : i64,len : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_con_name_len2(task : Task_t,con_idx : i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_djc_name_len2(task : Task_t,djc_idx : i64) -> i32;
    #[allow(unused)]
    fn MSK12_get_con_name(task : Task_t,con_idx : i64,capacity : i32,buf : *mut c_char) -> i32;
    #[allow(unused)]
    fn MSK12_get_djc_name(task : Task_t,djc_idx : i64,capacity : i32,buf : *mut c_char) -> i32;
    #[allow(unused)]
    fn MSK12_put_con_name(task : Task_t,con_idx : i64,buf : *const c_char) -> i32;
    #[allow(unused)]
    fn MSK12_put_djc_name(task : Task_t,djc_idx : i64,buf : *const c_char) -> i32;
    #[allow(unused)]
    fn MSK12_write_task_to_file(task : Task_t,filename : *const c_char) -> i32;
    #[allow(unused)]
    fn MSK12_write_task_to_handle(task : Task_t,format : i32,compress : i32,handle : c_void_p,func : extern "C" fn (h : c_void_p,src : c_void_p,num : usize) -> usize) -> i32;
    #[allow(unused)]
    fn MSK12_write_solution_to_file(task : Task_t,filename : *const c_char) -> i32;
    #[allow(unused)]
    fn MSK12_write_solution_to_handle(task : Task_t,format : i32,compress : i32,handle : c_void_p,func : extern "C" fn (h : c_void_p,src : c_void_p,num : usize) -> usize) -> i32;
    #[allow(unused)]
    fn MSK12_read_from_file(task : Task_t,filename : *const c_char) -> i32;
    #[allow(unused)]
    fn MSK12_read_from_handle(task : Task_t,format : i32,compress : i32,handle : c_void_p,func : extern "C" fn (h : c_void_p,dest : c_void_p,num : usize) -> usize) -> i32;
    #[allow(unused)]
    fn MSK12_put_stream_callback(task : Task_t,whichstream : i32,handle : c_void_p,func : extern "C" fn (h : c_void_p,src : *const c_char)) -> i32;
    #[allow(unused)]
    fn MSK12_clear_stream_callback(task : Task_t,whichstream : i32) -> i32;
    #[allow(unused)]
    fn MSK12_put_error_callback(task : Task_t,handle : c_void_p,func : extern "C" fn (h : c_void_p,r : i32,name : *const c_char,desc : *const c_char,message : *const c_char) -> i32) -> i32;
    #[allow(unused)]
    fn MSK12_put_warning_callback(task : Task_t,handle : c_void_p,func : extern "C" fn (h : c_void_p,r : i32,name : *const c_char,desc : *const c_char,message : *const c_char) -> i32) -> i32;
    #[allow(unused)]
    fn MSK12_clear_error_callback(task : Task_t) -> i32;
    #[allow(unused)]
    fn MSK12_clear_warning_callback(task : Task_t) -> i32;
    #[allow(unused)]
    fn MSK12_license_cleanup();
    #[allow(unused)]
    fn MSK12_shutdown_global_threadpool();
    #[allow(unused)]
    fn MSK12_axpy(n : i32,alpha : f64,x : *const f64,y : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_dot(n : i32,x : *const f64,y : *const f64,xty : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_gemv(transa : i32,m : i32,n : i32,alpha : f64,a : *const f64,x : *const f64,beta : f64,y : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_gemm(transa : i32,transb : i32,m : i32,n : i32,k : i32,alpha : f64,a : *const f64,b : *const f64,beta : f64,c : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_syrk(is_upr : i32,trans : i32,n : i32,k : i32,alpha : f64,a : *const f64,beta : f64,c : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_sparse_triangular_solve_dense(transposed : i32,n : i32,lnzc : *const i32,lptrc : *const i64,nnz : i64,lsubc : *const i32,lvalc : *const f64,b : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_potrf(is_upr : i32,n : i32,a : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_syeig(is_upr : i32,n : i32,a : *const f64,w : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_syevd(is_upr : i32,n : i32,a : *mut f64,w : *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_compute_sparse_cholesky(num_threads : i32,order_method : i32,tol_singular : f64,n : i32,a_col_num_nonzero : *const i32,a_subi : *const i32,a_val : *const f64,perm : *mut i32,diag : *mut f64,alloc : extern "C" fn (handle : c_void_p,num : usize) -> *mut u8,alloc_handle : c_void_p,l_col_num_nonzero : *mut i32,l_subi : *mut *mut i32,l_val : *mut *mut f64) -> i32;
    #[allow(unused)]
    fn MSK12_optimize_batch(is_race : i32,max_time_sec : f64,num_threads : i32,num_task : i64,tasks : *const Task_t,trm_code : *mut i32,res_code : *mut i32) -> i32;
    #[allow(unused)]
    fn MSK12_check_out_license(feature : i32) -> i32;
    #[allow(unused)]
    fn MSK12_check_in_license(feature : i32) -> i32;
    #[allow(unused)]
    fn MSK12_check_in_all() -> i32;
    #[allow(unused)]
    fn MSK12_echo_intro(long_ver : i32) -> i32;
    #[allow(unused)]
    fn MSK12_get_version(major : *mut i32,minor : *mut i32,revision : *mut i32);
    #[allow(unused)]
    fn MSK12_put_license_debug(lic_debug : i32) -> i32;
    #[allow(unused)]
    fn MSK12_put_license_code(code : *const i32) -> i32;
    #[allow(unused)]
    fn MSK12_put_license_wait(lic_wait : i32) -> i32;
    #[allow(unused)]
    fn MSK12_put_license_path(license_path : *const c_char) -> i32;

}

pub struct APIError {
    name  : String,
    descr : String,
    msg   : String
}

impl std::fmt::Debug for APIError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        std::fmt::Display::fmt(self.name.as_str(),f)?;
        std::fmt::Display::fmt(": ",f)?;
        std::fmt::Display::fmt(self.msg.as_str(),f)
    }
}

impl From<APIError> for String {
    fn from(value : APIError) -> Self {
        format!("{:?}",value)
    }

}

extern "C" fn memory_alloc(handle : *mut c_void, size : usize) -> *mut u8 {
    let allocated : &mut Vec<(std::alloc::Layout,*mut u8)> = unsafe { &mut *(handle as * mut Vec<(std::alloc::Layout,*mut u8)>) };
    let layout = std::alloc::Layout::from_size_align(size,8).unwrap();
    let p = unsafe { std::alloc::alloc(layout) };
    allocated.push((layout,p));
    p
}

extern "C" fn write_proxy(handle : c_void_p, src : c_void_p,  num : usize) -> usize {
    let f = handle as *mut Box<dyn FnMut(&[u8]) -> usize>;
    unsafe{ (*f)(std::slice::from_raw_parts(src as *const u8, num)) }
}

extern "C" fn read_proxy(handle : c_void_p, dst : c_void_p, num : usize) -> usize {
    let f = handle as *mut Box<dyn FnMut(&mut [u8]) -> usize>;
    unsafe{ (*f)(std::slice::from_raw_parts_mut(dst as *mut u8, num)) }
}

pub struct Task {
    task : Task_t,
}
unsafe impl std::marker::Send for Task {}

pub struct MosekStableAPI {}
unsafe impl std::marker::Send for MosekStableAPI {}

impl APIError {
    fn new<S>(code : i32, msg : S) -> APIError where S : Into<String> {
        let rname = unsafe{ MSK12_get_resp_name(code) };
        let rdesc = unsafe{ MSK12_get_resp_descr(code) };

        let name  = if rname != std::ptr::null() { unsafe { CStr::from_ptr(rname) }.to_str().unwrap_or("") } else { "" };
        let descr = if rdesc != std::ptr::null() { unsafe { CStr::from_ptr(rdesc) }.to_str().unwrap_or("") } else { "" };

        APIError {
            name  : name.to_string(),
            descr : descr.to_string(),
            msg   : msg.into()
        }
    }
    fn from<S1,S2,S3>(name : S1, descr : S2, msg : S3) -> APIError where S1 : Into<String>, S2 : Into<String>, S3 : Into<String> {
        APIError{
            name  : name.into(),
            descr : descr.into(),
            msg   : msg.into()
        }
    }

    pub fn name(&self) -> &str { self.name.as_str() }
    pub fn descr(&self) -> &str { self.descr.as_str() }
    pub fn message(&self) -> &str { self.msg.as_str() }
}


pub fn is_initialized() -> bool {
    unsafe { MSK12_library_initialized() != 0 }
}
pub fn initialize() -> Result<MosekStableAPI,APIError>
{
    let load_res = unsafe { MSK12_initialize_library() };
    if load_res != 0 {
        Err(APIError::from("err_mosek_core_dynamic_load", "","Failed to load MOSEK Core API library"))
    }
    else {
        Ok(MosekStableAPI{})
    }
}
pub fn initialize_with_paths(paths : &[&str]) -> Result<MosekStableAPI,APIError>
{
    if ! is_initialized() {
        let mut ps = Vec::with_capacity(paths.len()+1);
        for p in paths {
            ps.push(CStr::from_bytes_until_nul(p.as_bytes()).map_err(|_| APIError::from("err_invalid_path", "", "Invalid path cannot be converted to a C string"))?.as_ptr())
        }
        ps.push(std::ptr::null());

        if unsafe { MSK12_initialize_library_with_paths(ps.as_ptr()) } == 0 {
            Err(APIError::from("err_mosek_core_dynamic_load", "","Failed to load MOSEK Core API library"))
        }
        else {
            Ok(MosekStableAPI{})
        }
    }
    else {
        Ok(MosekStableAPI{})
    }
}

impl MosekStableAPI {
    pub fn task(&self) -> Result<Task,APIError> {
        let p = unsafe { MSK12_new_task() };
        if p == std::ptr::null_mut() {
            Err(APIError::from("err_task_creation","","Failed to create MOSEK Core API Task"))
        }
        else {
            Ok(Task::from_ptr(p))
        }
    }

    /// Get the name of a callback code 
    /// 
    /// # Arguments
    /// 
    /// - `code` 
    pub fn get_callback_code_name(&self,code : i32) -> String
    {
        // Arg processing order: code
        let returned_value = unsafe{ MSK12_get_callback_code_name(code) };
        unsafe { CStr::from_ptr(returned_value) }.to_string_lossy().into_owned()
    }
    /// Get string representing the given response code.
    /// 
    /// # Arguments
    /// 
    /// - `r` 
    pub fn get_resp_name(&self,r : ResCode) -> String
    {
        // Arg processing order: r
        let returned_value = unsafe{ MSK12_get_resp_name(r as i32) };
        unsafe { CStr::from_ptr(returned_value) }.to_string_lossy().into_owned()
    }
    /// Get string with a description of the given response code.
    /// 
    /// # Arguments
    /// 
    /// - `r` 
    pub fn get_resp_descr(&self,r : ResCode) -> String
    {
        // Arg processing order: r
        let returned_value = unsafe{ MSK12_get_resp_descr(r as i32) };
        unsafe { CStr::from_ptr(returned_value) }.to_string_lossy().into_owned()
    }
    /// Get string representing the given termination code.
    /// 
    /// # Arguments
    /// 
    /// - `trm` 
    pub fn get_trm_name(&self,trm : TrmCode) -> String
    {
        // Arg processing order: trm
        let returned_value = unsafe{ MSK12_get_trm_name(trm as i32) };
        unsafe { CStr::from_ptr(returned_value) }.to_string_lossy().into_owned()
    }
    /// Get string with a description if the given termination code.
    /// 
    /// # Arguments
    /// 
    /// - `trm` 
    pub fn get_trm_descr(&self,trm : TrmCode) -> String
    {
        // Arg processing order: trm
        let returned_value = unsafe{ MSK12_get_trm_descr(trm as i32) };
        unsafe { CStr::from_ptr(returned_value) }.to_string_lossy().into_owned()
    }
    /// Create a new task from a file. Accepted file formats are PTF, JSON and TASK, and accepted compressions are plain, zstd and gzip.
    pub fn new_task(&self) -> Task
    {
        // Arg processing order: 
        let returned_value = unsafe{ MSK12_new_task() };
        Task::from_ptr(returned_value)
    }
    pub fn get_num_iinf(&self) -> i32
    {
        // Arg processing order: 
        let returned_value = unsafe{ MSK12_get_num_iinf() };
        returned_value
    }
    pub fn get_num_liinf(&self) -> i32
    {
        // Arg processing order: 
        let returned_value = unsafe{ MSK12_get_num_liinf() };
        returned_value
    }
    pub fn get_num_dinf(&self) -> i32
    {
        // Arg processing order: 
        let returned_value = unsafe{ MSK12_get_num_dinf() };
        returned_value
    }
    /// Get the index of the integer information item corresponding to the given name. If the index is invalid, NULL is returned.
    /// 
    /// # Arguments
    /// 
    /// - `par_idx` 
    pub fn get_iinf_name(&self,par_idx : i32) -> String
    {
        // Arg processing order: par_idx
        let returned_value = unsafe{ MSK12_get_iinf_name(par_idx) };
        unsafe { CStr::from_ptr(returned_value) }.to_string_lossy().into_owned()
    }
    /// Get the index of the long integer information item corresponding to the given name. If the index is invalid, NULL is returned.
    /// 
    /// # Arguments
    /// 
    /// - `par_idx` 
    pub fn get_liinf_name(&self,par_idx : i32) -> String
    {
        // Arg processing order: par_idx
        let returned_value = unsafe{ MSK12_get_liinf_name(par_idx) };
        unsafe { CStr::from_ptr(returned_value) }.to_string_lossy().into_owned()
    }
    /// Get the index of the long integer information item corresponding to the given name. If the index is invalid, NULL is returned.
    /// 
    /// # Arguments
    /// 
    /// - `par_idx` 
    pub fn get_dinf_name(&self,par_idx : i32) -> String
    {
        // Arg processing order: par_idx
        let returned_value = unsafe{ MSK12_get_dinf_name(par_idx) };
        unsafe { CStr::from_ptr(returned_value) }.to_string_lossy().into_owned()
    }
    /// This will retur the index of the integer item orresponding to name, or -1 if the name is not recognized.
    /// 
    /// # Arguments
    /// 
    /// - `par_name[.cstring]` (in) Name of the parameter
    pub fn get_iinf_index(&self,par_name : &str) -> Result<i32,APIError>
    {
        // Arg processing order: par_name
        let cstring_par_name_ =
          CString::new(par_name)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: par_name")))?;
        let returned_value = unsafe{ MSK12_get_iinf_index(cstring_par_name_.as_ptr()) };
        Ok(returned_value)
    }
    /// This will retur the index of the long integer item orresponding to name, or -1 if the name is not recognized.
    /// 
    /// # Arguments
    /// 
    /// - `par_name[.cstring]` (in) Name of the parameter
    pub fn get_liinf_index(&self,par_name : &str) -> Result<i32,APIError>
    {
        // Arg processing order: par_name
        let cstring_par_name_ =
          CString::new(par_name)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: par_name")))?;
        let returned_value = unsafe{ MSK12_get_liinf_index(cstring_par_name_.as_ptr()) };
        Ok(returned_value)
    }
    /// This will retur the index of the double item orresponding to name, or -1 if the name is not recognized.
    /// 
    /// # Arguments
    /// 
    /// - `name[.cstring]` (in) 
    pub fn get_dinf_index(&self,name : &str) -> Result<i32,APIError>
    {
        // Arg processing order: name
        let cstring_name_ =
          CString::new(name)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: name")))?;
        let returned_value = unsafe{ MSK12_get_dinf_index(cstring_name_.as_ptr()) };
        Ok(returned_value)
    }
    /// Get the index corresponding to a double parameter name.
    /// 
    /// # Arguments
    /// 
    /// - `par_name[.cstring]` (in) Name of the parameter
    pub fn get_double_param_index(&self,par_name : &str) -> Result<i32,APIError>
    {
        // Arg processing order: par_name
        let cstring_par_name_ =
          CString::new(par_name)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: par_name")))?;
        let returned_value = unsafe{ MSK12_get_double_param_index(cstring_par_name_.as_ptr()) };
        Ok(returned_value)
    }
    /// Get the index corresponding to a double parameter name.
    /// 
    /// # Arguments
    /// 
    /// - `par_idx` 
    pub fn get_double_param_name(&self,par_idx : i32) -> String
    {
        // Arg processing order: par_idx
        let returned_value = unsafe{ MSK12_get_double_param_name(par_idx) };
        unsafe { CStr::from_ptr(returned_value) }.to_string_lossy().into_owned()
    }
    /// Get the index corresponding to a double parameter name.
    pub fn get_num_double_param(&self) -> i32
    {
        // Arg processing order: 
        let returned_value = unsafe{ MSK12_get_num_double_param() };
        returned_value
    }
    /// Get the index corresponding to a integer parameter name.
    /// 
    /// # Arguments
    /// 
    /// - `par_name[.cstring]` (in) Name of the parameter
    pub fn get_int_param_index(&self,par_name : &str) -> Result<i32,APIError>
    {
        // Arg processing order: par_name
        let cstring_par_name_ =
          CString::new(par_name)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: par_name")))?;
        let returned_value = unsafe{ MSK12_get_int_param_index(cstring_par_name_.as_ptr()) };
        Ok(returned_value)
    }
    /// Get the index corresponding to a integer parameter name.
    /// 
    /// # Arguments
    /// 
    /// - `par_idx` 
    pub fn get_int_param_name(&self,par_idx : i32) -> String
    {
        // Arg processing order: par_idx
        let returned_value = unsafe{ MSK12_get_int_param_name(par_idx) };
        unsafe { CStr::from_ptr(returned_value) }.to_string_lossy().into_owned()
    }
    /// Get the index corresponding to a double parameter name.
    pub fn get_num_int_param(&self) -> i32
    {
        // Arg processing order: 
        let returned_value = unsafe{ MSK12_get_num_int_param() };
        returned_value
    }
    /// Stops all threads and deletes all handles used by the license system. If this
    /// function is called, it must be called as the last MOSEK API call. No other
    /// MOSEK API calls are valid after this.
    pub fn license_cleanup(&self)
    {
        // Arg processing order: 
        unsafe{ MSK12_license_cleanup() };
    }
    /// If MOSEK is using a global threadpool, attempt to shut
    /// this down. If there are currently jobs running, this will do
    /// nothing.
    pub fn shutdown_global_threadpool(&self)
    {
        // Arg processing order: 
        unsafe{ MSK12_shutdown_global_threadpool() };
    }
    /// Computes vector addition and multiplication by a scalar. 
    /// 
    /// # Arguments
    /// 
    /// - `n` Length of the vectors. 
    /// - `alpha` The scalar that multiplies x. 
    /// - `x[n]` (in) The x vector. 
    /// - `y[n]` (in-out) The y vector. 
    pub fn axpy(&self,alpha : f64,x : &[f64],y : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: n,alpha,x,y
        let n = i32::try_from([Some(x.len()),Some(y.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_axpy(n,alpha,x.as_ptr(),y.as_mut_ptr()) };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        Ok(())
    }
    /// Computes the inner product of two vectors. 
    /// 
    /// # Arguments
    /// 
    /// - `n` Length of the vectors. 
    /// - `xty[1]` (out) The result of the inner product. 
    /// - `x[n]` (in) The x vector. 
    /// - `y[n]` (in) The y vector. 
    pub fn dot(&self,x : &[f64],y : &[f64]) -> Result<f64,APIError>
    {
        // Arg processing order: n,xty,x,y
        let n = i32::try_from([Some(x.len()),Some(y.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let mut xty : f64 = Default::default();
        let returned_value = unsafe{ MSK12_dot(n,x.as_ptr(),y.as_ptr(),std::ptr::from_mut(&mut xty)) };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        Ok(xty)
    }
    /// Computes the multiplication of a scaled dense matrix times a dense vector, plus a scaled dense vector. Precisely, if `transa` is false then the update is
    /// 
    /// $$
    /// y := \\alpha A x + \\beta y,
    /// $$
    /// 
    /// and if `transa` is true
    /// 
    /// $$
    /// y := \\alpha A^T x + \\beta y,
    /// $$
    /// 
    /// where \\(\\alpha,\\beta\\) are scalar values and \\(A\\) is a matrix with \\(m\\) rows and \\(n\\) columns.
    /// 
    /// Note that the result is stored overwriting \\(y\\). It must not overlap with the other input arrays.
    /// 
    /// # Arguments
    /// 
    /// - `transa` Indicates whether the matrix A must be transposed. 
    /// - `m` Specifies the number of rows of the matrix A. 
    /// - `n` Specifies the number of columns of the matrix A. 
    /// - `alpha` A scalar value multiplying the matrix A. 
    /// - `beta` A scalar value multiplying the vector y. 
    /// - `a` (in) A pointer to the array storing matrix A in a column-major format. 
    /// - `x` (in) A pointer to the array storing the vector x. 
    /// - `y` (in-out) A pointer to the array storing the vector y. 
    pub fn gemv(&self,transa : bool,m : i32,n : i32,alpha : f64,a : &[f64],x : &[f64],beta : f64,y : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: transa,m,n,alpha,beta,a,x,y
        let returned_value = unsafe{ MSK12_gemv(if transa {1} else {0},m,n,alpha,a.as_ptr(),x.as_ptr(),beta,y.as_mut_ptr()) };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        Ok(())
    }
    /// Performs a matrix multiplication plus addition of dense matrices.
    /// 
    /// Given
    /// \\(A\\), \\(B\\) and \\(C\\) of compatible dimensions, this function
    /// computes
    /// 
    /// .. math:: C:= \\alpha \\mathrm{op}(A)\\mathrm{op}(B) + \\beta C
    /// 
    /// where \\(\\alpha,\\beta\\) are two scalar values. The function \\(\mathrm{op}(X)\\)
    /// denotes \\(X\\) if transX is false, or \\(X^T\\) if set to true. The matrix \\(C\\) has \\(m\\) rows and \\(n\\) columns, and the other matrices must have compatible dimensions.
    /// 
    /// The result of this operation is stored in \\(C\\). It must not overlap with the other input arrays.
    /// 
    /// # Arguments
    /// 
    /// - `transa` Indicates whether the matrix A must be transposed. 
    /// - `transb` Indicates whether the matrix B must be transposed. 
    /// - `m` Indicates the number of rows of matrix C. 
    /// - `n` Indicates the number of columns of matrix C. 
    /// - `k` Specifies the common dimension along which op(A) and op(B) are multiplied. 
    /// - `alpha` A scalar value multiplying the result of the matrix multiplication. 
    /// - `beta` A scalar value that multiplies C. 
    /// - `a` (in) The pointer to the array storing matrix A in a column-major format. 
    /// - `b` (in) The pointer to the array storing matrix B in a column-major format.  
    /// - `c` (in-out) The pointer to the array storing matrix C in a column-major format. 
    pub fn gemm(&self,transa : bool,transb : bool,m : i32,n : i32,k : i32,alpha : f64,a : &[f64],b : &[f64],beta : f64,c : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: transa,transb,m,n,k,alpha,beta,a,b,c
        let returned_value = unsafe{ MSK12_gemm(if transa {1} else {0},if transb {1} else {0},m,n,k,alpha,a.as_ptr(),b.as_ptr(),beta,c.as_mut_ptr()) };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        Ok(())
    }
    /// Performs a symmetric rank-\\(k\\) update for a symmetric matrix.
    /// 
    /// Given a symmetric matrix \\(C\\in \\real^{n\\times n}\\), two scalars
    /// \\(\\alpha,\\beta\\) and a matrix \\(A\\) of rank \\(k\\leq n\\), it
    /// computes either
    /// 
    /// .. math:: C := \\alpha A A^T + \\beta C,
    /// 
    /// when `trans` is set to false and \\(A\\in \\real^{n\\times k}\\), or
    /// 
    /// .. math:: C := \\alpha A^T A + \\beta C,
    /// 
    /// when `trans` is set to true and \\(A\\in \\real^{k\\times n}\\).
    /// 
    /// Only the part of \\(C\\) indicated by `is_upr` is used and only that part is updated with the result. It must not overlap with the other input arrays.
    /// 
    /// # Arguments
    /// 
    /// - `is_upr` Indicates whether the upper or lower triangular part of C is used. 
    /// - `trans` Indicates whether the matrix A must be transposed. 
    /// - `n` Specifies the order of \\(C\\).
    /// - `k` Indicates the number of rows or columns of \\(A\\), depending on whether or not it is transposed, and its rank.
    /// - `alpha` A scalar value multiplying the result of the matrix multiplication. 
    /// - `beta` A scalar value that multiplies C. 
    /// - `a` (in) The pointer to the array storing matrix A in a column-major format. 
    /// - `c` (in-out) The pointer to the array storing matrix C in a column-major format. 
    pub fn syrk(&self,is_upr : bool,trans : bool,n : i32,k : i32,alpha : f64,a : &[f64],beta : f64,c : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: is_upr,trans,n,k,alpha,beta,a,c
        let returned_value = unsafe{ MSK12_syrk(if is_upr {1} else {0},if trans {1} else {0},n,k,alpha,a.as_ptr(),beta,c.as_mut_ptr()) };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        Ok(())
    }
    /// The function solves a triangular system of the form
    /// 
    /// .. math:: L x = b
    /// 
    /// or
    /// 
    /// .. math:: L^T x = b
    /// 
    /// where \\(L\\) is a sparse lower triangular nonsingular matrix. This implies in particular that diagonals in \\(L\\) are nonzero.
    /// 
    /// # Arguments
    /// 
    /// - `transposed` Controls whether the solve is with L or the transposed L. 
    /// - `n` Specifies the dimension of L. 
    /// - `nnz` Number of elements in lsubc and lvalc. 
    /// - `lnzc[n]` (in) `lnzc[j]` is the number of nonzeros in column j. 
    /// - `lptrc[n]` (in) `lptrc[j]` is a pointer to the first row index and value in column j. 
    /// - `lsubc[nnz]` (in) Row indexes for each column stored sequentially. 
    /// - `lvalc[nnz]` (in) The value corresponding to row indexed stored lsubc. 
    /// - `b[n]` (in-out) The right-hand side of linear equation system to be solved as a dense vector. 
    pub fn sparse_triangular_solve_dense(&self,transposed : bool,lnzc : &[i32],lptrc : &[i64],lsubc : &[i32],lvalc : &[f64],b : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: transposed,n,nnz,lnzc,lptrc,lsubc,lvalc,b
        let n = i32::try_from([Some(lnzc.len()),Some(lptrc.len()),Some(b.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let nnz = i64::try_from([Some(lsubc.len()),Some(lvalc.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_sparse_triangular_solve_dense(if transposed {1} else {0},n,lnzc.as_ptr(),lptrc.as_ptr(),nnz,lsubc.as_ptr(),lvalc.as_ptr(),b.as_mut_ptr()) };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        Ok(())
    }
    /// Computes a Cholesky factorization of a real symmetric positive definite dense matrix.
    /// 
    /// # Arguments
    /// 
    /// - `is_upr` Indicates whether the upper or lower triangular part of the matrix is stored. 
    /// - `n` Dimension of the symmetric matrix. 
    /// - `a` (in-out) A symmetric matrix stored in column-major order. 
    pub fn potrf(&self,is_upr : bool,n : i32,a : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: is_upr,n,a
        let returned_value = unsafe{ MSK12_potrf(if is_upr {1} else {0},n,a.as_mut_ptr()) };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        Ok(())
    }
    /// Computes all eigenvalues of a real symmetric matrix \\(A\\). Given a matrix \\(A\\in\\real^{n\\times n}\\) it returns a vector \\(w\\in\\real^n\\) containing the eigenvalues of \\(A\\). 
    /// 
    /// # Arguments
    /// 
    /// - `is_upr` Indicates whether the upper or lower triangular part is used. 
    /// - `n` Dimension of the symmetric input matrix. 
    /// - `a` (in) Input matrix A. 
    /// - `w[n]` (out) Array of length at least n containing the eigenvalues of A. 
    pub fn syeig(&self,is_upr : bool,a : &[f64],w : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: is_upr,n,a,w
        let n = i32::try_from([Some(w.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_syeig(if is_upr {1} else {0},n,a.as_ptr(),w.as_mut_ptr()) };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        Ok(())
    }
    /// Computes all the eigenvalues and eigenvectors a real symmetric matrix.
    /// Given the input matrix \\(A\\in \\real^{n\\times n}\\), this function returns a
    /// vector \\(w\\in \\real^n\\) containing the eigenvalues of \\(A\\) and it also computes the eigenvectors
    /// of \\(A\\). Therefore, this function computes the eigenvalue decomposition of \\(A\\) as
    /// 
    /// .. math:: A= U V U^T,
    /// 
    /// where \\(V=\\diag(w)\\) and \\(U\\) contains the eigenvectors of \\(A\\).
    /// 
    /// Note that the matrix \\(U\\) overwrites the input data \\(A\\).
    /// 
    /// # Arguments
    /// 
    /// - `is_upr` Indicates whether the upper or lower triangular part is used. 
    /// - `n` Dimension of the symmetric input matrix. 
    /// - `a` (in-out) Input matrix A. 
    /// - `w[n]` (in-out) Array of length at least n containing the eigenvalues of A. 
    pub fn syevd(&self,is_upr : bool,a : &mut [f64],w : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: is_upr,n,a,w
        let n = i32::try_from([Some(w.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_syevd(if is_upr {1} else {0},n,a.as_mut_ptr(),w.as_mut_ptr()) };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        Ok(())
    }
    /// The function computes a Cholesky factorization of a sparse positive semidefinite matrix. Sparsity is exploited
    /// during the computations to reduce the amount of space and work required. Both the input and output matrices
    /// are represented using the sparse format.
    /// 
    /// To be precise, given a symmetric matrix \\(A \\in \\real^{n\\times n}\\) the function computes a nonsingular lower triangular matrix \\(L\\), a diagonal matrix \\(D\\) and a permutation matrix \\(P\\) such that
    /// 
    /// $$
    /// LL^T - D = P A P^T
    /// $$
    /// 
    /// If ``order_method`` is zero then reordering heuristics are not employed and \\(P\\) is the identity.
    /// 
    /// If a pivot during the computation of the Cholesky factorization is less than
    /// 
    /// $$
    /// -\\rho\\cdot\\max((PAP^T)_{jj},1.0)
    /// $$
    /// 
    /// then the matrix is declared negative semidefinite. On the hand if a pivot is smaller than
    /// 
    /// $$
    /// \\rho\\cdot\\max((PAP^T)_{jj},1.0),
    /// $$
    /// 
    /// then \\(D_{jj}\\) is increased from zero to
    /// 
    /// $$
    /// \\rho\\cdot\\max((PAP^T)_{jj},1.0).
    /// $$
    /// 
    /// Therefore, if \\(A\\) is sufficiently positive definite then \\(D\\) will be the zero matrix.
    /// Here \\(\\rho\\) is set equal to value of ``tol_singular``.
    /// 
    /// # Arguments
    /// 
    /// - `num_threads` The number threads that can be used to do the computation. 0 means the code makes the choice. 
    /// - `order_method` If nonzero, then a sparsity preserving ordering will be employed. 
    /// - `tol_singular` A positive parameter controlling when a pivot is declared zero. 
    /// - `n` Specifies the order of \\(A\\). 
    /// - `alloc` Memory allocation function for allocating the result. If `null`, the system `malloc` function is used. 
    /// - `alloc_handle` Handle passed to the memory allocation function 
    /// - `a_col_num_nonzero[n]` (in) `a_col_num_nonzero[j]` is a pointer to the first element in column \\(j\\). 
    /// - `perm[n]` (out) Permutation array used to specify the permutation matrix \\(P\\) computed by the function. 
    /// - `diag[n]` (out) The diagonal elements of matrix \\(D\\). 
    /// - `l_col_num_nonzero[n]` (out) `l_col_num_nonzero[j]` is the number of non zero elements in column \\(j\\) of \\(L\\). 
    /// - `a_subi` (in) Row indexes for each column stored in increasing order. 
    /// - `a_val` (in) The value corresponding to row indexed stored in asubc. 
    /// - `l_subi` (out) Row indexes for each column stored in increasing order. The returned array is guaranteed to be allocated with the allocation function `alloc`.
    ///   
    ///   Notice that upon return, whether the function failed or suceeded, if a non-null value is returned here, it means that it was allocated and it must be deallocated acordingly.
    /// - `l_val` (out) The values corresponding to row indexed stored in lsubc. The returned array is guaranteed to be allocated with the allocation function `alloc`.
    ///   
    ///   Notice that upon return, whether the function failed or suceeded, if a non-null value is returned here, it means that it was allocated and it must be deallocated acordingly.
    pub fn compute_sparse_cholesky(&self,num_threads : i32,order_method : i32,tol_singular : f64,a_col_num_nonzero : &[i32],a_subi : &[i32],a_val : &[f64],perm : &mut [i32],diag : &mut [f64],l_col_num_nonzero : &mut [i32]) -> Result<(Vec<i32>,Vec<f64>),APIError>
    {
        // Arg processing order: num_threads,order_method,tol_singular,n,alloc,alloc_handle,a_col_num_nonzero,perm,diag,l_col_num_nonzero,a_subi,a_val,l_subi,l_val
        let n = i32::try_from([Some(a_col_num_nonzero.len()),Some(perm.len()),Some(diag.len()),Some(l_col_num_nonzero.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let mut allocated_items : Vec<(std::alloc::Layout,*mut u8)> = Vec:: new();
        let mut ptr_l_subi : * mut i32 = std::ptr::null_mut();
        let mut ptr_l_val : * mut f64 = std::ptr::null_mut();
        let returned_value = unsafe{ MSK12_compute_sparse_cholesky(num_threads,order_method,tol_singular,n,a_col_num_nonzero.as_ptr(),a_subi.as_ptr(),a_val.as_ptr(),perm.as_mut_ptr(),diag.as_mut_ptr(),memory_alloc,(&mut allocated_items) as * mut _ as c_void_p,l_col_num_nonzero.as_mut_ptr(),&mut ptr_l_subi,&mut ptr_l_val) };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        let l_subi_len : usize = l_col_num_nonzero.iter().map(|&v| usize::try_from(v).unwrap()).sum::<usize>().try_into().unwrap();
        let mut l_subi = vec![i32::default(); l_subi_len];
        l_subi.copy_from_slice(unsafe{ std::slice::from_raw_parts(ptr_l_subi,l_subi_len) });
        let l_val_len : usize = l_col_num_nonzero.iter().map(|&v| usize::try_from(v).unwrap()).sum::<usize>().try_into().unwrap();
        let mut l_val = vec![f64::default(); l_val_len];
        l_val.copy_from_slice(unsafe{ std::slice::from_raw_parts(ptr_l_val,l_val_len) });
        for (layout,ptr) in allocated_items { unsafe{ std::alloc::dealloc(ptr,layout); } }
        Ok((l_subi,l_val))
    }
    /// Optimize a number of tasks in parallel using a specified number of threads. All
    /// callbacks and log output streams are disabled.
    /// 
    /// Assuming that each task takes about same time and there many more tasks than number of
    /// threads then a linear speedup can be achieved, also known as strong scaling. A typical
    /// application of this method is to solve many small tasks of similar type; in this case
    /// it is recommended that each of them is allocated a single thread by setting parameter `ipar_num_threads` to 1.
    /// 
    /// If the parameters `is_race` or `max_time_sec` are used, then the result may not be deterministic, in the sense that the tasks which complete first may vary between runs.
    /// 
    /// The remaining behavior, including termination and response codes returned for each task, are the same as if each task was optimized separately.
    /// 
    /// # Arguments
    /// 
    /// - `is_race` If nonzero, then the function is terminated after the first task has been completed.
    /// - `max_time_sec` Time limit for the function in seconds. 
    /// - `num_threads` Number of threads to be employed.
    /// - `num_task` Number of tasks to optimize. 
    /// - `tasks[num_task]` (in) An array of tasks to optimize in parallel. 
    /// - `trm_code[num_task]` (out) The termination code for each task. 
    /// - `res_code[num_task]` (out) The response code for each task. 
    pub fn optimize_batch(&self,is_race : bool,max_time_sec : f64,num_threads : i32,tasks : &[Task],trm_code : &mut [i32],res_code : &mut [i32]) -> Result<(),APIError>
    {
        // Arg processing order: is_race,max_time_sec,num_threads,num_task,tasks,trm_code,res_code
        let num_task = i64::try_from([Some(tasks.len()),Some(trm_code.len()),Some(res_code.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let ptrs_tasks_ : Vec<Task_t> = tasks.iter().map(|t| t.task).collect();
        let returned_value = unsafe{ MSK12_optimize_batch(if is_race {1} else {0},max_time_sec,num_threads,num_task,ptrs_tasks_.as_ptr(),trm_code.as_mut_ptr(),res_code.as_mut_ptr()) };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        Ok(())
    }
    /// Checks out a license feature from the license server. Normally the required
    /// license features will be automatically checked out the first time they are needed
    /// by the function [optimize](#func-optimize). This function can be used to check out one
    /// or more features ahead of time.
    /// 
    /// The feature will remain checked out until the environment is deleted or the function
    /// [check_in_license](func:check_in_license) is called.
    /// 
    /// If a given feature is already checked out when this function is called, the call has no effect.
    /// 
    /// # Arguments
    /// 
    /// - `feature` Feature to check out from the license system. 
    pub fn check_out_license(&self,feature : Feature) -> Result<(),APIError>
    {
        // Arg processing order: feature
        let returned_value = unsafe{ MSK12_check_out_license(feature as i32) };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        Ok(())
    }
    /// Check in a license feature to the license server. By default all licenses
    /// consumed by functions using a single environment are kept checked out for the
    /// lifetime of the MOSEK environment. This function checks in a given license
    /// feature back to the license server immediately.
    /// 
    /// If the given license feature is not checked out at all, or it is in use by a call to
    /// [optimize](func:optimize), calling this function has no effect.
    /// 
    /// Please note that returning a license to the license server incurs a small
    /// overhead, so frequent calls to this function should be avoided.
    /// 
    /// # Arguments
    /// 
    /// - `feature` Feature to check in to the license system. 
    pub fn check_in_license(&self,feature : Feature) -> Result<(),APIError>
    {
        // Arg processing order: feature
        let returned_value = unsafe{ MSK12_check_in_license(feature as i32) };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        Ok(())
    }
    /// Check in all unused license features to the license token server. 
    pub fn check_in_all(&self) -> Result<(),APIError>
    {
        // Arg processing order: 
        let returned_value = unsafe{ MSK12_check_in_all() };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        Ok(())
    }
    /// Prints an intro to message stream. 
    /// 
    /// # Arguments
    /// 
    /// - `long_ver` If non-zero, then the intro is slightly longer. 
    pub fn echo_intro(&self,long_ver : bool) -> Result<(),APIError>
    {
        // Arg processing order: long_ver
        let returned_value = unsafe{ MSK12_echo_intro(if long_ver {1} else {0}) };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        Ok(())
    }
    /// Obtains MOSEK version information. 
    /// 
    /// # Arguments
    /// 
    /// - `major[1]` (out) Major version number. 
    /// - `minor[1]` (out) Minor version number. 
    /// - `revision[1]` (out) Revision number. 
    pub fn get_version(&self) -> (i32,i32,i32)
    {
        // Arg processing order: major,minor,revision
        let mut major : i32 = Default::default();
        let mut minor : i32 = Default::default();
        let mut revision : i32 = Default::default();
        unsafe{ MSK12_get_version(std::ptr::from_mut(&mut major),std::ptr::from_mut(&mut minor),std::ptr::from_mut(&mut revision)) };
        (major,minor,revision)
    }
    /// Enables debug information for the license system. If `lic_debug` is non-zero, then MOSEK will print debug info regarding the license checkout.  
    /// 
    /// # Arguments
    /// 
    /// - `lic_debug` Whether license checkout debug info should be printed.  
    pub fn put_license_debug(&self,lic_debug : bool) -> Result<(),APIError>
    {
        // Arg processing order: lic_debug
        let returned_value = unsafe{ MSK12_put_license_debug(if lic_debug {1} else {0}) };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        Ok(())
    }
    /// Input a runtime license code.  This function has an effect only before the first optimization. 
    /// 
    /// # Arguments
    /// 
    /// - `code[21]` (in) A license key string. 
    pub fn put_license_code(&self,code : Option<&[i32]>) -> Result<(),APIError>
    {
        // Arg processing order: code
        let returned_value = unsafe{ MSK12_put_license_code(code.map(|a| a.as_ptr()).unwrap_or(std::ptr::null())) };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        Ok(())
    }
    /// Control whether MOSEK should wait for an available license if no license is available. If `lic_wait` is non-zero, then MOSEK will wait for `lic_wait-1` milliseconds between each check for an available license.
    /// 
    /// # Arguments
    /// 
    /// - `lic_wait` Enable waiting for a license until it is available. 
    pub fn put_license_wait(&self,lic_wait : i32) -> Result<(),APIError>
    {
        // Arg processing order: lic_wait
        let returned_value = unsafe{ MSK12_put_license_wait(lic_wait) };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        Ok(())
    }
    /// Set the path to the license file. This function has an effect only before the first optimization. 
    /// 
    /// # Arguments
    /// 
    /// - `license_path[.cstring]` (in) A path specifying where to search for the license. 
    pub fn put_license_path(&self,license_path : Option<&str>) -> Result<(),APIError>
    {
        // Arg processing order: license_path
        let cstring_license_path_ : Option<CString> =
          license_path.map(|n| CString::new(n).map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: {0}",n))))
            .transpose()?;
        let returned_value = unsafe{ MSK12_put_license_path(cstring_license_path_.map(|s| s.as_ptr()).unwrap_or(std::ptr::null())) };
        if 0 != returned_value { return Err(APIError::new(returned_value,"")); }
        Ok(())
    }

}

extern "C" fn stream_cb(handle : WriteHandle, msg : *const c_char) {
    let f = handle as * mut Box<dyn FnMut(&str)>;
    unsafe{ (*f)(CStr::from_ptr(msg).to_string_lossy().as_ref()) };
}

extern "C" fn info_cb(h : CallbackHandle,code : i32,len_iinf : i32,iinf : *const i32,len_liinf : i32,liinf : *const i64,len_dinf : i32,dinf : *const f64) -> i32 {
    let func = h as *mut Box<dyn Fn(i32,&[i32],&[i64],&[f64]) -> bool>;
    let r = unsafe{ (*func)(
        code,
        std::slice::from_raw_parts(iinf, usize::try_from(len_iinf).unwrap_or(0)),
        std::slice::from_raw_parts(liinf, usize::try_from(len_liinf).unwrap_or(0)),
        std::slice::from_raw_parts(dinf, usize::try_from(len_dinf).unwrap_or(0))) };
    if r { 1 } else { 0 }
}

extern "C" fn intsol_cb(h : CallbackHandle,num : i32,primal_obj : f64, xx : *const f64) {
    let func = h as *mut Box<dyn Fn(f64,&[f64])>;
    unsafe{ (*func)(primal_obj,std::slice::from_raw_parts(xx, usize::try_from(num).unwrap_or(0))) };
}


impl Task {
    fn from_ptr(task : Task_t) -> Task { Task{ task } }
    fn last_error(&self) -> Result<(),APIError> {
        let r = unsafe { MSK12_get_last_resp(self.task) };
        if r == 0 {
            return Ok(())
        }
        else {
            let rmsglen : usize = unsafe { MSK12_get_last_resp_msg_len(self.task) }
                .try_into()
                .map_err(|_| APIError::from("err_internal","","Internal cast of size failed"))?;
            let mut msgbuf : Vec<u8> = vec![Default::default(); rmsglen+1];
            unsafe { MSK12_get_last_resp_msg(self.task,msgbuf.as_mut_ptr() as *mut c_char,msgbuf.len()) };

            let smsg = CStr::from_bytes_until_nul(msgbuf.as_slice())
                .map_err(|_| APIError::from("err_internal","","Invalid string"))?
                .to_string_lossy()
                .into_owned();

            return Err(APIError::new(r,smsg))
        }
    }

    pub fn with_stream_callback<F,B,R>(&mut self,which : StreamType, f:F,body:B) -> Result<R,APIError>
        where
            F : FnMut(&str),
            B : FnOnce(&mut Self) -> Result<R,APIError>
    {
        let mut h : Box<dyn FnMut(&str)> = Box::new(f);
        let r = unsafe { MSK12_put_stream_callback(self.task,which as i32, (&mut h) as *mut Box<dyn FnMut(&str)> as WriteHandle, stream_cb) };
        if r != 0 { self.last_error()?; }
        let res = body(self);
        unsafe { MSK12_clear_stream_callback(self.task,StreamType::LOG as i32) };
        res
    }

    pub fn optimize_with_callbacks<F1,F2>(&mut self, info_f : Option<F1>, intsol_f : Option<F2>) -> Result<TrmCode,APIError>
        where
            F1 : FnMut(i32,&[i32],&[i64],&[f64]) -> bool,
            F2 : FnMut(f64,&[f64])
    {
        let info   = if let Some(f) = info_f { let r : Box<dyn FnMut(i32,&[i32],&[i64],&[f64]) -> bool> = Box::new(f); Some(r) } else { None };
        let intsol = if let Some(f) = intsol_f { let r : Box<dyn FnMut(f64,&[f64])> = Box::new(f); Some(r) } else { None };

        let has_info = info.is_some();
        let has_intsol = intsol.is_some();

        let mut trm : i32 = 0;
        let r = unsafe { MSK12_optimize_callback(
            self.task,
            &mut trm as * mut _,
            info.as_ref().map(|f| f as * const Box<dyn FnMut(i32,&[i32],&[i64],&[f64]) -> bool> as CallbackHandle).unwrap_or(std::ptr::null_mut()),
            if has_info { Some(info_cb as extern "C" fn(*mut c_void, i32, i32, *const i32, i32, *const i64, i32, *const f64) -> i32) } else {None},
            intsol.as_ref().map(|f| f as * const _ as CallbackHandle).unwrap_or(std::ptr::null_mut()),
            if has_intsol { Some(intsol_cb as extern "C" fn(*mut c_void, i32,f64,*const f64)) } else {None}) };
        if 0 != r {
            self.last_error()?
        }
        Ok(trm as TrmCode)
    }

    pub fn clone(&mut self) -> Result<Self,APIError> {
        let task2 = unsafe { MSK12_new_task_from_task(self.task) };
        if task2 == std::ptr::null_mut() {
            Err(APIError::from("err_task_creation","","Failed to create MOSEK Core API Task"))
        }
        else {
            Ok(Task::from_ptr(task2))
        }
    }

    // TODO:
    pub fn read_task_by_func<F>(&mut self, read : F, format : Format, compress : Compression) -> Result<(),APIError>
        where F : FnMut(&mut [u8]) -> usize
    {
        let f : Box<dyn FnMut(&mut [u8]) -> usize> = Box::new(read);
        let returned_value =
            unsafe{ MSK12_read_from_handle(
                self.task,
                format as i32,
                compress as i32,
                (&f) as * const _ as ReadHandle,// * const Box<dyn FnMut(&mut [u8]) -> usize>,
                read_proxy) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }

    /// Write task data
    pub fn write_task_by_func<F>(&mut self, write : F, format : Format, compress : Compression) -> Result<(),APIError>
        where F : FnMut(&[u8]) -> usize
    {
        let f : Box<dyn FnMut(&[u8]) -> usize> = Box::new(write);
        let returned_value =
            unsafe{ MSK12_write_task_to_handle(
                self.task,
                format as i32,
                compress as i32,
                (&f) as * const _ as WriteHandle,
                write_proxy) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }

    /// TODO
    pub fn write_solution_by_func<F>(&mut self, write : F, format : Format, compress : Compression) -> Result<(),APIError>
        where F : FnMut(&[u8]) -> usize
    {
        let f : Box<dyn FnMut(&[u8]) -> usize> = Box::new(write);
        let returned_value =
            unsafe{ MSK12_write_solution_to_handle(
                self.task,
                format as i32,
                compress as i32,
                (&f) as * const _ as WriteHandle,
                write_proxy) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }

    /// Reserve space for scalar variables. This is to be considered a _hint_, not a requirement. 
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `add_num` Number of item to add
    pub fn reserve_num_var(&mut self,add_num : i32) -> Result<(),APIError>
    {
        // Arg processing order: task,add_num
        let returned_value = unsafe{ MSK12_reserve_num_var(self.task,add_num) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Reserve space for semidefinite variables. This is to be considered a _hint_, not a requirement.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_barvar` Number of variables
    pub fn reserve_num_barvar(&mut self,num_barvar : i32) -> Result<(),APIError>
    {
        // Arg processing order: task,num_barvar
        let returned_value = unsafe{ MSK12_reserve_num_barvar(self.task,num_barvar) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Reserve space for constraints. This is to be considered a _hint_, not a requirement.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_con` Number of constraints
    pub fn reserve_num_con(&mut self,num_con : i32) -> Result<(),APIError>
    {
        // Arg processing order: task,num_con
        let returned_value = unsafe{ MSK12_reserve_num_con(self.task,num_con) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Reserve space for afes. This is to be considered a _hint_, not a requirement.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_row` Number of affine rows
    pub fn reserve_num_row(&mut self,num_row : i64) -> Result<(),APIError>
    {
        // Arg processing order: task,num_row
        let returned_value = unsafe{ MSK12_reserve_num_row(self.task,num_row) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Reserve space for coefficient matrix non-zeros. This is to be considered a _hint_, not a requirement.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_nz` 
    pub fn reserve_num_nz(&mut self,num_nz : i64) -> Result<(),APIError>
    {
        // Arg processing order: task,num_nz
        let returned_value = unsafe{ MSK12_reserve_num_nz(self.task,num_nz) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Reserve space for domains. This is to be considered a _hint_, not a requirement.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_dom` Number of domains
    pub fn reserve_num_dom(&mut self,num_dom : i64) -> Result<(),APIError>
    {
        // Arg processing order: task,num_dom
        let returned_value = unsafe{ MSK12_reserve_num_dom(self.task,num_dom) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Reserve space for symmetric matrixes. This is to be considered a _hint_, not a requirement.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_symmat` Number of symmetric matrixes
    pub fn reserve_num_symmat(&mut self,num_symmat : i64) -> Result<(),APIError>
    {
        // Arg processing order: task,num_symmat
        let returned_value = unsafe{ MSK12_reserve_num_symmat(self.task,num_symmat) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Reserve space for symmetric matrix variables. This is to be considered a _hint_, not a requirement.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_nz` 
    pub fn reserve_num_symmat_nz(&mut self,num_nz : i64) -> Result<(),APIError>
    {
        // Arg processing order: task,num_nz
        let returned_value = unsafe{ MSK12_reserve_num_symmat_nz(self.task,num_nz) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get number of scalar variables. Cannot fail.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    pub fn get_num_var(&self) -> i32
    {
        // Arg processing order: task
        let returned_value = unsafe{ MSK12_get_num_var(self.task) };
        returned_value
    }
    /// Get number of semidefinite variables. Cannot fail.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    pub fn get_num_barvar(&self) -> i32
    {
        // Arg processing order: task
        let returned_value = unsafe{ MSK12_get_num_barvar(self.task) };
        returned_value
    }
    /// Get number of domains. Cannot fail. 
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    pub fn get_num_domain(&self) -> i64
    {
        // Arg processing order: task
        let returned_value = unsafe{ MSK12_get_num_domain(self.task) };
        returned_value
    }
    /// Get number of affine rows. Cannot fail. 
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    pub fn get_num_row(&self) -> i64
    {
        // Arg processing order: task
        let returned_value = unsafe{ MSK12_get_num_row(self.task) };
        returned_value
    }
    /// Get number of symmetric matrixes. Cannot fail. 
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    pub fn get_num_symmat(&self) -> i64
    {
        // Arg processing order: task
        let returned_value = unsafe{ MSK12_get_num_symmat(self.task) };
        returned_value
    }
    /// Get number of constraints. Cannot fail. 
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    pub fn get_num_con(&self) -> i64
    {
        // Arg processing order: task
        let returned_value = unsafe{ MSK12_get_num_con(self.task) };
        returned_value
    }
    /// Get number of disjunctive constraints. Cannot fail. 
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    pub fn get_num_djc(&self) -> i64
    {
        // Arg processing order: task
        let returned_value = unsafe{ MSK12_get_num_djc(self.task) };
        returned_value
    }
    /// Append scalar variables.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_var` Number of variables
    pub fn append_vars(&mut self,num_var : i32) -> Result<(),APIError>
    {
        // Arg processing order: task,num_var
        let returned_value = unsafe{ MSK12_append_vars(self.task,num_var) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Append a number of empty affine rows
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_row` Number of affine rows
    pub fn append_rows(&mut self,num_row : i64) -> Result<(),APIError>
    {
        // Arg processing order: task,num_row
        let returned_value = unsafe{ MSK12_append_rows(self.task,num_row) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Append a single positive semi-definite variable.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `dim` Dimension
    pub fn append_barvar(&mut self,dim : i32) -> Result<(),APIError>
    {
        // Arg processing order: task,dim
        let returned_value = unsafe{ MSK12_append_barvar(self.task,dim) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Append multiple positive semi-definite variables with the given dimensions.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_barvar` Number of variables
    /// - `dims[num_barvar]` (in) Array of dimensionms
    pub fn append_barvars(&mut self,dims : &[i32]) -> Result<(),APIError>
    {
        // Arg processing order: task,num_barvar,dims
        let num_barvar = i32::try_from([Some(dims.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_append_barvars(self.task,num_barvar,dims.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Append a single symmetric matrix.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `dim` Dimension
    /// - `nnz` Number of nonzeros
    /// - `symmat_i[nnz]` (in) Symmetric matrix row subscripts
    /// - `symmat_j[nnz]` (in) Symmetric matrix column subscripts
    /// - `symmat_val[nnz]` (in) Symmetric matrix values
    pub fn append_symmat(&mut self,dim : i32,symmat_i : &[i32],symmat_j : &[i32],symmat_val : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,dim,nnz,symmat_i,symmat_j,symmat_val
        let nnz = i64::try_from([Some(symmat_i.len()),Some(symmat_j.len()),Some(symmat_val.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_append_symmat(self.task,dim,nnz,symmat_i.as_ptr(),symmat_j.as_ptr(),symmat_val.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Append a list of symmetric matrixes.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_symmat` Number of symmetric matrixes
    /// - `dim[num_symmat]` (in) Dimension
    /// - `nnz[num_symmat]` (in) Number of nonzeros
    /// - `symmat_i[nnz]` (in) Symmetric matrix row subscripts
    /// - `symmat_j[nnz]` (in) Symmetric matrix column subscripts
    /// - `symmat_val[nnz]` (in) Symmetric matrix values
    pub fn append_symmats(&mut self,dim : &[i32],nnz : &[i64],symmat_i : &[i32],symmat_j : &[i32],symmat_val : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,num_symmat,dim,nnz,symmat_i,symmat_j,symmat_val
        let num_symmat = i64::try_from([Some(dim.len()),Some(nnz.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_append_symmats(self.task,num_symmat,dim.as_ptr(),nnz.as_ptr(),symmat_i.as_ptr(),symmat_j.as_ptr(),symmat_val.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Append a number of empty constraints, initially they will have domain `null`.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_con` Number of constraints
    pub fn append_empty_cons(&mut self,num_con : i64) -> Result<(),APIError>
    {
        // Arg processing order: task,num_con
        let returned_value = unsafe{ MSK12_append_empty_cons(self.task,num_con) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Append a number of empty disjunctive constraints.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_djc` Number of disjunctive constraints
    pub fn append_empty_djcs(&mut self,num_djc : i64) -> Result<(),APIError>
    {
        // Arg processing order: task,num_djc
        let returned_value = unsafe{ MSK12_append_empty_djcs(self.task,num_djc) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Set variable type to integer or continuous.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `j` 
    /// - `var_type` 
    pub fn put_var_type(&mut self,j : i32,var_type : VariableType) -> Result<(),APIError>
    {
        // Arg processing order: task,j,var_type
        let returned_value = unsafe{ MSK12_put_var_type(self.task,j,var_type as i32) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Set variable types in a slice to integer or continuous.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_var` Variable index
    /// - `num_var` Number of variables
    /// - `var_types[num_var]` (in) 
    pub fn put_var_type_slice(&mut self,first_var : i32,var_types : &[VariableType]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_var,num_var,var_types
        let num_var = i32::try_from([Some(var_types.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        // CHECK: Enum RType(VariableType), var_types, mut=, nullable=False
        let i32_var_types : Vec<i32> = var_types.iter().map(|v| *v as i32).collect();
        let returned_value = unsafe{ MSK12_put_var_type_slice(self.task,first_var,num_var,i32_var_types.as_slice().as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Set variable types for all entries in a slice to a single value.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_var` Variable index
    /// - `num_var` Number of variables
    /// - `var_type` 
    pub fn put_var_type_slice_value(&mut self,first_var : i32,num_var : i32,var_type : VariableType) -> Result<(),APIError>
    {
        // Arg processing order: task,first_var,num_var,var_type
        let returned_value = unsafe{ MSK12_put_var_type_slice_value(self.task,first_var,num_var,var_type as i32) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Set variable types in a list to integer or continuous.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_var` Number of variables
    /// - `var_idxs[num_var]` (in) Array of variable indexes
    /// - `var_types[num_var]` (in) 
    pub fn put_var_type_list(&mut self,var_idxs : &[i32],var_types : &[VariableType]) -> Result<(),APIError>
    {
        // Arg processing order: task,num_var,var_idxs,var_types
        let num_var = i32::try_from([Some(var_idxs.len()),Some(var_types.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        // CHECK: Enum RType(VariableType), var_types, mut=, nullable=False
        let i32_var_types : Vec<i32> = var_types.iter().map(|v| *v as i32).collect();
        let returned_value = unsafe{ MSK12_put_var_type_list(self.task,num_var,var_idxs.as_ptr(),i32_var_types.as_slice().as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get variable type as integer or continuous.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `var_idx` Variable index
    /// - `var_type[1]` (out) 
    pub fn get_var_type(&self,var_idx : i32) -> Result<VariableType,APIError>
    {
        // Arg processing order: task,var_idx,var_type
        let mut var_type : i32 = 0;
        let returned_value = unsafe{ MSK12_get_var_type(self.task,var_idx,std::ptr::from_mut(&mut var_type)) };
        if 0 != returned_value { self.last_error()?; }
        let enum_var_type_ = VariableType::from(var_type).map_err(|i| APIError::from("err_invalid_enum_value","",format!("Invalid value ({0}) for enum VariableType",i)))?;
        Ok(enum_var_type_)
    }
    /// Get variable slice types as integer or continuous.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_var` Variable index
    /// - `num_var` Number of variables
    /// - `var_types[num_var]` (out) 
    pub fn get_var_type_slice(&self,first_var : i32,var_types : &mut [VariableType]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_var,num_var,var_types
        let num_var = i32::try_from([Some(var_types.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        // CHECK: Enum RType(VariableType), var_types, mut=mut , nullable=False
        let mut i32_var_types : Vec<i32> = var_types.iter().map(|v| *v as i32).collect();
        let returned_value = unsafe{ MSK12_get_var_type_slice(self.task,first_var,num_var,i32_var_types.as_mut_slice().as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        for (a,b) in i32_var_types.iter().zip(var_types.iter_mut()) {
          *b = VariableType::from(*a).map_err(|i| APIError::from("err_invalid_enum_value","",format!("Invalid value ({0}) for enum VariableType",i)))?;}
        Ok(())
    }
    /// Set variable bounds.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `var_idx` Variable index
    /// - `low` Lower bound
    /// - `upr` Upper bound
    pub fn put_var_bound(&mut self,var_idx : i32,low : f64,upr : f64) -> Result<(),APIError>
    {
        // Arg processing order: task,var_idx,low,upr
        let returned_value = unsafe{ MSK12_put_var_bound(self.task,var_idx,low,upr) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Set variable bounds for a slice.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_var` Variable index
    /// - `num_var` Number of variables
    /// - `low[num_var]` (in) Lower bound
    /// - `upr[num_var]` (in) Upper bound
    pub fn put_var_bound_slice(&mut self,first_var : i32,low : &[f64],upr : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_var,num_var,low,upr
        let num_var = i32::try_from([Some(low.len()),Some(upr.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_var_bound_slice(self.task,first_var,num_var,low.as_ptr(),upr.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Set identical bound for a slice of variables.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_var` Variable index
    /// - `num_var` Number of variables
    /// - `low` Lower bound
    /// - `upr` Upper bound
    pub fn put_var_bound_slice_value(&mut self,first_var : i32,num_var : i32,low : f64,upr : f64) -> Result<(),APIError>
    {
        // Arg processing order: task,first_var,num_var,low,upr
        let returned_value = unsafe{ MSK12_put_var_bound_slice_value(self.task,first_var,num_var,low,upr) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Set variable bounds.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `var_idx` Variable index
    /// - `low` Lower bound
    pub fn put_var_low_bound(&mut self,var_idx : i32,low : f64) -> Result<(),APIError>
    {
        // Arg processing order: task,var_idx,low
        let returned_value = unsafe{ MSK12_put_var_low_bound(self.task,var_idx,low) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Set variable bounds for a slice.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_var` Variable index
    /// - `num_var` Number of variables
    /// - `low[num_var]` (in) Lower bound
    pub fn put_var_low_bound_slice(&mut self,first_var : i32,low : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_var,num_var,low
        let num_var = i32::try_from([Some(low.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_var_low_bound_slice(self.task,first_var,num_var,low.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Set identical bound for a slice of variables.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_var` Variable index
    /// - `num_var` Number of variables
    /// - `low` Lower bound
    pub fn put_var_low_bound_slice_value(&mut self,first_var : i32,num_var : i32,low : f64) -> Result<(),APIError>
    {
        // Arg processing order: task,first_var,num_var,low
        let returned_value = unsafe{ MSK12_put_var_low_bound_slice_value(self.task,first_var,num_var,low) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Set variable bounds.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `var_idx` Variable index
    /// - `upr` Upper bound
    pub fn put_var_upr_bound(&mut self,var_idx : i32,upr : f64) -> Result<(),APIError>
    {
        // Arg processing order: task,var_idx,upr
        let returned_value = unsafe{ MSK12_put_var_upr_bound(self.task,var_idx,upr) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Set variable bounds for a slice.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_var` Variable index
    /// - `num_var` Number of variables
    /// - `upr[num_var]` (in) Upper bound
    pub fn put_var_upr_bound_slice(&mut self,first_var : i32,upr : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_var,num_var,upr
        let num_var = i32::try_from([Some(upr.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_var_upr_bound_slice(self.task,first_var,num_var,upr.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Set identical bound for a slice of variables.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_var` Variable index
    /// - `num_var` Number of variables
    /// - `upr` Upper bound
    pub fn put_var_upr_bound_slice_value(&mut self,first_var : i32,num_var : i32,upr : f64) -> Result<(),APIError>
    {
        // Arg processing order: task,first_var,num_var,upr
        let returned_value = unsafe{ MSK12_put_var_upr_bound_slice_value(self.task,first_var,num_var,upr) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get variable bounds.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `var_idx` Variable index
    /// - `low[1]` (out) Lower bound
    /// - `upr[1]` (out) Upper bound
    pub fn get_var_bound(&self,var_idx : i32) -> Result<(f64,f64),APIError>
    {
        // Arg processing order: task,var_idx,low,upr
        let mut low : f64 = Default::default();
        let mut upr : f64 = Default::default();
        let returned_value = unsafe{ MSK12_get_var_bound(self.task,var_idx,std::ptr::from_mut(&mut low),std::ptr::from_mut(&mut upr)) };
        if 0 != returned_value { self.last_error()?; }
        Ok((low,upr))
    }
    /// Get variable bounds for a slice.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_var` Variable index
    /// - `num_var` Number of variables
    /// - `low[num_var]` (out) Lower bound
    /// - `upr[num_var]` (out) Upper bound
    pub fn get_var_bound_slice(&self,first_var : i32,low : &mut [f64],upr : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_var,num_var,low,upr
        let num_var = i32::try_from([Some(low.len()),Some(upr.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_var_bound_slice(self.task,first_var,num_var,low.as_mut_ptr(),upr.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Compute the number of scalar elements in a slice of semidefinite variables.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_barvar` First semidefinite variable index in a slice
    /// - `num_barvar` Number of variables
    /// - `num_elm[1]` (out) Number of positive semidefinite non-zero entries
    pub fn get_barvar_slice_num_elm(&self,first_barvar : i32,num_barvar : i32) -> Result<i64,APIError>
    {
        // Arg processing order: task,first_barvar,num_barvar,num_elm
        let mut num_elm : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_barvar_slice_num_elm(self.task,first_barvar,num_barvar,std::ptr::from_mut(&mut num_elm)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(num_elm)
    }
    /// Get dimension of semidefinite variable
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `barvar_idx` Positive semi-definite variable index
    /// - `dim[1]` (out) Dimension
    pub fn get_barvar_dim(&self,barvar_idx : i32) -> Result<i32,APIError>
    {
        // Arg processing order: task,barvar_idx,dim
        let mut dim : i32 = Default::default();
        let returned_value = unsafe{ MSK12_get_barvar_dim(self.task,barvar_idx,std::ptr::from_mut(&mut dim)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(dim)
    }
    /// Get dimensions of a slice of semidefinite variables.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_barvar` First semidefinite variable index in a slice
    /// - `num_barvar` Number of variables
    /// - `dim[num_barvar]` (out) Dimension
    pub fn get_barvar_slice_dims(&self,first_barvar : i32,dim : &mut [i32]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_barvar,num_barvar,dim
        let num_barvar = i32::try_from([Some(dim.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_barvar_slice_dims(self.task,first_barvar,num_barvar,dim.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Return index of a domain of the requested type
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `dom_type` 
    /// - `dim` Dimension
    /// - `num_alpha` Number of alpha values in array
    /// - `dom_idx[1]` (out) Index of the domain
    /// - `alpha[dim]` (out) Array if alpha values for power cone domain
    pub fn get_domain(&mut self,dom_type : DomainType,num_alpha : i32,alpha : &mut [f64]) -> Result<i64,APIError>
    {
        // Arg processing order: task,dom_type,dim,num_alpha,dom_idx,alpha
        let dim = i64::try_from([Some(alpha.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let mut dom_idx : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_domain(self.task,dom_type as i32,dim,num_alpha,alpha.as_mut_ptr(),std::ptr::from_mut(&mut dom_idx)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(dom_idx)
    }
    /// Return index of the empty domain. Only one empty domain is created, so if one already exists, that one is returned instead of creating a new domain
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `dom_idx[1]` (out) Index of the domain
    pub fn get_domain_empty(&mut self) -> Result<i64,APIError>
    {
        // Arg processing order: task,dom_idx
        let mut dom_idx : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_domain_empty(self.task,std::ptr::from_mut(&mut dom_idx)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(dom_idx)
    }
    /// Return index of an `rzero` domain. Only one `rzero` domain is created, so if one already exists, that one is returned instead of creating a new domain
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `dom_idx[1]` (out) Index of the domain
    pub fn get_domain_rzero(&mut self) -> Result<i64,APIError>
    {
        // Arg processing order: task,dom_idx
        let mut dom_idx : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_domain_rzero(self.task,std::ptr::from_mut(&mut dom_idx)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(dom_idx)
    }
    /// Return index of an `rplus` domain. Only one `rplus` domain is created, so if one already exists, that one is returned instead of creating a new domain
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `dom_idx[1]` (out) Index of the domain
    pub fn get_domain_rplus(&mut self) -> Result<i64,APIError>
    {
        // Arg processing order: task,dom_idx
        let mut dom_idx : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_domain_rplus(self.task,std::ptr::from_mut(&mut dom_idx)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(dom_idx)
    }
    /// Return index of an `rminus` domain. Only one `rminus` domain is created, so if one already exists, that one is returned instead of creating a new domain
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `dom_idx[1]` (out) Index of the domain
    pub fn get_domain_rminus(&mut self) -> Result<i64,APIError>
    {
        // Arg processing order: task,dom_idx
        let mut dom_idx : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_domain_rminus(self.task,std::ptr::from_mut(&mut dom_idx)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(dom_idx)
    }
    /// Return index of an `r` domain. Only one `r` domain is created, so if one already exists, that one is returned instead of creating a new domain
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `dom_idx[1]` (out) Index of the domain
    pub fn get_domain_r(&mut self) -> Result<i64,APIError>
    {
        // Arg processing order: task,dom_idx
        let mut dom_idx : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_domain_r(self.task,std::ptr::from_mut(&mut dom_idx)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(dom_idx)
    }
    /// Return the index of a quadratic cone domain of the given size.
    /// 
    /// The quadratic cone of size \\(n\\) is defined as
    /// $$
    /// \\left\\{x\\in\\real^n~:~x_0 \\geq \\sqrt{\\sum_{i=1}^{n-1} x_i^2}\\right\\}
    /// $$
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `n` 
    /// - `dom_idx[1]` (out) Index of the domain
    pub fn get_domain_quadratic_cone(&mut self,n : i64) -> Result<i64,APIError>
    {
        // Arg processing order: task,n,dom_idx
        let mut dom_idx : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_domain_quadratic_cone(self.task,n,std::ptr::from_mut(&mut dom_idx)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(dom_idx)
    }
    /// Return the index of a rotated quadratic cone domain of the given size.
    /// 
    /// The rotated quadratic cone of size \\(n\\) is defined as
    /// $$
    /// \\left\{ x\\in \\real^3 ~:~ x_0 \\geq x_1 e^{x_2/x_1},\\ x_0,x_1\\geq; 0 \\right\\}
    /// $$
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `n` 
    /// - `dom_idx[1]` (out) Index of the domain
    pub fn get_domain_rotated_quadratic_cone(&mut self,n : i64) -> Result<i64,APIError>
    {
        // Arg processing order: task,n,dom_idx
        let mut dom_idx : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_domain_rotated_quadratic_cone(self.task,n,std::ptr::from_mut(&mut dom_idx)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(dom_idx)
    }
    /// Return index if an `primal_exponential` domain. Only one
    /// `primal_exponential` domain is created, so if one already exists,
    /// that one is returned instead of creating a new domain.
    /// 
    /// The primal exponential cone is defined as
    /// $$
    /// \\left\{ x\\in \\real^3 ~:~ x_0 \\geq x_1 e^{x_2/x_1},\\ x_0,x_1> 0 \\right\\}
    /// $$
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `dom_idx[1]` (out) Index of the domain
    pub fn get_domain_primal_exponential_cone(&mut self) -> Result<i64,APIError>
    {
        // Arg processing order: task,dom_idx
        let mut dom_idx : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_domain_primal_exponential_cone(self.task,std::ptr::from_mut(&mut dom_idx)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(dom_idx)
    }
    /// Return index if an `dual_exponential` domain. Only one
    /// `dual_exponential` domain is created, so if one already exists, that
    /// one is returned instead of creating a new domain
    /// 
    /// The dual exponential cone is defined as
    /// $$
    /// \\left\\{ x\\in \\real^3 ~:~ x_0 \\geq -x_2 e^{-1} e^{x_1/x_2},\\ x_0> 0,\\ x_2< 0 \\right\\}
    /// $$
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `dom_idx[1]` (out) Index of the domain
    pub fn get_domain_dual_exponential_cone(&mut self) -> Result<i64,APIError>
    {
        // Arg processing order: task,dom_idx
        let mut dom_idx : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_domain_dual_exponential_cone(self.task,std::ptr::from_mut(&mut dom_idx)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(dom_idx)
    }
    /// Return the index of a new primal power cone.
    /// 
    /// The primal power cone domain of dimension \\(n\\), with \\(n_\\ell\\) variables appearing on the left-hand side, where \\(n_\\ell\\) is the length of \\(\\alpha\\), and with a homogenous sequence of exponents \\(\\alpha_0,\\ldots,\\alpha_{n_\\ell-1}\\).
    /// 
    /// Formally, let \\(s = \\sum_i \\alpha_i\\) and \\(\\beta_i = \\alpha_i / s\\), so that \\(\\sum_i \\beta_i=1\\). Then the primal power cone is defined as follows:
    /// 
    /// $$
    /// \\left\\{ x\\in \\real^n ~:~ \\prod_{i=0}^{n_\\ell-1} x_i^{\\beta_i} \\geq \\sqrt{\\sum_{j=n_\\ell}^{n-1}x_j^2},\\ x_0\\ldots,x_{n_\\ell-1}\\geq 0 \\right\\}
    /// $$
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `n` 
    /// - `num_alpha` Number of alpha values in array
    /// - `dom_idx[1]` (out) Index of the domain
    /// - `alpha[num_alpha]` (in) Array if alpha values for power cone domain
    pub fn get_domain_primal_power_cone(&mut self,n : i64,alpha : &[f64]) -> Result<i64,APIError>
    {
        // Arg processing order: task,n,num_alpha,dom_idx,alpha
        let num_alpha = i64::try_from([Some(alpha.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let mut dom_idx : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_domain_primal_power_cone(self.task,n,num_alpha,alpha.as_ptr(),std::ptr::from_mut(&mut dom_idx)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(dom_idx)
    }
    /// Return the index of a new dual power cone.
    /// 
    /// Appends the dual power cone domain of dimension \\(n\\), with \\(n_\\ell\\) variables appearing on the left-hand side, where \\(n_\\ell\\) is the length of \\(\\alpha\\), and with a homogenous sequence of exponents \\(\\alpha_0,\\ldots,\\alpha_{n_\\ell-1}\\).
    /// 
    /// Formally, let \\(s = \\sum_i \\alpha_i\\) and \\(\\beta_i = \\alpha_i / s\\), so that \\(\\sum_i \\beta_i=1\\). Then the dual power cone is defined as follows:
    /// 
    /// $$
    /// \\left\\{ x\\in \\real^n ~:~ \\prod_{i=0}^{n_\\ell-1} \\left(\\frac{x_i}{\\beta_i}\\right)^{\\beta_i} \\geq \\sqrt{\\sum_{j=n_\\ell}^{n-1}x_j^2},\\ x_0\\ldots,x_{n_\\ell-1}\\geq 0 \\right\\}
    /// $$
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `n` 
    /// - `num_alpha` Number of alpha values in array
    /// - `dom_idx[1]` (out) Index of the domain
    /// - `alpha[num_alpha]` (in) Array if alpha values for power cone domain
    pub fn get_domain_dual_power_cone(&mut self,n : i64,alpha : &[f64]) -> Result<i64,APIError>
    {
        // Arg processing order: task,n,num_alpha,dom_idx,alpha
        let num_alpha = i64::try_from([Some(alpha.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let mut dom_idx : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_domain_dual_power_cone(self.task,n,num_alpha,alpha.as_ptr(),std::ptr::from_mut(&mut dom_idx)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(dom_idx)
    }
    /// Get the index of a new primal geometric mean cone.
    /// 
    /// The primal geometric mean cone is defined as
    /// $$
    /// \\left\\{ x\\in \\real^n ~:~ \\left(\\prod_{i=0}^{n-2} x_i\\right)^{1/(n-1)} \\geq |x_{n-1}|,\\ x_0\\ldots,x_{n-2}\\geq 0 \\right\\}
    /// $$
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `n` 
    /// - `dom_idx[1]` (out) Index of the domain
    pub fn get_domain_primal_geometric_mean_cone(&mut self,n : i64) -> Result<i64,APIError>
    {
        // Arg processing order: task,n,dom_idx
        let mut dom_idx : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_domain_primal_geometric_mean_cone(self.task,n,std::ptr::from_mut(&mut dom_idx)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(dom_idx)
    }
    /// Get the index of a new dual geometric mean cone.
    /// 
    /// The dual geometric mean cone is defined as
    /// $$
    /// \\left\\{ x\\in \\real^n ~:~ (n-1) \\left(\\prod_{i=0}^{n-2} x_i\\right)^{1/(n-1)} \\geq |x_{n-1}|,\\ x_0,\\ldots,x_{n-2}\\geq 0 \\right\\}
    /// $$
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `n` 
    /// - `dom_idx[1]` (out) Index of the domain
    pub fn get_domain_dual_geometric_mean_cone(&mut self,n : i64) -> Result<i64,APIError>
    {
        // Arg processing order: task,n,dom_idx
        let mut dom_idx : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_domain_dual_geometric_mean_cone(self.task,n,std::ptr::from_mut(&mut dom_idx)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(dom_idx)
    }
    /// Get the index of a new scaled vectorized PSD cone.
    /// 
    /// The domain consisting of vectors of length \\(n=d(d+1)/2\\) defined as follows
    /// 
    /// $$
    /// \\{(x_1,\\ldots,x_{d(d+1)/2})\\in \\real^n~:~ \\mathrm{sMat}(x)\\in\\PSD^d\\} = \\{\\mathrm{sVec}(X)~:~X\\in\\PSD^d\\},
    /// $$
    /// 
    /// where
    /// 
    /// $$
    /// \\mathrm{sVec}(X) = (X_{11},\\sqrt{2}X_{21},\\ldots,\\sqrt{2}X_{d1},X_{22},\\sqrt{2}X_{32},\\ldots,X_{dd}),
    /// $$
    /// 
    /// and
    /// 
    /// $$
    ///     \\mathrm{sMat}(x) = \\left[\\begin{array}{cccc}
    ///         x_1             & x_2/\\sqrt{2}      & \\cdots & x_{d}/\\sqrt{2} \\\\
    ///         x_2/\\sqrt{2}   & x_{d+1}            & \\cdots & x_{2d-1}/\\sqrt{2} \\\\
    ///         \\cdots         & \\cdots            & \\cdots & \\cdots \\\\
    ///         x_{d}/\\sqrt{2} & x_{2d-1}/\\sqrt{2} & \\cdots & x_{d(d+1)/2}
    ///     \\end{array}\\right].
    /// $$
    /// 
    /// In other words, the domain consists of vectorizations of the lower-triangular part of a positive semidefinite matrix, with the non-diagonal elements additionally rescaled.
    /// 
    /// This domain is a self-dual cone.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `n` The cone dimension. Note that only values such that \\(n\\cdot(n+1)/2\\) for some integer \\(d\\) are valid.
    /// - `dom_idx[1]` (out) Index of the domain
    pub fn get_domain_svecpsd_cone(&mut self,n : i64) -> Result<i64,APIError>
    {
        // Arg processing order: task,n,dom_idx
        let mut dom_idx : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_domain_svecpsd_cone(self.task,n,std::ptr::from_mut(&mut dom_idx)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(dom_idx)
    }
    /// Get domain information
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `dom_idx` Index of the domain
    /// - `dom_type[1]` (out) 
    /// - `size[1]` (out) 
    /// - `num_alpha[1]` (out) Number of alpha values in array
    pub fn get_domain_info(&self,dom_idx : i64) -> Result<(DomainType,i64,i32),APIError>
    {
        // Arg processing order: task,dom_idx,dom_type,size,num_alpha
        let mut dom_type : i32 = 0;
        let mut size : i64 = Default::default();
        let mut num_alpha : i32 = Default::default();
        let returned_value = unsafe{ MSK12_get_domain_info(self.task,dom_idx,std::ptr::from_mut(&mut dom_type),std::ptr::from_mut(&mut size),std::ptr::from_mut(&mut num_alpha)) };
        if 0 != returned_value { self.last_error()?; }
        let enum_dom_type_ = DomainType::from(dom_type).map_err(|i| APIError::from("err_invalid_enum_value","",format!("Invalid value ({0}) for enum DomainType",i)))?;
        Ok((enum_dom_type_,size,num_alpha))
    }
    /// For primal and dual power domains, get domain alpha vector
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `dom_idx` Index of the domain
    /// - `num_alpha` Number of alpha values in array
    /// - `alpha[num_alpha]` (out) Array if alpha values for power cone domain
    pub fn get_domain_alpha(&self,dom_idx : i64,alpha : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,dom_idx,num_alpha,alpha
        let num_alpha = i64::try_from([Some(alpha.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_domain_alpha(self.task,dom_idx,num_alpha,alpha.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Input linear terms for a single affine row.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `row_idx` Index of the affine row
    /// - `num_nz` Number of nonzeros
    /// - `subj[num_nz]` (in) Column indexes
    /// - `cof[num_nz]` (in) Coefficients
    pub fn put_row(&mut self,row_idx : i64,subj : &[i32],cof : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,row_idx,num_nz,subj,cof
        let num_nz = i32::try_from([Some(subj.len()),Some(cof.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_row(self.task,row_idx,num_nz,subj.as_ptr(),cof.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Input linear terms for a slice of affine rows.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_row` Index of first affine row
    /// - `num_row` Number of rows in the slice
    /// - `row_num_nz[num_row]` (in) Number of non-zeros per row.
    /// - `subj` (in) Column subscripts
    /// - `cof` (in) Coefficients
    pub fn put_row_slice(&mut self,first_row : i64,row_num_nz : &[i32],subj : &[i32],cof : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_row,num_row,row_num_nz,subj,cof
        let num_row = i64::try_from([Some(row_num_nz.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_row_slice(self.task,first_row,num_row,row_num_nz.as_ptr(),subj.as_ptr(),cof.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Input linear terms for a list of affine rows. This accepts subscripts and coefficients where rows are non-continuous.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_row` Number of rows in list
    /// - `row_idxs[num_row]` (in) Row indexes
    /// - `row_num_nz[num_row]` (in) Number of non-zeros per row.
    /// - `subj` (in) List of pointers to subscripts.
    /// - `cof` (in) Coefficients
    pub fn put_row_list(&mut self,row_idxs : &[i64],row_num_nz : &[i32],subj : &[&[i32]],cof : &[&[f64]]) -> Result<(),APIError>
    {
        // Arg processing order: task,num_row,row_idxs,row_num_nz,subj,cof
        let num_row = i64::try_from([Some(row_idxs.len()),Some(row_num_nz.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let subj_ptrs : Vec<*const i32> = subj.iter().map(|entry| entry.as_ptr()).collect();
        let cof_ptrs : Vec<*const f64> = cof.iter().map(|entry| entry.as_ptr()).collect();
        let returned_value = unsafe{ MSK12_put_row_list(self.task,num_row,row_idxs.as_ptr(),row_num_nz.as_ptr(),subj_ptrs.as_ptr(),cof_ptrs.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Input the constant term for a single affine row
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `row_idx` Index of the affine row
    /// - `g` Row fixed term
    pub fn put_row_g(&mut self,row_idx : i64,g : f64) -> Result<(),APIError>
    {
        // Arg processing order: task,row_idx,g
        let returned_value = unsafe{ MSK12_put_row_g(self.task,row_idx,g) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Input the constant terms for a slice of affine rows.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_row` Index of the first affine row in a slice
    /// - `num_row` Number of affine rows
    /// - `g[num_row]` (in) Row fixed term
    pub fn put_row_slice_g(&mut self,first_row : i64,g : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_row,num_row,g
        let num_row = i64::try_from([Some(g.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_row_slice_g(self.task,first_row,num_row,g.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Input the constant terms for a list of affine rows.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_row` Number of affine rows
    /// - `row_idxs[num_row]` (in) Array of row indexes
    /// - `g[num_row]` (in) Row fixed term
    pub fn put_row_list_g(&mut self,row_idxs : &[i64],g : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,num_row,row_idxs,g
        let num_row = i64::try_from([Some(row_idxs.len()),Some(g.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_row_list_g(self.task,num_row,row_idxs.as_ptr(),g.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Put a single column.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `col_idx` Variable index
    /// - `num_nz` 
    /// - `row_idxs[num_nz]` (in) Array of row indexes
    /// - `cof[num_nz]` (in) Coefficients
    pub fn put_col(&mut self,col_idx : i32,row_idxs : &[i64],cof : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,col_idx,num_nz,row_idxs,cof
        let num_nz = i64::try_from([Some(row_idxs.len()),Some(cof.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_col(self.task,col_idx,num_nz,row_idxs.as_ptr(),cof.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Put a slice of columns.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_col` Index of the first columns index in a slice
    /// - `num_col` Number of columns
    /// - `col_len[num_col]` (in) 
    /// - `row_idxs` (in) Array of row indexes
    /// - `cof` (in) Coefficients
    pub fn put_col_slice(&mut self,first_col : i32,col_len : &[i64],row_idxs : &[i64],cof : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_col,num_col,col_len,row_idxs,cof
        let num_col = i32::try_from([Some(col_len.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_col_slice(self.task,first_col,num_col,col_len.as_ptr(),row_idxs.as_ptr(),cof.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Put a list of columns where the individual columns can be non-contiguous. 
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_col` Number of columns
    /// - `col_idxs[num_col]` (in) Array of variable indexes
    /// - `col_lens[num_col]` (in) Array of columns lengths
    /// - `row_idxs` (in) Array of row indexes
    /// - `cof` (in) Coefficients
    pub fn put_col_list(&mut self,col_idxs : &[i32],col_lens : &[i64],row_idxs : &[&[i64]],cof : &[&[f64]]) -> Result<(),APIError>
    {
        // Arg processing order: task,num_col,col_idxs,col_lens,row_idxs,cof
        let num_col = i32::try_from([Some(col_idxs.len()),Some(col_lens.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let row_idxs_ptrs : Vec<*const i64> = row_idxs.iter().map(|entry| entry.as_ptr()).collect();
        let cof_ptrs : Vec<*const f64> = cof.iter().map(|entry| entry.as_ptr()).collect();
        let returned_value = unsafe{ MSK12_put_col_list(self.task,num_col,col_idxs.as_ptr(),col_lens.as_ptr(),row_idxs_ptrs.as_ptr(),cof_ptrs.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Put a non-zero triplet
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `row_idx` Index of the affine row
    /// - `var_idx` Variable index
    /// - `cof` Coefficients
    pub fn put_ijc(&mut self,row_idx : i64,var_idx : i32,cof : f64) -> Result<(),APIError>
    {
        // Arg processing order: task,row_idx,var_idx,cof
        let returned_value = unsafe{ MSK12_put_ijc(self.task,row_idx,var_idx,cof) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Put a list of non-zero triplets
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_nz` 
    /// - `row_idxs[num_nz]` (in) Array of row indexes
    /// - `col_idxs[num_nz]` (in) Array of variable indexes
    /// - `cof[num_nz]` (in) Coefficients
    pub fn put_ijc_list(&mut self,row_idxs : &[i64],col_idxs : &[i32],cof : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,num_nz,row_idxs,col_idxs,cof
        let num_nz = i64::try_from([Some(row_idxs.len()),Some(col_idxs.len()),Some(cof.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_ijc_list(self.task,num_nz,row_idxs.as_ptr(),col_idxs.as_ptr(),cof.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `row_idx` Index of the affine row
    /// - `num_nz[1]` (out) 
    pub fn get_row_num_nz(&self,row_idx : i64) -> Result<i32,APIError>
    {
        // Arg processing order: task,row_idx,num_nz
        let mut num_nz : i32 = Default::default();
        let returned_value = unsafe{ MSK12_get_row_num_nz(self.task,row_idx,std::ptr::from_mut(&mut num_nz)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(num_nz)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_row` Index of the first affine row in a slice
    /// - `num_row` Number of affine rows
    /// - `num_nz[num_row]` (out) 
    pub fn get_row_slice_num_nz(&self,first_row : i64,num_nz : &mut [i64]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_row,num_row,num_nz
        let num_row = i64::try_from([Some(num_nz.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_row_slice_num_nz(self.task,first_row,num_row,num_nz.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get nonzeros from a single row.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `row_idx` Index of the affine row
    /// - `nnz` Number of nonzeros
    /// - `subj[nnz]` (out) Variable indexes
    /// - `cof[nnz]` (out) Coefficients
    pub fn get_row(&self,row_idx : i64,subj : &mut [i32],cof : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,row_idx,nnz,subj,cof
        let nnz = i32::try_from([Some(subj.len()),Some(cof.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_row(self.task,row_idx,nnz,subj.as_mut_ptr(),cof.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get nonzeros from a slice of rows.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_row` Index of the first affine row in a slice
    /// - `num_row` Number of affine rows
    /// - `nnz` Number of nonzeros
    /// - `row_len[num_row]` (out) 
    /// - `subj[nnz]` (out) Variable indexes
    /// - `cof[nnz]` (out) Coefficients
    pub fn get_row_slice(&self,first_row : i64,row_len : &mut [i32],subj : &mut [i32],cof : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_row,num_row,nnz,row_len,subj,cof
        let num_row = i64::try_from([Some(row_len.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let nnz = i64::try_from([Some(subj.len()),Some(cof.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_row_slice(self.task,first_row,num_row,nnz,row_len.as_mut_ptr(),subj.as_mut_ptr(),cof.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `col_idx` Variable index
    /// - `num_nz[1]` (out) 
    pub fn get_col_num_nz(&self,col_idx : i32) -> Result<i64,APIError>
    {
        // Arg processing order: task,col_idx,num_nz
        let mut num_nz : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_col_num_nz(self.task,col_idx,std::ptr::from_mut(&mut num_nz)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(num_nz)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_col` Index of the first columns index in a slice
    /// - `num_col` Number of columns
    /// - `num_nz[num_col]` (out) 
    pub fn get_col_slice_num_nz(&self,first_col : i32,num_nz : &mut [i64]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_col,num_col,num_nz
        let num_col = i32::try_from([Some(num_nz.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_col_slice_num_nz(self.task,first_col,num_col,num_nz.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get nonzeros from a single column.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `col_idx` Variable index
    /// - `nnz` Number of nonzeros
    /// - `subi[nnz]` (out) 
    /// - `cof[nnz]` (out) Coefficients
    pub fn get_col(&self,col_idx : i32,subi : &mut [i64],cof : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,col_idx,nnz,subi,cof
        let nnz = i64::try_from([Some(subi.len()),Some(cof.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_col(self.task,col_idx,nnz,subi.as_mut_ptr(),cof.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get nonzeros from a slice of columns.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_col` Index of the first columns index in a slice
    /// - `num_col` Number of columns
    /// - `nnz` Number of nonzeros
    /// - `col_len[num_col]` (out) 
    /// - `subi[nnz]` (out) 
    /// - `cof[nnz]` (out) Coefficients
    pub fn get_col_slice(&self,first_col : i32,col_len : &mut [i64],subi : &mut [i64],cof : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_col,num_col,nnz,col_len,subi,cof
        let num_col = i32::try_from([Some(col_len.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let nnz = i64::try_from([Some(subi.len()),Some(cof.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_col_slice(self.task,first_col,num_col,nnz,col_len.as_mut_ptr(),subi.as_mut_ptr(),cof.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Input a single bar entry.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `row_idx` Index of the affine row
    /// - `barvar_idx` Positive semi-definite variable index
    /// - `num_weight` 
    /// - `matrix_idx[num_weight]` (in) 
    /// - `weight[num_weight]` (in) 
    pub fn put_bar_entry(&mut self,row_idx : i64,barvar_idx : i32,matrix_idx : &[i64],weight : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,row_idx,barvar_idx,num_weight,matrix_idx,weight
        let num_weight = i64::try_from([Some(matrix_idx.len()),Some(weight.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_bar_entry(self.task,row_idx,barvar_idx,num_weight,matrix_idx.as_ptr(),weight.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Input a list of bar entries.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_bar_entry` Number of entries
    /// - `row_idx[num_bar_entry]` (in) Row index list
    /// - `barvar_idx[num_bar_entry]` (in) Bar variable index list
    /// - `num_weight[num_bar_entry]` (in) Per entry, the number of terms
    /// - `matrix_idx` (in) Matrix indexes
    /// - `weight` (in) 
    pub fn put_bar_entry_list(&mut self,row_idx : &[i64],barvar_idx : &[i32],num_weight : &[i64],matrix_idx : &[i64],weight : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,num_bar_entry,row_idx,barvar_idx,num_weight,matrix_idx,weight
        let num_bar_entry = i64::try_from([Some(row_idx.len()),Some(barvar_idx.len()),Some(num_weight.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_bar_entry_list(self.task,num_bar_entry,row_idx.as_ptr(),barvar_idx.as_ptr(),num_weight.as_ptr(),matrix_idx.as_ptr(),weight.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Put bar entries for a single row, replacing all existing entries
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `row_idx` Index of the affine row
    /// - `num_bar_entry` 
    /// - `barvar_idx[num_bar_entry]` (in) Positive semi-definite variable index
    /// - `num_weight[num_bar_entry]` (in) 
    /// - `matrix_idx` (in) 
    /// - `weight` (in) 
    pub fn put_bar_row(&mut self,row_idx : i64,barvar_idx : &[i32],num_weight : &[i64],matrix_idx : &[i64],weight : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,row_idx,num_bar_entry,barvar_idx,num_weight,matrix_idx,weight
        let num_bar_entry = i32::try_from([Some(barvar_idx.len()),Some(num_weight.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_bar_row(self.task,row_idx,num_bar_entry,barvar_idx.as_ptr(),num_weight.as_ptr(),matrix_idx.as_ptr(),weight.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get information on symmetric matrix.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `symmat_idx` Symmetric matrix index
    /// - `dim[1]` (out) Dimension
    /// - `nnz[1]` (out) Number of nonzeros
    pub fn get_symmat_info(&self,symmat_idx : i64) -> Result<(i32,i64),APIError>
    {
        // Arg processing order: task,symmat_idx,dim,nnz
        let mut dim : i32 = Default::default();
        let mut nnz : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_symmat_info(self.task,symmat_idx,std::ptr::from_mut(&mut dim),std::ptr::from_mut(&mut nnz)) };
        if 0 != returned_value { self.last_error()?; }
        Ok((dim,nnz))
    }
    /// Get symmetric matrix data
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `symmat_idx` Symmetric matrix index
    /// - `nnz` Number of nonzeros
    /// - `symmat_i[nnz]` (out) Symmetric matrix row subscripts
    /// - `symmat_j[nnz]` (out) Symmetric matrix column subscripts
    /// - `symmat_val[nnz]` (out) Symmetric matrix values
    pub fn get_symmat(&self,symmat_idx : i64,symmat_i : &mut [i32],symmat_j : &mut [i32],symmat_val : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,symmat_idx,nnz,symmat_i,symmat_j,symmat_val
        let nnz = i64::try_from([Some(symmat_i.len()),Some(symmat_j.len()),Some(symmat_val.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_symmat(self.task,symmat_idx,nnz,symmat_i.as_mut_ptr(),symmat_j.as_mut_ptr(),symmat_val.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get information on a slice of symmetric matrixes.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_symmat` Index of the first symmetric matrix in a slice
    /// - `num_symmat` Number of symmetric matrixes
    /// - `dim[num_symmat]` (out) Dimension
    /// - `nnz[num_symmat]` (out) Number of nonzeros
    pub fn get_symmat_slice_info(&self,first_symmat : i64,dim : &mut [i32],nnz : &mut [i64]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_symmat,num_symmat,dim,nnz
        let num_symmat = i64::try_from([Some(dim.len()),Some(nnz.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_symmat_slice_info(self.task,first_symmat,num_symmat,dim.as_mut_ptr(),nnz.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get information on a slice of symmetric matrixes.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_symmat` First symmetric matrix in slice
    /// - `num_symmat` Number of symmetric matrixes in alice
    /// - `total_nnz` Expected number of nonzeros
    /// - `symmat_i[total_nnz]` (out) Symmetric matrix row subscripts
    /// - `symmat_j[total_nnz]` (out) Symmetric matrix column subscripts
    /// - `symmat_val[total_nnz]` (out) Symmetric matrix values
    pub fn get_symmat_slice(&self,first_symmat : i64,num_symmat : i64,symmat_i : &mut [i32],symmat_j : &mut [i32],symmat_val : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_symmat,num_symmat,total_nnz,symmat_i,symmat_j,symmat_val
        let total_nnz = i64::try_from([Some(symmat_i.len()),Some(symmat_j.len()),Some(symmat_val.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_symmat_slice(self.task,first_symmat,num_symmat,total_nnz,symmat_i.as_mut_ptr(),symmat_j.as_mut_ptr(),symmat_val.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Append constraints.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `dom_idx` Index of the domain
    /// - `num_rows` Row count for the constraint block.
    /// - `row_idxs[num_rows]` (in) Array of row indexes
    /// - `con_offset[num_rows]` (in) Constraint right-hand-side offset vector, where NULL means all zeros 
    pub fn append_con(&self,dom_idx : i64,row_idxs : &[i64],con_offset : Option<&[f64]>) -> Result<(),APIError>
    {
        // Arg processing order: task,dom_idx,num_rows,row_idxs,con_offset
        let num_rows = i64::try_from([Some(row_idxs.len()),con_offset.map(|v| v.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_append_con(self.task,dom_idx,num_rows,row_idxs.as_ptr(),con_offset.map(|a| a.as_ptr()).unwrap_or(std::ptr::null())) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Append constraints.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_con` Number of constraint blocks to add.
    /// - `dom_idxs[num_con]` (in) Index of the domain to use. The domain's size must be exactly `num_rows`.
    /// - `num_rows[num_con]` (in) List of row counts for each constraint block.
    /// - `row_idxs` (in) Array of row indexes
    /// - `con_offset` (in) Constraint right-hand-side offset vector, where NULL means all zeros 
    pub fn append_cons(&self,dom_idxs : &[i64],num_rows : &[i64],row_idxs : &[i64],con_offset : Option<&[f64]>) -> Result<(),APIError>
    {
        // Arg processing order: task,num_con,dom_idxs,num_rows,row_idxs,con_offset
        let num_con = i64::try_from([Some(dom_idxs.len()),Some(num_rows.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_append_cons(self.task,num_con,dom_idxs.as_ptr(),num_rows.as_ptr(),row_idxs.as_ptr(),con_offset.map(|a| a.as_ptr()).unwrap_or(std::ptr::null())) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Put domain and row indexes for constraint `index`.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `con_idx` Index of the constraint to change.
    /// - `num_rows` Total number of scalar rows we will input.
    /// - `dom_idx` Index of the domain to use. The domain's size must be exactly `num_rows`. 
    /// - `row_idxs[num_rows]` (in) Array of row indexes
    /// - `rhs_offset[num_rows]` (in) Domain offset
    pub fn put_con(&mut self,con_idx : i64,dom_idx : i64,row_idxs : &[i64],rhs_offset : Option<&[f64]>) -> Result<(),APIError>
    {
        // Arg processing order: task,con_idx,num_rows,dom_idx,row_idxs,rhs_offset
        let num_rows = i64::try_from([Some(row_idxs.len()),rhs_offset.map(|v| v.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_con(self.task,con_idx,num_rows,dom_idx,row_idxs.as_ptr(),rhs_offset.map(|a| a.as_ptr()).unwrap_or(std::ptr::null())) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Put domain and row indexes for a single scalar constraint `index`. 
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `con_idx` Index of the constraint to change.
    /// - `dom_idx` Index of the domain to use. The domain's size must be exactly 1. 
    /// - `row_idx` Index of the affine row
    /// - `rhs_offset` Domain offset
    pub fn put_scalar_con(&mut self,con_idx : i64,dom_idx : i64,row_idx : i64,rhs_offset : f64) -> Result<(),APIError>
    {
        // Arg processing order: task,con_idx,dom_idx,row_idx,rhs_offset
        let returned_value = unsafe{ MSK12_put_scalar_con(self.task,con_idx,dom_idx,row_idx,rhs_offset) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Put domains and row indexes for a slice of constraints.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_con` Index of the constraint to change.
    /// - `num_con` Index of the constraint to change.
    /// - `num_rows` Total number of scalar rows we will input.
    /// - `dom_idx[num_con]` (in) Indexes of the domains to use. The sum of domain sizees must be exactly `num_rows`. 
    /// - `row_idx[num_rows]` (in) Indexes of the scalar affine rows
    /// - `rhs_offset[num_rows]` (in) Domain offset
    pub fn put_con_slice(&mut self,first_con : i64,dom_idx : &[i64],row_idx : &[i64],rhs_offset : Option<&[f64]>) -> Result<(),APIError>
    {
        // Arg processing order: task,first_con,num_con,num_rows,dom_idx,row_idx,rhs_offset
        let num_con = i64::try_from([Some(dom_idx.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let num_rows = i64::try_from([Some(row_idx.len()),rhs_offset.map(|v| v.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_con_slice(self.task,first_con,num_con,num_rows,dom_idx.as_ptr(),row_idx.as_ptr(),rhs_offset.map(|a| a.as_ptr()).unwrap_or(std::ptr::null())) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get domains from a slice of constraints
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_con` First constraint
    /// - `num_con` Number of constraints
    /// - `dom_idx[num_con]` (out) Index of the domain
    pub fn get_con_slice_domains(&self,first_con : i64,dom_idx : &mut [i64]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_con,num_con,dom_idx
        let num_con = i64::try_from([Some(dom_idx.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_con_slice_domains(self.task,first_con,num_con,dom_idx.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get number of rows in a slice of constraints.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_con` First constraint index in a slice
    /// - `num_con` Number of constraints
    /// - `num_row[1]` (out) Number of affine rows
    pub fn get_con_slice_num_row(&self,first_con : i64,num_con : i64) -> Result<i64,APIError>
    {
        // Arg processing order: task,first_con,num_con,num_row
        let mut num_row : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_con_slice_num_row(self.task,first_con,num_con,std::ptr::from_mut(&mut num_row)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(num_row)
    }
    /// Get domains and indexes for a slice of constraints. Note that if the output is longer than the provided buffers, the result is silently truncated.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_con` Constraint index
    /// - `num_con` Number of constraint
    /// - `num_row` Length of row_index.
    /// - `row_idx[num_row]` (out) Index of the affine row
    /// - `rhs_offset[num_row]` (out) Domain offset
    /// - `dom_idx[num_con]` (out) Index of the domain
    pub fn get_con_slice(&self,first_con : i64,row_idx : &mut [i64],rhs_offset : &mut [f64],dom_idx : &mut [i64]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_con,num_con,num_row,row_idx,rhs_offset,dom_idx
        let num_con = i64::try_from([Some(dom_idx.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let num_row = i64::try_from([Some(row_idx.len()),Some(rhs_offset.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_con_slice(self.task,first_con,num_con,num_row,row_idx.as_mut_ptr(),rhs_offset.as_mut_ptr(),dom_idx.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Put domain and row indexes for constraint `index`.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_rows` Total number of scalar rows we will input.
    /// - `num_dom` Number of domains
    /// - `num_terms` Number of terms
    /// - `dom_idx[num_dom]` (in) Index of the domains to use. The sum of the domain sizes must sum to `num_rows`.
    /// - `term_size[num_terms]` (in) Term sizes. Each entry denotes the number of domains in the corresponding term. These must sum to `num_domains`
    /// - `row_idx[num_rows]` (in) Indexes of the scalar affine rows
    /// - `rhs_offset[num_rows]` (in) Domain offset
    pub fn append_djc(&mut self,dom_idx : &[i64],term_size : &[i64],row_idx : &[i64],rhs_offset : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,num_rows,num_dom,num_terms,dom_idx,term_size,row_idx,rhs_offset
        let num_rows = i64::try_from([Some(row_idx.len()),Some(rhs_offset.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let num_dom = i64::try_from([Some(dom_idx.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let num_terms = i64::try_from([Some(term_size.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_append_djc(self.task,num_rows,num_dom,num_terms,dom_idx.as_ptr(),term_size.as_ptr(),row_idx.as_ptr(),rhs_offset.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Put domain and row indexes for constraint `index`.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `djc_idx` Index of the djc to change.
    /// - `num_rows` Total number of scalar rows we will input.
    /// - `num_dom` Number of domains
    /// - `num_terms` Number of terms
    /// - `dom_idx[num_dom]` (in) Index of the domains to use. The sum of the domain sizes must sum to `num_rows`.
    /// - `term_size[num_terms]` (in) Term sizes. Each entry denotes the number of domains in the corresponding term. These must sum to `num_domains`
    /// - `row_idx[num_rows]` (in) Indexes of the scalar affine rows
    /// - `rhs_offset[num_rows]` (in) Domain offset
    pub fn put_djc(&mut self,djc_idx : i64,dom_idx : &[i64],term_size : &[i64],row_idx : &[i64],rhs_offset : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,djc_idx,num_rows,num_dom,num_terms,dom_idx,term_size,row_idx,rhs_offset
        let num_rows = i64::try_from([Some(row_idx.len()),Some(rhs_offset.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let num_dom = i64::try_from([Some(dom_idx.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let num_terms = i64::try_from([Some(term_size.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_djc(self.task,djc_idx,num_rows,num_dom,num_terms,dom_idx.as_ptr(),term_size.as_ptr(),row_idx.as_ptr(),rhs_offset.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Put domains and row indexes for a slice of constraints.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_djc` Index of the constraint to change.
    /// - `num_djc` Index of the constraint to change.
    /// - `num_rows` Total number of scalar rows we will input.
    /// - `num_dom` Number of domains
    /// - `num_terms` Number of terms
    /// - `dom_idx[num_dom]` (in) Index of the domains to use. The sum of the domain sizes must sum to `num_rows`.
    /// - `term_size[num_terms]` (in) Term sizes. Each entry denotes the number of domains in the corresponding term. These must sum to `num_domains` 
    /// - `row_idx[num_rows]` (in) Indexes of the scalar affine rows
    /// - `rhs_offset[num_rows]` (in) Right-hand-side offset
    /// - `djc_numterm[num_djc]` (in) 
    pub fn put_djc_slice(&mut self,first_djc : i64,dom_idx : &[i64],term_size : &[i64],row_idx : &[i64],rhs_offset : &[f64],djc_numterm : &[i64]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_djc,num_djc,num_rows,num_dom,num_terms,dom_idx,term_size,row_idx,rhs_offset,djc_numterm
        let num_djc = i64::try_from([Some(djc_numterm.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let num_rows = i64::try_from([Some(row_idx.len()),Some(rhs_offset.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let num_dom = i64::try_from([Some(dom_idx.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let num_terms = i64::try_from([Some(term_size.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_djc_slice(self.task,first_djc,num_djc,num_rows,num_dom,num_terms,dom_idx.as_ptr(),term_size.as_ptr(),row_idx.as_ptr(),rhs_offset.as_ptr(),djc_numterm.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get information in a single DJC.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `djc_idx` DJC index
    /// - `num_term[1]` (out) number of terms in DJC
    /// - `num_dom[1]` (out) Number of domains
    /// - `num_row[1]` (out) Number of affine rows
    pub fn get_djc_info(&self,djc_idx : i64) -> Result<(i64,i64,i64),APIError>
    {
        // Arg processing order: task,djc_idx,num_term,num_dom,num_row
        let mut num_term : i64 = Default::default();
        let mut num_dom : i64 = Default::default();
        let mut num_row : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_djc_info(self.task,djc_idx,std::ptr::from_mut(&mut num_term),std::ptr::from_mut(&mut num_dom),std::ptr::from_mut(&mut num_row)) };
        if 0 != returned_value { self.last_error()?; }
        Ok((num_term,num_dom,num_row))
    }
    /// Get DJC data. Together with `MSK120_get_djc_info` this extracts DJC data.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `djc_idx` DJC index
    /// - `num_terms` Expect number of terms
    /// - `num_dom` Expect number of domain indexes
    /// - `num_row` Expect number of rows
    /// - `term_size[num_terms]` (out) 
    /// - `dom_idx[num_dom]` (out) Index of the domain
    /// - `row_idx[num_row]` (out) Index of the affine row
    /// - `rhs_offset[num_row]` (out) Domain offset
    pub fn get_djc(&self,djc_idx : i64,term_size : &mut [i64],dom_idx : &mut [i64],row_idx : &mut [i64],rhs_offset : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,djc_idx,num_terms,num_dom,num_row,term_size,dom_idx,row_idx,rhs_offset
        let num_terms = i64::try_from([Some(term_size.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let num_dom = i64::try_from([Some(dom_idx.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let num_row = i64::try_from([Some(row_idx.len()),Some(rhs_offset.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_djc(self.task,djc_idx,num_terms,num_dom,num_row,term_size.as_mut_ptr(),dom_idx.as_mut_ptr(),row_idx.as_mut_ptr(),rhs_offset.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get information on a slice of DJCs. This will return total sizes for terms, clauses and rows. To get information on the number of clauses and rows for each DJC, call `MSK120_get_djc_slice_term`.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_djc` first DJC index
    /// - `num_djc` number of DJCs
    /// - `num_term[1]` (out) total number of terms
    /// - `num_dom[1]` (out) total number of clauses/domains
    /// - `num_row[1]` (out) total number of rows
    pub fn get_djc_slice_info(&self,first_djc : i64,num_djc : i64) -> Result<(i64,i64,i64),APIError>
    {
        // Arg processing order: task,first_djc,num_djc,num_term,num_dom,num_row
        let mut num_term : i64 = Default::default();
        let mut num_dom : i64 = Default::default();
        let mut num_row : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_djc_slice_info(self.task,first_djc,num_djc,std::ptr::from_mut(&mut num_term),std::ptr::from_mut(&mut num_dom),std::ptr::from_mut(&mut num_row)) };
        if 0 != returned_value { self.last_error()?; }
        Ok((num_term,num_dom,num_row))
    }
    /// Get information on a terms of a slice of DJCs. This will return total sizes for terms, clauses and rows. Together with `MSK120_get_djc_slice_info` this extracts DJC slice data.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `first_djc` first DJC index
    /// - `num_djc` number of DJCs
    /// - `num_term` Expect number of terms
    /// - `num_dom` Expect number of domain indexes
    /// - `num_row` Expect number of rows
    /// - `term_size[num_term]` (out) 
    /// - `dom_idx[num_dom]` (out) Index of the domain
    /// - `row_idx[num_row]` (out) Index of the affine row
    /// - `rhs_offset[num_row]` (out) Domain offset
    /// - `djc_num_term[num_djc]` (out) 
    pub fn get_djc_slice(&self,first_djc : i64,term_size : &mut [i64],dom_idx : &mut [i64],row_idx : &mut [i64],rhs_offset : &mut [f64],djc_num_term : &mut [i64]) -> Result<(),APIError>
    {
        // Arg processing order: task,first_djc,num_djc,num_term,num_dom,num_row,term_size,dom_idx,row_idx,rhs_offset,djc_num_term
        let num_djc = i64::try_from([Some(djc_num_term.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let num_term = i64::try_from([Some(term_size.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let num_dom = i64::try_from([Some(dom_idx.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let num_row = i64::try_from([Some(row_idx.len()),Some(rhs_offset.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_djc_slice(self.task,first_djc,num_djc,num_term,num_dom,num_row,term_size.as_mut_ptr(),dom_idx.as_mut_ptr(),row_idx.as_mut_ptr(),rhs_offset.as_mut_ptr(),djc_num_term.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Input objective sense.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sense` 
    pub fn put_obj_sense(&mut self,sense : ObjSense)
    {
        // Arg processing order: task,sense
        unsafe{ MSK12_put_obj_sense(self.task,sense as i32) };
    }
    /// Get objectiev sense. This cannot fail.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    pub fn get_obj_sense(&self) -> Result<ObjSense,APIError>
    {
        // Arg processing order: task
        let returned_value = unsafe{ MSK12_get_obj_sense(self.task) };
        Ok(ObjSense::from(returned_value).map_err(|i| APIError::from("err_invalid_enum_value","",format!("Invalid value ({0}) for enum ObjSense",i)))?)
    }
    /// Set the affine row to use as objective
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `row_idx` Index of the affine row
    pub fn put_obj_row(&mut self,row_idx : i64) -> Result<(),APIError>
    {
        // Arg processing order: task,row_idx
        let returned_value = unsafe{ MSK12_put_obj_row(self.task,row_idx) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get objective row index. If the objective row is set, `asgn[0]` will be set to 1 and `index[0]` is set to the row index, otherwise `asgn[0]` is set to 0.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `row_idx[1]` (out) Index of the affine row
    /// - `asgn[1]` (out) Returns non-zero to indicate that a value was assigned, or zero if it was not
    pub fn get_obj_row(&self) -> (i64,bool)
    {
        // Arg processing order: task,row_idx,asgn
        let mut row_idx : i64 = Default::default();
        let mut asgn : i32 = 0;
        unsafe{ MSK12_get_obj_row(self.task,std::ptr::from_mut(&mut row_idx),std::ptr::from_mut(&mut asgn)) };
        (row_idx,asgn != 0)
    }
    /// Call optimizer. On return, all input solutions have been cleared.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `trm[1]` (out) 
    pub fn optimize(&mut self) -> Result<i32,APIError>
    {
        // Arg processing order: task,trm
        let mut trm : i32 = Default::default();
        let returned_value = unsafe{ MSK12_optimize(self.task,std::ptr::from_mut(&mut trm)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(trm)
    }
    /// Prints a short summary of the current solutions. 
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `whichstream` 
    pub fn solution_summary(&mut self,whichstream : StreamType) -> Result<(),APIError>
    {
        // Arg processing order: task,whichstream
        let returned_value = unsafe{ MSK12_solution_summary(self.task,whichstream as i32) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Specify a remote OptServer to use instead of built-in solver.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `server[.cstring]` (in) Server name with protocol and port, e.g. `https://optserver.mydomain:9876`.
    /// - `cert[.cstring]` (in) 
    pub fn put_remote_solver(&mut self,server : &str,cert : Option<&str>) -> Result<(),APIError>
    {
        // Arg processing order: task,server,cert
        let cstring_server_ =
          CString::new(server)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: server")))?;
        let cstring_cert_ : Option<CString> =
          cert.map(|n| CString::new(n).map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: {0}",n))))
            .transpose()?;
        unsafe{ MSK12_put_remote_solver(self.task,cstring_server_.as_ptr(),cstring_cert_.map(|s| s.as_ptr()).unwrap_or(std::ptr::null())) };
        Ok(())
    }
    /// Specify access token to be used for remote solving.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `token[.cstring]` (in) 
    pub fn put_optserver_access_token(&mut self,token : Option<&str>) -> Result<(),APIError>
    {
        // Arg processing order: task,token
        let cstring_token_ : Option<CString> =
          token.map(|n| CString::new(n).map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: {0}",n))))
            .transpose()?;
        unsafe{ MSK12_put_optserver_access_token(self.task,cstring_token_.map(|s| s.as_ptr()).unwrap_or(std::ptr::null())) };
        Ok(())
    }
    /// Get number of solutions. This cannot fail.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    pub fn get_num_sol(&self) -> i32
    {
        // Arg processing order: task
        let returned_value = unsafe{ MSK12_get_num_sol(self.task) };
        returned_value
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `sol_type[1]` (out) Returns the type of the solution requested
    pub fn get_sol_type(&self,sol_idx : i32) -> Result<SolType,APIError>
    {
        // Arg processing order: task,sol_idx,sol_type
        let mut sol_type : i32 = 0;
        let returned_value = unsafe{ MSK12_get_sol_type(self.task,sol_idx,std::ptr::from_mut(&mut sol_type)) };
        if 0 != returned_value { self.last_error()?; }
        let enum_sol_type_ = SolType::from(sol_type).map_err(|i| APIError::from("err_invalid_enum_value","",format!("Invalid value ({0}) for enum SolType",i)))?;
        Ok(enum_sol_type_)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `primal_sol_sta[1]` (out) 
    /// - `dual_sol_sta[1]` (out) 
    pub fn get_sol_status(&self,sol_idx : i32) -> Result<(SolSta,SolSta),APIError>
    {
        // Arg processing order: task,sol_idx,primal_sol_sta,dual_sol_sta
        let mut primal_sol_sta : i32 = 0;
        let mut dual_sol_sta : i32 = 0;
        let returned_value = unsafe{ MSK12_get_sol_status(self.task,sol_idx,std::ptr::from_mut(&mut primal_sol_sta),std::ptr::from_mut(&mut dual_sol_sta)) };
        if 0 != returned_value { self.last_error()?; }
        let enum_primal_sol_sta_ = SolSta::from(primal_sol_sta).map_err(|i| APIError::from("err_invalid_enum_value","",format!("Invalid value ({0}) for enum SolSta",i)))?;
        let enum_dual_sol_sta_ = SolSta::from(dual_sol_sta).map_err(|i| APIError::from("err_invalid_enum_value","",format!("Invalid value ({0}) for enum SolSta",i)))?;
        Ok((enum_primal_sol_sta_,enum_dual_sol_sta_))
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `pro_sta[1]` (out) 
    pub fn get_problem_status(&self,sol_idx : i32) -> Result<ProSta,APIError>
    {
        // Arg processing order: task,sol_idx,pro_sta
        let mut pro_sta : i32 = 0;
        let returned_value = unsafe{ MSK12_get_problem_status(self.task,sol_idx,std::ptr::from_mut(&mut pro_sta)) };
        if 0 != returned_value { self.last_error()?; }
        let enum_pro_sta_ = ProSta::from(pro_sta).map_err(|i| APIError::from("err_invalid_enum_value","",format!("Invalid value ({0}) for enum ProSta",i)))?;
        Ok(enum_pro_sta_)
    }
    /// Get primal objective value for solution `sol_idx`.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `obj_val[1]` (out) 
    pub fn get_primal_obj(&self,sol_idx : i32) -> Result<f64,APIError>
    {
        // Arg processing order: task,sol_idx,obj_val
        let mut obj_val : f64 = Default::default();
        let returned_value = unsafe{ MSK12_get_primal_obj(self.task,sol_idx,std::ptr::from_mut(&mut obj_val)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(obj_val)
    }
    /// Get dual objective value for solution `sol_idx`.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `obj_val[1]` (out) 
    pub fn get_dual_obj(&self,sol_idx : i32) -> Result<f64,APIError>
    {
        // Arg processing order: task,sol_idx,obj_val
        let mut obj_val : f64 = Default::default();
        let returned_value = unsafe{ MSK12_get_dual_obj(self.task,sol_idx,std::ptr::from_mut(&mut obj_val)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(obj_val)
    }
    /// Get primal variable solution slice. The value is always available, even if the primal solution status is unknown or undefined.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index.
    /// - `first_var` First in slice.
    /// - `num_var` Number of elements in slice.
    /// - `xx[num_var]` (out) 
    pub fn get_sol_xx_slice(&self,sol_idx : i32,first_var : i32,xx : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,first_var,num_var,xx
        let num_var = i32::try_from([Some(xx.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_sol_xx_slice(self.task,sol_idx,first_var,num_var,xx.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `first_var` Variable index
    /// - `num_var` Number of variables
    /// - `slx[num_var]` (out) 
    pub fn get_sol_slx_slice(&self,sol_idx : i32,first_var : i32,slx : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,first_var,num_var,slx
        let num_var = i32::try_from([Some(slx.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_sol_slx_slice(self.task,sol_idx,first_var,num_var,slx.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `first_var` Variable index
    /// - `num_var` Number of variables
    /// - `sux[num_var]` (out) 
    pub fn get_sol_sux_slice(&self,sol_idx : i32,first_var : i32,sux : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,first_var,num_var,sux
        let num_var = i32::try_from([Some(sux.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_sol_sux_slice(self.task,sol_idx,first_var,num_var,sux.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `barvar_idx` Positive semi-definite variable index
    /// - `num_var` Number of variables
    /// - `barx[num_var]` (out) 
    pub fn get_sol_barxj(&self,sol_idx : i32,barvar_idx : i32,barx : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,barvar_idx,num_var,barx
        let num_var = i64::try_from([Some(barx.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_sol_barxj(self.task,sol_idx,barvar_idx,num_var,barx.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `barvar_idx` Positive semi-definite variable index
    /// - `num_elm` Number of positive semidefinite non-zero entries
    /// - `bars[num_elm]` (out) 
    pub fn get_sol_barsj(&self,sol_idx : i32,barvar_idx : i32,bars : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,barvar_idx,num_elm,bars
        let num_elm = i64::try_from([Some(bars.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_sol_barsj(self.task,sol_idx,barvar_idx,num_elm,bars.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get primal semidefinite variable solution slice. This get the primal value for a slice of semidefinite variables. Note that the number of elements in the slice can be obtained with `MSK120_barvar_slice_num_elm`.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index.
    /// - `first_barvar` First in slice.
    /// - `num_barvar` Number of variables in slice.
    /// - `num_elm` Number of positive semidefinite non-zero entries
    /// - `barx[num_elm]` (out) 
    pub fn get_sol_barx_slice(&self,sol_idx : i32,first_barvar : i32,num_barvar : i32,barx : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,first_barvar,num_barvar,num_elm,barx
        let num_elm = i64::try_from([Some(barx.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_sol_barx_slice(self.task,sol_idx,first_barvar,num_barvar,num_elm,barx.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `first_barvar` First semidefinite variable index in a slice
    /// - `num_barvar` Number of variables
    /// - `num_elm` Number of positive semidefinite non-zero entries
    /// - `bars[num_elm]` (out) 
    pub fn get_sol_bars_slice(&self,sol_idx : i32,first_barvar : i32,num_barvar : i32,bars : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,first_barvar,num_barvar,num_elm,bars
        let num_elm = i64::try_from([Some(bars.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_sol_bars_slice(self.task,sol_idx,first_barvar,num_barvar,num_elm,bars.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get basis indicator for a single variable.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` index of the constraint
    /// - `var_idx` index of the variable
    /// - `basic[1]` (out) 
    pub fn get_sol_basic_xj(&self,sol_idx : i32,var_idx : i32) -> Result<bool,APIError>
    {
        // Arg processing order: task,sol_idx,var_idx,basic
        let mut basic : i32 = 0;
        let returned_value = unsafe{ MSK12_get_sol_basic_xj(self.task,sol_idx,var_idx,std::ptr::from_mut(&mut basic)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(basic != 0)
    }
    /// Get basis indicator for a single semidefinite variable. At the time of writing, it is not well-defined what a basic PSD variable is, exactly.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` index of the constraint
    /// - `barvar_idx` index of the variable
    /// - `basic[1]` (out) 
    pub fn get_sol_basic_barx(&self,sol_idx : i32,barvar_idx : i32) -> Result<bool,APIError>
    {
        // Arg processing order: task,sol_idx,barvar_idx,basic
        let mut basic : i32 = 0;
        let returned_value = unsafe{ MSK12_get_sol_basic_barx(self.task,sol_idx,barvar_idx,std::ptr::from_mut(&mut basic)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(basic != 0)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `con_idx` Constraint index 
    /// - `basic[1]` (out) 
    pub fn get_sol_basic_con(&self,sol_idx : i32,con_idx : i64) -> Result<bool,APIError>
    {
        // Arg processing order: task,sol_idx,con_idx,basic
        let mut basic : i32 = 0;
        let returned_value = unsafe{ MSK12_get_sol_basic_con(self.task,sol_idx,con_idx,std::ptr::from_mut(&mut basic)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(basic != 0)
    }
    /// Get bound status indicator for a single variable. Return a value for upper and lower bounds indicating if they are binding or non-binding.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Index of the constraint
    /// - `var_idx` Index of the variable
    /// - `low_binding[1]` (out) 
    /// - `upr_binding[1]` (out) 
    pub fn get_sol_sta_var(&self,sol_idx : i32,var_idx : i32) -> Result<(bool,bool),APIError>
    {
        // Arg processing order: task,sol_idx,var_idx,low_binding,upr_binding
        let mut low_binding : i32 = 0;
        let mut upr_binding : i32 = 0;
        let returned_value = unsafe{ MSK12_get_sol_sta_var(self.task,sol_idx,var_idx,std::ptr::from_mut(&mut low_binding),std::ptr::from_mut(&mut upr_binding)) };
        if 0 != returned_value { self.last_error()?; }
        Ok((low_binding != 0,upr_binding != 0))
    }
    /// Get bound status indicator for a single semidefinite variable. *
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Index of the constraint
    /// - `barvar_idx` Index of the variable
    /// - `binding[1]` (out) 
    pub fn get_sol_sta_barx(&self,sol_idx : i32,barvar_idx : i32) -> Result<bool,APIError>
    {
        // Arg processing order: task,sol_idx,barvar_idx,binding
        let mut binding : i32 = 0;
        let returned_value = unsafe{ MSK12_get_sol_sta_barx(self.task,sol_idx,barvar_idx,std::ptr::from_mut(&mut binding)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(binding != 0)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `con_idx` Constraint index 
    /// - `binding[1]` (out) 
    pub fn get_sol_sta_con(&self,sol_idx : i32,con_idx : i64) -> Result<bool,APIError>
    {
        // Arg processing order: task,sol_idx,con_idx,binding
        let mut binding : i32 = 0;
        let returned_value = unsafe{ MSK12_get_sol_sta_con(self.task,sol_idx,con_idx,std::ptr::from_mut(&mut binding)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(binding != 0)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `first_var` Variable index
    /// - `num_var` Number of variables
    /// - `basic[num_var]` (out) 
    pub fn get_sol_basic_x_slice(&self,sol_idx : i32,first_var : i32,basic : &mut [bool]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,first_var,num_var,basic
        let num_var = i32::try_from([Some(basic.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let mut i32_basic_ : Vec<i32> = basic.iter().map(|v| if *v {1} else {0}).collect();
        let returned_value = unsafe{ MSK12_get_sol_basic_x_slice(self.task,sol_idx,first_var,num_var,i32_basic_.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        basic.iter_mut().zip(i32_basic_.iter()).for_each(|(a,b)| *a = *b != 0);
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `first_var` Variable index
    /// - `num_var` Number of variables
    /// - `basic[num_var]` (out) 
    pub fn get_sol_basic_barx_slice(&self,sol_idx : i32,first_var : i32,basic : &mut [bool]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,first_var,num_var,basic
        let num_var = i32::try_from([Some(basic.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let mut i32_basic_ : Vec<i32> = basic.iter().map(|v| if *v {1} else {0}).collect();
        let returned_value = unsafe{ MSK12_get_sol_basic_barx_slice(self.task,sol_idx,first_var,num_var,i32_basic_.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        basic.iter_mut().zip(i32_basic_.iter()).for_each(|(a,b)| *a = *b != 0);
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `first_con` First constraint index in a slice
    /// - `num_con` Number of constraints
    /// - `basic[num_con]` (out) 
    pub fn get_sol_basic_con_slice(&self,sol_idx : i32,first_con : i64,basic : &mut [bool]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,first_con,num_con,basic
        let num_con = i64::try_from([Some(basic.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let mut i32_basic_ : Vec<i32> = basic.iter().map(|v| if *v {1} else {0}).collect();
        let returned_value = unsafe{ MSK12_get_sol_basic_con_slice(self.task,sol_idx,first_con,num_con,i32_basic_.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        basic.iter_mut().zip(i32_basic_.iter()).for_each(|(a,b)| *a = *b != 0);
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `first_var` Variable index
    /// - `num_var` Number of variables
    /// - `low_binding[num_var]` (out) 
    /// - `upr_binding[num_var]` (out) 
    pub fn get_sol_sta_var_slice(&self,sol_idx : i32,first_var : i32,low_binding : &mut [bool],upr_binding : &mut [bool]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,first_var,num_var,low_binding,upr_binding
        let num_var = i32::try_from([Some(low_binding.len()),Some(upr_binding.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let mut i32_low_binding_ : Vec<i32> = low_binding.iter().map(|v| if *v {1} else {0}).collect();
        let mut i32_upr_binding_ : Vec<i32> = upr_binding.iter().map(|v| if *v {1} else {0}).collect();
        let returned_value = unsafe{ MSK12_get_sol_sta_var_slice(self.task,sol_idx,first_var,num_var,i32_low_binding_.as_mut_ptr(),i32_upr_binding_.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        low_binding.iter_mut().zip(i32_low_binding_.iter()).for_each(|(a,b)| *a = *b != 0);
        upr_binding.iter_mut().zip(i32_upr_binding_.iter()).for_each(|(a,b)| *a = *b != 0);
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `first_barvar` First semidefinite variable index in a slice
    /// - `num_barvar` Number of variables
    /// - `bindnig[num_barvar]` (out) 
    pub fn get_sol_sta_barx_slice(&self,sol_idx : i32,first_barvar : i32,bindnig : &mut [bool]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,first_barvar,num_barvar,bindnig
        let num_barvar = i32::try_from([Some(bindnig.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let mut i32_bindnig_ : Vec<i32> = bindnig.iter().map(|v| if *v {1} else {0}).collect();
        let returned_value = unsafe{ MSK12_get_sol_sta_barx_slice(self.task,sol_idx,first_barvar,num_barvar,i32_bindnig_.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        bindnig.iter_mut().zip(i32_bindnig_.iter()).for_each(|(a,b)| *a = *b != 0);
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `first_con` First constraint index in a slice
    /// - `num_con` Number of constraints
    /// - `binding[num_con]` (out) 
    pub fn get_sol_sta_con_slice(&self,sol_idx : i32,first_con : i64,binding : &mut [bool]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,first_con,num_con,binding
        let num_con = i64::try_from([Some(binding.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let mut i32_binding_ : Vec<i32> = binding.iter().map(|v| if *v {1} else {0}).collect();
        let returned_value = unsafe{ MSK12_get_sol_sta_con_slice(self.task,sol_idx,first_con,num_con,i32_binding_.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        binding.iter_mut().zip(i32_binding_.iter()).for_each(|(a,b)| *a = *b != 0);
        Ok(())
    }
    /// Get primal solution for a slice of constraints, effectively the constraint expressions evaluated in solution.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index.
    /// - `first_con` First constraint.
    /// - `num_con` Number of constraints.
    /// - `num_elm` Total number of scalar elements in constraint slice.
    /// - `xc[num_elm]` (out) Constraint value at the given solution.
    pub fn get_sol_xc_slice(&self,sol_idx : i32,first_con : i64,num_con : i64,xc : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,first_con,num_con,num_elm,xc
        let num_elm = i64::try_from([Some(xc.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_sol_xc_slice(self.task,sol_idx,first_con,num_con,num_elm,xc.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get dual solution for a slice of constraints.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index.
    /// - `first_con` First constraint.
    /// - `num_con` Number of constraints.
    /// - `num_elm` Total number of scalar elements in constraint slice.
    /// - `y[num_elm]` (out) 
    pub fn get_sol_y_slice(&self,sol_idx : i32,first_con : i64,num_con : i64,y : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,first_con,num_con,num_elm,y
        let num_elm = i64::try_from([Some(y.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_get_sol_y_slice(self.task,sol_idx,first_con,num_con,num_elm,y.as_mut_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get current number of input solutions.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    pub fn get_num_input_solutions(&self) -> i32
    {
        // Arg processing order: task
        let returned_value = unsafe{ MSK12_get_num_input_solutions(self.task) };
        returned_value
    }
    /// Copy an output solution to the input solutions.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    pub fn copy_sol_to_input(&mut self,sol_idx : i32) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx
        let returned_value = unsafe{ MSK12_copy_sol_to_input(self.task,sol_idx) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Append an empty input solution.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `soltype` 
    pub fn append_sol(&mut self,soltype : SolType) -> Result<(),APIError>
    {
        // Arg processing order: task,soltype
        let returned_value = unsafe{ MSK12_append_sol(self.task,soltype as i32) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `num` Number of items
    /// - `val[num]` (in) 
    pub fn put_sol_xx(&mut self,sol_idx : i32,val : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,num,val
        let num = i32::try_from([Some(val.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_sol_xx(self.task,sol_idx,num,val.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `num` Number of items
    /// - `val[num]` (in) 
    pub fn put_sol_slx(&mut self,sol_idx : i32,val : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,num,val
        let num = i32::try_from([Some(val.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_sol_slx(self.task,sol_idx,num,val.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `num` Number of items
    /// - `val[num]` (in) 
    pub fn put_sol_sux(&mut self,sol_idx : i32,val : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,num,val
        let num = i32::try_from([Some(val.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_sol_sux(self.task,sol_idx,num,val.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `num` Number of items
    /// - `val[num]` (in) 
    pub fn put_sol_basic_x(&mut self,sol_idx : i32,val : &[i32]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,num,val
        let num = i32::try_from([Some(val.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_sol_basic_x(self.task,sol_idx,num,val.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `num` Number of items
    /// - `val[num]` (in) 
    pub fn put_sol_barx(&mut self,sol_idx : i32,val : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,num,val
        let num = i64::try_from([Some(val.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_sol_barx(self.task,sol_idx,num,val.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `num` Number of items
    /// - `xx[num]` (in) 
    pub fn put_sol_bars(&mut self,sol_idx : i32,xx : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,num,xx
        let num = i64::try_from([Some(xx.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_sol_bars(self.task,sol_idx,num,xx.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `i` 
    /// - `num` Number of items
    /// - `xx[num]` (in) 
    pub fn put_sol_yi(&mut self,sol_idx : i32,i : i64,xx : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,i,num,xx
        let num = i64::try_from([Some(xx.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_sol_yi(self.task,sol_idx,i,num,xx.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `sol_idx` Solution index
    /// - `num` Number of items
    /// - `val[num]` (in) 
    pub fn put_sol_basic_c(&mut self,sol_idx : i32,val : &[i32]) -> Result<(),APIError>
    {
        // Arg processing order: task,sol_idx,num,val
        let num = i64::try_from([Some(val.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_sol_basic_c(self.task,sol_idx,num,val.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `par_idx` 
    /// - `value[1]` (out) 
    pub fn get_iinf(&self,par_idx : i32) -> Result<i32,APIError>
    {
        // Arg processing order: task,par_idx,value
        let mut value : i32 = Default::default();
        let returned_value = unsafe{ MSK12_get_iinf(self.task,par_idx,std::ptr::from_mut(&mut value)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(value)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `par_idx` 
    /// - `value[1]` (out) 
    pub fn get_liinf(&self,par_idx : i32) -> Result<i64,APIError>
    {
        // Arg processing order: task,par_idx,value
        let mut value : i64 = Default::default();
        let returned_value = unsafe{ MSK12_get_liinf(self.task,par_idx,std::ptr::from_mut(&mut value)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(value)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `par_idx` 
    /// - `value[1]` (out) 
    pub fn get_dinf(&self,par_idx : i32) -> Result<f64,APIError>
    {
        // Arg processing order: task,par_idx,value
        let mut value : f64 = Default::default();
        let returned_value = unsafe{ MSK12_get_dinf(self.task,par_idx,std::ptr::from_mut(&mut value)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(value)
    }
    /// Get the current value of a named parameter. 
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `par_name[.cstring]` (in) Name of the parameter
    /// - `value[1]` (out) 
    pub fn get_double_param(&self,par_name : &str) -> Result<(i32,f64),APIError>
    {
        // Arg processing order: task,par_name,value
        let cstring_par_name_ =
          CString::new(par_name)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: par_name")))?;
        let mut value : f64 = Default::default();
        let returned_value = unsafe{ MSK12_get_double_param(self.task,cstring_par_name_.as_ptr(),std::ptr::from_mut(&mut value)) };
        Ok((returned_value,value))
    }
    /// Get the index corresponding to a double parameter name.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `buflen` 
    /// - `buf[buflen]` (out) Target buffer
    pub fn get_all_double_params(&self,buf : &mut [f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,buflen,buf
        let buflen : i32 =
          unsafe { MSK12_get_num_double_param() }.try_into().map_err(|_| APIError::from("err_internal","","Internal casting error"))?;
        unsafe{ MSK12_get_all_double_params(self.task,buflen,buf.as_mut_ptr()) };
        Ok(())
    }
    /// Set all double parameters.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_par` 
    /// - `params[num_par]` (in) 
    pub fn put_all_double_params(&mut self,params : &[f64]) -> Result<(),APIError>
    {
        // Arg processing order: task,num_par,params
        let num_par = i32::try_from([Some(params.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_all_double_params(self.task,num_par,params.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get the index corresponding to a double parameter name.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `buflen` 
    /// - `buf[buflen]` (out) Target buffer
    pub fn get_all_int_params(&self,buf : &mut [i32]) -> Result<(),APIError>
    {
        // Arg processing order: task,buflen,buf
        let buflen : i32 =
          unsafe { MSK12_get_num_int_param() }.try_into().map_err(|_| APIError::from("err_internal","","Internal casting error"))?;
        unsafe{ MSK12_get_all_int_params(self.task,buflen,buf.as_mut_ptr()) };
        Ok(())
    }
    /// Set all integer parameters.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `num_par` 
    /// - `params[num_par]` (in) 
    pub fn put_all_int_params(&mut self,params : &[i32]) -> Result<(),APIError>
    {
        // Arg processing order: task,num_par,params
        let num_par = i32::try_from([Some(params.len())].iter().cloned().filter_map(|v| v).min().unwrap_or(0))
          .map_err(|_| APIError::from("err_internal","","Invalid length conversion"))?;
        let returned_value = unsafe{ MSK12_put_all_int_params(self.task,num_par,params.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Get the current value of a named parameter. 
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `par_name[.cstring]` (in) Name of the parameter
    /// - `value[1]` (out) 
    pub fn get_int_param(&self,par_name : &str) -> Result<(i32,i32),APIError>
    {
        // Arg processing order: task,par_name,value
        let cstring_par_name_ =
          CString::new(par_name)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: par_name")))?;
        let mut value : i32 = Default::default();
        let returned_value = unsafe{ MSK12_get_int_param(self.task,cstring_par_name_.as_ptr(),std::ptr::from_mut(&mut value)) };
        Ok((returned_value,value))
    }
    /// Get the length of the string representation of the current value of a named parameter. 
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `par_name[.cstring]` (in) Name of the parameter
    pub fn get_param_str_len(&self,par_name : &str) -> Result<i32,APIError>
    {
        // Arg processing order: task,par_name
        let cstring_par_name_ =
          CString::new(par_name)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: par_name")))?;
        let returned_value = unsafe{ MSK12_get_param_str_len(self.task,cstring_par_name_.as_ptr()) };
        Ok(returned_value)
    }
    /// Get the string representation of the current value of a named parameter. 
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `name[.cstring]` (in) 
    /// - `length` 
    /// - `buf[length]` (out) If the parameter is not recognized, the returned string is 0.
    pub fn get_param_str(&self,name : &str) -> Result<String,APIError>
    {
        // Arg processing order: task,name,length,buf
        let cstring_name_ =
          CString::new(name)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: name")))?;
        let length : i32 =
          (1 + unsafe { MSK12_get_param_str_len(self.task,CString::new(name).map_err(|_| APIError::from("err_invalid_string","","String cannot be converted to C string"))?.as_ptr()) }).try_into().map_err(|_| APIError::from("err_internal","","Internal casting error"))?;
        let mut buf : Vec<u8> = Vec::new();
        buf.resize(usize::try_from(length).map_err(|_|APIError::from("err_invalid_array_size","",format!("Invalid array length for argument buf")))?,0);
        unsafe{ MSK12_get_param_str(self.task,cstring_name_.as_ptr(),length,buf.as_mut_ptr() as * mut c_char) };
        let res_buf_ = 
          CStr::from_bytes_until_nul(buf.as_slice())
            .map_err(|_| APIError::from("err_string_format","","Invalid string retrieved from MOSEK Stable API"))?
            .to_string_lossy().into_owned();
        Ok(res_buf_)
    }
    /// Set the value of a named parameter.
    /// 
    /// The function will succeed if either the parameter is _not_ recognized or if the parameter is
    /// recognized and the given value is within valid bounds it. If the parameter is recognized but
    /// the value is invalid, it will fail.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `par_name[.cstring]` (in) The parameter name is the lower case name of the MOSEK parameter without "MSK_" prefix
    /// - `value` 
    /// - `ok[1]` (out) Returns true if the parameter was found and successfully set, otherwise false. 
    pub fn put_double_param(&mut self,par_name : &str,value : f64) -> Result<bool,APIError>
    {
        // Arg processing order: task,par_name,value,ok
        let cstring_par_name_ =
          CString::new(par_name)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: par_name")))?;
        let mut ok : i32 = 0;
        let returned_value = unsafe{ MSK12_put_double_param(self.task,cstring_par_name_.as_ptr(),value,std::ptr::from_mut(&mut ok)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(ok != 0)
    }
    /// Set the value of a named parameter.
    /// 
    /// The function will succeed if either the parameter is _not_ recognized or if the parameter is
    /// recognized and the given value is within valid bounds it. If the parameter is recognized but
    /// the value is invalid, it will fail.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `par_name[.cstring]` (in) The parameter name is the lower case name of the MOSEK parameter without "MSK_" prefix
    /// - `value` 
    /// - `ok[1]` (out) Returns true if the parameter was found and successfully set, otherwise false. 
    pub fn put_int_param(&mut self,par_name : &str,value : i32) -> Result<bool,APIError>
    {
        // Arg processing order: task,par_name,value,ok
        let cstring_par_name_ =
          CString::new(par_name)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: par_name")))?;
        let mut ok : i32 = 0;
        let returned_value = unsafe{ MSK12_put_int_param(self.task,cstring_par_name_.as_ptr(),value,std::ptr::from_mut(&mut ok)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(ok != 0)
    }
    /// Set the value of a named parameter as a string.
    /// 
    /// The function will succeed if either the parameter is _not_ recognized or if the parameter is
    /// recognized and the given value is within valid bounds it. If the parameter is recognized but
    /// the value is invalid, it will fail.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `par_name[.cstring]` (in) The parameter name is the lower case name of the MOSEK parameter without "MSK_" prefix
    /// - `value[.cstring]` (in) The string representation of the parameter value. For double
    ///   parameters, this is the ascii string representation of the
    ///   floating point value. For integer parameters this can be
    ///   either the ascii representation of the integer value or, for
    ///   parameters that accept symbolic values, the lower case value
    ///   name without "MSK_" prefix.
    /// - `ok[1]` (out) Returns true if the parameter was found and successfully set, otherwise false. 
    pub fn put_param_str(&mut self,par_name : &str,value : &str) -> Result<bool,APIError>
    {
        // Arg processing order: task,par_name,value,ok
        let cstring_par_name_ =
          CString::new(par_name)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: par_name")))?;
        let cstring_value_ =
          CString::new(value)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: value")))?;
        let mut ok : i32 = 0;
        let returned_value = unsafe{ MSK12_put_param_str(self.task,cstring_par_name_.as_ptr(),cstring_value_.as_ptr(),std::ptr::from_mut(&mut ok)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(ok != 0)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    pub fn get_task_name_len(&self) -> i32
    {
        // Arg processing order: task
        let returned_value = unsafe{ MSK12_get_task_name_len(self.task) };
        returned_value
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    pub fn get_obj_name_len(&self) -> i32
    {
        // Arg processing order: task
        let returned_value = unsafe{ MSK12_get_obj_name_len(self.task) };
        returned_value
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `capacity` 
    /// - `buf[capacity]` (out) Target buffer
    pub fn get_task_name(&self) -> Result<String,APIError>
    {
        // Arg processing order: task,capacity,buf
        let capacity : i32 =
          unsafe { MSK12_get_task_name_len(self.task) }.try_into().map_err(|_| APIError::from("err_internal","","Internal casting error"))?;
        let mut buf : Vec<u8> = Vec::new();
        buf.resize(usize::try_from(capacity).map_err(|_|APIError::from("err_invalid_array_size","",format!("Invalid array length for argument buf")))?,0);
        unsafe{ MSK12_get_task_name(self.task,capacity,buf.as_mut_ptr() as * mut c_char) };
        let res_buf_ = 
          CStr::from_bytes_until_nul(buf.as_slice())
            .map_err(|_| APIError::from("err_string_format","","Invalid string retrieved from MOSEK Stable API"))?
            .to_string_lossy().into_owned();
        Ok(res_buf_)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `capacity` 
    /// - `buf[capacity]` (out) Target buffer
    pub fn get_obj_name(&self) -> Result<String,APIError>
    {
        // Arg processing order: task,capacity,buf
        let capacity : i32 =
          unsafe { MSK12_get_obj_name_len(self.task) }.try_into().map_err(|_| APIError::from("err_internal","","Internal casting error"))?;
        let mut buf : Vec<u8> = Vec::new();
        buf.resize(usize::try_from(capacity).map_err(|_|APIError::from("err_invalid_array_size","",format!("Invalid array length for argument buf")))?,0);
        unsafe{ MSK12_get_obj_name(self.task,capacity,buf.as_mut_ptr() as * mut c_char) };
        let res_buf_ = 
          CStr::from_bytes_until_nul(buf.as_slice())
            .map_err(|_| APIError::from("err_string_format","","Invalid string retrieved from MOSEK Stable API"))?
            .to_string_lossy().into_owned();
        Ok(res_buf_)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `name[.cstring]` (in) 
    pub fn put_task_name(&mut self,name : &str) -> Result<(),APIError>
    {
        // Arg processing order: task,name
        let cstring_name_ =
          CString::new(name)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: name")))?;
        let returned_value = unsafe{ MSK12_put_task_name(self.task,cstring_name_.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `name[.cstring]` (in) 
    pub fn put_obj_name(&mut self,name : &str) -> Result<(),APIError>
    {
        // Arg processing order: task,name
        let cstring_name_ =
          CString::new(name)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: name")))?;
        let returned_value = unsafe{ MSK12_put_obj_name(self.task,cstring_name_.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `var_idx` Variable index
    /// - `name_len[1]` (out) 
    pub fn get_var_name_len(&self,var_idx : i32) -> Result<i32,APIError>
    {
        // Arg processing order: task,var_idx,name_len
        let mut name_len : i32 = Default::default();
        let returned_value = unsafe{ MSK12_get_var_name_len(self.task,var_idx,std::ptr::from_mut(&mut name_len)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(name_len)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `var_idx` Variable index
    pub fn get_var_name_len2(&self,var_idx : i32) -> i32
    {
        // Arg processing order: task,var_idx
        let returned_value = unsafe{ MSK12_get_var_name_len2(self.task,var_idx) };
        returned_value
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `barvar_idx` Positive semi-definite variable index
    /// - `name_len[1]` (out) 
    pub fn get_barvar_name_len(&self,barvar_idx : i32) -> Result<i32,APIError>
    {
        // Arg processing order: task,barvar_idx,name_len
        let mut name_len : i32 = Default::default();
        let returned_value = unsafe{ MSK12_get_barvar_name_len(self.task,barvar_idx,std::ptr::from_mut(&mut name_len)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(name_len)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `barvar_idx` Positive semi-definite variable index
    pub fn get_barvar_name_len2(&self,barvar_idx : i32) -> i32
    {
        // Arg processing order: task,barvar_idx
        let returned_value = unsafe{ MSK12_get_barvar_name_len2(self.task,barvar_idx) };
        returned_value
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `var_idx` Variable index
    /// - `capacity` 
    /// - `buf[capacity]` (out) Target buffer
    pub fn get_var_name(&self,var_idx : i32) -> Result<String,APIError>
    {
        // Arg processing order: task,var_idx,capacity,buf
        let capacity : i32 =
          unsafe { MSK12_get_var_name_len2(self.task,var_idx) }.try_into().map_err(|_| APIError::from("err_internal","","Internal casting error"))?;
        let mut buf : Vec<u8> = Vec::new();
        buf.resize(usize::try_from(capacity).map_err(|_|APIError::from("err_invalid_array_size","",format!("Invalid array length for argument buf")))?,0);
        let returned_value = unsafe{ MSK12_get_var_name(self.task,var_idx,capacity,buf.as_mut_ptr() as * mut c_char) };
        if 0 != returned_value { self.last_error()?; }
        let res_buf_ = 
          CStr::from_bytes_until_nul(buf.as_slice())
            .map_err(|_| APIError::from("err_string_format","","Invalid string retrieved from MOSEK Stable API"))?
            .to_string_lossy().into_owned();
        Ok(res_buf_)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `barvar_idx` Positive semi-definite variable index
    /// - `capacity` 
    /// - `buf[capacity]` (out) Target buffer
    pub fn get_barvar_name(&self,barvar_idx : i32) -> Result<String,APIError>
    {
        // Arg processing order: task,barvar_idx,capacity,buf
        let capacity : i32 =
          unsafe { MSK12_get_barvar_name_len2(self.task,barvar_idx) }.try_into().map_err(|_| APIError::from("err_internal","","Internal casting error"))?;
        let mut buf : Vec<u8> = Vec::new();
        buf.resize(usize::try_from(capacity).map_err(|_|APIError::from("err_invalid_array_size","",format!("Invalid array length for argument buf")))?,0);
        let returned_value = unsafe{ MSK12_get_barvar_name(self.task,barvar_idx,capacity,buf.as_mut_ptr() as * mut c_char) };
        if 0 != returned_value { self.last_error()?; }
        let res_buf_ = 
          CStr::from_bytes_until_nul(buf.as_slice())
            .map_err(|_| APIError::from("err_string_format","","Invalid string retrieved from MOSEK Stable API"))?
            .to_string_lossy().into_owned();
        Ok(res_buf_)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `var_idx` Variable index
    /// - `name[.cstring]` (in) 
    pub fn put_var_name(&mut self,var_idx : i32,name : &str) -> Result<(),APIError>
    {
        // Arg processing order: task,var_idx,name
        let cstring_name_ =
          CString::new(name)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: name")))?;
        let returned_value = unsafe{ MSK12_put_var_name(self.task,var_idx,cstring_name_.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `barvar_idx` Positive semi-definite variable index
    /// - `name[.cstring]` (in) 
    pub fn put_barvar_name(&mut self,barvar_idx : i32,name : &str) -> Result<(),APIError>
    {
        // Arg processing order: task,barvar_idx,name
        let cstring_name_ =
          CString::new(name)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: name")))?;
        let returned_value = unsafe{ MSK12_put_barvar_name(self.task,barvar_idx,cstring_name_.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `con_idx` Constraint index 
    /// - `len[1]` (out) 
    pub fn get_con_name_len(&self,con_idx : i64) -> Result<i32,APIError>
    {
        // Arg processing order: task,con_idx,len
        let mut len : i32 = Default::default();
        let returned_value = unsafe{ MSK12_get_con_name_len(self.task,con_idx,std::ptr::from_mut(&mut len)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(len)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `djc_idx` Disjunctive constraint index
    /// - `len[1]` (out) 
    pub fn get_djc_name_len(&self,djc_idx : i64) -> Result<i32,APIError>
    {
        // Arg processing order: task,djc_idx,len
        let mut len : i32 = Default::default();
        let returned_value = unsafe{ MSK12_get_djc_name_len(self.task,djc_idx,std::ptr::from_mut(&mut len)) };
        if 0 != returned_value { self.last_error()?; }
        Ok(len)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `con_idx` Constraint index 
    pub fn get_con_name_len2(&self,con_idx : i64) -> i32
    {
        // Arg processing order: task,con_idx
        let returned_value = unsafe{ MSK12_get_con_name_len2(self.task,con_idx) };
        returned_value
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `djc_idx` Disjunctive constraint index
    pub fn get_djc_name_len2(&self,djc_idx : i64) -> i32
    {
        // Arg processing order: task,djc_idx
        let returned_value = unsafe{ MSK12_get_djc_name_len2(self.task,djc_idx) };
        returned_value
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `con_idx` Constraint index 
    /// - `capacity` 
    /// - `buf[capacity]` (out) Target buffer
    pub fn get_con_name(&self,con_idx : i64) -> Result<String,APIError>
    {
        // Arg processing order: task,con_idx,capacity,buf
        let capacity : i32 =
          unsafe { MSK12_get_con_name_len2(self.task,con_idx) }.try_into().map_err(|_| APIError::from("err_internal","","Internal casting error"))?;
        let mut buf : Vec<u8> = Vec::new();
        buf.resize(usize::try_from(capacity).map_err(|_|APIError::from("err_invalid_array_size","",format!("Invalid array length for argument buf")))?,0);
        let returned_value = unsafe{ MSK12_get_con_name(self.task,con_idx,capacity,buf.as_mut_ptr() as * mut c_char) };
        if 0 != returned_value { self.last_error()?; }
        let res_buf_ = 
          CStr::from_bytes_until_nul(buf.as_slice())
            .map_err(|_| APIError::from("err_string_format","","Invalid string retrieved from MOSEK Stable API"))?
            .to_string_lossy().into_owned();
        Ok(res_buf_)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `djc_idx` Disjunctive constraint index
    /// - `capacity` 
    /// - `buf[capacity]` (out) Target buffer
    pub fn get_djc_name(&self,djc_idx : i64) -> Result<String,APIError>
    {
        // Arg processing order: task,djc_idx,capacity,buf
        let capacity : i32 =
          unsafe { MSK12_get_djc_name_len2(self.task,djc_idx) }.try_into().map_err(|_| APIError::from("err_internal","","Internal casting error"))?;
        let mut buf : Vec<u8> = Vec::new();
        buf.resize(usize::try_from(capacity).map_err(|_|APIError::from("err_invalid_array_size","",format!("Invalid array length for argument buf")))?,0);
        let returned_value = unsafe{ MSK12_get_djc_name(self.task,djc_idx,capacity,buf.as_mut_ptr() as * mut c_char) };
        if 0 != returned_value { self.last_error()?; }
        let res_buf_ = 
          CStr::from_bytes_until_nul(buf.as_slice())
            .map_err(|_| APIError::from("err_string_format","","Invalid string retrieved from MOSEK Stable API"))?
            .to_string_lossy().into_owned();
        Ok(res_buf_)
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `con_idx` Constraint index 
    /// - `buf[.cstring]` (in) Target buffer
    pub fn put_con_name(&mut self,con_idx : i64,buf : &str) -> Result<(),APIError>
    {
        // Arg processing order: task,con_idx,buf
        let cstring_buf_ =
          CString::new(buf)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: buf")))?;
        let returned_value = unsafe{ MSK12_put_con_name(self.task,con_idx,cstring_buf_.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `djc_idx` Disjunctive constraint index
    /// - `buf[.cstring]` (in) Target buffer
    pub fn put_djc_name(&mut self,djc_idx : i64,buf : &str) -> Result<(),APIError>
    {
        // Arg processing order: task,djc_idx,buf
        let cstring_buf_ =
          CString::new(buf)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: buf")))?;
        let returned_value = unsafe{ MSK12_put_djc_name(self.task,djc_idx,cstring_buf_.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Write task, base the format at on the file name extension.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `filename[.cstring]` (in) 
    pub fn write_task_to_file(&self,filename : &str) -> Result<(),APIError>
    {
        // Arg processing order: task,filename
        let cstring_filename_ =
          CString::new(filename)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: filename")))?;
        let returned_value = unsafe{ MSK12_write_task_to_file(self.task,cstring_filename_.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Write solution, base the format at on the file name extension.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `filename[.cstring]` (in) 
    pub fn write_solution_to_file(&self,filename : &str) -> Result<(),APIError>
    {
        // Arg processing order: task,filename
        let cstring_filename_ =
          CString::new(filename)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: filename")))?;
        let returned_value = unsafe{ MSK12_write_solution_to_file(self.task,cstring_filename_.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// Reset task and read data from file, base the format on the file extension.
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `filename[.cstring]` (in) 
    pub fn read_from_file(&mut self,filename : &str) -> Result<(),APIError>
    {
        // Arg processing order: task,filename
        let cstring_filename_ =
          CString::new(filename)
             .map_err(|_| APIError::from("err_invalid_string","",format!("Invalid string: filename")))?;
        let returned_value = unsafe{ MSK12_read_from_file(self.task,cstring_filename_.as_ptr()) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    /// - `whichstream` 
    pub fn clear_stream_callback(&mut self,whichstream : StreamType) -> Result<(),APIError>
    {
        // Arg processing order: task,whichstream
        let returned_value = unsafe{ MSK12_clear_stream_callback(self.task,whichstream as i32) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    pub fn clear_error_callback(&mut self) -> Result<(),APIError>
    {
        // Arg processing order: task
        let returned_value = unsafe{ MSK12_clear_error_callback(self.task) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }
    /// 
    /// # Arguments
    /// 
    /// - `task` The optimizatioj task object
    pub fn clear_warning_callback(&mut self) -> Result<(),APIError>
    {
        // Arg processing order: task
        let returned_value = unsafe{ MSK12_clear_warning_callback(self.task) };
        if 0 != returned_value { self.last_error()?; }
        Ok(())
    }

}

impl Drop for Task
{
    fn drop(&mut self)
    {
       unsafe { MSK12_delete_task(self.task) };
    }
}
