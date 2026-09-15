//! Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//! File: `mico1.rs`
//!
//!
//!   Purpose :   Demonstrates how to solve a small mixed
//!               integer conic optimization problem.
//! ```
//! minimize    x^2 + y^2
//! subject to  x >= e^y + 3.8
//!             x, y - integer
//! ```
//! as

use mosek_stable_api as msk;

fn mico1() -> Result<(),msk::APIError> {
    msk::initialize_with_defaults()?;

    msk::Task::new()?
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{}",msg),
            |task| {
                let numvar = 3;  /* x, y, t */

                task.append_vars(numvar)?;
                task.put_var_name(0, "x")?;
                task.put_var_name(1, "y")?;
                task.put_var_name(2, "t")?;
                task.put_var_bound_slice_value(0,numvar,f64::NEG_INFINITY,f64::INFINITY)?;

                /* Integrality constraints */
                task.put_var_type_slice_value(0,2,msk::VariableType::INTEGER)?;

                task.append_rows(6)?;
                let roff_obj = 0;
                let roff_conic = roff_obj+1;

                /* Objective */
                task.put_obj_sense(msk::ObjSense::MINIMIZE);
                task.put_ijc(roff_obj,2,1.0)?;    /* Minimize t */
                task.put_obj_row(roff_obj)?;

                /* Conic part of the problem */
                /* Set up the affine expressions */
                /* x, x-3.8, y, t, 1.0 */
                let afeidx     = &[roff_conic, roff_conic+1, roff_conic+2, roff_conic+3];
                let varidx     = &[0, 0, 1, 2];
                let     val    = &[1.0, 1.0, 1.0, 1.0];
                let       g    = &[0.0, -3.8, 0.0, 0.0, 1.0];

                task.put_ijc_list(afeidx,
                                  varidx,
                                  val)?;
                task.put_row_slice_g(roff_conic, g)?;

                // Add constraint (x-3.8, 1, y) \in \EXP
                let dom_exp = task.get_domain_primal_exponential_cone()?;
                task.append_con(dom_exp, &[roff_conic+1,roff_conic+4,roff_conic+2], None)?;

                // Add constraint (t, x, y) \in \QUAD
                let dom_quad = task.get_domain_quadratic_cone(3)?;
                task.append_con(dom_quad, &[roff_conic+3,roff_conic+0,roff_conic+2], None)?;

                /* Optimize the problem */
                let _trm = task.optimize()?;

                task.solution_summary(msk::StreamType::MSG)?;

                let mut xx = vec![0.0,0.0];
                let solidx = 0;
                task.get_sol_xx_slice(solidx, 0, &mut xx)?;

                println!("x = {:?}", xx);

                Ok(())
            })
}

fn main() {
    mico1().unwrap();
}

#[cfg(test)]
mod test {
    #[test]
    fn test() { super::main(); }
}
