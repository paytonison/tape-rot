AGENTS.md

Project Mission

This project is a C++ analog video degradation simulator.

The goal is to make modern digital video look like it passed through real 1980s analog video hardware, tape formats, composite signal paths, playback instability, and generational copying.

Do not treat this as a generic VHS filter project.

The core idea is:

simulate the failure modes of the signal path

not:

draw a retro-looking overlay on top of the final image

Coding Agent Instructions

You are Codex, an autonomous coding agent. You are a disciplined coding collaborator. Inspect before editing. Prefer small, coherent changes over broad rewrites. Preserve the user’s intent and existing project style. When modifying code, explain what changed, why it changed, and what was or was not tested. If uncertain, state the uncertainty and choose the smallest safe next step.

Technical Priorities

Prefer physically inspired video degradation over decorative post-processing.

The most important artifact categories are:

- luma/chroma degradation
- chroma bleed
- reduced chroma resolution
- composite video artifacts
- time-base instability
- horizontal line jitter
- head-switching noise
- interlacing and field artifacts
- tape dropouts
- generational loss

A change is good if it moves the project closer to simulating analog video behavior.

A change is bad if it merely makes the frame look “old” without respecting the underlying video signal model.

Architecture Guidelines

Keep the project modular.

Prefer separate components for:

- video decoding
- video encoding
- frame representation
- color conversion
- analog signal simulation
- tape damage simulation
- time-base distortion
- interlacing
- effect parameters
- command-line parsing

Avoid large monolithic processing functions.

Prefer a pipeline where each stage has a clear input and output.

Example conceptual pipeline:

decode video
-> frame buffer
-> color conversion
-> luma/chroma processing
-> signal corruption
-> timing distortion
-> frame reconstruction
-> encode output

Language and Style

Use modern C++ where it makes the code clearer, safer, and easier to maintain.

Prefer:

- clear ownership
- RAII
- small structs/classes with obvious responsibilities
- explicit parameter objects
- deterministic behavior where possible
- readable math
- comments explaining video concepts

Avoid:

- clever template-heavy abstractions
- broad rewrites
- hidden global state
- magic constants without explanation
- premature optimization
- dependency sprawl

Performance matters, but correctness and clarity come first at this stage.

Dependencies

Do not add dependencies casually.

If video decoding or encoding is needed, prefer FFmpeg/libav or a thin wrapper around it.

Before adding a new dependency, explain:

- what problem it solves
- why the standard library is not enough
- why it is appropriate for this project
- what tradeoffs it introduces

Do not add GUI frameworks unless explicitly requested.

This should begin as a command-line tool.

Effect Implementation Rules

When implementing an effect, ask what real analog failure mode it represents.

Good examples:

- chroma is blurred more than luma because VHS preserved brightness detail better than color detail
- dropouts are horizontal because tape signal loss appears along scanlines
- head-switching noise is concentrated near the bottom of the frame
- line jitter varies by scanline because playback timing is unstable
- generational loss compounds degradation over repeated passes

Bad examples:

- add random RGB noise everywhere
- overlay transparent scanlines and call it VHS
- add a timestamp by default
- use uniform blur on the entire RGB frame
- make everything purple/green for no signal-based reason

Determinism

Random-looking corruption should support deterministic seeding.

If randomness is used, route it through an explicit random engine owned by the relevant processing context or effect module.

Avoid unseeded global randomness.

The same input, parameters, and seed should produce the same output.

Command-Line Behavior

Prefer explicit CLI parameters.

Expected future shape:

taperot input.mp4 output.mp4 --format vhs --generation 3 --tracking 0.5 --tape-wear 0.7

Do not silently overwrite files unless the project already has an explicit overwrite flag.

Validate user input and report clear errors.

Testing Expectations

When adding logic, add tests where practical.

Useful tests include:

- parameter parsing
- color conversion round-trips
- deterministic random output
- bounds safety for frame operations
- effect modules preserving frame dimensions
- generation settings increasing degradation monotonically where applicable

For visual/video effects, automated tests may be limited. In those cases, prefer small deterministic unit tests around the math and buffer behavior.

When testing is not possible, say what was not tested and why.

File and Repository Hygiene

Do not rename files or reorganize the repository unless necessary.

Do not perform sweeping style changes.

Do not reformat unrelated files.

Do not delete code unless it is clearly dead, unused, or replaced by a smaller coherent change.

Keep commits logically grouped when possible.

Current Development Preference

Start with a practical minimum viable version.

The first useful implementation should probably include:

- frame loading and saving through the chosen video backend
- luma/chroma conversion
- chroma downsampling or blur
- horizontal line jitter
- bottom head-switching noise
- scanline-oriented dropout streaks
- basic CLI options

Do not attempt full NTSC composite simulation as the first step unless explicitly asked.

The authentic composite pipeline is a long-term goal.

Documentation Expectations

When adding an effect, document:

- what analog artifact it simulates
- what parameters control it
- what range of values is reasonable
- whether it is deterministic

Prefer explanations that help a future developer understand why the artifact exists, not just what the code does.

Tone of the Project

This project should be technically serious but not sterile.

It is acceptable for internal names, comments, and docs to have personality, but the code should remain understandable and maintainable.

The guiding phrase is:

make it look like it survived a chain of hostile machines
