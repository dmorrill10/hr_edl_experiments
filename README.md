# Multi-Agent Learning and CFR Data

Multi-agent learning and CFR experiment data for AAAI 2021 submissions.


## Installation

Experiments require [https://github.com/dmorrill10/open_spiel-private](OpenSpiel-private), a C++ codebase built on OpenSpiel.
Experiments and plotting also requires Python3 and some associated libraries. Run `pip install -r requirements.txt` to install these libraries.


## Managing Experiments and Analysis

Data files have the following convention, `data/<game>.<sampler>.<setting, i.e., fixed or simultaneous>.gen`.
To generate the desired file, simply run the corresponding make command. E.g., `make data/leduc.null.fixed.gen.txt`.
