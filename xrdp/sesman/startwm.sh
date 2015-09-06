#!/bin/sh

# change the order in line below to run to run whatever window manager you
# want, default to kde
export XRDP_PROCESS=1
SESSIONS="x-session-manager x-window-manager"

DefaultShell=$1

# change PATH to be what your environment needs usually what is in
# /etc/environment
#PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games"
#export PATH=$PATH

# for PATH and LANG from /etc/environment
# pam will auto process the environment file if /etc/pam.d/xrdp-sesman
# includes
# auth       required     pam_env.so readenv=1
#. /etc/environment
#export PATH=$PATH
#export LANG=$LANG

# for bash profile
#. ~/.bash_profile

#. /etc/profile

xsetroot -solid white
which "$DefaultShell"
if test $? -eq 0
then
  echo "Starting $DefaultShell"
  $DefaultShell
  exit 0
else
  echo "$DefaultShell did not exist"
fi

for WindowManager in $SESSIONS
do
  which $WindowManager
  if test $? -eq 0
  then
    echo "Starting $WindowManager"
    $WindowManager
    exit 0
  fi
done

exit 1
