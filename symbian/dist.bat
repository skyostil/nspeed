@echo off
cd group
call abld build armi
cd ..
cd install
makesis n-speed.pkg
cd ..

