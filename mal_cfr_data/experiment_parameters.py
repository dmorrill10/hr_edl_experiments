class ExperimentParameters():
  GAME_MAP = {
      'leduc':
          'leduc_poker',
      'goofspiel':
          'goofspiel(imp_info=True,num_cards=5,points_order=descending)',
      'random_goofspiel':
          'goofspiel(imp_info=True,num_cards=4,points_order=random)',
      'tiny_bridge':
          'tiny_bridge_2p',
      'kuhn_3p':
          'kuhn_poker(players=3)',
  }
  NUM_ITERATIONS_MAP = {
      'leduc': 1000,
      'goofspiel': 200,
      'random_goofspiel': 200,
      'tiny_bridge': 200,
      'kuhn_3p': 10000,
  }

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
    alg_group_tags = self.alg_group.split('-')
    return 1 if any([tag == 'non_cfr' for tag in alg_group_tags[1:]]) else 0

  def game(self):
    return self.GAME_MAP[self.game_tag]

  def num_iterations(self):
    return self.NUM_ITERATIONS_MAP[self.game_tag]

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
