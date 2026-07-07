Tape Rot

Tape Rot is a C++ video processing project that transforms modern digital video into footage that looks like it passed through real 1980s analog video hardware.

The goal is not to make a fake “VHS filter.” The goal is to simulate the kinds of signal degradation, timing instability, color loss, chroma bleed, head-switching noise, dropouts, and generational decay that naturally happened because of analog tape formats, composite video, consumer playback hardware, and repeated copying.

Tape Rot should make video look like it survived hostile machines.

Project Goals

Tape Rot aims to model the video signal path rather than simply draw effects over a finished RGB image.

A good analog-video simulation should damage the signal before reconstruction, not merely add blur, noise, scanlines, and fake timestamps afterward.

The intended pipeline is:

input video
-> decoded frames
-> RGB/YUV conversion
-> analog signal approximation
-> tape and transport damage
-> luma/chroma degradation
-> timing instability
-> reconstructed frame
-> encoded output video

Core Effects

Tape Rot should eventually support several classes of analog video degradation.

Luma and Chroma Degradation

Analog video preserved brightness better than color. Chroma should be softer, noisier, less stable, and more prone to bleeding than luma.

Expected behavior:

- reduced horizontal detail
- soft color resolution
- color bleeding near edges
- desaturation under heavy degradation
- unstable reds and saturated colors

Composite Video Artifacts

Composite video mixes brightness and color information into a single signal. Imperfect encoding and decoding should create artifacts that feel native to the format.

Expected behavior:

- dot crawl
- rainbowing around fine detail
- false color
- cross-luma and cross-chroma artifacts
- unstable edges

Time-Base Instability

Analog tape playback is not perfectly stable. Lines may shift, bend, wobble, or tear because the timing of the physical tape and playback heads is imperfect.

Expected behavior:

- horizontal jitter
- line-by-line offset
- frame wobble
- vertical sync instability
- tracking drift

Head-Switching Noise

VHS and related formats often show distortion near the bottom of the frame where the rotating video heads switch.

Expected behavior:

- noisy band near the bottom of the frame
- horizontal tearing
- local brightness/color corruption
- unstable tracking region

Dropouts and Tape Damage

Physical tape damage, dust, wear, and signal loss should appear as scanline-oriented corruption rather than generic random film grain.

Expected behavior:

- white or black streaks
- horizontal speckles
- signal gaps
- short bursts of static
- random damaged bands

Interlacing

1980s video should usually feel interlaced. Motion should have field structure and occasional field mismatch.

Expected behavior:

- odd/even field separation
- temporal offset between fields
- combing under motion
- field-specific jitter

Generational Loss

Repeated copying should compound the damage. A first-generation tape should look soft and unstable. A fifth-generation copy should look like public-access television recovered from a cursed basement.

Expected behavior:

- cumulative blur
- increasing chroma loss
- more noise
- worse timing errors
- reduced contrast
- unstable sync

Command-Line Vision

The final tool should be usable from the command line.

Example:

taperot input.mp4 output.mp4 --format vhs --generation 4 --tracking 0.6 --tape-wear 0.8

Possible options:

--format vhs | betamax | umatic | broadcast | camcorder
--generation <number>
--tracking <0.0-1.0>
--tape-wear <0.0-1.0>
--chroma-bleed <0.0-1.0>
--timebase-jitter <0.0-1.0>
--dropouts <0.0-1.0>
--head-switch-noise on | off
--interlace on | off
--composite on | off

Current Scaffold

The current executable is named `taperot`.

It validates command-line options and prints the requested processing plan. The CLI does not decode, process, or encode video yet.

The core library can mutate synthetic in-memory planar frames for tests. Frames currently use one luma plane and two chroma planes.

Usage:

```sh
taperot <input> <output> [options]
```

Supported scaffold options:

```text
--format <vhs|betamax|umatic|broadcast|camcorder>
--generation <number>
--tracking <0.0-1.0>
--tape-wear <0.0-1.0>
--chroma-bleed <0.0-1.0>
--timebase-jitter <0.0-1.0>
--dropouts <0.0-1.0>
--head-switch-noise <on|off>
--interlace <on|off>
--composite <on|off>
--seed <integer>
--help
```

Implemented In-Memory Effects

The current frame effects operate on planar luma/chroma buffers and are deterministic for the same input, parameters, and seed.

- `ChromaDegrade` horizontally smears and weakens chroma planes while leaving luma detail intact. `strength` is expected to be `0.0` to `1.0`.
- `LineJitter` shifts individual scanlines left or right to approximate time-base instability. `strength` controls the maximum offset allowed.
- `Dropout` creates ragged horizontal luma/chroma damage streaks from simulated tape signal loss. `strength` controls count, length, thickness, and severity.
- `HeadSwitchNoise` corrupts a localized lower-frame band with tearing, luma disturbance, and chroma disturbance. `strength` is `0.0` to `1.0`; `bandFraction` controls the bottom band size.

Implementation Strategy

The first version should be practical and modular.

Minimum viable pipeline:

1. Decode input video with FFmpeg/libav or an equivalent backend.
2. Convert frames from RGB into a luma/chroma representation.
3. Degrade chroma more aggressively than luma.
4. Apply horizontal line jitter.
5. Add head-switching noise near the bottom of the frame.
6. Add scanline-oriented dropouts.
7. Optionally simulate interlaced fields.
8. Encode the processed frames back into a video file.

The more authentic long-term version should include approximate NTSC composite encoding and decoding:

RGB
-> YIQ/YUV
-> composite signal approximation
-> signal corruption
-> imperfect decode
-> output frame

Design Principles

Tape Rot should be physically inspired, not aesthetically lazy.

Prefer:

- signal-path simulation
- line-based corruption
- chroma/luma separation
- timing instability
- cumulative generational damage
- deterministic random seeds for reproducible outputs

Avoid:

- generic overlay noise
- fake scanlines as the main effect
- simple RGB blur only
- uniform random static
- modern glitch-art effects unless explicitly requested
- hardcoded “retro” timestamps

Build

Requirements:

- CMake 3.16 or newer
- A C++17 compiler

Build:

```sh
cmake -S . -B build
cmake --build build
```

Test:

```sh
ctest --test-dir build --output-on-failure
```

Run the scaffold:

```sh
./build/taperot input.mp4 output.mp4 --format vhs --generation 3 --tracking 0.5 --tape-wear 0.7 --seed 1234
```

Project Status

Experimental.

The current goal is to establish a clean C++ architecture for parsing user intent, representing frames, and shaping independent effect modules. Real video decoding and output encoding are intentionally not implemented yet.

License

BSD 3-Clause. See `LICENSE`.
