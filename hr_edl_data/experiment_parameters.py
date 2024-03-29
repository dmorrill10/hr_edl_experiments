GAME_MAP = {
    'sheriff':
        'sheriff',
    'leduc':
        'leduc_poker',
    'goofspiel':
        'goofspiel(imp_info=True,num_cards=5,points_order=descending)',
    'goofspiel_ascending':
        'goofspiel(imp_info=True,num_cards=5,points_order=ascending)',
    'random_goofspiel':
        'goofspiel(imp_info=True,num_cards=4,points_order=random)',
    'goofspiel_3p':
        'goofspiel(imp_info=True,num_cards=4,points_order=descending,players=3)',
    'goofspiel_ascending_3p':
        'goofspiel(imp_info=True,num_cards=4,points_order=ascending,players=3)',
    'tiny_bridge':
        'tiny_bridge_2p',
    'kuhn_3p':
        'kuhn_poker(players=3)',
    'kuhn_4p':
        'kuhn_poker(players=4)',
    'tiny_hanabi':
        'tiny_hanabi',
}
NUM_ITERATIONS_MAP = {
    'sheriff': 1000,
    'leduc': 1000,
    'goofspiel': 1000,
    'random_goofspiel': 1000,
    'goofspiel_3p': 1000,
    'goofspiel_ascending': 1000,
    'goofspiel_ascending_3p': 1000,
    'tiny_bridge': 1000,
    'kuhn_3p': 1000,
    'kuhn_4p': 1000,
    'tiny_hanabi': 1000,
}
NUM_PLAYERS_MAP = {
    'sheriff': 2,
    'leduc': 2,
    'goofspiel': 2,
    'goofspiel_ascending': 2,
    'random_goofspiel': 2,
    'goofspiel_3p': 3,
    'goofspiel_ascending_3p': 3,
    'tiny_bridge': 2,
    'kuhn_3p': 3,
    'kuhn_4p': 4,
    'tiny_hanabi': 2,
}
EXTRA_GAME_PARAMS_MAP = {
    'goofspiel': ('5', r'\downarrow'),
    'goofspiel_ascending': ('5', r'\uparrow'),
    'random_goofspiel': ('4', 'R'),
    'goofspiel_3p': ('4', r'\downarrow'),
    'goofspiel_ascending_3p': ('4', r'\uparrow'),
}


class ExperimentParameters():

  def __init__(self, args):
    args = args.split('.')
    self.alg_group = args[0].replace('data/', '')
    self.game_tag = args[1]
    self.mode = args[3]

  def game(self):
    return GAME_MAP[self.game_tag]

  def num_iterations(self):
    return NUM_ITERATIONS_MAP[self.game_tag]

  def command(self, exe_dir, sif):
    flags = [
        f'--game "{self.game()}"',
        f'--t {self.num_iterations()}',
        f'--alg_group 1'
    ]
    executable_name = 'run_simultaneous_ltbr' if self.mode == 'sim' else 'run_fixed_ltbr'
    exe = exe_dir + '/' + executable_name
    if sif:
        exe = f'singularity exec {sif} {exe}'
    return f'time {exe} ' + ' '.join(flags)
