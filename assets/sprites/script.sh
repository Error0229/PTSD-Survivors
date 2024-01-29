#!/bin/bash

sizex=$1
sizey=$2
text=$3
pointsize=$4

convert -size ${sizex}x${sizey} -background white -fill black -gravity center -pointsize $pointsize label:"$text" $text.png
