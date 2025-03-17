# song-soup for ubuntu
A handy (but slightly inconvenitent) tool for fixing up your totally legally acquired music library. Tweak tags, clean up track info, and make your collection feel ✨official✨ all at once. It's giving 2006 energy. 🔥 

## running
**Before you run. You'll need to make sure these things are in place:**
  The songs you want to update are all in the same folder as metamod.cpp. You'll also need the metadata.txt file to be filled with the song info you're looking to update. 
  The metadata.txt contents should be structured like this with each song on a new line:
  Song Name$Song Artist$Album$Year&Genre

There are online tools to export playlist info into an excel. From there the rows can be shifted into this order and exported into a .txt file. A text editor like vscode is great for replacing tabs from the exported file with delimeters.

**Additional notes: **To find a song in the directory, it only checks if the song name contains a partial match in the file name. This can lead to problems for songs that share the same name.

Once all this is in place these are the commands to run the script.

1. you'll need the gcc, g++, and nano (I think) installed
2. install taglib library. Copy this into the terminal sudo apt-get install libtag1-dev
3. navigate to where the metamod.cpp is and open the terminal in that directory.
4. compile the script with the taglib library. Copy this into the terminal g++ -o metamod metamod.cpp -ltag
5. run the script. ./metamod
