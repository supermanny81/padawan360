#$/bin/sh

rm normalized-*wav
rm wavtrigger/TRACK*
ffmpeg-normalize -v -f *.mp3

# get the defualt file if it dosen't already exist
if [ ! -f normalized-240.wav ]; then
  ffmpeg -i 240\ *.mp3 normalized-240.wav
fi 

for i in $( seq -w  1 255 ); do 
  echo "Copying sound file $i..."
  if [ -f $i*.mp3 ] && [ $i != 240 ]; then 
    if [ ! -f normalized-$i* ]; then
        ffmpeg -i $i*.mp3 normalized-$i.wav
    fi
    cp normalized-$i*wav wavtrigger/$i.wav
  else
    cp normalized-240.wav wavtrigger/$i.wav
  fi
  #sleep 1s  
done

rm normalized-*wav
