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
    'IPS-CFR': r'$\\\text{CFR}_{\text{IPS}}$',
    'IPS-CFR+': r'$\\\text{CFR}_{\text{IPS}}^+$',
    'TIPS-CFR': r'$\\\text{CFR}_{\text{TIPS}}$',
    'TIPS-CFR+': r'$\\\text{CFR}_{\text{TIPS}}^+$',
    'CFR_EX+IN': r'$\\\text{CFR}_{\text{EX} + \text{IN}}$',
    'CFR+_EX+IN': r'$\\\text{CFR}_{\text{EX} + \text{IN}}^+$',
    'BPS-CFR': r'$\\\text{CFR}_{\text{BPS}}$',
    'BPS-CFR+': r'$\\\text{CFR}_{\text{BPS}}^+$',
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
    'IPS-CFR': 16,
    'IPS-CFR+': 17,
    'TIPS-CFR': 18,
    'TIPS-CFR+': 19,
    'CFR_EX+IN': 10,
    'CFR+_EX+IN': 11,
    'BPS-CFR': 14,
    'BPS-CFR+': 15,
    'avg': 20,
    'BR': 21
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
def _fill_in_num_players_and_iterations(game_string, game_tag):
  return game_string.format(xp.NUM_PLAYERS_MAP[game_tag],
                            xp.NUM_ITERATIONS_MAP[game_tag])


_game_label_map = {
    'tiny_bridge':
        r'tiny bridge($N={},T=\num{{{}}}$)',
    'kuhn_3p':
        r'Kuhn poker($N={},T=\num{{{}}}$)',
    'leduc':
        r"Leduc hold'em($N={},T=\num{{{}}}$)",
    'goofspiel':
        r"$\\\text{{goofspiel}}_{{\\\text{{DET}}}}(5,N={},T=\num{{{}}})$",
    'random_goofspiel':
        r"$\\\text{{goofspiel}}_{{\\\text{{RNG}}}}(4,N={},T=\num{{{}}})$",
}
_game_label_map = {
    game_tag: _fill_in_num_players_and_iterations(game_string, game_tag)
    for game_tag, game_string in _game_label_map.items()
}


def game_label(tag):
  if tag in _game_label_map:
    return _game_label_map[tag]
  return tag


# Game order and sorting
_game_order_map = {
    'leduc': 0,
    'goofspiel': 1,
    'random_goofspiel': 2,
    'tiny_bridge': 3,
    'kuhn_3p': 4,
    'avg': 5,
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

  def det_table(self, game, mode):
    assert mode == 'fixed' or mode == 'sim'
    df = self.data.query(f'game_tag == "{game}" & mode == "{mode}"')
    return df.pivot(index='row_alg', columns='col_alg', values='value')

  def fixed_table(self, game):
    return self.det_table(game, 'fixed')

  def sim_table(self, game):
    return self.det_table(game, 'sim')

  def all_seeds(self):
    return self.data.seed.unique()

  def shuffled_table(self, game, seed):
    df = self.data.query(
        f'game_tag == "{game}" & mode == "shuffled" & seed == "{seed}"')
    return df.pivot(index='row_alg', columns='col_alg', values='value')

  def each_shuffled_table(self, game):
    for seed in self.all_seeds():
      yield self.shuffled_table(game, seed)

  def shuffled_table_by_seed(self, game):
    df = self.data.query(f'game_tag == "{game}" & mode == "shuffled"')
    return df.pivot(index='seed',
                    columns=['row_alg', 'col_alg'],
                    values='value')

  def avg_shuffled_table(self, game):
    return self.shuffled_table_by_seed(game).mean().unstack()

  def max_abs_diff(self, game):
    df = self.shuffled_table_by_seed(game)
    return df.max() - df.min()

  def max_abs_diff_from_mean(self, game):
    df = self.shuffled_table_by_seed(game)
    df_mean = df.mean()
    return np.maximum(df.max() - df_mean, df_mean - df.min())

  def det_avg_table(self, mode):
    assert mode == 'fixed' or mode == 'sim'
    return self.data.query(f'mode == "{mode}"').pivot(
        index='col_alg', columns=['row_alg',
                                  'game_tag'], values='value').mean().unstack()

  def fixed_avg_table(self):
    return self.det_avg_table('fixed')

  def sim_avg_table(self):
    return self.det_avg_table('sim')

  def shuffled_avg_table(self):
    df_shuffled = self.data.query('mode == "shuffled"').pivot(
        index='seed',
        columns=['row_alg', 'game_tag', 'col_alg'],
        values='value').mean().unstack()
    return df_shuffled.transpose().mean().unstack()


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
