@echo off

del install\*.sis

cd group
call abld clean armi udeb
call abld build armi udeb
cd ..
cd install
makesis N-Speed-Debug.pkg
cd ..

