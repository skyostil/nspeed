@echo off
cd group
call abld build armi
cd ..
cd install
makesis game.pkg
cd ..

