#ifndef STARFIELDSAVEGAME_H
#define STARFIELDSAVEGAME_H

#include "gamebryosavegame.h"
#include <QDataStream>

#include <Windows.h>

class GameStarfield;

class StarfieldSaveGame : public GamebryoSaveGame
{
public:
  StarfieldSaveGame(QString const& fileName, GameStarfield const* game);

protected:
  // Fetch easy-to-access information.
  void getData(FileWrapper& file) const;

  void fetchInformationFields(FileWrapper& file, unsigned long& saveNumber,
                              QString& playerName, unsigned short& playerLevel,
                              QString& playerLocation, FILETIME& creationTime) const;

  std::unique_ptr<DataFields> fetchDataFields() const override;
};

#endif  // STARFIELDSAVEGAME_H
