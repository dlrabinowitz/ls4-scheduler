#!/usr/bin/tcsh
## setup up links for /home/observer in order to
# run observing operations
#
#
set LS4_HOME = /home/ls4
set LS4_SRC = $LS4_HOME/testing

# remove linked files and directories
#
set remove_list = ( ".tcshrc" ".bashrc" "README.observer" "quest-src-lasilla" "questlib" "scheduler"  "observer_venv" "ls4_control" "bin" )

cd $LS4_ROOT
foreach file ($remove_list)
  echo "removing $file"
  rm -r $file
end

# link observer_conf entrries from ls4-scheduler
cd $LS4_ROOT
set link_src = ( "observer_tcshrc" "observer_bashrc" "README.observer" )
set link_dest = ( ".tcshrc" ".bashrc" "README.observer" )
set i = 0
foreach file ($link_src)
  @ i = $i + 1
  echo "linking $LS4_SRC/ls4-scheduler/observer_conf/$file to  $link_dest[$i]"
  ln -s $LS4_SRC/ls4-scheduler/observer_conf/$file $link_dest[$i]
end


# link ls4 source directories
cd $LS4_ROOT
set link_src = ( "quest-src-lasilla" "ls4-scheduler" "ls4_control" )
set link_dest = ( "quest-src-lasilla" "scheduler" "ls4_control" )
set i = 0
foreach file ($link_src)
  @ i = $i + 1
  echo "linking $LS4_SRC/$file to  $link_dest[$i]"
  ln -s $LS4_SRC/$file $link_dest[$i]
end

# link questlib and observer_venv
cd $LS4_ROOT
echo "linking quest-src-lasilla/questlib to questlib"
ln -s ./quest-src-lasilla/questlib questlib

echo "linking $LS4_HOME/observer_venv to observer_venv"
ln -s  $LS4_HOME/observer_venv ./observer_venv

# link programs and scripts to bin
cd $LS4_ROOT
mkdir bin
cd bin

set bin_src = ( "quest-src-lasilla/bin" "ls4-scheduler/scripts" "ls4-scheduler/bin" )
foreach file ($bin_src)
  echo "linking $file entries to  bin"
  ln -s $LS4_SRC/$file/* .
end


# mkdir logs and obsplans if they do not already exist
cd $LS4_ROOT
if ( ! -e logs ) mkdir logs
if ( ! -e obsplans ) mkdir obsplans


