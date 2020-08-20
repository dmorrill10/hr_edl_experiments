#!/usr/bin/env python

import os
import sys
from absl import app
from absl import flags
import frcfr_data

flags.DEFINE_string("exe", None, "The experiment executable to run.")
flags.mark_flag_as_required('exe')
flags.DEFINE_string("a", None, "The experiment parameters.")
flags.mark_flag_as_required('a')


def run_experiment(argv):
    del argv  # Unused.

    x_params = frcfr_data.ExperimentParameters(flags.FLAGS.a)
    command = x_params.command(flags.FLAGS.exe)
    print(command, file=sys.stderr, flush=True)
    os.system(command)


if __name__ == '__main__':
    app.run(run_experiment)
