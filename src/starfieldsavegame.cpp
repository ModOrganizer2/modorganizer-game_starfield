#include "starfieldsavegame.h"

#include <Windows.h>

#include "gamestarfield.h"

StarfieldSaveGame::StarfieldSaveGame(QString const &fileName, GameStarfield const* game) :
  GamebryoSaveGame(fileName, game, true)
{
  FileWrapper file(getFilepath(), "BCPS");

  getData(file);
  FILETIME creationTime;
  fetchInformationFields(file, m_SaveNumber, m_PCName, m_PCLevel, m_PCLocation, creationTime);
  file.closeCompressedData();
  file.close();

  //A file time is a 64-bit value that represents the number of 100-nanosecond
  //intervals that have elapsed since 12:00 A.M. January 1, 1601 Coordinated Universal Time (UTC).
  //So we need to convert that to something useful
  SYSTEMTIME ctime;
  ::FileTimeToSystemTime(&creationTime, &ctime);

  setCreationTime(ctime);
}

void StarfieldSaveGame::getData(
    FileWrapper& file) const
{
    file.skip<uint32_t>(); // header version
    file.skip<uint64_t>(); // zip start location
    file.skip<uint64_t>(); // unknown
    file.setCompressionType(1);
    file.openCompressedData(); // long = start, long = size
    // double
    // float
    // long
    // long
    // short
    return;
}

void StarfieldSaveGame::fetchInformationFields(
    FileWrapper& file,
    unsigned long& saveNumber,
    QString& playerName,
    unsigned short& playerLevel,
    QString& playerLocation,
    FILETIME& creationTime) const
{
  char fileID[12]; // SFS_SAVEGAME
  unsigned int headerSize;
  unsigned int version;
  unsigned char unknown;
  //file.read(fileID, 12);
  headerSize = file.readInt(12);
  version = file.readInt();
  unknown = file.readChar();
  saveNumber = file.readInt();
  file.read(playerName);

  unsigned int temp;
  temp = file.readInt();
  playerLevel = static_cast<unsigned short>(temp);
  file.read(playerLocation);

  QString ignore;
  file.read(ignore);   // playtime as ascii hh.mm.ss
  file.read(ignore);   // race name (i.e. BretonRace)

  unsigned short gender;
  gender = file.readShort(); // Player gender (0 = male)
  float experience, experienceRequired;
  experience = file.readFloat();
  experienceRequired = file.readFloat();

  unsigned long long time = file.readLong();
  creationTime.dwLowDateTime = (DWORD)time;
  creationTime.dwHighDateTime = time >> 32;
}

std::unique_ptr<GamebryoSaveGame::DataFields> StarfieldSaveGame::fetchDataFields() const
{
    FileWrapper file(getFilepath(), "BCPS"); //10bytes

    getData(file);
    FILETIME creationTime;

    {
        QString dummyName, dummyLocation;
        unsigned short dummyLevel;
        unsigned long dummySaveNumber;
        FILETIME dummyTime;

        fetchInformationFields(file, dummySaveNumber, dummyName, dummyLevel,
            dummyLocation, dummyTime);
    }

    QString ignore;
    std::unique_ptr<DataFields> fields = std::make_unique<DataFields>();

    //fields->Screenshot = file.readImage(384, true);

    uint8_t saveGameVersion = file.readChar(12);
    file.read(ignore);     // game version
    file.read(ignore);     // game version again?
    file.readInt();        // plugin info size

    fields->Plugins = file.readPlugins();
    if (saveGameVersion >= 82) {
        fields->LightPlugins = file.readLightPlugins();
    }
    file.closeCompressedData();
    file.close();

    return fields;
}
