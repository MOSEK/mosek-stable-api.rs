
fn main() {
    cc::Build::new()
        .file("src/mosekstabledynamic12.c")
        .compile("mosekstabledynamic12");
    println!("cargo::rerun-if-changed=src/mosekstabledynamic12.c");

}
