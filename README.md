# Hindsight Rationality, and Efficient Deviation Types and Learning Experiments

Experiment code for [Hindsight and Sequential Rationality of Correlated Play](https://arxiv.org/abs/2012.05874) and [Efficient Deviation Types and Learning for Hindsight Rationality in Extensive-Form Games](https://arxiv.org/abs/2102.06973) conference papers (AAAI-21 and ICML 2021, respectively).

This repository has a number of different components that work together to generate the experimental results.

The pipeline begins with `hr_edl`. This is C++ code built over [OpenSpiel](https://github.com/deepmind/open_spiel) that defines the experiments. See [`hr_edl/README.md`](hr_edl/README.md) for more information.

The `hr_edl` code allows us to run experiments, but experiments are run with help from the Python3 library, `hr_edl_data`. Run `pip install .` to install it.
With `hr_edl_data` installed, you can run `bin/run_experiment.py` to run an experiment.

The experiment configurations used in the papers are defined in `Makefile`. Assuming that `hr_edl_data` is installed, running `make` should compile `hr_edl` and run all experiments, updating data files in `data` as necessary, and depositing Numpy data files in `results`.

Finally, the Python Jupyter notebooks in `notebooks` process the Numpy data files into the final results, which are also saved in `results`.

## Virtual Machines and Containers

A [Vagrant](https://www.vagrantup.com/) virtual machine configuration to help run these experiments is defined in `Vagrantfile`.
If you already have Python3 installed though, you may not need to use it.
Typically, the most onerous part of the installation procedure is building `hr_edl` and its dependencies.
For this, you can use the [Singularity](https://sylabs.io/) container defined by `hr_edl/Singularity.def`.
Once you have Singularity installed or you are using the Vagrant virtual machine, you can run

```sudo singularity build ./hr_edl.sif Singularity.def```

to create a Singularity image. Then you can run

```singularity exec hr_edl.sif /code/build.optimized/bin/<command> [-- command flags]```

to run an `hr_edl` executable inside the container.
`bin/run_experiment.py` has a `--sif` option so you can specify a container image in which the experiment should be run.
You can set the variable `SIF` in `Makefile` (either in the file or in the command like `make SIF=my_image.sif`) to run all experiments in a given container image.
