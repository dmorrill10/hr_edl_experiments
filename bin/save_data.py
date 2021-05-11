#!/usr/bin/env python

import glob
from os import path

import numpy as np
from absl import app, flags

import hr_edl_data.experiment_parameters as xp

flags.DEFINE_list(
    "i", [], "The data files to compile together into a numpy data file. "
    "Defaults to all of them.")
flags.DEFINE_string("o", None, "The name of the output numpy data file.")
flags.mark_flag_as_required('o')

EXPERIMENT_TYPES = set(['efr', 'cor_gap'])
flags.DEFINE_string(
    "x", None, f"The type of experiment data file, one of {EXPERIMENT_TYPES}.")
flags.mark_flag_as_required('x')


def tournament_data_entries(file_name):
  params = xp.ExperimentParameters(file_name)
  col_labels = None
  with open(file_name) as file:
    for line in file:
      if line[0] == '#':
        continue
      line = line.strip()
      if col_labels is None:
        split_line = line.split('  ')
        col_labels = split_line
        continue
      if line[:4] == 't = ':
        t = int(line[4:])
        continue
      split_line = line.split('  ')
      row_alg = split_line[0]
      for i, v in enumerate(split_line[1:]):
        # v is "(expected_value, milliseconds)"
        ev, milliseconds = v.split(', ')
        ev = float(ev[1:])
        milliseconds = float(milliseconds[:-1])
        yield {
            'game_tag': params.game_tag,
            'sampler': 'null',
            'seed': None,
            'mode': params.mode,
            'row_alg': row_alg,
            'col_alg': col_labels[i + 1],
            'value': ev,
            't': t,
            'milliseconds': milliseconds,
        }


def cor_gap_data_entries(file_name):
  '''
  cor_gap.<game>.<param>.<training_regime>.dat
  Contents:
  CFRTest <iterations> <AFCCE gap> <AFCE gap> <EFCCE gap> <EFCE gap> <exp value for first player> <exp value for second player>
  '''
  file_comp = file_name.split('.')
  with open(file_name) as file:
    for line in file:
      if line[0] == '#':
        continue
      split_line = [s.strip() for s in line.strip().split(' ')]
      yield {
          'game_tag': file_comp[1],
          'param': file_comp[2],
          'training_regime': file_comp[3],
          'num_iterations': int(split_line[1]),
          'afcce_gap': float(split_line[2]),
          'afce_gap': float(split_line[3]),
          'efcce_gap': float(split_line[4]),
          'efce_gap': float(split_line[5]),
          'value_1': float(split_line[6]),
          'value_2': float(split_line[7]),
      }


def save_data(_):
  if flags.FLAGS.x == 'cor_gap':
    pattern = 'data/cor_gap.*.dat'
    data_processor = cor_gap_data_entries
  else:
    pattern = 'data/*.ssv'
    data_processor = tournament_data_entries

  if len(flags.FLAGS.i) < 1:
    results_files = glob.glob(pattern)
  else:
    results_files = flags.FLAGS.i

  data = sum([
      list(data_processor(file)) for file in results_files if path.isfile(file)
  ], [])
  np.save(flags.FLAGS.o, data)
  output_file = flags.FLAGS.o if flags.FLAGS.o[
      -4:] == '.npy' else flags.FLAGS.o + '.npy'
  print(f'Saved data to {output_file}')


if __name__ == '__main__':
  app.run(save_data)
