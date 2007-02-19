pkgname=codeine
pkgver=1.0.1
pkgrel=1
pkgdesc="A simple xine-based video player"
url="http://www.methylblue.com/codeine/"

build() {
  echo -e "\033[0;34m==>\033[0;0;1m Configure \033[0;0m"
  cd "$startdir"
  ./configure prefix=/opt/kde

  echo -e "\033[0;34m==>\033[0;0;1m Make \033[0;0m"
  make || return 1

  echo -e "\033[0;34m==>\033[0;0;1m Install \033[0;0m"
  DESTDIR="$startdir/pkg" make install
}
