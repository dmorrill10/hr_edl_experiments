class ExperimentParameters():
    GAME_MAP = {
        'leduc':
        'leduc_poker',
        'goofspiel':
        'goofspiel(imp_info=True,num_cards=5,points_order=descending)',
        'random_goofspiel':
        'goofspiel(imp_info=True,num_cards=4,points_order=random)',
    }
    NUM_ITERATIONS_MAP = {
        'leduc': 100000,
        'goofspiel': 100000,
        'random_goofspiel': 100000,
    }
    REPORT_GAP_FACTOR_MAP = {
        'leduc': 2,
        'goofspiel': 2,
        'random_goofspiel': 2,
    }
    ALG_MAP = {
        'rm': 'LsRcfrRm',
        'hedge': 'LsRcfrHedge',
        'poly': 'LsRcfrPoly',
    }

    def __init__(self, args):
        args = args.split('.')
        self.game = args[0].replace('data/', '')
        alg_and_args = args[2]

        if len(args) > 3:
            self.seed = args[3]

        alg_and_args = alg_and_args.split('-')
        self.alg = alg_and_args[0]

        self.alg_args = {}
        if len(alg_and_args) > 1:
            self.alg_args = alg_and_args[1:]
            self.alg_args = {
                a[0]: a[1:].replace('_', '.')
                for a in self.alg_args
            }

    def is_tabular(self):
        return 'n' not in self.alg_args

    def num_iterations(self):
        return self.NUM_ITERATIONS_MAP[self.game]

    def report_gap_factor(self):
        return self.REPORT_GAP_FACTOR_MAP[self.game]

    def command(self, exe):
        if self.is_tabular():
            return ('time {exe} '
                    "--game '{game}' "
                    '-t {t} '
                    '--alg CfrRm '
                    '--report_gap_factor {report_gap_factor} ').format(
                        exe=exe,
                        game=self.GAME_MAP[self.game],
                        t=self.num_iterations(),
                        report_gap_factor=self.report_gap_factor())
        command = ('time {exe} '
                   "--game '{game}' "
                   '-t {t} '
                   '--alg {alg} '
                   '--fractional_partition_size "{s:0.3f}" '
                   '--num_partitions {n} '
                   '--random_seed {random_seed} '
                   '--report_gap_factor {report_gap_factor} ').format(
                       exe=exe,
                       game=self.GAME_MAP[self.game],
                       t=self.num_iterations(),
                       alg=self.ALG_MAP[self.alg],
                       s=float(self.alg_args['s']) / 100.0,
                       n=self.alg_args['n'],
                       random_seed=self.seed,
                       report_gap_factor=self.report_gap_factor())

        if 't' in self.alg_args:
            command += '--temperature {}'.format(self.alg_args['t'])
        if 'p' in self.alg_args:
            command += '--p_exponent {}'.format(self.alg_args['p'])
        return command
