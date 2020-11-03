GAME_MAP = {
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
    'crewmates_3p':
        'crewmates(rounds=6,ranks=4,stack=2,players=3,growth=0.1,last_plunder_reward=2)',
    'crewmates_3p_5h':
        'crewmates(rounds=6,ranks=5,stack=2,players=3,growth=0.1,last_plunder_reward=2)',
    'crewmates_3p_10r':
        'crewmates(rounds=10,ranks=4,stack=2,players=3,growth=0.1,last_plunder_reward=2)',
    'crewmates_3p_10r_10g':
        'crewmates(rounds=10,ranks=4,stack=2,players=3,growth=0.1,last_plunder_reward=2)',
    'crewmates_3p_10r_20g':
        'crewmates(rounds=10,ranks=4,stack=2,players=3,growth=0.2,last_plunder_reward=2)',
    'crewmates_3p_10r_30g':
        'crewmates(rounds=10,ranks=4,stack=2,players=3,growth=0.3,last_plunder_reward=2)',
    'crewmates_3p_10r_5h_10g':
        'crewmates(rounds=10,ranks=5,stack=2,players=3,growth=0.1,last_plunder_reward=2)',
    'crewmates_3p_10r_5h_20g':
        'crewmates(rounds=10,ranks=5,stack=2,players=3,growth=0.2,last_plunder_reward=2)',
    'crewmates_3p_10r_5h_30g':
        'crewmates(rounds=10,ranks=5,stack=2,players=3,growth=0.3,last_plunder_reward=2)',
    'crewmates_3p_10r_1l':
        'crewmates(rounds=10,ranks=4,stack=2,players=3,growth=0.1,last_plunder_reward=1)',
    'crewmates_3p_10r_10g_1l':
        'crewmates(rounds=10,ranks=4,stack=2,players=3,growth=0.1,last_plunder_reward=1)',
    'crewmates_3p_10r_20g_1l':
        'crewmates(rounds=10,ranks=4,stack=2,players=3,growth=0.2,last_plunder_reward=1)',
    'crewmates_3p_10r_30g_1l':
        'crewmates(rounds=10,ranks=4,stack=2,players=3,growth=0.3,last_plunder_reward=1)',
    'crewmates_3p_10r_5h_10g_1l':
        'crewmates(rounds=10,ranks=5,stack=2,players=3,growth=0.1,last_plunder_reward=1)',
    'crewmates_3p_10r_5h_20g_1l':
        'crewmates(rounds=10,ranks=5,stack=2,players=3,growth=0.2,last_plunder_reward=1)',
    'crewmates_3p_10r_5h_30g_1l':
        'crewmates(rounds=10,ranks=5,stack=2,players=3,growth=0.3,last_plunder_reward=1)',
}
NUM_ITERATIONS_MAP = {
    'leduc': 400,
    'goofspiel': 400,
    'random_goofspiel': 400,
    'goofspiel_3p': 400,
    'goofspiel_ascending': 400,
    'goofspiel_ascending_3p': 400,
    'tiny_bridge': 400,
    'kuhn_3p': 400,
    'kuhn_4p': 400,
    'tiny_hanabi': 400,
    'crewmates_3p': 400,
    'crewmates_3p_5h': 400,
    'crewmates_3p_10r': 400,
    'crewmates_3p_10r_10g': 400,
    'crewmates_3p_10r_20g': 400,
    'crewmates_3p_10r_30g': 400,
    'crewmates_3p_10r_5h_10g': 400,
    'crewmates_3p_10r_5h_20g': 400,
    'crewmates_3p_10r_5h_30g': 400,
    'crewmates_3p_10r_1l': 400,
    'crewmates_3p_10r_10g_1l': 400,
    'crewmates_3p_10r_20g_1l': 400,
    'crewmates_3p_10r_30g_1l': 400,
    'crewmates_3p_10r_5h_10g_1l': 400,
    'crewmates_3p_10r_5h_20g_1l': 400,
    'crewmates_3p_10r_5h_30g_1l': 400,
}
NUM_PLAYERS_MAP = {
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
    'crewmates_3p': 3,
    'crewmates_3p_5h': 3,
    'crewmates_3p_10r': 3,
    'crewmates_3p_10r_10g': 3,
    'crewmates_3p_10r_20g': 3,
    'crewmates_3p_10r_30g': 3,
    'crewmates_3p_10r_5h_10g': 3,
    'crewmates_3p_10r_5h_20g': 3,
    'crewmates_3p_10r_5h_30g': 3,
    'crewmates_3p_10r_1l': 3,
    'crewmates_3p_10r_10g_1l': 3,
    'crewmates_3p_10r_20g_1l': 3,
    'crewmates_3p_10r_30g_1l': 3,
    'crewmates_3p_10r_5h_10g_1l': 3,
    'crewmates_3p_10r_5h_20g_1l': 3,
    'crewmates_3p_10r_5h_30g_1l': 3,
}
EXTRA_GAME_PARAMS_MAP = {
    'goofspiel': ('5', r'\downarrow'),
    'goofspiel_ascending': ('5', r'\uparrow'),
    'random_goofspiel': ('4', 'R'),
    'goofspiel_3p': ('4', r'\downarrow'),
    'goofspiel_ascending_3p': ('4', r'\uparrow'),
    'crewmates_3p': ('R=6', 'H=4', 'S=2', 'G=0.1', 'L=2'),
    'crewmates_3p_5h': ('R=6', 'H=5', 'S=2', 'G=0.1', 'L=2'),
    'crewmates_3p_10r': ('R=10', 'H=4', 'S=2', 'G=0.1', 'L=2'),
    'crewmates_3p_10r_10g': ('R=10', 'H=4', 'S=2', 'G=0.10', 'L=2'),
    'crewmates_3p_10r_20g': ('R=10', 'H=4', 'S=2', 'G=0.20', 'L=2'),
    'crewmates_3p_10r_30g': ('R=10', 'H=4', 'S=2', 'G=0.30', 'L=2'),
    'crewmates_3p_10r_5h_10g': ('R=10', 'H=5', 'S=2', 'G=0.10', 'L=2'),
    'crewmates_3p_10r_5h_20g': ('R=10', 'H=5', 'S=2', 'G=0.20', 'L=2'),
    'crewmates_3p_10r_5h_30g': ('R=10', 'H=5', 'S=2', 'G=0.30', 'L=2'),
    'crewmates_3p_10r_1l': ('R=10', 'H=4', 'S=2', 'G=0.1', 'L=1'),
    'crewmates_3p_10r_10g_1l': ('R=10', 'H=4', 'S=2', 'G=0.10', 'L=1'),
    'crewmates_3p_10r_20g_1l': ('R=10', 'H=4', 'S=2', 'G=0.20', 'L=1'),
    'crewmates_3p_10r_30g_1l': ('R=10', 'H=4', 'S=2', 'G=0.30', 'L=1'),
    'crewmates_3p_10r_5h_10g_1l': ('R=10', 'H=5', 'S=2', 'G=0.10', 'L=1'),
    'crewmates_3p_10r_5h_20g_1l': ('R=10', 'H=5', 'S=2', 'G=0.20', 'L=1'),
    'crewmates_3p_10r_5h_30g_1l': ('R=10', 'H=5', 'S=2', 'G=0.30', 'L=1'),
}


class ExperimentParameters():

  def __init__(self, args):
    args = args.split('.')
    self.alg_group = args[0].replace('data/', '')
    self.game_tag = args[1]
    self.sampler = args[2]
    self.mode = args[3]
    try:
      self.seed = int(args[4])
    except:
      self.seed = None

  def cfr_group(self):
    alg_group_tags = self.alg_group.split('-')
    if alg_group_tags[0] == 'ltbr':
      v = 1
    else:
      v = 2
    return v if any([tag == 'cfr' for tag in alg_group_tags[1:]]) else 0

  def cfr_plus_group(self):
    alg_group_tags = self.alg_group.split('-')
    if alg_group_tags[0] == 'ltbr':
      v = 1
    else:
      v = 2
    return v if any([tag == 'cfr_plus' for tag in alg_group_tags[1:]]) else 0

  def non_cfr_group(self):
    return 0

  def game(self):
    return GAME_MAP[self.game_tag]

  def num_iterations(self):
    return NUM_ITERATIONS_MAP[self.game_tag]

  def command(self, exe_dir):
    flags = [
        f'--game "{self.game()}"',
        f'--sampler {self.sampler}',
        f'--t {self.num_iterations()}',
        f'--cfr_group {self.cfr_group()}',
        f'--non_cfr_group {self.non_cfr_group()}',
    ]
    exe = exe_dir + '/' + ('run_simultaneous_ltbr'
                           if self.mode == 'sim' else 'run_fixed_ltbr')
    if self.mode == 'shuffled':
      flags.append('--shuffle')
      if self.seed is not None:
        flags.append(f'--random_seed {self.seed}')
    return f'time {exe} ' + ' '.join(flags)
