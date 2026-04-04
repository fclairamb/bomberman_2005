#!/bin/bash
# Convert BMP assets to PNG with transparency
# Requires: ImageMagick (convert/magick, montage)

set -uo pipefail

SRC="$(cd "$(dirname "$0")/../../images" && pwd)"
DST="$(cd "$(dirname "$0")/../public/assets" && pwd)"

mkdir -p "$DST/sprites" "$DST/ui"

# Detect ImageMagick command (v7 uses magick, v6 uses convert)
if command -v magick &>/dev/null; then
  CONVERT="magick"
  MONTAGE="magick montage"
else
  CONVERT="convert"
  MONTAGE="montage"
fi

echo "Using ImageMagick: $CONVERT"

convert_with_transparency() {
  $CONVERT "$1" -transparent "rgb(255,0,255)" "$2"
}

# 1. Individual tile sprites
echo "Converting tile sprites..."
for f in sol caisse incassable end_door diamant bonus_vitesse bonus_explosion bonus_bombes bonus_sante; do
  if [ -f "$SRC/$f.bmp" ]; then
    convert_with_transparency "$SRC/$f.bmp" "$DST/sprites/$f.png"
    echo "  $f.png"
  fi
done

# 2. Player spritesheets (5 frames x 4 directions)
for player in player1 player2; do
  echo "Converting $player sprites..."
  TMPDIR=$(mktemp -d)
  for dir in haut droite bas gauche; do
    for i in $(seq 0 4); do
      if [ -f "$SRC/$player/${dir}_${i}.bmp" ]; then
        convert_with_transparency "$SRC/$player/${dir}_${i}.bmp" "$TMPDIR/${player}_${dir}_${i}.png"
      fi
    done
  done
  # Montage into single sheet: 5 columns (frames) x 4 rows (directions: up, right, down, left)
  $MONTAGE "$TMPDIR/${player}_haut_"*.png "$TMPDIR/${player}_droite_"*.png \
           "$TMPDIR/${player}_bas_"*.png "$TMPDIR/${player}_gauche_"*.png \
           -tile 5x4 -geometry +0+0 -background none "$DST/sprites/${player}.png"
  echo "  ${player}.png (spritesheet)"
  rm -rf "$TMPDIR"
done

# 3. Enemy spritesheet
echo "Converting enemy sprites..."
TMPDIR=$(mktemp -d)
for dir in haut droite bas gauche; do
  for i in $(seq 0 4); do
    if [ -f "$SRC/mechant/${dir}_${i}.bmp" ]; then
      convert_with_transparency "$SRC/mechant/${dir}_${i}.bmp" "$TMPDIR/mechant_${dir}_${i}.png"
    fi
  done
done
$MONTAGE "$TMPDIR/mechant_haut_"*.png "$TMPDIR/mechant_droite_"*.png \
         "$TMPDIR/mechant_bas_"*.png "$TMPDIR/mechant_gauche_"*.png \
         -tile 5x4 -geometry +0+0 -background none "$DST/sprites/mechant.png"
echo "  mechant.png (spritesheet)"
rm -rf "$TMPDIR"

# 4. Bomb spritesheet (4 frames)
echo "Converting bomb sprites..."
TMPDIR=$(mktemp -d)
for i in $(seq 0 3); do
  convert_with_transparency "$SRC/bombe_${i}.bmp" "$TMPDIR/bombe_${i}.png"
done
$MONTAGE "$TMPDIR/bombe_"*.png -tile 4x1 -geometry +0+0 -background none "$DST/sprites/bombe.png"
echo "  bombe.png (spritesheet)"
rm -rf "$TMPDIR"

# 5. Explosion spritesheet (4 frames)
echo "Converting explosion sprites..."
TMPDIR=$(mktemp -d)
for i in $(seq 0 3); do
  convert_with_transparency "$SRC/explosion_${i}.bmp" "$TMPDIR/explosion_${i}.png"
done
$MONTAGE "$TMPDIR/explosion_"*.png -tile 4x1 -geometry +0+0 -background none "$DST/sprites/explosion.png"
echo "  explosion.png (spritesheet)"
rm -rf "$TMPDIR"

# 6. Death spritesheet (5 frames)
echo "Converting death sprites..."
TMPDIR=$(mktemp -d)
for i in $(seq 0 4); do
  convert_with_transparency "$SRC/mort_${i}.bmp" "$TMPDIR/mort_${i}.png"
done
$MONTAGE "$TMPDIR/mort_"*.png -tile 5x1 -geometry +0+0 -background none "$DST/sprites/mort.png"
echo "  mort.png (spritesheet)"
rm -rf "$TMPDIR"

# 7. UI backgrounds (large BMPs → compressed PNGs)
echo "Converting UI backgrounds..."
for f in jeu menu_principal fond_highscores game_over pause; do
  if [ -f "$SRC/$f.bmp" ]; then
    $CONVERT "$SRC/$f.bmp" "$DST/ui/$f.png"
    echo "  $f.png"
  fi
done

# 8. Menu buttons
echo "Converting menu buttons..."
for f in un_joueur un_joueur_hover deux_joueurs deux_joueurs_hover \
         highscores highscores_hover editeur editeur_hover \
         quitter quitter_hover; do
  if [ -f "$SRC/$f.bmp" ]; then
    convert_with_transparency "$SRC/$f.bmp" "$DST/ui/$f.png"
    echo "  $f.png"
  fi
done

# 9. Editor UI
echo "Converting editor UI..."
mkdir -p "$DST/ui/editeur"
for f in fond menu1 menu2; do
  if [ -f "$SRC/editeur/$f.bmp" ]; then
    convert_with_transparency "$SRC/editeur/$f.bmp" "$DST/ui/editeur/$f.png"
    echo "  editeur/$f.png"
  fi
done

echo ""
echo "Asset conversion complete!"
