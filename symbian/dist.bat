@echo off
cd group
abld build armi
cd ..
cd install
makesis game.pkg
cd ..

