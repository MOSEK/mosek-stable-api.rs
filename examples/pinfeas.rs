//!  Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//!  File: `pinfeas.rs`
//!
//!  Purpose: Demonstrates how to fetch a primal infeasibility certificate
//!           for a linear problem
//!
use itertools::izip;
use mosek_stable_api::{self as msk, APIError};

//TAG:begin-pinfeas
fn pinfeas() ->Result<(),APIError>
{

    // In this example we set up a simple problem
    // One could use any task or a task read from a file

    test_problem()?
        // Directs the log task stream to the 'printstr' function.
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{0}",msg),
            |task| {
                // Useful for debugging
                task.write_task_to_file("pinfeas.ptf")?; // Write file in human-readable format

                // Perform the optimization.
                _ = task.optimize()?;

                task.solution_summary(msk::StreamType::LOG)?;

                let solidx = 0;
                //TAG:begin-check-status
                // Check problem status, we use the basic solution (BAS)
                match task.get_problem_status(solidx)? {
                    msk::ProSta::PRIMAL_AND_DUAL_INFEASIBLE => {
                        // Set the tolerance at which we consider a dual value as essential
                        let eps = 1e-7;
                        //TAG:end-check-status

                        let m = task.get_num_var() as usize;
                        let n = task.get_num_con() as usize;

                        let mut slx = vec![0.0;m];
                        let mut sux = vec![0.0;m];
                        let mut y   = vec![0.0;n];

                        task.get_sol_slx_slice(solidx,0,&mut slx)?;
                        task.get_sol_sux_slice(solidx,0,&mut sux)?;

                        analyze_certificate(&mut slx, &mut sux, &mut y, eps);
                    },
                    _ => {
                        println!("The problem is not primal infeasible, no certificate to show.");
                    }
                }
                Ok(())
            })
}
//TAG:end-pinfeas

//TAG:begin-example-def
// Set up a simple linear problem from the manual for test purposes
const PTF_DATA : &[u8] = b"Task ''
Objective ''
  Minimize + @x0 + 2 @x1 + 5 @x2 + 2 @x3 + @x4 + 2 @x5 + @x6
Constraints
  @c0 [-inf;200] + @x0 + @x1
  @c1 [-inf;1000] + @x2 + @x3
  @c2 [-inf;1000] + @x4 + @x5 + @x6
  @c3 [1100] + @x0 + @x4
  @c4 [200] + @x1
  @c5 [500] + @x2 + @x5
  @c6 [500] + @x3 + @x6
Variables
  @x0 [0;+inf]
  @x1 [0;+inf]
  @x2 [0;+inf]
  @x3 [0;+inf]
  @x4 [0;+inf]
  @x5 [0;+inf]
  @x6 [0;+inf]
  ";

fn test_problem() -> Result<msk::Task,APIError>
{
    msk::initialize_with_defaults()?;
    let mut task = msk::Task::new()?;
    let mut pos = 0;
    task.read_task_by_func(
        |buf| {
            let n = buf.len().min(PTF_DATA.len()-pos);
            buf.copy_from_slice(&PTF_DATA[pos..pos+n]);
            pos += n;
            n
        },
        msk::Format::PTF,
        msk::Compression::NONE)?;
    Ok(task)
}
//TAG:end-example-def

//TAG:begin-analyze-certificate
// Analyzes and prints infeasibility contributing elements
// n - length of arrays
// sl - dual values for lower bounds
// su - dual values for upper bounds
// eps - tolerance for when a nunzero dual value is significant
fn analyze_certificate(slx : &[f64], sux : &[f64], y : &[f64], eps : f64) {

    println!("Variable bounds important for infeasibility:");
    for (i,sl,su) in izip!(0..,slx.iter(),sux.iter()) {
        if sl.abs() > eps { println!("#{}, lower, dual = {:e}",i,sl) }
        if su.abs() > eps { println!("#{}, upper, dual = {:e}",i,su) }
    }
    println!("Constraint bounds important for infeasibility:");
    for (i,y) in izip!(0..,y.iter()) {
        if y.abs() > eps { println!("#{}, dual = {:e}",i,y) }
    }
}
//TAG:end-analyze-certificate


fn main() {
    pinfeas().unwrap();
}
