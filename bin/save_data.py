#!/usr/bin/env python

import glob
from os import path

import numpy as np
from absl import app, flags

import mal_cfr_data.experiment_parameters as xp

flags.DEFINE_list(
    "i", [], "The data files to compile together into a numpy data file. "
    "Defaults to all of them.")
flags.DEFINE_string("o", None, "The name of the output numpy data file.")
flags.mark_flag_as_required('o')


def data_entries(file_name):
  params = xp.ExperimentParameters(file_name)
  col_labels = None
  with open(file_name) as file:
    for line in file:
      if line[0] == '#':
        continue
      split_line = line.strip().split('  ')
      if col_labels is None:
        col_labels = split_line
        continue
      row_alg = split_line[0]
      for i, v in enumerate(split_line[1:]):
        yield {
            'game_tag': params.game_tag,
            'sampler': params.sampler,
            'seed': params.seed,
            'mode': params.mode,
            'row_alg': row_alg,
            'col_alg': col_labels[i + 1],
            'value': float(v)
        }


def save_data(_):
  if len(flags.FLAGS.i) < 1:
    results_files = glob.glob('data/*')
  else:
    results_files = flags.FLAGS.i

  data = sum(
      [list(data_entries(file)) for file in results_files if path.isfile(file)],
      [])
  np.save(flags.FLAGS.o, data)
  output_file = flags.FLAGS.o if flags.FLAGS.o[
      -4:] == '.npy' else flags.FLAGS.o + '.npy'
  print(f'Saved data to {output_file}')


if __name__ == '__main__':
  app.run(save_data)
