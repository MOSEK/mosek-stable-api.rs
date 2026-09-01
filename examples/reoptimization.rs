//!
//!   Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//!   File: `reoptimization.rs`
//!
//!   Purpose:   To demonstrate how to solve a  linear
//!              optimization problem using the MOSEK API
//!              and modify and re-optimize the problem.
//!

use std::f64;

use mosek_stable_api as msk;

fn reoptimization() -> Result<(),msk::APIError> {
    let mskapi = msk::initialize()?;

    let numvar = 3;
    let numrow = 4;

    let mut task = mskapi.task()?;
    /* Append the constraints. */
    task.append_rows(numrow)?;

    /* Append the variables. */
    task.append_vars(numvar)?;
    task.put_var_bound_slice_value(0, numvar, 0.0, f64::INFINITY)?;

    // Put objective
    let objrow = 0;
    task.put_row(objrow, &[0,1,2], &[1.5,2.5,3.0])?;
    task.put_obj_row(objrow)?;
    task.put_obj_sense(msk::ObjSense::MAXIMIZE);

    // Put constraints
    task.put_row_slice(
        1,
        &[3,3,3],
        &[0,1,2,
          0,1,2,
          0,1,2],
        &[2.0,4.0,3.0,
          3.0,2.0,3.0,
          2.0,3.0,2.0])?;

    let dom_rminus = task.get_domain_rplus()?;

    task.append_cons(
        &[dom_rminus,dom_rminus,dom_rminus],
        &[1,2,3],
        Some(&[100000.0, 50000.0, 60000.0]))?;


    {
        _ = task.optimize()?;
        let numvar = task.get_num_var();
        let mut xx = vec![0.0; numvar as usize];
        task.get_sol_xx_slice(0, 0, &mut xx)?;

        println!("1. Number of variables: {numvar}. xx = {:?}",xx);
    }






    /******************** Make a change to the A matrix **********/
    task.put_ijc(1,0,3.0)?;

    {
        _ = task.optimize()?;
        let numvar = task.get_num_var();
        let mut xx = vec![0.0; numvar as usize];
        task.get_sol_xx_slice(0, 0, &mut xx)?;

        println!("2. Number of variables: {numvar}. xx = {:?}",xx);
    }

    /*********************** Add a new variable ******************/
    /* Get index of new variable, this should be 3 */
    let varidx = task.get_num_var();
    task.append_vars(1)?;

    /* Set bounds on new variable */
    task.put_var_bound(varidx, 0.0, f64::INFINITY)?;

    /* Add coefficients for the new variable to objective and constraint rows */
    task.put_col(varidx, &[objrow,1,3], &[1.0,4.0,1.0])?;

    /* Change optimizer to free simplex and reoptimize */
    //assert!(task.put_param_str("ipar_optimizer", "optimizer_free_simplex")?);

    {
        _ = task.optimize()?;
        let numvar = task.get_num_var();
        let mut xx = vec![0.0; numvar as usize];
        task.get_sol_xx_slice(0, 0, &mut xx)?;

        println!("3. Number of variables: {numvar}. xx = {:?}",xx);
    }

    /* **************** Add a new constraint ******************* */

    /* Get index of new constraint*/
    let rowidx = task.get_num_row();

    /* Append a new constraint */
    task.append_rows(1)?;
    task.append_con(dom_rminus, &[rowidx], Some(&[30000.0]))?;
    task.put_row(rowidx, &[0,1,2,3], &[1.0, 2.0, 1.0, 1.0])?;

    {
        _ = task.optimize()?;
        let numvar = task.get_num_var();
        let mut xx = vec![0.0; numvar as usize];
        task.get_sol_xx_slice(0, 0, &mut xx)?;

        println!("4. Number of variables: {numvar}. xx = {:?}",xx);
    }


    /* **************** Change constraint bounds ******************* */

    task.put_con_slice(
        0,
        &[dom_rminus,dom_rminus,dom_rminus,dom_rminus],
        &[1,2,3,4],
        Some(&[80000.0, 40000.0, 50000.0, 22000.0 ]))?;

    {
        _ = task.optimize()?;
        let numvar = task.get_num_var();
        let mut xx = vec![0.0; numvar as usize];
        task.get_sol_xx_slice(0, 0, &mut xx)?;

        println!("5. Number of variables: {numvar}. xx = {:?}",xx);
    }

    Ok(())
} /* main */


fn main() {
    reoptimization().unwrap();
}

#[cfg(test)]
mod test {
    #[test]
    fn test() { super::main(); }
}
