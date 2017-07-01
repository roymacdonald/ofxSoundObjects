# uses uname to get a more grained os type
# from http://stackoverflow.com/questions/394230/detect-the-os-from-a-bash-script

function lowercase() {
	echo "$1" | sed "y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/"
}

OS=`lowercase \`uname\``

if [ "$OS" == "darwin" ]; then
	OS="osx"
elif [ "$OS" == "windowsnt" ] ; then
	OS="vs"
elif [ "${OS:0:5}" == "mingw" -o "$OS" == "msys_nt-6.3" ]; then
	OS="msys2"
elif [ "$OS" == "linux" ]; then
	ARCH=`uname -m`
	if [ "$ARCH" == "i386" -o "$ARCH" == "i686" ] ; then
		OS="linux"
	elif [ "$ARCH" == "x86_64" ] ; then
		OS="linux64"
	elif [ "$ARCH" == "armv6l" ] ; then
		OS="linuxarmv6l"
	elif [ "$ARCH" == "armv7l" ] ; then
		# Make an exception for raspberry pi to run on armv6l, to conform
		# with openFrameworks.
		if [ -f /opt/vc/include/bcm_host.h ]; then
			OS="linuxarmv6l"
		else
			OS="linuxarmv7l"
		fi
	else
		# We don't know this one, but we will try to make a reasonable guess.
		OS="linux"$ARCH
	fi
fi

echo "$OS"
