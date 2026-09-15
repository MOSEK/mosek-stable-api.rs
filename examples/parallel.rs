//!
//! Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//! File: `parallel.rs`
//!
//!   Purpose: Demonstrates parallel optimization usint optimizebatch()

use itertools::izip;
use mosek_stable_api as msk;

///Example of how to use `optimizebatch()`.
///    Optimizes tasks whose names were read from command line.
fn parallel(filenames : &[&str]) -> Result<(),msk::APIError>
{
    msk::initialize_with_defaults()?;

    /* Create an example list of tasks to optimize */
    let mut tasks : Vec<msk::Task> =
        filenames.iter()
            .filter_map(|&fname| {
                let mut t : msk::Task = msk::Task::new().ok()?;
                t.read_from_file(fname).ok()?;
                /* We can set the number of threads for each task */
                t.put_int_param("ipar_num_threads",2).ok()?;
                Some(t)
            }).collect();
    let mut trm_code = vec![-1 as msk::TrmCode; tasks.len()];
    let mut res_code = vec![-1 as msk::ResCode; tasks.len()];

    /* Size of thread pool available for all tasks */
    let threadpoolsize = 6;

    /* Optimize all the given tasks in parallel */
    msk::optimize_batch(
        false, // not a race
        -1.0, // max_time_sec
        threadpoolsize, // num_threads
        &tasks.iter_mut().collect::<Vec<&mut msk::Task>>(),
        &mut trm_code,
        &mut res_code)?;

    for (i,trm,res,task) in izip!(0..,trm_code.iter(),res_code.iter(),tasks.iter()) {
        let obj2 = task.get_primal_obj(0)?;
        let obj  = task.get_dinf(msk::get_dinf_index("dinf_intpnt_primal_obj" )?)?;
        let tm   = task.get_dinf(msk::get_dinf_index("dinf_optimizer_time")?)?;

        println!("Task  {i}  res: {res}  trm: {trm}   obj_val: {obj:.5}/{obj2:.5}  time: {tm:.5}");
    }

    Ok(())
}

fn main() {
    let mut argi = std::env::args();
    argi.next();
    let args : Vec<String> = argi.collect();
    if args.len() >= 2 {
        parallel(&args.iter().map(|s| s.as_str()).collect::<Vec<&str>>().as_slice()).unwrap();
    }
    else {
        println!("Requires at least two filenames");
    }
}



#[cfg(test)]
mod test {
    #[test]
    fn test() {
       super::parallel(
           &["data/portfolio_1.ptf",
             "data/portfolio_2.ptf",
             "data/portfolio_3.ptf",
             "data/portfolio_4.ptf",
             "data/portfolio_5.ptf",
             "data/portfolio_6.ptf"]).unwrap();
    }
}
