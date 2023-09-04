#include "starfieldbsainvalidation.h"

#include "dummybsa.h"
#include "iplugingame.h"
#include "iprofile.h"
#include <imoinfo.h>
#include <utility.h>
#include "registry.h"

StarfieldBSAInvalidation::StarfieldBSAInvalidation(DataArchives *dataArchives, MOBase::IPluginGame const *game)
  : GamebryoBSAInvalidation(dataArchives, "StarfieldCustom.ini", game)
{
  m_IniFileName = "StarfieldCustom.ini";
  m_Game = game;
}

bool StarfieldBSAInvalidation::isInvalidationBSA(const QString& bsaName)
{
  return true;
}

QString StarfieldBSAInvalidation::invalidationBSAName() const
{
  return "";
}

unsigned long StarfieldBSAInvalidation::bsaVersion() const
{
  return 0x68;
}

bool StarfieldBSAInvalidation::prepareProfile(MOBase::IProfile* profile)
{
  bool dirty = false;
  QString basePath
    = profile->localSettingsEnabled()
    ? profile->absolutePath()
    : m_Game->documentsDirectory().absolutePath();
  QString iniFilePath = basePath + "/" + m_IniFileName;
  WCHAR setting[MAX_PATH];

  // write bInvalidateOlderFiles = 1, if needed
  if (!::GetPrivateProfileStringW(L"General", L"bEnableMessageOfTheDay", L"1", setting, MAX_PATH, iniFilePath.toStdWString().c_str())
    || wcstol(setting, nullptr, 10) != 0) {
    dirty = true;
    if (!MOBase::WriteRegistryValue(L"General", L"bEnableMessageOfTheDay", L"0", iniFilePath.toStdWString().c_str())) {
      qWarning("failed to override data directory in \"%s\"", qUtf8Printable(m_IniFileName));
    }
  }

  QString dataDirName(m_Game->documentsDirectory().absolutePath() + "/" + "Data");
  QString dataDirBackupName(profile->absolutePath() + "/" + "DocsData");
  QDir dataDir(dataDirName);
  QDir dataDirBackup(dataDirBackupName);
  if (profile->invalidationActive(nullptr)) {
    if (!::GetPrivateProfileStringW(L"Display", L"sPhotoModeFolder", L"Data\\Textures\\Photos", setting, MAX_PATH, iniFilePath.toStdWString().c_str())
      || wcscmp(setting, L"Photos") != 0) {
      dirty = true;
      if (!MOBase::WriteRegistryValue(L"Display", L"sPhotoModeFolder", L"Photos", iniFilePath.toStdWString().c_str())) {
        qWarning("failed to redirect photo directory in in \"%s\"", qUtf8Printable(m_IniFileName));
      }
    }
    if (dataDir.exists()) {
      if (dataDirBackup.exists()) {
        dataDirBackup.removeRecursively();
      }
      dataDir.rename(dataDirName, dataDirBackupName);
    }
  } else {
    if (::GetPrivateProfileStringW(L"Display", L"sPhotoModeFolder", L"Photos", setting, MAX_PATH, iniFilePath.toStdWString().c_str())
      || wcscmp(setting, L"Data\\Textures\\Photos") != 0) {
      dirty = true;
      if (!MOBase::WriteRegistryValue(L"Display", L"sPhotoModeFolder", L"Data\\Textures\\Photos", iniFilePath.toStdWString().c_str())) {
        qWarning("failed to redirect photo directory in \"%s\"", qUtf8Printable(m_IniFileName));
      }
    }
    if (dataDirBackup.exists()) {
      if (dataDir.exists()) {
        dataDir.removeRecursively();
      }
      dataDir.rename(dataDirName, dataDirBackupName);
    }
  }

  return dirty;
}