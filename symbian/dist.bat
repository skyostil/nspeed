@echo off

del install\*.sis

cd group
call abld build armi
cd ..
cd install
makesis N-Speed.pkg
makesis Car1.pkg
makesis Car2.pkg
makesis Car3.pkg
makesis Car4.pkg
makesis GameTrack.pkg
makesis TestTrack.pkg
makesis IceTrack.pkg
makesis DesertTrack.pkg
cd ..

