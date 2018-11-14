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

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
	int rc;
	char *error;

	// Open Database
	std::cout << "Opening MyDb.db ..." << std::endl;
	sqlite3 *db;
	rc = sqlite3_open("MyDb.db", &db);
	if (rc) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << std::endl << std::endl;
		sqlite3_close(db);
		return 1;
	}
	else {
		std::cout << "Opened MyDb.db." << std::endl << std::endl;
	}



	// Execute SQL
	std::cout << "Creating MyTable ..." << std::endl;
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
		std::cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << std::endl << std::endl;
		sqlite3_free(error);
	}
	else {
		std::cout << "Created MyTable." << std::endl << std::endl;
	}



	sqlite3_stmt *insertStmt;

	std::string insertSQL = "INSERT INTO songs (Title, TitleUnicode, Artist, ArtistUnicode, Creator, Version, Source, "
		"Tags, BeatmapID, BeatmapSetID, AudioFilename, AudioLeadIn, PreviewTime, HPDrainRate, CircleSize, "
		"OverallDifficulty, ApproachRate, SliderMultiplier, OsuFile, OsuDir, nHitcircles, nSlider, "
		"nSplinners) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

	rc = sqlite3_prepare_v2(db, insertSQL.c_str(), insertSQL.size(), &insertStmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << std::endl << std::endl;
		sqlite3_close(db);
		return 1;
	}
	else {
		std::cout << "Prepare Insertion SQL Finish" << std::endl << std::endl;
	}

	sqlite3_stmt *findStmt;
	std::string findSQL = "SELECT id FROM songs WHERE OsuFile = ? AND OsuDir = ?;";

	rc = sqlite3_prepare_v2(db, findSQL.c_str(), findSQL.size(), &findStmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << std::endl << std::endl;
		sqlite3_close(db);
		return 1;
	}
	else {
		std::cout << "Prepare Find SQL Finish" << std::endl << std::endl;
	}

	system("pause");

	std::string path = "D:/osu!/Songs/";
	for (const auto & beatmapSet : fs::directory_iterator(path)) {
		//std::cout << beatmapSet.path().filename().string() << std::endl;
		for (const auto & beatmap : fs::directory_iterator(beatmapSet.path())) {
			if (fs::is_regular_file(beatmap) && beatmap.path().extension() == ".osu") {
				std::string path = beatmapSet.path().filename().string();
				std::string file = beatmap.path().filename().string();

				sqlite3_bind_text(findStmt, 1, file.c_str(), file.size(), 0);
				sqlite3_bind_text(findStmt, 2, path.c_str(), path.size(), 0);

				bool willAdd = true;
				do {
					rc = sqlite3_step(findStmt);
					switch (rc) {
					case SQLITE_DONE:
						break;
					case SQLITE_ROW:
						//std::cout << sqlite3_column_text(findStmt, 0) << "  ";
						willAdd = false;
						break;
					default:
						sqlite3_finalize(findStmt);
						sqlite3_close(db);
						std::cout << "!!! ERROR !!!" << std::endl;
						return 1;
						break;
					}
				} while (rc == SQLITE_ROW);

				//std::cout << "\t" << beatmap.path().filename().string() << std::endl;

				if (willAdd) {
					std::cout << beatmap.path().filename().string() << std::endl;
					Beatmap::Beatmap bm(beatmap.path().string(), false);

					sqlite3_bind_text(insertStmt, 1, bm.Metadata.Title.c_str(), bm.Metadata.Title.size(), 0);
					sqlite3_bind_text(insertStmt, 2, bm.Metadata.TitleUnicode.c_str(), bm.Metadata.TitleUnicode.size(), 0);
					sqlite3_bind_text(insertStmt, 3, bm.Metadata.Artist.c_str(), bm.Metadata.Artist.size(), 0);
					sqlite3_bind_text(insertStmt, 4, bm.Metadata.ArtistUnicode.c_str(), bm.Metadata.ArtistUnicode.size(), 0);
					sqlite3_bind_text(insertStmt, 5, bm.Metadata.Creator.c_str(), bm.Metadata.Creator.size(), 0);
					sqlite3_bind_text(insertStmt, 6, bm.Metadata.Version.c_str(), bm.Metadata.Version.size(), 0);
					sqlite3_bind_text(insertStmt, 7, bm.Metadata.Source.c_str(), bm.Metadata.Source.size(), 0);
					sqlite3_bind_text(insertStmt, 8, bm.Metadata.Tags.c_str(), bm.Metadata.Tags.size(), 0);
					sqlite3_bind_int(insertStmt, 9, bm.Metadata.BeatmapID);
					sqlite3_bind_int(insertStmt, 10, bm.Metadata.BeatmapSetID);

					sqlite3_bind_text(insertStmt, 11, bm.General.AudioFilename.c_str(), bm.General.AudioFilename.size(), 0);
					sqlite3_bind_int(insertStmt, 12, bm.General.AudioLeadIn);
					sqlite3_bind_int(insertStmt, 13, bm.General.PreviewTime);

					sqlite3_bind_double(insertStmt, 14, bm.Difficulty.HPDrainRate);
					sqlite3_bind_double(insertStmt, 15, bm.Difficulty.CircleSize);
					sqlite3_bind_double(insertStmt, 16, bm.Difficulty.OverallDifficulty);
					sqlite3_bind_double(insertStmt, 17, bm.Difficulty.ApproachRate);
					sqlite3_bind_double(insertStmt, 18, bm.Difficulty.SliderMultiplier);

					sqlite3_bind_text(insertStmt, 19, file.c_str(), file.size(), 0);
					sqlite3_bind_text(insertStmt, 20, path.c_str(), path.size(), 0);
					sqlite3_bind_int(insertStmt, 21, bm.nHitcircles);
					sqlite3_bind_int(insertStmt, 22, bm.nSlider);
					sqlite3_bind_int(insertStmt, 23, bm.nSplinners);

					if (sqlite3_step(insertStmt) != SQLITE_DONE) {
						sqlite3_finalize(insertStmt);
						sqlite3_close(db);

						std::cout << "!!! ERROR !!!" << std::endl;

						return 1;
					}

					sqlite3_reset(insertStmt);
					sqlite3_clear_bindings(insertStmt);
				}
				

				sqlite3_reset(findStmt);
				sqlite3_clear_bindings(findStmt);
			}
		}
		//std::cout << std::endl;
	}
	sqlite3_finalize(findStmt);
	sqlite3_finalize(insertStmt);
	
	system("pause");
	//return 0;


	// Display MyTable
	std::cout << "Retrieving values in MyTable ..." << std::endl;
	const char *sqlSelect = "SELECT * FROM songs;";
	char **results = NULL;
	int rows, columns;
	sqlite3_get_table(db, sqlSelect, &results, &rows, &columns, &error);
	//if (rc) {
	//	std::cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << std::endl << std::endl;
	//	sqlite3_free(error);
	//}
	//else {
		// Display Table
		for (int rowCtr = 0; rowCtr <= rows; ++rowCtr) {
			for (int colCtr = 0; colCtr < columns; ++colCtr) {
				// Determine Cell Position
				int cellPosition = (rowCtr * columns) + colCtr;

				// Display Cell Value
				//std::cout.width(12);
				//std::cout.setf(std::ios::left);
				//std::cout << results[cellPosition] << " ";
			}

			// End Line
			//std::cout << std::endl;

			// Display Separator For Header
			if (0 == rowCtr) {
				for (int colCtr = 0; colCtr < columns; ++colCtr) {
					//std::cout.width(12);
					//std::cout.setf(std::ios::left);
					//std::cout << "~~~~~~~~~~~~ ";
				}
				//std::cout << std::endl;
			}
		}
	//}
	sqlite3_free_table(results);

	// Close Database
	std::cout << "Closing MyDb.db ..." << std::endl;
	sqlite3_close(db);
	std::cout << "Closed MyDb.db" << std::endl << std::endl;

	// Wait For User To Close Program
	std::cout << "Please press any key to exit the program ..." << std::endl;
	std::cin.get();

	return 0;
}
