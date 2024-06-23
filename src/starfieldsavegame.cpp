#include "starfieldsavegame.h"

#include <Windows.h>

#include "gamestarfield.h"

StarfieldSaveGame::StarfieldSaveGame(QString const& fileName, GameStarfield const* game)
    : GamebryoSaveGame(fileName, game, true, true)
{
  FileWrapper file(getFilepath(), "BCPS");

  getData(file);
  FILETIME creationTime;
  unsigned char saveVersion;
  fetchInformationFields(file, m_SaveNumber, saveVersion, m_PCName, m_PCLevel,
                         m_PCLocation, creationTime);
  file.closeCompressedData();
  file.close();

  // A file time is a 64-bit value that represents the number of 100-nanosecond
  // intervals that have elapsed since 12:00 A.M. January 1, 1601 Coordinated Universal
  // Time (UTC). So we need to convert that to something useful
  SYSTEMTIME ctime;
  ::FileTimeToSystemTime(&creationTime, &ctime);

  setCreationTime(ctime);
}

void StarfieldSaveGame::getData(FileWrapper& file) const
{
  file.skip<uint32_t>();  // header version
  file.skip<uint64_t>();  // chunk compressed size array start location
  file.skip<uint64_t>();  // unknown (0?)
  file.setCompressionType(1);
  /*
   * Parse following variables then begin decompressing data
   * - 64-bit int = compressed data start location
   * - 64-bit int = complete uncompressed data size
   */
  file.openCompressedData();
  /*
   * Remaining headers before start of compressed data:
   * - 32-bit float (version? appears to be 2.0)
   * - 64-bit int - size of uncompressed chunks (250 KiB)
   * - 64-bit int - size of byte rows? (16 bytes) used to determine start of each
   *   compressed chunk
   * - 32-bit int - number of chunks?
   * - 'ZIP ' - denotes start of chunk compressed size array
   * - compressed size array - array of 32-bit ints containing the compressed size of
   *   each compressed chunk (see number of chunks above)
   */
}

void StarfieldSaveGame::fetchInformationFields(
    FileWrapper& file, unsigned long& saveNumber, unsigned char& saveVersion,
    QString& playerName, unsigned short& playerLevel, QString& playerLocation,
    FILETIME& creationTime) const
{
  char fileID[12];  // SFS_SAVEGAME
  unsigned int headerSize;
  unsigned int version;
  // file.read(fileID, 12);
  headerSize  = file.readInt(12);
  version     = file.readInt();
  saveVersion = file.readChar();
  saveNumber  = file.readInt();
  file.read(playerName);

  unsigned int temp;
  temp        = file.readInt();
  playerLevel = static_cast<unsigned short>(temp);
  file.read(playerLocation);

  QString ignore;
  file.read(ignore);  // playtime as ascii hh.mm.ss
  file.read(ignore);  // race name (i.e. BretonRace)

  unsigned short gender;
  gender = file.readShort();  // Player gender (0 = male)
  float experience, experienceRequired;
  experience         = file.readFloat();
  experienceRequired = file.readFloat();

  unsigned long long time     = file.readLong();
  creationTime.dwLowDateTime  = (DWORD)time;
  creationTime.dwHighDateTime = time >> 32;
}

std::unique_ptr<GamebryoSaveGame::DataFields> StarfieldSaveGame::fetchDataFields() const
{
  FileWrapper file(getFilepath(), "BCPS");  // 10bytes

  getData(file);
  FILETIME creationTime;
  unsigned char saveVersion;

  {
    QString dummyName, dummyLocation;
    unsigned short dummyLevel;
    unsigned long dummySaveNumber;
    FILETIME dummyTime;

    fetchInformationFields(file, dummySaveNumber, saveVersion, dummyName, dummyLevel,
                           dummyLocation, dummyTime);
  }

  bool extraInfo          = saveVersion >= 122;
  QStringList gamePlugins = m_Game->primaryPlugins() + m_Game->enabledPlugins();

  QString ignore;
  std::unique_ptr<DataFields> fields = std::make_unique<DataFields>();

  file.readChar(12);
  file.read(ignore);  // game version
  file.read(ignore);  // game version again?
  file.readInt();     // plugin info size

  fields->Plugins      = file.readPlugins(0, extraInfo, gamePlugins);
  fields->LightPlugins = file.readLightPlugins(0, extraInfo, gamePlugins);
  if (saveVersion >= 122)
    fields->MediumPlugins = file.readMediumPlugins(0, extraInfo, gamePlugins);
  file.closeCompressedData();
  file.close();

  return fields;
}
