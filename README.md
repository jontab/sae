# sae

I was going through my college laptop and uncovered the code to a toy, collaborative terminal text editor I wrote in 2022 (?). It uses CRDTs, communicates over TCP, and uses curses to display to the terminal.

# Building

Just run `make` in the repository root.

# Usage

- `./bin/sae -i localhost -p 8080 -s` - Starts a server at localhost:8080.
- `./bin/sae -i localhost -p 8080 -c` - Connects to localhost:8080.
- Press `i` when in "NORMAL" mode to go into "INSERT" mode.
- Press `:` when in "NORMAL" mode to go into "COMMAND" mode.
- Press `ESC` when in "INSERT" or "COMMAND" mode to return to "NORMAL" mode.

# Commands

- `:q` - Quit.
- `:w filename` - Write to file.
