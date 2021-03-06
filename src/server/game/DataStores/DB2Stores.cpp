/*
* Copyright (C) 2011 TrintiyCore <http://www.trinitycore.org/>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "DB2fmt.h"
#include "DB2Stores.h"
#include "Common.h"
#include "Log.h"
#include "SharedDefines.h"
#include "SpellMgr.h"
#include "DBCStores.h"

#include <map>

DB2Storage <BroadcastTextEntry> sBroadcastTextStore(BroadcastTextfmt/*, &DB2Utilities::HasBroadcastTextEntry, &DB2Utilities::WriteBroadcastTextDbReply*/);
DB2Storage <ItemEntry> sItemStore(Itemfmt);
DB2Storage <ItemCurrencyCostEntry> sItemCurrencyCostStore(ItemCurrencyCostfmt);
DB2Storage <ItemExtendedCostEntry> sItemExtendedCostStore(ItemExtendedCostEntryfmt);
DB2Storage <ItemSparseEntry> sItemSparseStore(ItemSparsefmt);
DB2Storage <KeyChainEntry> sKeyChainStore(KeyChainfmt);
DB2Storage <QuestPackageItemEntry> sQuestPackageItemStore(QuestPackageItemfmt);
//DB2Storage <BattlePetSpeciesEntry> sBattlePetSpeciesStore(BattlePetSpeciesEntryfmt);
DB2Storage <SpellReagentsEntry> sSpellReagentsStore(SpellReagentsfmt);

typedef std::list<std::string> StoreProblemList1;

typedef std::map<uint32 /*hash*/, DB2StorageBase*> DB2StorageMap;
DB2StorageMap DB2Stores;

uint32 DB2FilesCount = 0;

static bool LoadDB2_assert_print(uint32 fsize, uint32 rsize, const std::string& filename)
{
	sLog->outError(LOG_FILTER_GENERAL, "Size of '%s' setted by format string (%u) not equal size of C++ structure (%u).", filename.c_str(), fsize, rsize);

	// ASSERT must fail after function call
	return false;
}

template<class T>
inline void LoadDB2(uint32& availableDb2Locales, StoreProblemList1& errlist, DB2Storage<T>& storage, const std::string& db2_path, const std::string& filename)
{
	// compatibility format and C++ structure sizes
	ASSERT(DB2FileLoader::GetFormatRecordSize(storage.GetFormat()) == sizeof(T) || LoadDB2_assert_print(DB2FileLoader::GetFormatRecordSize(storage.GetFormat()), sizeof(T), filename));

	++DB2FilesCount;

	std::string db2_filename = db2_path + filename;
	if (storage.Load(db2_filename.c_str(), uint32(sWorld->GetDefaultDbcLocale())))
	{
		for (uint32 i = 0; i < TOTAL_LOCALES; ++i)
		{
			if (!(availableDb2Locales & (1 << i)))
				continue;

			if (uint32(sWorld->GetDefaultDbcLocale()) == i)
				continue;

			std::string localizedName(db2_path);
			localizedName.append(localeNames[i]);
			localizedName.push_back('/');
			localizedName.append(filename);

			if (!storage.LoadStringsFrom(localizedName.c_str(), i))
				availableDb2Locales &= ~(1 << i);             // mark as not available for speedup next checks
		}
	}
	else
	{
		// sort problematic db2 to (1) non compatible and (2) nonexistent
		if (FILE* f = fopen(db2_filename.c_str(), "rb"))
		{
			char buf[100];
			snprintf(buf, 100, "(exist, but have %d fields instead " SIZEFMTD ") Wrong client version DBC file?", storage.GetFieldCount(), strlen(storage.GetFormat()));
			errlist.push_back(db2_filename + buf);
			fclose(f);
		}
		else
			errlist.push_back(db2_filename);
	}

	DB2Stores[storage.GetHash()] = &storage;
}

void LoadDB2Stores(const std::string& dataPath)
{
	std::string db2Path = dataPath + "dbc/";

	StoreProblemList1 bad_db2_files;
	uint32 availableDb2Locales = 0xFF;

	LoadDB2(availableDb2Locales, bad_db2_files, sBroadcastTextStore, db2Path, "BroadcastText.db2");
	//LoadDB2(availableDb2Locales, bad_db2_files, sBattlePetSpeciesStore, db2Path, "BattlePetSpecies.db2");
	LoadDB2(availableDb2Locales, bad_db2_files, sItemStore, db2Path, "Item.db2");
	LoadDB2(availableDb2Locales, bad_db2_files, sItemCurrencyCostStore, db2Path, "ItemCurrencyCost.db2");
	LoadDB2(availableDb2Locales, bad_db2_files, sItemSparseStore, db2Path, "Item-sparse.db2");
	LoadDB2(availableDb2Locales, bad_db2_files, sItemExtendedCostStore, db2Path, "ItemExtendedCost.db2");
	LoadDB2(availableDb2Locales, bad_db2_files, sKeyChainStore, db2Path, "KeyChain.db2");
	LoadDB2(availableDb2Locales, bad_db2_files, sQuestPackageItemStore, db2Path, "QuestPackageItem.db2");
	LoadDB2(availableDb2Locales, bad_db2_files, sSpellReagentsStore, db2Path, "SpellReagents.db2");

	// error checks
	if (bad_db2_files.size() >= DB2FilesCount)
	{
		sLog->outError(LOG_FILTER_GENERAL, "\nIncorrect DataDir value in worldserver.conf or ALL required *.db2 files (%d) not found by path: %sdb2", DB2FilesCount, dataPath.c_str());
		exit(1);
	}
	else if (!bad_db2_files.empty())
	{
		std::string str;
		for (std::list<std::string>::iterator i = bad_db2_files.begin(); i != bad_db2_files.end(); ++i)
			str += *i + "\n";

		sLog->outError(LOG_FILTER_GENERAL, "\nSome required *.db2 files (%u from %d) not found or not compatible:\n%s", (uint32)bad_db2_files.size(), DB2FilesCount, str.c_str());
		exit(1);
	}

	// Check loaded DB2 files proper version
	if (!sBroadcastTextStore.LookupEntry(77161) ||        // last broadcast text added in 5.4.8 (18414)
		!sItemStore.LookupEntry(112353) ||                // last item added in 5.4.8 (18414)
		!sItemExtendedCostStore.LookupEntry(5280) ||      // last item extended cost added in 5.4.8 (18414)
		!sQuestPackageItemStore.LookupEntry(2256))        // last quest package item in 5.4.8 (18414)
	{
		sLog->outError(LOG_FILTER_GENERAL, "Please extract correct db2 files from client 5.4.8 18414.");
		exit(1);
	}

	sLog->outInfo(LOG_FILTER_GENERAL, ">> Initialized %d DB2 data stores.", DB2FilesCount);
}

DB2StorageBase const* GetDB2Storage(uint32 type)
{
	DB2StorageMap::const_iterator itr = DB2Stores.find(type);
	if (itr != DB2Stores.end())
		return itr->second;

	return NULL;
}