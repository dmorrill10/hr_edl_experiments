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
      'leduc': 500,
      'goofspiel': 100,
      'random_goofspiel': 100,
      'tiny_bridge': 100,
      'kuhn_3p': 10000
  }

  def __init__(self, args):
    args = args.split('.')
    self.game_tag = args[0].replace('data/', '')
    self.sampler = args[1]
    self.mode = args[2]
    try:
      self.seed = int(args[3])
    except:
      self.seed = None

  def game(self):
    return self.GAME_MAP[self.game_tag]

  def num_iterations(self):
    return self.NUM_ITERATIONS_MAP[self.game_tag]

  def command(self, exe_dir):
    flags = [
        f'--game "{self.game()}"',
        f'--sampler {self.sampler}',
        f'--t {self.num_iterations()}',
    ]
    exe = exe_dir + '/' + ('run_simultaneous_ltbr'
                           if self.mode == 'sim' else 'run_fixed_ltbr')
    if self.mode == 'shuffled':
      flags.append('--shuffle')
    return f'time {exe} ' + ' '.join(flags)
