#include "pch.h"
#include "feature.h"
#include "include/Console.hpp"
#include <drawing.hpp>
using namespace SDK;


inline float calculateDistance(const FVector vector1, const FVector vector2) {
	float x_diff = vector2.X - vector1.X;
	float y_diff = vector2.Y - vector1.Y;
	float z_diff = vector2.Z - vector1.Z;

	return std::sqrt(x_diff * x_diff + y_diff * y_diff + z_diff * z_diff);
}
//	should only be called from a GUI thread with ImGui context
void ESP()
{

/*
	UPalShooterComponent* pShootComponent = pPalCharacter->ShooterComponent;
	if (!pShootComponent)
		return;

	APalWeaponBase* pWeapon = pShootComponent->HasWeapon;
	if (pWeapon)
		DrawUActorComponent(pWeapon->InstanceComponents, ImColor(128, 0, 0));
	*/
	auto UWorld = Config.GetUWorld();
	if (!UWorld || !Config.IsInGame(UWorld))return;
	APalPlayerCharacter* pLocalPlayer = Config.GetPalPlayerCharacter();
	SDK::UPalUtility* pUtility = Config.pPalUtility;
	if (!UWorld->PersistentLevel || !pUtility) return;

		
	auto pPlayerController = pUtility->GetLocalPlayerController(UWorld); //(APalPlayerController*)pLocalPlayer->Controller;// pUtility->GetLocalPlayerController(UWorld);//static_cast<APalPlayerController*>(pLocalPlayer->Controller);
	if (!pPlayerController) return;


	SDK::TArray<SDK::AActor*> T = UWorld->PersistentLevel->Actors;

	for (int i = 0; i < T.Count(); i++)
	{
		SDK::AActor* Actor = T[i];
		FVector2D outScreen;
		
		if (Actor == nullptr) continue;

		
		if (Actor->IsA(SDK::APalMapObjectPalEgg::StaticClass()))
		{
			FVector EggLocation = Actor->K2_GetActorLocation();
			if (!pPlayerController->ProjectWorldLocationToScreen(EggLocation, &outScreen, true))
				continue;
			FVector myloc = pLocalPlayer->K2_GetActorLocation();
			int distance = calculateDistance(EggLocation, myloc) / 100;

			DrawTextWithColorsCode(ImGui::GetFont(), "^6Pal Egg", ImVec2(outScreen.X, outScreen.Y), 14, ImColor(0, 0, 0), true, 1);
		}
		if (Actor->IsA(SDK::APalMapObjectTreasureBox::StaticClass()))
		{
			FVector BoxLocation = Actor->K2_GetActorLocation();
			if (!pPlayerController->ProjectWorldLocationToScreen(BoxLocation, &outScreen, true))
				continue;
			FVector myloc = pLocalPlayer->K2_GetActorLocation();
			int distance = calculateDistance(BoxLocation, myloc) / 100;

			DrawTextWithColorsCode(ImGui::GetFont(), "^4Treasure", ImVec2(outScreen.X, outScreen.Y), 14, ImColor(0, 0, 0), true, 1);
		}
		if (Actor->IsA(SDK::APalCharacter::StaticClass()))
		{
			SDK::APalCharacter* Character = (SDK::APalCharacter*)Actor;
			if (!Character->IsInitialized()) continue;

			ImColor drawColor = ImColor(255, 255, 255);
			std::string name;
			SDK::FString fn;

			Character->CharacterParameterComponent->GetNickname(&fn);
			if (fn.IsValid())
				name = fn.ToString();
			else
				name = "Invalid";

			auto hp = Character->CharacterParameterComponent->GetHP().Value / 1000;
			if (hp <= 0)continue;

			auto lvl = Character->CharacterParameterComponent->GetLevel();
			auto maxhp = Character->CharacterParameterComponent->GetMaxHP().Value / 1000;
			//auto mp = Character->CharacterParameterComponent->GetMP().Value;
			//auto maxmp = Character->CharacterParameterComponent->GetMaxMP().Value;
			FVector actorLocation= Character->K2_GetActorLocation();//, boxExtent;// 
			//Character->GetActorBounds(true, &actorLocation, &boxExtent, true);
			FVector myloc = pLocalPlayer->K2_GetActorLocation();
			int distance = calculateDistance(actorLocation, myloc) / 100;
			actorLocation.Z += 100;
			if (Actor->IsA(SDK::APalPlayerCharacter::StaticClass()))
			{
				drawColor = ImColor(31, 240, 138);
			}

			if (!pPlayerController->ProjectWorldLocationToScreen(actorLocation, &outScreen, true))
				continue;

			char drawstr[100];
			sprintf_s(drawstr, "^7[^0Lv %d^7]^0%s\n^7[^3%d^7|^3%d^7][^5%dm^7]", lvl, name.c_str(), hp, maxhp, distance);

			DrawTextWithColorsCode(ImGui::GetFont(), drawstr, ImVec2(outScreen.X, outScreen.Y-14), 14, drawColor, true, 1);
		}
		
	}

}

//	draws debug information for the input actor array
//	should only be called from a GUI thread with ImGui context
void ESP_DEBUG(float mDist, ImVec4 color, UClass* mEntType)
{
	APalPlayerCharacter* pLocalPlayer = Config.GetPalPlayerCharacter();
	if (!pLocalPlayer)
		return;

	APalPlayerController* pPlayerController = static_cast<APalPlayerController*>(pLocalPlayer->Controller);
	if (!pPlayerController)
		return;

	std::vector<AActor*> actors;
	if (!config::GetAllActorsofType(mEntType, &actors, true))
		return;
	
	auto draw = ImGui::GetBackgroundDrawList();

	__int32 actorsCount = actors.size();
	for (AActor* actor : actors)
	{
		FVector actorLocation = actor->K2_GetActorLocation();
		FVector localPlayerLocation = pLocalPlayer->K2_GetActorLocation();
		float distanceTo = GetDistanceToActor(pLocalPlayer, actor);
		if (distanceTo > mDist)
			continue;

		FVector2D outScreen;
		if (!pPlayerController->ProjectWorldLocationToScreen(actorLocation, &outScreen, true))
			continue;

		char data[0x256];
		const char* StringData = "OBJECT: [%s]\nCLASS: [%s]\nPOSITION: { %0.0f, %0.0f, %0.0f }\nDISTANCE: [%.0fm]";
		if (distanceTo >= 1000.f)
		{
			distanceTo /= 1000.f;
			StringData = "OBJECT: [%s]\nCLASS: [%s]\nPOSITION: { %0.0f, %0.0f, %0.0f }\nDISTANCE: [%.0fkm]";
		}
		sprintf_s(data, StringData, actor->GetName().c_str(), actor->Class->GetFullName().c_str(), actorLocation.X, actorLocation.Y, actorLocation.Z, distanceTo);

		ImVec2 screen = ImVec2(static_cast<float>(outScreen.X), static_cast<float>(outScreen.Y));
		draw->AddText(screen, ImColor(color), data);
	}
}

//	should only be called from a GUI thread with ImGui context
void DrawUActorComponent(TArray<UActorComponent*> Comps,ImColor color)
{
	ImGui::GetBackgroundDrawList()->AddText(nullptr, 16, ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), color, "Drawing...");
	if (!Comps.IsValid())
		return; 
	for (int i = 0; i < Comps.Count(); i++)
	{
		
		if (!Comps[i])
			continue;

		ImGui::GetBackgroundDrawList()->AddText(nullptr, 16, ImVec2(10, 10 + (i * 30)), color, Comps[i]->GetFullName().c_str());
	}
}
void DestroyObject(AActor* Object)
{
	APalPlayerCharacter* pLocalPlayer = Config.GetPalPlayerCharacter();
	if (!pLocalPlayer) return;
	FVector myloc = pLocalPlayer->K2_GetActorLocation();
	FVector ObjLoc = Object->K2_GetActorLocation();
	int distance = calculateDistance(ObjLoc, myloc) / 100;
	if (distance < 150)
	{
		SDK::APalBuildObject* BuildObject = (SDK::APalBuildObject*)Object;
		pLocalPlayer->GetPalPlayerController()->Transmitter->MapObject->RequestDismantleObject_ToServer(BuildObject->ModelInstanceId);
	}
}
//	credit: 
void UnlockAllEffigies() 
{
	APalPlayerCharacter* pPalCharacter = Config.GetPalPlayerCharacter();
	APalPlayerState* pPalPlayerState = Config.GetPalPlayerState();
	if (!pPalCharacter || !pPalPlayerState)
		return;

	UWorld* world = Config.GetUWorld();
	if (!world)
		return;

	TUObjectArray* objects = world->GObjects;

	for (int i = 0; i < objects->NumElements; ++i) 
	{
		UObject* object = objects->GetByIndex(i);

		if (!object)
			continue;

		if (!object->IsA(APalLevelObjectRelic::StaticClass()))
			continue;

		APalLevelObjectObtainable* relic = (APalLevelObjectObtainable*)object;
		if (!relic) {
			continue;
		}

		pPalPlayerState->RequestObtainLevelObject_ToServer(relic);
	}
}

//	Credit: BennettStaley
void IncrementInventoryItemCountByIndex(__int32 mCount, __int32 mIndex)
{
	APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
	if (!p_appc != NULL)
		return;

	APalPlayerController* p_apc = static_cast<APalPlayerController*>(p_appc->Controller);
	if (!p_apc)
		return;

	APalPlayerState* p_apps = static_cast<SDK::APalPlayerState*>(p_apc->PlayerState);
	if (!p_apps)
		return;

	UPalPlayerInventoryData* InventoryData = p_apps->GetInventoryData();
	if (!InventoryData)
		return;

	UPalItemContainerMultiHelper* InventoryMultiHelper = InventoryData->InventoryMultiHelper;
	if (!InventoryMultiHelper)
		return;

	TArray<class SDK::UPalItemContainer*> Containers = InventoryMultiHelper->Containers;
	if (Containers.Count() <= 0)
		return;

	UPalItemSlot* pSelectedSlot = Containers[0]->Get(mIndex);

	if (!pSelectedSlot != NULL)
		return;

	FPalItemId FirstItemId = pSelectedSlot->GetItemId();
	__int32 StackCount = pSelectedSlot->GetStackCount();
	__int32 mNewCount = StackCount += mCount;
	InventoryData->RequestAddItem(FirstItemId.StaticId, mNewCount, true);
}

//	
void AddItemToInventoryByName(UPalPlayerInventoryData* data, const char* itemName, int count)
{
	// obtain lib instance
	static UKismetStringLibrary* lib = UKismetStringLibrary::GetDefaultObj();

	// Convert FNAME
	wchar_t  ws[255];
	swprintf(ws, 255, L"%hs", itemName);
	FName Name = lib->Conv_StringToName(FString(ws));

	// Call
	data->RequestAddItem(Name, count, true);
}

// Credit: asashi
void SpawnMultiple_ItemsToInventory(config::QuickItemSet Set)
{
	SDK::UPalPlayerInventoryData* InventoryData = Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->GetInventoryData();
	switch (Set)
	{
	case 0:
		for (int i = 0; i < IM_ARRAYSIZE(database::basic_items_stackable); i++)
			AddItemToInventoryByName(InventoryData, _strdup(database::basic_items_stackable[i].c_str()), 100);
		break;
	case 1:
		for (int i = 0; i < IM_ARRAYSIZE(database::basic_items_single); i++)
			AddItemToInventoryByName(InventoryData, _strdup(database::basic_items_single[i].c_str()), 1);
		break;
	case 2:
		for (int i = 0; i < IM_ARRAYSIZE(database::pal_unlock_skills); i++)
			AddItemToInventoryByName(InventoryData, _strdup(database::pal_unlock_skills[i].c_str()), 1);
		break;
	case 3:
		for (int i = 0; i < IM_ARRAYSIZE(database::spheres); i++)
			AddItemToInventoryByName(InventoryData, _strdup(database::spheres[i].c_str()), 100);
		break;
	case 4:
		for (int i = 0; i < IM_ARRAYSIZE(database::tools); i++)
			AddItemToInventoryByName(InventoryData, _strdup(database::tools[i].c_str()), 1);
		break;
	case 5:
		for (int i = 0; i < IM_ARRAYSIZE(database::skillfruits); i++)
			AddItemToInventoryByName(InventoryData, _strdup(database::skillfruits[i].c_str()), 1);
		break;
	default:
		break;
	}
}

//	
void AnyWhereTP(FVector& vector, bool IsSafe)
{
	APalPlayerState* pPalPlayerState = Config.GetPalPlayerState();
	APalPlayerController* pPalPlayerController = Config.GetPalPlayerController();
	if (!pPalPlayerController || !pPalPlayerState)
		return;

	vector = { vector.X,vector.Y + 100,vector.Z };
	FGuid guid = pPalPlayerController->GetPlayerUId();
	//pPalPlayerController->ServerSetSpectatorLocation(vector, 0);
	//pPalPlayerController->Transmitter->Player->RegisterRespawnLocation_ToServer(guid, vector);
	//pPalPlayerState->RequestRespawn();
	SDK::FPalSyncTeleportRequestParameter tpParam;
	tpParam.bIsFadeOutSkip = true;
	tpParam.Location = vector;
	//tpParam.Location = { -321248.80, 208959.95, 115.59 };
	tpParam.Rotation = 0;
	tpParam.bIsFadeOutSkip = true;
	Config.GetPalPlayerState()->GetSyncTeleportComp()->RequestSyncTeleportStart_ToServer(tpParam);
	Config.GetPalPlayerState()->GetSyncTeleportComp()->RequestSyncTeleportMove_ToServer();
	Config.GetPalPlayerState()->GetSyncTeleportComp()->RequestSyncTeleportEnd_ToServer();
}

//	
void ExploitFly(bool IsFly)
{
	SDK::APalPlayerCharacter* pPalPlayerCharacter = Config.GetPalPlayerCharacter();
	if (!pPalPlayerCharacter)
		return;

	APalPlayerController* pPalPlayerController = pPalPlayerCharacter->GetPalPlayerController();
	if (!pPalPlayerController)
		return;

	IsFly ? pPalPlayerController->StartFlyToServer() : pPalPlayerController->EndFlyToServer();
}

void SetFullbright(bool bIsSet)
{
	ULocalPlayer* pLocalPlayer = Config.GetLocalPlayer();
	if (!pLocalPlayer)
		return;

	UGameViewportClient* pViewport = pLocalPlayer->ViewportClient;
	if (!pViewport)
		return;

	pViewport->mViewMode = bIsSet ? 1 : 3;
}

//	
void SpeedHack(float mSpeed)
{

	UWorld* pWorld = Config.GetUWorld();
	if (!pWorld)
	{
		Config.IsSpeedHack = false;
		return;
	}

	ULevel* pLevel = pWorld->PersistentLevel;
	if (!pLevel)
		return;

	AWorldSettings* pWorldSettings = pLevel->WorldSettings;
	if (!pWorldSettings)
		return;

	pWorld->PersistentLevel->WorldSettings->TimeDilation = mSpeed;

}
//Fast run Credit: naythefirst && Omega172 https://www.unknowncheats.me/forum/3974115-post129.html
void FastRun(float mSpeed)
{
	SDK::APalPlayerCharacter* pPalPlayerCharacter = Config.GetPalPlayerCharacter();
	if (!pPalPlayerCharacter)
		return;

	UPalCharacterMovementComponent* pMovementComponent = pPalPlayerCharacter->GetPalCharacterMovementComponent();
	if (!pMovementComponent)
		return;
	int speed = mSpeed;

	if (!Config.MovementSpeed)speed = 1.0f;
	pMovementComponent->MaxAcceleration = 2048 * mSpeed;

	if (!Config.MovementSpeed)speed = 1.0f;
	pMovementComponent->MaxFlySpeed = 350 * speed;
	
	if (!Config.fastRun)speed = 1.0f;
	pMovementComponent->MaxWalkSpeed = 350 * speed;
	pMovementComponent->MaxWalkSpeedCrouched = 350 * speed;
	pMovementComponent->SprintMaxSpeed = 700 * speed;
	pMovementComponent->SprintMaxAcceleration = 2048 * speed;
	
	speed = mSpeed; if (!Config.fastSwim)speed = 1.0f;
	pMovementComponent->MaxSwimSpeed = 350 * speed;

	speed = mSpeed; if (!Config.fastClimb)speed = 1.0f;
	pMovementComponent->ClimbMaxSpeed = 350 * speed;

	speed = mSpeed; if (!Config.fastRolling)speed = 1.0f;
	pMovementComponent->RollingMaxSpeed = 350 * speed;

	speed = mSpeed; if (!Config.fastGlider)speed = 1.0f;
	//pMovementComponent->SlidingMaxSpeed = 350 * speed;

	//pMovementComponent->GliderMaxSpeed = 0.f;
	
}
void SendDamageToActor(APalCharacter* pTarget, int32 damage, bool bSpoofAttacker)
{
	APalPlayerState* pPalPlayerState = Config.GetPalPlayerState();
	APalPlayerCharacter* pPalPlayerCharacter = Config.GetPalPlayerCharacter();
	if (!pPalPlayerState || !pPalPlayerCharacter)
		return;

	FPalDamageInfo  info = FPalDamageInfo();
	info.AttackElementType = EPalElementType::Normal;
	info.Attacker = pPalPlayerCharacter;		//	@TODO: spoof attacker
	info.AttackerGroupID = Config.GetPalPlayerState()->IndividualHandleId.PlayerUId;
	info.AttackerLevel = 50;
	info.AttackType = EPalAttackType::Weapon;
	info.bApplyNativeDamageValue = true;
	info.bAttackableToFriend = true;
	info.IgnoreShield = true;
	info.NativeDamageValue = damage;
	pPalPlayerState->SendDamage_ToServer(pTarget, info);
}

//	 NOTE: only targets pals
void DeathAura(__int32 dmgAmount, float mDistance, bool bIntensityEffect, bool bVisualAffect, EPalVisualEffectID visID)
{
	APalCharacter* pPalCharacter = Config.GetPalPlayerCharacter();
	if (!pPalCharacter)
		return;

	UPalCharacterParameterComponent* pParams = pPalCharacter->CharacterParameterComponent;
	if (!pParams)
		return;

	TArray<APalCharacter*> outPals;
	if (!Config.GetTAllPals(&outPals))
		return;

	DWORD palsCount = outPals.Count();
	for (auto i = 0; i < palsCount; i++)
	{
		APalCharacter* cEnt = outPals[i];

		if (!cEnt || !cEnt->IsA(APalCharacter::StaticClass()) || cEnt == pPalCharacter)
			continue;

		if (cEnt->CharacterParameterComponent && cEnt->CharacterParameterComponent->IsOtomo())
			continue;

		float distanceTo = GetDistanceToActor(pPalCharacter, cEnt);
		if (distanceTo > mDistance)
			continue;

		float dmgScalar = dmgAmount * (1.0f - distanceTo / mDistance);
		if (bIntensityEffect)
			dmgAmount = dmgScalar;

		UPalVisualEffectComponent* pVisComp = cEnt->VisualEffectComponent;
		if (bVisualAffect && pVisComp)
		{
			FPalVisualEffectDynamicParameter fvedp;
			if (!pVisComp->ExecutionVisualEffects.Count())
				pVisComp->AddVisualEffect_ToServer(visID, fvedp, 1);	//	uc: killer1478
		}
		SendDamageToActor(cEnt, dmgAmount);
	}
}

APalCharacter* FindOtomoPal()
{
	APalCharacter* pPlayerPal = NULL;
	TArray<SDK::APalCharacter*> outPals;
	if (!Config.GetTAllPals(&outPals))
		return NULL;

	DWORD palsCount = outPals.Count();
	for (auto i = 0; i < palsCount; i++)
	{
		APalCharacter* cEnt = outPals[i];

		if (!cEnt || !cEnt->IsA(SDK::APalCharacter::StaticClass()))
			continue;

		if (cEnt->CharacterParameterComponent && cEnt->CharacterParameterComponent->IsOtomo())
		{
			return cEnt;
			break;
		}
	}
	return NULL;
}
//	
void InfiniteShield(bool bIsSet)//not working
{
	auto pCharacter = Config.GetPalPlayerCharacter();
	if (!pCharacter) return;

	auto pParams = pCharacter->CharacterParameterComponent;
	if (!pParams) return;

	auto mIVs = pParams->IndividualParameter;
	if (!mIVs) return;

	auto sParams = mIVs->SaveParameter;

	if (bIsSet)
		pParams->bIsEnableMuteki = bIsSet;	//	Credit: Mokobake

	if(Config.infShield)
		mIVs->SetShieldHP(sParams.ShieldMaxHP);
	//	attempt test additional parameters
	
	sParams.HP.Value = sParams.MaxHP.Value;
	sParams.MP.Value = sParams.MaxMP.Value;
	sParams.FullStomach = sParams.MaxFullStomach;
	sParams.PhysicalHealth = EPalStatusPhysicalHealthType::Healthful;
	sParams.SanityValue = 100.f;
	sParams.HungerType = EPalStatusHungerType::Default;
}

//	
void RespawnLocalPlayer(bool bIsSafe)
{
	APalPlayerController* pPalPlayerController = Config.GetPalPlayerController();
	APalPlayerState* pPalPlayerState = Config.GetPalPlayerState();
	if (!pPalPlayerController || !pPalPlayerState)
		return;

	bIsSafe ? pPalPlayerController->TeleportToSafePoint_ToServer() : pPalPlayerState->RequestRespawn();
}

//	
void ReviveLocalPlayer()
{
	APalPlayerCharacter* pPalPlayerCharacter = Config.GetPalPlayerCharacter();
	if (!pPalPlayerCharacter)
		return;

	FFixedPoint newHealthPoint = FFixedPoint(INT_MAX);
	if (Config.GetPalPlayerCharacter()->CharacterParameterComponent->IsDying())
		Config.GetPalPlayerCharacter()->CharacterParameterComponent->ReviveFromDying();
	pPalPlayerCharacter->ReviveCharacter_ToServer(newHealthPoint);
}

void NoReload()
{
	auto pCharacter = Config.GetPalPlayerCharacter();
	if (!pCharacter) return;
	auto loadout = pCharacter->LoadoutSelectorComponent;
	//if (loadout) return;
	SDK::UPalItemSlot* itemSlot = loadout->GetNowSelectedItemSlot(SDK::EPalPlayerInventoryType(3)); // WeaponLoadout
	SDK::UPalDynamicItemDataBase* itemData = 0;
	if(itemSlot) itemSlot->TryGetDynamicItemData(&itemData);
	if (itemData) {
		auto weaponData = (SDK::UPalDynamicWeaponItemDataBase*)itemData;
		weaponData->RemainingBullets = 30;
	}
}
//	
void MaxWeight()
{
	UWorld* pWorld = Config.GetUWorld();
	if (!pWorld)
	{
		Config.MaxWeight = false;
		return;
	}
	auto pCharacter = Config.GetPalPlayerCharacter();
	if (!pCharacter)
		return;

	if (pCharacter != NULL)
	{
		auto controller = pCharacter->GetPalPlayerController(); if (controller != NULL) return;
		auto PlayerState = controller->GetPalPlayerState(); if (PlayerState != NULL) return;
		auto InventoryData = PlayerState->InventoryData; if (InventoryData != NULL) return;
		InventoryData->MaxInventoryWeight = 9999999;

	}

}
void ResetStamina()
{
	APalPlayerCharacter* pPalCharacter = Config.GetPalPlayerCharacter();

	if (pPalCharacter && pPalCharacter->CharacterParameterComponent)
	{
		pPalCharacter->CharacterParameterComponent->ResetSP();
	}

	SDK::TArray<SDK::AActor*> Actors = Config.GetUWorld()->PersistentLevel->Actors;

	for (int i = 0; i < Actors.Count(); i++)
	{
		if (Actors[i] && Actors[i]->IsA(SDK::APalCharacter::StaticClass()))
		{
			SDK::APalCharacter* Character = static_cast<SDK::APalCharacter*>(Actors[i]);

			if (Character->CharacterParameterComponent && Character->CharacterParameterComponent->IsOtomo())
			{
				Character->CharacterParameterComponent->ResetSP();
			}
		}
	}
}
// credit xCENTx
void ForgeActor(SDK::AActor* pTarget, float mDistance, float mHeight, float mAngle)
{
	APalPlayerCharacter* pPalPlayerCharacter = Config.GetPalPlayerCharacter();
	APlayerController* pPlayerController = Config.GetPalPlayerController();
	if (!pTarget || !pPalPlayerCharacter || !pPlayerController)
		return;

	APlayerCameraManager* pCamera = pPlayerController->PlayerCameraManager;
	if (!pCamera)
		return;

	FVector playerLocation = pPalPlayerCharacter->K2_GetActorLocation();
	FVector camFwdDir = pCamera->GetActorForwardVector() * (mDistance * 100.f);
	FVector targetLocation = playerLocation + camFwdDir;

	if (mHeight != 0.0f)
		targetLocation.Y += mHeight;

	FRotator targetRotation = pTarget->K2_GetActorRotation();
	if (mAngle != 0.0f)
		targetRotation.Roll += mAngle;

	pTarget->K2_SetActorLocation(targetLocation, false, nullptr, true);
	pTarget->K2_SetActorRotation(targetRotation, true);
}

//	credit: nknights23
void SetBasePalsCraftingSpeed(float mNewSpeed, bool bRestoreDefault)
{

	TArray<SDK::APalCharacter*> mPals;
	if (!Config.GetTAllPals(&mPals))
		return;

	DWORD palsCount = mPals.Count();
	for (int i = 0; i < palsCount; i++)
	{
		APalCharacter* obj = mPals[i];
		if (!obj || !obj->IsA(APalMonsterCharacter::StaticClass()) || !Config.IsABaseWorker(obj,false))
			continue;

		UPalCharacterParameterComponent* pParams = obj->CharacterParameterComponent;
		if (!pParams)
			return;

		auto ivParams = pParams->IndividualParameter;
		if (!ivParams)
			return;

		FPalIndividualCharacterSaveParameter sParams = ivParams->SaveParameter;
		TArray<FFloatContainer_FloatPair> mCraftSpeedArray = sParams.CraftSpeedRates.Values;

		if (mCraftSpeedArray.Count() > 0)
		{
			
			auto count = mCraftSpeedArray.Count();
			for (int i = 0; i < count; i++)
			{
				mCraftSpeedArray[i].Value = bRestoreDefault ? 1.0f : mNewSpeed;
				auto name = mCraftSpeedArray[i].Key.ToString().c_str();
				auto value = mCraftSpeedArray[i].Value;
				DX11_Base::g_Console->printdbg("\n[+] name: %s\0 value: %llx [+]", DX11_Base::Console::Colors::green, ivParams->GetFullName().c_str(), &mCraftSpeedArray);
			}
		}
			
	}
}
void CureAll()
{

	TArray<SDK::APalCharacter*> mPals;
	if (!Config.GetTAllPals(&mPals))
		return;

	DWORD palsCount = mPals.Count();
	for (int i = 0; i < palsCount; i++)
	{
		APalCharacter* obj = mPals[i];
		if (!obj || !obj->IsA(APalMonsterCharacter::StaticClass()) || !Config.IsABaseWorker(obj, false))
			continue;
		
		UPalCharacterParameterComponent* pParams = obj->CharacterParameterComponent;
		if (!pParams)
			return;

		auto palInstance = pParams->IndividualHandle->ID;
		Config.GetPalPlayerCharacter()->GetPalPlayerController()->Transmitter->NetworkIndividualComponent->SetWorkerSick_ToServer(palInstance, EPalBaseCampWorkerSickType::None);
		pParams->GetIndividualParameter()->SaveParameter.FullStomach = pParams->GetIndividualParameter()->SaveParameter.MaxFullStomach;
		pParams->GetIndividualParameter()->SaveParameter.SanityValue = 100;

	}
}
// credit: xCENTx
void TeleportAllPalsToCrosshair(float mDistance)
{
	TArray<APalCharacter*> outPals;
	Config.GetTAllPals(&outPals);
	DWORD palsCount = outPals.Count();
	for (int i = 0; i < palsCount; i++)
	{
		APalCharacter* cPal = outPals[i];

		if (!cPal || !cPal->IsA(APalMonsterCharacter::StaticClass()))
			continue;

		//	@TODO: displace with entity width for true distance, right now it is distance from origin
		//	FVector palOrigin;
		//	FVector palBounds;
		//	cPal->GetActorBounds(true, &palOrigin, &palBounds, false);
		//	float adj = palBounds.X * .5 + mDistance;

		ForgeActor(cPal, mDistance);
	}
}

// credit: xCENTx
void AddWaypointLocation(std::string wpName)
{
	APalCharacter* pPalCharacater = Config.GetPalPlayerCharacter();
	if (!pPalCharacater)
		return;

	FVector wpLocation = pPalCharacater->K2_GetActorLocation();
	FRotator wpRotation = pPalCharacater->K2_GetActorRotation();
	config::SWaypoint newWaypoint = config::SWaypoint("[WAYPOINT]" + wpName, wpLocation, wpRotation);
	Config.db_waypoints.push_back(newWaypoint);
}

// credit: xCENTx
//	must be called from a rendering thread with imgui context
void RenderWaypointsToScreen()
{
	APalCharacter* pPalCharacater = Config.GetPalPlayerCharacter();
	APalPlayerController* pPalController = Config.GetPalPlayerController();
	if (!pPalCharacater || !pPalController)
		return;

	ImDrawList* draw = ImGui::GetWindowDrawList();

	for (auto waypoint : Config.db_waypoints)
	{
		FVector2D vScreen;
		if (!pPalController->ProjectWorldLocationToScreen(waypoint.waypointLocation, &vScreen, false))
			continue;

		auto color = ImColor(1.0f, 1.0f, 1.0f, 1.0f);

		draw->AddText(ImVec2(vScreen.X, vScreen.Y), color, waypoint.waypointName.c_str());
	}
}

//	
void GiveExperiencePoints(__int32 mXP)
{
	auto pPalPlayerState = Config.GetPalPlayerState();
	if (!pPalPlayerState)
		return;

	pPalPlayerState->GrantExpForParty(mXP);
}

//	
void SetPlayerAttackParam(__int32 mNewAtk)
{
	APalPlayerCharacter* pPalPlayerCharacter = Config.GetPalPlayerCharacter();
	if (!pPalPlayerCharacter)
		return;

	UPalCharacterParameterComponent* pParams = pPalPlayerCharacter->CharacterParameterComponent;
	if (!pParams)
		return;

	if (pParams->AttackUp != mNewAtk)
		pParams->AttackUp = mNewAtk;
}

//	
void SetPlayerDefenseParam(__int32 mNewDef)
{
	APalPlayerCharacter* pPalPlayerCharacter = Config.GetPalPlayerCharacter();
	if (!pPalPlayerCharacter)
		return;

	UPalCharacterParameterComponent* pParams = pPalPlayerCharacter->CharacterParameterComponent;
	if (!pParams)
		return;
	
	if (pParams->DefenseUp != mNewDef)
		pParams->DefenseUp = mNewDef;
}

//	
void SetInfiniteAmmo(bool bInfAmmo)
{
	APalPlayerCharacter* pPalCharacter = Config.GetPalPlayerCharacter();
	if (!pPalCharacter)
		return;

	UPalShooterComponent* pShootComponent = pPalCharacter->ShooterComponent;
	if (!pShootComponent)
		return;

	APalWeaponBase* pWeapon = pShootComponent->HasWeapon;
	if (pWeapon)
		pWeapon->IsRequiredBullet = bInfAmmo ? false : true;

}

//	
void SetCraftingSpeed(float mNewSpeed, bool bRestoreDefault)
{
	APalPlayerCharacter* pPalCharacter = Config.GetPalPlayerCharacter();
	if (!pPalCharacter)
		return;

	UPalCharacterParameterComponent* pParams = pPalCharacter->CharacterParameterComponent;
	if (!pParams)
		return;

	UPalIndividualCharacterParameter* ivParams = pParams->IndividualParameter;
	if (!ivParams)
		return;

	FPalIndividualCharacterSaveParameter sParams = ivParams->SaveParameter;
	TArray<FFloatContainer_FloatPair> mCraftSpeedArray = sParams.CraftSpeedRates.Values;

	if (mCraftSpeedArray.Count() > 0)
		mCraftSpeedArray[0].Value = bRestoreDefault ? 1.0f : mNewSpeed;
}

//	
void AddTechPoints(__int32 mPoints)
{
	APalPlayerState* mPlayerState = Config.GetPalPlayerState();
	if (!mPlayerState)
		return;

	UPalTechnologyData* pTechData = mPlayerState->TechnologyData;
	if (!pTechData)
		return;

	pTechData->TechnologyPoint += mPoints;
}

//	
void AddAncientTechPoints(__int32 mPoints)
{
	APalPlayerState* mPlayerState = Config.GetPalPlayerState();
	if (!mPlayerState)
		return;

	UPalTechnologyData* pTechData = mPlayerState->TechnologyData;
	if (!pTechData)
		return;

	pTechData->bossTechnologyPoint += mPoints;
}

//	
void RemoveTechPoints(__int32 mPoints)
{
	APalPlayerState* mPlayerState = Config.GetPalPlayerState();
	if (!mPlayerState)
		return;

	UPalTechnologyData* pTechData = mPlayerState->TechnologyData;
	if (!pTechData)
		return;

	pTechData->TechnologyPoint -= mPoints;
}

//	
void RemoveAncientTechPoint(__int32 mPoints)
{
	APalPlayerState* mPlayerState = Config.GetPalPlayerState();
	if (!mPlayerState)
		return;

	UPalTechnologyData* pTechData = mPlayerState->TechnologyData;
	if (!pTechData)
		return;

	pTechData->bossTechnologyPoint -= mPoints;
}

float GetDistanceToActor(AActor* pLocal, AActor* pTarget)
{
	if (!pLocal || !pTarget)
		return -1.f;
	
	FVector pLocation = pLocal->K2_GetActorLocation();
	FVector pTargetLocation = pTarget->K2_GetActorLocation();
	double distance = sqrt(pow(pTargetLocation.X - pLocation.X, 2.0) + pow(pTargetLocation.Y - pLocation.Y, 2.0) + pow(pTargetLocation.Z - pLocation.Z, 2.0));

	return distance / 100.0f;
}
//	credit: emoisback
void TeleportToMapMarker()
{
	UWorld* pWorld = Config.GetUWorld();
	UPalUtility* pUtility = Config.pPalUtility;
	if (!pWorld || !pUtility)
		return;

	UPalLocationManager* pLocationMan = pUtility->GetLocationManager(pWorld);
	
	if (!pLocationMan)
		return;

	auto locations = pLocationMan->CustomLocations;
	if (locations.Count() > 0)
	{
		auto last = locations.Count()-1;
		auto mark = locations[last]->Location;

		//g_Console->printdbg("MapMarker: x:%f|Y:%f|Z:%f", Console::Colors::green, mark.X, mark.Y, mark.Z);
		mark.Z += 99999.000;
		auto id = locations[last]->ID;
		AnyWhereTP(mark, false);
		pLocationMan->RemoveLocalCustomLocation(id);
	}
}
///	OLDER METHODS
//SDK::FPalDebugOtomoPalInfo palinfo = SDK::FPalDebugOtomoPalInfo();
//SDK::TArray<SDK::EPalWazaID> EA = { 0U };
//void SpawnPal(char* PalName, bool IsMonster, int rank=1, int lvl = 1, int count=1)
//{
//    SDK::UKismetStringLibrary* lib = SDK::UKismetStringLibrary::GetDefaultObj();
//
//    //Convert FNAME
//    wchar_t  ws[255];
//    swprintf(ws, 255, L"%hs", PalName);
//    SDK::FName Name = lib->Conv_StringToName(SDK::FString(ws));
//    //Call
//    if (Config.GetPalPlayerCharacter() != NULL)
//    {
//        if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
//        {
//            if (Config.GetPalPlayerCharacter()->GetPalPlayerController())
//            {
//                if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState())
//                {
//                    if (IsMonster)
//                    {
//                        Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->RequestSpawnMonsterForPlayer(Name, count, lvl);
//                        return;
//                    }
//                    EA[0] = SDK::EPalWazaID::AirCanon;
//                    palinfo.Level = lvl;
//                    palinfo.Rank = rank;
//                    palinfo.PalName.Key = Name;
//                    palinfo.WazaList = EA;
//                    palinfo.PassiveSkill = NULL;
//                    Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->Debug_CaptureNewMonsterByDebugOtomoInfo_ToServer(palinfo);
//                }
//            }
//        }
//    }
//}

