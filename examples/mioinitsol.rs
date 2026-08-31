//!   Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//!   File: mioinitsol.rs
//!
//!   Purpose:   To demonstrate how to solve a MIP with a start guess.
//!



use mosek_stable_api as msk;

fn mioinitsol() -> Result<(),msk::APIError> {
    let mskapi = msk::initialize()?;

    let numvar    = 4;
    let numrow    = 1;

    mskapi.task()?
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{}",msg),
            |task| {
                task.append_vars(numvar)?;
                task.put_var_bound_slice_value(0, numvar, f64::NEG_INFINITY, f64::INFINITY)?;
                task.append_rows(numrow+1)?;
                task.put_row(1, &[0,1,2,3], &[1.0,1.0,1.0,1.0])?;
                task.put_var_type_list(&[0,1,2],&[msk::VariableType::INTEGER,msk::VariableType::INTEGER,msk::VariableType::INTEGER])?;
                let dom_rminus = task.get_domain_rminus()?;
                task.append_con(dom_rminus, &[0], Some(&[2.5]))?;

                task.put_row(0,&[0,1,2,3],&[7.0,10.0,1.0,5.0])?;
                task.put_obj_row(0)?;
                task.put_obj_sense(msk::ObjSense::MAXIMIZE);


                /* Assign values to integer variables */
                task.append_sol(msk::SolType::INTEGER)?;
                task.put_sol_xx(0, &[1.0,1.0,0.0,0.0])?;

                /* Request constructing the solution from integer variable values */
                task.put_int_param("ipar_mio_construct_sol", 1)?;

                /* solve */
                let _trmcode = task.optimize()?;

                task.solution_summary(msk::StreamType::LOG)?;

                /* Read back the solution */
                let mut xx = vec![0.0; numvar as usize];
                task.get_sol_xx_slice(0, 0, &mut xx)?;

                {
                    println!("Solution: {:?}",xx);
                    /*  Was the initial guess used?     */
                    let constr = mskapi.get_iinf_index("iinf_mio_construct_solution").ok().map(|i| task.get_iinf(i)).transpose()?;
                    let constr_obj = mskapi.get_dinf_index("dinf_mio_construct_solution_obj").ok().map(|i| task.get_dinf(i)).transpose()?;

                    if let (Some(constr),Some(constr_obj)) = (constr,constr_obj) {
                        println!("Construct solution utilization: {}\nConstruct solution objective: {:.3}", constr, constr_obj);
                    }
                    else {
                        println!("Failed to obtain information of initial solution utilization");
                    }
                }
        Ok(())
    })
}

fn main() {
    mioinitsol().unwrap();
}


#[cfg(test)]
mod test {
    #[test]
    fn test() { super::main(); }
}
