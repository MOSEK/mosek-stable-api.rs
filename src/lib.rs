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

mod api;
pub use api::*;

use std::path::{Path, PathBuf};
use std::env;


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

impl APIError {
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

impl From<APIError> for String {
    fn from(value : APIError) -> Self {
        format!("{:?}",value)
    }
}


/// Load MOSEK library, searching for library in default paths, depending on the platform.
///
/// Search in this order:
/// - osx, linux: `$HOME/mosek`
/// - osx: `$HOME/Applications/mosek`
/// - linux, osx: `$HOME/.local/mosek`
/// - win: `$USERDRIVE/$USERPATH/mosek`
/// - win: `$USERPROFILE/mosek`
/// - win: `$LOCALAPPDATA/mosek`
pub fn initialize_with_defaults() -> Result<MosekStableAPI,APIError>
{
    initialize()
        .or_else(|_| {
            let pfname =
                match (std::env::consts::OS,std::env::consts::ARCH) {
                    ("linux",  "x86_64") => "linux64x86",
                    ("linux",  "arm") => "linuxaarch64",
                    ("osx",    "arm") => "osxaarch64",
                    ("windows","x86_64") => "win64x86",
                    _ => return Err(APIError::from("err_incompatible_platform",
                                                  "Unsupported platform OS and/or architecture",
                                                  "Unsupported platform OS and/or architecture"))
                };

            let mut basepaths = Vec::new();
            match env::consts::OS {
                "linux"|"osx" =>
                    if let Some(homep) = env::var("HOME").ok() {
                        let p = Path::new(&homep).join("mosek");
                        if p.exists() { if let Some(p) = p.to_str() { basepaths.push(p.to_string()) } }
                        if env::consts::OS == "osx" {
                            let p = Path::new(&homep).join("Applications").join("mosek");
                            if p.exists() { if let Some(p) = p.to_str() { basepaths.push(p.to_string()) } }
                        }
                        let p = Path::new(&homep).join(".local").join("mosek");
                        if p.exists() { if let Some(p) = p.to_str() { basepaths.push(p.to_string()) } }
                    },
                "windows" => {
                    if let Some(p) = env::var("LOCALAPPDATA").ok() {
                        let p = Path::new(&p).join("mosek");
                        if p.exists() { if let Some(p) = p.to_str() { basepaths.push(p.to_string()); } }
                    }

                    if let Some(p) = env::var("USERPROFILE").ok() {
                        let p = Path::new(&p).join("mosek");
                        if p.exists() { if let Some(p) = p.to_str() { basepaths.push(p.to_string()); } }
                    }

                    if let Some(p) = env::var("USERDRIVE").ok().and_then(|drive| env::var("USERPATH").ok().map(|p| format!("{drive}{p}"))) {
                        let p = Path::new(&p).join("mosek");
                        if p.exists() { if let Some(p) = p.to_str() { basepaths.push(p.to_string()); } }
                    }
                },
                _ => {}
            }

            let spaths : Vec<PathBuf> =
                basepaths.iter().filter_map(|p| Path::new(p).read_dir().ok())
                    .flat_map(|entry| entry.filter_map(|e| e.ok()).map(|entry| entry.path().join("tools").join(pfname).join("bin")))
                    .collect();

            let paths : Vec<&str> = spaths.iter().filter_map(|p| if p.exists() { p.to_str() } else { None }).collect();

            println!("Search in paths: {:?}",paths);
            if paths.is_empty() {
                initialize()
            }
            else {
                initialize_with_paths(&paths)
            }
        })
}
