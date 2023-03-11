// Keeper Digital © 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/SpotLightComponent.h" // Parent
#include "Materials/MaterialParameterCollection.h" // ParameterCollection
#include "ScannerLightFilterBackend.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class KEEPERDIGITAL_API UScannerLightFilterBackend : public USpotLightComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scanner|Collection", DisplayName = "Parameter Collection")
	UMaterialParameterCollection* ParameterCollection = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scanner|Collection")
	FName ScannerStart;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scanner|Collection")
	FName ScannerStop;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scanner|Collection")
	FName ScannerAttenuation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scanner|Collection")
	FName ScannerRadius;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scanner|Collection")
	FName ScannerCurrentChannelID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Scannery|Property", DisplayName="Is Scanner Active?")
	bool bIsThisScannerActive;
	
	/*
	 * Every Scanner Instance has this ID.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scanner|Property")
	double CurrentChannelID;
	
	virtual void BeginPlay() override;

	// Update The Assigned Collection Parameters For Their Material Functions; Does not do safety checking!
	UFUNCTION(BlueprintCallable, DisplayName = "Update Collection Parameters")
	void UpdateCollectionParameters();

	UFUNCTION(BlueprintCallable, DisplayName = "Tangent of Outer Cone Angle", Category = "Scanner|Implementation")
	double TangentOfOuterConeAngle();

	// Wrapper Method for Visibility In Game
	UFUNCTION(BlueprintCallable, DisplayName = "Enable Scanner", Category = "Scanner|Operation")
	void EnableScanner();
	// Wrapper Method for Visibility In Game: 
	UFUNCTION(BlueprintCallable, DisplayName = "Disable Scanner", Category = "Scanner|Operation")
	void DisableScanner();

	UFUNCTION(BlueprintCallable, DisplayName = "Swap To This Scanner", Category = "Scanner|Operation")
	void SwapToThisScanner();
	
	/*
	 *  Debug *Only*
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Get Scanner Radius", Category = "Scanner|Debug", BlueprintPure)
	double GetScannerRadius(FName& Namespace);
	/*
	 *  Debug *Only*
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Get Scanner Attenuation", Category = "Scanner|Debug", BlueprintPure)
	double GetScannerAttenuation(FName& Namespace);
	/*
	 *  Debug *Only*
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Get Scanner Start", Category = "Scanner|Debug", BlueprintPure)
	FLinearColor GetScannerStart(FName& Namespace);
	/*
	 *  Debug *Only*
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Get Scanner Stop", Category = "Scanner|Debug", BlueprintPure)
	FLinearColor GetScannerStop(FName& Namespace);
};
