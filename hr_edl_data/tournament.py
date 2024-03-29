import numpy as np
import pandas as pd
import matplotlib.lines as mlines
import seaborn as sns
import hr_edl_data.experiment_parameters as xp

# Algorithm labels
_alg_label_map = {
    'CFR': r'$\\\text{CF}$',
    'A-EFR': r'$\\\text{ACT}$',
    'CFR_IN': r'$\\\text{CF}_{\text{IN}}$',
    'A-EFR_IN': r'$\\\text{ACT}_{\text{IN}}$',
    'CSPS-EFR': r'$\\\text{CSPS}$',
    'CFPS-EFR': r'$\\\text{CFPS}$',
    'CFPS-EFR_EX+IN': r'$\\\text{CFPS}_{\text{EX} + \text{IN}}$',
    'TIPS-EFR': r'$\\\text{TIPS}$',
    'TIPS-EFR_EX+IN': r'$\\\text{TIPS}_{\text{EX} + \text{IN}}$',
    'CFR_EX+IN': r'$\\\text{CF}_{\text{EX} + \text{IN}}$',
    'BPS-EFR': r'$\\\text{BPS}$',
    'BEHAV-EFR': r'$\\\text{BHV}$',
}


def alg_label(tag):
  if tag in _alg_label_map:
    return _alg_label_map[tag]
  return tag


# Algorithm ordering and sorting
_alg_order_map = {
    'CFR': 0,
    'CFR+': 1,
    'A-EFR': 6,
    'CFR_IN': 8,
    'A-EFR_IN': -1,
    'CSPS-EFR': 20,
    'CFPS-EFR': 16,
    'CFPS-EFR_EX+IN': 18,
    'TIPS-EFR': 22,
    'TIPS-EFR_EX+IN': 24,
    'CFR_EX+IN': 10,
    'BPS-EFR': 14,
    'BEHAV-EFR': 26,
    'avg': 30,
    'BR': 31,
}


def alg_sort_key(tag):
  return _alg_order_map[tag]


def alg_sort_keys(tags):
  return pd.Index([alg_sort_key(tag) for tag in tags], name=tags.name)


def with_sorted_algs(df):
  df = df.sort_index(axis=0, key=alg_sort_keys)
  df.sort_index(axis=1, key=alg_sort_keys, inplace=True)
  return df


_game_tags = [
    'leduc',
    'kuhn_3p',
    'kuhn_4p',
    'goofspiel',
    'goofspiel_ascending',
    'random_goofspiel',
    'goofspiel_3p',
    'goofspiel_ascending_3p',
    'sheriff',
    'tiny_bridge',
    'tiny_hanabi',
]

# Game labels
_game_label_map = {
    'sheriff': r'Sheriff',
    'tiny_bridge': r'tiny bridge',
    'kuhn_3p': r'Kuhn poker',
    'kuhn_4p': r'Kuhn poker',
    'leduc': r"Leduc hold'em",
    'random_goofspiel': r"goofspiel",
    'tiny_hanabi': r"tiny Hanabi"
}


def game_label(tag, t=None):
  if tag in _game_label_map:
    name = _game_label_map[tag]
  elif tag[:len('goofspiel')] == 'goofspiel':
    name = 'goofspiel'
  else:
    name = tag

  params_string = ''
  if tag in xp.NUM_PLAYERS_MAP:
    params_string = r'N=\num{{{}}}'.format(xp.NUM_PLAYERS_MAP[tag])
  if t is not None:
    params_string = r'{},T=\num{{{}}}'.format(params_string, t)
  if tag in xp.EXTRA_GAME_PARAMS_MAP:
    return '{}$({})$'.format(
        name,
        ','.join(xp.EXTRA_GAME_PARAMS_MAP[tag] + (params_string,)))
  if params_string != '':
    return '{}$({})$'.format(name, params_string)
  else:
    return name


# Game order and sorting
_game_order_map = {tag: i for i, tag in enumerate(_game_tags + ['avg'])}


def game_sort_key(tag):
  return _game_order_map[tag]


def game_sort_keys(tags):
  return pd.Index([game_sort_key(tag) for tag in tags], name=tags.name)


def with_sorted_alg_game(df):
  df = df.sort_index(axis=0, key=alg_sort_keys)
  df.sort_index(axis=1, key=game_sort_keys, inplace=True)
  return df


# Utility adjustment


def adjust_game_utility(game_tag, value):
  if game_tag == 'leduc':
    return value / 13.
  if game_tag == 'kuhn_3p':
    return (value - 1) / 3.
  if game_tag == 'tiny_bridge':
    return (value + 2) / 38.
  if game_tag == 'goofspiel' or game_tag == 'random_goofspiel':
    return value
  else:
    raise BaseException(f"No adjustment registered for {game_tag}")


def adjust_utilities(series_by_game):
  return pd.Series({
      game_tag: adjust_game_utility(game_tag, series_by_game.at[game_tag])
      for game_tag in series_by_game.index
  })


# Data manipulation


def load_df(file_name='../results/efr_data.npy', **kwargs):
  return pd.DataFrame.from_records(
      np.load('../results/efr_data.npy', allow_pickle=True, **kwargs))


def without_br_row(df):
  return df.query('row_alg != "BR"')


def br_row(df):
  return df.query('row_alg == "BR"')


def with_avg_row(df):
  row_avgs = df.mean(axis=1)
  row_avgs.name = 'avg'
  return pd.concat([df, row_avgs], axis=1)


def with_avg_row_col(df, exclude_br=True):
  if exclude_br and 'BR' in df.index:
    col_avgs = without_br_row(df).mean(axis=0)
  else:
    col_avgs = df.mean(axis=0)
  mean_of_means = col_avgs.mean()
  col_avgs = col_avgs.to_frame('avg').transpose()
  row_avgs = df.mean(axis=1)
  row_avgs = pd.concat([row_avgs, pd.Series({'avg': mean_of_means})], axis=0)
  row_avgs.name = 'avg'
  df = pd.concat([df, col_avgs], axis=0)
  return pd.concat([df, row_avgs], axis=1)


def max_element(df):
  return df.max().max().squeeze()


def min_element(df):
  return df.min().min().squeeze()


def mean_element(df):
  return df.mean().mean().squeeze()


def midpoint(df):
  return (max_element(df) - min_element(df)) / 2.0


class Data():

  @classmethod
  def load(cls, file_name='../results/efr_data.npy', **kwargs):
    return cls(load_df(file_name, **kwargs))

  def __init__(self, data):
    self.data = data

  def games(self):
    return sorted(self.data.game_tag.unique(), key=game_sort_key)

  def det_table(self, game, mode, t):
    assert mode == 'fixed' or mode == 'sim'
    df = self.data.query(f'game_tag == "{game}" & mode == "{mode}" & t == {t}')
    return df.pivot(index='row_alg', columns='col_alg', values='value')

  def time_avg_det_table(self, game, mode, num_iterations):
    assert mode == 'fixed' or mode == 'sim'
    df = self.data.query(
        f'game_tag == "{game}" & mode == "{mode}" & t < {num_iterations}')
    return df.pivot(index='t', columns=['row_alg', 'col_alg'],
                    values='value').mean().unstack()

  def fixed_table(self, game, t):
    return self.det_table(game, 'fixed', t)

  def time_avg_fixed_table(self, game, num_iterations):
    return self.time_avg_det_table(game, 'fixed', num_iterations)

  def sim_table(self, game, t):
    return self.det_table(game, 'sim', t)

  def time_avg_sim_table(self, game, num_iterations):
    return self.time_avg_det_table(game, 'sim', num_iterations)

  def all_seeds(self):
    return self.data.seed.unique()

  def shuffled_table(self, game, seed, t):
    df = self.data.query(
        f'game_tag == "{game}" & mode == "shuffled" & seed == "{seed}" & t == {t}'
    )
    return df.pivot(index='row_alg', columns='col_alg', values='value')

  def each_shuffled_table(self, game, t):
    for seed in self.all_seeds():
      yield self.shuffled_table(game, seed, t)

  def shuffled_table_by_seed(self, game, t):
    df = self.data.query(
        f'game_tag == "{game}" & mode == "shuffled" & t == {t}')
    return df.pivot(index='seed',
                    columns=['row_alg', 'col_alg'],
                    values='value')

  def avg_shuffled_table(self, game, t):
    return self.shuffled_table_by_seed(game, t).mean().unstack()

  def max_abs_diff(self, game, t):
    df = self.shuffled_table_by_seed(game, t)
    return df.max() - df.min()

  def max_abs_diff_from_mean(self, game, t):
    df = self.shuffled_table_by_seed(game, t)
    df_mean = df.mean()
    return np.maximum(df.max() - df_mean, df_mean - df.min())

  def det_avg_table(self, mode, t):
    assert mode == 'fixed' or mode == 'sim'
    return self.data.query(f'mode == "{mode}" & t == {t}').pivot(
        index='col_alg', columns=['game_tag',
                                  'row_alg'], values='value').mean().unstack()

  def time_avg_det_avg_table(self, mode, num_iterations):
    assert mode == 'fixed' or mode == 'sim'
    return self.data.query(f'mode == "{mode}" & t < {num_iterations}').pivot(
        index=['t', 'col_alg'], columns=['game_tag', 'row_alg'],
        values='value').mean().unstack()

  def fixed_avg_table(self, t):
    return self.det_avg_table('fixed', t)

  def time_avg_fixed_avg_table(self, num_iterations):
    return self.time_avg_det_avg_table('fixed', num_iterations)

  def sim_avg_table(self, t):
    return self.det_avg_table('sim', t)

  def time_avg_sim_avg_table(self, num_iterations):
    return self.time_avg_det_avg_table('sim', num_iterations)

  def shuffled_avg_table(self, t):
    return self.data.query(f'mode == "shuffled" & t == {t}').pivot(
        index=['seed', 'game_tag'],
        columns=['row_alg', 'col_alg'],
        values='value').mean().unstack()


# Plotting


def add_lines_to_separate_avg_row_col(axes, seperate_br=False):
  xmin, xmax = axes.get_xbound()
  ymin, ymax = axes.get_ybound()
  axes.add_line(
      mlines.Line2D([xmax - 1, xmax - 1], [ymin, ymax],
                    color='black',
                    linewidth=3))
  axes.add_line(
      mlines.Line2D([xmin, xmax], [ymax - 1, ymax - 1],
                    color='black',
                    linewidth=3))
  if seperate_br:
    axes.add_line(
        mlines.Line2D([xmin, xmax], [ymax - 2, ymax - 2],
                      color='black',
                      linewidth=3))


def br_percentage_heatmap(df, ax=None, fmt="0.2f", **kwargs):
  df_with_avgs = with_avg_row_col(without_br_row(df))
  df_col_mins = df_with_avgs.min(axis=0)
  df_as_frac_of_br = (df_with_avgs - df_col_mins) / (
      with_avg_row(br_row(df)).squeeze() - df_col_mins)
  g = sns.heatmap(
      with_sorted_algs(df_as_frac_of_br),
      annot=True,
      fmt=fmt,
      xticklabels=[
          alg_label(tag)
          for tag in sorted(df_with_avgs.columns, key=alg_sort_key)
      ],
      yticklabels=[
          alg_label(tag) for tag in sorted(df_with_avgs.index, key=alg_sort_key)
      ],
      annot_kws={'size': 'large'},
      center=df_as_frac_of_br.at['avg', 'avg'],
      vmin=0,
      vmax=1,
      ax=ax,
      **kwargs)
  g.axes.xaxis.set_ticks_position("top")
  add_lines_to_separate_avg_row_col(g.axes)
  return g


def heatmap(df, ax=None, **kwargs):
  df_with_avgs = with_avg_row_col(df)
  g = sns.heatmap(
      with_sorted_algs(df_with_avgs),
      annot=True,
      xticklabels=[
          alg_label(tag)
          for tag in sorted(df_with_avgs.columns, key=alg_sort_key)
      ],
      yticklabels=[
          alg_label(tag) for tag in sorted(df_with_avgs.index, key=alg_sort_key)
      ],
      annot_kws={'size': 'large'},
      # center=df_with_avgs.at['avg', 'avg'],
      ax=ax,
      **kwargs)
  g.axes.xaxis.set_ticks_position("top")
  add_lines_to_separate_avg_row_col(g.axes, 'BR' in df.index)
  return g


def br_percentage_heatmap_avg(df, ax=None, fmt="0.2f", **kwargs):
  df_with_avgs = with_avg_row_col(without_br_row(df))
  df_col_mins = df_with_avgs.min(axis=0)
  df_as_frac_of_br = (df_with_avgs - df_col_mins) / (
      with_avg_row(br_row(df)).squeeze() - df_col_mins)
  g = sns.heatmap(
      with_sorted_alg_game(df_as_frac_of_br),
      annot=True,
      fmt=fmt,
      xticklabels=[
          game_label(tag)
          for tag in sorted(df_with_avgs.columns, key=game_sort_key)
      ],
      yticklabels=[
          alg_label(tag) for tag in sorted(df_with_avgs.index, key=alg_sort_key)
      ],
      annot_kws={'size': 'large'},
      center=df_as_frac_of_br.at['avg', 'avg'],
      vmin=0,
      vmax=1,
      ax=ax,
      **kwargs)
  g.axes.xaxis.set_ticks_position("top")
  add_lines_to_separate_avg_row_col(g.axes)
  return g


def heatmap_avg(df, ax=None, **kwargs):
  df_with_avgs = with_avg_row_col(df)
  g = sns.heatmap(
      with_sorted_alg_game(df_with_avgs),
      annot=True,
      xticklabels=[
          game_label(tag)
          for tag in sorted(df_with_avgs.columns, key=game_sort_key)
      ],
      yticklabels=[
          alg_label(tag) for tag in sorted(df_with_avgs.index, key=alg_sort_key)
      ],
      annot_kws={'size': 'large'},
      center=df_with_avgs.at['avg', 'avg'],
      ax=ax,
      **kwargs)
  g.axes.xaxis.set_ticks_position("top")
  add_lines_to_separate_avg_row_col(g.axes, 'BR' in df.index)
  return g
