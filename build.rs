
fn main() {
    cc::Build::new()
        .file("src/mosekcoredynamic12_0.c")
        .compile("mosekcoredynamic12_0");
    println!("cargo::rerun-if-changed=src/mosekcoredynamic12_0.c");

}
