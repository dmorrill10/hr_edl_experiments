# Hindsight Rationality, and Efficient Deviation Types and Learning

C++ code to run experiments. The build is managed by [CMake](https://cmake.org/).

- Run `make` to construct an optimized build in the `build.optimized` directory.
- Run `make test` to construct an optimized build in the `build.optimized` directory and run tests afterward.

Non-test executables will be generated in the `build.optimized/bin` directory.


## Dependencies

Most of the external dependencies are downloaded as necessary by `make` rules to directories in `src` (see `Makefile`). The exceptions are `bash`, `make`, `clang`, `glibc`, `cmake`, `unzip`, `git`, and `wget`. Please install these using your system installer, or use either [Singularity](https://sylabs.io/) or [Vagrant](https://www.vagrantup.com/) to avoid installing anything.

For example, on Fedora, run

```
sudo yum update -y
sudo yum install bash make clang glibc cmake unzip git wget -y
```

This is the installation procedure done in the Singularity image (see `Singularity.def`).

On Ubuntu 20.04 (Focal Fossa), run

```
apt-get update -y
apt-get install bash make clang unzip git wget singularity gdb valgrind -y
snap install cmake --classic
```

This is the installation procedure done in the Vagrant image (see `Vagrantfile`).
