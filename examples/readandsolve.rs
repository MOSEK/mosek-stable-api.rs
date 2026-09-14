use mosek_stable_api::{self as msk, APIError};

fn read_and_solve(filenames : &[String]) -> Result<(),APIError>
{
    let mskapi = msk::initialize_with_defaults()?;
    let mut res = Vec::new();
    for (i,fname) in filenames.iter().enumerate() {
        mskapi.task()?
            .with_stream_callback(msk::StreamType::LOG,
                |msg| print!("{msg}"),
                |t| {
                    t.write_task_to_file(&format!("{fname}.jtask"))?;
                    t.read_from_file(&fname)?;
                    let trm = t.optimize()?;
                    let obj = t.get_primal_obj(0)?;
                    res.push((fname.clone(),trm,obj));
                    Ok(())
                })?;
    }

    for (fname,trm,obj) in res.iter() {
        println!("{fname} Trm = {trm:?}, Obj = {obj:.5}");
    }
    Ok(())
}

fn main() {
    let mut argit = std::env::args();
    argit.next();

    let args = argit.collect::<Vec<String>>();

    read_and_solve(&args).unwrap();
}


#[cfg(test)]
mod test {
    #[test]
    fn test() { super::read_and_solve(&[
        "data/portfolio_1.ptf".to_string(),
        "data/portfolio_2.ptf".to_string(),
        "data/portfolio_3.ptf".to_string(),
    ]).unwrap(); }
}
