# TicTacToe_PS_Project

## Main goal of this project is to play TicTacToe with second player over LAN.
## Configure Machine

Use commands in this order
```
sudo apt-get update
sudo apt-get install git
git config --global user.email "your mail"
git config --global user.name "your name"
```
Then you can download our repo using this command
```
git clone https://github.com/JOQUR/TicTacToe_PS_Project.git
```
then `cd TicTacToe_PS_Project`.
If you want to run this project, type `make`, then `./server` and on the other machine `./client <ipaddr>`.

## TODO
- [ ] Check if connection from second client works
- [ ] Add flags to let only one client save his move
- [ ] Send to clients refreshed gameboard after each move