import seaborn as sns
import matplotlib.pyplot as plt


def plt_use_tex():
  params = {
      'text.usetex':
          True,
      'text.latex.preamble': [
          r'\usepackage{amsmath}', r'\usepackage{nicefrac}',
          r'\usepackage[group-separator={,}]{siunitx}'
      ]
  }
  plt.rcParams.update(params)


def set_plt_params():
  plt_use_tex()


def diverging_color_palette():
  '''A red-blue color palette that should be good for colorblindness.'''
  return sns.color_palette("RdBu", 128)


def sequential_color_palette():
  '''A blue color palette that should be good for colorblindness.'''
  return sns.color_palette("Blues", 128)
