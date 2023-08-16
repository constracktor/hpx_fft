#!/bin/bash
# create python enviroment
python3 -m venv plot_env
# activate enviroment
source plot_env/bin/activate
# install requirements
pip install numpy matplotlib
# launch game
python3 plot/plot.py