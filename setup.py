from setuptools import setup, find_packages

setup(
    name='mal_cfr_data',
    version='0.0.1',
    license='',
    packages=find_packages(),
    install_requires=[
        'setuptools >= 20.2.2',
        'absl-py',
        'matplotlib',
        'seaborn',
        'numpy',
        'pandas',
    ],
)
