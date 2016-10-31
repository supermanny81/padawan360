#$/bin/sh

rm normalized*mp3
rm mp3trigger/TRACK*
ffmpeg-normalize -u -v -f -alibmp3lame -e "-b:a 192k" -m -l 0  *.mp3

# get the defualt file if it dosen't already exist
if [ ! -f normalized-240* ]; then
  ffmpeg -i 240\ *.mp3 -b:a 192k normalized-240.mp3
fi 

for i in $( seq -w  1 255 ); do 
  echo "Copying sound file $i..."
  if [ -f $i*.mp3 ]; then 
    if [ ! -f normalized-$i* ]; then
        ffmpeg -i $i*.mp3 -b:a 192k normalized-$i.mp3
    fi
    cp normalized-$i* mp3trigger/TRACK$i.MP3 
  else
    cp normalized-240\ proc-1.mp3 mp3trigger/TRACK$i.MP3
  fi
  sleep 1s  
done

rm normalized*mp3
