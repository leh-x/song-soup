//lehx
//feb 23 2025
//metamod
//to change find and change the metadata and file names of a collection of  songs

#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <filesystem>
#include <unordered_set>
#include <unordered_map>

using namespace std;

#define FILE "metadata.txt"
#define NOTFOUND "NOT FOUND"
#define PATH "./"
#define UNKNOWN "Unknown"

//Album Info
class Album
{
private:
	string name;
	string year;
	string genre;

public:
	//constructors
	Album() : name("Unknown"), year("Unknown"), genre("Unknown") {}
	Album(string a) : name(a), year("Unknown"), genre("Unknown") {}
	Album(string a, string y) : name(a), year(y), genre("Unknown") {}
	Album(string a, string y, string g) : name(a), year(y), genre(g) {}

	//copy constructor (needed for unordered_map)
	Album(const Album &rhs) : name(rhs.name), year(rhs.year), genre(rhs.genre) {}

	//getters
	string getAlbum(void) 	const	{	return name;	}
	string getYear(void) 	const	{	return year;	}
	string getGenre(void) 	const	{	return genre;	}

	void setAlbum(string n)	{	name = n;	}
	void setYear(string y)	{	year = y;	}
	void setGenre(string g)	{	genre = g;	}

	//overload operator
	Album operator=(Album &rhs)
	{
		name = rhs.getAlbum();
		year = rhs.getYear();
		genre = rhs.getGenre();

		return *this;
	}

	bool operator==(const Album &rhs) const
	{
		//only compare the name
		return name == rhs.name;
	}
};

//custom hash class for finding an album in a unordered set by name
class AlbumHash
{
public:
	size_t operator()(const Album &a) const
	{
		return hash<string>()(a.getAlbum());
	}
};

class Song
{
private:
	string title;
	string artist;
	Album* album;

public:
	Song() : title("Unknown"), artist("Unknown"), album(new Album()) {}
	Song(string t, string a) : title(t), artist(a), album(new Album()) {}
	Song(string t, string a, string an) : title(t), artist(a), album(new Album(an)) {}
	Song(string t, string a, string an, string ay) : title(t), artist(a), album(new Album(an, ay)) {}
	Song(string t, string a, string an, string ay, string ag) : title(t), artist(a), album(new Album(an,ay, ag)) {}

	//copy constructor
	Song(const Song &rhs) : title(rhs.title), artist(rhs.artist), album(new Album(*rhs.album)) {}

	//getters
	string getTitle(void) 		const	{	return title;	}
	string getArtist(void) 		const	{	return artist;	}
	string getAlbumTitle(void)	const	{	return album->getAlbum();	}
	string getYear(void)		const	{	return album->getYear();	}
	string getGenre(void)		const	{	return album->getGenre();	}
	Album& getAlbum(void)		{	return *album;	}

	void setTitle(string t)	{	title = t;	}
	void setArtist(string a){	artist = a;	}
	void setAlbum(Album a)	{	*album = a;	}
	void setYear(string y)	{	album->setYear(y);	}

	~Song() { delete album; }

	bool operator==(const Song& s) const
	{
		return title == s.title;
	}
};

string findSongInDirectory(string path, string key)
{
	for(auto& file : filesystem::directory_iterator(path))
	{
		string filename = file.path().filename().string();
		if (filename.find(key) != string::npos)
		{
			return filename;
		}
	}

	return NOTFOUND;
}

class SongHash
{
public:
	size_t operator()(const Song &s) const
	{
		return hash<string>()(s.getTitle());
	}
};

int main(void)
{
	//Fill the list full of playist songs from metadata.txt
	unordered_set<Album, AlbumHash> albums;
	unordered_set<Song, SongHash> songs;

	//open metadata.txt
	ifstream data(FILE);
	string line;

	//error check
	if (!data)
	{
		cerr << "ERROR: File does not exist" << endl;
		return 1;
	}

	while(getline(data, line))
	{
		stringstream ss(line);

		string songTitle;
		string artist;
		string albumName;
		string year;
		string genre;

		//find each delimeter and set it to the correct value
		getline(ss, songTitle, '$');
		getline(ss, artist, '$');
		getline(ss, albumName, '$');
		getline(ss, year, '$');
		getline(ss, genre, '$');

		Song song(songTitle, artist, albumName, year, genre);
		Album album = song.getAlbum();

		//the following section is because I'm lazy and do not want to replace the unknown year for every album
		//if an album is already in the album list then so not add it
		string find = album.getAlbum();
		auto searchedAlbum = find_if(albums.begin(), albums.end(),[&find](const Album &a)
		{	return  a.getAlbum() == find;	});

		//found album in the set
		if (searchedAlbum != albums.end())
		{
			//if the year of the current song is unknown
			if(year == UNKNOWN)
			{
				//set the year to the found album
				year = searchedAlbum->getYear();
				song.setYear(year);
			}
		}

		//not found
		else
		{
			//if the year is not unknown and the album was not already in the list, add it
			if(year != UNKNOWN)
			{
				albums.insert(album);
			}
		}
		//insert the song into the list
		songs.insert(song);
	}

	cout << "Playlist Loaded" << endl;

	//foreach song in songs
	for (const auto& s : songs)
	{
		//store the name
		string searchSong = s.getTitle();

		//search for the song in the directory
		string found = findSongInDirectory(PATH, searchSong);

		if (found != NOTFOUND)			//no song :(
		{
			//find the '.' to isolate filetype
			int pos = found.find(".m");

			//check if the delimiter was found
			if (pos != string::npos)
			{
				//make a string starting at the delimeter position
				string filetype = found.substr(pos);

				//print what we will be changing the file song to
				//print the song info
				string newfilename = s.getTitle() + filetype;

				cout << "Found: " << found << endl;
				cout << "Changing filename to: " << newfilename << endl;

				//change the metadata
				TagLib::FileRef song(found.c_str());

				if (!song.isNull() && song.tag())
				{
					TagLib::Tag* tag = song.tag();

					if (s.getYear() != UNKNOWN)
					{
						int year = stoi(s.getYear());
						tag->setYear(year);
					}

					tag->setTitle(s.getTitle());
					tag->setArtist(s.getArtist());
					tag->setAlbum(s.getAlbumTitle());
					tag->setGenre(s.getGenre());

					song.save();	//save changes

					if (rename(found.c_str(), newfilename.c_str()) == 0)
					{
						cout << "Meta updated and renamed successfully" << endl;
					}
					else
					{
						cerr << "Error renaming file" << endl;
					}
				}
			}
		}

	}

	return 0;

}
