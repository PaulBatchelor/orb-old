function make_icon {
	inkscape -z -e ic_launcher.png -w $1 -h $1 icon.svg
	mv ic_launcher.png $2
}

make_icon 72 res/mipmap-hdpi
make_icon 48 res/mipmap-mdpi
make_icon 96 res/mipmap-xhdpi
make_icon 144 res/mipmap-xxhdpi
	
# inkscape -z -e icon_hires.png -w 512 -h 512 icon.svg


# ./res/mipmap-hdpi/ic_launcher.png[3] PNG 72x72 72x72+0+0 8-bit sRGB 3.42KB 0.000u 0:00.000
# ./res/mipmap-mdpi/ic_launcher.png[1] PNG 48x48 48x48+0+0 8-bit sRGB 2.21KB 0.000u 0:00.000
# ./res/mipmap-xhdpi/ic_launcher.png PNG 96x96 96x96+0+0 8-bit sRGB 4.84KB 0.000u 0:00.000
# ./res/mipmap-xxhdpi/ic_launcher.png[2] PNG 144x144 144x144+0+0 8-bit sRGB 7.72KB 0.000u 0:00.000
# ./res/mipmap-xxxhdpi/ic_launcher.png[4] PNG 192x192 192x192+0+0 8-bit sRGB 10.5KB 0.000u 0:00.000
