#!/usr/bin/env python

from os import path
import glob

from absl import app
from absl import flags
import numpy as np

flags.DEFINE_list(
    "i", [],
    "The data files to compile together into a numpy data file. "
    "Defaults to all of them."
)
flags.DEFINE_string("o", None, "The name of the output numpy data file.")
flags.mark_flag_as_required('o')


ALG_NAME_MAP = {
    'rm': 'RM',
    'hedge_s': 'S-Hedge',
    'hedge_m': 'M-Hedge',
    'hedge_h': 'H-Hedge',
    'ada_normal_hedge': 'AdaNormalHedge',
    'elm_mu': 'MU-ELM',
    'simple_ada_normal_hedge': 'SimpleANH',
    'max_simple_ada_normal_hedge': 'MaxSANH',
    'hedge_k': 'K-Hedge',
}

GAME_NAME_MAP = {
    'leduc': 'Leduc Poker',
    'liars_dice': "Liar's Dice",
    'goofspiel3': 'Goofspiel(3)',
    'goofspiel4': 'Goofspiel(4)'
}

SAMPLER_NAME_MAP = {
    'null': 'No',
    'external': "External",
    'outcome': 'Outcome(0.6)'
}


def read_data_file(file_name):
    file_name_components = file_name.split('.')
    try:
        seed = int(file_name_components[3])
    except:
        seed = None

    final_data = {
        'game':
        file_name_components[0],
        'sampler':
        file_name_components[1],
        '_alg':
        file_name_components[2],
        '_seed': seed,
    }

    num_iterations_completed = []
    exploitability_cur = []
    exploitability_avg = []
    nodes_touched = []
    with open(file_name) as file:
        for line in file:
            if line[0] == '#': continue
            t, data = line.split(':')
            num_iterations_completed.append(int(t))

            data_components = [datum.strip() for datum in data.split(',')]
            exploitability_cur.append(float(data_components[0]))
            exploitability_avg.append(float(data_components[1]))

            if len(data_components) > 2:
                nodes_touched.append(int(data_components[2]))

    final_data['num_iterations_completed'] = np.array(num_iterations_completed)
    final_data['exploitability_cur'] = np.array(exploitability_cur) * 1000
    final_data['exploitability_avg'] = np.array(exploitability_avg) * 1000
    final_data['nodes_touched'] = np.array(nodes_touched)
    final_data['label'] = ALG_NAME_MAP[final_data['_alg']]
    final_data['is_one_of_many_reps'] = final_data['_seed'] is not None
    return final_data


def save_data(argv):
    del argv  # Unused.

    if len(flags.FLAGS.i) < 1:
        results_files = glob.glob('*.gen.txt')
    else:
        results_files = flags.FLAGS.i

    data = sorted(
        [read_data_file(file) for file in results_files if path.isfile(file)],
        key=lambda results: results['label'])

    np.save(flags.FLAGS.o, data)
    print('Saved data to {}'.format(flags.FLAGS.o))


if __name__ == '__main__':
    app.run(save_data)
