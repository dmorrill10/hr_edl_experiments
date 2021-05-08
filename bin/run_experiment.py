#!/usr/bin/env python

import os
import sys

from absl import app, flags

import hr_edl_data.experiment_parameters as xp

flags.DEFINE_string("exe_dir", None, "The experiment executable directory.")
flags.mark_flag_as_required('exe_dir')
flags.DEFINE_string("a", None, "The experiment parameters.")
flags.mark_flag_as_required('a')


def run_experiment(_):
  x_params = xp.ExperimentParameters(flags.FLAGS.a)
  command = x_params.command(flags.FLAGS.exe_dir)
  print(command, file=sys.stderr, flush=True)
  os.system(command)


if __name__ == '__main__':
  app.run(run_experiment)
