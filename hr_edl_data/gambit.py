def prologue(game_name, num_players):
  players = ' '.join([f'"Player {i + 1}"' for i in range(num_players)])
  return f'EFG 2 R "{game_name}" {{ {players} }}'


def node_name(is_root):
  return "ROOT" if is_root else ""


def uniform_chance_node(num_outcomes, parent=None):
  my_idx = parent + 1
  outcomes = ' '.join(
      [f'"{o}" {1.0 / num_outcomes}' for o in range(num_outcomes)])
  s = f'c "{node_name(parent is None)}" {my_idx} "" {{ {outcomes} }}'
  return (my_idx, s)


def player_node(player, actions, parent=None):
  my_idx = parent + 1
  actions = ' '.join([f'"{a}"' for a in actions])
  s = f'p "{node_name(parent is None)}" {player} {my_idx} "" {{ {actions} }}'
  return (my_idx, s)


def terminal_node(payoffs, parent=None):
  my_idx = parent + 1
  payoffs = ' '.join([f'{z}' for z in payoffs])
  s = f't "{node_name(parent is None)}" {my_idx} "" {{ {payoffs} }}'
  return (my_idx, s)
