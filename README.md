# Midilute

Midilute is a command-line application that turns midi input into keystrokes
that causes the lute in Mordhau to be played.

## Not for Windows

I wrote this program for myself, but decided to publish it. I play Mordhau **on
Linux** via software made by Valve called Proton. Because of this the program I
wrote unfortunately only works on Linux.

It uses Guile, ALSA, and xdotool.

* Guile runs on Windows via Cygwin or MinGW, but 
* ALSA only runs on Linux.
* xdotool is available on many \*nix platforms.

If you want something like this for Windows:

I know that the creator of Lutebot is [working on a new version that has midi device support](https://mordhau.com/forum/topic/13519/mordhau-lute-bot/?page=10#c182).
There is also [another project](https://github.com/Pygex/LuteController) that seems to support this feature.

## How to use the program

You'll need to build the C++ files with CMake. Let's assume the resulting
libguilemidi.so file is in a directory called build/.

Now you should be able to run this command.

```
$ LTDL_LIBRARY_PATH=build guile main.scm
```

This will launch Midilute, but playing your midi device will not yet do
anything. You'll need to connect your midi device with the program using
`aconnect`.

```sh
# Show all midi input and output devices
$ aconnect -l
client 0: 'System' [type=kernel]
    0 'Timer           '
    1 'Announce        '
client 14: 'Midi Through' [type=kernel]
    0 'Midi Through Port-0'
client 28: 'Digital Piano' [type=kernel,card=3]
    0 'Digital Piano MIDI 1'
client 128: 'mordhau input' [type=user,pid=19573]
    0 'Mordhau input   '

# Digital piano is my midi device, Mordhau input is the virtual device that we
# want to connect to.

$ aconnect 28:0 128:0

# Now when you press a key on your midi device that is in Mordhau's lute range
# it should automatically type something like `equipmentcommand 16.
```

Now you should be able to play the Lute with your midi device!
