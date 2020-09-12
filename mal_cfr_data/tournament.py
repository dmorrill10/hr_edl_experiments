import numpy as np
import pandas as pd
import mal_cfr_data.experiment_parameters as xp


def load_df(file_name='results/mal_cfr_data.npy', **kwargs):
  return pd.DataFrame.from_records(
      np.load('results/mal_cfr_data.npy', allow_pickle=True, **kwargs))


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
    'avg': 20
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
