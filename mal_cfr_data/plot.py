import seaborn as sns
import matplotlib.pyplot as plt


def plt_use_tex():
  params = {
      'text.usetex':
          True,
      'text.latex.preamble': ''.join([
          r'\usepackage{amsmath}', r'\usepackage{nicefrac}',
          r'\usepackage[group-separator={,}]{siunitx}'
      ])
  }
  plt.rcParams.update(params)


def set_plt_params(use_serif=True, font_size=24):
  sns.set_style("white")
  sns.set_style("ticks")
  plt_use_tex()
  font_params = {'size': font_size}
  if use_serif:
    font_params['family'] = 'serif'
  plt.rc('font', **font_params)
  plt.rc('pdf', fonttype=42)
  plt.rc('text', color='0')
  plt.rc('axes', labelcolor='0')


def diverging_color_palette():
  '''A red-blue color palette that should be good for colorblindness.'''
  return sns.color_palette("RdBu", 128)


def sequential_color_palette():
  '''A blue color palette that should be good for colorblindness.'''
  return sns.color_palette("Blues", 128)
