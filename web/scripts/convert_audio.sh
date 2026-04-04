#!/bin/bash
# Convert WAV/MIDI audio to OGG + MP3 for web
# Requires: ffmpeg, optionally timidity for MIDI

set -euo pipefail

SRC="$(cd "$(dirname "$0")/../../sons" && pwd)"
DST="$(cd "$(dirname "$0")/../public/assets/audio" && pwd)"

mkdir -p "$DST"

# Convert WAV files to OGG + MP3
echo "Converting WAV files..."
for f in "$SRC"/*.wav; do
  name=$(basename "$f" .wav)
  echo "  $name..."
  ffmpeg -y -i "$f" -c:a vorbis -q:a 6 "$DST/${name}.ogg" 2>/dev/null || true
  ffmpeg -y -i "$f" -c:a aac -q:a 2 "$DST/${name}.mp3" 2>/dev/null || true
done

# Convert MIDI if timidity is available
if [ -f "$SRC/musique.mid" ]; then
  if command -v timidity &>/dev/null; then
    echo "  Converting MIDI with timidity..."
    timidity "$SRC/musique.mid" -Ow -o /tmp/musique.wav
    ffmpeg -y -i /tmp/musique.wav -c:a libvorbis -q:a 6 "$DST/musique.ogg" 2>/dev/null
    ffmpeg -y -i /tmp/musique.wav -c:a libmp3lame -q:a 2 "$DST/musique.mp3" 2>/dev/null
    rm -f /tmp/musique.wav
  elif command -v fluidsynth &>/dev/null; then
    echo "  Converting MIDI with fluidsynth..."
    # Try common soundfont locations
    SF2=""
    for sf in /usr/share/sounds/sf2/*.sf2 /usr/share/soundfonts/*.sf2 /usr/local/share/fluidsynth/*.sf2; do
      if [ -f "$sf" ]; then SF2="$sf"; break; fi
    done
    if [ -n "$SF2" ]; then
      fluidsynth -ni "$SF2" "$SRC/musique.mid" -F /tmp/musique.wav
      ffmpeg -y -i /tmp/musique.wav -c:a libvorbis -q:a 6 "$DST/musique.ogg" 2>/dev/null
      ffmpeg -y -i /tmp/musique.wav -c:a libmp3lame -q:a 2 "$DST/musique.mp3" 2>/dev/null
      rm -f /tmp/musique.wav
    else
      echo "  WARNING: No SoundFont found for MIDI conversion. Skipping musique.mid"
    fi
  else
    echo "  WARNING: Neither timidity nor fluidsynth found. Skipping MIDI conversion."
  fi
fi

echo ""
echo "Audio conversion complete!"
