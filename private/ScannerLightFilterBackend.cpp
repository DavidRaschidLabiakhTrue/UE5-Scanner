// Keeper Digital © 2023

#include "Devices/ScannerLightFilterBackend.h"
#include "Kismet/KismetMaterialLibrary.h" // setting scalar and vector parameters on collections(Shared Shader Memory)
#include "Kismet/KismetMathLibrary.h" // Vector to Linear Color to upload into collection
#include "Kismet/KismetSystemLibrary.h" // Line Trace By Channel

void UScannerLightFilterBackend::BeginPlay()
{
	Super::BeginPlay();
	TArray<FName> VectorNames;
	TArray<FName> FloatNames;

	// Get the names and load them up.
	this->ParameterCollection->GetParameterNames(VectorNames, true);
	this->ParameterCollection->GetParameterNames(FloatNames, false);

	// Filter them out of here regardless of the name, enforce they must have these tags in the names.
	for(const auto&Name: VectorNames)
	{
		const auto Str = Name.ToString();
		if(Str.Contains("Start") || Str.Contains("Begin")) // Give some leeway with collection if needed
		{
			this->ScannerStart = Name;
			continue;
		}
		if(Name.ToString().Contains("Stop") || Str.Contains("End")) // Give some leeway with collection if needed
		{
			this->ScannerStop = Name;
		}
	}
	for(const auto&Name: FloatNames)
	{
		const auto Str = Name.ToString();
		if(Str.Contains("Attenuation")) // No leeway granted here.
		{
			this->ScannerAttenuation = Name;
			continue;
		}
		if(Str.Contains("Radius")) // No leeway granted here.
		{
			this->ScannerRadius = Name;
			continue;
		}
		if(Str.Contains("Channel"))
		{
			this->ScannerCurrentChannelID = Name;
		}
	}
}

void UScannerLightFilterBackend::UpdateCollectionParameters()
{
	if (bIsThisScannerActive) // Fast Track to off.
	{
		const auto World = GetWorld(); // Store Temp Copy of World Pointer. (Possible Optimization: Cache World Pointer)
		const auto WorldPosition = GetComponentLocation(); // Store Temp Copy Of Scanner World Position
		UKismetMaterialLibrary::SetScalarParameterValue(World, ParameterCollection, this->ScannerAttenuation, AttenuationRadius); // Load Attenuation into Collection. (Possible Optimization: Load Once)
		UKismetMaterialLibrary::SetVectorParameterValue(World, ParameterCollection, this->ScannerStart,UKismetMathLibrary::Conv_VectorToLinearColor(WorldPosition));
		if(IsVisible())
		{
			FHitResult Hit; // Hit structure that will be sent into LineTraceSingle (Possible Optimization: Alternative Hit Structure with less parameters)
			const TArray<AActor* > ActorsToIgnore; // Ignore None.
			const FVector EndOfTrace = WorldPosition + GetForwardVector() * AttenuationRadius; // Standard Ray Cast Formula.
			UKismetMaterialLibrary::SetScalarParameterValue(World, ParameterCollection, this->ScannerCurrentChannelID, this->CurrentChannelID);
			// Perform a simple line trace against the world on the visibility channel.
			if (UKismetSystemLibrary::LineTraceSingle(World, WorldPosition, EndOfTrace,UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, Hit, true))
			{
				// There was a hit
				const double NewRadius = UKismetMathLibrary::FClamp(UKismetMathLibrary::VSize(Hit.ImpactPoint - WorldPosition), 0.0f, AttenuationRadius) * TangentOfOuterConeAngle();
				// The Place To Stop Is The Impact Point.
				UKismetMaterialLibrary::SetVectorParameterValue(World, ParameterCollection, this->ScannerStop,UKismetMathLibrary::Conv_VectorToLinearColor(Hit.ImpactPoint));
				UKismetMaterialLibrary::SetScalarParameterValue(World, ParameterCollection, this->ScannerRadius, NewRadius);
			}
			else
			{
				// There was no hit -> The cone either goes too far without hitting anything or it's being pointed off into the void. 
				const double NewRadius = AttenuationRadius *  TangentOfOuterConeAngle();
				// The Place To Stop Is The End Of The Trace
				UKismetMaterialLibrary::SetVectorParameterValue(World, ParameterCollection, this->ScannerStop,UKismetMathLibrary::Conv_VectorToLinearColor(EndOfTrace));
				UKismetMaterialLibrary::SetScalarParameterValue(World, ParameterCollection, this->ScannerRadius, NewRadius);
			}
		}
		else
		{
			// Scanner is *probably* off.
			UKismetMaterialLibrary::SetVectorParameterValue(World, ParameterCollection, ScannerStop,UKismetMathLibrary::Conv_VectorToLinearColor(WorldPosition));
			UKismetMaterialLibrary::SetScalarParameterValue(World, ParameterCollection, ScannerAttenuation, 0.0f);
		}
	}

}

double UScannerLightFilterBackend::TangentOfOuterConeAngle()
{
	return  UKismetMathLibrary::DegTan(OuterConeAngle);
}

void UScannerLightFilterBackend::EnableScanner()
{
	SetHiddenInGame(false, true);
	bIsThisScannerActive = true;
}

void UScannerLightFilterBackend::DisableScanner() // "Should this 0/INF all Parameter Collection Values?" - David
{
	SetHiddenInGame(true, true);
	bIsThisScannerActive = false;
}

void UScannerLightFilterBackend::SwapToThisScanner()
{
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), ParameterCollection, this->ScannerCurrentChannelID, this->CurrentChannelID);
}

double UScannerLightFilterBackend::GetScannerRadius(FName& Namespace)
{
	Namespace = ScannerRadius;
	return UKismetMaterialLibrary::GetScalarParameterValue(GetWorld(), this->ParameterCollection, this->ScannerRadius);
}

double UScannerLightFilterBackend::GetScannerAttenuation(FName& Namespace)
{
	Namespace = ScannerAttenuation;
	return UKismetMaterialLibrary::GetScalarParameterValue(GetWorld(), this->ParameterCollection, this->ScannerAttenuation);
}

FLinearColor UScannerLightFilterBackend::GetScannerStart(FName& Namespace)
{
	Namespace = ScannerStart;
	return UKismetMaterialLibrary::GetVectorParameterValue(GetWorld(), this->ParameterCollection, this->ScannerStart);
}

FLinearColor UScannerLightFilterBackend::GetScannerStop(FName& Namespace)
{
	Namespace = ScannerStop;
	return UKismetMaterialLibrary::GetVectorParameterValue(GetWorld(), this->ParameterCollection, this->ScannerStop);
}
