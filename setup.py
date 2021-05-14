from setuptools import setup, find_packages

setup(
    name='hr_edl_data',
    version='0.0.1',
    license='',
    packages=find_packages(),
    install_requires=[
        'setuptools >= 20.2.2',
        'absl-py == 0.9.0',
        'matplotlib == 3.3.2',
        'seaborn == 0.9.0',
        'numpy == 1.20.2',
        'pandas == 1.1.5',
        'jupyter == 1.0.0',
    ],
)
