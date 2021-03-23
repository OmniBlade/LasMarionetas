/**
 * @file
 *
 * @brief Collection of known class IDs, may be moved to more appropriate locations later.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once

#include <stdint.h>

// Common IDs shared between GZ/RZ applications from macOS symbol table.
static constexpr uint32_t kGZCLSID_cGZWinCustom = 0x478D1E6F;
static constexpr uint32_t kGZCLSID_cGZWinGrid = 0xDAA6B9BF;
static constexpr uint32_t kGZCLSID_cGZWinFolders = 0x78E8055;
static constexpr uint32_t kGZCLSID_cGZWinTreeView = 0x3AE8BAE1;
static constexpr uint32_t kGZCLSID_cGZWinFileBrowser = 0x1AA52EA4;
static constexpr uint32_t kGZCLSID_cGZWinGen = 0x4386D516;
static constexpr uint32_t kGZCLSID_cGZWinTextTicker = 0xE32F0B31;
static constexpr uint32_t kGZCLSID_cGZWinOutline = 0x4303E5B6;
static constexpr uint32_t kGZCLSID_cGZWinSpinner = 0x612CE0C4;
static constexpr uint32_t kGZCLSID_cGZWinOptGrp = 0xA1336CC1;
static constexpr uint32_t kGZCLSID_cGZWinLineInput = 0x21335C5A;
static constexpr uint32_t kGZCLSID_cGZWinScrollbar2 = 0x4B42DA5D;
static constexpr uint32_t kGZCLSID_cGZWinScrollbar = 0x61325A2E;
static constexpr uint32_t kGZCLSID_cGZWinSlider = 0x21325208;
static constexpr uint32_t kGZCLSID_cGZWinBMP = 0x82FE68C4;
static constexpr uint32_t kGZCLSID_cGZWinBtn = 0x22ECFC47;
static constexpr uint32_t kGZCLSID_cGZWinCombo = 0x0000059B;
static constexpr uint32_t kGZCLSID_cGZWinListBox = 0x00000598;
static constexpr uint32_t kGZCLSID_cGZWinTextEdit = 0x231A1C58;
static constexpr uint32_t kGZCLSID_cGZWinText = 0x00000592;
static constexpr uint32_t kGZCLSID_cGZWinFlatRect = 0x0C2AFA76E;

// Common IDs shared between GZ/RZ applications that are literals.
static constexpr uint32_t kGZCLSID_cGZDBSerialRecordAsIGZFile = 0xA32C0DA;

// TS2 specific class IDs
static constexpr uint32_t kGZCLSID_cHitList = 0x000000CD;
static constexpr uint32_t kGZCLSID_cTSNeighborGeneticDataTable = 0x2C310F46;
static constexpr uint32_t kGZCLSID_cTSRelationshipTable = 0xCC364C2A;
static constexpr uint32_t kGZCLSID_cTSRelationshipEntry = 0xCC364C18;
static constexpr uint32_t kGZCLSID_cTSFamily = 0x46414D49;
static constexpr uint32_t kGZCLSID_cTSNeighbor = 0xAACE2EFB;
static constexpr uint32_t kGZCLSID_cTSNeighborManager = 0xCC2A6A34;
static constexpr uint32_t kGZCLSID_cTSAudioSoundPlaylist = 0x000000CD;
static constexpr uint32_t kGZCLSID_cTSBinInfo = 0x6C4F359D;
static constexpr uint32_t kGZCLSID_cTSSimFragment = 0x0C560F39;
static constexpr uint32_t kGZCLSID_cTSSkinEntry = 0xEBCF3E27;
static constexpr uint32_t kGZCLSID_cEdithObjectDefinitionResource = 0x4F424A44;
static constexpr uint32_t kGZCLSID_cBHTree = 0x54524545;
static constexpr uint32_t kGZCLSID_cTSSlots = 0x534C4F54;
static constexpr uint32_t kGZCLSID_cTSBehavior = 0x1B269450;
static constexpr uint32_t kGZCLSID_cTSBehaviorConstants = 0x42434F4E;
static constexpr uint32_t kGZCLSID_cTSBehaviorTree = 0x42484156;
static constexpr uint32_t kGZCLSID_cEdithObjectFunctionTable = 0x4F424A66;
static constexpr uint32_t kGZCLSID_cTSTreeTable = 0x54544142;
static constexpr uint32_t kGZCLSID_cTSGlobalObjectLuaScriptRecord = 0x9012468A;
static constexpr uint32_t kGZCLSID_cTSObjectLuaScriptRecord = 0x9012468B;
static constexpr uint32_t kGZCLSID_cTSObjectSelector = 0x5B12BF90;
