# LD33

My LD#33 entry

# Dependencies

This game depends on SDL2 and GFraMe.

You can get SDL2 from the its site (https://www.libsdl.org/) or from your
distro's package manager (well, on linux). On Ubuntu, run:

```
$ sudo apt-get install libsdl2-dev
```

Note that to run the game you should simply install the runtime libs;

```
$ sudo apt-get install libsdl2-2.0.0
```

GFraMe can only be obtained from its repository (https://github.com/SirGFM/GFraMe). Clone/Download it and run

```
$ sudo make install
```

# Compiling

To compile the game, simply run:

```
$ make RELEASE=yes
```

Since the default target is the debug one, you've gotta set it to release mode manually.
