# GabibBot
### 🥇2022-2023 winner of the Tablut Challenge for Fundamentals of AI and Knowledge Representation, hosted by Michela Milano
### Author: Davide Gardenal

## How it works?
It's a very simple Min-Max algorithm with alpha-beta cuts written in C++.\
The idea was to develop a very fast implementation of the algorithm and heuristic so that it could explore many states in the limited amount of time provived by the challege. For that the heuristic is not performing big calculations but it extracts information when copying the state of the board.\
No std lib have been used for the state representation. This is becasue we have a static dimensionality and the overhead ad dynamic structures (e.g. std::vector) is too big and slows down the bot quite a lot.

## How the project is structured?
We have two components:
- The C++ module compiled with pybind11
- A Python script

The Python script is used to compunicate with the server and to call the C++ module for executing the actual bot.\
Why splitting the code? Because writing net code in Python is easier than doing the same in C++.

## Where can I find the server?
Here: https://github.com/AGalassi/TablutCompetition

## Do you want to develop a Tablut Bot in C++?
Then you can use my project as a tamplate and edit the C++ code without bothering with the server comunication aspect. Feel free to use it.

## Furthen improvements
- Use bitvectors for state representation instead of shorts. This will result in a speed up in terms of speed with all the operators (but it's quite a bit more complicated to develop and manage)
- Develop a stronger heuristic (of fine tune mine). I spent quite a good chunk of time developing my heuristic but I think there is room for improvement (a tournament can be created with all the heuristics).
