#include "../pch.h"
#include "../include/Menu.hpp"
#include "SDK.hpp"
#include "config.h"
#include <algorithm>
#include <drawing.hpp>

std::string rand_str(const int len)
{
    std::string str;
    char c;
    int idx;
    for (idx = 0; idx < len; idx++)
    {
        c = 'a' + rand() % 26;
        str.push_back(c);
    }
    return str;
}
void Damage(SDK::APalCharacter* character, int32 damage)
{
    SDK::FPalDamageInfo  info = SDK::FPalDamageInfo();
    info.AttackElementType = SDK::EPalElementType::Normal;
    info.Attacker = Config.GetPalPlayerCharacter();
    info.AttackerGroupID = Config.GetPalPlayerState()->IndividualHandleId.PlayerUId;
    info.AttackerLevel = 50;
    info.AttackType = SDK::EPalAttackType::Weapon;
    info.bApplyNativeDamageValue = true;
    info.bAttackableToFriend = true;
    info.IgnoreShield = true;
    info.NativeDamageValue = damage;
    Config.GetPalPlayerState()->SendDamage_ToServer(character, info);
}

int InputTextCallback(ImGuiInputTextCallbackData* data) {
    char inputChar = data->EventChar;

    Config.Update(Config.inputTextBuffer);

    return 0;
}

CatchRate CRate;
CatchRate OldRate;

void DetourCatchRate(SDK::APalCaptureJudgeObject* p_this) {
    if (p_this) {
        //p_this->ChallengeCapture_ToServer(Config.localPlayer, Config.CatchRate);
        p_this->ChallengeCapture(Config.localPlayer, Config.CatchRate);
    }
}
void ToggleCatchRate(bool catchrate) {
    if (catchrate) {
        if (CRate == NULL) {
            CRate = (CatchRate)(Config.ClientBase + Config.offset_CatchRate);
            MH_CreateHook(CRate, DetourCatchRate, reinterpret_cast<void**>(OldRate));
            MH_EnableHook(CRate);
            return;
        }
        MH_EnableHook(CRate);
        return;
    }
    else
    {
        MH_DisableHook(CRate);

    }
}

namespace DX11_Base {

    //helper variables
    char inputBuffer_getFnAddr[100];
    char inputBuffer_getClass[100];
    char inputBuffer_setWaypoint[32];

    namespace Styles {
        void InitStyle()
        {
            ImGuiStyle& style = ImGui::GetStyle();
            ImVec4* colors = ImGui::GetStyle().Colors;

            //	STYLE PROPERTIES
            style.WindowPadding = ImVec2(15, 15);
            style.WindowRounding = 5.0f;
            style.FramePadding = ImVec2(5, 5);
            style.FrameRounding = 4.0f;
            style.ItemSpacing = ImVec2(12, 8);
            style.ItemInnerSpacing = ImVec2(8, 6);
            style.IndentSpacing = 25.0f;
            style.ScrollbarSize = 15.0f;
            style.ScrollbarRounding = 9.0f;
            style.GrabMinSize = 5.0f;
            style.GrabRounding = 3.0f;

            //  Base ImGui Styling , Aplying a custyom style is left up to you.
            ImGui::StyleColorsDark();

            /// EXAMPLE COLOR 
            //colors[ImGuiCol_FrameBg] = ImVec4(0, 0, 0, 0);

            //	COLORS
            if (g_Menu->dbg_RAINBOW_THEME) {
                //  RGB MODE STLYE PROPERTIES
                colors[ImGuiCol_Separator] = ImVec4(g_Menu->dbg_RAINBOW);
                colors[ImGuiCol_TitleBg] = ImVec4(0, 0, 0, 1.0f);
                colors[ImGuiCol_TitleBgActive] = ImVec4(0, 0, 0, 1.0f);
                colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0, 0, 0, 1.0f);
            }
            else {
                /// YOUR DEFAULT STYLE PROPERTIES HERE

                colors[ImGuiCol_WindowBg] = ImColor(0, 0, 0, 230);
                colors[ImGuiCol_Border] = ImColor(0, 0, 0, 0);
                colors[ImGuiCol_BorderShadow] = ImColor(0, 0, 0, 0);
                colors[ImGuiCol_Button] = ImColor(31, 30, 31, 255);
                colors[ImGuiCol_ButtonActive] = ImColor(239, 73, 88, 255);
                colors[ImGuiCol_ButtonHovered] = ImColor(173, 55, 65, 255);
                colors[ImGuiCol_FrameBg] = ImColor(31, 30, 31, 255);
                colors[ImGuiCol_FrameBgActive] = ImColor(44, 43, 44, 255);
                colors[ImGuiCol_FrameBgHovered] = ImColor(37, 36, 37, 255);
                colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
                colors[ImGuiCol_ChildBg] = ImColor(33, 34, 45, 150);
                colors[ImGuiCol_CheckMark] = ImColor(240, 50, 66, 255);
                colors[ImGuiCol_SliderGrab] = ImColor(240, 74, 88, 255);
                colors[ImGuiCol_SliderGrabActive] = ImColor(240, 50, 66, 255);
                colors[ImGuiCol_Header] = ImColor(240, 50, 66, 255);
                colors[ImGuiCol_HeaderHovered] = ImColor(240, 74, 88, 255);
                colors[ImGuiCol_HeaderActive] = ImColor(240, 50, 66, 255);
                colors[ImGuiCol_ResizeGrip] = ImColor(220, 50, 66, 120);
                colors[ImGuiCol_ResizeGripHovered] = ImColor(250, 50, 66, 140);
                colors[ImGuiCol_ResizeGripActive] = ImColor(240, 50, 66, 255);
                colors[ImGuiCol_SeparatorHovered] = ImColor(250, 50, 66, 140);
                colors[ImGuiCol_SeparatorActive] = ImColor(240, 50, 66, 255);
                colors[ImGuiCol_TitleBgActive] = ImVec4(0.13f, 0.12f, 0.12f, 1.00f);
                colors[ImGuiCol_Tab] = ImVec4(0.58f, 0.18f, 0.18f, 0.86f);
                colors[ImGuiCol_TabHovered] = ImVec4(0.98f, 0.26f, 0.26f, 0.80f);
                colors[ImGuiCol_TabActive] = ImVec4(0.68f, 0.20f, 0.20f, 1.00f);
            }
        }
    }


    namespace Tabs
    {
        void TABPlayer()
        {
            ImGui::BeginGroup();//"#PlayerOption1", ImVec2(ImGui::GetContentRegionAvail().x / 2 - 20, 250), ImGuiWindowFlags_NoBackground);
            ImGui::Checkbox("Speed Hack", &Config.IsSpeedHack);

            if (Config.IsSpeedHack)
                ImGui::SliderFloat("Speed Multiply", &Config.SpeedModiflers, 1, 10);

            ImGui::Checkbox("Movement Speed Hack", &Config.MovementSpeed);
            if (Config.MovementSpeed)
            {
                ImGui::SliderFloat("Speed Multiply", &Config.SpeedModiflers, 1, 10);
                //("#movementspeed", ImVec2(200, 200), true);
                ImGui::Checkbox("Fast Run", &Config.fastRun); ImGui::SameLine();
                ImGui::Checkbox("Fast Swim", &Config.fastSwim);; ImGui::SameLine();
                ImGui::Checkbox("Fast Climb", &Config.fastClimb);; ImGui::SameLine();
                ImGui::Checkbox("Fast Rolling", &Config.fastRolling);; ImGui::SameLine();
                ImGui::Checkbox("Fast Glider", &Config.fastGlider);
                ImGui::Separator();
            }
            static bool DesyncMove;
            if (ImGui::Checkbox("Desync Movement", &DesyncMove))
            {
                SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
                //p_appc->SetReplicateMovement(DesyncMove);
                p_appc->GetPalCharacterMovementComponent()->bIgnoreClientMovementErrorChecksAndCorrection = true;

            }

            ImGui::Checkbox("Damage", &Config.IsAttackModiler);
            if (Config.IsAttackModiler)
                ImGui::SliderInt("Damage Multiply", &Config.DamageUp, 0, 2000);

            ImGui::Checkbox("DefenseHack", &Config.IsDefuseModiler);

            if (Config.IsDefuseModiler)
                ImGui::SliderInt("Defense Multiply", &Config.DefuseUp, 0, 2000);

            ImGui::Checkbox("Godmode", &Config.godMode);

            ImGui::Checkbox("Infinite Shield", &Config.infShield);
            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::SetCursorPosX(444);
            ImGui::BeginGroup();//("#PlayerOption2", ImVec2(ImGui::GetContentRegionAvail().x , 250), ImGuiWindowFlags_NoBackground);
            ImGui::Checkbox("Max CarryWeight", &Config.MaxWeight);

            ImGui::Checkbox("Infinited Ammo", &Config.IsInfinAmmo);

            ImGui::Checkbox("No Reload", &Config.NoReload);

            if (ImGui::Checkbox("FullBright", &Config.IsFullbright))
                SetFullbright(Config.IsFullbright);

            ImGui::Checkbox("InfStamina", &Config.IsInfStamina);



            if (ImGui::Checkbox("Invisible (F4)", &Config.spec))
            {
                SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
                if (p_appc != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        Config.GetPalPlayerCharacter()->SetSpectatorMode(Config.spec);

                    }
                }
            }
            ImGui::EndGroup();
            //ImGui::Checkbox("Revive", &Config.IsRevive);


            ImGui::InputText("Name Changer", Config.CharName, sizeof(Config.CharName));
            if (ImGui::Button("Rename", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                if (Config.GetPalPlayerCharacter() != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        SDK::UKismetStringLibrary* lib = SDK::UKismetStringLibrary::GetDefaultObj();

                        wchar_t ws[255];
                        swprintf(ws, 255, L"%hs", Config.CharName);
                        Config.GetPalPlayerCharacter()->GetPalPlayerController()->Transmitter->NetworkIndividualComponent->UpdateCharacterNickName_ToServer(Config.GetPalPlayerCharacter()->CharacterParameterComponent->IndividualHandle->ID, SDK::FString(ws));
                    }
                }
            }

            if (ImGui::Button("RandomName", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                if (Config.GetPalPlayerCharacter() != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        SDK::UKismetStringLibrary* lib = SDK::UKismetStringLibrary::GetDefaultObj();
                        std::string s = rand_str(20);

                        wchar_t  ws[255];
                        swprintf(ws, 255, L"%hs", s.c_str());
                        sprintf_s(Config.CharName, "%hs", s.c_str());
                        
                        Config.GetPalPlayerCharacter()->GetPalPlayerController()->Transmitter->NetworkIndividualComponent->UpdateCharacterNickName_ToServer(Config.GetPalPlayerCharacter()->CharacterParameterComponent->IndividualHandle->ID, SDK::FString(ws));
                    }
                }
            }



            if (ImGui::Button("PrintCoords", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
                if (p_appc != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        SDK::FVector PlayerLocation = p_appc->K2_GetActorLocation();
                        std::string MyLocation = std::format("\n{}, {}, {}\n", PlayerLocation.X, PlayerLocation.Y, PlayerLocation.Z);
                        g_Console->printdbg(MyLocation.c_str(), Console::Colors::green, p_appc);
                    }
                }
            }

            SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
            if (p_appc != NULL && Config.GetUWorld())
            {
                if (p_appc->GetPalPlayerController() != NULL)
                {
                    SDK::FVector PlayerLocation = p_appc->K2_GetActorLocation();
                    std::string MyLocation = std::format("X: {} | Y: {} | Z: {}", PlayerLocation.X, PlayerLocation.Y, PlayerLocation.Z);
                    ImGui::Text(MyLocation.c_str());
                }
            }
        }

        void TABMisc()
        {
            static bool buildInAir = false,buildInWater = false,DisableSnap = false, OverLapping = false, WorldCollision = false;
            static uint64_t buildInAirAOB, buildInWaterAOB, DisableSnapAOB1, DisableSnapAOB2, OverLappingAOB, WorldCollisionAOB;
            ImGui::Checkbox("Crafting Speed", &Config.craftspeed);
            ImGui::SameLine();
            if (ImGui::Checkbox("Build In Air", &buildInAir))
            {
                if (buildInAir)
                {
                    if (!buildInAirAOB)
                        buildInAirAOB = signature("E8 89 5B 2B 00 F6 85 BD 00 00 00 01 0F 84 8A 00 00 00 48 8D 8D E0 00 00 00").GetPointer() + 12;
                    BYTE patch[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
                    memory::WriteToMemory(buildInAirAOB, patch, 6);
                    //Credit CactusPi3
                }
                else
                {
                    BYTE patch[] = { 0x0F, 0x84, 0x8A, 0x00, 0x00, 0x00 };
                    memory::WriteToMemory(buildInAirAOB, patch, 6);
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Build In Water", &buildInWater))
            {
                if (buildInWater)
                {
                    if (!buildInWaterAOB)
                        buildInWaterAOB = signature("48 85 C9 74 05 E8 F6 5A 2B 00 84 DB 75 0E 0F B6 4E 30").GetPointer() + 12;
                    BYTE patch[] = { 0xEB, 0x0E };
                    memory::WriteToMemory(buildInWaterAOB, patch, 2);
                    //Credit CactusPi3
                }
                else
                {
                    BYTE patch[] = { 0x75, 0x0E };
                    memory::WriteToMemory(buildInWaterAOB, patch, 2);
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Overlapping Bases", &OverLapping))
            {
                if (OverLapping)
                {
                    if (!OverLappingAOB)
                        OverLappingAOB = signature("48 8B B4 24 80 00 00 00 3C 28 75 53 48 8B 9F 00 01 00 00 48 63 BF 08 01 00 00").GetPointer() + 10;
                    BYTE patch[] = { 0x90, 0x90 };
                    memory::WriteToMemory(OverLappingAOB, patch, 2);
                    //Credit CactusPi3
                }
                else
                {
                    BYTE patch[] = { 0x75, 0x53 };
                    memory::WriteToMemory(OverLappingAOB, patch, 2);
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("World Collision", &WorldCollision))
            {
                if (WorldCollision)
                {
                    if (!WorldCollisionAOB)
                        WorldCollisionAOB = signature("E8 A3 E0 00 00 84 C0 74 07 B0 15 E9 2F 01 00 00").GetPointer() + 7;
                    BYTE patch[] = { 0xEB };
                    memory::WriteToMemory(WorldCollisionAOB, patch, 1);
                    //Credit CactusPi3
                }
                else
                {
                    BYTE patch[] = { 0x74 };
                    memory::WriteToMemory(WorldCollisionAOB, patch, 1);
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Without Support", &DisableSnap))
            {
                if (DisableSnap)
                {
                    if (!DisableSnapAOB1)
                        DisableSnapAOB1 = signature("E8 47 5F 12 00 85 C0 7E 68 48 8B 4C 24 28").GetPointer() + 7;
                    if (!DisableSnapAOB2)
                        DisableSnapAOB2 = signature("C6 66 0F 2F D8 73 09 C7 43 28 01 00 00 00 EB 35").GetPointer() + 5;
                    BYTE patch1[] = { 0x90, 0x90};
                    memory::WriteToMemory(DisableSnapAOB1, patch1, 2);
                    //BYTE patch2[] = { 0xE9, 0x49, 0x04, 0x00, 0x00, 0x90 };
                    memory::WriteToMemory(DisableSnapAOB2, patch1, 2);
                    //Credit CactusPi3
                }
                else
                {
                    BYTE patch1[] = { 0x7E, 0x68 };
                    memory::WriteToMemory(DisableSnapAOB1, patch1, 2);
                    BYTE patch2[] = { 0x73, 0x09 };
                    memory::WriteToMemory(DisableSnapAOB2, patch2, 2);
                }
            }
            if (ImGui::Button("Cure All Workers", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                CureAll();
            }
            if (ImGui::Button("Fast Crafting", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
                SetCraftingSpeed(9999.f, false);
            if (ImGui::Button("Easy Pal Condensation", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                //auto ezcondenseAOB = signature("E8 ?? ?? ?? ?? 89 ?? ?? ?? 00 00 E9 ?? ?? ?? ?? 33").GetPointer();
                //BYTE patch[] = { 0x31, 0xC0, 0xFF, 0xC0, 0x90 };
                //memory::WriteToMemory(ezcondenseAOB, patch, 5);
                //Credit emoisback & Zanzer

                SDK::TMap<int32, int32> RankRequired = SDK::TMap<int32, int32>();
                SDK::UWorld* world = Config.GetUWorld();
                SDK::UPalUtility* aPalUtility = SDK::UPalUtility::GetDefaultObj();
                aPalUtility->GetGameSetting(world)->CharacterRankUpRequiredNumDefault = 1;
                aPalUtility->GetGameSetting(world)->CharacterRankUpRequiredNumMap = RankRequired;

            }

            if (ImGui::Button("ToggleFly", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                Config.IsToggledFly = !Config.IsToggledFly;
                ExploitFly(Config.IsToggledFly);
            }

            if (ImGui::Button("NormalHealth", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
                if (p_appc != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState() != NULL)
                        {
                            SDK::FFixedPoint fixpoint = SDK::FFixedPoint();
                            fixpoint.Value = Config.GetPalPlayerCharacter()->CharacterParameterComponent->GetMaxHP().Value;
                            Config.GetPalPlayerCharacter()->ReviveCharacter_ToServer(fixpoint);
                        }
                    }
                }
            }

            if (ImGui::Button("GodHealth", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
                if (p_appc != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState() != NULL)
                        {
                            Config.GetPalPlayerCharacter()->ReviveCharacter_ToServer(SDK::FFixedPoint(INT_MAX));
                        }
                    }
                }
            }



            if (ImGui::Button("Starter Pack", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25))) {
                SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
                if (p_appc != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState() != NULL)
                        {
                            SDK::UPalPlayerInventoryData* InventoryData = Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->GetInventoryData();
                            if (InventoryData != NULL) {
                                AddItemToInventoryByName(InventoryData, "AssaultRifle_Default5", 1);
                                AddItemToInventoryByName(InventoryData, "AssaultRifleBullet", 1000);
                                AddItemToInventoryByName(InventoryData, "Launcher_Default_5", 1);
                                AddItemToInventoryByName(InventoryData, "ExplosiveBullet", 50);
                                AddItemToInventoryByName(InventoryData, "PalSphere_Debug", 200);
                                AddItemToInventoryByName(InventoryData, "CornSoup", 20);
                                AddItemToInventoryByName(InventoryData, "Accessory_HeatResist_3", 1);
                                AddItemToInventoryByName(InventoryData, "Accessory_CoolResist_3", 1);
                                AddItemToInventoryByName(InventoryData, "AutoMealPouch_Tier1", 1);
                                AddItemToInventoryByName(InventoryData, "AutoMealPouch_Tier2", 1);
                                AddItemToInventoryByName(InventoryData, "AutoMealPouch_Tier3", 1);
                                AddItemToInventoryByName(InventoryData, "AutoMealPouch_Tier4", 1);
                                AddItemToInventoryByName(InventoryData, "AutoMealPouch_Tier5", 1);
                                AddItemToInventoryByName(InventoryData, "Lantern", 1);
                                AddItemToInventoryByName(InventoryData, "StealArmor_5", 1);
                                AddItemToInventoryByName(InventoryData, "StealHelmet_5", 1);
                                AddItemToInventoryByName(InventoryData, "Glider_Legendary", 1);
                                AddItemToInventoryByName(InventoryData, "Shield_04", 1);
                                AddItemToInventoryByName(InventoryData, "Pal_crystal_S", 1);
                                AddItemToInventoryByName(InventoryData, "Wood", 8);
                                AddItemToInventoryByName(InventoryData, "Stone", 3);
                                AddItemToInventoryByName(InventoryData, "SkillUnlock_JetDragon", 1);
                                AddItemToInventoryByName(InventoryData, "SkillUnlock_Anubis", 1);
                            }
                        }
                    }
                }
            }
            if (ImGui::Button("Spheres", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
                SpawnMultiple_ItemsToInventory(config::QuickItemSet::spheres);

            if (ImGui::Button("Tools", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
                SpawnMultiple_ItemsToInventory(config::QuickItemSet::tools);


            if (ImGui::Button("Skill Fruits", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
                SpawnMultiple_ItemsToInventory(config::QuickItemSet::skillfruits);

            if (ImGui::Button("Catch Rate", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                Config.isCatchRate = !Config.isCatchRate;
                ToggleCatchRate(Config.isCatchRate);
            }
            ImGui::InputFloat("Catch Rate Modifier", &Config.CatchRate);


        }
        void TABCageSpawner()
        {

            static char palSpawnCage[20] = "";
            std::initializer_list list = itemlist::pals;

            int cur_size = 0;

            static char pal_search[100];

            ImGui::InputText("Search", pal_search, IM_ARRAYSIZE(pal_search));
            ImGui::BeginListBox("##LIST_CAGE_MONSTERS", ImVec2(ImGui::GetContentRegionAvail().x, 500));
            int i = 0;
            static int selectedid = 0;
            for (const auto& pal : list) {

                std::istringstream ss(pal);
                std::string left_text, right_text;

                std::getline(ss, left_text, '|');
                std::getline(ss, right_text);

                auto right_to_lower = right_text;
                std::string pal_search_to_lower = pal_search;

                std::transform(right_to_lower.begin(), right_to_lower.end(), right_to_lower.begin(), ::tolower);
                std::transform(pal_search_to_lower.begin(), pal_search_to_lower.end(), pal_search_to_lower.begin(), ::tolower);

                if (pal_search[0] != '\0' && (right_to_lower.find(pal_search_to_lower) == std::string::npos))
                    continue;

                cur_size += right_text.length();

                bool isSelected = selectedid == i;
                ImGui::PushID(pal);
                if (ImGui::Selectable(right_text.c_str(), isSelected))
                {
                    selectedid = i;
                    static SDK::UKismetStringLibrary* lib = SDK::UKismetStringLibrary::GetDefaultObj();
                    wchar_t  ws[255];
                    swprintf(ws, 255, L"%hs", left_text.c_str());
                    SDK::FName Name = lib->Conv_StringToName(SDK::FString(ws));
                    Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->RequestSpawnMonsterForPlayer(Name, 1, 50);
                    strcpy_s(palSpawnCage, left_text.c_str());
                }
                i++;
                ImGui::PopID();
            }

            ImGui::EndListBox();
            if (ImGui::Button("Add Pal To Nearby Cage", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))//
            {
                SDK::APalPlayerCharacter* pLocalPlayer = Config.GetPalPlayerCharacter();
                if (!pLocalPlayer)
                    return;

                std::vector<SDK::AActor*> actors;

                //if (!config::GetAllActorsofType(SDK::APalCapturedCage::StaticClass(), &actors, true))
                //    return;
                SDK::UWorld* world = Config.GetUWorld();
                SDK::TUObjectArray* objects = world->GObjects;

                auto draw = ImGui::GetBackgroundDrawList();

                for (int i = 0; i < objects->NumElements; ++i)//(auto actor : actors)
                {
                    SDK::UObject* object = objects->GetByIndex(i);
                    if (!object) continue;
                    if (!object->IsA(SDK::APalCapturedCage::StaticClass())) continue;

                    SDK::APalCapturedCage* cage = static_cast<SDK::APalCapturedCage*>(object); // Try to cast actor to APalCapturedCage
                    if (!cage) continue; // Skip to the next actor if casting fails

                    SDK::FVector actorLocation = cage->K2_GetActorLocation();
                    SDK::FVector localPlayerLocation = pLocalPlayer->K2_GetActorLocation();
                    float distanceTo = GetDistanceToActor(pLocalPlayer, cage);

                    if (distanceTo < 50)
                    {
                        static SDK::UKismetStringLibrary* lib = SDK::UKismetStringLibrary::GetDefaultObj();

                        std::string palname = palSpawnCage;
                        wchar_t  ws[255];
                        swprintf(ws, 255, L"%hs", palname.c_str());
                        SDK::FName Name = lib->Conv_StringToName(SDK::FString(ws));

                        cage->SpawnPal(Name, 50);
                    }
                }
            }
        }
        void TABTeleporter()
        {
            auto UWorld = Config.GetUWorld();
            SDK::UPalUtility* pUtility = Config.pPalUtility;
            auto pController = Config.GetPalPlayerController();
            if (!UWorld || !pController || !pUtility) return;
            SDK::UPalLocationManager* pLocationMan = pUtility->GetLocationManager(UWorld);
            if (!pLocationMan) return;
            auto pLoccationBase = pLocationMan->GetLocationMap();
            static bool BaseCamp = true;
            ImGui::Checkbox("SafeTeleport", &Config.IsSafe);
            ImGui::SameLine();
            ImGui::Checkbox("Base Camps", &BaseCamp);
            //ImGui::SameLine();
            //ImGui::Checkbox("Custom Waypoints", &Config.bisOpenWaypoints);
            if (ImGui::Button("Go Home (F6)", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
                RespawnLocalPlayer(Config.IsSafe);
            if (ImGui::Button("Tele To MapMarker (F2)", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
                TeleportToMapMarker();

            ImGui::InputFloat3("Pos", Config.Pos);
            ImGui::SameLine();
            if (ImGui::Button("Go To", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                SDK::FVector vector = { Config.Pos[0],Config.Pos[1],Config.Pos[2] };
                AnyWhereTP(vector, Config.IsSafe);
            }

            //  Waypoints
            ImGui::InputTextWithHint("##INPUT_SETWAYPOINT", "CUSTOM WAYPOINT NAME", inputBuffer_setWaypoint, 32);
            ImGui::SameLine();
            if (ImGui::Button("SET", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                std::string wpName = inputBuffer_setWaypoint;
                if (wpName.size() > 0)
                {
                    AddWaypointLocation(wpName);
                    memset(inputBuffer_setWaypoint, 0, 32);
                }
            }

            if (ImGui::BeginListBox("##BOSS_LOCATIONS", ImVec2(ImGui::GetContentRegionAvail().x / 2, 500)))
            {
                for (const auto& pair : database::locationMap)
                {
                    const std::string& locationName = pair.first;
                    if (ImGui::Selectable(locationName.c_str()))
                    {
                        SDK::FVector location = SDK::FVector(pair.second[0], pair.second[1], pair.second[2]);
                        AnyWhereTP(location, Config.IsSafe);
                    }
                }
                ImGui::EndListBox();
            }
            ImGui::SameLine();
            if(BaseCamp)

            if (ImGui::BeginListBox("##LIST_LOCATIONS", ImVec2(ImGui::GetContentRegionAvail().x, 500)))
            {
                static int baseIdx;
                auto allLocation = pLoccationBase.Data;
                auto GuildMap = pUtility->GetGroupManager(UWorld)->GuildMap.Data;
                baseIdx = 1;
                for (int j = 0; j < GuildMap.Count(); j++)
                {
                    auto eGuild = GuildMap[j].Value.Second;
                    auto campids = eGuild->BaseCampIds;

                    for (int k = 0; k < campids.Count(); k++)
                    {
                        std::string gName;
                        baseIdx++;
                        gName = "[Lv." + std::to_string(eGuild->BaseCampLevel) + "] " + eGuild->GuildName.ToString() + " #";
                        gName += std::to_string(baseIdx);

                        if (ImGui::Selectable(gName.c_str()))
                        {
                            for (int i = 0; i < allLocation.Count(); i++)
                            {
                                if (!allLocation.IsValid())continue;
                                auto camploc = allLocation[i].Value.Second;
                                if (!camploc)continue;
                                //std::string name = camploc->GetName();
                                //auto type = camploc->GetType();
                                if (camploc->IsA(SDK::UPalLocationPoint::StaticClass()))//(strstr(name.c_str(), "BaseCamp") != nullptr)
                                {
                                    auto camplocpoint = (SDK::UPalLocationPointBaseCamp*)camploc->LocationPoint();
                                    if (campids[k] == camplocpoint->BaseCampId)
                                    {
                                        camplocpoint->InvokeFastTravel();
                                        break;
                                    }
                                }
                            }

                        }

                    }
                }
                ImGui::EndListBox();
            }
            /*
            if (ImGui::BeginListBox("##LIST_LOCATIONS", ImVec2(ImGui::GetContentRegionAvail().x, 500)))
            {
                auto allLocation = pLoccationBase.Data;
                
                for (int i = 0; i < allLocation.Count(); i++)
                {
                    //if (!allLocation.IsValidIndex(i))continue;
                    auto element = allLocation[i];
                    auto pLocation = element.Value.Second;
                    //auto type = pLocation->GetType();
                    if (!pLocation) continue;
                    std::string name = pLocation->GetName();
                    if(strstr(name.c_str(), "BaseCamp") != nullptr) //(type == SDK::EPalLocationType::PointBaseCamp)//
                    {
                        //class UPalLocationPointBaseCamp
                        auto location = (SDK::UPalLocationPointBaseCamp*)pLocation->LocationPoint();
                        SDK::UPalBaseCampModel *campModel;
                        SDK::FString GuildName;
                        if (pUtility->GetBaseCampManager(UWorld)->TryGetModel(location->BaseCampId, &campModel))
                        {
                            if(pUtility->GetGroupManager(UWorld)->TryGetGuildName(campModel->GroupIdBelongTo, &GuildName))
                                name = GuildName.ToString();
                            else
                                name = "Player Camp #";
                        } 
                        else
                            name = "Player Camp #";
                        name += std::to_string(i);
                        if (ImGui::Selectable(name.c_str()))
                        {
                            
                            if (!location) continue;
                            location->InvokeFastTravel();
                            //AnyWhereTP(location, Config.IsSafe);
                        }
                    }
                }
                ImGui::EndListBox();
            }
            */
            
            if (Config.db_waypoints.size() > 0)
            {
                if (ImGui::BeginListBox("##LIST_WAYPOINTS", { ImGui::GetContentRegionAvail().x / 2, 125 }))
                {
                    DWORD index = -1;
                    for (auto waypoint : Config.db_waypoints)
                    {
                        index++;
                        ImGui::PushID(index);
                        //  ImGui::Checkbox("SHOW", &waypoint.bIsShown);
                        //  ImGui::SameLine();
                        if (ImGui::Selectable(waypoint.waypointName.c_str()))
                            AnyWhereTP(waypoint.waypointLocation, false);
                        ImGui::PopID();
                    }

                    ImGui::EndListBox();
                }
            }
        }

        void TABExploit()
        {
            ImGui::InputInt("EXP :", &Config.EXP);
            ImGui::SameLine();
            if (ImGui::Button("Give", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
                if (p_appc != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState() != NULL)
                        {
                            if (Config.EXP >= 0)
                            {
                                Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->GrantExpForParty(Config.EXP);
                            }
                        }
                    }
                }
            }

            if (ImGui::Button("Max Level<50>", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
                if (p_appc != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState() != NULL)
                        {
                            Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->GrantExpForParty(99999999);
                        }
                    }
                }
            }

            if (ImGui::Button("Unlock Technologies", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                SDK::APalPlayerCharacter* pPalCharacter = Config.GetPalPlayerCharacter();

                if (!pPalCharacter)
                    return;

                auto Unlocked = pPalCharacter->GetPalPlayerController()->GetPalPlayerState()->TechnologyData->UnlockedTechnologyNameArray;

                SDK::TArray<SDK::FName> mos = {};
                SDK::UDataTableFunctionLibrary::GetDefaultObj()->GetDataTableRowNames(pPalCharacter->GetPalPlayerController()->GetPalPlayerState()->TechnologyData->TechnologyDataSet.RecipeUnlockDataTable, &mos);

                for (int i = 0; i < Unlocked.Count(); i++) {
                    //g_Console->printdbg("[+] %s already unlocked\n", Console::Colors::green, Unlocked[i].GetRawString().c_str());
                }

                for (int i = 0; i < mos.Count(); i++) {
                    bool skip = false;

                    for (int j = 0; j < Unlocked.Count(); j++)
                        if (mos[i].GetRawString() == Unlocked[j].GetRawString())
                            skip = true;

                    if (skip) continue;

                    //g_Console->printdbg("[+] Unlocking %s\n", Console::Colors::green, mos[i].GetRawString().c_str());
                    pPalCharacter->GetPalPlayerController()->Transmitter->Player->RequestUnlockTechnology_ToServer(mos[i]);
                }
            }
            if (ImGui::Button("Tech Books", ImVec2(130, 20))) {
                SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
                if (p_appc != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState() != NULL)
                        {
                            SDK::UPalPlayerInventoryData* InventoryData = Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->GetInventoryData();
                            if (InventoryData != NULL)
                                AddItemToInventoryByName(InventoryData, "TechnologyBook_G3", 50);
                        }
                    }
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Use Books (1st Slot)", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25))) {
                SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
                if (p_appc != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState() != NULL)
                        {
                            SDK::UPalPlayerInventoryData* InventoryData = Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->GetInventoryData();
                            if (InventoryData != NULL) {
                                SDK::UPalItemContainerMultiHelper* InventoryMultiHelper = InventoryData->InventoryMultiHelper;
                                if (InventoryMultiHelper != NULL) {
                                    SDK::TArray<class SDK::UPalItemContainer*> Containers = InventoryMultiHelper->Containers;
                                    if (Containers.Count() == 0) {
                                        return;
                                    }

                                    SDK::UPalItemSlot* FirstSlot = Containers[0]->Get(0);

                                    if (FirstSlot != NULL)
                                    {
                                        SDK::FPalItemSlotId ID = FirstSlot->GetSlotId();
                                        for (int i = 0; i < FirstSlot->GetStackCount(); i++)
                                            Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->TechnologyData->RequestAddTechnologyPointByItem(ID);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (ImGui::Button("Unlock Pal skills", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
                SpawnMultiple_ItemsToInventory(config::QuickItemSet::pal_unlock_skills);
            if (ImGui::Button("All Effigies", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25))) //credit to bennett
            {
                SDK::APalPlayerCharacter* pPalCharacter = Config.GetPalPlayerCharacter();
                if (!pPalCharacter)
                    return;

                SDK::UWorld* world = Config.GetUWorld();
                if (!world)
                    return;

                SDK::TUObjectArray* objects = world->GObjects;

                for (int i = 0; i < objects->NumElements; ++i) {
                    SDK::UObject* object = objects->GetByIndex(i);

                    if (!object) continue;

                    if (!object->IsA(SDK::APalLevelObjectRelic::StaticClass()))
                        continue;

                    SDK::APalLevelObjectObtainable* relic = (SDK::APalLevelObjectObtainable*)object;
                    if (!relic) continue;

                    ((SDK::APalPlayerState*)pPalCharacter->PlayerState)->RequestObtainLevelObject_ToServer(relic);
                }
            }
            if (ImGui::Button("Unlock Fast Travel", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25))) //credit aaacaaac
            {
                std::vector<std::string> keyStrings = {
                    "6E03F8464BAD9E458B843AA30BE1CC8F","DDBBFFAF43D9219AE68DF98744DF0831","603ED0CD4CFB9AFDC9E11F805594CCE5","6282FE1E4029EDCDB14135AA4C171E4C","9FBB93D84811BE424A37C391DBFBB476","979BF2044C8E8FE559B598A95A83EDE3","923B781345D2AB7ECED6BABD6E97ECE8",
                    "16C7164C43E2B96BEDCC6696E2E592F7","D27AFCAD472164F000D33A8D8B61FE8B","41727100495D21DC905D309C53989914","6DB6B7B2431CA2EFFFC88EB45805BA6A","74270C2F45B8DCA66B6A1FAAA911D024","DF9FB9CB41B43052A9C74FA79A826A50","8CA5E9774FF1BBC1156ABCA09E616480",
                    "15314BE94E2FB8D018D5749BE9A318F0","79C561B747335A7A0A8FBF9FAE280E62","23B9E99C4A454B99220AF7B4A58FD8DE","A1BC65AA445619EF338E0388BC010648","BF8B123244ADB794A06EA8A10503FBDD","F8DF603B4C750B37D943C9AF6A911946","596996B948716D3FD2283C8B5C6E829C",
                    "28D514E74B51FD9EB078A891DB0787C2","ACAE5FB04D48DE4197443E9C0993086B","4D2F204549AB656CA1EA4B8E39C484F3","1BDEABA240B1699541C17F83E59E61DF","2BC5E46049E69D3549CFB58948BE3288","91DAC6F34D2A9FD7F01471B5166C6C02","41E36D9A4B2BA79A3AD1B7B83B16F77D",
                    "76B000914943BADDC56BCFBAE2BF051E","DC0ECF9241B4410C59EE619F56D1106A","71C4B2B2407F2BBBD77572A20C7FF0F5","EC94023A4CA571FF0FD19E90944F4231","2A2B744B41AC79964DAE6B89CAC51FC3","E0819EFB41581AEAC3A029B0EE2FE195","22095BFA48A46172F8D154B2EBEB7493",
                    "7C5E91514F6E84B0C1DEFFB52C7C4DBA","AECFED0D41AFEE11F30B4F9687BC3243","2EC07ACC4505CB726DE38A84246CA999","F8E5CB8143F4FA2F6213E6B454569F87","5F426B49469368B0C131D3A6DB8F7831","A277AE6341EF40D84D711EA52303353F","6231802D40C81C00445379AE238D9F90",
                    "F6C005A14B38FE0B57F1C7869FD899CB","7170881D44249E90902F728E240493AF","3E8E504B4A3975FD3862E1BC85A5D4F6","B001852C491FF5E70C4747BFF9972924","2DE1702048A1D4A82126168C49BE51A9","E88379634CB5B6117DA2E7B8810BFE0A","3697999C458BF8A3C7973382969FBDF9",
                    "65B10BB14ABDA9C2109167B21901D195","4669582D4081BF550AFB66A05D043A3D","FE90632845114C7FBFA4669D071E285F","5970E8E648D2A83AFDFF7C9151D9BEF5","B639B7ED4EE18A7AA09BA189EA703032","099440764403D1508D9BADADF4848697","B44AA24445864494E7569597ACCAEFC6",
                    "3A0F123947BE045BC415C6B061A5285A","F382ADAE4259150BF994FF873ECF242B", "866881DB443444AA7F4E7C8E5DCDAA29", "01ACCA6E4BDAA68220821FB05AB54E4D", "75BD9923489E2A4EBCED5A81175D5928", "513E166044565A0BD3360F94142577E8"
                };

                SDK::APalPlayerCharacter* pPalCharacter = Config.GetPalPlayerCharacter();

                for (const std::string& keyString : keyStrings)
                {
                    SDK::UKismetStringLibrary* lib = SDK::UKismetStringLibrary::GetDefaultObj();
                    //Convert FNAME
                    wchar_t  ws[255];
                    swprintf(ws, 255, L"%hs", keyString.c_str());
                    SDK::FName FNameKS = lib->Conv_StringToName(SDK::FString(ws));

                    ((SDK::APalPlayerState*)pPalCharacter->PlayerState)->RequestUnlockFastTravelPoint_ToServer(FNameKS);
                }

            }
            if (ImGui::Button("Unlock All Chests", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                SDK::UWorld* world = Config.GetUWorld();
                if (!world) return;

                SDK::TUObjectArray* objects = world->GObjects;
                if (!objects) return;

                for (int i = 0; i < objects->NumElements; ++i)
                {
                    SDK::UObject* object = objects->GetByIndex(i);
                    if (object && object->IsA(SDK::UPalMapObjectPasswordLockModule::StaticClass()))
                    {
                        SDK::UPalMapObjectPasswordLockModule* locked = static_cast<SDK::UPalMapObjectPasswordLockModule*>(object);
                        if (locked) locked->LockState = SDK::EPalMapObjectPasswordLockState::Unlock;
                    }
                    if (object && object->IsA(SDK::APalMapObjectTreasureBox::StaticClass()))
                    {
                        auto box = static_cast<SDK::APalMapObjectTreasureBox*>(object);
                        if (box) box->BroadcastTriggerOpen();
                    }
                }
            }
            if (ImGui::Button("Reset Status Points", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {

                Config.GetPalPlayerController()->ServerRestartPlayer();
            }
            if(0)//disabled
            if (ImGui::Button("Crash Server", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                if (Config.GetPalPlayerCharacter() != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        SDK::UWorld* world = Config.GetUWorld();
                        if (!world) return;

                        SDK::TUObjectArray* objects = world->GObjects;

                        for (int i = 0; i < objects->NumElements; ++i) {
                            SDK::UObject* object = objects->GetByIndex(i);

                            if (!object) continue;

                            if (!object->IsA(SDK::APalMonsterCharacter::StaticClass())) {
                                continue;
                            }

                            SDK::APalMonsterCharacter* Monster = (SDK::APalMonsterCharacter*)object;

                            Config.GetPalPlayerCharacter()->GetPalPlayerController()->RequestLiftup_ToServer((SDK::APalCharacter*)object);
                        }

                    }
                }
            }
            if (ImGui::Button("Killaura", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                if (Config.GetPalPlayerCharacter() != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState())
                        {
                            SDK::TArray<SDK::AActor*> T = Config.GetUWorld()->PersistentLevel->Actors;
                            for (int i = 0; i < T.Count(); i++)
                            {

                                if (T[i] != NULL)
                                {
                                    if (T[i]->IsA(SDK::APalCharacter::StaticClass()))
                                    {
                                        SDK::APalCharacter* monster = (SDK::APalCharacter*)T[i];
                                        if (monster->IsLocallyControlled())
                                        {
                                            continue;
                                        }
                                        Config.GetPalPlayerState()->SendDeath_ToServer(monster);
                                    }
                                }
                            }
                        }
                    }
                }
            }



            if (ImGui::Checkbox("TELEPORT PALS TO XHAIR", &Config.IsTeleportAllToXhair) && !Config.IsTeleportAllToXhair)
                Config.mDebugEntCapDistance = 10.f;
            {
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##ENT_CAP_DISTANCE", &Config.mDebugEntCapDistance, 1.0f, 100.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
            }
            if (ImGui::Checkbox("DEATH AURA", &Config.IsDeathAura) && !Config.IsDeathAura)
            {
                Config.mDeathAuraDistance = 10.0f;
                Config.mDeathAuraAmount = 1;
            }
            if (Config.IsDeathAura)
            {
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * .7);
                ImGui::SliderFloat("##AURA_DISTANCE", &Config.mDeathAuraDistance, 1.0f, 100.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderInt("##AURA_DMG", &Config.mDeathAuraAmount, 1, 100, "%d", ImGuiSliderFlags_AlwaysClamp);
            }
        }

        void TABConfig()
        {

            ImGui::Text("PalCrack Menu");
            ImGui::Text("Version: v1.0");
            ImGui::Text("This is just an Free ModMenu. if you paid for this you have been scammed");
            ImGui::Text("Credits to: bluesword007");
            ImGui::Text("Credits to: Palworld Offsets UnknownCheats.me");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            if (ImGui::Button("Unload DLL", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25))) {
#if DEBUG
                g_Console->printdbg("\n\n[+] UNHOOK INITIALIZED [+]\n\n", Console::Colors::red);

#endif
                g_KillSwitch = TRUE;
            }
        }



        void TABItemSpawner()
        {
            static int num_to_add = 1;
            static int category = 0;

            ImGui::InputInt("Num To Add", &num_to_add);

            ImGui::Combo("Item Category", &category, "All Items\0Accessories\0Ammo\0Armor\0Blueprints\0Crafting Materials\0Eggs\0Food\0Hats\0\Medicine\0Money\0Other\0Pal Spheres\0Saddles\0Seeds\0Tools\0Weapons\0Skill Fruit\0");

            std::initializer_list list = database::all;

            switch (category)
            {
            case 1:
                list = database::accessories;
            case 2:
                list = database::ammo;
                break;
            case 3:
                list = database::armor;
                break;
            case 4:
                list = itemlist::blueprints;
                break;
            case 5:
                list = database::craftingmaterials;
                break;
            case 6:
                list = database::eggs;
                break;
            case 7:
                list = database::food;
                break;
            case 8:
                list = database::hats;
                break;
            case 9:
                list = database::medicine;
                break;
            case 10:
                list = database::money;
                break;
            case 11:
                list = database::other;
                break;
            case 12:
                list = database::palspheres;
                break;
            case 13:
                list = database::saddles;
                break;
            case 14:
                list = database::seeds;
                break;
            case 15:
                list = database::toolss;
                break;
            case 16:
                list = database::weapons;
                break;
            case 17:
                list = database::skillfruit;
                break;
            default:
                list = database::all;
            }

            int cur_size = 0;

            static char item_search[100];

            ImGui::InputText("Search", item_search, IM_ARRAYSIZE(item_search));

            ImGui::BeginListBox("##LIST_ITEMS", ImVec2(ImGui::GetContentRegionAvail().x, 500));
            for (const auto& item : list) {
                std::istringstream ss(item);
                std::string left_text, right_text;

                std::getline(ss, left_text, '|');
                std::getline(ss, right_text);

                auto right_to_lower = right_text;
                std::string item_search_to_lower = item_search;

                std::transform(right_to_lower.begin(), right_to_lower.end(), right_to_lower.begin(), ::tolower);
                std::transform(item_search_to_lower.begin(), item_search_to_lower.end(), item_search_to_lower.begin(), ::tolower);

                if (item_search[0] != '\0' && (right_to_lower.find(item_search_to_lower) == std::string::npos))
                    continue;


                //ImGui::PushID(item);
                if (ImGui::Selectable(right_text.c_str()))
                {
                    auto pController = Config.GetPalPlayerCharacter()->GetPalPlayerController();
                    if (pController)
                    {
                        SDK::UPalPlayerInventoryData* InventoryData = pController->GetPalPlayerState()->GetInventoryData();
                        AddItemToInventoryByName(InventoryData, left_text.c_str(), num_to_add);
                    }
                    
                }
                //ImGui::PopID();
            }
            ImGui::EndListBox();
        }

        void TABQuick()
        {
            if (ImGui::Button("Basic Items stack", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
                SpawnMultiple_ItemsToInventory(config::QuickItemSet::basic_items_stackable);

            if (ImGui::Button("Basic Items single", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
                SpawnMultiple_ItemsToInventory(config::QuickItemSet::basic_items_single);

            if (ImGui::Button("Unlock Pal skills", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
                SpawnMultiple_ItemsToInventory(config::QuickItemSet::pal_unlock_skills);

            if (ImGui::Button("Spheres", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
                SpawnMultiple_ItemsToInventory(config::QuickItemSet::spheres);

            if (ImGui::Button("Tools", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
                SpawnMultiple_ItemsToInventory(config::QuickItemSet::tools);
        }


        void TABDebug()
        {
            if (ImGui::Checkbox("DEBUG ESP", &Config.isDebugESP) && !Config.isDebugESP)
                Config.mDebugESPDistance = 10.f;
            if (Config.isDebugESP)
            {
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##DISTANCE", &Config.mDebugESPDistance, 1.0f, 100.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
            }

            //  @TODO: print additional debug information
            if (ImGui::Button("PrintPlayerAddr", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
                if (p_appc)
                    g_Console->printdbg("\n\n[+] APalPlayerCharacter: 0x%llX\n", Console::Colors::green, p_appc);

            }
            if (ImGui::Button("Print All Players", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {

                SDK::TArray<SDK::APalCharacter*> OutPlayers;
                SDK::UPalUtility::GetDefaultObj()->GetAllPlayerCharacters(Config.GetUWorld(), &OutPlayers);
                for (int i = 0; i < OutPlayers.Count(); i++)
                {
                    std::string name;
                    SDK::FString fn;
                    OutPlayers[i]->CharacterParameterComponent->GetNickname(&fn);
                    name = fn.ToString();
                    g_Console->printdbg("\n[+] APalPlayerCharacter: %s\n", Console::Colors::green, name.c_str());
                }
            }

            if (ImGui::Button("Style Editor", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                g_GameVariables->m_ShowDemo = !g_GameVariables->m_ShowDemo;
            }
            if (ImGui::Button("RAINBOW THEME", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {
                g_Menu->dbg_RAINBOW_THEME = !g_Menu->dbg_RAINBOW_THEME;
            }
            ImGui::InputTextWithHint("##INPUT", "INPUT GOBJECT fn NAME", inputBuffer_getFnAddr, 100);
            ImGui::SameLine();
            if (ImGui::Button("GET fn", ImVec2(ImGui::GetContentRegionAvail().x, 20)))
            {
                std::string input = inputBuffer_getFnAddr;
                SDK::UFunction* object = SDK::UObject::FindObject<SDK::UFunction>(input);
                if (object)
                {
                    static __int64 dwHandle = reinterpret_cast<__int64>(GetModuleHandle(0));
                    void* fnAddr = object->ExecFunction;
                    unsigned __int64 fnOffset = (reinterpret_cast<__int64>(fnAddr) - dwHandle);
                    g_Console->printdbg("[+] Found [%s] -> 0x%llX\n", Console::Colors::yellow, input.c_str(), fnOffset);
                }
                else
                    g_Console->printdbg("[!] OBJECT [%s] NOT FOUND!\n", Console::Colors::red, input.c_str());
                memset(inputBuffer_getFnAddr, 0, 100);
            }

            //  Get Class pointer by name
            ImGui::InputTextWithHint("##INPUT_GETCLASS", "INPUT OBJECT CLASS NAME", inputBuffer_getClass, 100);
            ImGui::SameLine();
            if (ImGui::Button("GET CLASS", ImVec2(ImGui::GetContentRegionAvail().x, 20)))
            {
                std::string input = inputBuffer_getClass;
                SDK::UClass* czClass = SDK::UObject::FindObject<SDK::UClass>(input.c_str());
                if (czClass)
                {
                    static __int64 dwHandle = reinterpret_cast<__int64>(GetModuleHandle(0));
                    g_Console->printdbg("[+] Found [%s] -> 0x%llX\n", Console::Colors::yellow, input.c_str(), czClass->Class);
                }
                else
                    g_Console->printdbg("[!] CLASS [%s] NOT FOUND!\n", Console::Colors::red, input.c_str());

            }



            if (ImGui::Button("PRINT ENGINE GLOBALS", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25)))
            {

                g_Console->printdbg("[+] [UNREAL ENGINE GLOBALS]\n"
                    "UWorld:\t\t\t0x%llX\n"
                    "ULocalPlayer:\t\t0x%llX\n"
                    "APalPlayerController:\t0x%llX\n"
                    "APalPlayerCharacter:\t0x%llX\n"
                    "APalPlayerState:\t0x%llX\n"
                    "UCharacterImpMan:\t0x%llX\n"
                    "UPalPlayerInventory:\t0x%llX\n"
                    "APalWeaponBase:\t\t0x%llX\n",
                    Console::Colors::yellow,
                    Config.gWorld,
                    Config.GetLocalPlayer(),
                    Config.GetPalPlayerController(),
                    Config.GetPalPlayerCharacter(),
                    Config.GetPalPlayerState(),
                    Config.GetCharacterImpManager(),
                    Config.GetInventoryComponent(),
                    Config.GetPlayerEquippedWeapon()
                );

            }

        }
        void TABPalSpawner()
        {
            SDK::UWorld* pWorld = Config.GetUWorld();
            SDK::UPalUtility* pUtility = Config.pPalUtility;
            SDK::UKismetStringLibrary* lib = SDK::UKismetStringLibrary::GetDefaultObj();
            SDK::UPalDatabaseCharacterParameter* paramDatabase = SDK::UPalDatabaseCharacterParameter::GetDefaultObj();
            SDK::UBlueprintFunctionLibrary* BPlib = SDK::UBlueprintFunctionLibrary::GetDefaultObj();
            auto pController = Config.GetPalPlayerController();

            if (!pController || !pWorld || !pUtility || !lib || !paramDatabase)return;//empty object check to prevent crash   

            static int level = 50, nameIdx, rank = 5, health = 4500, hunger = 600, attk = 100, defense = 1000, stamina = 1000, workspeed = 300;
            static int healthIV = 100, attackIV = 100, defendIV = 100, shotIV = 100;
            static int healthEnhc = 10, attackEnhc = 10, defendEnhc = 10, workEnhc = 10;
            static int passive1 = 0, passive2 = 0, passive3 = 0, passive4 = 0;
            static int skill1, skill2, skill3, skill4, skill5, skill6, skill7;
            static int passivePreset;
            static bool isBoss, isRare, isFemale, customStats, customSkills, CustomIV, EnhancePal, FullSkill, GiveNick = true;
            static bool None, EmitFlame, Watering, Seeding, GenerateElectricity, Handcraft, Collection, Deforest, Mining, OilExtraction, ProductMedicine, Cool, Transport, MonsterFarm, Anyone, MAX;

            static char filter[30];

            static int currentIndex = 0;
            // Open combo box
            //ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 230));
            //ImGui::BeginChild("#Spawnpal1", ImVec2(ImGui::GetContentRegionAvail().x/2, 280), true);
            if (ImGui::BeginCombo(":Pal Name", database::pals_realnames[currentIndex]))
            {
                ImGui::InputText("Filter", filter, IM_ARRAYSIZE(filter));
                for (int i = 0; i < 171; ++i)
                {
                    // Apply the filter
                    std::string name_to_lower = database::pals_realnames[i];
                    std::string filter_to_lower = filter;

                    std::transform(name_to_lower.begin(), name_to_lower.end(), name_to_lower.begin(), ::tolower);
                    std::transform(filter_to_lower.begin(), filter_to_lower.end(), filter_to_lower.begin(), ::tolower);
                    if (filter_to_lower[0] != '\0' && (name_to_lower.find(filter_to_lower) == std::string::npos))
                        continue;
                    else
                    {
                        bool is_selected = (currentIndex == i);
                        if (ImGui::Selectable(database::pals_realnames[i], is_selected))
                        {
                            currentIndex = i;
                            nameIdx = i;
                        }


                        if (is_selected)
                            ImGui::SetItemDefaultFocus(); // Set the initial focus when opening the combo
                    }
                }

                ImGui::EndCombo();
            }
            ImGui::InputInt("Rank (1-5)", &rank, 1, 5);
            ImGui::SliderInt("Level", &level, 1, 50);
            ImGui::Checkbox("Default Nickname", &GiveNick);
            ImGui::Checkbox("Is Boss", &isBoss);
            ImGui::Checkbox("Is Shiny", &isRare);
            ImGui::Checkbox("Is Female", &isFemale);
            static const char* pPresets[] = { "None", "Speeder", "Worker", "Tanker", "DPS"};
            if (ImGui::Combo("Passive Presets", &passivePreset, pPresets, 5))
            {
                switch (passivePreset)
                {
                default:passive1 = 0; passive2 = 0; passive3 = 0; passive4 = 0;
                    break;
                case 0:passive1 = 0; passive2 = 0; passive3 = 0; passive4 = 0;
                    break;
                case 1:passive1 = 64; passive2 = 55; passive3 = 54; passive4 = 53;
                    break;
                case 2:passive1 = 50; passive2 = 49; passive3 = 63; passive4 = 56;
                    break;
                case 3:passive1 = 64; passive2 = 33; passive3 = 32; passive4 = 40;
                    break;
                case 4:passive1 = 64; passive2 = 58; passive3 = 29; passive4 = 63;
                    break;
                }
            }
            ImGui::Combo("Passive 1:", &passive1, database::passive_skill_name, 65); ImGui::SameLine(); ImGui::Text(database::passive_skill_descriptions[passive1]);
            ImGui::Combo("Passive 2:", &passive2, database::passive_skill_name, 65); ImGui::SameLine(); ImGui::Text(database::passive_skill_descriptions[passive2]);
            ImGui::Combo("Passive 3:", &passive3, database::passive_skill_name, 65); ImGui::SameLine(); ImGui::Text(database::passive_skill_descriptions[passive3]);
            ImGui::Combo("Passive 4:", &passive4, database::passive_skill_name, 65); ImGui::SameLine(); ImGui::Text(database::passive_skill_descriptions[passive4]);
            ImGui::Checkbox("Custom Skills:", &customSkills); ImGui::SameLine(); ImGui::Checkbox("Full Skils", &FullSkill);
            if (customSkills)
            {
                ImGui::Combo("Equiped Skill 1", &skill1, database::pal_skills, 146);
                ImGui::Combo("Equiped Skill 2", &skill2, database::pal_skills, 146);
                ImGui::Combo("Equiped Skill 3", &skill3, database::pal_skills, 146);
                ImGui::Combo("UnEquiped Skill 1", &skill4, database::pal_skills, 146);
                ImGui::Combo("UnEquiped Skill 2", &skill5, database::pal_skills, 146);
                ImGui::Combo("UnEquiped Skill 3", &skill6, database::pal_skills, 146);
                ImGui::Combo("UnEquiped Skill 4", &skill7, database::pal_skills, 146);

            }

            ImGui::Checkbox("Custom IV's:", &CustomIV);
            if (CustomIV)
            {
                ImGui::InputInt("Healths IV", &healthIV, 1, 100);
                ImGui::InputInt("Attacks IV", &attackIV, 1, 100);
                ImGui::InputInt("Defenses IV", &defendIV, 1, 100);
                ImGui::InputInt("Work IV", &shotIV, 1, 100);
            }
            ImGui::Checkbox("Enhance Pal:", &EnhancePal);
            if (EnhancePal)
            {
                ImGui::SliderInt("Healths Enhance", &healthEnhc, 0, 10);
                ImGui::SliderInt("Attacks Enhance", &attackEnhc, 1, 10);
                ImGui::SliderInt("Defenses Enhance", &defendEnhc, 1, 10);
                ImGui::SliderInt("Works Enhance", &workEnhc, 1, 10);
            }
            ImGui::Checkbox("Custom Final Stats:", &customStats);
            if (customStats)
            {
                ImGui::InputInt("Health Final", &health, 1, 1000);
                ImGui::InputInt("Hunger Final", &hunger, 1, 1000);
                ImGui::InputInt("Attack (+AttkIV + base*level)", &attk, 1, 1000);
                //ImGui::InputInt("Defense", &defense, 1, 1000);
                ImGui::InputInt("Stamina Final", &stamina, 1, 1000);
                ImGui::InputInt("Work Speed (+Base)", &workspeed, 1, 1000);
            }
            //ImGui::Text("SoTMaulder Menu");
            ImGui::Spacing();
            ImGui::Separator();
            static bool isSphere;
            ImGui::Checkbox("Spawn Sphere instead", &isSphere);
            ImGui::SameLine();
            if (ImGui::Button("Spawn This Pal", ImVec2(ImGui::GetContentRegionAvail().x - 3, 25))) {

                SDK::FGuid myPlayerId = pController->GetPlayerUId();
                SDK::FGuid guid = SDK::UKismetGuidLibrary::GetDefaultObj()->NewGuid();
                std::wstring spawnName = database::pals_codenames[nameIdx];
                if (isBoss || isRare) spawnName = L"Boss_" + spawnName;
                auto CharacterID = lib->Conv_StringToName( SDK::FString( spawnName.c_str() ) );//lib->Conv_StringToName(SDK::FString(database::pals_codenames[nameIdx]));
                auto indParam = Config.GetPalPlayerCharacter()->CharacterParameterComponent->IndividualParameter;
                
                SDK::FPalInstanceID instanceid = {};
                instanceid.InstanceId = guid;
                instanceid.PlayerUId = myPlayerId;
                
                SDK::FPalIndividualCharacterSaveParameter initParameters = {};
                SDK::TArray<SDK::EPalWazaID> wazalist = {};
                SDK::TArray<SDK::FName> PassiveSkillList = {};
            
                if(!pUtility->GetInitializedCharacterSaveParemter_DebugDefaultParamSetup(pWorld, CharacterID, CharacterID, level, myPlayerId, &initParameters, true, wazalist, PassiveSkillList,rank, false) )
                    g_Console->printdbg("[+] Error: Fail to allocate params\n", Console::Colors::white);
                //if(paramUtility->IsValid(initParameters))
                    //paramDatabase->SetupSaveParameter(CharacterID, level, myPlayerId, &initParameters);
                
                if(GiveNick)
                    initParameters.NickName = SDK::FString(database::db_palnames[nameIdx]);
                else 
                    initParameters.NickName = SDK::FString(L"None");

                initParameters.CharacterID = CharacterID;

                initParameters.Rank = rank;
                initParameters.Level = level;
                initParameters.IsRarePal = isRare;
                
               
                if (customStats)
                {
                    initParameters.HP.Value = health * 1000;
                    initParameters.MaxHP.Value = health * 1000;
                    initParameters.MP.Value = stamina*1000;
                    initParameters.MaxMP.Value = stamina*1000;
                    initParameters.FullStomach = hunger;
                    initParameters.MaxFullStomach = hunger;
                    initParameters.CraftSpeed = workspeed;
                    initParameters.CraftSpeedRates.Values = 10.0f;
                    auto statuslist = indParam->GetSaveParameter().GotStatusPointList;

                    if (indParam)
                    {
                        //0=health|1=stamina|2=attk|3=weight|4=defend|5=workspeed
                        SDK::FPalGotStatusPoint hpSP = {}; hpSP.StatusName = statuslist[0].StatusName; hpSP.StatusPoint = health;
                        SDK::FPalGotStatusPoint stamSP = {}; stamSP.StatusName = statuslist[1].StatusName; stamSP.StatusPoint = stamina;
                        SDK::FPalGotStatusPoint attkSP = {}; attkSP.StatusName = statuslist[2].StatusName; attkSP.StatusPoint = attk + attackIV;
                        SDK::FPalGotStatusPoint carryw = {}; carryw.StatusName = statuslist[3].StatusName; carryw.StatusPoint = 9999;
                        SDK::FPalGotStatusPoint defSP = {}; defSP.StatusName = statuslist[4].StatusName; defSP.StatusPoint = defense;
                        SDK::FPalGotStatusPoint workSP = {}; workSP.StatusName = statuslist[5].StatusName; workSP.StatusPoint = workspeed;
                        initParameters.GotStatusPointList.Add(hpSP);
                        initParameters.GotStatusPointList.Add(stamSP);
                        initParameters.GotStatusPointList.Add(attkSP);
                        initParameters.GotStatusPointList.Add(carryw);
                        initParameters.GotStatusPointList.Add(defSP);
                        initParameters.GotStatusPointList.Add(workSP);
                    }

                }
                else//legit stats
                {
                    int hpCalc = ((78 * level) + 500) * 1000;
                    if(EnhancePal)
                        hpCalc += (healthEnhc * 0.03)* hpCalc;
                    if (CustomIV)
                        hpCalc += healthIV * 1000;
                    initParameters.HP.Value = hpCalc;
                    initParameters.MaxHP.Value = hpCalc;
                    initParameters.MP.Value = 500*1000;
                    initParameters.MaxMP.Value = 500*1000;
                    initParameters.FullStomach = 600;
                    initParameters.MaxFullStomach = 600;
                    initParameters.CraftSpeed = 100;
                    initParameters.CraftSpeedRates = indParam->GetSaveParameter().CraftSpeedRates;
                    
                }
                if (CustomIV )
                {

                    initParameters.Talent_HP = healthIV;
                    initParameters.Talent_Shot = attackIV;
                    initParameters.Talent_Defense = defendIV;
                    initParameters.Talent_Melee = shotIV;
                }
                if (EnhancePal)
                {
                    initParameters.Rank_HP = healthEnhc;
                    initParameters.Rank_Attack = attackEnhc;
                    initParameters.Rank_Defence = defendEnhc;
                    initParameters.Rank_CraftSpeed = workEnhc;
                }
               
                initParameters.SanityValue = 100;

                
                //initParameters.PalReviveTimer = 1.f;
                
                initParameters.AffectSanityRates.Values = 0.01f;
                initParameters.DecreaseFullStomachRates.Values = 0.01f;
                initParameters.FullStomachDecreaseRate_Tribe = 0.01f;
                
                initParameters.BaseCampWorkerEventProgressTime = 0.5f;
                initParameters.Gender = isFemale ? SDK::EPalGenderType::Female : SDK::EPalGenderType::Male;

                if (passive1)PassiveSkillList.Add(lib->Conv_StringToName(SDK::FString(database::passive_skill_list[passive1])));
                if (passive2)PassiveSkillList.Add(lib->Conv_StringToName(SDK::FString(database::passive_skill_list[passive2])));
                if (passive3)PassiveSkillList.Add(lib->Conv_StringToName(SDK::FString(database::passive_skill_list[passive3])));
                if (passive4)PassiveSkillList.Add(lib->Conv_StringToName(SDK::FString(database::passive_skill_list[passive4])));
                initParameters.PassiveSkillList = PassiveSkillList;
                if (customSkills)
                {
                    SDK::TArray<SDK::EPalWazaID> MasteredWaza = {};
                    if (FullSkill)
                    {
                        const int enumCount = 146;//static_cast<int>(SDK::EPalWazaID::MAX) + 1;
                        for (int i = 1; i < enumCount; i++)
                        {
                            MasteredWaza.Add((SDK::EPalWazaID)i);
                        }
                        initParameters.MasteredWaza = MasteredWaza;
                    }
                    else
                    {
                        if (skill1) wazalist.Add((SDK::EPalWazaID)skill1);
                        if (skill2) wazalist.Add((SDK::EPalWazaID)skill2);
                        if (skill3) wazalist.Add((SDK::EPalWazaID)skill3);
                        initParameters.EquipWaza = wazalist;
                        
                        if (skill4) MasteredWaza.Add((SDK::EPalWazaID)skill4);
                        if (skill5) MasteredWaza.Add((SDK::EPalWazaID)skill5);
                        if (skill6) MasteredWaza.Add((SDK::EPalWazaID)skill6);
                        if (skill7) MasteredWaza.Add((SDK::EPalWazaID)skill7);
                        initParameters.MasteredWaza = MasteredWaza;
                    }
                }

                //for (int i = 0; i < 7; ++i)initParameters.Pad_3D33[i] = i;
                                
                SDK::FPalWorkSuitabilityInfo tempData;
                tempData.Rank = 5;
                const int max = static_cast<int>(SDK::EPalWorkSuitability::MAX);
                
                for (int i = 1; i < max; i++)
                {
                    tempData.WorkSuitability = (SDK::EPalWorkSuitability)i;
                    initParameters.CraftSpeeds.Add(tempData);
                }

                SDK::FName Name = lib->Conv_StringToName(SDK::FString(L"None"));

                SDK::FNetworkActorSpawnParameters sp = {};

                sp.Owner = Config.GetPalPlayerCharacter();
                sp.NetworkOwner = Config.GetPalPlayerCharacter();
                sp.Name = Name;

                sp.SpawnRotation = Config.GetPalPlayerCharacter()->K2_GetActorRotation();
                sp.SpawnLocation = Config.GetPalPlayerCharacter()->K2_GetActorLocation();
                float rotRadX = sp.SpawnRotation.Yaw * (3.1415 / 180.0f);
                float distance = 10 * 100;//10m ahead

                sp.SpawnLocation.X += distance * std::cos(rotRadX);
                sp.SpawnLocation.Y += distance * std::sin(rotRadX);
                sp.SpawnLocation.Y += 300;
                //sp.SpawnScale = { 1.0,1.0,1.0 };

                sp.ControllerClass = pUtility->GetNPCManager(pWorld)->NPCAIControllerBaseClass.Get();//Config.GetPalPlayerCharacter()->AIControllerClass;//SDK::AController::StaticClass();
                
                sp.SpawnCollisionHandlingOverride = SDK::ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
                
                //auto FlatfID = Config.GetPalPlayerController()->GetPlatformUserId();
                
                pController->Transmitter->NetworkIndividualComponent->CreateIndividualID_ToServer(initParameters, guid, 0);
                pController->Transmitter->NetworkIndividualComponent->CreateFixedIndividualID_ToServer(instanceid, initParameters, guid, 0);
                if(!isSphere)
                    pController->Transmitter->NetworkIndividualComponent->SpawnIndividualActor_ToServer(instanceid, sp, guid);
                else
                    pController->GetPalPlayerState()->DropOtomoSingle_ToServer(Config.GetPalPlayerCharacter()->K2_GetActorLocation(), instanceid);
                //pController->Transmitter->NetworkIndividualComponent->SpawnPhantomActor_ToServer(instanceid, sp, guid);
            }
        }
        void TabEntityManager()
        {
            static bool FilterPlayer = true, FilterPals, FilterGuilds, FilterObjects,FilterDropItem, Export, DestroyAll;
            //if (ImGui::Begin("Manager", &g_GameVariables->m_ShowMenu, 96))
            {
                if (Config.GetUWorld() != NULL)
                {
                    ImGui::Checkbox("ENABLE ESP", &Config.IsESP);
                    ImGui::SameLine();
                    ImGui::Checkbox("Players", &FilterPlayer);
                    ImGui::SameLine();
                    ImGui::Checkbox("Pals", &FilterPals);
                    //ImGui::SameLine();
                    //ImGui::Checkbox("Guilds", &FilterGuilds);
                    ImGui::SameLine();
                    ImGui::Checkbox("DropItem", &FilterDropItem);
                    ImGui::SameLine();
                    ImGui::Checkbox("Map Objects", &FilterObjects);
                    ImGui::SameLine();
                    ImGui::Checkbox("Destroy NearBy Objects", &DestroyAll);
                    //ImGui::SameLine();
                    //ImGui::Checkbox("Dump All", &Export);
                    SDK::TArray<SDK::AActor*> T = Config.GetUWorld()->PersistentLevel->Actors;

                    ImGui::BeginChild("#EntityManager", ImVec2(0, 500), true);
                    for (int i = 0; i < T.Count(); i++)
                    {
                        SDK::AActor* Actor = T[i];

                        if (Actor == nullptr) continue;

                        SDK::APalCharacter* Character = (SDK::APalCharacter*)Actor;
                        std::string name;

                        if (Export)//Dump All Actor debug only
                        {
                            if (i == T.Count() - 1)Export = false;
                            name = Character->GetFullName();
                            auto coord = Actor->K2_GetActorLocation();
                            auto rot = Actor->K2_GetActorRotation();
                            g_Console->printdbg("\n[+] %s %f,%f,%f\t\t%f,%f,%f\n", Console::Colors::green, name.c_str(), coord.X, coord.Y, coord.Z, rot.Pitch, rot.Yaw, rot.Roll);
                        }
                        bool Skip = true;

                        if (FilterPlayer && Actor->IsA(SDK::APalPlayerCharacter::StaticClass())) Skip = false;
                        if (FilterPals && Actor->IsA(SDK::APalCharacter::StaticClass())) Skip = false;
                        //if (FilterGuilds && Actor->IsA(SDK::APalGuildInfo::StaticClass())) Skip = false;
                        if (FilterDropItem && Actor->IsA(SDK::APalMapObjectDropItem::StaticClass())) Skip = false;
                        if (FilterObjects && Actor->IsA(SDK::APalBuildObject::StaticClass())) Skip = false;
                        //if (Actor->IsA(SDK::APalCaptureJudgeObject::StaticClass())) Skip = false;

                        if (Skip) continue;

                        if (Actor->IsA(SDK::APalCharacter::StaticClass()))
                        {
                            if (!Character) { continue; }
                            SDK::FString fn;
                            Character->CharacterParameterComponent->GetNickname(&fn);
                            
                            auto lvl = Character->CharacterParameterComponent->GetLevel();
                            name = "[Lv:" + std::to_string(lvl)+ "]";
                            if (fn.IsValid())
                                name += fn.ToString();
                            else
                                name += "No Name";
                        }
                        //else if (Actor->IsA(SDK::APalGuildInfo::StaticClass()))
                        //    name = ((SDK::APalGuildInfo*)Actor)->Guild->GuildName.ToString();
                        else if (Actor->IsA(SDK::APalBuildObject::StaticClass()))//mapobject
                        {
                            SDK::APalBuildObject* Object = (SDK::APalBuildObject*)Actor;

                            name = Object->BuildObjectId.ToString();
                            if (DestroyAll)
                            {
                                DestroyObject(Actor);
                            }
                        }
                        else
                        {
                            
                            name = Character->Name.GetRawString();
                            name = name.substr(3);
                            name = name.substr(0, name.length() - 13);
                        }

                        ImGui::Text(name.c_str());
                        ImGui::SameLine();
                        ImGui::PushID(i);
                        if (ImGui::Button("TP"))
                        {
                            if (Config.GetPalPlayerCharacter() != NULL)
                            {
                                if (!Character) { continue; }
                                SDK::FVector vector = Character->K2_GetActorLocation();
                                AnyWhereTP(vector, Config.IsSafe);
                            }
                        }

                        if (Actor->IsA(SDK::APalCharacter::StaticClass())) {
                            ImGui::SameLine();
                            if (ImGui::Button("Kill"))
                            {
                                SendDamageToActor(Character, 99999999999);
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Set 1HP"))
                            {
                                Damage(Character, (Character->CharacterParameterComponent->GetHP().Value - 1) / 1000);
                            }
                            static SDK::APawn* myPawn;
                            ImGui::SameLine();
                            if (ImGui::Button("Test"))
                            {
                                if (!myPawn && Character->GetInstigatorController())
                                {
                                    //auto a1 = (SDK::ABP_MonsterAIController_Wild_C*)Character->AIControllerClass;
                                    //a1->K2_AttachToActor(Actor);
                                    //myPawn = Config.GetPalPlayerController()->Pawn;
                                    //Config.GetPalPlayerController()->Possess(myPawn);
                                    //Config.GetPalPlayerController()->Possess(Character->GetInstigatorController()->Pawn);
                                }
                                else
                                {
                                    //Config.GetPalPlayerController()->Possess(myPawn);
                                    //myPawn = NULL;
                                }
                                
                                
                                //SDK::UPalUtility* pUtility = Config.pPalUtility;
                                //SDK::UKismetStringLibrary* lib = SDK::UKismetStringLibrary::GetDefaultObj();
                                //Config.GetPalPlayerController()->UnPossess();
                                
                                //SDK::FName socketName = lib->Conv_StringToName(SDK::FString(L"None"));
                                //Config.GetPalPlayerController()->K2_AttachToActor(Character, socketName, SDK::EAttachmentRule::KeepRelative, SDK::EAttachmentRule::KeepRelative, SDK::EAttachmentRule::KeepRelative, true);
                                
                            }
                            if (!Actor->IsA(SDK::APalPlayerCharacter::StaticClass()))
                            {
                                ImGui::SameLine(); if (ImGui::Button("Clone")) {
                                    SDK::UKismetStringLibrary* lib = SDK::UKismetStringLibrary::GetDefaultObj();
                                    SDK::UWorld* pWorld = Config.GetUWorld();
                                    SDK::UPalUtility* pUtility = Config.pPalUtility;

                                    SDK::FGuid myPlayerId = Config.GetPalPlayerController()->GetPlayerUId();
                                    SDK::FGuid guid = SDK::UKismetGuidLibrary::GetDefaultObj()->NewGuid();

                                    SDK::FPalIndividualCharacterSaveParameter initParameters;
                                    initParameters = Character->GetCharacterParameterComponent()->GetIndividualParameter()->GetSaveParameter();

                                    SDK::FPalInstanceID instanceid = {};
                                    instanceid.InstanceId = guid;
                                    instanceid.PlayerUId = myPlayerId;

                                    SDK::FName Name = lib->Conv_StringToName(SDK::FString(L"None"));

                                    auto statuslist = initParameters.GotStatusPointList;
                                    if(0)//debug
                                    for (int i = 0; i < statuslist.Count(); i++)
                                    {
                                        g_Console->printdbg("[+] Status %d: %s || %d\n", Console::Colors::white, i, statuslist[i].StatusName.GetRawString().c_str(), statuslist[i].StatusPoint);
                                    }

                                    SDK::FNetworkActorSpawnParameters sp;
                                    sp.NetworkOwner = Config.GetPalPlayerCharacter();
                                    sp.Name = Name;
                                    sp.Owner = Config.GetPalPlayerCharacter();

                                    sp.SpawnRotation = Config.GetPalPlayerCharacter()->K2_GetActorRotation();
                                    sp.SpawnLocation = Config.GetPalPlayerCharacter()->K2_GetActorLocation();
                                    float rotRadX = sp.SpawnRotation.Yaw * (3.1415 / 180.0f);
                                    float distance = 10 * 100;//10m ahead

                                    sp.SpawnLocation.X += distance * std::cos(rotRadX);
                                    sp.SpawnLocation.Y += distance * std::sin(rotRadX);

                                    sp.SpawnScale = { 1.0,1.0,1.0 };

                                    sp.ControllerClass = pUtility->GetNPCManager(pWorld)->NPCAIControllerBaseClass.Get();//SDK::AController::StaticClass();//Character->AIControllerClass;//

                                    sp.SpawnCollisionHandlingOverride = SDK::ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;


                                    Config.GetPalPlayerCharacter()->GetPalPlayerController()->Transmitter->NetworkIndividualComponent->CreateIndividualID_ToServer(initParameters, guid, 0);
                                    Config.GetPalPlayerCharacter()->GetPalPlayerController()->Transmitter->NetworkIndividualComponent->CreateFixedIndividualID_ToServer(instanceid, initParameters, guid, 0);
                                    Config.GetPalPlayerCharacter()->GetPalPlayerController()->Transmitter->NetworkIndividualComponent->SpawnIndividualActor_ToServer(instanceid, sp, guid);
                                    //Config.GetPalPlayerCharacter()->GetPalPlayerController()->Transmitter->NetworkIndividualComponent->SpawnPhantomActor_SyncCheck(instanceid,0, guid);
                                    //Config.GetPalPlayerCharacter()->GetPalPlayerController()->Transmitter->NetworkIndividualComponent->SpawnPhantomActor_ToServer(instanceid, sp, guid);
                                    g_Console->printdbg("[+] Cloned: %s\n", Console::Colors::white, initParameters.CharacterID.ToString().c_str());
                                }
                            }


                            ImGui::SameLine();
                            if (ImGui::Button("MaxHP")) {
                                Damage(Character, -2147483647);
                            }
                        }
                        if (Character->IsA(SDK::APalPlayerCharacter::StaticClass()))
                        {
                            ImGui::SameLine();
                            if (ImGui::Button("BossFight"))
                            {
                                if (Config.GetPalPlayerCharacter() != NULL)
                                {
                                    auto controller = Config.GetPalPlayerCharacter()->GetPalPlayerController();

                                    if (controller != NULL)
                                    {
                                        SDK::FPalSyncTeleportRequestParameter tpParam;
                                        tpParam.bIsFadeOutSkip = true;
                                        tpParam.Location = { -321596.265032, 211085,-64908.850078047355 };
                                        //tpParam.Location = { -321248.80, 208959.95, 115.59 };
                                        tpParam.Rotation = 0;
                                        
                                        controller->Transmitter->BossBattle->RequestBossBattleEntry_ToServer(SDK::EPalBossType::ElectricBoss, (SDK::APalPlayerCharacter*)Character);
                                        static_cast<SDK::APalPlayerState*>(Character->PlayerState)->GetSyncTeleportComp()->RequestSyncTeleportStart_ToServer(tpParam);
                                        static_cast<SDK::APalPlayerState*>(Character->PlayerState)->GetSyncTeleportComp()->RequestSyncTeleportMove_ToServer();
                                        static_cast<SDK::APalPlayerState*>(Character->PlayerState)->GetSyncTeleportComp()->RequestSyncTeleportEnd_ToServer();
                                        controller->Transmitter->BossBattle->RequestBossBattleStart_ToServer(SDK::EPalBossType::ElectricBoss, (SDK::APalPlayerCharacter*)Character);
                                        //controller->CutsceneComponent->SetAutoActivate(true);
                                        controller->CutsceneComponent->PlayCutsceneToServer();
                                    }

                                }
                            }
                        }

                        if (Actor->IsA(SDK::APalPlayerCharacter::StaticClass())) {
                            ImGui::SameLine();
                            if (ImGui::Button("JoinGuild")) {
                                SDK::FGuid myPlayerId = Config.GetPalPlayerController()->GetPlayerUId();
                                SDK::FGuid playerId = SDK::UPalUtility::GetDefaultObj()->GetPlayerUIDByActor(Actor);

                                //g_Console->printdbg("\n\n[+] myPlayer GUID: %x\player GUID: %x [+]\n\n", Console::Colors::green, myPlayerId, playerId);
                                //Config.GetPalPlayerCharacter()->GetPalPlayerController()->Transmitter->Group->RequestJoinGuildRequestForPlayer_ToServer(myPlayerId, playerId);
                                Config.GetPalPlayerCharacter()->GetPalPlayerController()->Transmitter->Group->RequestJoinGuildForPlayer_ToServer(myPlayerId, playerId);
                            }

                            ImGui::SameLine();
                            if (ImGui::Button("JoinMe")) {
                                SDK::FGuid myPlayerId = Config.GetPalPlayerController()->GetPlayerUId();
                                SDK::FGuid playerId = SDK::UPalUtility::GetDefaultObj()->GetPlayerUIDByActor(Actor);

                                //g_Console->printdbg("\n\n[+] myPlayer GUID: %x\player GUID: %x [+]\n\n", Console::Colors::green, myPlayerId, playerId);

                                Config.GetPalPlayerCharacter()->GetPalPlayerController()->Transmitter->Group->RequestJoinGuildForPlayer_ToServer(playerId, myPlayerId);
                            }

                            ImGui::SameLine();
                            if (ImGui::Button("StealLeader")) {
                                SDK::FGuid myPlayerId = Config.GetPalPlayerController()->GetPlayerUId();
                                SDK::FGuid playerId = SDK::UPalUtility::GetDefaultObj()->GetPlayerUIDByActor(Actor);


                                g_Console->printdbg("\n\n[+] myPlayer GUID: %x\player GUID: %x [+]\n\n", Console::Colors::green, myPlayerId, playerId);

                                //Config.GetPalPlayerCharacter()->GetPalPlayerController()->Transmitter->Group->RequestJoinGuildForPlayer_ToServer(guid1, guid2);
                                Config.GetPalPlayerCharacter()->GetPalPlayerController()->Transmitter->Group->RequestChangeGuildAdmin_ToServer(playerId, myPlayerId);
                            }
                        }
                        if (Actor->IsA( SDK::APalBuildObject::StaticClass() ) ) {
                            ImGui::SameLine();
                            if (ImGui::Button("Destroy")) {
                                SDK::APalBuildObject* Object = (SDK::APalBuildObject*)Actor;
                                
                                /*
                                auto pBuilder = SDK::UPalBuilderComponent::GetDefaultObj();
                                auto loc = Object->K2_GetActorLocation();
                                loc.X += 200; loc.Y += 200;
                                auto rot = Object->K2_GetActorRotation();
                                SDK::FQuat rot2 = { 0,rot.Roll,0,0 };
                                SDK::TArray<SDK::FPalNetArchive> extraParam;
                                SDK::FPalNetArchive aaa; aaa.Bytes = 1;
                                SDK::FPalBuildRequestDebugParameter buildParams = {};
                                extraParam.Add(aaa);
                                extraParam.Add(aaa); extraParam.Add(aaa); extraParam.Add(aaa); extraParam.Add(aaa);
                                buildParams.bNotConsumeMaterials = false;
                                pBuilder->RequestBuild_ToServer(Object->BuildObjectId, loc, rot2, extraParam, buildParams);
                                //bSpawnableIfOverlapped
                                */
                                Config.GetPalPlayerCharacter()->GetPalPlayerController()->Transmitter->MapObject->RequestDismantleObject_ToServer(Object->ModelInstanceId);
                            }
                        }
                        if (Actor->IsA(SDK::APalCharacter::StaticClass())) {
                            ImGui::SameLine();
                            if (ImGui::Button("Freeze")) {
                                SDK::FStatusDynamicParameter statusParam;
                                Character->StatusComponent->AddStatus_ToServer(SDK::EPalStatusID::Freeze, statusParam, 256);
                            }
                        }
                        if (Character->IsA(SDK::APalPlayerCharacter::StaticClass()))
                        {
                            ImGui::SameLine();
                            if (ImGui::Button("Steal Name"))
                            {
                                if (Config.GetPalPlayerCharacter() != NULL)
                                {
                                    auto controller = Config.GetPalPlayerCharacter()->GetPalPlayerController();
                                    if (controller != NULL)
                                    {
                                        auto player = (SDK::APalPlayerCharacter*)Character;
                                        SDK::FString hisName,myName;
                                        player->CharacterParameterComponent->GetNickname(&hisName);
                                        Config.GetPalPlayerCharacter()->GetPalPlayerController()->Transmitter->NetworkIndividualComponent->UpdateCharacterNickName_ToServer(Config.GetPalPlayerCharacter()->CharacterParameterComponent->IndividualHandle->ID, hisName);
                                        Config.GetPalPlayerCharacter()->CharacterParameterComponent->GetNickname(&myName);
                                        Config.GetPalPlayerCharacter()->GetPalPlayerController()->Transmitter->NetworkIndividualComponent->UpdateCharacterNickName_ToServer(player->CharacterParameterComponent->IndividualHandle->ID, myName);
                                    }
                                }
                            }
                        }


                        ImGui::PopID();
                    }
                    ImGui::EndChild();
                }
            }
            //ImGui::End();
        }
    }

    void Menu::Draw()
    {

        if (g_GameVariables->m_ShowMenu)
            MainMenu();

        if (g_GameVariables->m_ShowHud)
            HUD(&g_GameVariables->m_ShowHud);

        if (g_GameVariables->m_ShowDemo)
            ImGui::ShowDemoWindow();
    }
    inline static std::string a_replaceAll(std::string subject, const std::string& search,
        const std::string& replace) {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != std::string::npos) {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return subject;
    }


    void Menu::MainMenu()
    {
        if (!g_GameVariables->m_ShowDemo)
            Styles::InitStyle();

        if (g_Menu->dbg_RAINBOW_THEME) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(g_Menu->dbg_RAINBOW));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(g_Menu->dbg_RAINBOW));
            ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(g_Menu->dbg_RAINBOW));
        }
        if (!ImGui::Begin("PalWorld ModMenu", &g_GameVariables->m_ShowMenu, 96))
        {
            ImGui::End();
            return;
        }
        if (g_Menu->dbg_RAINBOW_THEME) {
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        }

        ImGuiContext* pImGui = GImGui;

        //  Display Menu Content
        //Tabs::TABMain();

        //ImGui::Text("W.I.P. Some Functions Might Not work.");

        if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Player"))
            {
                Tabs::TABPlayer();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Unlockers"))
            {
                Tabs::TABExploit();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Misc"))
            {
                Tabs::TABMisc();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("TELEPORT"))
            {
                Tabs::TABTeleporter();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Item Spawner"))
            {
                Tabs::TABItemSpawner();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Pal Spawner"))
            {
                Tabs::TABPalSpawner();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Cage Spawner"))
            {
                Tabs::TABCageSpawner();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Entity Manager"))
            {
                Tabs::TabEntityManager();
                ImGui::EndTabItem();
            }

#if DEBUG
            if (ImGui::BeginTabItem("DEBUG"))
            {
                Tabs::TABDebug();
                ImGui::EndTabItem();
            }
#endif
            if (ImGui::BeginTabItem("Info"))
            {
                Tabs::TABConfig();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::End();




    }

    void Menu::HUD(bool* p_open)
    {

        ImGui::SetNextWindowPos(g_D3D11Window->pViewport->WorkPos);
        ImGui::SetNextWindowSize(g_D3D11Window->pViewport->WorkSize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, NULL);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.00f, 0.00f, 0.00f, 0.00f));
        if (!ImGui::Begin("##HUDWINDOW", (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs))
        {
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            ImGui::End();
            return;
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        auto ImDraw = ImGui::GetWindowDrawList();
        auto draw_size = g_D3D11Window->pViewport->WorkSize;
        auto center = ImVec2({ draw_size.x * .5f, draw_size.y * .5f });
        auto top_center = ImVec2({ draw_size.x * .5f, draw_size.y * 0.0f });


        auto fps = std::to_string((int)ImGui::GetIO().Framerate);
        DrawTextWithColorsCode(ImGui::GetFont(), fps.c_str(), ImVec2(20, 10), 20, ImColor(237, 28, 36), true, 1);

        if (Config.IsESP)
            ESP();

        if (Config.isDebugESP)
            ESP_DEBUG(Config.mDebugESPDistance);

        if (Config.db_waypoints.size() > 0)
            RenderWaypointsToScreen();

        ImGui::End();
    }

    void Menu::Loops()
    {

        if ((GetAsyncKeyState(VK_F4) & 1))//spectate
        {
            SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
            if (p_appc != NULL)
            {
                if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                {
                    Config.spec = !Config.spec;
                    
                    Config.GetPalPlayerCharacter()->SetSpectatorMode(Config.spec);
                    if (Config.spec)
                    {
                        Config.GetPalPlayerCharacter()->CharacterMovement->MovementMode = SDK::EMovementMode::MOVE_Flying;
                        Config.GetPalPlayerCharacter()->CharacterMovement->MaxFlySpeed = 350 * 200;
                        //Config.GetPalPlayerCharacter()->CharacterMovement->MaxAcceleration = 350 * 200;
                        Config.GetPalPlayerCharacter()->SetActorEnableCollision(false);
                        Config.GetPalPlayerController()->StartFlyToServer();
                        //Config.GetPalPlayerCharacter()->SetReplicateMovement(false);
                    }
                    else
                    {
                        Config.GetPalPlayerCharacter()->CharacterMovement->MovementMode = SDK::EMovementMode::MOVE_None;
                        Config.GetPalPlayerCharacter()->SetActorEnableCollision(true);
                        Config.GetPalPlayerController()->EndFlyToServer();
                       // Config.GetPalPlayerCharacter()->SetReplicateMovement(true);
                    }
                        
                        
                }
            }
        }
        if (Config.craftspeed)
            SetBasePalsCraftingSpeed(1000, 0);

        if ((GetAsyncKeyState(VK_F2) & 1))
            TeleportToMapMarker();
        //  Respawn
        if ((GetAsyncKeyState(VK_F5) & 1))
            ReviveLocalPlayer();


        //  Revive Player
        if ((GetAsyncKeyState(VK_F6) & 1))
            RespawnLocalPlayer(Config.IsSafe);

        static bool restSpeedHack = false;
        if (Config.IsSpeedHack)
        {
            restSpeedHack = true;
            SpeedHack(Config.SpeedModiflers);
        }
        else if (restSpeedHack)
        {
            restSpeedHack = false;
            SpeedHack(1.0f); // Turn off the feature
        }

        //  
        static bool restAttModifier = false;
        if (Config.IsAttackModiler)
        {
            restAttModifier = true;
            SetPlayerAttackParam(Config.DamageUp * 10);
        }
        else if (restAttModifier)
        {
            restAttModifier = false;
            SetPlayerAttackParam(1); // Turn off the feature
        }

        //  
        static bool restDefModifier = false;
        if (Config.IsDefuseModiler)
        {
            restDefModifier = true;
            SetPlayerDefenseParam(Config.DefuseUp * 10);
        }
        else if (restDefModifier)
        {
            restDefModifier = false;
            SetPlayerDefenseParam(1); // Turn off the feature
        }


        static bool bMovSpeedRestore = false;
        if (Config.MovementSpeed)
        {
            bMovSpeedRestore = true;
            FastRun(Config.SpeedModiflers);
        }
        else if (bMovSpeedRestore)
        {

            bMovSpeedRestore = false;
            FastRun(1.f);
        }
            

        if (Config.IsTeleportAllToXhair)
            TeleportAllPalsToCrosshair(Config.mDebugEntCapDistance);

        //
        if (Config.IsDeathAura)
            DeathAura(Config.mDeathAuraAmount, Config.mDeathAuraDistance, true, true, SDK::EPalVisualEffectID::Poison);

        //  
        if (Config.IsInfStamina)
            ResetStamina();

        if (Config.NoReload)
            NoReload();
        //  
        if (Config.MaxWeight)
            MaxWeight();

        static bool restoreInfammo = false;
        if (Config.GetPalPlayerCharacter() != NULL && Config.IsInfinAmmo)
        {
            if (Config.GetPalPlayerCharacter()->ShooterComponent != NULL && 
                Config.GetPalPlayerCharacter()->ShooterComponent->GetHasWeapon() != NULL && 
                Config.GetPalPlayerCharacter()->ShooterComponent->CanShoot())
            {
                Config.GetPalPlayerCharacter()->ShooterComponent->GetHasWeapon()->IsRequiredBullet = !Config.IsInfinAmmo;
                restoreInfammo = true;
            }
        }
        else if (restoreInfammo)
        {
            if (Config.GetPalPlayerCharacter()->ShooterComponent != NULL && 
                Config.GetPalPlayerCharacter()->ShooterComponent->GetHasWeapon() != NULL && 
                Config.GetPalPlayerCharacter()->ShooterComponent->CanShoot())
            {
                Config.GetPalPlayerCharacter()->ShooterComponent->GetHasWeapon()->IsRequiredBullet = !Config.IsInfinAmmo;
                restoreInfammo = false;
            }
        }
        if (Config.IsMuteki || Config.infShield)
            InfiniteShield(Config.IsMuteki);
        //
        if (Config.godMode)
        {
            if (Config.GetPalPlayerCharacter() && Config.GetPalPlayerCharacter()->CharacterParameterComponent)
            {
                auto indParam = Config.GetPalPlayerCharacter()->CharacterParameterComponent->IndividualParameter;
                if (indParam->GetHP().Value < INT_MAX)
                    Config.GetPalPlayerCharacter()->ReviveCharacter_ToServer(SDK::FFixedPoint(INT_MAX));

            }
        }
    }
}
