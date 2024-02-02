sizex=$1
sizey=$2
text=$3
pointsize=$4

convert -size ${sizex}x${sizey} -background white -fill LightSkyBlue -gravity center -depth 8 -pointsize $pointsize label:"$text" $text.png;
