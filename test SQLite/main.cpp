/*
 *
 *	https://dcravey.wordpress.com/2011/03/21/using-sqlite-in-a-visual-c-application/
 *
 */

#include <ios>
#include <iostream>
#include <string>
#include <iomanip>
#include <unordered_map>
#include <filesystem>
#include "sqlite3.h"

#include "Beatmap.h"

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
	int rc;
	char *error;

	// Open Database
	cout << "Opening MyDb.db ..." << endl;
	sqlite3 *db;
	rc = sqlite3_open("MyDb.db", &db);
	if (rc) {
		cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_close(db);
		return 1;
	}
	else {
		cout << "Opened MyDb.db." << endl << endl;
	}



	// Execute SQL
	cout << "Creating MyTable ..." << endl;
	const char *sqlCreateTable = 
	"CREATE TABLE songs ("
		"id	INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,"
		"Title	TEXT,"
		"TitleUnicode	TEXT,"
		"Artist	TEXT,"
		"ArtistUnicode	TEXT,"
		"Creator	TEXT,"
		"Version	TEXT,"
		"Source	TEXT,"
		"Tags	TEXT,"
		"BeatmapID	NUMERIC,"
		"BeatmapSetID	NUMERIC,"
		"AudioFilename	TEXT,"
		"AudioLeadIn	NUMERIC,"
		"PreviewTime	NUMERIC,"
		"HPDrainRate	REAL,"
		"CircleSize	REAL,"
		"OverallDifficulty	REAL,"
		"ApproachRate	REAL,"
		"SliderMultiplier	REAL,"
		"OsuFile	TEXT,"
		"OsuDir	TEXT,"
		"nHitcircles	NUMERIC,"
		"nSlider	NUMERIC,"
		"nSplinners	NUMERIC"
	");";
	rc = sqlite3_exec(db, sqlCreateTable, NULL, NULL, &error);
	if (rc) {
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else {
		cout << "Created MyTable." << endl << endl;
	}



	sqlite3_stmt *stmt;
	const char *pzTest;

	std::string szSQL = "INSERT INTO songs (Title, TitleUnicode, Artist, ArtistUnicode, Creator, Version, Source, "
		"Tags, BeatmapID, BeatmapSetID, AudioFilename, AudioLeadIn, PreviewTime, HPDrainRate, CircleSize, "
		"OverallDifficulty, ApproachRate, SliderMultiplier, OsuFile, OsuDir, nHitcircles, nSlider, "
		"nSplinners) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

	rc = sqlite3_prepare_v2(db, szSQL.c_str(), szSQL.size(), &stmt, &pzTest);

	std::string path = "D:/osu!/Songs/";
	for (const auto & beatmapSet : fs::directory_iterator(path)) {
		std::cout << beatmapSet.path().filename().string() << std::endl;
		for (const auto & beatmap : fs::directory_iterator(beatmapSet.path())) {
			if (fs::is_regular_file(beatmap) && beatmap.path().extension() == ".osu") {
				std::cout << "\t" << beatmap.path().filename().string() << std::endl;

				Beatmap::Beatmap bm(beatmap.path().string());
				
				sqlite3_bind_text(stmt, 1, bm.Metadata.Title.c_str(),			bm.Metadata.Title.size(), 0);
				sqlite3_bind_text(stmt, 2, bm.Metadata.TitleUnicode.c_str(),	bm.Metadata.TitleUnicode.size(), 0);
				sqlite3_bind_text(stmt, 3, bm.Metadata.Artist.c_str(),			bm.Metadata.Artist.size(), 0);
				sqlite3_bind_text(stmt, 4, bm.Metadata.ArtistUnicode.c_str(),	bm.Metadata.ArtistUnicode.size(), 0);
				sqlite3_bind_text(stmt, 5, bm.Metadata.Creator.c_str(),			bm.Metadata.Creator.size(), 0);
				sqlite3_bind_text(stmt, 6, bm.Metadata.Version.c_str(),			bm.Metadata.Version.size(), 0);
				sqlite3_bind_text(stmt, 7, bm.Metadata.Source.c_str(),			bm.Metadata.Source.size(), 0);
				sqlite3_bind_text(stmt, 8, bm.Metadata.Tags.c_str(),			bm.Metadata.Tags.size(), 0);
				sqlite3_bind_int(stmt, 9, bm.Metadata.BeatmapID);
				sqlite3_bind_int(stmt, 10, bm.Metadata.BeatmapSetID);

				sqlite3_bind_text(stmt, 11, bm.General.AudioFilename.c_str(),	bm.General.AudioFilename.size(), 0);
				sqlite3_bind_int(stmt, 12, bm.General.AudioLeadIn);
				sqlite3_bind_int(stmt, 13, bm.General.PreviewTime);

				sqlite3_bind_double(stmt, 14, bm.Difficulty.HPDrainRate);
				sqlite3_bind_double(stmt, 15, bm.Difficulty.CircleSize);
				sqlite3_bind_double(stmt, 16, bm.Difficulty.OverallDifficulty);
				sqlite3_bind_double(stmt, 17, bm.Difficulty.ApproachRate);
				sqlite3_bind_double(stmt, 18, bm.Difficulty.SliderMultiplier);

				std::string path = beatmapSet.path().filename().string();
				std::string file = beatmap.path().filename().string();

				sqlite3_bind_text(stmt, 19, file.c_str(), file.size(), 0);
				sqlite3_bind_text(stmt, 20, path.c_str(), path.size(), 0);
				sqlite3_bind_int(stmt, 21, bm.nHitcircles);
				sqlite3_bind_int(stmt, 22, bm.nSlider);
				sqlite3_bind_int(stmt, 23, bm.nSplinners);

				if (rc != SQLITE_OK) {
					std::cout << "!!! ERROR !!!" << std::endl;

					return 1;
				}

				if (sqlite3_step(stmt) != SQLITE_DONE) {
					sqlite3_finalize(stmt);
					sqlite3_close(db);

					std::cout << "!!! ERROR !!!2" << std::endl;

					return 1;
				}

				sqlite3_reset(stmt);
				sqlite3_clear_bindings(stmt);
			}
		}
	}
	
	system("pause");
	//return 0;
	

	

	// Execute SQL
	cout << "Inserting a value into MyTable ..." << endl;
	const char *sqlInsert = "INSERT INTO MyTable VALUES(NULL, 'A Value');";
	rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
	if (rc) {
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else {
		cout << "Inserted a value into MyTable." << endl << endl;
	}

	system("pause");
	return 0;

	// Display MyTable
	cout << "Retrieving values in MyTable ..." << endl;
	const char *sqlSelect = "SELECT * FROM MyTable;";
	char **results = NULL;
	int rows, columns;
	sqlite3_get_table(db, sqlSelect, &results, &rows, &columns, &error);
	if (rc) {
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else {
		// Display Table
		for (int rowCtr = 0; rowCtr <= rows; ++rowCtr) {
			for (int colCtr = 0; colCtr < columns; ++colCtr) {
				// Determine Cell Position
				int cellPosition = (rowCtr * columns) + colCtr;

				// Display Cell Value
				cout.width(12);
				cout.setf(ios::left);
				cout << results[cellPosition] << " ";
			}

			// End Line
			cout << endl;

			// Display Separator For Header
			if (0 == rowCtr) {
				for (int colCtr = 0; colCtr < columns; ++colCtr) {
					cout.width(12);
					cout.setf(ios::left);
					cout << "~~~~~~~~~~~~ ";
				}
				cout << endl;
			}
		}
	}
	sqlite3_free_table(results);

	// Close Database
	cout << "Closing MyDb.db ..." << endl;
	sqlite3_close(db);
	cout << "Closed MyDb.db" << endl << endl;

	// Wait For User To Close Program
	cout << "Please press any key to exit the program ..." << endl;
	cin.get();

	return 0;
}
