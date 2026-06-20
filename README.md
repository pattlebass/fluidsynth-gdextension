# FluidSynth-GDExtension

A cross-platform GDExtension that lets you play MIDI files in your Godot projects.

> [!WARNING]  
> As it was made for one specific project, this extension doesn't expose all of FluidSynth's functionality (yet). Also, the API is subject to change.

## Features

- Play MIDI files using a custom soundfont
- Arrange them using `AudioStreamMidiSequencer`
- Change the BPM
- Multi-platform

## Building

```powershell
git clone https://github.com/pattlebass/fluidsynth-gdextension.git
git submodule update --init --recursive
```

Then check out [build_fluidsynth.md](docs/build_fluidsynth.md).

Finally, run this in the root directory:

```powershell
scons compiledb=yes use_mingw=yes
```
