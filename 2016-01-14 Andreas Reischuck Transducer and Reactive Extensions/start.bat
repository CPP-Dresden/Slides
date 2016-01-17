@echo off
cd %~dp0
vagrant up

start "rsync auto" vagrant rsync-auto
vagrant ssh -c /vagrant/guard.sh
