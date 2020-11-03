import numpy as np
import pandas as pd
import matplotlib.lines as mlines
import seaborn as sns
import mal_cfr_data.experiment_parameters as xp

# Algorithm labels
_alg_label_map = {
    'CFR+': r'$\\\text{CFR}^+$',
    'A-CFR': r'$\\\text{CFR}_{\text{A}}$',
    'A-CFR+': r'$\\\text{CFR}_{\text{A}}^+$',
    'PGPI(10000/U)': r'$\\\text{PGPI}(\nicefrac{10000}{U})$',
    'PGPI(0.1/U)': r'$\\\text{PGPI}(\nicefrac{0.1}{U})$',
    'CFR_IN': r'$\\\text{CFR}_{\text{IN}}$',
    'CFR+_IN': r'$\\\text{CFR}^+_{\text{IN}}$',
    'A-CFR_IN': r'$\\\text{CFR}_{\text{A}, \text{IN}}$',
    'A-CFR+_IN': r'$\\\text{CFR}^+_{\text{A}, \text{IN}}$',
    'CSPS-CFR': r'$\\\text{CFR}_{\text{CSPS}}$',
    'CSPS-CFR+': r'$\\\text{CFR}_{\text{CSPS}}^+$',
    'CFPS-CFR': r'$\\\text{CFR}_{\text{CFPS}}$',
    'CFPS-CFR+': r'$\\\text{CFR}_{\text{CFPS}}^+$',
    'CFPS-CFR_EX+IN': r'$\\\text{CFR}_{\text{CFPS}, \text{EX} + \text{IN}}$',
    'CFPS-CFR+_EX+IN': r'$\\\text{CFR}_{\text{CFPS}, \text{EX} + \text{IN}}^+$',
    'TIPS-CFR': r'$\\\text{CFR}_{\text{TIPS}}$',
    'TIPS-CFR+': r'$\\\text{CFR}_{\text{TIPS}}^+$',
    'TIPS-CFR_EX+IN': r'$\\\text{CFR}_{\text{TIPS}, \text{EX} + \text{IN}}$',
    'TIPS-CFR+_EX+IN': r'$\\\text{CFR}_{\text{TIPS}, \text{EX} + \text{IN}}^+$',
    'CFR_EX+IN': r'$\\\text{CFR}_{\text{EX} + \text{IN}}$',
    'CFR+_EX+IN': r'$\\\text{CFR}_{\text{EX} + \text{IN}}^+$',
    'BPS-CFR': r'$\\\text{CFR}_{\text{BPS}}$',
    'BPS-CFR+': r'$\\\text{CFR}_{\text{BPS}}^+$',
    'greed_punisher': 'GP'
}


def alg_label(tag):
  if tag in _alg_label_map:
    return _alg_label_map[tag]
  return tag


# Algorithm ordering and sorting
_alg_order_map = {
    'CFR': 0,
    'CFR+': 1,
    'PI': 2,
    'PGPI(1)': 3,
    'PGPI(10000/U)': 4,
    'PGPI(0.1/U)': 5,
    'A-CFR': 6,
    'A-CFR+': 7,
    'CFR_IN': 8,
    'CFR+_IN': 9,
    'A-CFR_IN': 12,
    'A-CFR+_IN': 13,
    'CSPS-CFR': 20,
    'CSPS-CFR+': 21,
    'CFPS-CFR': 16,
    'CFPS-CFR+': 17,
    'CFPS-CFR_EX+IN': 18,
    'CFPS-CFR+_EX+IN': 19,
    'TIPS-CFR': 22,
    'TIPS-CFR+': 23,
    'TIPS-CFR_EX+IN': 24,
    'TIPS-CFR+_EX+IN': 25,
    'CFR_EX+IN': 10,
    'CFR+_EX+IN': 11,
    'BPS-CFR': 14,
    'BPS-CFR+': 15,
    'FP': 26,
    'PI': 27,
    'greed_punisher': 28,
    'avg': 29,
    'BR': 30,
}


def alg_sort_key(tag):
  return _alg_order_map[tag]


def alg_sort_keys(tags):
  return pd.Index([alg_sort_key(tag) for tag in tags], name=tags.name)


def with_sorted_algs(df):
  df = df.sort_index(axis=0, key=alg_sort_keys)
  df.sort_index(axis=1, key=alg_sort_keys, inplace=True)
  return df


# Game labels
_game_label_map = {
    'tiny_bridge': r'tiny bridge',
    'kuhn_3p': r'Kuhn poker',
    'kuhn_4p': r'Kuhn poker',
    'leduc': r"Leduc hold'em",
    'goofspiel': r"goofspiel",
    'goofspiel_ascending': r"goofspiel",
    'random_goofspiel': r"goofspiel",
    'goofspiel_3p': r"goofspiel",
    'goofspiel_ascending_3p': r"goofspiel",
    'tiny_hanabi': r"tiny Hanabi",
    'crewmates_3p': r"crewmates",
    'crewmates_3p_5h': r'crewmates',
    'crewmates_3p_10r': r'crewmates',
    'crewmates_3p_10r_10g': r'crewmates',
    'crewmates_3p_10r_20g': r'crewmates',
    'crewmates_3p_10r_30g': r'crewmates',
    'crewmates_3p_10r_5h_10g': r'crewmates',
    'crewmates_3p_10r_5h_20g': r'crewmates',
    'crewmates_3p_10r_5h_30g': r'crewmates',
    'crewmates_3p_10r_1l': r'crewmates',
    'crewmates_3p_10r_10g_1l': r'crewmates',
    'crewmates_3p_10r_20g_1l': r'crewmates',
    'crewmates_3p_10r_30g_1l': r'crewmates',
    'crewmates_3p_10r_5h_10g_1l': r'crewmates',
    'crewmates_3p_10r_5h_20g_1l': r'crewmates',
    'crewmates_3p_10r_5h_30g_1l': r'crewmates',
}


def game_label(tag, t=None):
  if tag in _game_label_map:
    params_string = r'N=\num{{{}}}'.format(xp.NUM_PLAYERS_MAP[tag])
    if t is not None:
      params_string = r'{},T=\num{{{}}}'.format(params_string, t)
    if tag in xp.EXTRA_GAME_PARAMS_MAP:
      return '{}$({})$'.format(
          _game_label_map[tag],
          ','.join(xp.EXTRA_GAME_PARAMS_MAP[tag] + (params_string,)))
    return '{}$({})$'.format(_game_label_map[tag], params_string)
  return tag


# Game order and sorting
_game_order_map = {
    'leduc': 0,
    'kuhn_3p': 1,
    'kuhn_4p': 2,
    'goofspiel': 3,
    'goofspiel_ascending': 4,
    'random_goofspiel': 5,
    'goofspiel_3p': 6,
    'goofspiel_ascending_3p': 8,
    'tiny_bridge': 9,
    'tiny_hanabi': 10,
    'crewmates_3p': 11,
    'crewmates_3p_5h': 13,
    'crewmates_3p_10r': 12,
    'crewmates_3p_10r_10g': 14,
    'crewmates_3p_10r_20g': 15,
    'crewmates_3p_10r_30g': 16,
    'crewmates_3p_10r_5h_10g': 17,
    'crewmates_3p_10r_5h_20g': 18,
    'crewmates_3p_10r_5h_30g': 19,
    'crewmates_3p_10r_1l': 20,
    'crewmates_3p_10r_10g_1l': 21,
    'crewmates_3p_10r_20g_1l': 22,
    'crewmates_3p_10r_30g_1l': 23,
    'crewmates_3p_10r_5h_10g_1l': 24,
    'crewmates_3p_10r_5h_20g_1l': 25,
    'crewmates_3p_10r_5h_30g_1l': 26,
    'avg': 27,
}


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


def load_df(file_name='results/mal_cfr_data.npy', **kwargs):
  return pd.DataFrame.from_records(
      np.load('results/mal_cfr_data.npy', allow_pickle=True, **kwargs))


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
  def load(cls, file_name='results/mal_cfr_data.npy', **kwargs):
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
